/**
 * \file
 * \author Ginam Park
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "../../../Engine/Vec2.h"
#include <string>
#include <vector>

class Character;

struct AbilityResult
{ // about ability's result. is success? who affected? totaldamage. why failed?
  bool					  success;
  std::vector<Character*> affectedTargets; // maybe changed
  int					  damageDealt;
  std::string			  failureReason;
};


enum class AbilityType
{ // what kind of attack used?
  BasicAttack,
  SpecialAbility,
  BonusAction
};

class AbilityBase
{
  public:
  virtual ~AbilityBase() = default;

  virtual std::string GetName() const		 = 0;
  virtual AbilityType GetAbilityType() const = 0;
  virtual int		  GetAPCost() const		 = 0;
  virtual int		  GetRange() const		 = 0;
  virtual std::string GetDescription() const = 0;

  // checking abil can use?
  virtual bool CanUse(Character* user, Character* target) const = 0;

  // using abil
  virtual AbilityResult Use(Character* user, Character* target) = 0;

  protected:
  // Check if target is adjacent
  bool IsAdjacent(Math::ivec2 pos1, Math::ivec2 pos2) const
  {
	int dx = abs(pos2.x - pos1.x);
	int dy = abs(pos2.y - pos1.y);
	return (dx <= 1 && dy <= 1 && (dx + dy) > 0); // Adjacent, not same tile
  }
};
