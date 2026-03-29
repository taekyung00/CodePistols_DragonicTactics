/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Input.h"
#include "./Engine/Window.h"
#include "RenderingTest.h"

#include "./CS200/IRenderer2D.h"
#include "./CS200/NDC.h"

#include "./Game/MainMenu.h"
#include "Game/DragonicTactics/Factories/CharacterFactory.h"
#include "Game/DragonicTactics/Objects/Character.h"
#include "Game/Samurai.h"

RenderingTest::RenderingTest()
{
}

void RenderingTest::Load()
{
  AddGSComponent(new CS230::GameObjectManager());
  AddGSComponent(new CharacterFactory());
  [[maybe_unused]] CS230::GameObjectManager* go_manager		  = GetGSComponent<CS230::GameObjectManager>();
  CharacterFactory*			character_factory = GetGSComponent<CharacterFactory>();
  auto						player_ptr		  = character_factory->Create(CharacterTypes::Dragon, { 5, 5 });
  go_manager->Add(std::move(player_ptr));
  go_manager->Add(std::make_unique<Samurai>());

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

  Math::TransformationMatrix camera_matrix = CS200::build_ndc_matrix(Engine::GetWindow().GetSize());
  renderer_2d->BeginScene(camera_matrix);
  CS230::GameObjectManager* goMgr = GetGSComponent<CS230::GameObjectManager>();
  if (goMgr)
  {
	goMgr->DrawAll(Math::TransformationMatrix());
  }

  renderer_2d->EndScene();
}

void RenderingTest::DrawImGui()
{
}

void RenderingTest::Unload()
{
}