#include "AITest.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Input.hpp"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Window.hpp"

#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"

#include "./Game/MainMenu.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"

bool testAi = true;

AITest::AITest()
{
}

void AITest::Load()
{
	
}

void AITest::Update([[maybe_unused]] double dt)
{
    
	if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
	{
		Engine::GetGameStateManager().PopState();
		Engine::GetGameStateManager().PushState<MainMenu>();
	}
    if(testAi) {
        AddGSComponent(new GridSystem());
        //AddGSComponent(new AISystem());
        RunFighterAITests();
        testAi = false;
        RemoveGSComponent<GridSystem>();
    }
}

void AITest::Draw()
{
	Engine::GetWindow().Clear(0x1a1a1aff);
}

void AITest::DrawImGui()
{
}

void AITest::Unload()
{
}