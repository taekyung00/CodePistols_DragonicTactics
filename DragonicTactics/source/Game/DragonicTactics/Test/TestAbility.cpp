#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/GameStateManager.hpp"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/Abilities/AbilityBase.h"
#include "Game/DragonicTactics/Abilities/MeleeAttack.h"
#include "Game/DragonicTactics/Abilities/ShieldBash.h"
#include "Game/DragonicTactics/Objects/Character.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
void test_MeleeAttack_WithGrid()
{
    Fighter* fighter = new Fighter({1,1});
    Dragon* dragon = new Dragon({3,3});
	Engine::GetLogger().LogEvent("========== Grid Test - Melee Attack ==========");
	if (fighter == nullptr || dragon == nullptr)
	{
		Engine::GetLogger().LogError("Fighter or Dragon is null!");
		return;
	}


	Math::ivec2 fighterPos = fighter->GetGridPosition()->Get();
	Math::ivec2 dragonPos  = dragon->GetGridPosition()->Get();

	Engine::GetLogger().LogEvent("Fighter at (" + std::to_string(fighterPos.x) + "," + std::to_string(fighterPos.y) + ")");
	Engine::GetLogger().LogEvent("Dragon at (" + std::to_string(dragonPos.x) + "," + std::to_string(dragonPos.y) + ")");

	int initialHP = dragon->GetStatsComponent()->GetCurrentHP();
	fighter->GetActionPointsComponent()->Refresh();

	AbilityResult result = fighter->PerformMeleeAttack(dragon);

	if (result.success)
	{
		int finalHP = dragon->GetStatsComponent()->GetCurrentHP();
		Engine::GetLogger().LogEvent("SUCCESS! Damage: " + std::to_string(result.damageDealt));
		Engine::GetLogger().LogEvent("Dragon HP: " + std::to_string(initialHP) + " -> " + std::to_string(finalHP));
	}
	else
	{
		Engine::GetLogger().LogError("FAILED: " + result.failureReason);
	}
}

void test_ShieldBash_WithGrid()
{
    Fighter* fighter = new Fighter({1,1});
    Dragon* dragon = new Dragon({3,3});
	Engine::GetLogger().LogEvent("========== Grid Test - Shield Bash ==========");
	if (fighter == nullptr || dragon == nullptr)
	{
		Engine::GetLogger().LogError("Fighter or Dragon is null!");
		return;
	}


	Math::ivec2 initialPos = dragon->GetGridPosition()->Get();
	int			initialHP  = dragon->GetStatsComponent()->GetCurrentHP();

	Engine::GetLogger().LogEvent("Before: Dragon at (" + std::to_string(initialPos.x) + "," + std::to_string(initialPos.y) + ")");

	fighter->GetActionPointsComponent()->Refresh();
	AbilityResult result = fighter->PerformShieldBash(dragon);

	if (result.success)
	{
		Math::ivec2 finalPos = dragon->GetGridPosition()->Get();
		int			finalHP	 = dragon->GetStatsComponent()->GetCurrentHP();

		Engine::GetLogger().LogEvent("SUCCESS! Damage: " + std::to_string(result.damageDealt));
		Engine::GetLogger().LogEvent("Dragon HP: " + std::to_string(initialHP) + " -> " + std::to_string(finalHP));
		Engine::GetLogger().LogEvent(
			"Dragon pushed: (" + std::to_string(initialPos.x) + "," + std::to_string(initialPos.y) + ") -> (" + std::to_string(finalPos.x) + "," + std::to_string(finalPos.y) + ")");

		// update dragon visual position
		dragon->SetPosition({ static_cast<double>(finalPos.x * GridSystem::TILE_SIZE), static_cast<double>(finalPos.y * GridSystem::TILE_SIZE) });
	}
	else
	{
		Engine::GetLogger().LogError("FAILED: " + result.failureReason);
	}
}

void test_ShieldBash_IntoWall()
{
    Fighter* fighter = new Fighter({1,1});
    Dragon* dragon = new Dragon({3,3});
	Engine::GetLogger().LogEvent("========== Grid Test - Shield Bash Into Wall ==========");
	if (fighter == nullptr || dragon == nullptr)
	{
		Engine::GetLogger().LogError("Fighter or Dragon is null!");
		return;
	}

	GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

	// Ginam
	Math::ivec2 dragonPos  = { 2, 6 };
	Math::ivec2 fighterPos = { 3, 5 };

	// Ginam
	Math::ivec2 oldDragonPos  = dragon->GetGridPosition()->Get();
	Math::ivec2 oldFighterPos = fighter->GetGridPosition()->Get();

	grid->MoveCharacter(oldDragonPos, dragonPos);
	grid->MoveCharacter(oldFighterPos, fighterPos);

 //   dragon->SetPosition(dragonPos);
	//fighter->SetPosition(fighterPos);

	dragon->GetGridPosition()->Set(dragonPos);
	fighter->GetGridPosition()->Set(fighterPos);

	dragon->SetPosition({ static_cast<double>(dragonPos.x * GridSystem::TILE_SIZE), static_cast<double>(dragonPos.y * GridSystem::TILE_SIZE) });
	fighter->SetPosition({ static_cast<double>(fighterPos.x * GridSystem::TILE_SIZE), static_cast<double>(fighterPos.y * GridSystem::TILE_SIZE) });

	Engine::GetLogger().LogEvent("Setup: Dragon at (" + std::to_string(dragonPos.x) + "," + std::to_string(dragonPos.y) + ")");
	Engine::GetLogger().LogEvent("Setup: Fighter at (" + std::to_string(fighterPos.x) + "," + std::to_string(fighterPos.y) + ")");
	Engine::GetLogger().LogEvent("Wall should be at (1, 6) - Dragon should NOT be pushable!");

	fighter->GetActionPointsComponent()->Refresh();
	AbilityResult result = fighter->PerformShieldBash(dragon);

	if (result.success)
	{
		Engine::GetLogger().LogError("UNEXPECTED: Shield Bash succeeded (should have failed due to wall)");
	}
	else
	{
		Engine::GetLogger().LogEvent("CORRECT: Shield Bash failed as expected!");
		Engine::GetLogger().LogEvent("Reason: " + result.failureReason);
	}
}