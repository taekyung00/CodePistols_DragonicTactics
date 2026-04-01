# RogueStrategy 구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: RogueStrategy 신규 구현 — `rouge.mmd` 플로우차트 완전 반영
**작성일**: 2026-03-08

**관련 파일**:
- [RogueStrategy.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/RogueStrategy.h) ← 신규 생성
- [IAIStrategy.h](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/IAIStrategy.h)
- [AISystem.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AISystem.h)
- [architecture/character_flowchart/rouge.mmd](../../../architecture/character_flowchart/rouge.mmd)

---

## 개요

`rouge.mmd` 플로우차트를 기반으로 RogueStrategy를 새로 구현합니다.

### rouge.mmd 플로우차트 요약

```
로그 턴 시작
  ↓ 상태 변수 초기화: [공격 여부 = False]
  ↓
[0. 킬캐치] 은신 2배 & A* 경로로 드래곤 처치 가능?
  Yes → Kill_Loop (약점 공략/일반 반복)
         K_CanMove→No → Phase_Decision (이동력 소진 시 일반 판단 전환)
         K_AP→No      → Phase_Decision (공격 소진 시 은신 여부 확인)
  No  →
[1. 버프 페이즈]
  신속 없음 & 1레벨 슬롯 있음 & 유의미함? → 질풍의 보법 → Phase_Decision
  그 외 → Phase_Decision
[2. 판단 지점 루프]
  AP > 0?
    No → 턴 종료
    Yes → 인접했는가?
      No  → MP > 0 & 경로? → 이동 → 판단 지점
             이동 불가 & [공격 여부=False] & 비은신 → 그림자 숨기 → 종료
             그 외 → 종료
      Yes → 은신 상태?
              Yes → 2레벨 슬롯? → 약점 공략(2배) → 판단 지점
                    없음 → 평타 은신 깰 가치? → 일반 공격(2배) → 판단 지점
                           가치 없음 → MP > 0? → 재포지셔닝 이동 → (반복)
                                       MP = 0 → 종료 (은신 유지)
              No  → 이미 공격했는가?
                    No  → 그림자 숨기 → 종료 (은신 유지)
                    Yes → 일반 공격 → 판단 지점
```

### 핵심 어빌리티

| 어빌리티 | `abilityName` | 슬롯 | 효과 |
|---------|--------------|------|------|
| 약점 공략 (Weak Point) | `"약점 공략"` | 2레벨 | 공격력 2배 (은신 중) |
| 질풍의 보법 (Haste) | `"질풍의 보법"` | 1레벨 | 버프: AP+1, MP+1 |
| 그림자 숨기 (Shadow Hide) | `"그림자 숨기"` | 없음 (슬롯 불필요) | 버프: 은신 상태 진입 |

### 상태 변수

`[공격 여부]` = `actor->HasAttackedThisTurn()` — 턴 내 공격 수행 여부 (Character 레벨 팩트 쿼리)

**⚠️ 현재 미구현**: `HasBuff`, `IsInStealth`, `HasAttackedThisTurn` 모두 Week 6+ StatusEffect 연동 전까지 **stub (false 반환)**.

---

## Implementation Tasks

### Task 1: RogueStrategy.h 생성

```cpp
/**
 * @file RogueStrategy.h
 * @brief 로그 AI 전략 — rouge.mmd 플로우차트 완전 반영
 *        은신/기습 전문, 신속 버프, 약점 공략 활용
 */
#pragma once
#include "IAIStrategy.h"

class GridSystem;

class RogueStrategy : public IAIStrategy
{
public:
    AIDecision MakeDecision(Character* actor) override;

private:
    // --- 타겟 탐색 ---
    Character* FindDragon();

    // --- 판단 헬퍼 ---
    bool CanKillWithStealth(Character* actor, Character* dragon, GridSystem* grid) const;
    bool IsInStealth(Character* actor) const;               // TODO: Week 6+ StatusEffect 연동
    bool HasBuff(Character* actor, const std::string& buffName) const; // TODO: Week 6+ StatusEffect 연동
    bool HasAttackedThisTurn(Character* actor) const;       // TODO: Week 6+ StatusEffect 연동
    bool HasSpellSlot(Character* actor, int level) const;
    bool ShouldBreakStealth(Character* actor, Character* dragon) const;
    bool IsHasteMeaningful(Character* actor) const;

    // --- 이동 ---
    Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
    Math::ivec2 FindRepositionPos(Character* actor, Character* dragon, GridSystem* grid);

    // --- 서브 로직 ---
    AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeBuffPhaseDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeCombatDecision(Character* actor, Character* dragon, GridSystem* grid);
};
```

---

### Task 2: MakeDecision() — 최상위 흐름

```cpp
AIDecision RogueStrategy::MakeDecision(Character* actor)
{
    GridSystem* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    Character*  dragon = FindDragon();

    if (!dragon)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "No dragon found" };

    // ============================================================
    // 0. 킬캐치 오버라이드
    // 은신 2배 데미지 & A* 경로로 이번 턴 내 드래곤 처치 가능 시
    // ============================================================
    if (CanKillWithStealth(actor, dragon, grid))
        return MakeKillLoopDecision(actor, dragon, grid);

    // ============================================================
    // 1. 버프 페이즈 — 신속 시전 여부 결정
    // ============================================================
    AIDecision buffDecision = MakeBuffPhaseDecision(actor, dragon, grid);
    if (buffDecision.type != AIDecisionType::None)
        return buffDecision;

    // ============================================================
    // 2. 판단 지점
    // ============================================================
    if (actor->GetActionPoints() <= 0)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "No AP" };

    int  dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
        return MakeFarMoveDecision(actor, dragon, grid);
    else
        return MakeCombatDecision(actor, dragon, grid);
}
```

---

### Task 3: Kill_Loop — 확정 처치 루프

**⚠️ 수정 포인트**: `K_CanMove→No`는 `EndTurn`이 아닌 **`MakeFarMoveDecision` 호출** (mmd: Phase_Decision으로 합류)
- 이동력 소진 시 Phase_Decision에서 그림자 숨기 가능성 확인
- `K_AP→No`는 어차피 Phase_Decision→CheckAP→No→EndTurn이므로 EndTurn 반환 가능

```cpp
AIDecision RogueStrategy::MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    int  dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
    {
        // K_CanMove -- No --> Phase_Decision (mmd 명시)
        // MP 소진 시 이동 불가 → 그림자 숨기 가능성 확인을 위해 Phase_Decision 진입
        if (actor->GetMovementRange() <= 0)
            return MakeFarMoveDecision(actor, dragon, grid);

        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        return { AIDecisionType::Move, nullptr, movePos, "", "Kill loop: moving to dragon" };
    }

    // 인접 — AP 체크
    // K_AP -- No --> Phase_Decision (mmd 명시)
    // AP=0이면 Phase_Decision→CheckAP→No→EndTurn과 동일
    if (actor->GetActionPoints() <= 0)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no AP" };

    // 2레벨 슬롯 → 약점 공략, 없으면 일반 공격
    if (HasSpellSlot(actor, 2))
        return { AIDecisionType::UseAbility, dragon, {}, "약점 공략", "Kill loop: weak point (2x dmg)" };
    else
        return { AIDecisionType::Attack, dragon, {}, "", "Kill loop: normal attack" };
}
```

---

### Task 4: 버프 페이즈 — 질풍의 보법

```cpp
AIDecision RogueStrategy::MakeBuffPhaseDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    (void)dragon; (void)grid;

    // 이미 신속 상태 → 패스
    if (HasBuff(actor, "신속"))
        return { AIDecisionType::None, nullptr, {}, "", "" };

    // 1레벨 슬롯 없음 → 패스
    if (!HasSpellSlot(actor, 1))
        return { AIDecisionType::None, nullptr, {}, "", "" };

    // 신속이 유의미한가? (MP 부족 or AP 활용 가능)
    if (!IsHasteMeaningful(actor))
        return { AIDecisionType::None, nullptr, {}, "", "" };

    // 질풍의 보법 시전 (AP-1, 효과: AP+1, MP+1)
    return { AIDecisionType::UseAbility, actor, {}, "질풍의 보법", "Buff phase: cast haste" };
}
```

---

### Task 5: 이동 / 원거리 그림자 숨기 (MakeFarMoveDecision)

```cpp
AIDecision RogueStrategy::MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // 이동 가능? (MP > 0 & A* 경로 있음)
    if (actor->GetMovementRange() > 0)
    {
        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        if (movePos != actor->GetGridPosition()->Get())
            return { AIDecisionType::Move, nullptr, movePos, "", "Moving towards dragon" };
    }

    // 이동 불가 → CheckAttackedFar: [공격 여부] == False & 비은신 → 그림자 숨기
    if (!HasAttackedThisTurn(actor) && !IsInStealth(actor))
        return { AIDecisionType::UseAbility, actor, {}, "그림자 숨기", "Can't move: enter stealth" };

    return { AIDecisionType::EndTurn, nullptr, {}, "", "Can't move or stealth" };
}
```

---

### Task 6: 교전 로직 (MakeCombatDecision)

**⚠️ mmd 세부사항**:
- 비은신 + 미공격 → 그림자 숨기 후 `End_Stealth` (종료, Phase_Decision 미반환)
- 은신 + 가치 없음 → 재포지셔닝 이동 (`Repo_Move` 루프 — 1행동 반환 후 BattleOrchestrator 반복)

```cpp
AIDecision RogueStrategy::MakeCombatDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    bool inStealth   = IsInStealth(actor);
    bool hasAttacked = HasAttackedThisTurn(actor);

    // ── 은신 중 ──
    if (inStealth)
    {
        // 2레벨 슬롯 → 약점 공략 (2배, 은신 해제)
        if (HasSpellSlot(actor, 2))
            return { AIDecisionType::UseAbility, dragon, {}, "약점 공략",
                     "Combat: weak point (stealth 2x)" };

        // 슬롯 없음 → 평타 은신 깰 가치?
        if (ShouldBreakStealth(actor, dragon))
            return { AIDecisionType::Attack, dragon, {}, "",
                     "Combat: normal (stealth 2x, break stealth)" };

        // 가치 없음 → Repo_Move 루프 (1행동 반환, BattleOrchestrator 반복)
        if (actor->GetMovementRange() > 0)
        {
            Math::ivec2 repoPos = FindRepositionPos(actor, dragon, grid);
            return { AIDecisionType::Move, nullptr, repoPos, "",
                     "Combat: reposition (maintain stealth)" };
        }

        // MP = 0 → 은신 유지 종료
        return { AIDecisionType::EndTurn, nullptr, {}, "", "Combat: end turn (stealth, no MP)" };
    }

    // ── 비은신 ──
    // 미공격 → 그림자 숨기 (End_Stealth: 은신 유지 종료)
    if (!hasAttacked)
        return { AIDecisionType::UseAbility, actor, {}, "그림자 숨기",
                 "Combat: enter stealth (not attacked yet)" };

    // 이미 공격 → 일반 공격 → Phase_Decision 계속
    return { AIDecisionType::Attack, dragon, {}, "", "Combat: normal attack (already attacked)" };
}
```

---

### Task 7: 헬퍼 함수 구현

```cpp
Character* RogueStrategy::FindDragon()
{
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    for (auto* c : grid->GetAllCharacters())
    {
        if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Dragon)
            return c;
    }
    return nullptr;
}

// 은신 2배 데미지로 이번 턴 드래곤 처치 가능 여부
bool RogueStrategy::CanKillWithStealth(Character* actor, Character* dragon, GridSystem* grid) const
{
    // 은신 중이어야 2배 적용 (stub: 항상 false → kill loop 미진입)
    if (!IsInStealth(actor)) return false;

    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();

    // 이미 공격 사거리 내 (인접)
    if (grid->ManhattanDistance(myPos, tgtPos) <= actor->GetAttackRange())
        return dragon->GetHPPercentage() <= 0.25f;

    // A* 경로로 이번 턴 내 도달 가능?
    int mp = actor->GetMovementRange();
    static const Math::ivec2 offsets[4] = { {0,1},{0,-1},{-1,0},{1,0} };
    for (const auto& off : offsets)
    {
        Math::ivec2 attackPos = tgtPos + off;
        if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos)) continue;
        auto path = grid->FindPath(myPos, attackPos);
        if (!path.empty() && static_cast<int>(path.size()) <= mp)
        {
            // TODO: SpellSystem 연동 후 정교한 2배 데미지 계산으로 교체
            // 현재: 드래곤 HP 25% 이하면 처치 가능으로 간주
            return dragon->GetHPPercentage() <= 0.25f;
        }
    }
    return false;
}

// 은신 상태 여부 — StatusEffect 구현 전까지 stub (항상 false)
bool RogueStrategy::IsInStealth(Character* actor) const
{
    // TODO: Week 6+ actor->HasBuff("은신") 연동
    (void)actor;
    return false;
}

// 버프 확인 — StatusEffect 구현 전까지 stub (항상 false)
bool RogueStrategy::HasBuff(Character* actor, const std::string& buffName) const
{
    // TODO: Week 6+ actor->HasBuff(buffName) 연동
    (void)actor; (void)buffName;
    return false;
}

// 공격 여부 확인 — StatusEffect 구현 전까지 stub (항상 false)
bool RogueStrategy::HasAttackedThisTurn(Character* actor) const
{
    // TODO: Week 6+ actor->HasAttackedThisTurn() 연동
    (void)actor;
    return false;
}

// 특정 레벨 주문 슬롯 보유 여부
bool RogueStrategy::HasSpellSlot(Character* actor, int level) const
{
    return actor->HasSpellSlot(level); // Character::HasSpellSlot(int) 직접 사용
}

// 평타로 은신 깰 가치 판단
// 드래곤 HP 30% 이하면 평타 2배로 의미 있음
bool RogueStrategy::ShouldBreakStealth(Character* actor, Character* dragon) const
{
    (void)actor;
    return dragon->GetHPPercentage() <= 0.3f;
}

// 신속 시전이 유의미한가 (MP 부족 or AP 활용 여분 있음)
bool RogueStrategy::IsHasteMeaningful(Character* actor) const
{
    bool mpLacking = (actor->GetMovementRange() <= 0);
    bool apUsable  = (actor->GetActionPoints() >= 2); // AP+1 후에도 추가 행동 가능
    return mpLacking || apUsable;
}

// 재포지셔닝 위치 — 드래곤 반대 방향 1칸
Math::ivec2 RogueStrategy::FindRepositionPos(Character* actor, Character* dragon, GridSystem* grid)
{
    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();

    Math::ivec2 dir = { myPos.x - tgtPos.x, myPos.y - tgtPos.y };
    Math::ivec2 candidate = {
        myPos.x + (dir.x != 0 ? (dir.x > 0 ? 1 : -1) : 0),
        myPos.y + (dir.y != 0 ? (dir.y > 0 ? 1 : -1) : 0)
    };

    if (grid->IsValidTile(candidate) && grid->IsWalkable(candidate))
        return candidate;
    return myPos; // 안전한 위치 없으면 제자리
}

Math::ivec2 RogueStrategy::FindNextMovePos(Character* actor, Character* target, GridSystem* grid)
{
    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = target->GetGridPosition()->Get();

    static const Math::ivec2 offsets[4] = { {0,1},{0,-1},{-1,0},{1,0} };
    std::vector<Math::ivec2> bestPath;
    int bestCost = 999999;

    for (const auto& off : offsets)
    {
        Math::ivec2 attackPos = tgtPos + off;
        if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos)) continue;
        auto path = grid->FindPath(myPos, attackPos);
        if (!path.empty() && static_cast<int>(path.size()) < bestCost)
        {
            bestCost = static_cast<int>(path.size());
            bestPath = path;
        }
    }

    if (!bestPath.empty())
    {
        int maxReach  = std::min(static_cast<int>(bestPath.size()), actor->GetMovementRange());
        int destIndex = maxReach - 1;
        if (destIndex >= 0)
            return bestPath[static_cast<std::size_t>(destIndex)];
    }
    return myPos;
}
```

---

### Task 8: AISystem에 RogueStrategy 등록

**파일**: `AISystem.cpp` — `#include` 추가 및 `Init()` 수정

```cpp
#include "AI/FighterStrategy.h"
#include "AI/RogueStrategy.h"   // 신규

void AISystem::Init()
{
    m_strategies[CharacterTypes::Fighter] = new FighterStrategy();
    m_strategies[CharacterTypes::Rogue]   = new RogueStrategy();  // 신규
    // m_strategies[CharacterTypes::Cleric] = new ClericStrategy();
    // m_strategies[CharacterTypes::Wizard] = new WizardStrategy();
}
```

### Task 9: CharacterFactory에 Rogue 생성 추가

**파일**: `CharacterFactory.cpp`

```cpp
std::unique_ptr<Character> CharacterFactory::Create(CharacterTypes type, Math::ivec2 pos)
{
    switch (type)
    {
        case CharacterTypes::Dragon:  return CreateDragon(pos);
        case CharacterTypes::Fighter: return CreateFighter(pos);
        case CharacterTypes::Rogue:   return CreateRogue(pos);   // 신규
        default:
            Engine::GetLogger().LogError("CharacterFactory: Unknown type");
            return nullptr;
    }
}
```

---

## Implementation Examples

### 시나리오 1: 비은신 → 그림자 숨기 → 약점 공략

```
[1턴] Rogue 드래곤 인접, 비은신, 미공격
  → CanKillWithStealth: IsInStealth=false → false (stub)
  → BuffPhase: HasBuff("신속")=false, 슬롯 있음, IsHasteMeaningful 체크
  → Phase_Decision: adjacent → MakeCombatDecision
  → !inStealth, !hasAttacked → UseAbility("그림자 숨기") → End_Stealth

[2턴] (은신 진입 후 — Week 6+ StatusEffect 연동 시)
  → CanKillWithStealth: IsInStealth=true, dragon HP ≤ 25%? → (조건에 따라)
  → Phase_Decision: adjacent → MakeCombatDecision
  → inStealth, HasSpellSlot(2) → UseAbility("약점 공략") [2배, 은신 해제]
  → Phase_Decision 재진입: !inStealth, hasAttacked → Attack (일반)
```

### 시나리오 2: 신속 버프 → 이동 → 전투

```
[1턴] Rogue 비은신, 드래곤 거리 4칸, MP=0, 1레벨 슬롯 있음
  → BuffPhase: !HasBuff("신속"), HasSpellSlot(1), IsHasteMeaningful(MP=0)=true
  → UseAbility("질풍의 보법") [효과: AP+1, MP+1]

  Phase_Decision (다음 step): AP > 0, 미인접 → MakeFarMoveDecision → Move (MP=1 소모)
```

### 시나리오 3: 킬캐치 — 이동력 소진 시 그림자 숨기 대안

```
[조건] IsInStealth=true(Week 6+), dragon HP ≤ 25%, 거리 3칸, MP=1
  → CanKillWithStealth=true → MakeKillLoopDecision
  → !adjacent, MP=1 → Move (A*)

[다음 step] 거리 2칸, MP=0 → MakeKillLoopDecision
  → !adjacent, MP=0 → MakeFarMoveDecision
  → !HasAttackedThisTurn, !IsInStealth... 실제로는 은신 중이므로 스킵
  → EndTurn (이 경우 은신 유지로 종료)
```

---

## Rigorous Testing

### 테스트 1: 비은신 미공격 시 그림자 숨기 우선

```cpp
// Rogue 드래곤 인접, 비은신(stub=false), 미공격(stub=false)

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should use shadow hide");
ASSERT(decision.abilityName == "그림자 숨기", "Enter stealth when adjacent, not attacked");
ASSERT(decision.target == rogue, "Shadow hide targets self");
```

### 테스트 2: 은신 + 2레벨 슬롯 → 약점 공략 (Week 6+ 이후)

```cpp
// Week 6+ StatusEffect 연동 후 테스트
// Rogue 은신 상태(IsInStealth=true), 드래곤 인접, 2레벨 슬롯 보유

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should use weak point in stealth");
ASSERT(decision.abilityName == "약점 공략", "Weak point with 2nd level slot");
ASSERT(decision.target == dragon, "Target is dragon");
```

### 테스트 3: 신속 조건 만족 시 버프 시전

```cpp
// Rogue: 신속 없음(HasBuff stub=false), 1레벨 슬롯 있음
// IsHasteMeaningful: MP=0 → true

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should cast haste");
ASSERT(decision.abilityName == "질풍의 보법", "Cast haste when MP lacking");
ASSERT(decision.target == rogue, "Haste targets self");
```

### 테스트 4: 이동 불가 & 비은신 & 미공격 → 그림자 숨기

```cpp
// Rogue: MP=0, 드래곤 거리 3칸(미인접), 비은신, 미공격
// FindNextMovePos가 제자리 반환하도록 설정

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should stealth when can't move");
ASSERT(decision.abilityName == "그림자 숨기", "Shadow hide when movement blocked");
```

### 테스트 5: Kill Loop — MP 소진 시 EndTurn 아닌 MakeFarMoveDecision 진입

```cpp
// Week 6+: IsInStealth=true, dragon HP ≤ 25%, 거리 2칸, MP=0, 미인접
// CanKillWithStealth → MakeKillLoopDecision → !adjacent, MP=0
// → MakeFarMoveDecision 호출 (EndTurn 직반환 X)

AIDecision decision = rogue_strategy.MakeDecision(rogue);
// MakeFarMoveDecision: !HasAttackedThisTurn, IsInStealth=true → skip 그림자 숨기
// → EndTurn (은신 유지)
ASSERT(decision.type == AIDecisionType::EndTurn, "End turn maintaining stealth");
```

### 테스트 6: 은신 + 슬롯 없음 + 드래곤 HP 낮음 → 은신 깨고 일반 공격

```cpp
// Week 6+: 은신(IsInStealth=true), 슬롯 없음, dragon HP 25%
// ShouldBreakStealth: 0.25 ≤ 0.3 → true

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::Attack, "Break stealth with normal attack");
ASSERT(decision.target == dragon, "Attack dragon");
```

### 테스트 7: 이미 공격 후 비은신 → 일반 공격 반복

```cpp
// Week 6+: 비은신, 이미 공격(HasAttackedThisTurn=true), 드래곤 인접, AP 남아 있음

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::Attack, "Normal attack after first attack");
ASSERT(decision.target == dragon, "Target is dragon");
```

### 테스트 8: 신속 이미 있음 → 버프 페이즈 스킵

```cpp
// Week 6+: HasBuff("신속")=true

// BuffPhase → None 반환 → Phase_Decision으로 진행
// (결정은 드래곤 위치/상태에 따라)
AIDecision buff = rogue_strategy.MakeBuffPhaseDecision(rogue, dragon, grid);
ASSERT(buff.type == AIDecisionType::None, "Skip haste when already buffed");
```

---

## GamePlay / BattleOrchestrator 연동 주의사항

### BattleOrchestrator 동작 방식 (fighter_strategy.md 동일)

`BattleOrchestrator::HandleAITurn`은 매 프레임:
1. `MovementComponent::IsMoving()` → 이동 중이면 대기
2. 0.4초 busy-wait
3. `ai_system->MakeDecision(actor)` → 단 하나의 행동 결정
4. `EndTurn` → `EndCurrentTurn()` / 그 외 → `ExecuteDecision()` → return

**rouge.mmd의 `Repo_Move` 루프**는 BattleOrchestrator 반복 호출로 구현됩니다. 1번 호출 = 1칸 이동.

---

### ⚠️ 필수 전제 조건: SpellSystem 등록

`AISystem::ExecuteDecision`의 `UseAbility`:
```cpp
case AIDecisionType::UseAbility:
  if (spell_system)  // nullptr이면 아무것도 안 함, AP 소모 없음
    spell_system->CastSpell(actor, decision.abilityName, decision.target->GetGridPosition()->Get());
  break;
```

**현재 `GamePlay::Load()`에 `SpellSystem` 미등록 → UseAbility 결정 시 AP/슬롯 미소모 → 무한 루프.**

영향받는 Rogue 행동: `약점 공략`, `질풍의 보법`, **`그림자 숨기`** (슬롯 불필요지만 UseAbility로 실행)

특히 `그림자 숨기`는 초반부터 자주 호출됨. SpellSystem 미등록 시 비은신+미공격 상태에서 무한 루프.

**수정 필요** (`GamePlay::Load()` 내):
```cpp
AddGSComponent(new SpellSystem());
GetGSComponent<SpellSystem>()->SetEventBus(GetGSComponent<EventBus>());
```

현재 동작 가능한 Rogue 결정: `Attack`(일반 공격), `Move`(이동), `EndTurn`(턴 종료).

---

### stub 현황 및 Week 6+ 이후 해제 목록

| stub 메서드 | 현재 반환값 | 실제 연동 방법 |
|---|---|---|
| `IsInStealth(actor)` | `false` | `actor->HasBuff("은신")` |
| `HasBuff(actor, name)` | `false` | `actor->HasBuff(name)` |
| `HasAttackedThisTurn(actor)` | `false` | `actor->HasAttackedThisTurn()` |

**stub 상태의 실제 동작 흐름** (인접 상황):
- `inStealth=false`, `hasAttacked=false` → 항상 `그림자 숨기` 시도
- SpellSystem 미등록 시 → 아무것도 안 일어남 → **무한 루프**
- SpellSystem 등록 후 → 그림자 숨기 성공 → 다음 turn에도 `inStealth=false` (stub) → 다시 그림자 숨기

→ **Rogue의 전전 완전 동작은 Week 6+ StatusEffect 시스템 + SpellSystem 등록 후 가능**

### UseAbility 자기 자신 타겟 처리

자신이 타겟인 어빌리티(그림자 숨기, 질풍의 보법)는 `target = actor`로 설정:
```cpp
return { AIDecisionType::UseAbility, actor, {}, "그림자 숨기", "..." };
// AISystem::ExecuteDecision: decision.target->GetGridPosition()->Get() → actor의 위치 ✓
```
