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

void Test::LogFighterStatus()
{
    if (fighter == nullptr)
        return;

    GridPosition*   grid_pos = fighter->GetGridPosition();
    StatsComponent* stats    = fighter->GetStatsComponent();
    ActionPoints*   ap       = fighter->GetActionPointsComponent();

    Engine::GetLogger().LogDebug("========== Fighter Status Report ==========");

    if (grid_pos != nullptr)
    {
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    }
    if (stats != nullptr)
    {
        std::string hp_status = "HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP());
        hp_status += " (" + std::to_string(stats->GetHealthPercentage() * 100.0f) + "%)";
        Engine::GetLogger().LogDebug(hp_status);
        Engine::GetLogger().LogDebug("IsAlive: " + std::string(stats->IsAlive() ? "true" : "false"));
        Engine::GetLogger().LogDebug("Speed: " + std::to_string(stats->GetSpeed()));
    }
    if (ap != nullptr)
    {
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    }

    Engine::GetLogger().LogDebug("==========================================");
}

void Test::LogDragonStatus()
{
    if (dragon == nullptr)
        return;
    Engine::GetLogger().LogDebug("========== Dragon Status Report ==========");

    GridPosition*   grid_pos = dragon->GetGridPosition();
    StatsComponent* stats    = dragon->GetStatsComponent();
    ActionPoints*   ap       = dragon->GetActionPointsComponent();
    SpellSlots*     ss       = dragon->GetSpellSlots();

    if (grid_pos != nullptr)
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    if (stats != nullptr)
        Engine::GetLogger().LogDebug("HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP()));
    if (ap != nullptr)
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    if (ss != nullptr)
        Engine::GetLogger().LogDebug("Has Lvl 5 Slot: " + std::string(ss->HasSlot(5) ? "Yes" : "No"));

    Engine::GetLogger().LogDebug("==========================================");
}

void Test::Unload()
{
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    fighter = nullptr;
    dragon  = nullptr;
}

//-----------------------------------------------------------------------------------

Test2::Test2() : fighter(nullptr), dragon(nullptr)
{
}

void Test2::Load()
{
    AddGSComponent(new CS230::GameObjectManager());

    AddGSComponent(new GridSystem());
    CS230::GameObjectManager* go_manager = GetGSComponent<CS230::GameObjectManager>();

    GridSystem* grid_system = GetGSComponent<GridSystem>();

    const std::vector<std::string> map_data = { "wwwwwwww", "weefeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weedeeew", "wwwwwwww" };


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

void Test2::Update([[maybe_unused]] double dt)
{
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

void Test2::LogFighterStatus()
{
    if (fighter == nullptr)
        return;

    GridPosition*   grid_pos = fighter->GetGridPosition();
    StatsComponent* stats    = fighter->GetStatsComponent();
    ActionPoints*   ap       = fighter->GetActionPointsComponent();

    Engine::GetLogger().LogDebug("========== Fighter Status Report ==========");

    if (grid_pos != nullptr)
    {
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    }
    if (stats != nullptr)
    {
        std::string hp_status = "HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP());
        hp_status += " (" + std::to_string(stats->GetHealthPercentage() * 100.0f) + "%)";
        Engine::GetLogger().LogDebug(hp_status);
        Engine::GetLogger().LogDebug("IsAlive: " + std::string(stats->IsAlive() ? "true" : "false"));
        Engine::GetLogger().LogDebug("Speed: " + std::to_string(stats->GetSpeed()));
    }
    if (ap != nullptr)
    {
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    }

    Engine::GetLogger().LogDebug("==========================================");
}

void Test2::LogDragonStatus()
{
    if (dragon == nullptr)
        return;
    Engine::GetLogger().LogDebug("========== Dragon Status Report ==========");

    GridPosition*   grid_pos = dragon->GetGridPosition();
    StatsComponent* stats    = dragon->GetStatsComponent();
    ActionPoints*   ap       = dragon->GetActionPointsComponent();
    SpellSlots*     ss       = dragon->GetSpellSlots();

    if (grid_pos != nullptr)
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    if (stats != nullptr)
        Engine::GetLogger().LogDebug("HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP()));
    if (ap != nullptr)
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    if (ss != nullptr)
        Engine::GetLogger().LogDebug("Has Lvl 5 Slot: " + std::string(ss->HasSlot(5) ? "Yes" : "No"));

    Engine::GetLogger().LogDebug("==========================================");
}

void Test2::Unload()
{
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    fighter = nullptr;
    dragon  = nullptr;
}

void Test2::DrawImGui()
{
}

gsl::czstring Test2::GetName() const
{
    return "Test2";
}