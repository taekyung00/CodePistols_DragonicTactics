/**
 * \file
 * \author Ginam Park
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "AbilityBase.h"

class ShieldBash : public AbilityBase
{
  public:
  ShieldBash();
  ~ShieldBash() override = default;

  // Ability info
  std::string GetName() const override
  {
	return "Shield Bash";
  }

  AbilityType GetAbilityType() const override
  {
	return AbilityType::SpecialAbility;
  }

  int GetAPCost() const override
  {
	return 2;
  }

  int GetRange() const override
  {
	return 1;
  }

  std::string GetDescription() const override
  {
	return "Bash adjacent enemy with shield, dealing 1d8 + STR damage and pushing them back 1 tile.";
  }

  bool CanUse(Character* user, Character* target) const override;

  AbilityResult Use(Character* user, Character* target) override;

  private:
  const std::string baseDamage	 = "1d8";
  [[maybe_unused]] const int pushDistance = 1;

  // calculate how many tile will pushed
  Math::ivec2 CalculatePushDirection(Math::ivec2 userPos, Math::ivec2 targetPos) const;
};
