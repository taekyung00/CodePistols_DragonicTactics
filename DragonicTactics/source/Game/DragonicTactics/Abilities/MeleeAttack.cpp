#include "MeleeAttack.h"
#include "pch.h"

#include "../Objects/Character.h"
#include "../StateComponents/DiceManager.h"
#include "../StateComponents/EventBus.h"

#include "../Objects/Components/ActionPoints.h"
#include "../Objects/Components/GridPosition.h"
#include "../Objects/Components/StatsComponent.h"

MeleeAttack::MeleeAttack()
{
}

bool MeleeAttack::CanUse(Character* user, Character* target) const
{
  // checking null
  if (!user || !target)
  {
	return false;
  }

  // checking is target alive?
  if (target->GetStatsComponent()->GetCurrentHP() <= 0)
  {
	return false;
  }

  // checking does target is near?
  Math::ivec2 userPos	= user->GetGridPosition()->Get();
  Math::ivec2 targetPos = target->GetGridPosition()->Get();
  if (!IsAdjacent(userPos, targetPos))
  {
	return false;
  }

  // checking is enough AP cost have?
  if (user->GetActionPointsComponent()->GetCurrentPoints() < GetAPCost())
  {
	return false;
  }

  // blocking attacking self
  if (user == target)
  {
	return false;
  }

  return true;
}

AbilityResult MeleeAttack::Use(Character* user, Character* target)
{
  AbilityResult result;
  result.success = false;

  // double checking
  if (!CanUse(user, target))
  {
	result.failureReason = "Cannot use Melee Attack on this target";
	return result;
  }

  // using Ap
  if (!user->GetActionPointsComponent()->Consume(GetAPCost()))
  {
	result.failureReason = "Not enough action points";
	return result;
  }

  // rolldice
  DiceManager* dice			  = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
  std::string  attackRoll	  = user->GetStatsComponent()->GetAttackDice(); // "2d6"
  int		   baseDamage	  = dice->RollDiceFromString(attackRoll);		// 2~12
  int		   attackModifier = user->GetStatsComponent()->GetBaseAttack(); // ex) +3
  int		   totalDamage	  = baseDamage + attackModifier;				// 5~15

  // get damage to target
  target->TakeDamage(totalDamage, user);
  result.affectedTargets.push_back(target);
  result.damageDealt = totalDamage;

  // event

  Engine::GetGameStateManager().GetGSComponent<EventBus>()->Publish(AbilityUsedEvent{ user, target, GetName(), totalDamage });

  // Log output
  Engine::GetLogger().LogEvent(
	user->TypeName() + " attacks " + target->TypeName() + " for " + std::to_string(totalDamage) + " damage " + "(" + attackRoll + " + " + std::to_string(attackModifier) + ")");

  result.success = true;
  return result;
}
