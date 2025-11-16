#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Window.hpp"
#include "Test.h"

#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include "./Game/DragonicTactics/Test/Week3TestMocks.h"
#include "./Game/DragonicTactics/Test/TestAssert.h"
#include "./Game/DragonicTactics/Types/Events.h"

#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/MainMenu.h"

#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/Objects/Character.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"

// File: CS230/Game/Test/SpellSystemTests.cpp
#include "./Game/DragonicTactics/Singletons/SpellSystem.h"
#include "./Game/DragonicTactics/Singletons/AISystem.h"
#include "../Test/AITest.h"

bool Test::Test_TurnManager_Initialize()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };

    // Action
    turnMgr->InitializeTurnOrder(characters);

    // Assertions
    std::vector<Character*> turnOrder = turnMgr->GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(2));
    ASSERT_EQ(turnOrder[0], static_cast<Character*>(&TestDragon));
    ASSERT_EQ(turnOrder[1], static_cast<Character*>(&TestFighter));
    ASSERT_FALSE(turnMgr->IsCombatActive());

    std::cout << "Test_TurnManager_Initialize passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_Initialize_Empty()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    std::vector<Character*> characters = {};

    // Action
    turnMgr->InitializeTurnOrder(characters);

    // Assertions - Should handle gracefully
    std::vector<Character*> turnOrder = turnMgr->GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(0));

    std::cout << "Test_TurnManager_Initialize_Empty passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_Initialize_DeadCharacters()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });
    TestFighter.TakeDamage(TestFighter.GetStats().current_hp, nullptr);

    std::vector<Character*> characters = { &TestDragon, &TestFighter };

    // Action
    turnMgr->InitializeTurnOrder(characters);

    // Assertions - Dead characters should be removed
    std::vector<Character*> turnOrder = turnMgr->GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(1));
    ASSERT_EQ(turnOrder[0], reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_Initialize_DeadCharacters passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_StartCombat()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr->InitializeTurnOrder(characters);

    // Action
    turnMgr->StartCombat();

    // Assertions
    ASSERT_TRUE(turnMgr->IsCombatActive());
    ASSERT_EQ(turnMgr->GetCurrentTurnNumber(), 1);
    ASSERT_EQ(turnMgr->GetRoundNumber(), 1);
    ASSERT_EQ(turnMgr->GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_StartCombat passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_EndCurrentTurn()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr->InitializeTurnOrder(characters);
    turnMgr->StartCombat();

    ASSERT_EQ(turnMgr->GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    // Action - End Dragon's turn
    turnMgr->EndCurrentTurn();

    // Assertions - Should be Fighter's turn now
    ASSERT_EQ(turnMgr->GetCurrentCharacter(), reinterpret_cast<Character*>(&TestFighter));
    ASSERT_EQ(turnMgr->GetCurrentTurnNumber(), 2);
    ASSERT_EQ(turnMgr->GetRoundNumber(), 1); // Still round 1

    std::cout << "Test_TurnManager_EndCurrentTurn passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_RoundProgression()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr->InitializeTurnOrder(characters);
    turnMgr->StartCombat();

    // Action - Complete one full round
    ASSERT_EQ(turnMgr->GetRoundNumber(), 1);

    turnMgr->EndCurrentTurn(); // Dragon -> Fighter (still round 1)
    ASSERT_EQ(turnMgr->GetRoundNumber(), 1);

    turnMgr->EndCurrentTurn(); // Fighter -> Dragon (round 2)
    ASSERT_EQ(turnMgr->GetRoundNumber(), 2);
    ASSERT_EQ(turnMgr->GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_RoundProgression passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_ActionPointRefresh()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon TestDragon({ 0, 0 });

    std::vector<Character*> characters = { &TestDragon };
    turnMgr->InitializeTurnOrder(characters);

    // Consume some AP
    TestDragon.GetActionPointsComponent()->Consume(2);
    ASSERT_EQ(TestDragon.GetActionPoints(), 1); // 3 - 2 = 1

    // Action - Start combat (should refresh AP)
    turnMgr->StartCombat();

    // Assertions - AP should be refreshed
    ASSERT_EQ(TestDragon.GetActionPoints(), 3); // Refreshed to max

    std::cout << "Test_TurnManager_ActionPointRefresh passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_SkipDeadCharacter()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon                  TestDragon({ 0, 0 });
    Fighter                 TestFighter1({ 1, 1 });
    Fighter                 TestFighter2({ 2, 2 });
    std::vector<Character*> characters = { &TestDragon, &TestFighter1, &TestFighter2 };
    turnMgr->InitializeTurnOrder(characters);
    turnMgr->StartCombat();

    // Kill fighter1 during Dragon's turn
    TestFighter1.TakeDamage(TestFighter1.GetStats().current_hp, nullptr);

    // Action - End Dragon's turn
    turnMgr->EndCurrentTurn();

    // Assertions - Should skip fighter1 (dead) and go to fighter2
    ASSERT_EQ(turnMgr->GetCurrentCharacter(), reinterpret_cast<Character*>(&TestFighter2));

    std::cout << "Test_TurnManager_SkipDeadCharacter passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_AllCharactersDead()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr->InitializeTurnOrder(characters);
    turnMgr->StartCombat();

    // Kill both characters
    TestDragon.TakeDamage(TestDragon.GetStats().current_hp, nullptr);
    TestFighter.TakeDamage(TestFighter.GetStats().current_hp, nullptr);

    // Action - Try to end turn
    turnMgr->EndCurrentTurn();

    // Assertions - Combat should end
    ASSERT_FALSE(turnMgr->IsCombatActive());

    std::cout << "Test_TurnManager_AllCharactersDead passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_GetCharacterTurnIndex()
{
    // Setup
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    turnMgr->Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    turnMgr->InitializeTurnOrder(characters);

    // Action
    int dragonIndex  = turnMgr->GetCharacterTurnIndex(&TestDragon);
    int fighterIndex = turnMgr->GetCharacterTurnIndex(&TestFighter);

    // Assertions
    ASSERT_EQ(dragonIndex, 0);
    ASSERT_EQ(fighterIndex, 1);

    std::cout << "Test_TurnManager_GetCharacterTurnIndex passed" << std::endl;
    return true;
}






// File: CS230/Game/Test/FighterAITests.cpp

bool AITest::TestAITargetsClosestEnemy() {
    // Test: AI should target closest enemy
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    grid->Reset();

    Fighter testfighter({ 1, 1 });
    testfighter.SetGridPosition({1, 1});
    grid->AddCharacter(&testfighter, Math::vec2{1, 1});
    Dragon testdragon({ 3, 3 });
    testdragon.SetGridPosition({3, 3}); // Distance = 4
     grid->AddCharacter(&testdragon, Math::vec2{3, 3});

    AISystem& ai = AISystem::GetInstance();
    Character* target = ai.AssessThreats(&testfighter);

    bool passed = (target == &testdragon);

    if (!passed) {
        std::cout << "  FAILED: AI didn't target Dragon\n";
    }

    return passed;
}

bool AITest::TestAIMovesCloserWhenOutOfRange() {
    // Test: AI should move closer if target out of range
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    grid->Reset();

    Fighter testfighter({1,1});
    testfighter.SetGridPosition({1, 1});
    grid->AddCharacter(&testfighter, Math::vec2{1, 1});
    testfighter.SetAttackRange(1); // Melee

    Dragon testdragon({2,2});
    testdragon.SetGridPosition({6, 6}); // Far away
    grid->AddCharacter(&testdragon, Math::vec2{6, 6});

    AISystem& ai = AISystem::GetInstance();
    AIDecision decision = ai.MakeDecision(&testfighter);

    bool passed = (decision.type == AIDecisionType::Move);

    if (!passed) {
        std::cout << "  FAILED: AI didn't move closer (decision: "
                  << (int)decision.type << ")\n";
    }

    return passed;
}

bool AITest::TestAIAttacksWhenInRange() {
    // Test: AI should attack if target in range
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    grid->Reset();

    Fighter testfighter({1,1});
    testfighter.SetGridPosition({4, 4});
    grid->AddCharacter(&testfighter, Math::vec2{4, 4});
    testfighter.SetActionPoints(10); // Enough for attack
    testfighter.SetAttackRange(1);

    Dragon testdragon({2,2});
    testdragon.SetGridPosition({4, 5}); // Adjacent (distance = 1)
    grid->AddCharacter(&testdragon, Math::vec2{4, 5});

    AISystem& ai = AISystem::GetInstance();
    AIDecision decision = ai.MakeDecision(&testfighter);

    bool passed = (decision.type == AIDecisionType::Attack ||
                   decision.type == AIDecisionType::UseAbility);

    if (!passed) {
        std::cout << "  FAILED: AI didn't attack when in range\n";
    }

    return passed;
}

bool AITest::TestAIUsesShieldBashWhenAdjacent() {
    // Test: AI should use Shield Bash when adjacent to healthy target
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    grid->Reset();

    Fighter testfighter({1,1});
    testfighter.SetGridPosition({4, 4});
    grid->AddCharacter(&testfighter, Math::vec2{4, 4});
    testfighter.SetActionPoints(10);
    //fighter.EnableAbility("Shield Bash"); // Ability available

    Dragon testdragon({2,2});
    testdragon.SetGridPosition({4, 5}); // Adjacent
    grid->AddCharacter(&testdragon, Math::vec2{4, 5});
    testdragon.SetHP(testdragon.GetMaxHP()); // Full HP

    AISystem& ai = AISystem::GetInstance();
    bool shouldUse = ai.ShouldUseAbility(&testfighter, &testdragon);

    bool passed = shouldUse;

    if (!passed) {
        std::cout << "  FAILED: AI didn't use Shield Bash when appropriate\n";
    }

    return passed;
}

bool AITest::TestAIEndsTurnWhenNoActions() {
    // Test: AI should end turn if no valid actions
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    grid->Reset();

    Fighter testfighter({1,1});
    testfighter.SetGridPosition({1, 1});
    grid->AddCharacter(&testfighter, Math::vec2{1, 1});
    testfighter.SetActionPoints(0); // No action points

    Dragon testdragon({2,2});
    testdragon.SetGridPosition({2, 2}); // Far away
    grid->AddCharacter(&testdragon, Math::vec2{2, 2});

    AISystem& ai = AISystem::GetInstance();
    AIDecision decision = ai.MakeDecision(&testfighter);

    bool passed = (decision.type == AIDecisionType::EndTurn);

    if (!passed) {
        std::cout << "  FAILED: AI didn't end turn when no actions available\n";
    }

    return passed;
}

void AITest::RunFighterAITests() {
    std::cout << "\n=== FIGHTER AI TESTS ===\n";
    std::cout << (TestAITargetsClosestEnemy() ? "O" : "X") << " AI targets closest enemy\n";
    std::cout << (TestAIMovesCloserWhenOutOfRange() ? "O" : "X") << " AI moves closer when out of range\n";
    std::cout << (TestAIAttacksWhenInRange() ? "O" : "X") << " AI attacks when in range\n";
    std::cout << (TestAIUsesShieldBashWhenAdjacent() ? "O" : "X") << " AI uses Shield Bash appropriately\n";
    std::cout << (TestAIEndsTurnWhenNoActions() ? "O" : "X") << " AI ends turn when no actions\n";
    std::cout << "==========================\n";
}

void Test::test_turnmanager_all()
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


