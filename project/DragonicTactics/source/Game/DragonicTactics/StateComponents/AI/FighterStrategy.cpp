/**
 * @file FighterStrategy.cpp
 * @author Sangyun Lee
 * @brief 파이터 AI 구현: fighter.mmd 플로우차트 기반 의사결정
 * @date 2025-12-04
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
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "FighterStrategy.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

// ============================================================
// MakeDecision — 플로우차트 최상위 흐름
// BattleOrchestrator가 EndTurn 반환 시까지 매 프레임 반복 호출.
// 루프 상태는 Character의 현재 AP/MP/HP로 자연스럽게 유지됨.
// ============================================================

AIDecision FighterStrategy::MakeDecision(Character* actor)
{
  GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

  // ============================================================
  // TODO [미구현]: 보물 탈출 / 클레릭 추적 분기
  // 보물 시스템(grid->HasExit, grid->GetExitPosition) 및
  // Cleric 캐릭터 구현 후 아래 주석을 활성화할 것.
  // ============================================================
  //
  // if (actor->HasTreasure()) {
  //   if (!grid->HasExit()) {
  //     Engine::GetLogger().LogError("Fighter has treasure but no exit found!");
  //     return { AIDecisionType::EndTurn, nullptr, {}, "", "No exit available" };
  //   }
  //   Math::ivec2 exitPos = grid->GetExitPosition();
  //   if (actor->GetGridPosition()->Get() == exitPos) {
  //     Engine::GetGameStateManager().GetGSComponent<EventBus>()->Publish(CharacterEscapedEvent{ actor });
  //     return { AIDecisionType::EndTurn, nullptr, {}, "", "Escaped with treasure!" };
  //   }
  //   return { AIDecisionType::Move, nullptr, exitPos, "", "Escaping with treasure", LAVA_TILE_PENALTY };
  // }
  //
  // if (IsInDanger(actor)) {
  //   Character* cleric = FindCleric();
  //   if (cleric != nullptr) {
  //     int clericDist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
  //                                              cleric->GetGridPosition()->Get());
  //     if (clericDist <= 1) {
  //       return { AIDecisionType::EndTurn, nullptr, {}, "", "Waiting for heal from Cleric" };
  //     }
  //     if (actor->GetMovementRange() > 0) {
  //       Math::ivec2 movePos = FindNextMovePos(actor, cleric, grid);
  //       if (movePos != actor->GetGridPosition()->Get()) {
  //         return { AIDecisionType::Move, nullptr, movePos, "", "Moving toward Cleric", LAVA_TILE_PENALTY };
  //       }
  //     }
  //     return { AIDecisionType::EndTurn, nullptr, {}, "", "Can't reach Cleric" };
  //   }
  // }
  //
  // ============================================================

  // 드래곤 탐색
  Character* dragon = FindDragon();
  if (dragon == nullptr)
  {
    return { AIDecisionType::EndTurn, nullptr, {}, "", "No dragon found" };
  }

  int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                          dragon->GetGridPosition()->Get());

  // ── 0. 킬캐치 진입 검증 ──────────────────────────────────────
  // 이번 턴 내 도달 가능 AND 드래곤 확정 처치 가능 → Kill_Loop
  if (CanReachThisTurn(actor, dragon, grid) && CanKillDragonThisTurn(actor, dragon, grid))
  {
    return MakeKillLoopDecision(actor, dragon, grid);
  }

  // ── Phase_Decision ────────────────────────────────────────────
  if (actor->GetActionPoints() <= 0)
  {
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Phase: no AP remaining" };
  }

  if (distance > 1)
  {
    // 인접 아님 → 이동 or 원거리 공포
    return MakeFarMoveDecision(actor, dragon, grid);
  }

  // 인접 (distance <= 1) → HP 판단
  if (actor->GetHPPercentage() <= 0.4f)
  {
    return MakeSurvivalDecision(actor, dragon, grid);
  }
  return MakeNormalCombatDecision(actor, dragon, grid);
}

// ============================================================
// Kill_Loop: 최고 화력 집중 루프
// 킬 가능 판단이 됐을 때 인접 → 공격 / 미인접 → 이동
// ============================================================

AIDecision FighterStrategy::MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                          dragon->GetGridPosition()->Get());

  if (distance > 1)
  {
    // 아직 인접하지 않음 → 이동 시도
    if (actor->GetMovementRange() > 0)
    {
      Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
      if (movePos != actor->GetGridPosition()->Get())
      {
        return { AIDecisionType::Move, nullptr, movePos, "", "Kill: moving to reach dragon", LAVA_TILE_PENALTY };
      }
    }
    // 이동 불가 & 비인접 → Phase_Decision으로 낙하 (비인접이므로 FarMove가 적절)
    if (actor->GetActionPoints() <= 0)
    {
      return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill: no AP, can't move" };
    }
    return MakeFarMoveDecision(actor, dragon, grid);
  }

  // 인접 상태
  if (actor->GetActionPoints() <= 0)
  {
    return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill loop: no AP" };
  }

  // 오버킬 방지: 드래곤 HP에 맞는 최저 슬롯 탐색
  int bestSlot = FindBestSmiteSlot(actor, dragon);
  if (bestSlot > 0)
  {
    std::string reason = "[STUB] Kill: Smite lv" + std::to_string(bestSlot) + " (overkill guard)";
    Engine::GetLogger().LogEvent(reason);
    return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_050", reason };
  }

  return { AIDecisionType::Attack, dragon, {}, "", "Kill: Basic attack" };
}

// ============================================================
// MakeFarMoveDecision: 인접 안 됨 → 이동 or 원거리 공포
// ============================================================

AIDecision FighterStrategy::MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  // MP > 0 이면 이동
  if (actor->GetMovementRange() > 0)
  {
    Math::ivec2 movePos = FindNextMovePos(actor, dragon, grid);
    if (movePos != actor->GetGridPosition()->Get())
    {
      return { AIDecisionType::Move, nullptr, movePos, "", "Far: moving toward dragon", LAVA_TILE_PENALTY };
    }
  }

  // 이동 불가 → 드래곤 공포 없음 & 1레벨 슬롯 & 사거리 내 → 공포의 외침
  if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
  {
    std::string reason = "[STUB] Far: Fear Cry on Dragon (can't move, range cast)";
    Engine::GetLogger().LogEvent(reason);
    return { AIDecisionType::UseAbility, actor, {}, "S_DEB_020", reason };
  }

  return { AIDecisionType::EndTurn, nullptr, {}, "", "Far: no move, no fear cast available" };
}

// ============================================================
// MakeSurvivalDecision: HP ≤ 40% 생존 시퀀스
// ============================================================

AIDecision FighterStrategy::MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                          dragon->GetGridPosition()->Get());

  if (!HasBuff(actor, "Bloodlust"))
  {
    // 피의 갈망 없음
    if (HasSpellSlot(actor, 2))
    {
      std::string reason = "[STUB] Survival: Casting Bloodlust (self-buff, lifesteal on next attacks)";
      Engine::GetLogger().LogEvent(reason);
      return { AIDecisionType::UseAbility, actor, {}, "S_ENH_010", reason };
    }
    // 2레벨 슬롯 없음 → CheckFearNear (사거리 체크 추가)
    if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
    {
      std::string reason = "[STUB] Survival: Fear Cry at Dragon (no bloodlust, near)";
      Engine::GetLogger().LogEvent(reason);
      return { AIDecisionType::UseAbility, actor, {}, "S_DEB_020", reason };
    }
    return { AIDecisionType::Attack, dragon, {}, "", "Survival: basic attack (no slots)" };
  }
  else
  {
    // 피의 갈망 활성 → 강타로 피흡 극대화 (인접 보장 필요: range=1)
    int bestSlot = FindBestSmiteSlot(actor, dragon);
    if (bestSlot > 0 && distance <= 1)
    {
      std::string reason = "[STUB] Survival+Bloodlust: Smite lv" + std::to_string(bestSlot) + " (lifesteal maximize)";
      Engine::GetLogger().LogEvent(reason);
      return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_050", reason };
    }
    return { AIDecisionType::Attack, dragon, {}, "", "Survival+Bloodlust: basic attack (lifesteal active)" };
  }
}

// ============================================================
// MakeNormalCombatDecision: HP > 40%, 인접 — 일반 교전 로직
// ============================================================

AIDecision FighterStrategy::MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid)
{
  int        distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                                  dragon->GetGridPosition()->Get());
  Character* cleric   = FindCleric();

  // 공통: 강타 or 일반 공격 (Smite range=1: 인접 보장 필요)
  auto SmiteOrNormal = [&]() -> AIDecision
  {
    int slot = FindHighestSmiteSlot(actor);
    if (slot > 0 && distance <= 1)
    {
      std::string reason = "[STUB] Normal: Smite lv" + std::to_string(slot) + " (highest available slot)";
      Engine::GetLogger().LogEvent(reason);
      return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_050", reason };
    }
    return { AIDecisionType::Attack, dragon, {}, "", "Normal: basic attack (no slots)" };
  };

  // 공통: 인접 공포 or 일반 공격
  auto FearNearOrAttack = [&]() -> AIDecision
  {
    if (!IsFearActive(dragon) && HasSpellSlot(actor, 1) && IsInFearRange(actor, dragon, grid))
    {
      std::string reason = "[STUB] Normal: Fear Cry (near, dragon not feared)";
      Engine::GetLogger().LogEvent(reason);
      return { AIDecisionType::UseAbility, actor, {}, "S_DEB_020", reason };
    }
    return { AIDecisionType::Attack, dragon, {}, "", "Normal: basic attack" };
  };

  if (HasBuff(actor, "Blessing"))
  {
    // 축복 적용 중
    if (!HasBuff(actor, "Frenzy") && actor->GetActionPoints() >= 2)
    {
      std::string reason = "[STUB] Normal+Blessing: Casting Frenzy (AP bonus buff)";
      Engine::GetLogger().LogEvent(reason);
      return { AIDecisionType::UseAbility, actor, {}, "S_ENH_020", reason };
    }
    // 광란 이미 있거나 AP 부족 → 강타 or 일반
    return SmiteOrNormal();
  }
  else
  {
    // 축복 없음
    if (cleric != nullptr)
    {
      // 클레릭 생존 → CheckFearNear
      return FearNearOrAttack();
    }
    else
    {
      // 클레릭 없음 → 독립 광란 판단
      if (!HasBuff(actor, "Frenzy") && actor->GetActionPoints() >= 2)  // Frenzy는 레벨 0: 슬롯 불필요
      {
        std::string reason = "[STUB] Normal(no cleric): Casting Frenzy independently";
        Engine::GetLogger().LogEvent(reason);
        return { AIDecisionType::UseAbility, actor, {}, "S_ENH_020", reason };
      }
      if (HasBuff(actor, "Frenzy"))
      {
        return SmiteOrNormal();
      }
      return FearNearOrAttack();
    }
  }
}

// ============================================================
// 탐색 헬퍼
// ============================================================

Character* FighterStrategy::FindDragon()
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

Character* FighterStrategy::FindCleric()
{
  GridSystem* grid     = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  auto        allChars = grid->GetAllCharacters();

  for (auto* c : allChars)
  {
    if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Cleric)
    {
      return c;
    }
  }
  return nullptr; // TODO: Cleric 캐릭터 구현 전까지 항상 nullptr 반환
}

// ============================================================
// 판단 헬퍼
// ============================================================

bool FighterStrategy::IsInDanger(Character* actor) const
{
  return actor->GetHPPercentage() <= 0.4f;
}

bool FighterStrategy::HasBuff(Character* actor, const std::string& buffName) const
{
  return actor->Has(buffName);
}

bool FighterStrategy::HasSpellSlot(Character* actor, int level) const
{
  return actor->HasSpellSlot(level);
}

bool FighterStrategy::IsFearActive(Character* dragon) const
{
  return dragon->Has("Fear");
}

bool FighterStrategy::IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const
{
  int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                      dragon->GetGridPosition()->Get());
  return dist <= FEAR_RANGE;
}

bool FighterStrategy::CanReachThisTurn(Character* actor, Character* target, GridSystem* grid) const
{
  int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
                                      target->GetGridPosition()->Get());
  if (dist <= 1)
    return true; // 이미 인접

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

bool FighterStrategy::CanKillDragonThisTurn(Character* actor, Character* dragon, [[maybe_unused]] GridSystem* grid) const
{
  // 간이 추정: 남은 AP × 평균 데미지 ≥ 드래곤 현재 HP
  int estimatedDamage = actor->GetActionPoints() * AVG_DAMAGE_ESTIMATE;
  return dragon->GetHP() <= estimatedDamage;
}

// ============================================================
// 슬롯 탐색 헬퍼
// ============================================================

int FighterStrategy::FindBestSmiteSlot(Character* actor, Character* dragon) const
{
  // 오버킬 방지: 드래곤 HP를 처치할 수 있는 최저 슬롯 레벨 반환
  for (int level = 1; level <= 9; ++level)
  {
    if (actor->HasSpellSlot(level) && dragon->GetHP() <= SMITE_BASE_DAMAGE * level)
    {
      return level;
    }
  }
  return 0; // 적합한 슬롯 없음 (일반 공격 사용)
}

int FighterStrategy::FindHighestSmiteSlot(Character* actor) const
{
  for (int level = 9; level >= 1; --level)
  {
    if (actor->HasSpellSlot(level))
    {
      return level;
    }
  }
  return 0;
}

// ============================================================
// 이동 경로 탐색 (기존 로직 유지)
// ============================================================

Math::ivec2 FighterStrategy::FindNextMovePos(Character* actor, Character* target, GridSystem* grid)
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

  return myPos; // 갈 곳 없으면 제자리
}

int FighterStrategy::CountLavaTiles(const std::vector<Math::ivec2>& path, GridSystem* grid) const
{
  int count = 0;
  for (const auto& tile : path)
  {
    if (grid->GetTileType(tile) == GridSystem::TileType::Lava)
      ++count;
  }
  return count;
}

int FighterStrategy::ComputePathCost(const std::vector<Math::ivec2>& path, GridSystem* grid) const
{
  return static_cast<int>(path.size()) + CountLavaTiles(path, grid) * LAVA_TILE_PENALTY;
}
