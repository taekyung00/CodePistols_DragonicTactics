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




bool Test::TestSpellRegistration() {
    SpellSystem& spellSys = Engine::GetSpellSystem();

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
	GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    SpellSystem& spellSys = Engine::GetSpellSystem();

    // Register Fireball
    spellSys.RegisterSpell("Fireball", new MockFireball());

    // Create caster with spell slots
    Character* caster = new Dragon(Math::vec2{0, 0});
    caster->SetSpellSlots({{3, 2}}); // 3 level 2 slots

    // Create target
    Character* target = new Dragon(Math::vec2{3, 3});
    grid->AddCharacter(target, Math::vec2{3, 3});
     
    // Test: Cast spell through SpellSystem
    MockSpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{3, 3});

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.total_damage > 0);
    std::cout << caster->GetSpellSlotCount(3) << " should be 1" << std::endl;
    ASSERT_TRUE(caster->GetSpellSlotCount(3) == 1); // Used 1 slot  ///////fail

    delete caster;
    delete target;
    grid->Reset();
    return true;
}

bool Test::TestSpellUpcast() {
    Character* caster = new Dragon(Math::vec2{0, 0});

    caster->SetSpellSlots({{1, 2}}); // 1 level 2 slot
    caster->SetSpellSlots({{2, 3}}); // 2 level 3 slots

    SpellSystem& spellSys = Engine::GetSpellSystem();
    spellSys.RegisterSpell("Fireball", new MockFireball());

    // Test: Upcast Fireball to level 3
    MockSpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{3, 3}, 3);

    ASSERT_TRUE(result.success);  
    std::cout << caster->GetSpellSlotCount(1) << " should be 2" << std::endl;
    ASSERT_TRUE(caster->GetSpellSlotCount(1) == 2); // Didn't use level 2 slot  
    std::cout << caster->GetSpellSlotCount(2) << " should be 3" << std::endl;
    ASSERT_TRUE(caster->GetSpellSlotCount(2) == 3); // Used level 3 slot     

    delete caster;
    return true;
}

bool Test::TestGetAvailableSpells() {
    Character* caster = new Dragon(Math::vec2{0, 0});
    caster->SetSpellSlots({{2, 1}});
    caster->SetSpellSlots({{3, 2}});

    SpellSystem& spellSys = Engine::GetSpellSystem();
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
    SpellSystem& spellSys = Engine::GetSpellSystem();
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
