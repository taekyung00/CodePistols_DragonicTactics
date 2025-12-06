#include "pch.h"
#include "FighterStrategy.h"
#include "./Game/DragonicTactics/Objects/Character.h"

AIDecision FighterStrategy::DecideAttackAction([[maybe_unused]] Character* actor, Character* target, [[maybe_unused]] int distance)
{
  // Week 6+ Ability 시스템 구현 전까지는 기본 공격만 사용
  // TODO: Ability 시스템 구현 후 ShouldUseSpellAttack 로직 활성화

  // 현재는 항상 기본 공격
  return { AIDecisionType::Attack, target, {}, "", "Basic attack" };
}
