#include "pch.h"

#include "ConsoleTest.h"


#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"

#include "Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/Test/TestAStar.h"
#include "Game/DragonicTactics/Test/TestEventBus.h"
#include "Game/DragonicTactics/Test/TestSpellSystem.h"
#include "Game/DragonicTactics/Test/TestCombatSystem.h"
#include "Game/DragonicTactics/Test/TestDataRegistry.h"
#include "Game/DragonicTactics/Test/TestDiceManager.h"
#include "Game/DragonicTactics/Test/TestTurnInit.h"
#include "Game/DragonicTactics/Test/TestTurnManager.h"
#include "Game/DragonicTactics/Test/TestAI.h"
#include "Game/DragonicTactics/Test/TestNew.h"
#include "Game/MainMenu.h"


bool TestAStar	  = false;
bool TestEventBus = false;
bool TestSpellSystem = false;
bool TestDataRegistry = false;
bool TestDiceManager = false;
bool TestCombatSystem = false;
bool TestTrunManager = false;
bool TestAI = false;
bool TestNewFile = false;

ConsoleTest::ConsoleTest()
{
}

void ConsoleTest::Load()
{
}

void ConsoleTest::Update([[maybe_unused]] double dt)
{
	if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
	{
		Engine::GetGameStateManager().PopState();
		Engine::GetGameStateManager().PushState<MainMenu>();
	}

	if (TestAStar) // astar test
	{
		AddGSComponent(new GridSystem());

		TestPathfindingStraightLine();
		TestPathfindingAroundObstacle();
		TestPathfindingNoPath();
		TestPathfindingAlreadyAtGoal();


		TestPathfindingInvalidStart();
		TestPathfindingInvalidGoal();
		TestPathfindingUnwalkableGoal();

		RemoveGSComponent<GridSystem>();

		TestAStar = false;
	}

	if (TestEventBus) // EventBus test
	{
		test_subscribe_publish_singleSubscriber();
		test_multiple_subscribers_sameEvent();
		test_multiple_different_events();
		test_EventData_CompleteTransfer();
		test_EventData_MultiplePublishes();

		TestEventBus = false;
	}

	if(TestSpellSystem){
		AddGSComponent(new GridSystem());

		RunSpellSystemTests();

		RemoveGSComponent<GridSystem>();

		TestSpellSystem = false;
	}
	// ===== DataRegistry Tests =====
	if (TestDataRegistry)
	{
		Engine::GetLogger().LogEvent("========== DataRegistry Tests ==========");

		// Basic Loading Tests
		TestDataRegistry_LoadJSON();
		TestDataRegistry_GetValue();
		TestDataRegistry_HasKey();
		
		// Character Data Tests
		TestDataRegistry_GetCharacterData();
		TestDataRegistry_GetArray();
		TestDataRegistry_DragonStats();
		TestDataRegistry_FighterStats();

		// Hot-Reload Tests
		TestDataRegistry_ReloadAll();
		TestDataRegistry_FileModificationDetection();

		// Validation Tests
		TestDataRegistry_ValidateCharacterJSON();
		TestDataRegistry_InvalidJSONHandling();

		Engine::GetLogger().LogEvent("========== All DataRegistry Tests Complete ==========");
		TestDataRegistry = false;
	}

	// ===== DiceManager Tests =====
	if (TestDiceManager)
	{
		Engine::GetLogger().LogEvent("========== DiceManager Tests ==========");

		// Basic Dice Rolling Tests
		TestDiceManager_RollDice();
		TestDiceManager_RollMultipleDice();
		TestDiceManager_RollFromString();

		// Dice String Parsing Tests
		TestDiceManager_ParseSimpleDice();
		TestDiceManager_ParseDiceWithModifier();
		TestDiceManager_ParseInvalidString();

		// Seed and Determinism Tests
		TestDiceManager_SetSeed();
		TestDiceManager_DeterministicRolls();
		TestDiceManager_DifferentSeeds();

		// Range Tests
		TestDiceManager_RollRange();
		TestDiceManager_MaxRoll();
		TestDiceManager_MinRoll();

		Engine::GetLogger().LogEvent("========== All DiceManager Tests Complete ==========");
		TestDiceManager = false;
	}

	if (TestCombatSystem)
    {
        Test_CombatSystem_CalculateDamage();
        Test_CombatSystem_CalculateDamage_MinRoll();
        Test_CombatSystem_CalculateDamage_MaxRoll();
        Test_CombatSystem_ApplyDamage();
        Test_CombatSystem_ApplyDamage_Negative();
        Test_CombatSystem_ExecuteAttack_Valid();
        Test_CombatSystem_ExecuteAttack_OutOfRange();
        Test_CombatSystem_ExecuteAttack_NotEnoughAP();
        Test_CombatSystem_IsInRange_Adjacent();
        Test_CombatSystem_IsInRange_TooFar();
        Test_CombatSystem_GetDistance();

        // Run all initiative tests
        //RunTurnManagerInitiativeTests();

		TestCombatSystem = false;
    }
	if(TestTrunManager)
	{
		test_turnmanager_all();
		TestTrunManager = false;
	}

	if(TestAI) {
        AddGSComponent(new GridSystem());
        //AddGSComponent(new AISystem());
        RunFighterAITests();
        TestAI = false;
        RemoveGSComponent<GridSystem>();
    }

	if(TestNewFile)
	{
		TestNewFunction();
		TestNewFile = false;
	}
}

void ConsoleTest::Draw()
{
	Engine::GetWindow().Clear(0x1a1a1aff);
	auto& renderer_2d = Engine::GetRenderer2D();

	renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));


	renderer_2d.EndScene();
}

void ConsoleTest::DrawImGui()
{
	ImGui::Begin("Tests");
	if (ImGui::Button("TestAStar"))
	{
		TestAStar = true;
	}
	if (ImGui::Button("TestEventBus"))
	{
		TestEventBus = true;
	}
	if (ImGui::Button("TestSpellSystem"))
	{
		TestSpellSystem = true;
	}

	if (ImGui::Button("TestCombatSystem"))
	{
		TestCombatSystem = true;
	}
	if (ImGui::Button("TestDiceManager"))
	{
		TestDiceManager = true;
	}
	if (ImGui::Button("TestDataRegistry"))
	{
		TestDataRegistry = true;
	}

	if (ImGui::Button("TestTrunManager"))
	{
		TestTrunManager = true;
	}

	if (ImGui::Button("TestAI"))
	{
		TestAI = true;
	}
	if (ImGui::Button("TestNewFile"))
	{
		TestNewFile = true;
	}
	
	ImGui::End();
}

void ConsoleTest::Unload()
{
}
