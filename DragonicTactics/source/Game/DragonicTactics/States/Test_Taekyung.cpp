#include "Test.h"
#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Window.hpp"

#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include "./Game/DragonicTactics/Test/TestAssert.h"
#include "./Game/DragonicTactics/Types/Events.h"

#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/MainMenu.h"

#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../Singletons/DataRegistry.h"
#include "Game/DragonicTactics/Singletons/CombatSystem.h"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"

#include "Game/DragonicTactics/Test/TestTurnInit.h"
#include "Game/DragonicTactics/Test/TestCombatSystem.h"



void Test::Update([[maybe_unused]] double dt)
{



    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::T))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'T': Starting Fighter's Turn ---");
        fighter->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Y))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'Y': Starting Dragon's Turn ---");
        dragon->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::D))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'D': Testing PerformAttack ---");
        Engine::GetCombatSystem().ExecuteAttack(dragon, fighter);
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::H))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'H': Applying 10 Heal to Fighter ---");
        fighter->ReceiveHeal(10);
        LogFighterStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::S))
    { // junyoung
        GridSystem grid;

        // Should NOT crash, should log error instead
        grid.SetTileType(Math::vec2{ -1, 0 }, GridSystem::TileType::Wall);    // Invalid
        grid.SetTileType(Math::vec2{ 8, 8 }, GridSystem::TileType::Wall);     // Invalid
        grid.SetTileType(Math::vec2{ 100, 100 }, GridSystem::TileType::Wall); // Invalid

        // Verify valid tiles unaffected
        ASSERT_EQ(grid.GetTileType(Math::vec2{ 0, 0 }), GridSystem::TileType::Empty);
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::P))
    { // junyoung
        // test_spellsystem_all();
        test_turnmanager_all();
        
        //test_turnmanager_all();
    }


    
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}








