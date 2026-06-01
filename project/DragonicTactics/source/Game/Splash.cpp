#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/
#include "CS200/IRenderer2D.h"
#include "CS200/NDC.h"
#include "CS200/RenderingAPI.h"
#include "Engine/Engine.h"
#include "Engine/GameStateManager.h"
#include "Engine/Logger.h"
#include "Engine/TextureManager.h"
#include "Engine/Window.h"
#include "MainMenu.h"
#include "Splash.h"

void Splash::Load()
{
  counter = 0.0;
  texture = Engine::GetTextureManager().Load("Assets/images/Splash/DigiPen.png");
  Math::ivec2 window_size = Engine::GetWindow().GetSize();
  Engine::GetLogger().LogDebug("Window Size: " + std::to_string(window_size.x) + ", " + std::to_string(window_size.y));

  m_cutscene_textures_.resize(CUTSCENE_COUNT);
  m_cutscene_textures_[0] = Engine::GetTextureManager().Load("Assets/images/cut1.png");
  m_cutscene_textures_[1] = Engine::GetTextureManager().Load("Assets/images/cut2.png");
  m_cutscene_textures_[2] = Engine::GetTextureManager().Load("Assets/images/cut3.png");
  m_cutscene_textures_[3] = Engine::GetTextureManager().Load("Assets/images/cut4.png");
  m_cutscene_idx_   = -1;
  m_cutscene_timer_ = 0.0;
}

void Splash::Update(double dt)
{
  dt = std::min(dt, 0.05); // 초기 로딩 스파이크로 인한 페이즈 즉시 전환 방지
#if defined(DEVELOPER_VERSION)
  constexpr double SPLASH_DURATION = 0.3;
#else
  constexpr double SPLASH_DURATION = 2.0;
#endif

  if (m_cutscene_idx_ < 0)
  {
	// DigiPen 페이즈
	counter += dt;
	if (counter >= SPLASH_DURATION)
	{
	  m_cutscene_idx_   = 0;
	  m_cutscene_timer_ = 0.0;
	}
  }
  else
  {
	// 컷신 페이즈
	auto& inp = Engine::GetInput();
	if (inp.KeyJustPressed(CS230::Input::Keys::Escape))
	{
	  Engine::GetGameStateManager().PopState();
	  Engine::GetGameStateManager().PushState<MainMenu>();
	  return;
	}
	if (inp.MouseJustPressed(0) || inp.KeyJustPressed(CS230::Input::Keys::Space))
	  m_cutscene_timer_ = CUTSCENE_DURATION;

	m_cutscene_timer_ += dt;
	if (m_cutscene_timer_ >= CUTSCENE_DURATION)
	{
	  m_cutscene_timer_ = 0.0;
	  ++m_cutscene_idx_;
	  if (m_cutscene_idx_ >= CUTSCENE_COUNT)
	  {
		Engine::GetGameStateManager().PopState();
		Engine::GetGameStateManager().PushState<MainMenu>();
	  }
	}
  }
}

void Splash::Unload()
{
}

void Splash::Draw()
{
  CS200::RenderingAPI::Clear();
  auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
  auto win = Engine::GetWindow().GetSize();
  renderer_2d->BeginScene(CS200::build_ndc_matrix(win));

  if (m_cutscene_idx_ < 0)
  {
	{
	  auto   sz = texture->GetSize();
	  double sx = static_cast<double>(win.x) / sz.x;
	  double sy = static_cast<double>(win.y) / sz.y;
	  double s  = std::min(sx, sy);
	  double bx = (win.x - sz.x * s) * 0.5;
	  double by = (win.y - sz.y * s) * 0.5;
	  texture->Draw(Math::TranslationMatrix(Math::vec2{ bx, by }) *
	                Math::ScaleMatrix(Math::vec2{ s, s }));
	}
  }
  else
  {
	auto& tex = m_cutscene_textures_[static_cast<size_t>(m_cutscene_idx_)];
	if (tex)
	{
	  auto   sz = tex->GetSize();
	  double sx = static_cast<double>(win.x) / sz.x;
	  double sy = static_cast<double>(win.y) / sz.y;
	  double s  = std::min(sx, sy);
	  double bx = (win.x - sz.x * s) * 0.5;
	  double by = (win.y - sz.y * s) * 0.5;
	  tex->Draw(Math::TranslationMatrix(Math::vec2{ bx, by }) *
	            Math::ScaleMatrix(Math::vec2{ s, s }));
	}
  }

  renderer_2d->EndScene();
}

void Splash::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
    // ... ImGui 코드
#endif
}

gsl::czstring Splash::GetName() const
{
  return "Splash";
}
