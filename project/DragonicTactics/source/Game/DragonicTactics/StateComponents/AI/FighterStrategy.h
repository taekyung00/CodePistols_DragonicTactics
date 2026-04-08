/**
 * @file FighterStrategy.h
 * @author Sangyun Lee
 * @brief 파이터 전용 AI 전략 (드래곤 추적 및 근접 공격)
 * @date 2025-12-04
 */
#pragma once
#include "IAIStrategy.h"

class GridSystem;

class FighterStrategy : public IAIStrategy
{
  public:
  AIDecision MakeDecision(Character* actor) override;

  private:
  Character* FindDragon();
  Character* FindCleric();

  // --- 판단 헬퍼 ---
<<<<<<< HEAD
  bool IsInDanger(Character* actor) const; // HP 40% 이하
  bool CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const;
  bool HasBuff(Character* actor, const std::string& buffName) const; // TODO: Week 6+ StatusEffect 연동
  bool HasSpellSlot(Character* actor, int level) const;
  bool IsFearActive(Character* dragon) const; // TODO: Week 6+ StatusEffect 연동
  bool IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const;

  // --- 이동 ---
  Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
  bool		  CanReachThisTurn(Character* actor, Character* target, GridSystem* grid) const;

  // --- 서브 의사결정 ---
  AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid);

  //old
  bool ShouldUseSpellAttack(Character* actor, Character* target) const;


=======
  bool IsInDanger(Character* actor) const;                                         // HP 40% 이하
  bool HasBuff(Character* actor, const std::string& buffName) const;               // StatusEffect 확인
  bool HasSpellSlot(Character* actor, int level) const;                            // 주문 슬롯 확인
  bool IsFearActive(Character* dragon) const;                                      // 드래곤 공포 상태 확인
  bool IsInFearRange(Character* actor, Character* dragon, GridSystem* grid) const; // 공포 사거리 내 여부
  bool CanReachThisTurn(Character* actor, Character* target, GridSystem* grid) const;
  bool CanKillDragonThisTurn(Character* actor, Character* dragon, GridSystem* grid) const;

  // --- 슬롯 탐색 ---
  int FindBestSmiteSlot(Character* actor, Character* dragon) const;  // 오버킬 방지: 최저 적합 슬롯
  int FindHighestSmiteSlot(Character* actor) const;                  // 최고 레벨 슬롯

  // --- 이동 ---
  Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
>>>>>>> ginam

  // --- 용암 회피 ---
  static constexpr int LAVA_TILE_PENALTY   = 3;
  static constexpr int FEAR_RANGE          = 3;   // 공포의 외침 사거리 (타일)
  static constexpr int AVG_DAMAGE_ESTIMATE = 5;   // CanKill 계산용 평균 공격 데미지
  static constexpr int SMITE_BASE_DAMAGE   = 8;   // FindBestSmiteSlot 기준 데미지/레벨
  int                  CountLavaTiles(const std::vector<Math::ivec2>& path, GridSystem* grid) const;
  int                  ComputePathCost(const std::vector<Math::ivec2>& path, GridSystem* grid) const;

  // --- 서브 의사결정 ---
  AIDecision MakeKillLoopDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeFarMoveDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeSurvivalDecision(Character* actor, Character* dragon, GridSystem* grid);
  AIDecision MakeNormalCombatDecision(Character* actor, Character* dragon, GridSystem* grid);
};
