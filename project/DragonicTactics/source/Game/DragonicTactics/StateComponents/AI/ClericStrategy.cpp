/**
 * @file ClericStrategy.cpp
 * @brief 클레릭 AI 구현: cleric.mmd 플로우차트 기반 의사결정
 *
 * 스펠 시전은 모두 [STUB] 로그 출력 + UseAbility 반환으로 대체.
 * AISystem::ExecuteDecision이 CastSpell을 호출하므로 실제 효과는 SpellSystem이 처리.
 *
 * 클레릭 스펠 (공격 스펠 없음 — 기본 공격으로만 딜):
 *   S_ENH_030  Healing Touch   Ally:Single:5   힐 + Blessing 3턴
 *   S_BUF_010  Divine Shield   Ally:Single:4   Blessing 3턴
 *   S_DEB_010  Curse           Enemy:Single:5  Curse 3턴
 *
 * Kill_Loop K_Slot Yes 경로가 cleric.mmd에 미정의 → 공격 스펠 없으므로 항상 기본 공격.
 */
#include "pch.h"

#include "../../Objects/Components/ActionPoints.h"
#include "../../Objects/Components/GridPosition.h"
#include "../../Objects/Components/SpellSlots.h"
#include "../../Objects/Components/StatsComponent.h"
#include "../../StateComponents/CombatSystem.h"
#include "../../StateComponents/GridSystem.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "ClericStrategy.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

// ============================================================
// MakeDecision — 플로우차트 최상위 흐름 (cleric.mmd)
// ============================================================

AIDecision ClericStrategy::MakeDecision(Character* actor)
{
  GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

  Character* dragon = FindDragon();
  if (dragon == nullptr)
  {
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Cleric: No dragon found" };
  }

  // ── [1순위] 확정 킬 ────────────────────────────────────────
  if (CanReachThisTurn(actor, dragon, grid) && CanKillDragonThisTurn(actor, dragon, grid))
  {
    return MakeKillLoopDecision(actor, dragon, grid);
  }

  // ── Phase_Decision ────────────────────────────────────────
  if (actor->GetActionPoints() <= 0)
  {
    // AP 없음. MP 남아있으면 다음 턴 대비 전술 이동.
    if (actor->GetMovementRange() > 0)
    {
      Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
      if (movePos != actor->GetGridPosition()->Get())
      {
        return { AIDecisionType::Move, nullptr, movePos, "", "Cleric: Tactical positioning (no AP)", LAVA_TILE_PENALTY };
      }
    }
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Cleric: No AP remaining" };
  }

  // ── [2순위] 힐 필요 여부 체크 ─────────────────────────────
  Character* healTarget = FindAllyNeedingHeal(HEAL_THRESHOLD);
  if (healTarget != nullptr && HasSpellSlot(actor, 1))
  {
    int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                        healTarget->GetGridPosition()->Get());
    if (dist <= HEAL_RANGE)
    {
      std::string reason = "[STUB] Cleric: Healing Touch on " + healTarget->TypeName();
      Engine::GetLogger().LogEvent(reason);
      return { AIDecisionType::UseAbility, healTarget, {}, "S_ENH_030", reason };
    }
    // 사거리 밖이면 힐 건너뜀 → 지원/근접 단계로 계속
  }

  // ── 버프/디버프 지원 ──────────────────────────────────────
  if (HasSpellSlot(actor, 1))
  {
    return MakeSupportDecision(actor, dragon, grid);
  }

  // ── 슬롯 없음 → 근접 공격 ────────────────────────────────
  return MakeMeleePhaseDecision(actor, dragon, grid);
}

// ============================================================
// Kill_Loop: 확정 킬 시퀀스 (cleric.mmd Kill_Loop 서브그래프)
// 클레릭은 공격 스펠 없음 → K_Slot Yes/No 무관, 항상 기본 공격
// ============================================================

AIDecision ClericStrategy::MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                          dragon->GetGridPosition()->Get());

  if (distance > 1)
  {
    if (actor->GetMovementRange() > 0)
    {
      Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
      if (movePos != actor->GetGridPosition()->Get())
      {
        return { AIDecisionType::Move, nullptr, movePos, "", "Cleric Kill: moving to reach dragon", LAVA_TILE_PENALTY };
      }
    }
    if (actor->GetActionPoints() <= 0)
    {
      return { AIDecisionType::EndTurn, nullptr, {}, "", "Cleric Kill: no AP, can't move" };
    }
    return MakeMeleePhaseDecision(actor, dragon, grid);
  }

  if (actor->GetActionPoints() <= 0)
  {
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Cleric Kill: no AP" };
  }

  // 공격 스펠 없으므로 슬롯 유무 관계없이 기본 공격
  return { AIDecisionType::Attack, dragon, {}, "", "Cleric Kill: basic attack" };
}

// ============================================================
// MakeSupportDecision: 버프/디버프 우선순위 결정
//   1. Dragon에 Curse 없으면 → Curse (S_DEB_010, range 5)
//   2. 자신에 Blessing 없으면 → Divine Shield (S_BUF_010, range 4)
//      ※ Healing Touch로 힐받은 아군은 이미 Blessed → 중복 방지
//   3. 지원 불필요 → MeleePhase
// ============================================================

AIDecision ClericStrategy::MakeSupportDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  int dist_to_dragon = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                                dragon->GetGridPosition()->Get());

  if (!IsCurseActive(dragon) && HasSpellSlot(actor, 1) && dist_to_dragon <= CURSE_RANGE)
  {
    std::string reason = "[STUB] Cleric: Curse of Suffering on " + dragon->TypeName();
    Engine::GetLogger().LogEvent(reason);
    return { AIDecisionType::UseAbility, dragon, {}, "S_DEB_010", reason };
  }

  // Healing Touch가 Blessing을 같이 부여하므로 힐받은 아군은 이미 Blessed
  if (!IsBlessingActive(actor) && HasSpellSlot(actor, 1))
  {
    std::string reason = "[STUB] Cleric: Divine Shield on self";
    Engine::GetLogger().LogEvent(reason);
    return { AIDecisionType::UseAbility, actor, {}, "S_BUF_010", reason };
  }

  return MakeMeleePhaseDecision(actor, dragon, grid);
}

// ============================================================
// MakeMeleePhaseDecision: 기본 공격 프로세스 (cleric.mmd MeleePhase)
// ============================================================

AIDecision ClericStrategy::MakeMeleePhaseDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                          dragon->GetGridPosition()->Get());

  if (distance <= 1)
  {
    return { AIDecisionType::Attack, dragon, {}, "", "Cleric Melee: basic attack (adjacent)" };
  }

  if (actor->GetMovementRange() > 0)
  {
    Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
    if (movePos != actor->GetGridPosition()->Get())
    {
      return { AIDecisionType::Move, nullptr, movePos, "", "Cleric Melee: moving toward dragon", LAVA_TILE_PENALTY };
    }
  }

  return { AIDecisionType::EndTurn, nullptr, {}, "", "Cleric Melee: no move possible" };
}

// ============================================================
// 탐색 헬퍼
// ============================================================

Character* ClericStrategy::FindDragon()
{
  GridSystem* grid     = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  auto        allChars = grid->GetAllCharacters();

  for (auto* c : allChars)
  {
    if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Dragon)
    {
      return c;
    }
  }
  return nullptr;
}

Character* ClericStrategy::FindAllyNeedingHeal(float hpThreshold)
{
  GridSystem* grid     = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  auto        allChars = grid->GetAllCharacters();

  Character* bestTarget    = nullptr;
  float      lowestHPRatio = hpThreshold;

  for (auto* c : allChars)
  {
    if (c && c->IsAlive() && c->GetCharacterType() != CharacterTypes::Dragon)
    {
      float ratio = c->GetHPPercentage();
      if (ratio < lowestHPRatio)
      {
        lowestHPRatio = ratio;
        bestTarget    = c;
      }
    }
  }
  return bestTarget;
}

// ============================================================
// 조건 헬퍼
// ============================================================

bool ClericStrategy::IsHealNeeded(Character* ally) const
{
  return ally->GetHPPercentage() < HEAL_THRESHOLD;
}

bool ClericStrategy::IsBlessingActive(Character* target) const
{
  return target->Has("Blessing");
}

bool ClericStrategy::IsCurseActive(Character* target) const
{
  return target->Has("Curse");
}

bool ClericStrategy::HasSpellSlot(Character* actor, int level) const
{
  return actor->HasSpellSlot(level);
}

bool ClericStrategy::CanReachThisTurn(Character* actor, Character* target, GridSystem* grid) const
{
  int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                      target->GetGridPosition()->Get());
  if (dist <= 1)
    return true;

  Math::ivec2              myPos     = actor->GetGridPosition()->Get();
  Math::ivec2              targetPos = target->GetGridPosition()->Get();
  static const Math::ivec2 offsets[4] = { { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 } };

  for (const auto& offset : offsets)
  {
    Math::ivec2 attackPos = targetPos + offset;
    if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos))
      continue;
    std::vector<Math::ivec2> path = grid->FindPath(myPos, attackPos, LAVA_TILE_PENALTY);
    if (!path.empty() && static_cast<int>(path.size()) <= actor->GetMovementRange())
    {
      return true;
    }
  }
  return false;
}

bool ClericStrategy::CanKillDragonThisTurn(Character* actor, Character* dragon, [[maybe_unused]] GridSystem* grid) const
{
  int estimatedDamage = actor->GetActionPoints() * AVG_DAMAGE_ESTIMATE;
  return dragon->GetHP() <= estimatedDamage;
}

// ============================================================
// 이동 경로 탐색 (FighterStrategy 동일 로직)
// ============================================================

Math::ivec2 ClericStrategy::FindNextMovePos(Character* actor, Character* target, GridSystem* grid)
{
  Math::ivec2 targetPos = target->GetGridPosition()->Get();
  Math::ivec2 myPos     = actor->GetGridPosition()->Get();

  std::vector<Math::ivec2> bestPath;
  int                      bestPathCost = 999999;

  static const Math::ivec2 offsets[4] = { { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 } };
  for (const auto& offset : offsets)
  {
    Math::ivec2 attackPos = targetPos + offset;
    if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos))
      continue;

    std::vector<Math::ivec2> currentPath = grid->FindPath(myPos, attackPos, LAVA_TILE_PENALTY);
    if (!currentPath.empty())
    {
      int effectiveCost = ComputePathCost(currentPath, grid);
      if (effectiveCost < bestPathCost)
      {
        bestPathCost = effectiveCost;
        bestPath     = currentPath;
      }
    }
  }

  if (!bestPath.empty())
  {
    int maxReach  = std::min(static_cast<int>(bestPath.size()), actor->GetMovementRange());
    int destIndex = maxReach - 1;
    if (destIndex >= 0)
    {
      return bestPath[static_cast<std::size_t>(destIndex)];
    }
  }

  return myPos;
}

int ClericStrategy::CountLavaTiles(const std::vector<Math::ivec2>& path, GridSystem* grid) const
{
  int count = 0;
  for (const auto& tile : path)
  {
    if (grid->GetTileType(tile) == GridSystem::TileType::Lava)
      ++count;
  }
  return count;
}

int ClericStrategy::ComputePathCost(const std::vector<Math::ivec2>& path, GridSystem* grid) const
{
  return static_cast<int>(path.size()) + CountLavaTiles(path, grid) * LAVA_TILE_PENALTY;
}
