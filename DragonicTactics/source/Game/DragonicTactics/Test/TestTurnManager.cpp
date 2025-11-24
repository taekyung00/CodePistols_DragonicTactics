#include "TestTurnManager.h"
#include "Engine/GameStateManager.hpp"
#include "Engine/Engine.hpp"
#include "Game/DragonicTactics/StateComponents/TurnManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Character.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/Test/TestAssert.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
bool Test_TurnManager_Initialize()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };

    // Action
    turnMgr.InitializeTurnOrder(characters);

    // Assertions
    std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(2));
    ASSERT_EQ(turnOrder[0], static_cast<Character*>(&TestDragon));
    ASSERT_EQ(turnOrder[1], static_cast<Character*>(&TestFighter));
    ASSERT_FALSE(turnMgr.IsCombatActive());

    std::cout << "Test_TurnManager_Initialize passed" << std::endl;
    return true;
}

bool Test_TurnManager_Initialize_Empty()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    std::vector<Character*> characters = {};

    // Action
    turnMgr.InitializeTurnOrder(characters);

    // Assertions - Should handle gracefully
    std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(0));

    std::cout << "Test_TurnManager_Initialize_Empty passed" << std::endl;
    return true;
}

bool Test_TurnManager_Initialize_DeadCharacters()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });
    TestFighter.TakeDamage(TestFighter.GetStats().current_hp, nullptr);

    std::vector<Character*> characters = { &TestDragon, &TestFighter };

    // Action
    turnMgr.InitializeTurnOrder(characters);

    // Assertions - Dead characters should be removed
    std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(1));
    ASSERT_EQ(turnOrder[0], reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_Initialize_DeadCharacters passed" << std::endl;
    return true;
}

bool Test_TurnManager_StartCombat()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr.InitializeTurnOrder(characters);

    // Action
    turnMgr.StartCombat();

    // Assertions
    ASSERT_TRUE(turnMgr.IsCombatActive());
    ASSERT_EQ(turnMgr.GetCurrentTurnNumber(), 1);
    ASSERT_EQ(turnMgr.GetRoundNumber(), 1);
    ASSERT_EQ(turnMgr.GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_StartCombat passed" << std::endl;
    return true;
}

bool Test_TurnManager_EndCurrentTurn()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr.InitializeTurnOrder(characters);
    turnMgr.StartCombat();

    ASSERT_EQ(turnMgr.GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    // Action - End Dragon's turn
    turnMgr.EndCurrentTurn();

    // Assertions - Should be Fighter's turn now
    ASSERT_EQ(turnMgr.GetCurrentCharacter(), reinterpret_cast<Character*>(&TestFighter));
    ASSERT_EQ(turnMgr.GetCurrentTurnNumber(), 2);
    ASSERT_EQ(turnMgr.GetRoundNumber(), 1); // Still round 1

    std::cout << "Test_TurnManager_EndCurrentTurn passed" << std::endl;
    return true;
}

bool Test_TurnManager_RoundProgression()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr.InitializeTurnOrder(characters);
    turnMgr.StartCombat();

    // Action - Complete one full round
    ASSERT_EQ(turnMgr.GetRoundNumber(), 1);

    turnMgr.EndCurrentTurn(); // Dragon -> Fighter (still round 1)
    ASSERT_EQ(turnMgr.GetRoundNumber(), 1);

    turnMgr.EndCurrentTurn(); // Fighter -> Dragon (round 2)
    ASSERT_EQ(turnMgr.GetRoundNumber(), 2);
    ASSERT_EQ(turnMgr.GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_RoundProgression passed" << std::endl;
    return true;
}

bool Test_TurnManager_ActionPointRefresh()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon TestDragon({ 0, 0 });

    std::vector<Character*> characters = { &TestDragon };
    turnMgr.InitializeTurnOrder(characters);

    // Consume some AP
    TestDragon.GetActionPointsComponent()->Consume(2);
    ASSERT_EQ(TestDragon.GetActionPoints(), 1); // 3 - 2 = 1

    // Action - Start combat (should refresh AP)
    turnMgr.StartCombat();

    // Assertions - AP should be refreshed
    ASSERT_EQ(TestDragon.GetActionPoints(), 3); // Refreshed to max

    std::cout << "Test_TurnManager_ActionPointRefresh passed" << std::endl;
    return true;
}

bool Test_TurnManager_SkipDeadCharacter()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon                  TestDragon({ 0, 0 });
    Fighter                 TestFighter1({ 1, 1 });
    Fighter                 TestFighter2({ 2, 2 });
    std::vector<Character*> characters = { &TestDragon, &TestFighter1, &TestFighter2 };
    turnMgr.InitializeTurnOrder(characters);
    turnMgr.StartCombat();

    // Kill fighter1 during Dragon's turn
    TestFighter1.TakeDamage(TestFighter1.GetStats().current_hp, nullptr);

    // Action - End Dragon's turn
    turnMgr.EndCurrentTurn();

    // Assertions - Should skip fighter1 (dead) and go to fighter2
    ASSERT_EQ(turnMgr.GetCurrentCharacter(), reinterpret_cast<Character*>(&TestFighter2));

    std::cout << "Test_TurnManager_SkipDeadCharacter passed" << std::endl;
    return true;
}

bool Test_TurnManager_AllCharactersDead()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr.InitializeTurnOrder(characters);
    turnMgr.StartCombat();

    // Kill both characters
    TestDragon.TakeDamage(TestDragon.GetStats().current_hp, nullptr);
    TestFighter.TakeDamage(TestFighter.GetStats().current_hp, nullptr);

    // Action - Try to end turn
    turnMgr.EndCurrentTurn();

    // Assertions - Combat should end
    ASSERT_FALSE(turnMgr.IsCombatActive());

    std::cout << "Test_TurnManager_AllCharactersDead passed" << std::endl;
    return true;
}

bool Test_TurnManager_GetCharacterTurnIndex()
{
    // Setup
<<<<<<< HEAD
    EventBus eventBus;
    TurnManager turnMgr;
    turnMgr.SetEventBus(&eventBus);
=======
    TurnManager turnMgr;
>>>>>>> 61da4f8967dea2ff00ef3ca7d85a5a562ebdc8e2
    turnMgr.Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr.InitializeTurnOrder(characters);

    // Action
    int dragonIndex  = turnMgr.GetCharacterTurnIndex(&TestDragon);
    int fighterIndex = turnMgr.GetCharacterTurnIndex(&TestFighter);

    // Assertions
    ASSERT_EQ(dragonIndex, 0);
    ASSERT_EQ(fighterIndex, 1);

    std::cout << "Test_TurnManager_GetCharacterTurnIndex passed" << std::endl;
    return true;
}

void test_turnmanager_all()
{
    Test_TurnManager_Initialize();
    Test_TurnManager_Initialize_Empty();
    Test_TurnManager_Initialize_DeadCharacters();
    Test_TurnManager_StartCombat();
    Test_TurnManager_EndCurrentTurn();
    Test_TurnManager_RoundProgression();
    Test_TurnManager_ActionPointRefresh();
    Test_TurnManager_SkipDeadCharacter();
    Test_TurnManager_AllCharactersDead();
    Test_TurnManager_GetCharacterTurnIndex();
}

