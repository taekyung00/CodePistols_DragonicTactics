#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Window.hpp"
#include "Test.h"

#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include "./Game/DragonicTactics/Types/Events.h"

#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/MainMenu.h"

#include "../Singletons/DataRegistry.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"

#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"

Test::Test() : fighter(nullptr), dragon(nullptr)
{
}

void Test::Load()
{
    DataRegistry::Instance().LoadFromFile("Assets/Data/characters.json");
    AddGSComponent(new CS230::GameObjectManager());
    fighter = new Fighter({ 5, 5 });
    GetGSComponent<CS230::GameObjectManager>()->Add(fighter);
    dragon = new Dragon({ 6, 5 });
    GetGSComponent<CS230::GameObjectManager>()->Add(dragon);
    Engine::GetLogger().LogEvent("========== Combat Testbed Initialized ==========");
    Engine::GetLogger().LogEvent("'T' -> Fighter's Turn | 'Y' -> Dragon's Turn | 'D' -> Damage Dragon | 'H' -> Heal Fighter");
    LogFighterStatus();
    LogDragonStatus();
}
void Test::Draw()
{
    Engine::GetWindow().Clear(0x1a1a1aff);
    auto& renderer_2d = Engine::GetRenderer2D();
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    renderer_2d.DrawCircle(Math::TranslationMatrix(Math::ivec2{ 100, 100 }) * Math::ScaleMatrix(30.0));

    renderer_2d.EndScene();
}

void Test::DrawImGui()
{
}

gsl::czstring Test::GetName() const
{
    return "Test";
}
void Test2::Draw()
{
    Engine::GetWindow().Clear(0x1a1a1aff);
    auto& renderer_2d = Engine::GetRenderer2D();
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    if (grid_system != nullptr)
    {
        grid_system->Draw();
    }
    renderer_2d.EndScene();
}
void Test2::Update([[maybe_unused]] double dt)
{
	// Test 1 - Melee Attack on grid (press '1')
	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::One))
	{
		test_MeleeAttack_WithGrid();
	}

	// Test 2 - Shield Bash on grid (press '2')
	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Two))
	{
		test_ShieldBash_WithGrid();
	}

	// Test 3 - Shield Bash into wall (press '3')
	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Three))
	{
		test_ShieldBash_IntoWall();
	}
    if (dragon != nullptr && dragon->IsAlive())
    {
        Math::ivec2 current_pos    = dragon->GetGridPosition()->Get();
        Math::ivec2 target_pos     = current_pos;
        bool        move_requested = false;

        if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
        {
            target_pos.y++;
            move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Down))
        {
            target_pos.y--;
            move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Left))
        {
            target_pos.x--;
            move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Right))
        {
            target_pos.x++;
            move_requested = true;
        }

        if (move_requested)
        {
            GridSystem* grid = GetGSComponent<GridSystem>();
            if (grid != nullptr && grid->IsWalkable(target_pos))
            {
                grid->MoveCharacter(current_pos, target_pos);

                dragon->GetGridPosition()->Set(target_pos);

                dragon->SetPosition({ static_cast<double>(target_pos.x * GridSystem::TILE_SIZE), static_cast<double>(target_pos.y * GridSystem::TILE_SIZE) });

                Engine::GetLogger().LogEvent("Dragon moved to (" + std::to_string(target_pos.x) + ", " + std::to_string(target_pos.y) + ")");
                LogDragonStatus();
            }
            else
            {
                Engine::GetLogger().LogEvent("Dragon cannot move there! (Wall or Occupied)");
            }
        }
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}
void Test2::Load()
{
    AddGSComponent(new CS230::GameObjectManager());

    AddGSComponent(new GridSystem());
    CS230::GameObjectManager* go_manager = GetGSComponent<CS230::GameObjectManager>();

    GridSystem* grid_system = GetGSComponent<GridSystem>();

    const std::vector<std::string> map_data = { "wwwwwwww", "weeeeeew", "wefeeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weedeeew", "wwwwwwww" };


    for (int y = 0; y < map_data.size(); ++y)
    {
        for (int x = 0; x < map_data[y].length(); ++x)
        {
            char tile_char = map_data[y][x];

            Math::ivec2 current_pos = { x, static_cast<int>(map_data.size()) - 1 - y };

            switch (tile_char)
            {
                case 'w': grid_system->SetTileType(current_pos, GridSystem::TileType::Wall); break;
                case 'e': grid_system->SetTileType(current_pos, GridSystem::TileType::Empty); break;
                case 'f':
                    grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
                    fighter = new Fighter(current_pos);
                    go_manager->Add(fighter);
                    grid_system->AddCharacter(fighter, current_pos);
                    break;
                case 'd':
                    grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
                    dragon = new Dragon(current_pos);
                    go_manager->Add(dragon);
                    grid_system->AddCharacter(dragon, current_pos);
                    break;
            }
        }
    }
}

void Test::test_json()
{
    Engine::GetLogger().LogEvent("========== JSON Test ==========");

    DataRegistry& registry = DataRegistry::Instance();

    int dragonHP  = registry.GetValue<int>("Dragon.maxHP", 0);
    int fighterHP = registry.GetValue<int>("Fighter.maxHP", 0);

    Engine::GetLogger().LogDebug("Dragon HP: " + std::to_string(dragonHP));
    Engine::GetLogger().LogDebug("Fighter HP: " + std::to_string(fighterHP));

    bool hasRogue = registry.HasKey("Rogue.maxHP");
    Engine::GetLogger().LogDebug("Has Rogue: " + std::string(hasRogue ? "Yes" : "No"));
}

void Test::test_json_reload()
{
    Engine::GetLogger().LogEvent("--- Reloading characters.json ---");
    DataRegistry::Instance().ReloadFile("Assets/Data/characters.json");
    Engine::GetLogger().LogEvent("Reload complete!");
}

void Test::test_json_log()
{
    DataRegistry::Instance().LogAllKeys();
}

void Test::test_dice_manager()
{
    DiceManager& dice = DiceManager::Instance();
    dice.SetSeed(42);
    dice.RollDiceFromString("4d8+2");
}

void Test::test_EventData_MultiplePublishes()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    std::vector<int> damages;
    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) { damages.push_back(e.damageAmount); });

    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 20, 70, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 30, 40, nullptr, true });

    ASSERT_EQ(static_cast<int>(damages.size()), 3);
    ASSERT_EQ(damages[0], 10);
    ASSERT_EQ(damages[1], 20);
    ASSERT_EQ(damages[2], 30);
}

// Ginam
void Test2::test_MeleeAttack_WithGrid()
{
	Engine::GetLogger().LogEvent("========== Grid Test - Melee Attack ==========");
	if (fighter == nullptr || dragon == nullptr)
	{
		Engine::GetLogger().LogError("Fighter or Dragon is null!");
		return;
	}


	Math::ivec2 fighterPos = fighter->GetGridPosition()->Get();
	Math::ivec2 dragonPos  = dragon->GetGridPosition()->Get();

	Engine::GetLogger().LogEvent("Fighter at (" + std::to_string(fighterPos.x) + "," + std::to_string(fighterPos.y) + ")");
	Engine::GetLogger().LogEvent("Dragon at (" + std::to_string(dragonPos.x) + "," + std::to_string(dragonPos.y) + ")");

	int initialHP = dragon->GetStatsComponent()->GetCurrentHP();
	fighter->GetActionPointsComponent()->Refresh();

	AbilityResult result = fighter->PerformMeleeAttack(dragon);

	if (result.success)
	{
		int finalHP = dragon->GetStatsComponent()->GetCurrentHP();
		Engine::GetLogger().LogEvent("SUCCESS! Damage: " + std::to_string(result.damageDealt));
		Engine::GetLogger().LogEvent("Dragon HP: " + std::to_string(initialHP) + " -> " + std::to_string(finalHP));
	}
	else
	{
		Engine::GetLogger().LogError("FAILED: " + result.failureReason);
	}
}

void Test2::test_ShieldBash_WithGrid()
{
	Engine::GetLogger().LogEvent("========== Grid Test - Shield Bash ==========");
	if (fighter == nullptr || dragon == nullptr)
	{
		Engine::GetLogger().LogError("Fighter or Dragon is null!");
		return;
	}


	Math::ivec2 initialPos = dragon->GetGridPosition()->Get();
	int			initialHP  = dragon->GetStatsComponent()->GetCurrentHP();

	Engine::GetLogger().LogEvent("Before: Dragon at (" + std::to_string(initialPos.x) + "," + std::to_string(initialPos.y) + ")");

	fighter->GetActionPointsComponent()->Refresh();
	AbilityResult result = fighter->PerformShieldBash(dragon);

	if (result.success)
	{
		Math::ivec2 finalPos = dragon->GetGridPosition()->Get();
		int			finalHP	 = dragon->GetStatsComponent()->GetCurrentHP();

		Engine::GetLogger().LogEvent("SUCCESS! Damage: " + std::to_string(result.damageDealt));
		Engine::GetLogger().LogEvent("Dragon HP: " + std::to_string(initialHP) + " -> " + std::to_string(finalHP));
		Engine::GetLogger().LogEvent(
			"Dragon pushed: (" + std::to_string(initialPos.x) + "," + std::to_string(initialPos.y) + ") -> (" + std::to_string(finalPos.x) + "," + std::to_string(finalPos.y) + ")");

		// update dragon visual position
		dragon->SetPosition({ static_cast<double>(finalPos.x * GridSystem::TILE_SIZE), static_cast<double>(finalPos.y * GridSystem::TILE_SIZE) });
	}
	else
	{
		Engine::GetLogger().LogError("FAILED: " + result.failureReason);
	}
}

void Test2::test_ShieldBash_IntoWall()
{
	Engine::GetLogger().LogEvent("========== Grid Test - Shield Bash Into Wall ==========");
	if (fighter == nullptr || dragon == nullptr)
	{
		Engine::GetLogger().LogError("Fighter or Dragon is null!");
		return;
	}

	GridSystem* grid = GetGSComponent<GridSystem>();

	// Ginam
	Math::ivec2 dragonPos  = { 2, 6 };
	Math::ivec2 fighterPos = { 3, 5 };

	// Ginam
	Math::ivec2 oldDragonPos  = dragon->GetGridPosition()->Get();
	Math::ivec2 oldFighterPos = fighter->GetGridPosition()->Get();

	grid->MoveCharacter(oldDragonPos, dragonPos);
	grid->MoveCharacter(oldFighterPos, fighterPos);

 //   dragon->SetPosition(dragonPos);
	//fighter->SetPosition(fighterPos);

	dragon->GetGridPosition()->Set(dragonPos);
	fighter->GetGridPosition()->Set(fighterPos);

	dragon->SetPosition({ static_cast<double>(dragonPos.x * GridSystem::TILE_SIZE), static_cast<double>(dragonPos.y * GridSystem::TILE_SIZE) });
	fighter->SetPosition({ static_cast<double>(fighterPos.x * GridSystem::TILE_SIZE), static_cast<double>(fighterPos.y * GridSystem::TILE_SIZE) });

	Engine::GetLogger().LogEvent("Setup: Dragon at (" + std::to_string(dragonPos.x) + "," + std::to_string(dragonPos.y) + ")");
	Engine::GetLogger().LogEvent("Setup: Fighter at (" + std::to_string(fighterPos.x) + "," + std::to_string(fighterPos.y) + ")");
	Engine::GetLogger().LogEvent("Wall should be at (1, 6) - Dragon should NOT be pushable!");

	fighter->GetActionPointsComponent()->Refresh();
	AbilityResult result = fighter->PerformShieldBash(dragon);

	if (result.success)
	{
		Engine::GetLogger().LogError("UNEXPECTED: Shield Bash succeeded (should have failed due to wall)");
	}
	else
	{
		Engine::GetLogger().LogEvent("CORRECT: Shield Bash failed as expected!");
		Engine::GetLogger().LogEvent("Reason: " + result.failureReason);
	}
}