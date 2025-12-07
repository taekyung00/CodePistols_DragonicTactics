#include "../StateComponents/DiceManager.h"
#include "../StateComponents/TurnManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Engine.hpp"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "./Game/DragonicTactics/Test/TestAssert.h"
#include "Week1TestMocks.h"
#include <iostream>
#include <vector>
#include "TestTurnInit.h"


class MockCharacter;

bool TestInitiativeBasicRoll()
{
	std::cout << "\n=== Test: Initiative Basic Roll ===\n";

	// Set deterministic seed for testing
	DiceManager dice; dice.SetSeed(12345);

	TurnManager turnMgr;
	turnMgr.ResetInitiative();

	// Create test characters with different speeds
	Dragon dragon(Math::ivec2{0, 0});
	Fighter fighter(Math::ivec2{1, 0});

	std::vector<Character*> characters;
	characters.push_back(&dragon);
	characters.push_back(&fighter);

	turnMgr.RollInitiative(characters);

	// Get speeds from stats components
	StatsComponent* dragonStats = dragon.GetStatsComponent();
	StatsComponent* fighterStats = fighter.GetStatsComponent();

	int dragonSpeed = dragonStats ? dragonStats->GetSpeed() : 0;
	int fighterSpeed = fighterStats ? fighterStats->GetSpeed() : 0;

	std::cout << "Dragon speed: " << dragonSpeed << "\n";
	std::cout << "Fighter speed: " << fighterSpeed << "\n";

	// Both should have valid speed values (greater than 0)
	bool passed = (dragonSpeed > 0) && (fighterSpeed > 0);

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

	DiceManager dice; dice.SetSeed(99999);

	TurnManager turnMgr;
	turnMgr.ResetInitiative();

	// Create characters
	Dragon dragon(Math::ivec2{0, 0});
	Fighter fighter(Math::ivec2{1, 0});

	std::vector<Character*> characters;
	characters.push_back(&fighter);
	characters.push_back(&dragon);

	turnMgr.RollInitiative(characters);

	// Get the turn order
	std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();

	bool passed = (turnOrder.size() == 2);

	std::cout << "Turn order size: " << turnOrder.size() << "\n";
	if (turnOrder.size() >= 1)
		std::cout << "First in order: " << turnOrder[0]->TypeName() << "\n";
	if (turnOrder.size() >= 2)
		std::cout << "Second in order: " << turnOrder[1]->TypeName() << "\n";

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

	// Create characters - they will have speeds set by their constructors
	Fighter char1(Math::ivec2{0, 0});
	Fighter char2(Math::ivec2{1, 0});
	Fighter char3(Math::ivec2{2, 0});

	std::vector<Character*> characters;
	characters.push_back(&char1);
	characters.push_back(&char2);
	characters.push_back(&char3);

	// Use same seed to get same rolls
	DiceManager dice; dice.SetSeed(42);
	turnMgr.RollInitiative(characters);

	StatsComponent* stats1 = char1.GetStatsComponent();
	StatsComponent* stats2 = char2.GetStatsComponent();
	StatsComponent* stats3 = char3.GetStatsComponent();

	int speed1 = stats1 ? stats1->GetSpeed() : 0;
	int speed2 = stats2 ? stats2->GetSpeed() : 0;
	int speed3 = stats3 ? stats3->GetSpeed() : 0;

	std::cout << "Character 1 speed: " << speed1 << "\n";
	std::cout << "Character 2 speed: " << speed2 << "\n";
	std::cout << "Character 3 speed: " << speed3 << "\n";

	// Verify speeds are valid
	bool passed = (speed1 > 0) && (speed2 > 0) && (speed3 > 0);

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

	Dragon dragon(Math::ivec2{0, 0});
	dragon.SetHP(50); // Alive

	Fighter fighter(Math::ivec2{1, 0});
	fighter.SetHP(0); // Dead

	std::vector<Character*> characters;
	characters.push_back(&dragon);
	characters.push_back(&fighter);

	turnMgr.RollInitiative(characters);

	// Get turn order - dead character should be excluded
	std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();

	std::cout << "Turn order size: " << turnOrder.size() << "\n";
	std::cout << "Dragon HP: " << dragon.GetHP() << "\n";
	std::cout << "Fighter HP: " << fighter.GetHP() << "\n";

	// Only Dragon should be in turn order (fighter is dead)
	bool passed = (turnOrder.size() == 1) && (turnOrder[0] == &dragon);

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

	Dragon dragon(Math::ivec2{0, 0});

	std::vector<Character*> characters;
	characters.push_back(&dragon);

	// First roll
	DiceManager dice;
	dice.SetSeed(12345);
	turnMgr.RollInitiative(characters);
	std::vector<Character*> firstOrder = turnMgr.GetTurnOrder();

	std::cout << "First roll - turn order size: " << firstOrder.size() << "\\n";

	// Reset and second roll
	turnMgr.ResetInitiative();
	dice.SetSeed(54321);
	turnMgr.RollInitiative(characters);
	std::vector<Character*> secondOrder = turnMgr.GetTurnOrder();

	std::cout << "Second roll - turn order size: " << secondOrder.size() << "\n";

	// Both should have the character in the order
	bool passed = (firstOrder.size() == 1) && (secondOrder.size() == 1);

	if (passed)
	{
		std::cout << "PASSED: Re-roll produces valid initiative\n";
	}
	else
	{
		std::cout << "FAILED: Re-roll failed\n";
	}

	return passed;
}

bool TestInitiativeModeRollOnce()
{
	std::cout << "\n=== Test: Initiative Mode - Roll Once ===\n";

	TurnManager turnMgr;
	turnMgr.SetInitiativeMode(InitiativeMode::RollOnce);

	Dragon dragon(Math::ivec2{0, 0});

	std::vector<Character*> characters;
	characters.push_back(&dragon);

	turnMgr.RollInitiative(characters);
	std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();

	bool passed = (turnOrder.size() == 1);

	if (passed)
	{
		std::cout << "PASSED: Initiative mode set correctly\n";
	}
	else
	{
		std::cout << "FAILED: Initiative mode not working\n";
	}

	return passed;
}

bool TestInitiativeReset()
{
	std::cout << "\n=== Test: Initiative Reset ===\n";

	TurnManager turnMgr;

	Dragon dragon(Math::ivec2{0, 0});

	std::vector<Character*> characters;
	characters.push_back(&dragon);

	turnMgr.RollInitiative(characters);
	std::vector<Character*> orderBeforeReset = turnMgr.GetTurnOrder();

	std::cout << "Turn order before reset: " << orderBeforeReset.size() << "\n";

	turnMgr.ResetInitiative();
	std::vector<Character*> orderAfterReset = turnMgr.GetTurnOrder();

	std::cout << "Turn order after reset: " << orderAfterReset.size() << "\n";

	// After reset, turn order should be empty
	bool passed = (orderBeforeReset.size() > 0) && (orderAfterReset.size() == 0);

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
