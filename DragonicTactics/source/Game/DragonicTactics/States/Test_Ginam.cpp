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