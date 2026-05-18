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
  bool IsInStealth(Character* actor) const;
  bool HasSpellSlot(Character* actor, int level) const;
  bool CanKillWithStealth(Character* actor, Character* dragon, GridSystem* grid) const;
  bool ShouldBreakStealth(Character* actor, Character* dragon) const;
  bool IsHasteMeaningful(Character* actor) const;

  // --- 이동 ---
  Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
  Math::ivec2 FindRepositionPos(Character* actor, Character* dragon, GridSystem* grid);

  // --- 경로 비용 ---
  int CountLavaTiles(const std::vector<Math::ivec2>& path, GridSystem* grid) const;
  int ComputePathCost(const std::vector<Math::ivec2>& path, GridSystem* grid) const;

  // --- 서브 의사결정 ---
  AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeBuffPhaseDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeCombatDecision(Character* actor, Character* dragon, GridSystem* grid);

  static constexpr float STEALTH_KILL_HP_THRESHOLD = 0.25f;
  static constexpr float BREAK_STEALTH_THRESHOLD   = 0.30f;
  static constexpr int   LAVA_TILE_PENALTY          = 2;
};
