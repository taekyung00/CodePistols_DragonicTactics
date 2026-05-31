/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "ConsoleTest.h"

#include "CS200/IRenderer2D.h"
#include "CS200/NDC.h"

#include "Game/MainMenu.h"

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
}

void ConsoleTest::Draw()
{
  Engine::GetWindow().Clear(0x1a1a1aff);
  auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();

  renderer_2d->BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

  renderer_2d->EndScene();
}

void ConsoleTest::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
  ImGui::Begin("Tests");
  ImGui::Text("All tests removed.");
  ImGui::End();
#endif
}

void ConsoleTest::Unload()
{
}
