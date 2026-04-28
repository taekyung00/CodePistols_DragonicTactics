/**
 * @file ClericStrategy.h
 * @brief 스펠 시전은 [STUB] 로그 출력으로 대체 (SpellSystem 연동 시 stub 해제)
 */
#pragma once
#include "IAIStrategy.h"

class GridSystem;

class ClericStrategy : public IAIStrategy
{
  public:
  AIDecision MakeDecision(Character* actor) override;

  private:
  // --- 탐색 헬퍼 ---
  Character* FindDragon();
  Character* FindAllyNeedingHeal(float hpThreshold); // HP < threshold 인 아군 (자신 포함)

  // --- 조건 헬퍼 ---
  bool IsHealNeeded(Character* ally) const;
  bool IsBlessingActive(Character* target) const;
  bool IsCurseActive(Character* target) const;
  bool HasSpellSlot(Character* actor, int level) const;
  bool CanReachThisTurn(Character* actor, Character* target, GridSystem* grid) const;
  bool CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const;

  // --- 이동 헬퍼 (FighterStrategy 동일 로직) ---
  Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
  int         CountLavaTiles(const std::vector<Math::ivec2>& path, GridSystem* grid) const;
  int         ComputePathCost(const std::vector<Math::ivec2>& path, GridSystem* grid) const;

  // --- 서브 의사결정 ---
  AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeSupportDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeMeleePhaseDecision(Character* actor, Character* dragon, GridSystem* grid);

  static constexpr int   LAVA_TILE_PENALTY   = 3;
  static constexpr int   AVG_DAMAGE_ESTIMATE = 4;    // 클레릭 평균 딜 (1d6 기준)
  static constexpr float HEAL_THRESHOLD      = 0.5f; // 50% 미만이면 힐 대상
  static constexpr int   CURSE_RANGE         = 5;    // S_DEB_010 Enemy:Single:5
  static constexpr int   HEAL_RANGE          = 5;    // S_ENH_030 Ally:Single:5
  // S_BUF_010 Divine Shield: self 타겟 → 거리=0, 체크 불필요
};
