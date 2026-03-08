# AI 전략 구현 계획 - ClericStrategy / WizardStrategy

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: ClericStrategy, WizardStrategy 구현 (플로우차트 검수 완료 후)
**작성일**: 2026-03-08

**관련 파일**:

- [IAIStrategy.h](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/IAIStrategy.h)
- [FighterStrategy.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/FighterStrategy.h) ← 참고 구현체
- [AISystem.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AISystem.h)
- [architecture/character_flowchart/cleric.mmd](../../../architecture/character_flowchart/cleric.mmd)
- [architecture/character_flowchart/wizard.mmd](../../../architecture/character_flowchart/wizard.mmd)

---

## 개요

**⚠️ 전제 조건**: 각 전략 구현 전 해당 캐릭터의 Mermaid 플로우차트가 팀 검수 완료되어야 합니다.

- Fighter: ✅ 검수 완료 → 구현 완료
- Cleric: ⬜ 플로우차트 검수 후 구현
- Wizard: ⬜ 플로우차트 검수 후 구현
- Rogue: ⬜ 플로우차트 검수 후 구현

**핵심 원칙**: "사실은 Character가, 판단은 Strategy가"

- Character: `GetHPPercentage()` → 사실 (0.35f)
- Strategy: `IsInDanger()` → 판단 (0.35f <= X → true)

---

## Implementation Tasks

### Task 0: 플로우차트 검수 체크리스트

각 전략 구현 전, 다음 항목이 플로우차트에 명시되어야 함:

- [ ] 타겟 설정 조건 (누구를 목표로 하는가)
- [ ] 위험 상태 정의 (HP 임계값 %)
- [ ] 스펠 사용 조건 (슬롯 레벨, 스펠 ID)
- [ ] 이동 우선순위 (도망? 접근? 대기?)
- [ ] 행동 종료 조건

---

### Task 1: ClericStrategy 구현

**특성**: 아군 힐링 우선, 디버프 부여, 원거리 지원

**파일 생성**:

```
DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/ClericStrategy.h
DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/ClericStrategy.cpp
```

**ClericStrategy.h**:

```cpp
/**
 * @file ClericStrategy.h
 * @brief 클레릭 AI 전략 - 힐링 우선, 디버프 지원
 */
#pragma once
#include "IAIStrategy.h"

class GridSystem;

class ClericStrategy : public IAIStrategy
{
public:
    AIDecision MakeDecision(Character* actor) override;

private:
    // 타겟 탐색
    Character* FindLowestHPAlly();      // 가장 HP가 낮은 아군
    Character* FindNearestEnemy();       // 가장 가까운 적

    // 전략별 판단 헬퍼
    bool IsInDanger(Character* actor) const;       // Cleric: HP 50% 이하
    bool AllyNeedsHeal(Character* ally) const;     // 아군 HP 70% 이하
    bool ShouldDebuff(Character* actor) const;     // 주문 슬롯 1+ 이상

    // 이동 위치 계산
    Math::ivec2 FindSafePosition(Character* actor, Character* nearest_enemy, GridSystem* grid);
    Math::ivec2 FindNextMoveTo(Character* actor, Character* target, GridSystem* grid);

    // 스펠 결정
    AIDecision DecideSpellAction(Character* actor, Character* target, bool target_is_ally);
};
```

**ClericStrategy.cpp - MakeDecision 핵심 로직**:

```cpp
AIDecision ClericStrategy::MakeDecision(Character* actor)
{
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    // ============================================================
    // 1단계: 위험 체크 - HP 50% 이하면 도주 우선
    // ============================================================
    if (IsInDanger(actor))
    {
        Character* nearest_enemy = FindNearestEnemy();
        if (nearest_enemy && actor->GetMovementRange() > 0)
        {
            Math::ivec2 safe_pos = FindSafePosition(actor, nearest_enemy, grid);
            if (safe_pos != actor->GetGridPosition()->Get())
            {
                return { AIDecisionType::Move, nullptr, safe_pos, "", "Cleric retreating - low HP" };
            }
        }
    }

    // ============================================================
    // 2단계: 아군 힐 필요 여부 체크
    // ============================================================
    Character* hurt_ally = FindLowestHPAlly();
    if (hurt_ally && AllyNeedsHeal(hurt_ally) && actor->HasAnySpellSlot())
    {
        // 힐 스펠 시전 가능한지 사거리 확인
        int distance = grid->ManhattanDistance(
            actor->GetGridPosition()->Get(),
            hurt_ally->GetGridPosition()->Get()
        );
        // 사거리 내라면 힐 시전
        if (distance <= actor->GetAttackRange())
        {
            return DecideSpellAction(actor, hurt_ally, true); // ally heal
        }
        // 사거리 밖이면 이동
        if (actor->GetMovementRange() > 0)
        {
            Math::ivec2 move_pos = FindNextMoveTo(actor, hurt_ally, grid);
            return { AIDecisionType::Move, nullptr, move_pos, "", "Moving to heal ally" };
        }
    }

    // ============================================================
    // 3단계: 적에게 디버프/공격 스펠
    // ============================================================
    Character* target = FindNearestEnemy();
    if (target && actor->HasAnySpellSlot())
    {
        int distance = grid->ManhattanDistance(
            actor->GetGridPosition()->Get(),
            target->GetGridPosition()->Get()
        );
        if (distance <= actor->GetAttackRange())
        {
            return DecideSpellAction(actor, target, false); // enemy debuff/attack
        }
        // 이동 후 공격 가능한지 체크
        if (actor->GetMovementRange() > 0)
        {
            Math::ivec2 move_pos = FindNextMoveTo(actor, target, grid);
            return { AIDecisionType::Move, nullptr, move_pos, "", "Moving to cast spell on enemy" };
        }
    }

    return { AIDecisionType::EndTurn, nullptr, {}, "", "No action available" };
}

bool ClericStrategy::IsInDanger(Character* actor) const
{
    return actor->GetHPPercentage() <= 0.5f; // Cleric: 50% 이하를 위험으로 판단
}

bool ClericStrategy::AllyNeedsHeal(Character* ally) const
{
    return ally->GetHPPercentage() <= 0.7f; // 아군 HP 70% 이하면 힐 대상
}
```

---

### Task 2: WizardStrategy 구현

**특성**: 원거리 마법 공격 우선, 업캐스트 활용, 슬롯 관리

**파일 생성**:

```
DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/WizardStrategy.h
DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/WizardStrategy.cpp
```

**WizardStrategy.h**:

```cpp
/**
 * @file WizardStrategy.h
 * @brief 위저드 AI 전략 - 원거리 마법 공격, 슬롯 관리
 */
#pragma once
#include "IAIStrategy.h"

class GridSystem;

class WizardStrategy : public IAIStrategy
{
public:
    AIDecision MakeDecision(Character* actor) override;

private:
    Character* FindPriorityTarget();     // 가장 HP가 낮은 적 (집중 공격)
    Character* FindNearestEnemy();

    // 전략별 판단 헬퍼
    bool IsInDanger(Character* actor) const;          // Wizard: HP 40% 이하
    bool ShouldUseHighLevelSpell(Character* actor) const; // 레벨 2+ 슬롯 보유
    bool IsInSafeRange(Character* actor, Character* enemy, GridSystem* grid) const;

    Math::ivec2 FindSafePosition(Character* actor, Character* enemy, GridSystem* grid);
    Math::ivec2 FindNextMoveTo(Character* actor, Character* target, GridSystem* grid);

    AIDecision DecideBestSpell(Character* actor, Character* target, GridSystem* grid);

    // 스펠 우선순위: 고레벨 > 저레벨, 업캐스트 가능 > 불가능
    std::string SelectBestSpellId(Character* actor, bool high_value_target) const;
    int         SelectUpcastLevel(Character* actor, const std::string& spell_id) const;
};
```

**WizardStrategy.cpp - MakeDecision 핵심 로직**:

```cpp
AIDecision WizardStrategy::MakeDecision(Character* actor)
{
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    // ============================================================
    // 1단계: 위험 체크 - HP 40% 이하 또는 근접 적 있음
    // ============================================================
    Character* nearest_enemy = FindNearestEnemy();
    if (IsInDanger(actor) || (nearest_enemy && !IsInSafeRange(actor, nearest_enemy, grid)))
    {
        if (actor->GetMovementRange() > 0)
        {
            Math::ivec2 safe_pos = FindSafePosition(actor, nearest_enemy, grid);
            if (safe_pos != actor->GetGridPosition()->Get())
            {
                return { AIDecisionType::Move, nullptr, safe_pos, "", "Wizard retreating" };
            }
        }
    }

    // ============================================================
    // 2단계: 스펠 슬롯 있으면 원거리 마법 공격
    // ============================================================
    Character* target = FindPriorityTarget(); // 가장 HP 낮은 적
    if (target && actor->HasAnySpellSlot())
    {
        return DecideBestSpell(actor, target, grid);
    }

    // ============================================================
    // 3단계: 스펠 슬롯 없으면 기본 공격 또는 대기
    // ============================================================
    if (target)
    {
        int distance = grid->ManhattanDistance(
            actor->GetGridPosition()->Get(),
            target->GetGridPosition()->Get()
        );
        if (distance <= actor->GetAttackRange() && actor->GetActionPoints() > 0)
        {
            return { AIDecisionType::Attack, target, {}, "", "Basic attack (no spell slots)" };
        }
    }

    return { AIDecisionType::EndTurn, nullptr, {}, "", "No spell slots, waiting" };
}

bool WizardStrategy::IsInDanger(Character* actor) const
{
    return actor->GetHPPercentage() <= 0.4f; // Wizard: 40% 이하를 위험으로 판단
}

bool WizardStrategy::IsInSafeRange(Character* actor, Character* enemy, GridSystem* grid) const
{
    int distance = grid->ManhattanDistance(
        actor->GetGridPosition()->Get(),
        enemy->GetGridPosition()->Get()
    );
    return distance >= 3; // 3칸 이상 거리 유지
}
```

---

### Task 3: AISystem에 새 전략 등록

**파일**: `AISystem.cpp`의 `Init()` 함수:

```cpp
void AISystem::Init()
{
    m_strategies[CharacterTypes::Fighter] = new FighterStrategy();
    m_strategies[CharacterTypes::Cleric]  = new ClericStrategy();   // 신규
    m_strategies[CharacterTypes::Wizard]  = new WizardStrategy();   // 신규
    // m_strategies[CharacterTypes::Rogue] = new RogueStrategy();   // 추후
}
```

### Task 4: CharacterFactory에 Cleric/Wizard 생성 추가

**파일**: `CharacterFactory.cpp`:

```cpp
std::unique_ptr<Character> CharacterFactory::Create(CharacterTypes type, Math::ivec2 pos)
{
    switch (type)
    {
        case CharacterTypes::Dragon:  return CreateDragon(pos);
        case CharacterTypes::Fighter: return CreateFighter(pos);
        case CharacterTypes::Cleric:  return CreateCleric(pos);   // 신규
        case CharacterTypes::Wizard:  return CreateWizard(pos);   // 신규
        default:
            Engine::GetLogger().LogError("CharacterFactory: Unknown type");
            return nullptr;
    }
}
```

---

## Implementation Examples

### Cleric 전투 시나리오

```
라운드 1:
  Fighter(아군) HP 30% → Cleric: AllyNeedsHeal() = true
  → 힐 스펠 시전 (S_ENH_030 "치유의 빛")
  → Fighter HP 회복

라운드 2:
  Dragon(플레이어)이 Cleric에 인접 공격
  Cleric HP 45% → IsInDanger() = true (50% 이하)
  → FindSafePosition() → 도주
```

### Wizard 전투 시나리오

```
라운드 1:
  Dragon이 근접 (거리 2)
  IsInSafeRange() = false (3 미만)
  → 도주 이동 (거리 4 확보)

라운드 2:
  안전 거리 확보됨
  HasAnySpellSlot() = true
  → S_ATK_010(화염탄) 업캐스트 시전
  → Dragon에 큰 데미지
```

---

## Rigorous Testing

테스트 파일: `Test/TestAI.h/cpp` 확장

### 테스트 1: ClericStrategy - 아군 힐 우선

```cpp
// Fighter HP를 60%로 설정
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.6f));

// Cleric 턴 실행
AIDecision decision = cleric_strategy.MakeDecision(cleric);

// 힐 스펠 결정을 반환해야 함
ASSERT(decision.type == AIDecisionType::CastSpell, "Cleric should heal ally");
ASSERT(decision.target == fighter, "Cleric should target hurt Fighter");
```

### 테스트 2: ClericStrategy - 위험 시 도주

```cpp
// Cleric HP를 40%로 설정 (50% 임계값 이하)
cleric->SetHP(static_cast<int>(cleric->GetMaxHP() * 0.4f));

AIDecision decision = cleric_strategy.MakeDecision(cleric);

// 이동(도주) 결정
ASSERT(decision.type == AIDecisionType::Move, "Cleric should retreat when in danger");
```

### 테스트 3: WizardStrategy - 안전 거리 유지

```cpp
// Dragon을 Wizard 바로 옆에 배치 (거리 1)
grid->MoveCharacter(dragon_pos, {wizard_pos.x + 1, wizard_pos.y});

AIDecision decision = wizard_strategy.MakeDecision(wizard);

// 도주 결정 (거리 확보)
ASSERT(decision.type == AIDecisionType::Move, "Wizard should retreat from adjacent enemy");
```

### 테스트 4: WizardStrategy - 슬롯 없을 때 기본 공격

```cpp
// 모든 주문 슬롯 소모
wizard->SetSpellSlots({});

AIDecision decision = wizard_strategy.MakeDecision(wizard);

// 기본 공격 또는 EndTurn
ASSERT(decision.type == AIDecisionType::Attack ||
       decision.type == AIDecisionType::EndTurn,
       "Wizard without slots should attack or end turn");
```

### 게임 내 수동 테스트

맵에 Cleric/Wizard 캐릭터를 추가하고 자동 전투 관찰:

```cpp
// GamePlay::LoadFirstMap() 또는 JSON 맵에서
auto cleric = character_factory->Create(CharacterTypes::Cleric, {2, 5});
go_manager->Add(std::move(cleric));
// 턴 순서에 추가하여 AI 행동 관찰
```
