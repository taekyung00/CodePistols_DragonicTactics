#include "RenderingTest.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Input.hpp"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Window.hpp"

#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"

#include "./Game/MainMenu.h"

RenderingTest::RenderingTest()
{
}

void RenderingTest::Load()
{
	
}

void RenderingTest::Update([[maybe_unused]] double dt)
{
	if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
	{
		Engine::GetGameStateManager().PopState();
		Engine::GetGameStateManager().PushState<MainMenu>();
	}
}

void RenderingTest::Draw()
{
	Engine::GetWindow().Clear(0x1a1a1aff);
	auto& renderer_2d = Engine::GetRenderer2D();

	renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));


	renderer_2d.EndScene();
}

void RenderingTest::DrawImGui()
{
}

void RenderingTest::Unload()
{
}