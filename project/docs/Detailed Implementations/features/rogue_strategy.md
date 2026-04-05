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
[0. 킬캐치] 은신 2배 & A* 경로로 드래곤 처치 가능? → Kill_Loop (약점 공략/일반 반복)
  ↓ No
[1. 버프 페이즈] 신속 없음 & 1레벨 슬롯 있음 & 유의미함? → 질풍의 보법 시전
  ↓
[판단 지점 루프]
  AP > 0?
    No → 턴 종료
    Yes → 인접했는가?
      No  → MP > 0 & 경로? → 이동 → 판단 지점
             이동 불가 & 미공격 & 비은신 → 그림자 숨기 → 종료
      Yes → 은신 상태?
              Yes → 2레벨 슬롯? → 약점 공략(2배) → 판단 지점
                    없음 → 평타 은신 깰 가치? → 일반 공격(2배) → 판단 지점
                           가치 없음 → 재포지셔닝 이동 → 종료
              No  → 이미 공격? → 일반 공격 → 판단 지점
                    미공격 → 그림자 숨기 → 종료
```

### 핵심 어빌리티

| 어빌리티 | `abilityName` | 슬롯 | 효과 |
|---------|--------------|------|------|
| 약점 공략 (Weak Point) | `"약점 공략"` | 2레벨 | 공격력 2배 (은신 중) |
| 질풍의 보법 (Haste) | `"질풍의 보법"` | 1레벨 | 버프: AP+1, MP+1 |
| 그림자 숨기 (Shadow Hide) | `"그림자 숨기"` | 0 (슬롯 불필요) | 버프: 은신 상태 진입 |

### 상태 변수

`MakeDecision()`이 매 호출마다 **단 하나의 행동**을 결정하고 반환합니다.
`[공격 여부]`는 Character 레벨의 턴 상태 쿼리로 확인합니다: `actor->HasAttackedThisTurn()`

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
    bool IsInStealth(Character* actor) const;
    bool HasBuff(Character* actor, const std::string& buffName) const;
    bool HasSpellSlot(Character* actor, int level) const;
    bool ShouldBreakStealth(Character* actor, Character* dragon) const; // 평타 은신 깰 가치 판단
    bool IsHasteMeaningful(Character* actor) const;                     // 신속 효과 유의미한가

    // --- 이동 ---
    Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
    Math::ivec2 FindRepositionPos(Character* actor, Character* dragon, GridSystem* grid);

    // --- 서브 로직 ---
    AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeBuffPhaseDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeCombatDecision(Character* actor, Character* dragon, GridSystem* grid);
    AIDecision MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid);
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
    // 은신 중 2배 데미지 & A* 경로로 이번 턴 내 드래곤 처치 가능 시
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
    // 2. 판단 지점 루프
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

```cpp
AIDecision RogueStrategy::MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    int  dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(), dragon->GetGridPosition()->Get());
    bool adjacent = (dist <= 1);

    if (!adjacent)
    {
        // 이동
        if (actor->GetMovementRange() <= 0)
        {
            // MP 소진 → 이동 실패 → Phase_Decision으로 전환 (재귀 방지: 행동 반환)
            return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no MP" };
        }
        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        return { AIDecisionType::Move, nullptr, movePos, "", "Kill loop: moving to dragon" };
    }

    // 인접 — AP 체크
    if (actor->GetActionPoints() <= 0)
        return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no AP" };

    // 2레벨 슬롯 있으면 약점 공략 (2배)
    if (HasSpellSlot(actor, 2))
        return { AIDecisionType::UseAbility, dragon, {}, "약점 공략", "Kill loop: weak point (2x)" };
    else
        return { AIDecisionType::Attack, dragon, {}, "", "Kill loop: normal attack" };
}
```

---

### Task 4: 버프 페이즈 — 질풍의 보법

```cpp
AIDecision RogueStrategy::MakeBuffPhaseDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // 이미 신속 상태면 패스
    if (HasBuff(actor, "신속"))
        return { AIDecisionType::None, nullptr, {}, "", "" };

    // 1레벨 슬롯 없으면 패스
    if (!HasSpellSlot(actor, 1))
        return { AIDecisionType::None, nullptr, {}, "", "" };

    // 신속이 유의미한가? (MP 부족 or AP 활용 가능)
    if (!IsHasteMeaningful(actor))
        return { AIDecisionType::None, nullptr, {}, "", "" };

    // 질풍의 보법 시전 (AP-1, AP+1, MP+1)
    return { AIDecisionType::UseAbility, actor, {}, "질풍의 보법", "Buff phase: cast haste" };
}
```

---

### Task 5: 이동 불가 처리 (MakeFarMoveDecision)

```cpp
AIDecision RogueStrategy::MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    // 이동 가능?
    if (actor->GetMovementRange() > 0)
    {
        Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
        if (movePos != actor->GetGridPosition()->Get())
            return { AIDecisionType::Move, nullptr, movePos, "", "Moving towards dragon" };
    }

    // 이동 불가 — 미공격 & 비은신 → 그림자 숨기
    if (!actor->HasAttackedThisTurn() && !IsInStealth(actor))
        return { AIDecisionType::UseAbility, actor, {}, "그림자 숨기", "Can't move: enter stealth" };

    return { AIDecisionType::EndTurn, nullptr, {}, "", "Can't move or stealth" };
}
```

---

### Task 6: 교전 로직 (MakeCombatDecision)

```cpp
AIDecision RogueStrategy::MakeCombatDecision(Character* actor, Character* dragon, GridSystem* grid)
{
    bool inStealth   = IsInStealth(actor);
    bool hasAttacked = actor->HasAttackedThisTurn();

    // ── 은신 중 ──
    if (inStealth)
    {
        // 2레벨 슬롯 → 약점 공략 (2배)
        if (HasSpellSlot(actor, 2))
            return { AIDecisionType::UseAbility, dragon, {}, "약점 공략",
                     "Combat: weak point (stealth 2x)" };

        // 슬롯 없음 → 평타 은신 깰 가치?
        if (ShouldBreakStealth(actor, dragon))
            return { AIDecisionType::Attack, dragon, {}, "", "Combat: normal (stealth 2x, break)" };

        // 가치 없음 → 재포지셔닝
        if (actor->GetMovementRange() > 0)
        {
            Math::ivec2 repoPos = FindRepositionPos(actor, dragon, grid);
            return { AIDecisionType::Move, nullptr, repoPos, "", "Combat: reposition (stealth)" };
        }
        return { AIDecisionType::EndTurn, nullptr, {}, "", "Combat: stealth end (no reposition)" };
    }

    // ── 비은신 ──
    // 아직 공격 안 했으면 → 그림자 숨기
    if (!hasAttacked)
        return { AIDecisionType::UseAbility, actor, {}, "그림자 숨기", "Combat: enter stealth (not attacked)" };

    // 이미 공격 → 일반 공격
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
    if (!IsInStealth(actor)) return false; // 은신 중이어야 2배 적용

    // A* 경로로 이번 턴 내 도달 가능?
    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();
    static const Math::ivec2 offsets[4] = { {0,1},{0,-1},{-1,0},{1,0} };
    int mp = actor->GetMovementRange();

    bool reachable = false;
    for (const auto& off : offsets)
    {
        Math::ivec2 attackPos = tgtPos + off;
        if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos)) continue;
        auto path = grid->FindPath(myPos, attackPos);
        if (!path.empty() && static_cast<int>(path.size()) <= mp) { reachable = true; break; }
    }
    // 이미 인접한 경우도 reachable
    int dist = grid->ManhattanDistance(myPos, tgtPos);
    if (dist <= 1) reachable = true;

    if (!reachable) return false;

    // TODO: SpellSystem 연동 후 2배 데미지 계산으로 교체
    // 현재는 드래곤 HP 25% 이하면 처치 가능으로 간주
    return dragon->GetHPPercentage() <= 0.25f;
}

bool RogueStrategy::IsInStealth(Character* actor) const
{
    return actor->HasBuff("은신"); // StatusEffect 시스템 구현 후 정확한 쿼리로 교체
}

bool RogueStrategy::HasBuff(Character* actor, const std::string& buffName) const
{
    return actor->HasBuff(buffName);
}

bool RogueStrategy::HasSpellSlot(Character* actor, int level) const
{
    return actor->GetSpellSlots()->HasSlot(level);
}

// 평타로 은신 깰 가치 판단
// 드래곤 HP가 낮아 평타 2배로도 처치 가능하거나, 슬롯이 없어 선택지가 없을 때
bool RogueStrategy::ShouldBreakStealth(Character* actor, Character* dragon) const
{
    // 드래곤 HP 30% 이하면 평타 2배로 의미 있음
    return dragon->GetHPPercentage() <= 0.3f;
}

// 신속 시전이 유의미한가
// MP가 0이거나 AP >= 2일 때 (추가 AP를 활용할 수 있을 때)
bool RogueStrategy::IsHasteMeaningful(Character* actor) const
{
    bool mpLacking = (actor->GetMovementRange() <= 0);
    bool apUsable  = (actor->GetActionPoints() >= 2); // AP+1 후에도 행동 가능
    return mpLacking || apUsable;
}

// 재포지셔닝 위치 계산 — 드래곤과 거리를 벌리는 방향으로 이동
Math::ivec2 RogueStrategy::FindRepositionPos(Character* actor, Character* dragon, GridSystem* grid)
{
    Math::ivec2 myPos  = actor->GetGridPosition()->Get();
    Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();

    // 드래곤 반대 방향으로 1칸 이동
    Math::ivec2 dir = { myPos.x - tgtPos.x, myPos.y - tgtPos.y };
    Math::ivec2 candidate = { myPos.x + (dir.x != 0 ? (dir.x > 0 ? 1 : -1) : 0),
                               myPos.y + (dir.y != 0 ? (dir.y > 0 ? 1 : -1) : 0) };

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

**파일**: `AISystem.cpp`의 `Init()` 함수

```cpp
void AISystem::Init()
{
    m_strategies[CharacterTypes::Fighter] = new FighterStrategy();
    m_strategies[CharacterTypes::Rogue]   = new RogueStrategy();  // 신규 추가
    // m_strategies[CharacterTypes::Cleric] = new ClericStrategy(); // 추후
    // m_strategies[CharacterTypes::Wizard] = new WizardStrategy(); // 추후
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

### 은신 진입 → 약점 공략 시나리오

```
라운드 1:
  Rogue 비은신 상태, 드래곤 인접
  → MakeCombatDecision: !inStealth, !hasAttacked → 그림자 숨기
  → return UseAbility("그림자 숨기")

라운드 2 (은신 상태):
  → MakeCombatDecision: inStealth, HasSpellSlot(2) → 약점 공략
  → return UseAbility("약점 공략") [은신 해제, 2배 데미지]
```

### 신속 버프 → 이동 → 공격 시나리오

```
라운드 1:
  Rogue 비은신, 드래곤 거리 4칸, MP=0(부족), 1레벨 슬롯 있음
  → MakeBuffPhaseDecision: 신속 없음, 슬롯 있음, IsHasteMeaningful(MP=0)→true
  → return UseAbility("질풍의 보법") [AP-1+1=AP유지, MP+1=1]

  Phase_Decision: AP > 0, 미인접 → MakeFarMoveDecision → Move (MP 소모)
```

### 킬캐치 — 은신 약점 공략으로 처치

```
드래곤 HP 20%, Rogue 은신 상태, A* 2칸 이내
→ CanKillWithStealth: IsInStealth=true, reachable=true, dragon HP 20% ≤ 25%
→ MakeKillLoopDecision: 미인접 → Move; 인접 후 → HasSpellSlot(2) → UseAbility("약점 공략")
```

---

## Rigorous Testing

### 테스트 1: 비은신 미공격 시 그림자 숨기 우선

```cpp
// Rogue 드래곤 인접, 비은신, 미공격
rogue->SetInStealth(false);
rogue->SetHasAttackedThisTurn(false);

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should use shadow hide");
ASSERT(decision.abilityName == "그림자 숨기", "Should enter stealth when adjacent and not attacked");
ASSERT(decision.target == rogue, "Shadow hide targets self");
```

### 테스트 2: 은신 + 2레벨 슬롯 → 약점 공략

```cpp
// Rogue 은신 상태, 드래곤 인접, 2레벨 슬롯 보유
rogue->SetInStealth(true);
rogue->SetSpellSlot(2, 1);

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should use weak point in stealth");
ASSERT(decision.abilityName == "약점 공략", "Should use weak point with 2nd level slot");
ASSERT(decision.target == dragon, "Target is dragon");
```

### 테스트 3: 신속 조건 만족 시 버프 시전

```cpp
// Rogue 신속 없음, 1레벨 슬롯 있음, MP=0 (유의미함)
rogue->SetBuff("신속", false);
rogue->SetSpellSlot(1, 1);
rogue->SetMovementRange(0);

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should cast haste");
ASSERT(decision.abilityName == "질풍의 보법", "Should cast haste when MP lacking");
```

### 테스트 4: 이동 불가 & 비은신 & 미공격 → 그림자 숨기

```cpp
// Rogue 이동 불가, 드래곤 거리 3칸, 비은신, 미공격
rogue->SetMovementRange(0);
// A* 경로 없음 설정
rogue->SetInStealth(false);
rogue->SetHasAttackedThisTurn(false);

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::UseAbility, "Should stealth when can't move");
ASSERT(decision.abilityName == "그림자 숨기", "Shadow hide when movement blocked");
```

### 테스트 5: 은신 & 슬롯 없음 & 드래곤 HP 낮음 → 은신 깨고 일반 공격

```cpp
// Rogue 은신, 슬롯 없음, 드래곤 HP 25% (ShouldBreakStealth=true)
rogue->SetInStealth(true);
rogue->ClearAllSpellSlots();
dragon->SetHP(static_cast<int>(dragon->GetMaxHP() * 0.25f));

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::Attack, "Should break stealth with normal attack");
```

### 테스트 6: 이미 공격 후 비은신 → 일반 공격 반복

```cpp
// Rogue 비은신, 이미 공격함, AP 남아 있음
rogue->SetInStealth(false);
rogue->SetHasAttackedThisTurn(true);

AIDecision decision = rogue_strategy.MakeDecision(rogue);
ASSERT(decision.type == AIDecisionType::Attack, "Should normal attack after first attack (no stealth)");
ASSERT(decision.target == dragon, "Target is dragon");
```

### 테스트 7: 킬캐치 진입 조건

```cpp
// Rogue 은신, 드래곤 HP 20%, 이동 범위 내 도달 가능
rogue->SetInStealth(true);
dragon->SetHP(static_cast<int>(dragon->GetMaxHP() * 0.20f));
// A* 경로 1칸 내 설정

AIDecision decision = rogue_strategy.MakeDecision(rogue);
// Kill loop 진입: 슬롯 있으면 약점 공략, 없으면 일반
ASSERT(decision.type == AIDecisionType::UseAbility ||
       decision.type == AIDecisionType::Attack,     "Should enter kill loop");
```
