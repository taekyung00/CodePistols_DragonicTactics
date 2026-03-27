# FighterStrategy 재구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: FighterStrategy 재구현 — `fighter.mmd` 플로우차트 완전 반영
**작성일**: 2026-03-08

**관련 파일**:
- [FighterStrategy.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/FighterStrategy.h)
- [IAIStrategy.h](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/IAIStrategy.h)
- [architecture/character_flowchart/fighter.mmd](../../../architecture/character_flowchart/fighter.mmd)

---

## 개요

현재 `FighterStrategy.cpp`는 단순 이동+공격 로직만 구현되어 있으며, 업데이트된 `fighter.mmd` 플로우차트와 다릅니다.
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
              Yes (생존) → 피의 갈망 있음? → 강타/일반(피흡)
                           없음 → 2슬롯 → 피의 갈망 시전
                                  없음 → 공포의 외침 or 종료
              No  (정상) → 축복 적용 중?
                           Yes → 격앙 버프? → 강타/일반
                           No  → 클레릭 생존? → 공포 시전 → 강타/일반
                                               → (독립) 격앙 → 공포 → 강타/일반
```

### 핵심 어빌리티

| 어빌리티 | `abilityName` | 슬롯 | 효과 |
|---------|--------------|------|------|
| 강타 (Smite) | `"강타"` | 가용 최고레벨 | 강화 공격 |
| 피의 갈망 (Bloodlust) | `"피의 갈망"` | 2레벨 | 버프: 공격 시 피흡 |
| 공포의 외침 (Fear) | `"공포의 외침"` | 1레벨 | 디버프: 드래곤 공포 |
| 격앙 (Burst) | `"격앙"` | 2레벨 | 버프: 공격력 강화 |

---

## Implementation Tasks

### Task 1: FighterStrategy.h 재작성

**기존 코드와 차이점**:
- `IsInDanger()`: HP 임계값 **40%** (기존 코드는 30%였으나 플로우차트가 40%로 변경)
- 새 헬퍼: `CanKillDragonThisTurn()`, `HasBuff()`, `HasSpellSlot()`, `IsFearActive()`, `IsInFearRange()`
- `DecideAttackAction()` 제거 → 명시적 분기로 교체

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
    bool HasBuff(Character* actor, const std::string& buffName) const;
    bool HasSpellSlot(Character* actor, int level) const;
    bool IsFearActive(Character* dragon) const;                 // 드래곤에 공포 디버프 적용 여부
    bool IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const;

    // --- 이동 ---
    Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
    bool        CanReachThisTurn(Character* actor, Character* target, GridSystem* grid) const;

    // --- Kill_Loop 서브 로직 ---
    AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);

    // --- Phase_Decision 서브 로직 ---
    AIDecision MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid);
};
```

---

### Task 2: MakeDecision() — 최상위 흐름

플로우차트의 구조 그대로, **재귀 없이 루프** 형태로 구현합니다.
`BattleOrchestrator`가 `MakeDecision()`을 반복 호출하는 구조이므로,
한 번 호출 시 **단 하나의 행동**만 결정하고 반환합니다.

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
    {
        return MakeKillLoopDecision(actor, dragon, grid);
    }

    // ============================================================
    // Phase_Decision: AP 체크
    // ============================================================
    if (actor->GetActionPoints() <= 0)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "No AP" };

    // 드래곤 인접(1칸) 여부
    int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
    {
        // 이동 구간
        return MakeFarMoveDecision(actor, dragon, grid);
    }
    else
    {
        // 교전 구간
        if (IsInDanger(actor))
            return MakeSurvivalDecision(actor, dragon, grid);
        else
            return MakeNormalCombatDecision(actor, dragon, grid);
    }
}
```

---

### Task 3: Kill_Loop — 확정 처치 루프

```cpp
AIDecision FighterStrategy::MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
    {
        // 이동
        if (actor->GetMovementRange() <= 0)
            return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no MP to reach" };

        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        return { AIDecisionType::Move, nullptr, movePos, "", "Kill loop: moving to dragon" };
    }

    // 인접 — AP 체크
    if (actor->GetActionPoints() <= 0)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no AP" };

    // 최적 슬롯 탐색 (오버킬 방지 업캐스팅)
    // 슬롯 있음 → 강타, 없음 → 일반 공격
    if (actor->HasAnySpellSlot())
        return { AIDecisionType::UseAbility, dragon, {}, "강타", "Kill loop: smite" };
    else
        return { AIDecisionType::Attack, dragon, {}, "", "Kill loop: normal attack" };
}
```

---

### Task 4: 이동 불가 시 원거리 공포 (MakeFarMoveDecision)

```cpp
AIDecision FighterStrategy::MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // 이동 가능?
    if (actor->GetMovementRange() > 0)
    {
        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        if (movePos != actor->GetGridPosition()->Get())
            return { AIDecisionType::Move, nullptr, movePos, "", "Moving towards dragon" };
    }

    // 이동 불가 — 원거리 공포 시전 가능한가?
    // 조건: 드래곤에 공포 없음 & 1레벨 슬롯 있음 & 공포 사거리 내
    if (!IsFearActive(dragon) &&
        HasSpellSlot(actor, 1) &&
        IsInFearRange(actor, dragon, grid))
    {
        return { AIDecisionType::UseAbility, dragon, {}, "공포의 외침", "Fear from range (can't move)" };
    }

    return { AIDecisionType::EndTurn, nullptr, {}, "", "Can't move or cast fear" };
}
```

---

### Task 5: 생존 시퀀스 (HP ≤ 40%)

```cpp
AIDecision FighterStrategy::MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // 피의 갈망 버프 있는가?
    if (!HasBuff(actor, "피의 갈망"))
    {
        // 없음 → 2레벨 슬롯 있으면 시전
        if (HasSpellSlot(actor, 2))
            return { AIDecisionType::UseAbility, actor, {}, "피의 갈망", "Survival: cast bloodlust" };

        // 없음 → 공포 시전 가능한가?
        if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
            return { AIDecisionType::UseAbility, dragon, {}, "공포의 외침", "Survival: cast fear (no bloodlust slot)" };

        return { AIDecisionType::EndTurn, nullptr, {}, "", "Survival: no resources" };
    }
    else
    {
        // 피의 갈망 있음 → 강타용 슬롯 있으면 피흡 극대화 강타
        if (actor->HasAnySpellSlot())
            return { AIDecisionType::UseAbility, dragon, {}, "강타", "Survival: smite (lifesteal max)" };
        else
            return { AIDecisionType::Attack, dragon, {}, "", "Survival: normal attack (lifesteal)" };
    }
}
```

---

### Task 6: 일반 교전 로직 (HP > 40%)

```cpp
AIDecision FighterStrategy::MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    bool hasBless  = HasBuff(actor, "축복");
    bool hasBurst  = HasBuff(actor, "격앙");
    bool clericAlive = (FindCleric() != nullptr);

    // ----- 축복 적용 중 -----
    if (hasBless)
    {
        if (!hasBurst)
        {
            // 격앙 없음 → AP >= 2이면 격앙 시전
            if (actor->GetActionPoints() >= 2 && HasSpellSlot(actor, 2))
                return { AIDecisionType::UseAbility, actor, {}, "격앙", "Normal: cast burst (blessed)" };
        }
        // 격앙 있음(혹은 AP 부족) → 강타 or 일반
        if (actor->HasAnySpellSlot())
            return { AIDecisionType::UseAbility, dragon, {}, "강타", "Normal: smite (blessed+burst)" };
        else
            return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (blessed+burst, no slot)" };
    }

    // ----- 축복 없음 -----
    if (clericAlive)
    {
        // 클레릭 생존 → 공포 시전 가능한가?
        if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
            return { AIDecisionType::UseAbility, dragon, {}, "공포의 외침", "Normal: fear (cleric alive)" };

        // 공포 이미 있거나 슬롯 없음 → 강타 or 일반
        if (actor->HasAnySpellSlot())
            return { AIDecisionType::UseAbility, dragon, {}, "강타", "Normal: smite (cleric alive)" };
        else
            return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (cleric alive)" };
    }
    else
    {
        // 클레릭 부재 → 격앙 있는가?
        if (!hasBurst)
        {
            if (HasSpellSlot(actor, 2) && actor->GetActionPoints() >= 2)
                return { AIDecisionType::UseAbility, actor, {}, "격앙", "Normal: cast burst (independent)" };
        }

        // 격앙 있거나 슬롯 없음 → 공포 → 강타 → 일반
        if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
            return { AIDecisionType::UseAbility, dragon, {}, "공포의 외침", "Normal: fear (no cleric)" };

        if (actor->HasAnySpellSlot())
            return { AIDecisionType::UseAbility, dragon, {}, "강타", "Normal: smite (no cleric)" };
        else
            return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (no cleric)" };
    }
}
```

---

### Task 7: 헬퍼 함수 구현

```cpp
// HP 40% 이하 판단 (플로우차트: CheckHP 40%)
bool FighterStrategy::IsInDanger(Character* actor) const
{
    return actor->GetHPPercentage() <= 0.4f;
}

// 버프 확인 — StatusEffect 시스템 구현 후 교체
bool FighterStrategy::HasBuff(Character* actor, const std::string& buffName) const
{
    return actor->HasBuff(buffName); // Character::HasBuff(string) 구현 필요
}

// 특정 레벨 주문 슬롯 보유 여부
bool FighterStrategy::HasSpellSlot(Character* actor, int level) const
{
    return actor->GetSpellSlots()->HasSlot(level);
}

// 드래곤에 '공포' 디버프 활성 여부
bool FighterStrategy::IsFearActive(Character* dragon) const
{
    return dragon->HasDebuff("공포"); // Character::HasDebuff(string) 구현 필요
}

// 공포의 외침 사거리 내 여부 (통상 3칸 이하)
bool FighterStrategy::IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const
{
    int dist = grid->ManhattanDistance(
        actor->GetGridPosition()->Get(),
        dragon->GetGridPosition()->Get()
    );
    return dist <= 3; // 공포의 외침 사거리 — spell_table.csv에서 확인 후 조정
}

// 이번 턴 내 드래곤 도달 가능 여부 (A* 경로 길이 <= 현재 MP)
bool FighterStrategy::CanReachThisTurn(Character* actor, Character* dragon, GridSystem* grid) const
{
    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();

    // 드래곤 인접 타일 중 경로 탐색
    static const Math::ivec2 offsets[4] = { {0,1},{0,-1},{-1,0},{1,0} };
    int mp = actor->GetMovementRange();

    for (const auto& off : offsets)
    {
        Math::ivec2 attackPos = tgtPos + off;
        if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos)) continue;
        auto path = grid->FindPath(myPos, attackPos);
        if (!path.empty() && static_cast<int>(path.size()) <= mp)
            return true;
    }
    return false; // 이미 인접한 경우도 true
}

// 가용 슬롯 총합 데미지로 드래곤 확정 처치 가능 여부
// (SpellSystem 연동 후 정교하게 구현 — 현재는 간소화 버전)
bool FighterStrategy::CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const
{
    // TODO: SpellSystem::CalculateSpellDamage 연동 후 정교한 계산
    // 현재는 HP 20% 이하면 처치 가능으로 간주
    return dragon->GetHPPercentage() <= 0.2f;
}
```

---

### Task 8: FindCleric() 활성화

```cpp
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
  → CanReachThisTurn: true (인접)
  → CanKillDragonThisTurn: false (드래곤 HP 70%)
  → Phase_Decision: AP > 0 → adjacent → HP 40% 이하(35%) → MakeSurvivalDecision
  → HasBuff("피의 갈망"): false → HasSpellSlot(2): true
  → return UseAbility("피의 갈망")

다음 BattleOrchestrator 호출:
  MakeDecision 호출:
  → Phase_Decision: AP > 0 → adjacent → HP 40% 이하 → MakeSurvivalDecision
  → HasBuff("피의 갈망"): true → HasAnySpellSlot: true
  → return UseAbility("강타")
```

### 정상 교전 시나리오 (HP 70%, 축복 없음, 클레릭 생존, 드래곤 공포 없음)

```
MakeDecision:
  → Phase_Decision: adjacent, HP > 40% → MakeNormalCombatDecision
  → hasBless: false → clericAlive: true
  → !IsFearActive(dragon): true → HasSpellSlot(1): true → IsInFearRange: true
  → return UseAbility("공포의 외침")

다음 호출 (드래곤에 공포 적용됨):
  → MakeNormalCombatDecision
  → hasBless: false → clericAlive: true → IsFearActive(dragon): true (이미 공포)
  → HasAnySpellSlot: true → return UseAbility("강타")
```

---

## Rigorous Testing

### 테스트 1: 킬캐치 — 이번 턴 강타 루프 시작

```cpp
// 드래곤 HP를 15%로 설정
dragon->SetHP(static_cast<int>(dragon->GetMaxHP() * 0.15f));
// Fighter를 1칸 거리에 배치, MP > 0, 슬롯 보유

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should enter kill loop with smite");
ASSERT(decision.abilityName == "강타", "Kill loop ability should be 강타");
```

### 테스트 2: 생존 — 피의 갈망 시전 우선

```cpp
// Fighter HP 38%, 피의 갈망 없음, 2레벨 슬롯 있음, 드래곤 인접
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.38f));

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should use ability in survival");
ASSERT(decision.abilityName == "피의 갈망", "Should cast bloodlust when HP <= 40% and no bloodlust");
ASSERT(decision.target == fighter, "Bloodlust targets self");
```

### 테스트 3: 정상 교전 — 축복+격앙 → 강타

```cpp
// HP > 40%, 축복 버프 있음, 격앙 버프 있음, 슬롯 있음, 드래곤 인접
fighter->ApplyBuff("축복");
fighter->ApplyBuff("격앙");

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should smite");
ASSERT(decision.abilityName == "강타", "Should use smite with bless+burst");
ASSERT(decision.target == dragon, "Target should be dragon");
```

### 테스트 4: 이동 불가 시 원거리 공포

```cpp
// Fighter를 이동 불가 위치에 배치 (MP=0 or 경로 없음)
// 드래곤과 공포 사거리(3칸) 내, 드래곤에 공포 없음, 1레벨 슬롯 있음
fighter->SetMovementRange(0);

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should cast fear from range");
ASSERT(decision.abilityName == "공포의 외침", "Should use fear when can't move");
```

### 테스트 5: 클레릭 부재 시 독립 격앙 시전

```cpp
// HP > 40%, 축복 없음, 격앙 없음, 클레릭 없음, AP >= 2, 2레벨 슬롯 있음, 드래곤 인접
// (FindCleric returns nullptr)

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should cast burst independently");
ASSERT(decision.abilityName == "격앙", "Should cast burst when no cleric");
```

### 테스트 6: HP 임계값 경계 (40% 정확히)

```cpp
// HP 정확히 40%
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.40f));

AIDecision survival = fighter_strategy.MakeDecision(fighter);
// IsInDanger: <= 0.4f → true → Survival sequence
ASSERT(/* survival sequence entered */);

// HP 41%
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.41f));
AIDecision normal = fighter_strategy.MakeDecision(fighter);
// IsInDanger: false → Normal combat
ASSERT(/* normal combat entered */);
```
