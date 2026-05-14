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
#include "Engine/Particle.h"
#include "Game/Particles.h"
#include "RenderingTest.h"

#include "./CS200/IRenderer2D.h"
#include "./CS200/NDC.h"

#include "./Game/MainMenu.h"
#include "Game/DragonicTactics/Factories/CharacterFactory.h"
#include "Game/DragonicTactics/Objects/Character.h"
#include <numbers>

RenderingTest::RenderingTest()
{
}

void RenderingTest::Load()
{
	AddGSComponent(new CS230::GameObjectManager());
	AddGSComponent(new CharacterFactory());
	[[maybe_unused]] CS230::GameObjectManager* go_manager		 = GetGSComponent<CS230::GameObjectManager>();
	CharacterFactory*						   character_factory = GetGSComponent<CharacterFactory>();
	dragon														 = character_factory->Create(CharacterTypes::Dragon, { 5, 5 });
	dragon->SetPosition({ 0.0, 0.0 });
	fighter = character_factory->Create(CharacterTypes::Fighter, { 5, 5 });
	AddGSComponent(new CS230::ParticleManager<Particles::Hit>());
	testTexture = Engine::GetTextureManager().Load("Assets/images/dragon.png");
	timer = timer_max;
}

void RenderingTest::Update([[maybe_unused]] double dt)
{
	dragon->Update(dt);
	fighter->Update(dt);
	if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
	{
		Engine::GetGameStateManager().PopState();
		Engine::GetGameStateManager().PushState<MainMenu>();
	}
	timer -= static_cast<float>(dt);

  
  CS230::GameObjectManager* goMgr		 = GetGSComponent<CS230::GameObjectManager>();
  goMgr->UpdateAll(dt);
	if (timer <= 0.f)
	{
		timer				  = timer_max;
		auto* particleManager = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Hit>>();
		if (particleManager)
		{
			particleManager->Emit(5, { 50.0, 50.0 }, { 0, 0 }, { 0, 100 }, 3.14159265f / 2.0f);
			Engine::GetLogger().LogEvent("Emitted particles at (50,50)");
		}
	}
}

void RenderingTest::Draw()
{
	Engine::GetWindow().Clear(0x1a1a1aff);
	auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();

	Math::TransformationMatrix camera_matrix = CS200::build_ndc_matrix(Engine::GetWindow().GetSize());
	renderer_2d->BeginScene(camera_matrix);
	dragon->Draw(Math::TranslationMatrix(Math::to_vec2(translate)) * Math::RotationMatrix(static_cast<double>(rotate / 180 * std::numbers::pi_v<float>)) * Math::ScaleMatrix(Math::to_vec2(scale)));
	fighter->Draw(Math::TransformationMatrix{});
	CS230::GameObjectManager* goMgr = GetGSComponent<CS230::GameObjectManager>();
	if (goMgr)
	{
		goMgr->DrawAll(Math::TransformationMatrix{});
	}
	if (testTexture)
	{
		testTexture->Draw(Math::TranslationMatrix(Math::vec2{ 400.0, 300.0 }) );
	}

	renderer_2d->EndScene();
}

void RenderingTest::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
	if (ImGui::Begin("Texture Controls"))
	{
		ImGui::SliderFloat("Scale X", &(scale.x), -20.f, 20.0f, "%.1f px/s");
		ImGui::SliderFloat("Scale Y", &(scale.y), -20.f, 20.0f, "%.1f px/s");
		ImGui::SliderFloat("Rotate", &rotate, 0.f, 360.0f, "%.1f px/s");
		ImGui::SliderFloat("Translate X", &(translate.x), -(static_cast<float>(window_size.x) - 100.f), static_cast<float>(window_size.x) - 100.f, "%.1f px/s");
		ImGui::SliderFloat("Translate Y", &(translate.y), -(static_cast<float>(window_size.y) - 100.f), static_cast<float>(window_size.y) - 100.f, "%.1f px/s");
	}
	ImGui::End();
#endif
}

void RenderingTest::Unload()
{
}