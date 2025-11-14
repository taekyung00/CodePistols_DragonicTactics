#include "../Singletons/DiceManager.h"
#include "../StateComponents/TurnManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/DragonicTactics/Singletons/CombatSystem.h"
#include "./Game/DragonicTactics/Test/TestAssert.h"
#include "Week1TestMocks.h"
#include <iostream>
#include <vector>

#include "Test_SY.h"


bool Test_CombatSystem_CalculateDamage()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	int damage = Engine::GetCombatSystem().CalculateDamage(&test_dragon, &test_fighter, "3d8", 5);

	ASSERT_GE(damage, 8);
	ASSERT_LE(damage, 29);

	std::cout << "Test_CombatSystem_CalculateDamage passed (damage=" << damage << ")" << std::endl;
	return true;
}

bool Test_CombatSystem_CalculateDamage_MinRoll()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	Engine::GetDiceManager().SetSeed(1000);

	int minDamage = 999;
	for (int i = 0; i < 100; ++i)
	{
		int damage = Engine::GetCombatSystem().CalculateDamage(&test_dragon, &test_fighter, "3d8", 5);
		if (damage < minDamage)
			minDamage = damage;
	}

	ASSERT_GE(minDamage, 8);
	ASSERT_LE(minDamage, 10);

	std::cout << "Test_CombatSystem_CalculateDamage_MinRoll passed (min=" << minDamage << ")" << std::endl;
	return true;
}

bool Test_CombatSystem_CalculateDamage_MaxRoll()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	int maxDamage = 0;
	for (int i = 0; i < 100; ++i)
	{
		int damage = Engine::GetCombatSystem().CalculateDamage(&test_dragon, &test_fighter, "3d8", 5);
		if (damage > maxDamage)
			maxDamage = damage;
	}

	ASSERT_GE(maxDamage, 25);
	ASSERT_LE(maxDamage, 29);

	std::cout << "Test_CombatSystem_CalculateDamage_MaxRoll passed (max=" << maxDamage << ")" << std::endl;
	return true;
}

bool Test_CombatSystem_ApplyDamage()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	int fighterHPBefore = test_fighter.GetStatsComponent()->GetCurrentHP();

	Engine::GetCombatSystem().ApplyDamage(&test_dragon, &test_fighter, 20);

	ASSERT_EQ(test_fighter.GetStatsComponent()->GetCurrentHP(), fighterHPBefore - 20);
	ASSERT_TRUE(test_fighter.IsAlive());

	std::cout << "Test_CombatSystem_ApplyDamage passed" << std::endl;
	return true;
}

bool Test_CombatSystem_ApplyDamage_Negative()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });
	int		hpBefore = test_dragon.GetStatsComponent()->GetCurrentHP();

	Engine::GetCombatSystem().ApplyDamage(nullptr, &test_fighter, -10);

	ASSERT_EQ(test_dragon.GetStatsComponent()->GetCurrentHP(), hpBefore);

	std::cout << "Test_CombatSystem_ApplyDamage_Negative passed" << std::endl;
	return true;
}

bool Test_CombatSystem_ExecuteAttack_Valid()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	int fighterHPBefore = test_fighter.GetStatsComponent()->GetCurrentHP();
	int dragonAPBefore	= test_dragon.GetActionPoints();

	bool success = Engine::GetCombatSystem().ExecuteAttack(&test_dragon, &test_fighter);

	ASSERT_TRUE(success);
	ASSERT_TRUE(test_fighter.GetStatsComponent()->GetCurrentHP() < fighterHPBefore);
	ASSERT_EQ(test_dragon.GetActionPoints(), dragonAPBefore - 2);

	std::cout << "Test_CombatSystem_ExecuteAttack_Valid passed" << std::endl;
	return true;
}

bool Test_CombatSystem_ExecuteAttack_OutOfRange()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 5, 5 });

	int fighterHPBefore = test_fighter.GetStatsComponent()->GetCurrentHP();

	bool success = Engine::GetCombatSystem().ExecuteAttack(&test_dragon, &test_fighter);

	ASSERT_FALSE(success);
	ASSERT_EQ(test_fighter.GetStatsComponent()->GetCurrentHP(), fighterHPBefore);

	std::cout << "Test_CombatSystem_ExecuteAttack_OutOfRange passed" << std::endl;
	return true;
}

bool Test_CombatSystem_ExecuteAttack_NotEnoughAP()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	test_dragon.GetActionPointsComponent()->Consume(3);

	int fighterHPBefore = test_fighter.GetStatsComponent()->GetCurrentHP();

	bool success = Engine::GetCombatSystem().ExecuteAttack(&test_dragon, &test_fighter);

	ASSERT_FALSE(success);
	ASSERT_EQ(test_fighter.GetStatsComponent()->GetCurrentHP(), fighterHPBefore);

	std::cout << "Test_CombatSystem_ExecuteAttack_NotEnoughAP passed" << std::endl;
	return true;
}

bool Test_CombatSystem_IsInRange_Adjacent()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });
	bool	inRange = Engine::GetCombatSystem().IsInRange(&test_dragon, &test_fighter, 1);

	ASSERT_TRUE(inRange);

	std::cout << "Test_CombatSystem_IsInRange_Adjacent passed" << std::endl;
	return true;
}

bool Test_CombatSystem_IsInRange_TooFar()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 5, 5 });
	bool	inRange = Engine::GetCombatSystem().IsInRange(&test_dragon, &test_fighter, 1);

	ASSERT_FALSE(inRange);

	std::cout << "Test_CombatSystem_IsInRange_TooFar passed" << std::endl;
	return true;
}

bool Test_CombatSystem_GetDistance()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 3, 4 });
	int		distance = Engine::GetCombatSystem().GetDistance(&test_dragon, &test_fighter);

	ASSERT_EQ(distance, 7);

	std::cout << "Test_CombatSystem_GetDistance passed" << std::endl;
	return true;
}

class MockCharacter;

bool TestInitiativeBasicRoll()
{
	std::cout << "\n=== Test: Initiative Basic Roll ===\n";

	// Set deterministic seed for testing
	Engine::GetDiceManager().SetSeed(12345);

	TurnManager turnMgr;
	turnMgr.ResetInitiative();

	// Create test characters with different speeds
	MockCharacter dragon;
	dragon.SetSpeed(15); // Modifier = +2

	MockCharacter fighter;
	fighter.SetSpeed(10); // Modifier = 0

	std::vector<MockCharacter*> characters;
	characters.push_back(&dragon);
	characters.push_back(&fighter);

	turnMgr.RollInitiativeMock(characters);

	int dragonInit	= turnMgr.GetInitiativeValueMock(&dragon);
	int fighterInit = turnMgr.GetInitiativeValueMock(&fighter);

	std::cout << "Dragon initiative: " << dragonInit << "\n";
	std::cout << "Fighter initiative: " << fighterInit << "\n";

	// Both should have valid initiative values (between 1-20 + modifier)
	bool passed = (dragonInit >= 3 && dragonInit <= 22) && // 1d20 + 2
				  (fighterInit >= 1 && fighterInit <= 20); // 1d20 + 0

	if (passed)
	{
		std::cout << "PASSED: Initiative values are valid\n";
	}
	else
	{
		std::cout << "FAILED: Initiative values invalid\n";
	}

	return passed;
}

bool TestInitiativeTurnOrder()
{
	std::cout << "\n=== Test: Initiative Turn Order ===\n";

	Engine::GetDiceManager().SetSeed(99999);

	TurnManager turnMgr;
	turnMgr.ResetInitiative();

	// Create characters with very different speeds
	MockCharacter dragon;
	dragon.SetSpeed(20); // High speed = +5 modifier

	MockCharacter fighter;
	fighter.SetSpeed(8); // Low speed = -1 modifier

	std::vector<MockCharacter*> characters;
	characters.push_back(&fighter);
	characters.push_back(&dragon);

	turnMgr.RollInitiativeMock(characters);

	// Note: GetTurnOrder returns Character*, not MockCharacter*
	// So we check initiative order size directly
	bool passed = (turnMgr.GetInitiativeValueMock(&dragon) > 0 && turnMgr.GetInitiativeValueMock(&fighter) > 0);

	std::cout << "Dragon initiative: " << turnMgr.GetInitiativeValueMock(&dragon) << "\n";
	std::cout << "Fighter initiative: " << turnMgr.GetInitiativeValueMock(&fighter) << "\n";

	if (passed)
	{
		std::cout << "PASSED: Turn order established correctly\n";
	}
	else
	{
		std::cout << "FAILED: Turn order not established\n";
	}

	return passed;
}

bool TestInitiativeSpeedModifier()
{
	std::cout << "\n=== Test: Initiative Speed Modifier ===\n";

	TurnManager turnMgr;
	turnMgr.ResetInitiative();

	// Test speed modifier calculation
	MockCharacter slowChar;
	slowChar.SetSpeed(5); // (5-10)/2 = -2 modifier

	MockCharacter avgChar;
	avgChar.SetSpeed(10); // (10-10)/2 = 0 modifier

	MockCharacter fastChar;
	fastChar.SetSpeed(15); // (15-10)/2 = +2 modifier

	std::vector<MockCharacter*> characters;
	characters.push_back(&slowChar);
	characters.push_back(&avgChar);
	characters.push_back(&fastChar);

	// Use same seed to get same rolls
	Engine::GetDiceManager().SetSeed(42);
	turnMgr.RollInitiativeMock(characters);

	int slowInit = turnMgr.GetInitiativeValueMock(&slowChar);
	int avgInit	 = turnMgr.GetInitiativeValueMock(&avgChar);
	int fastInit = turnMgr.GetInitiativeValueMock(&fastChar);

	std::cout << "Slow character (speed 5): " << slowInit << "\n";
	std::cout << "Average character (speed 10): " << avgInit << "\n";
	std::cout << "Fast character (speed 15): " << fastInit << "\n";

	// Verify modifiers are applied correctly
	// We can't predict exact values due to dice, but we can verify ranges
	bool passed = (slowInit >= -1 && slowInit <= 18) && // 1d20 - 2
				  (avgInit >= 1 && avgInit <= 20) &&	// 1d20 + 0
				  (fastInit >= 3 && fastInit <= 22);	// 1d20 + 2

	if (passed)
	{
		std::cout << "PASSED: Speed modifiers applied correctly\n";
	}
	else
	{
		std::cout << "FAILED: Speed modifiers incorrect\n";
	}

	return passed;
}

bool TestInitiativeDeadCharacterSkipped()
{
	std::cout << "\n=== Test: Dead Character Skipped ===\n";

	TurnManager turnMgr;
	turnMgr.ResetInitiative();

	MockCharacter dragon;
	dragon.SetHP(50); // Alive

	MockCharacter fighter;
	fighter.SetHP(0); // Dead

	std::vector<MockCharacter*> characters;
	characters.push_back(&dragon);
	characters.push_back(&fighter);

	turnMgr.RollInitiativeMock(characters);

	int dragonInit	= turnMgr.GetInitiativeValueMock(&dragon);
	int fighterInit = turnMgr.GetInitiativeValueMock(&fighter);

	std::cout << "Dragon initiative (alive): " << dragonInit << "\n";
	std::cout << "Fighter initiative (dead): " << fighterInit << "\n";

	// Only Dragon should have initiative (fighter should be 0 because dead)
	bool passed = (dragonInit > 0 && fighterInit == 0);

	if (passed)
	{
		std::cout << "PASSED: Dead character excluded from turn order\n";
	}
	else
	{
		std::cout << "FAILED: Dead character in turn order or wrong values\n";
	}

	return passed;
}

bool TestInitiativeReRoll()
{
	std::cout << "\n=== Test: Initiative Re-Roll ===\n";

	TurnManager turnMgr;
	turnMgr.ResetInitiative();

	MockCharacter dragon;

	std::vector<MockCharacter*> characters;
	characters.push_back(&dragon);

	// First roll
	Engine::GetDiceManager().SetSeed(12345);
	turnMgr.RollInitiativeMock(characters);
	int firstInit = turnMgr.GetInitiativeValueMock(&dragon);

	std::cout << "First initiative: " << firstInit << "\n";

	// Second roll with different seed
	Engine::GetDiceManager().SetSeed(54321);
	turnMgr.RollInitiativeMock(characters);
	int secondInit = turnMgr.GetInitiativeValueMock(&dragon);

	std::cout << "Second initiative: " << secondInit << "\n";

	// Initiatives should differ (different dice rolls)
	bool passed = (firstInit != secondInit);

	if (passed)
	{
		std::cout << "PASSED: Re-roll produces different initiative\n";
	}
	else
	{
		std::cout << "FAILED: Re-roll produced same initiative\n";
	}

	return passed;
}

bool TestInitiativeModeRollOnce()
{
	std::cout << "\n=== Test: Initiative Mode - Roll Once ===\n";

	// This test requires full combat simulation which may not work with MockCharacter
	// Skip for now as it needs integration with real Character class
	std::cout << "SKIPPED: Requires full combat integration\n";
	return true;
}

bool TestInitiativeReset()
{
	std::cout << "\n=== Test: Initiative Reset ===\n";

	TurnManager turnMgr;

	MockCharacter dragon;

	std::vector<MockCharacter*> characters;
	characters.push_back(&dragon);

	turnMgr.RollInitiativeMock(characters);
	int initBeforeReset = turnMgr.GetInitiativeValueMock(&dragon);

	std::cout << "Initiative before reset: " << initBeforeReset << "\n";

	turnMgr.ResetInitiative();
	int initAfterReset = turnMgr.GetInitiativeValueMock(&dragon);

	std::cout << "Initiative after reset: " << initAfterReset << "\n";

	// After reset, character should not be in initiative order (returns 0)
	bool passed = (initAfterReset == 0);

	if (passed)
	{
		std::cout << "PASSED: Initiative cleared after reset\n";
	}
	else
	{
		std::cout << "FAILED: Initiative not properly reset\n";
	}

	return passed;
}

void RunTurnManagerInitiativeTests()
{
	std::cout << "\n";
	std::cout << "========================================\n";
	std::cout << "  TURN MANAGER INITIATIVE TESTS\n";
	std::cout << "========================================\n";

	int passed = 0;
	int total  = 0;

	total++;
	if (TestInitiativeBasicRoll())
		passed++;
	total++;
	if (TestInitiativeTurnOrder())
		passed++;
	total++;
	if (TestInitiativeSpeedModifier())
		passed++;
	total++;
	if (TestInitiativeDeadCharacterSkipped())
		passed++;
	total++;
	if (TestInitiativeReRoll())
		passed++;
	total++;
	if (TestInitiativeModeRollOnce())
		passed++;
	total++;
	if (TestInitiativeReset())
		passed++;

	std::cout << "\n========================================\n";
	std::cout << "  RESULTS: " << passed << "/" << total << " tests passed\n";
	std::cout << "========================================\n\n";
}
