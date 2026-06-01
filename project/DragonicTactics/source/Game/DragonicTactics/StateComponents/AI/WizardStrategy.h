#pragma once
#include "IAIStrategy.h"

class GridSystem;

class WizardStrategy : public IAIStrategy
{
  public:
  AIDecision MakeDecision(Character* actor) override;

  private:
  // --- 타겟 탐색 ---
  Character* FindDragon();

  // --- 판단 헬퍼 ---
  bool  IsInSweetSpot(Character* actor, Character* dragon, GridSystem* grid) const;
  bool  CanKillThisTurn(Character* actor, Character* dragon, GridSystem* grid, bool has_fear) const;
  bool  HasSpellSlot(Character* actor, int level) const;
  float GetSlotRatio(Character* actor) const;

  // --- 이동 ---
  Math::ivec2 FindSweetSpotTile(Character* actor, Character* dragon, GridSystem* grid) const;
  Math::ivec2 FindNextMoveToward(Character* actor, Math::ivec2 goal, GridSystem* grid) const;

  // --- 서브 의사결정 ---
  AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid, bool has_fear);
  AIDecision MakeMoveDecision(Character* actor, Character* dragon, GridSystem* grid, bool has_fear);
  AIDecision MakeAttackDecision(Character* actor, Character* dragon, GridSystem* grid);

  // --- Sweet Spot: Dragon에서 3~4타일 거리 (Fire Bolt 사거리 내, 근접 안전) ---
  static constexpr int   SWEET_SPOT_MIN         = 3;
  static constexpr int   SWEET_SPOT_MAX         = 4;  // Fire Bolt 최대 사거리
  static constexpr int   FIRE_BOLT_RANGE        = 4;  // S_ATK_010 Enemy:Single:4
  static constexpr int   TELEPORT_RANGE         = 4;  // S_GEO_030 CSV Empty:Point:4 와 일치
  static constexpr float SLOT_THRESHOLD         = 0.5f;
  static constexpr float MANA_CONV_HP_THRESHOLD = 0.30f;
  static constexpr int   AVG_FIRE_BOLT_DAMAGE   = 9;  // avg 2d8
  static constexpr int   FEAR_DMG_PENALTY       = 3;
  static constexpr int   LAVA_TILE_PENALTY      = 2;

  // 최대 슬롯 합계 (Lv1×4 + Lv2×3 = 7) — GetSlotRatio 계산용
  static constexpr int MAX_TOTAL_SLOTS = 7;
};
