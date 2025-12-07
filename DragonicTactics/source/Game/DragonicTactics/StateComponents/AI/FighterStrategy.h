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

  // 전략별 판단 헬퍼 (Decision Helpers)
  // Character의 팩트 쿼리를 사용하여 Fighter만의 기준으로 판단
  bool IsInDanger(Character* actor) const; // Fighter: HP 30% 이하
  bool ShouldUseSpellAttack(Character* actor, Character* target) const;

  Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);

  // 공격 전략
  AIDecision DecideAttackAction(Character* actor, Character* target, int distance);
};