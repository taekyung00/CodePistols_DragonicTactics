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
  Character* FindCleric(); // TODO: Cleric 구현 후 활성화

  // --- 판단 헬퍼 ---
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
  bool IsInDanger(Character* actor) const; // Fighter: HP 30% 이하
  bool ShouldUseSpellAttack(Character* actor, Character* target) const;

  Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);

  // 공격 전략
  AIDecision DecideAttackAction(Character* actor, Character* target, int distance);
};