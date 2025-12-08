#pragma once
#include "AbilityBase.h"

class MeleeAttack : public AbilityBase
{
  public:
  MeleeAttack();
  ~MeleeAttack() override = default;

  // Ability info
  std::string GetName() const override
  {
	return "Melee Attack";
  }

  AbilityType GetAbilityType() const override
  {
	return AbilityType::BasicAttack;
  }

  int GetAPCost() const override
  {
	return 1;
  } // 1 AP to attack

  int GetRange() const override
  {
	return 1;
  } // Melee range (adjacent)

  std::string GetDescription() const override
  {
	return "Make a melee weapon attack against an adjacent enemy. Deals 2d6 + STR modifier damage.";
  }

  // Ability validation
  // Reason: Check if attack is valid before executing
  bool CanUse(Character* user, Character* target) const override;

  // Ability execution
  // Reason: Perform the attack
  AbilityResult Use(Character* user, Character* target) override;
};
