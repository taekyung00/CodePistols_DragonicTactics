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