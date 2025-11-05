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

bool Test::Test_TurnManager_Initialize()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };

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

bool Test::Test_TurnManager_Initialize_Empty()
{
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

bool Test::Test_TurnManager_Initialize_DeadCharacters()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });
    // auto& eventbus = Engine::GetEventBus();
    // eventbus.Subscribe<CharacterDeathEvent>([]([[maybe_unused]] const CharacterDeathEvent& e) {
    //     e.character->TakeDamage(e.character->GetStats().current_hp, e.killer);
    // });
    // //fighter.SetHP(0);  // Fighter is dead
    // CharacterDeathEvent event{&TestFighter, &TestDragon};
    // eventbus.Publish(event);
    TestFighter.TakeDamage(TestFighter.GetStats().current_hp, nullptr);

    std::vector<Character*> characters = { &TestDragon, &TestFighter };

    // Action
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Assertions - Dead characters should be removed
    std::vector<Character*> turnOrder = TurnManager::Instance().GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), static_cast<size_t>(1));
    ASSERT_EQ(turnOrder[0], reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_Initialize_DeadCharacters passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_StartCombat()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
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

bool Test::Test_TurnManager_EndCurrentTurn()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    // Action - End Dragon's turn
    TurnManager::Instance().EndCurrentTurn();

    // Assertions - Should be Fighter's turn now
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestFighter));
    ASSERT_EQ(TurnManager::Instance().GetCurrentTurnNumber(), 2);
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1); // Still round 1

    std::cout << "Test_TurnManager_EndCurrentTurn passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_RoundProgression()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    // Action - Complete one full round
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);

    TurnManager::Instance().EndCurrentTurn(); // Dragon -> Fighter (still round 1)
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);

    TurnManager::Instance().EndCurrentTurn(); // Fighter -> Dragon (round 2)
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 2);
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), reinterpret_cast<Character*>(&TestDragon));

    std::cout << "Test_TurnManager_RoundProgression passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_ActionPointRefresh()
{ ////!!!!!!!!!!!!1111
    // Setup
    TurnManager::Instance().Reset();
    Dragon TestDragon({ 0, 0 });

    std::vector<Character*> characters = { &TestDragon };
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Consume some AP
    TestDragon.GetActionPointsComponent()->Consume(2);
    ASSERT_EQ(TestDragon.GetActionPoints(), 1); // 5 - 3 = 2

    // Action - Start combat (should refresh AP)
    TurnManager::Instance().StartCombat();

    // Assertions - AP should be refreshed
    ASSERT_EQ(TestDragon.GetActionPoints(), 3); // Refreshed to max

    std::cout << "Test_TurnManager_ActionPointRefresh passed" << std::endl;
    return true;
}

bool Test::Test_TurnManager_SkipDeadCharacter()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon                  TestDragon({ 0, 0 });
    Fighter                 TestFighter1({ 1, 1 });
    Fighter                 TestFighter2({ 2, 2 });
    std::vector<Character*> characters = { &TestDragon, &TestFighter1, &TestFighter2 };
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

bool Test::Test_TurnManager_AllCharactersDead()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
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

bool Test::Test_TurnManager_GetCharacterTurnIndex()
{
    // Setup
    TurnManager::Instance().Reset();
    Dragon  TestDragon({ 0, 0 });
    Fighter TestFighter({ 1, 1 });

    std::vector<Character*> characters = { &TestDragon, &TestFighter };
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Action
    int dragonIndex  = TurnManager::Instance().GetCharacterTurnIndex(&TestDragon);
    int fighterIndex = TurnManager::Instance().GetCharacterTurnIndex(&TestFighter);

    // Assertions
    ASSERT_EQ(dragonIndex, 0);
    ASSERT_EQ(fighterIndex, 1);

    std::cout << "Test_TurnManager_GetCharacterTurnIndex passed" << std::endl;
    return true;
}





bool Test::TestSpellRegistration() {
    SpellSystem& spellSys = SpellSystem::GetInstance();

    // Test: Register a spell
    MockFireball* fireball = new MockFireball();
    spellSys.RegisterSpell("Fireball", fireball);

    // Test: Retrieve the spell
    MockSpellBase* retrieved = spellSys.GetSpell("Fireball");
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_TRUE(retrieved->GetName() == "Fireball");

    return true;
}

bool Test::TestSpellCasting() {
    GridSystem& grid = GridSystem::GetInstance();

    SpellSystem& spellSys = SpellSystem::GetInstance();

    // Register Fireball
    spellSys.RegisterSpell("Fireball", new MockFireball());

    // Create caster with spell slots
    Character* caster = new Dragon(Math::vec2{0, 0});
    caster->SetSpellSlots({{3, 2}}); // 3 level 2 slots

    // Create target
    Character* target = new Dragon(Math::vec2{3, 3});
    grid.AddCharacter(target, Math::vec2{3, 3});

    // Test: Cast spell through SpellSystem
    MockSpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{3, 3});

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.total_damage > 0);
    std::cout << caster->GetSpellSlotCount(3) << " should be 1" << std::endl;
    ASSERT_TRUE(caster->GetSpellSlotCount(3) == 1); // Used 1 slot  ///////fail

    delete caster;
    delete target;
    grid.Clear();
    return true;
}

bool Test::TestSpellUpcast() {
    Character* caster = new Dragon(Math::vec2{0, 0});
    caster->SetSpellSlots({{1, 2}}); // 1 level 2 slot
    caster->SetSpellSlots({{2, 3}}); // 2 level 3 slots

    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new MockFireball());

    // Test: Upcast Fireball to level 3
    MockSpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{3, 3}, 3);

    ASSERT_TRUE(result.success);    //fail//fail//fail
    std::cout << caster->GetSpellSlotCount(1) << " should be 2" << std::endl;
    ASSERT_TRUE(caster->GetSpellSlotCount(1) == 2); // Didn't use level 2 slot   //fail//fail//fail
    std::cout << caster->GetSpellSlotCount(2) << " should be 3" << std::endl;
    ASSERT_TRUE(caster->GetSpellSlotCount(2) == 3); // Used level 3 slot     //fail//fail//fail

    delete caster;
    return true;
}

bool Test::TestGetAvailableSpells() {
    Character* caster = new Dragon(Math::vec2{0, 0});
    caster->SetSpellSlots({{2, 1}});
    caster->SetSpellSlots({{3, 2}});

    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new MockFireball());  // Level 2
    spellSys.RegisterSpell("CreateWall", new MockCreateWall());  // Level 1

    // Test: Get available spells
    std::vector<std::string> available = spellSys.GetAvailableSpells(caster);

    ASSERT_TRUE(available.size() == 2);
    ASSERT_TRUE(std::find(available.begin(), available.end(), "Fireball") != available.end());
    ASSERT_TRUE(std::find(available.begin(), available.end(), "CreateWall") != available.end());

    delete caster;
    return true;
}

bool Test::TestPreviewSpellArea() {
    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new MockFireball());

    // Test: Preview Fireball area
    std::vector<Math::vec2> tiles = spellSys.PreviewSpellArea("Fireball", Math::vec2{4, 4});

    ASSERT_TRUE(tiles.size() > 0);
    // Fireball has radius 2, so should affect multiple tiles
    ASSERT_TRUE(tiles.size() >= 9);  // At least 3x3 area

    return true;
}

void Test::RunSpellSystemTests() {
    std::cout << "\n=== SpellSystem Tests ===\n";
    std::cout << (TestSpellRegistration() ? "O" : "X") << " Spell Registration\n";
    std::cout << (TestSpellCasting() ? "O" : "X") << " Spell Casting\n";
    std::cout << (TestSpellUpcast() ? "O" : "X") << " Spell Upcasting\n";
    std::cout << (TestGetAvailableSpells() ? "O" : "X") << " Get Available Spells\n";
    std::cout << (TestPreviewSpellArea() ? "O" : "X") << " Preview Spell Area\n";
}

void Test::test_turnmanager_all()
{
    Test_TurnManager_Initialize();
    Test_TurnManager_Initialize_Empty();
    Test_TurnManager_Initialize_DeadCharacters(); ///////////////////
    Test_TurnManager_StartCombat();
    Test_TurnManager_EndCurrentTurn();
    Test_TurnManager_RoundProgression();
    Test_TurnManager_ActionPointRefresh(); ///////
    Test_TurnManager_SkipDeadCharacter();
    Test_TurnManager_AllCharactersDead();
    Test_TurnManager_GetCharacterTurnIndex();
    TestSpellRegistration();
    TestSpellCasting();
    TestSpellUpcast();
    TestGetAvailableSpells();
    TestPreviewSpellArea();
    RunSpellSystemTests();
}


void Test::test_spellsystem_all()
{
    TestSpellRegistration();
    TestSpellCasting();
    TestSpellUpcast();
    TestGetAvailableSpells();
    TestPreviewSpellArea();
    RunSpellSystemTests();
}
