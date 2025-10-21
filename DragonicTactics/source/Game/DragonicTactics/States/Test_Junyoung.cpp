#include "Test.h"
#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Window.hpp"

#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
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

#include "../StateComponents/TurnManager.h"
#include "../StateComponents/GridSystem.h"

bool Test::Test_TurnManager_Initialize() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter({1, 1});

    std::vector<Character*> characters = {&TestDragon, &TestFighter};

    // Action
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Assertions
    std::vector<Character*> turnOrder = TurnManager::Instance().GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(2)); 
    ASSERT_EQ(turnOrder[0], static_cast<Character*>(&TestDragon));
    ASSERT_EQ(turnOrder[1], static_cast<Character*>(&TestFighter));
    ASSERT_FALSE(TurnManager::Instance().IsCombatActive());

    std::cout << "Test_TurnManager_Initialize passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_Initialize_Empty() {
    // Setup
    TurnManager::Instance().Reset();
    std::vector<Character*> characters = {};

    // Action
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Assertions - Should handle gracefully
    std::vector<Character*> turnOrder = TurnManager::Instance().GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(0));

    std::cout << "Test_TurnManager_Initialize_Empty passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_Initialize_DeadCharacters() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter({1, 1});   
    // auto& eventbus = Engine::GetEventBus();
    // eventbus.Subscribe<CharacterDeathEvent>([]([[maybe_unused]] const CharacterDeathEvent& e) { 
    //     e.character->TakeDamage(e.character->GetStats().current_hp, e.killer);
    // });
    // //fighter.SetHP(0);  // Fighter is dead
    // CharacterDeathEvent event{&TestFighter, &TestDragon};
    // eventbus.Publish(event);
    TestFighter.TakeDamage(TestFighter.GetStats().current_hp, nullptr);

    std::vector<Character*> characters = {&TestDragon, &TestFighter};

    // Action
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Assertions - Dead characters should be removed
    std::vector<Character*> turnOrder = TurnManager::Instance().GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(1));
    ASSERT_EQ(turnOrder[0], reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_Initialize_DeadCharacters passed" << std::endl;
    return true;
}
bool Test::Test_TurnManager_StartCombat() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter({1, 1});   

    std::vector<Character*> characters = {&TestDragon, &TestFighter};
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Action
    TurnManager::Instance().StartCombat();

    // Assertions
    ASSERT_TRUE(TurnManager::Instance().IsCombatActive());
    ASSERT_EQ(TurnManager::Instance().GetCurrentTurnNumber(), 1);
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_StartCombat passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_EndCurrentTurn() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter({1, 1});   

    std::vector<Character*> characters = {&TestDragon, &TestFighter};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    // Action - End Dragon's turn
    TurnManager::Instance().EndCurrentTurn();

    // Assertions - Should be Fighter's turn now
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestFighter));
    ASSERT_EQ(TurnManager::Instance().GetCurrentTurnNumber(), 2);
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);  // Still round 1

    std::cout << "Test_TurnManager_EndCurrentTurn passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_RoundProgression() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter({1, 1});   

    std::vector<Character*> characters = {&TestDragon, &TestFighter};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    // Action - Complete one full round
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);

    TurnManager::Instance().EndCurrentTurn();  // Dragon -> Fighter (still round 1)
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);

    TurnManager::Instance().EndCurrentTurn();  // Fighter -> Dragon (round 2)
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 2);
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_RoundProgression passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_ActionPointRefresh() {  ////!!!!!!!!!!!!1111
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});

    std::vector<Character*> characters = {&TestDragon};
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Consume some AP
    TestDragon.GetActionPointsComponent()->Consume(2);
    ASSERT_EQ(TestDragon.GetActionPoints(), 1);  // 5 - 3 = 2

    // Action - Start combat (should refresh AP)
    TurnManager::Instance().StartCombat();

    // Assertions - AP should be refreshed
    ASSERT_EQ(TestDragon.GetActionPoints(), 3);  // Refreshed to max

    std::cout << "Test_TurnManager_ActionPointRefresh passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_SkipDeadCharacter() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter1({1, 1});   
    Fighter TestFighter2({2, 2});   
    std::vector<Character*> characters = {&TestDragon, &TestFighter1, &TestFighter2};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    // Kill fighter1 during Dragon's turn
    TestFighter1.TakeDamage(TestFighter1.GetStats().current_hp, nullptr);
    //
    // Action - End Dragon's turn
    TurnManager::Instance().EndCurrentTurn();

    // Assertions - Should skip fighter1 (dead) and go to fighter2
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestFighter2));

    std::cout << "Test_TurnManager_SkipDeadCharacter passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_AllCharactersDead() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter({1, 1});   

    std::vector<Character*> characters = {&TestDragon, &TestFighter};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    // Kill both characters
    // auto& eventbus = Engine::GetEventBus();
    // eventbus.Subscribe<CharacterDeathEvent>([]([[maybe_unused]] const CharacterDeathEvent& e) { 
    //     e.character->TakeDamage(e.character->GetStats().current_hp, e.killer);
    // });
    TestDragon.TakeDamage(TestDragon.GetStats().current_hp, nullptr);
    TestFighter.TakeDamage(TestFighter.GetStats().current_hp, nullptr);

    // Action - Try to end turn
    TurnManager::Instance().EndCurrentTurn();

    // Assertions - Combat should end
    ASSERT_FALSE(TurnManager::Instance().IsCombatActive());

    std::cout << "Test_TurnManager_AllCharactersDead passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_GetCharacterTurnIndex() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({0, 0});
    Fighter TestFighter({1, 1});  

    std::vector<Character*> characters = {&TestDragon, &TestFighter};
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Action
    int dragonIndex = TurnManager::Instance().GetCharacterTurnIndex(&TestDragon);
    int fighterIndex = TurnManager::Instance().GetCharacterTurnIndex(&TestFighter);

    // Assertions
    ASSERT_EQ(dragonIndex, 0);
    ASSERT_EQ(fighterIndex, 1);

    std::cout << "Test_TurnManager_GetCharacterTurnIndex passed" << std::endl;
    return true;
}
