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
              Yes (생존) → 피의 갈망 있음?
                           있음 → 강타용 슬롯? → 강타(피흡) / 일반(피흡)
                           없음 → 2슬롯 → 피의 갈망 시전 / 없음 → 공포 or 일반 공격
              No  (정상) → 축복 적용 중?
                           Yes → 격앙 버프? → 격앙 시전(AP>=2) or 강타/일반
                           No  → 클레릭 생존? → Yes: 공포 or 일반 공격
                                               No:  격앙 있음? → 강타/일반
                                                    없음 → 격앙 시전(AP>=2) / 공포 or 일반 공격
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
    bool IsInDanger(Character* actor) const;                                                   // HP 40% 이하
    bool CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const;
    bool HasBuff(Character* actor, const std::string& buffName) const;                        // TODO: Week 6+ StatusEffect 연동
    bool HasSpellSlot(Character* actor, int level) const;
    bool IsFearActive(Character* dragon) const;                                               // TODO: Week 6+ StatusEffect 연동
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
    int  dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
    {
        return MakeFarMoveDecision(actor, dragon, grid);
    }
    else
    {
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
    int  dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
    {
        // 이동
        if (actor->GetMovementRange() <= 0)
        {
            // K_CanMove -- No --> Phase_Decision (mmd 명시)
            // MP 소진 후 인접 미달 → 일반 교전 판단으로 위임
            // (CanReachThisTurn이 true였으므로 이 분기는 경로 차단 등 예외 상황)
            return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no MP to reach" };
        }

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

### Task 4: 이동 / 원거리 공포 (MakeFarMoveDecision)

```cpp
AIDecision FighterStrategy::MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // 이동 가능? (MP > 0 & A* 경로 있음)
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

**⚠️ 수정 포인트**: 피의 갈망 없음 + 2레벨 슬롯 없음 시 `EndTurn`이 아닌 **일반 공격** (mmd: `Act_NormalLoop`)

```cpp
AIDecision FighterStrategy::MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // 피의 갈망 버프 있는가?
    if (!HasBuff(actor, "피의 갈망"))
    {
        // 없음 → 2레벨 슬롯 있으면 피의 갈망 시전
        if (HasSpellSlot(actor, 2))
            return { AIDecisionType::UseAbility, actor, {}, "피의 갈망", "Survival: cast bloodlust" };

        // 2레벨 슬롯 없음 → CheckFearNear
        if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
            return { AIDecisionType::UseAbility, dragon, {}, "공포의 외침", "Survival: cast fear" };

        // 공포도 못 씀 → 일반 공격 (Act_NormalLoop, NOT EndTurn)
        return { AIDecisionType::Attack, dragon, {}, "", "Survival: normal attack (no resources)" };
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

**⚠️ 수정 포인트**:
- **클레릭 생존 경로**: mmd는 `공포 → 일반 공격`만 존재. 강타 없음
- **클레릭 부재 + 격앙 있음**: mmd는 `CheckSmiteSlot`으로 직행. 공포 먼저 체크하지 않음

```cpp
AIDecision FighterStrategy::MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    bool hasBless    = HasBuff(actor, "축복");
    bool hasBurst    = HasBuff(actor, "격앙");
    bool clericAlive = (FindCleric() != nullptr);

    // ----- 축복 적용 중 -----
    if (hasBless)
    {
        if (!hasBurst)
        {
            // 격앙 없음 → AP >= 2 & 2레벨 슬롯 있으면 격앙 시전
            if (actor->GetActionPoints() >= 2 && HasSpellSlot(actor, 2))
                return { AIDecisionType::UseAbility, actor, {}, "격앙", "Normal: cast burst (blessed)" };
            // AP 부족 → CheckSmiteSlot 진행
        }
        // 격앙 있거나(HasBurst=Yes) AP 부족(CheckBurstAP→No) → CheckSmiteSlot
        if (actor->HasAnySpellSlot())
            return { AIDecisionType::UseAbility, dragon, {}, "강타", "Normal: smite (blessed+burst)" };
        else
            return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (blessed, no slot)" };
    }

    // ----- 축복 없음 -----
    if (clericAlive)
    {
        // 클레릭 생존 → CheckFearNear → 공포 or 일반 공격 (강타 없음, mmd 명시)
        if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
            return { AIDecisionType::UseAbility, dragon, {}, "공포의 외침", "Normal: fear (cleric alive)" };

        return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (cleric alive)" };
    }
    else
    {
        // 클레릭 부재
        if (hasBurst)
        {
            // 격앙 있음 → 바로 CheckSmiteSlot (공포 먼저 체크 안 함, mmd 명시)
            if (actor->HasAnySpellSlot())
                return { AIDecisionType::UseAbility, dragon, {}, "강타", "Normal: smite (no cleric, has burst)" };
            else
                return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (no cleric, has burst)" };
        }
        else
        {
            // 격앙 없음 → 격앙 시전 시도
            if (HasSpellSlot(actor, 2) && actor->GetActionPoints() >= 2)
                return { AIDecisionType::UseAbility, actor, {}, "격앙", "Normal: cast burst (independent)" };

            // 슬롯/AP 부족 → CheckFearNear → 공포 or 일반 공격
            if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
                return { AIDecisionType::UseAbility, dragon, {}, "공포의 외침", "Normal: fear (no cleric)" };

            return { AIDecisionType::Attack, dragon, {}, "", "Normal: attack (no cleric)" };
        }
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

// 버프 확인 — StatusEffect 시스템 구현 전까지 stub (항상 false)
bool FighterStrategy::HasBuff(Character* actor, const std::string& buffName) const
{
    // TODO: Week 6+ actor->HasBuff(buffName) 연동
    (void)actor; (void)buffName;
    return false;
}

// 특정 레벨 주문 슬롯 보유 여부
bool FighterStrategy::HasSpellSlot(Character* actor, int level) const
{
    return actor->HasSpellSlot(level); // Character::HasSpellSlot(int) 사용
}

// 드래곤에 '공포' 디버프 활성 여부 — StatusEffect 시스템 구현 전까지 stub (항상 false)
bool FighterStrategy::IsFearActive(Character* dragon) const
{
    // TODO: Week 6+ dragon->HasDebuff("공포") 연동
    (void)dragon;
    return false;
}

// 공포의 외침 사거리 내 여부 (통상 3칸 이하)
bool FighterStrategy::IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const
{
    int dist = grid->ManhattanDistance(
        actor->GetGridPosition()->Get(),
        dragon->GetGridPosition()->Get()
    );
    return dist <= 3; // 공포의 외침 사거리 — spell_table.csv 기준
}

// 이번 턴 내 드래곤 도달 가능 여부 (A* 경로 길이 <= 현재 MP)
bool FighterStrategy::CanReachThisTurn(Character* actor, Character* dragon, GridSystem* grid) const
{
    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();

    // 이미 공격 사거리 내라면 도달한 것으로 간주
    int attackRange = actor->GetAttackRange();
    if (grid->ManhattanDistance(myPos, tgtPos) <= attackRange)
        return true;

    int mp = actor->GetMovementRange();

    // 드래곤 인접 4방향 중 경로 탐색
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

// 가용 슬롯 총합 데미지로 드래곤 확정 처치 가능 여부
// (SpellSystem 연동 후 정교하게 구현 — 현재는 간소화 버전)
bool FighterStrategy::CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const
{
    // TODO: SpellSystem::CalculateSpellDamage 연동 후 정교한 계산
    // 현재는 HP 20% 이하면 처치 가능으로 간주
    (void)actor; (void)grid;
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
  → Phase_Decision: AP > 0 → adjacent → HP 40% 이하 → MakeSurvivalDecision
  → HasBuff("피의 갈망"): true → HasAnySpellSlot: true
  → return UseAbility("강타")
```

### 생존 시나리오 (HP 35%, 피의 갈망 없음, 슬롯 없음)

```
MakeDecision 호출:
  → MakeSurvivalDecision
  → HasBuff("피의 갈망"): false → HasSpellSlot(2): false
  → IsFearActive: false & HasSpellSlot(1): false
  → return Attack (일반 공격, NOT EndTurn)
```

### 정상 교전 — 축복+격앙 → 강타

```
MakeDecision:
  → MakeNormalCombatDecision
  → hasBless: true → hasBurst: true → CheckSmiteSlot
  → HasAnySpellSlot: true → return UseAbility("강타")
```

### 정상 교전 — 축복 없음, 클레릭 생존, 드래곤 공포 없음

```
MakeDecision:
  → MakeNormalCombatDecision
  → hasBless: false → clericAlive: true
  → !IsFearActive: true → HasSpellSlot(1): true → IsInFearRange: true
  → return UseAbility("공포의 외침")

다음 호출 (IsFearActive: true):
  → clericAlive: true → IsFearActive: true
  → return Attack (일반 공격, 강타 아님)
```

### 정상 교전 — 축복 없음, 클레릭 없음, 격앙 있음

```
MakeDecision:
  → MakeNormalCombatDecision
  → hasBless: false → clericAlive: false → hasBurst: true
  → CheckSmiteSlot (공포 먼저 아님)
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

### 테스트 3: 생존 — 슬롯 없음 시 일반 공격 (EndTurn 아님)

```cpp
// Fighter HP 38%, 피의 갈망 없음, 슬롯 없음, 드래곤 인접
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.38f));
fighter->SetSpellSlots({}); // 슬롯 없음

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::Attack, "Should normal attack when no resources (NOT EndTurn)");
ASSERT(decision.target == dragon, "Attack target should be dragon");
```

### 테스트 4: 정상 교전 — 축복+격앙 → 강타

```cpp
// HP > 40%, 축복 버프 있음, 격앙 버프 있음, 슬롯 있음, 드래곤 인접
// (HasBuff stub이 false이므로 Week 6+ 이후 테스트 가능)

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should smite");
ASSERT(decision.abilityName == "강타", "Should use smite with bless+burst");
ASSERT(decision.target == dragon, "Target should be dragon");
```

### 테스트 5: 정상 교전 — 클레릭 생존 → 공포 후 일반 공격 (강타 아님)

```cpp
// HP > 40%, 축복 없음, 클레릭 생존, 드래곤 공포 없음, 1레벨 슬롯 있음
// 첫 호출: 공포의 외침
AIDecision first = fighter_strategy.MakeDecision(fighter);
ASSERT(first.abilityName == "공포의 외침", "First: fear");

// 두 번째 호출 (공포 적용 후 - IsFearActive stub이 false이므로 현재는 공포 반복)
// Week 6+ StatusEffect 연동 후: 공포 적용됨 → 일반 공격
AIDecision second = fighter_strategy.MakeDecision(fighter);
// TODO: Week 6+에서 검증 — second.type == AIDecisionType::Attack (강타 아님)
```

### 테스트 6: 정상 교전 — 클레릭 없음, 격앙 있음 → 강타 직행 (공포 먼저 아님)

```cpp
// HP > 40%, 축복 없음, 격앙 있음, 클레릭 없음, 슬롯 있음
// HasBuff stub 항상 false이므로 Week 6+ 이후 테스트 가능
// 현재: hasBurst=false → 격앙 시전 시도 (2레벨 슬롯 없으면) → 공포 → 일반
```

### 테스트 7: 이동 불가 시 원거리 공포

```cpp
// Fighter를 이동 불가 위치에 배치 (MP=0)
// 드래곤과 공포 사거리(3칸) 내, 1레벨 슬롯 있음
fighter->SetActionPoints(1); // AP 있음 (인접이 아니어도 Fear 쏘려면)
// SetMovementRange(0) 방법 필요

AIDecision decision = fighter_strategy.MakeDecision(fighter);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should cast fear from range");
ASSERT(decision.abilityName == "공포의 외침", "Should use fear when can't move");
```

### 테스트 8: HP 임계값 경계 (40% 정확히)

```cpp
// HP 정확히 40% → IsInDanger: <= 0.4f → true → Survival sequence
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.40f));
AIDecision survival = fighter_strategy.MakeDecision(fighter);
// MakeSurvivalDecision 진입 확인

// HP 41% → IsInDanger: false → Normal combat
fighter->SetHP(static_cast<int>(fighter->GetMaxHP() * 0.41f));
AIDecision normal = fighter_strategy.MakeDecision(fighter);
// MakeNormalCombatDecision 진입 확인
```

### 테스트 9: CanReachThisTurn — 이미 인접 시 true

```cpp
// Fighter와 Dragon을 인접 배치 (distance == 1)
// CanReachThisTurn이 true여야 함 (MP 관계없이)
// → Kill loop 진입 가능
```

---

## GamePlay / BattleOrchestrator 연동 주의사항

### BattleOrchestrator 동작 방식

`BattleOrchestrator::HandleAITurn`은 매 프레임:
1. `MovementComponent::IsMoving()` 체크 → 이동 중이면 대기
2. 0.4초 busy-wait
3. `ai_system->MakeDecision(actor)` → 단 하나의 행동 결정
4. `EndTurn` → `turn_manager->EndCurrentTurn()` / 그 외 → `ai_system->ExecuteDecision()` → return

**fighter_strategy.md의 "Phase_Decision 루프"는 BattleOrchestrator의 반복 호출로 구현됩니다.**
`MakeDecision`은 항상 단일 행동 하나를 반환해야 합니다 (재귀/내부 루프 금지).

---

### ⚠️ 필수 전제 조건: SpellSystem 등록

`AISystem::ExecuteDecision`의 `UseAbility` 처리:
```cpp
case AIDecisionType::UseAbility:
  if (spell_system)  // GetGSComponent<SpellSystem>()
    spell_system->CastSpell(actor, decision.abilityName, decision.target->GetGridPosition()->Get());
  break;
```

**현재 `GamePlay::Load()`에 `SpellSystem`이 등록되지 않아** `spell_system == nullptr`.
`UseAbility` 결정이 반환되면 아무 일도 안 일어나고 AP/슬롯 소모도 없어 **무한 루프** 발생.

**수정 필요** (`GamePlay::Load()` 내):
```cpp
AddGSComponent(new SpellSystem());
GetGSComponent<SpellSystem>()->SetEventBus(GetGSComponent<EventBus>());
```

`SpellSystem`이 등록될 때까지 `UseAbility` 계열 어빌리티(강타, 피의 갈망, 공포의 외침, 격앙)는 **실행되지 않습니다**.
현재 동작 가능한 결정: `Attack`(일반 공격), `Move`(이동), `EndTurn`(턴 종료).

---

### UseAbility 자기 자신 타겟 (피의 갈망, 격앙)

`AISystem::ExecuteDecision`은 `decision.target->GetGridPosition()->Get()`을 사용합니다.
자신이 타겟인 어빌리티(피의 갈망, 격앙)는 `target = actor`로 설정하면 크래시 없이 처리됩니다.
```cpp
return { AIDecisionType::UseAbility, actor, {}, "피의 갈망", "..." };  // target = actor (self)
```
