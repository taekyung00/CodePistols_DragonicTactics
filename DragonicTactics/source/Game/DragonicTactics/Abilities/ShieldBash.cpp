#include "ShieldBash.h"
#include "../Objects/Character.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/DiceManager.h"
#include "../StateComponents/EventBus.h"
#include "../Types/Events.h"
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/GameStateManager.hpp"
#include "../Objects/Components/GridPosition.h"
#include "../Objects/Components/StatsComponent.h"
#include "../Objects/Components/ActionPoints.h"

ShieldBash::ShieldBash() {}


Math::ivec2 ShieldBash::CalculatePushDirection(Math::ivec2 userPos, Math::ivec2 targetPos) const {
    Math::ivec2 direction = targetPos - userPos;//calculate direction vec

    if (direction.x > 0) direction.x = 1;
    else if (direction.x < 0) direction.x = -1;

    if (direction.y > 0) direction.y = 1;
    else if (direction.y < 0) direction.y = -1;

    return direction;
}

bool ShieldBash::CanUse(Character* user, Character* target) const {
    if (!user || !target) return false;
    if (target->GetStatsComponent()->GetCurrentHP() <= 0) return false;
    if (user == target) return false;

    Math::ivec2 userPos = user->GetGridPosition()->Get();
    Math::ivec2 targetPos = target->GetGridPosition()->Get();
    if (!IsAdjacent(userPos, targetPos)) return false;

    if (user->GetActionPointsComponent()->GetCurrentPoints() < GetAPCost()) {
        return false;
    }
    
    //checking space there is space behind when pushed
    Math::ivec2 pushDir = CalculatePushDirection(userPos, targetPos);
    Math::ivec2 pushDestination = targetPos + pushDir;

    GridSystem* grid_system = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!grid_system->IsValidTile(pushDestination) || !grid_system->IsWalkable(pushDestination))
	{
        return false;  // Can't push if wall or off grid
    }

    return true;
}

AbilityResult ShieldBash::Use(Character* user, Character* target) {
    AbilityResult result;
    result.success = false;

    if (!CanUse(user, target)) {
        result.failureReason = "Cannot use Shield Bash on this target";
        return result;
    }

    if (!user->GetActionPointsComponent()->Consume(GetAPCost())) {
        result.failureReason = "Not enough action points";
        return result;
    }

    DiceManager* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
    int attackModifier = user->GetStatsComponent()->GetBaseAttack();
    int damage = dice->RollDiceFromString(baseDamage) + attackModifier;

    target->TakeDamage(damage, user);
    result.affectedTargets.push_back(target);
    result.damageDealt = damage;

    //pushing
    Math::ivec2 userPos = user->GetGridPosition()->Get();
    Math::ivec2 targetPos = target->GetGridPosition()->Get();
    Math::ivec2 pushDir = CalculatePushDirection(userPos, targetPos);
    Math::ivec2 newPos = targetPos + pushDir;

    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    grid->MoveCharacter(targetPos, newPos); //moving at grid
    target->GetGridPosition()->Set(newPos); //charector position

    Engine::GetGameStateManager().GetGSComponent<EventBus>()->Publish(AbilityUsedEvent{
        user, 
        target, 
        GetName(), 
        damage
    });

    Engine::GetGameStateManager().GetGSComponent<EventBus>()->Publish(CharacterMovedEvent{
        target,
        targetPos,
        newPos,
        0 //no decrease AP
    });

    Engine::GetLogger().LogEvent(
        user->TypeName() + " uses Shield Bash on " + target->TypeName() +
        " for " + std::to_string(damage) + " damage and pushes them to (" +
        std::to_string(newPos.x) + "," + std::to_string(newPos.y) + ")"
    );

    result.success = true;
    return result;
}
