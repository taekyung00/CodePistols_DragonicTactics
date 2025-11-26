#include "pch.h"

#include "../StateComponents/DiceManager.h"
#include "../StateComponents/TurnManager.h"
#include "./Engine/Engine.hpp"
#include "./Engine/GameStateManager.hpp"
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
#include "TestCombatSystem.h"


bool Test_CombatSystem_CalculateDamage()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	DiceManager dice;
	CombatSystem combat;
	combat.SetDiceManager(&dice);
	int damage = combat.CalculateDamage(&test_dragon, &test_fighter, "3d8", 5);

	ASSERT_GE(damage, 8);
	ASSERT_LE(damage, 29);

	std::cout << "Test_CombatSystem_CalculateDamage passed (damage=" << damage << ")" << std::endl;
	return true;
}

bool Test_CombatSystem_CalculateDamage_MinRoll()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });

	DiceManager dice;
	dice.SetSeed(1000);

	CombatSystem combat;
	combat.SetDiceManager(&dice);
	int minDamage = 999;
	for (int i = 0; i < 100; ++i)
	{
		int damage = combat.CalculateDamage(&test_dragon, &test_fighter, "3d8", 5);
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

	DiceManager dice;
	CombatSystem combat;
	combat.SetDiceManager(&dice);
	int maxDamage = 0;
	for (int i = 0; i < 100; ++i)
	{
		int damage = combat.CalculateDamage(&test_dragon, &test_fighter, "3d8", 5);
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

	CombatSystem combat;
	DiceManager dice;
	combat.SetDiceManager(&dice);
	combat.ApplyDamage(&test_dragon, &test_fighter, 20);

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

	CombatSystem combat;
	DiceManager dice;
	combat.SetDiceManager(&dice);
	combat.ApplyDamage(&test_dragon, &test_fighter, -10);

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

	CombatSystem combat;
	DiceManager dice;
	combat.SetDiceManager(&dice);
	bool success = combat.ExecuteAttack(&test_dragon, &test_fighter);

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

	CombatSystem combat;
	bool success = combat.ExecuteAttack(&test_dragon, &test_fighter);

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

	CombatSystem combat;
	bool success = combat.ExecuteAttack(&test_dragon, &test_fighter);

	ASSERT_FALSE(success);
	ASSERT_EQ(test_fighter.GetStatsComponent()->GetCurrentHP(), fighterHPBefore);

	std::cout << "Test_CombatSystem_ExecuteAttack_NotEnoughAP passed" << std::endl;
	return true;
}

bool Test_CombatSystem_IsInRange_Adjacent()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 0, 1 });
	CombatSystem combat;
	bool inRange = combat.IsInRange(&test_dragon, &test_fighter, 1);

	ASSERT_TRUE(inRange);

	std::cout << "Test_CombatSystem_IsInRange_Adjacent passed" << std::endl;
	return true;
}

bool Test_CombatSystem_IsInRange_TooFar()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 5, 5 });
	CombatSystem combat;
	bool inRange = combat.IsInRange(&test_dragon, &test_fighter, 1);

	ASSERT_FALSE(inRange);

	std::cout << "Test_CombatSystem_IsInRange_TooFar passed" << std::endl;
	return true;
}

bool Test_CombatSystem_GetDistance()
{
	Dragon	test_dragon({ 0, 0 });
	Fighter test_fighter({ 3, 4 });
	CombatSystem combat; int distance = combat.GetDistance(&test_dragon, &test_fighter);

	ASSERT_EQ(distance, 7);

	std::cout << "Test_CombatSystem_GetDistance passed" << std::endl;
	return true;
}


