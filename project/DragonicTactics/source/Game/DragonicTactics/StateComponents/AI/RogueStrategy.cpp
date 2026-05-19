#include "pch.h"

#include "RogueStrategy.h"
#include "../../Objects/Components/ActionPoints.h"
#include "../../Objects/Components/GridPosition.h"
#include "../../Objects/Components/SpellSlots.h"
#include "../../Objects/Components/StatsComponent.h"
#include "../../StateComponents/GridSystem.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

// ============================================================
// MakeDecision — rouge.mmd 플로우차트 최상위 흐름
// ============================================================

AIDecision RogueStrategy::MakeDecision(Character* actor)
{
  GridSystem* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  Character*  dragon = FindDragon();

  if (!dragon)
    return { AIDecisionType::EndTurn, nullptr, {}, "", "No dragon found" };

  // ── [0] 킬캐치 오버라이드: 은신 중 & 처치 가능 ──────────────
  if (IsInStealth(actor) && CanKillWithStealth(actor, dragon, grid))
    return MakeKillLoopDecision(actor, dragon, grid);

  // ── [1] 버프 페이즈: Gale Step ──────────────────────────────
  AIDecision buffDecision = MakeBuffPhaseDecision(actor, dragon, grid);
  if (buffDecision.type != AIDecisionType::None)
    return buffDecision;

  // ── [2] 판단 지점 루프 ──────────────────────────────────────
  // Shadow Hide 직후: 스텔스 + AP=0 + 이동력 잔여 → 후퇴 이동
  if (IsInStealth(actor) && actor->GetActionPoints() <= 0 && actor->GetMovementRange() > 0)
  {
    Math::ivec2 retreatPos = FindRetreatPos(actor, dragon, grid);
    if (retreatPos != actor->GetGridPosition()->Get())
      return { AIDecisionType::Move, nullptr, retreatPos, "", "Stealth: retreat to safe dist", LAVA_TILE_PENALTY };
  }

  if (actor->GetActionPoints() <= 0)
    return { AIDecisionType::EndTurn, nullptr, {}, "", "No AP" };

  int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                      dragon->GetGridPosition()->Get());

  if (dist > 1)
    return MakeFarMoveDecision(actor, dragon, grid);
  else
    return MakeCombatDecision(actor, dragon, grid);
}

// ============================================================
// Kill_Loop: 은신 + 확정 킬 루프
// ============================================================

AIDecision RogueStrategy::MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                      dragon->GetGridPosition()->Get());

  if (dist > 1)
  {
    if (actor->GetMovementRange() > 0)
    {
      Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
      if (movePos != actor->GetGridPosition()->Get())
        return { AIDecisionType::Move, nullptr, movePos, "", "Kill: Moving to dragon", LAVA_TILE_PENALTY };
    }
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill: Blocked, can't reach" };
  }

  if (actor->GetActionPoints() <= 0)
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill: No AP" };

  // Weakpoint Strike (Lv2) — dist=1 이미 보장, 무한루프 없음
  if (HasSpellSlot(actor, 2))
    return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_070", "Kill: Weakpoint Strike (stealth 2x)" };

  return { AIDecisionType::Attack, dragon, {}, "", "Kill: Basic attack (stealth 2x)" };
}

// ============================================================
// 버프 페이즈: Gale Step(Haste) 시전 여부 결정
// ============================================================

AIDecision RogueStrategy::MakeBuffPhaseDecision(Character* actor, Character* /*dragon*/, GridSystem* /*grid*/)
{
  if (actor->Has("Haste"))
    return { AIDecisionType::None, nullptr, {}, "", "" };

  if (!HasSpellSlot(actor, 1))
    return { AIDecisionType::None, nullptr, {}, "", "" };

  if (!IsHasteMeaningful(actor))
    return { AIDecisionType::None, nullptr, {}, "", "" };

  return { AIDecisionType::UseAbility, actor, {}, "S_BUF_020", "Buff: Gale Step (Haste)", 0, 1 };
}

// ============================================================
// 비인접 상황: 이동 or Shadow Hide
// ============================================================

AIDecision RogueStrategy::MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  if (actor->GetMovementRange() > 0)
  {
    Math::ivec2 myPos   = actor->GetGridPosition()->Get();
    Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
    if (movePos != myPos)
      return { AIDecisionType::Move, nullptr, movePos, "", "Far: Moving to dragon", LAVA_TILE_PENALTY };
  }

  // 이동 불가 & 미공격 & 비은신 → 은신 진입
  if (!actor->HasAttackedThisTurn() && !IsInStealth(actor))
    return { AIDecisionType::UseAbility, actor, {}, "S_ENH_060", "Far: Enter stealth (can't move)" };

  return { AIDecisionType::EndTurn, nullptr, {}, "", "Far: No move or stealth option" };
}

// ============================================================
// 인접 교전 로직
// ============================================================

AIDecision RogueStrategy::MakeCombatDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  // ── 은신 중 ────────────────────────────────────────────────
  if (IsInStealth(actor))
  {
    // Weakpoint Strike (Lv2) — dist<=1 이미 보장, 무한루프 없음
    if (HasSpellSlot(actor, 2))
      return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_070",
               "Combat: Weakpoint Strike (stealth 2x)" };

    // 슬롯 없음 → 은신 깨고 평타 칠 가치?
    if (ShouldBreakStealth(actor, dragon))
      return { AIDecisionType::Attack, dragon, {}, "", "Combat: Normal attack (stealth 2x, break)" };

    // 가치 없음 → 재포지셔닝
    if (actor->GetMovementRange() > 0)
    {
      Math::ivec2 repoPos = FindRepositionPos(actor, dragon, grid);
      if (repoPos != actor->GetGridPosition()->Get())
        return { AIDecisionType::Move, nullptr, repoPos, "", "Combat: Reposition (stealth)" };
    }
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Combat: Stealth end (no reposition)" };
  }

  // ── 비은신 ─────────────────────────────────────────────────
  // 아직 공격 안 했으면 → 은신 진입
  if (!actor->HasAttackedThisTurn())
    return { AIDecisionType::UseAbility, actor, {}, "S_ENH_060",
             "Combat: Enter stealth (adjacent, not attacked)" };

  // 이미 공격 → 일반 공격
  return { AIDecisionType::Attack, dragon, {}, "", "Combat: Normal attack" };
}

// ============================================================
// 헬퍼 함수들
// ============================================================

Character* RogueStrategy::FindDragon()
{
  GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  if (!grid) return nullptr;

  for (auto* c : grid->GetAllCharacters())
  {
    if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Dragon)
      return c;
  }
  return nullptr;
}

bool RogueStrategy::IsInStealth(Character* actor) const
{
  return actor->Has("Stealth");
}

bool RogueStrategy::HasSpellSlot(Character* actor, int level) const
{
  return actor->GetAvailableSpellSlots(level) > 0;
}

bool RogueStrategy::CanKillWithStealth(Character* actor, Character* dragon, GridSystem* grid) const
{
  if (dragon->GetHPPercentage() > STEALTH_KILL_HP_THRESHOLD)
    return false;

  // 이미 인접한 경우
  Math::ivec2 myPos  = actor->GetGridPosition()->Get();
  Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();
  if (grid->ManhattanDistance(myPos, tgtPos) <= 1)
    return true;

  // A* 경로로 이번 턴 내 도달 가능?
  static const Math::ivec2 offsets[4] = { { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 } };
  int                      mp         = actor->GetMovementRange();
  for (const auto& off : offsets)
  {
    Math::ivec2 attackPos = tgtPos + off;
    if (!grid->IsValidTile(attackPos)) continue;
    GridSystem::TileType t = grid->GetTileType(attackPos);
    if ((t != GridSystem::TileType::Empty && t != GridSystem::TileType::Lava) || grid->IsOccupied(attackPos))
      continue;
    auto path = grid->FindPath(myPos, attackPos, LAVA_TILE_PENALTY);
    if (!path.empty() && static_cast<int>(path.size()) <= mp)
      return true;
  }
  return false;
}

bool RogueStrategy::ShouldBreakStealth(Character* /*actor*/, Character* dragon) const
{
  return dragon->GetHPPercentage() <= BREAK_STEALTH_THRESHOLD;
}

bool RogueStrategy::IsHasteMeaningful(Character* actor) const
{
  if (actor->GetActionPoints() <= 0)
    return false;
  // 이동력 소진(다음 턴 혜택) or AP 2 이상(이번 턴 추가 행동 가능)
  return actor->GetMovementRange() <= 0 || actor->GetActionPoints() >= 2;
}

// ============================================================
// 이동 경로 탐색 — FighterStrategy::FindNextMovePos와 동일 구조
// ============================================================

Math::ivec2 RogueStrategy::FindNextMovePos(Character* actor, Character* target, GridSystem* grid)
{
  Math::ivec2 targetPos    = target->GetGridPosition()->Get();
  Math::ivec2 myPos        = actor->GetGridPosition()->Get();

  // 협공 선호도: Fighter 반대편 포지션 선호
  Character* fighter_char = nullptr;
  for (auto* c : grid->GetAllCharacters())
  {
    if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Fighter)
    {
      fighter_char = c;
      break;
    }
  }

  int bestPathCost  = 999999;
  int bestPrefScore = 999999;

  std::vector<Math::ivec2> bestPath;

  static const Math::ivec2 offsets[4] = { { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 } };
  for (const auto& offset : offsets)
  {
    Math::ivec2          attackPos  = targetPos + offset;
    if (!grid->IsValidTile(attackPos)) continue;
    GridSystem::TileType attackTile = grid->GetTileType(attackPos);
    bool attack_ok = (attackTile == GridSystem::TileType::Empty || attackTile == GridSystem::TileType::Lava)
                      && !grid->IsOccupied(attackPos);
    if (!attack_ok) continue;

    auto path = grid->FindPath(myPos, attackPos, LAVA_TILE_PENALTY);
    if (!path.empty())
    {
      int cost      = ComputePathCost(path, grid);
      GridPosition* fp  = fighter_char ? fighter_char->GetGridPosition() : nullptr;
      int prefScore     = fp ? -grid->ManhattanDistance(attackPos, fp->Get()) : 0;
      if (cost < bestPathCost ||
          (cost == bestPathCost && prefScore < bestPrefScore))
      {
        bestPathCost  = cost;
        bestPrefScore = prefScore;
        bestPath      = path;
      }
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

// 드래곤 반대 방향으로 1칸 이동 (은신 유지 재포지셔닝)
Math::ivec2 RogueStrategy::FindRepositionPos(Character* actor, Character* dragon, GridSystem* grid)
{
  Math::ivec2 myPos  = actor->GetGridPosition()->Get();
  Math::ivec2 tgtPos = dragon->GetGridPosition()->Get();

  Math::ivec2 dir = { myPos.x - tgtPos.x, myPos.y - tgtPos.y };
  Math::ivec2 candidate = {
    myPos.x + (dir.x > 0 ? 1 : dir.x < 0 ? -1 : 0),
    myPos.y + (dir.y > 0 ? 1 : dir.y < 0 ? -1 : 0)
  };

  if (grid->IsValidTile(candidate))
  {
    GridSystem::TileType t = grid->GetTileType(candidate);
    if ((t == GridSystem::TileType::Empty || t == GridSystem::TileType::Lava)
        && !grid->IsOccupied(candidate))
      return candidate;
  }
  return myPos;
}

int RogueStrategy::CountLavaTiles(const std::vector<Math::ivec2>& path, GridSystem* grid) const
{
  int count = 0;
  for (const auto& tile : path)
  {
    if (grid->GetTileType(tile) == GridSystem::TileType::Lava)
      ++count;
  }
  return count;
}

int RogueStrategy::ComputePathCost(const std::vector<Math::ivec2>& path, GridSystem* grid) const
{
  return static_cast<int>(path.size()) + CountLavaTiles(path, grid) * LAVA_TILE_PENALTY;
}

Math::ivec2 RogueStrategy::FindRetreatPos(Character* actor, Character* dragon, GridSystem* grid)
{
  Math::ivec2 myPos     = actor->GetGridPosition()->Get();
  Math::ivec2 dragonPos = dragon->GetGridPosition()->Get();

  auto reachable = grid->GetReachableTiles(myPos, actor->GetMovementRange());

  Math::ivec2 best     = myPos;
  int         bestDist = 0;

  for (const auto& tile : reachable)
  {
    int d = grid->ManhattanDistance(tile, dragonPos);
    if (d < 2 || d > SAFE_RETREAT_DIST)
      continue;
    if (grid->GetTileType(tile) == GridSystem::TileType::Lava && bestDist >= 2)
      continue;
    if (d > bestDist)
    {
      bestDist = d;
      best     = tile;
    }
  }
  return best;
}
