#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameCursor.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    June 9, 2026
*/
#include "GameCursor.h"
#include "Engine/Engine.h"
#include "Engine/Input.h"
#include "Engine/Matrix.h"
#include "Engine/TextureManager.h"
#include "Engine/Window.h"
#include "Game/DragonicTactics/States/GamePlay.h"
#include <memory>

namespace
{
  std::shared_ptr<CS230::Texture> s_tex_normal;
  std::shared_ptr<CS230::Texture> s_tex_hover;

  void EnsureLoaded()
  {
    if (!s_tex_normal)
      s_tex_normal = Engine::GetTextureManager().Load("Assets/images/mouse.png");
    if (!s_tex_hover)
      s_tex_hover = Engine::GetTextureManager().Load("Assets/images/mouse_hover.png");
  }
} // namespace

void GameCursor::Enable()
{
  EnsureLoaded();
  SDL_ShowCursor(SDL_DISABLE);
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}

void GameCursor::Disable()
{
  SDL_ShowCursor(SDL_ENABLE);
  ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
  s_tex_normal.reset();
  s_tex_hover.reset();
}

void GameCursor::Draw(bool hovered)
{
  EnsureLoaded();
  auto& tex = hovered ? s_tex_hover : s_tex_normal;
  if (!tex) return;
  auto       win   = Engine::GetWindow().GetSize();
  Math::vec2 mouse = TacticalCamera::ScreenToVirtual(Engine::GetInput().GetMousePos(), win);
  double     h     = static_cast<double>(tex->GetSize().y);
  // depth 0.001f: DrawDepth::UI(0.01f)보다 작아 모든 UI 요소(슬롯 아이콘 0.005f 포함) 앞에 표시
  tex->Draw(Math::TranslationMatrix(Math::vec2{ mouse.x, mouse.y - h }), 0xFFFFFFFF, 0.001f);
}
