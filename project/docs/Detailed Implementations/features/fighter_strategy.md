# FighterStrategy 재구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: FighterStrategy 재구현 — `fighter.mmd` 플로우차트 완전 반영
**작성일**: 2026-03-08

**관련 파일**:

- [FighterStrategy.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/FighterStrategy.h)
- [IAIStrategy.h](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/IAIStrategy.h)
- [architecture/character_flowchart/fighter.mmd](../../../architecture/character_flowchart/fighter.mmd)
- [spell_system.md](./spell_system.md)
- [Assets/Data/spell_table.csv](../../../DragonicTactics/Assets/Data/spell_table.csv)

---

## 개요

현재 `FighterStrategy.cpp`는 단순 이동+공격 로직만 구현되어 있으며, `fighter.mmd` 플로우차트와 다릅니다.
이 문서는 플로우차트를 **완벽하게** 반영한 재구현을 다룹니다.

### fighter.mmd 플로우차트 요약

```
파이터 턴 시작
  ↓ 상태 변수 초기화: [공격 여부 = False]
  ↓
[0. 킬캐치] A* 도달 가능? → 확정 처치 가능? → Kill_Loop (강타/일반 반복)
  ↓ No
[판단 지점 루프]
  AP > 0?
    No → 턴 종료
    Yes → 드래곤 인접(1칸)?
      No  → MP > 0 & A* 경로?
              Yes → 이동 → 판단 지점
              No  → 공포 사거리 시전 or 종료
      Yes → HP 40% 이하?
              Yes (생존) → 피의 갈망 있음?
                           있음 → 강타용 슬롯? → 강타(피흡) / 일반(피흡)
                           없음 → 2슬롯 → 피의 갈망 시전 / 없음 → 공포 or 일반 공격
              No  (정상) → 축복 적용 중?
                           Yes → 광란 버프? → 광란 시전(AP>=2) or 강타/일반
                           No  → 클레릭 생존? → Yes: 공포 or 일반 공격
                                               No:  광란 있음? → 강타/일반
                                                    없음 → 광란 시전(AP>=2) / 공포 or 일반 공격
```

---

## 스펠 ID 매핑 (spell_table.csv 기준)

> **⚠️ `decision.abilityName`은 반드시 spell_table.csv의 ID 컬럼 값을 사용해야 합니다.**
> `SpellSystem::CastSpell`은 `spells_[spell_id]`로 조회합니다. 한글 이름으로 전달하면 스펠을 찾지 못합니다.

| 어빌리티              | `abilityName` (CSV ID) | 슬롯 레벨           | 대상        | 사거리    | 효과                                                                  |
| ----------------- | ---------------------- | --------------- | --------- | ------ | ------------------------------------------------------------------- |
| 강타 (Smite)        | `"S_ATK_050"`          | **1** (업캐스트 가능) | 적군 단일     | 1칸     | 3d8+(레벨차)d8 피해                                                      |
| 피의 갈망 (Bloodlust) | `"S_ENH_010"`          | **2**           | 자신        | -      | 이번 턴 피해 절반 회복                                                       |
| 공포의 외침 (Fear)     | `"S_DEB_020"`          | **1**           | 시전자 주변 적군 | **3칸** | 3턴: 피해 -3, 스피드 -1                                                   |
| 광란 (Frenzy)       | `"S_ENH_020"`          | **0 (슬롯 불필요)**  | 자신        | -      | 다음 공격 10↑ 피해 시 적 무작위 디버프                                            |
| 최후의 저항            | `"S_ENH_070"`          | **0 (슬롯 불필요)**  | 자신        | -      | 아군 2명↑ 사망 시 사용가능, 2레벨 슬롯 1 회복 + 디버프 제거 + 2d10 회복 + 축복/신속 (전투 1회 한정) |

> **⚠️ 광란 (S_ENH_020)은 스펠슬롯이 필요 없습니다 (level=0).**
> 기존에 `HasSpellSlot(actor, 2)` 조건으로 광란을 막던 코드는 **틀린 로직**입니다.
> 광란 시전 조건은 **AP >= 2** 만으로 충분합니다.

---

## Implementation Tasks

### Task 1: FighterStrategy.h 재작성

**기존 코드와 차이점**:

- `IsInDanger()`: HP 임계값 **40%** (기존 30%에서 변경)
- 새 헬퍼: `CanKillDragonThisTurn()`, `HasBuff()`, `IsFearActive()` (모두 Week 6+ stub)
- `ShouldUseSpellAttack()`, `DecideAttackAction()` 제거 → 명시적 분기로 교체

```cpp
/**
 * @file FighterStrategy.h
 * @brief 파이터 AI 전략 — fighter.mmd 플로우차트 완전 반영
 */
#pragma once
#include "IAIStrategy.h"

class GridSystem;

class FighterStrategy : public IAIStrategy
{
public:
    AIDecision MakeDecision(Character* actor) override;

private:
    // --- 타겟 탐색 ---
    Character* FindDragon();
    Character* FindCleric();

    // --- 판단 헬퍼 ---
    bool IsInDanger(Character* actor) const;                    // HP 40% 이하
    bool CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const;
    bool HasBuff(Character* actor, const std::string& buffName) const; // TODO: Week 6+ StatusEffect 연동
    bool HasSpellSlot(Character* actor, int level) const;
    bool IsFearActive(Character* dragon) const;                 // TODO: Week 6+ StatusEffect 연동
    bool IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const;

    // --- 이동 ---
    Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
    bool        CanReachThisTurn(Character* actor, Character* target, GridSystem* grid) const;

    // --- 서브 의사결정 ---
    AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid);
};
```

---

### Task 2: MakeDecision() — 최상위 흐름

`BattleOrchestrator`가 `MakeDecision()`을 반복 호출하므로, 한 번 호출 시 **단 하나의 행동**만 반환합니다.

```cpp
AIDecision FighterStrategy::MakeDecision(Character* actor)
{
    GridSystem* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    Character*  dragon = FindDragon();

    if (!dragon)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "No dragon found" };

    // ============================================================
    // 0. 킬캐치 오버라이드
    // A* 경로로 이번 턴 내 도달 가능 & 확정 처치 가능 시 진입
    // ============================================================
    if (CanReachThisTurn(actor, dragon, grid) && CanKillDragonThisTurn(actor, dragon, grid))
        return MakeKillLoopDecision(actor, dragon, grid);

    // ============================================================
    // Phase_Decision: AP 체크
    // ============================================================
    if (actor->GetActionPoints() <= 0)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "No AP" };

    int  dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
        return MakeFarMoveDecision(actor, dragon, grid);
    else if (IsInDanger(actor))
        return MakeSurvivalDecision(actor, dragon, grid);
    else
        return MakeNormalCombatDecision(actor, dragon, grid);
}
```

---

### Task 3: Kill_Loop — 확정 처치 루프

```cpp
AIDecision FighterStrategy::MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    int  dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
    {
        if (actor->GetMovementRange() <= 0)
            return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no MP to reach" };

        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        return { AIDecisionType::Move, nullptr, movePos, "", "Kill loop: moving to dragon" };
    }

    if (actor->GetActionPoints() <= 0)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no AP" };

    // 슬롯 있음 → 강타(S_ATK_050), 없음 → 일반 공격
    if (actor->HasAnySpellSlot())
        return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_050", "Kill loop: smite" };
    else
        return { AIDecisionType::Attack, dragon, {}, "", "Kill loop: normal attack" };
}
```

---

### Task 4: 이동 / 원거리 공포 (MakeFarMoveDecision)

공포의 외침 (S_DEB_020): 사거리 **3칸** (spell_table.csv 기준), level 1 슬롯 필요

```cpp
AIDecision FighterStrategy::MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    if (actor->GetMovementRange() > 0)
    {
        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        if (movePos != actor->GetGridPosition()->Get())
            return { AIDecisionType::Move, nullptr, movePos, "", "Moving towards dragon" };
    }

    // 이동 불가 — 원거리 공포(S_DEB_020) 시전 가능한가?
    // 조건: 드래곤에 공포 없음 & 1레벨 슬롯 있음 & 3칸 이내
    if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
        return { AIDecisionType::UseAbility, dragon, {}, "S_DEB_020", "Fear from range (can't move)" };

    return { AIDecisionType::EndTurn, nullptr, {}, "", "Can't move or cast fear" };
}
```

---

### Task 5: 생존 시퀀스 (HP ≤ 40%)

**⚠️ 수정 포인트**: 피의 갈망 없음 + 2레벨 슬롯 없음 시 `EndTurn`이 아닌 **일반 공격** (mmd: `Act_NormalLoop`)

```cpp
AIDecision FighterStrategy::MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // Lifesteal 버프 있는가? (stub: 항상 false → Week 6+ StatusEffect 연동)
    if (!HasBuff(actor, "Lifesteal"))
    {
        // 없음 → 2레벨 슬롯 있으면 Bloodlust(S_ENH_010) 시전
        if (HasSpellSlot(actor, 2))
            return { AIDecisionType::UseAbility, actor, {}, "S_ENH_010", "Survival: cast bloodlust" };

        // 없음 → Fear(S_DEB_020) 시전 가능한가?
        if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
            return { AIDecisionType::UseAbility, dragon, {}, "S_DEB_020", "Survival: cast fear" };

        // 자원 없음 → 일반 공격 (NOT EndTurn, mmd: Act_NormalLoop)
        return { AIDecisionType::Attack, dragon, {}, "", "Survival: normal attack (no resources)" };
    }
    else
    {
        // Lifesteal 있음 → 강타(S_ATK_050) 슬롯 있으면 피흡 극대화
        if (actor->HasAnySpellSlot())
            return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_050", "Survival: smite (lifesteal)" };
        else
            return { AIDecisionType::Attack, dragon, {}, "", "Survival: normal attack (lifesteal)" };
    }
}
```

---

### Task 6: 일반 교전 로직 (HP > 40%)

**⚠️ 핵심 수정**:

- **광란(S_ENH_020) = level 0** → `HasSpellSlot(actor, 2)` 조건 **삭제**, AP >= 2만 확인
- **클레릭 생존 경로**: mmd는 `공포 → 일반 공격`만 존재 (강타 없음)
- **클레릭 부재 + 광란 있음**: mmd는 CheckSmiteSlot 직행 (공포 먼저 체크 안 함)

> **⚠️ mmd 불일치 주의**: `fighter.mmd`의 `Check2ndSlot` 노드는 `"2레벨 슬롯 있음 & AP >= 2"` 라고 표기되어 있으나,
> `spell_table.csv`에서 `S_ENH_020 (광란)`의 `Required Slot Level = 0`이다. 슬롯 체크는 불필요하며
> **CSV가 정본**이므로 코드 구현 시 `AP >= 2` 조건만 사용한다. (mmd의 "2레벨 슬롯" 표기는 구버전 잔재)

```cpp
AIDecision FighterStrategy::MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    bool hasBless    = HasBuff(actor, "Blessing"); // stub: false
    bool hasBurst    = HasBuff(actor, "Frenzy");   // stub: false
    bool clericAlive = (FindCleric() != nullptr);

    // ----- 축복 적용 중 -----
    if (hasBless)
    {
        if (!hasBurst)
        {
            // 광란(S_ENH_020) 없음 → AP >= 2이면 광란 시전 (슬롯 불필요 — level 0)
            if (actor->GetActionPoints() >= 2)
                return { AIDecisionType::UseAbility, actor, {}, "S_ENH_020", "Normal: cast burst (blessed)" };
        }
        // 광란 있거나 AP 부족 → CheckSmiteSlot
        if (actor->HasAnySpellSlot())
            return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_050", "Normal: smite (blessed+burst)" };
        else
            return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (blessed, no slot)" };
    }

    // ----- 축복 없음 -----
    if (clericAlive)
    {
        // 클레릭 생존 → CheckFearNear → 공포 or 일반 공격 (강타 없음)
        if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
            return { AIDecisionType::UseAbility, dragon, {}, "S_DEB_020", "Normal: fear (cleric alive)" };

        return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (cleric alive)" };
    }
    else
    {
        // 클레릭 부재
        if (hasBurst)
        {
            // 광란 있음 → CheckSmiteSlot 직행 (공포 먼저 아님)
            if (actor->HasAnySpellSlot())
                return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_050", "Normal: smite (no cleric, has burst)" };
            else
                return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (no cleric, has burst)" };
        }
        else
        {
            // 광란 없음 → 광란(S_ENH_020) 시전 시도 (AP >= 2, 슬롯 불필요)
            if (actor->GetActionPoints() >= 2)
                return { AIDecisionType::UseAbility, actor, {}, "S_ENH_020", "Normal: cast burst (independent)" };

            // AP 부족 → CheckFearNear → 공포 or 일반 공격
            if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
                return { AIDecisionType::UseAbility, dragon, {}, "S_DEB_020", "Normal: fear (no cleric)" };

            return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (no cleric)" };
        }
    }
}
```

---

### Task 7: 헬퍼 함수 구현

```cpp
bool FighterStrategy::IsInDanger(Character* actor) const
{
    return actor->GetHPPercentage() <= 0.4f;
}

// 버프 확인 — StatusEffect 구현 전까지 stub (항상 false)
bool FighterStrategy::HasBuff(Character* actor, const std::string& buffName) const
{
    // TODO: Week 6+ actor->HasBuff(buffName) 연동
    (void)actor; (void)buffName;
    return false;
}

// 특정 레벨 주문 슬롯 보유 여부 (Character::HasSpellSlot 직접 사용)
bool FighterStrategy::HasSpellSlot(Character* actor, int level) const
{
    return actor->HasSpellSlot(level);
}

// 드래곤에 공포 디버프 활성 여부 — StatusEffect 구현 전까지 stub (항상 false)
bool FighterStrategy::IsFearActive(Character* dragon) const
{
    // TODO: Week 6+ dragon->HasDebuff("Fear") 연동
    (void)dragon;
    return false;
}

// 공포의 외침(S_DEB_020) 사거리 내 여부 — CSV: range=3칸
bool FighterStrategy::IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const
{
    int dist = grid->ManhattanDistance(
        actor->GetGridPosition()->Get(),
        dragon->GetGridPosition()->Get()
    );
    return dist <= 3; // S_DEB_020 range = 3칸 (spell_table.csv 기준)
}

// 이번 턴 내 드래곤 도달 가능 여부 (A* 경로 길이 <= 현재 MP)
bool FighterStrategy::CanReachThisTurn(Character* actor, Character* dragon, GridSystem* grid) const
{
    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();

    // 이미 공격 사거리 내 (인접)
    if (grid->ManhattanDistance(myPos, tgtPos) <= actor->GetAttackRange())
        return true;

    int mp = actor->GetMovementRange();
    static const Math::ivec2 offsets[4] = { {0,1}, {0,-1}, {-1,0}, {1,0} };
    for (const auto& off : offsets)
    {
        Math::ivec2 attackPos = tgtPos + off;
        if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos)) continue;
        auto path = grid->FindPath(myPos, attackPos);
        if (!path.empty() && static_cast<int>(path.size()) <= mp)
            return true;
    }
    return false;
}

// 확정 처치 가능 여부
// TODO: SpellSystem::CalculateSpellDamage 연동 후 정교한 계산
bool FighterStrategy::CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const
{
    (void)actor; (void)grid;
    return dragon->GetHPPercentage() <= 0.2f;
}
```

---

### Task 8: FindCleric() 구현

```cpp
Character* FighterStrategy::FindDragon()
{
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    for (auto* c : grid->GetAllCharacters())
    {
        if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Dragon)
            return c;
    }
    return nullptr;
}

Character* FighterStrategy::FindCleric()
{
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    for (auto* c : grid->GetAllCharacters())
    {
        if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Cleric)
            return c;
    }
    return nullptr;
}
```

---

## Implementation Examples

### 생존 시나리오 (HP 35%, 피의 갈망 없음, 2레벨 슬롯 있음)

```
MakeDecision 호출:
  → CanKillDragonThisTurn: false (드래곤 HP 70%)
  → Phase_Decision: AP > 0 → adjacent → HP 35% ≤ 40% → MakeSurvivalDecision
  → HasBuff("Lifesteal"): false (stub) → HasSpellSlot(2): true
  → return UseAbility("S_ENH_010")  // Bloodlust

다음 BattleOrchestrator 호출 (Week 6+ StatusEffect 연동 후):
  → MakeSurvivalDecision → HasBuff("Lifesteal"): true → HasAnySpellSlot: true
  → return UseAbility("S_ATK_050")  // 강타 (피흡 극대화)
```

### 생존 시나리오 (HP 35%, 자원 없음)

```
MakeDecision → MakeSurvivalDecision
  → HasBuff: false → HasSpellSlot(2): false
  → IsFearActive: false & HasSpellSlot(1): false
  → return Attack  // 일반 공격 (NOT EndTurn — mmd 명시)
```

### 정상 교전 — 축복 없음, 클레릭 없음, 광란 없음, AP=2

```
MakeDecision → MakeNormalCombatDecision
  → hasBless: false → clericAlive: false → hasBurst: false
  → GetActionPoints() >= 2: true
  → return UseAbility("S_ENH_020")  // 광란 (슬롯 불필요, AP만 소모)
```

### 정상 교전 — 축복 없음, 클레릭 생존

```
MakeDecision → MakeNormalCombatDecision
  → hasBless: false → clericAlive: true
  → !IsFearActive: true → HasSpellSlot(1): true → IsInFearRange: true
  → return UseAbility("S_DEB_020")  // 공포의 외침

다음 호출 (IsFearActive: true):
  → clericAlive: true → IsFearActive: true
  → return Attack  // 일반 공격 (강타 아님, mmd 명시)
```

---

## Rigorous Testing

### 테스트 1: 킬캐치 — 강타(S_ATK_050) 진입

```cpp
dragon->SetHP(static_cast<int>(dragon->GetMaxHP() * 0.15f)); // HP ≤ 20%

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Kill loop: smite");
ASSERT(decision.abilityName == "S_ATK_050", "Kill loop abilityName must be CSV ID");
```

### 테스트 2: 생존 — 피의 갈망(S_ENH_010) 시전

```cpp
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.38f));
// HasSpellSlot(2) = true

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility);
ASSERT(decision.abilityName == "S_ENH_010", "Bloodlust CSV ID");
ASSERT(decision.target == fighter, "Bloodlust targets self");
```

### 테스트 3: 생존 — 자원 없음 → 일반 공격 (EndTurn 아님)

```cpp
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.38f));
fighter->SetSpellSlots({});

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::Attack, "Normal attack when no resources (NOT EndTurn)");
ASSERT(decision.target == dragon);
```

### 테스트 4: 광란 — 슬롯 없어도 AP만 있으면 시전 가능

```cpp
// HP > 40%, 축복 없음, 클레릭 없음, 광란 없음, AP=2, 슬롯=0 (레벨 2 없음)
fighter->SetSpellSlots({}); // 슬롯 없음
fighter->SetActionPoints(2);

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Burst needs no slot");
ASSERT(decision.abilityName == "S_ENH_020", "Frenzy CSV ID");
// S_ENH_020 = level 0 spell → HasSpellSlot 체크 불필요
```

### 테스트 5: 공포의 외침 — CSV ID 검증

```cpp
// HP > 40%, 축복 없음, 클레릭 있음, 공포 없음, 1레벨 슬롯 있음, 3칸 내

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.abilityName == "S_DEB_020", "Fear CSV ID");
ASSERT(decision.target == dragon);
```

### 테스트 6: 이동 불가 시 원거리 공포

```cpp
// Fighter MP=0, 드래곤 3칸 내, 1레벨 슬롯 있음

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility);
ASSERT(decision.abilityName == "S_DEB_020", "Fear from range");
```

### 테스트 7: HP 임계값 경계 (40%)

```cpp
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.40f));
// IsInDanger: 0.40 <= 0.4f → true → Survival sequence
AIDecision s = fighter_strategy.MakeDecision(fighter);
// Survival sequence 진입 확인

fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.41f));
// IsInDanger: false → Normal combat
AIDecision n = fighter_strategy.MakeDecision(fighter);
// Normal combat 진입 확인
```

---

## GamePlay / BattleOrchestrator 연동 주의사항

### BattleOrchestrator 동작 방식

`BattleOrchestrator::HandleAITurn`은 매 프레임:

1. `MovementComponent::IsMoving()` → 이동 중이면 대기
2. 0.4초 busy-wait
3. `ai_system->MakeDecision(actor)` → 단 하나의 행동 결정
4. `EndTurn` → `EndCurrentTurn()` / 그 외 → `ExecuteDecision()` → return

**fighter.mmd의 "Phase_Decision 루프"는 BattleOrchestrator의 반복 호출로 구현됩니다.**

---

### ⚠️ UseAbility 실행 경로 (spell_system.md 기준)

`AISystem::ExecuteDecision`의 UseAbility:

```cpp
case AIDecisionType::UseAbility:
  if (spell_system)
    spell_system->CastSpell(actor, decision.abilityName, decision.target->GetGridPosition()->Get());
  break;
```

**`CastSpell`은 `spells_[spell_id]`로 조회** → `decision.abilityName`이 `"강타"` 등 한글 이름이면 스펠 미검색.

**현재 SpellSystem 상태**: `MockSpellBase` 기반 구형 구현. `spell_system.md`의 CSV 기반 재구현 완료 후 동작 가능.

현재 동작 가능한 Fighter 결정: **`Attack`(일반), `Move`(이동), `EndTurn`**

---

### stub 현황 및 Week 6+ 해제 목록

| stub                         | 현재      | 해제 조건                        |
| ---------------------------- | ------- | ----------------------------- |
| `HasBuff(actor, "Lifesteal")` | `false` | `actor->HasBuff("Lifesteal")` |
| `HasBuff(actor, "Blessing")`  | `false` | `actor->HasBuff("Blessing")`  |
| `HasBuff(actor, "Frenzy")`    | `false` | `actor->HasBuff("Frenzy")`    |
| `IsFearActive(dragon)`        | `false` | `dragon->HasDebuff("Fear")`   |

**stub 상태의 실제 동작 (HP > 40%, 인접 시)**:

- hasBless=false, clericAlive=false, hasBurst=false → 광란(S_ENH_020) 시전 시도
- SpellSystem 미등록 시 → 아무것도 안 일어남 (AP 미소모) → **무한 루프**
- SpellSystem 등록 후 → 광란 성공 → 다음 호출 → hasBurst=false(stub) → 광란 재시전 반복

→ **Fighter 전략 완전 동작은 Week 6+ StatusEffect 연동 후 가능**

---

### 미구현: 최후의 저항 (S_ENH_070)

`S_ENH_070` (파이터 + 로그/위자드/클레릭 공유)는 **특수 조건 스펠**로 현재 FighterStrategy에 미반영:

- 발동 조건: "아군 2명 이상 사망"
- level 0 (슬롯 불필요), 전투 1회 한정
- 효과: 2레벨 슬롯 1 회복 + 디버프 제거 + 2d10 회복 + 축복/신속 부여

구현 시 `MakeSurvivalDecision` 앞단에 조건 체크 추가 필요 (아군 사망 수 카운트 API 필요).
