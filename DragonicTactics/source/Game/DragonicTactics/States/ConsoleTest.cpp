#include "ConsoleTest.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Input.hpp"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Window.hpp"

#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"

#include "./Game/MainMenu.h"
#include "./Game/DragonicTactics/Test/TestAStar.h"
#include "./Game/DragonicTactics/Test/TestDataRegistry.h"
#include "./Game/DragonicTactics/Test/TestDiceManager.h"
#include "./Game/DragonicTactics/StateComponents/GridSystem.h"

#include <imgui.h>

bool TestAStar = false;
bool TestDataRegistry = false;
bool TestDiceManager = false;

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

	// ===== AStar Tests =====
	if (TestAStar)
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
	
	if (ImGui::Button("TestDataRegistry"))
	{
		TestDataRegistry = true;
	}
	
	if (ImGui::Button("TestDiceManager"))
	{
		TestDiceManager = true;
	}
	
	ImGui::End();
}

void ConsoleTest::Unload()
{
}
