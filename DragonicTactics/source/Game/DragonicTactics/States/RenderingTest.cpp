#include "pch.h"

#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Input.h"
#include "./Engine/Window.h"
#include "RenderingTest.h"

#include "./CS200/IRenderer2D.h"
#include "./CS200/NDC.h"

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
  auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();

  renderer_2d->BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));


  renderer_2d->EndScene();
}

void RenderingTest::DrawImGui()
{
}

void RenderingTest::Unload()
{
}