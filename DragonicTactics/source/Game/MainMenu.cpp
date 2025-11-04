/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MainMenu.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 6, 2025
*/
#include "MainMenu.h"
#include "./OpenGL/Environment.hpp"
#include "../CS200/IRenderer2D.hpp"
#include "../CS200/NDC.hpp"
#include "../CS200/RenderingAPI.hpp"
#include "../Engine/Engine.hpp"
#include "../Engine/GameStateManager.hpp"
#include "../Engine/Input.hpp"
#include "../Engine/TextManager.hpp"
#include "../Engine/Window.hpp"
#include "./Game/DragonicTactics/States/Test.h"
#include "States.h"
#include <imgui.h>

MainMenu::MainMenu() : current_option(Option::dragonic_tactics)
{
}

void MainMenu::DrawImGui(){}

void MainMenu::SelecetOption()
{
    switch (current_option)
    {

        case MainMenu::Option::dragonic_tactics:
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<Test>();
            break;

        case MainMenu::Option::test2:
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<Test2>();
            break;

        case MainMenu::Option::exit:
            Engine::GetGameStateManager().PopState(); 
            break;
    }
}


void MainMenu::Load()
{
    CS200::RenderingAPI::SetClearColor(0x000000FF);
    if(!OpenGL::IsWebGL){
		Engine::GetWindow().ForceResize(default_window_size.x,default_window_size.y);
		Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
	}
    update_colors();
}
void MainMenu::Update([[maybe_unused]] double dt)
{
  CS230::Input& input = Engine::GetInput();
  Math::vec2 mouse_pos = input.GetMousePos();

  if (input.KeyJustReleased(CS230::Input::Keys::Up))
  {
    int current_index = static_cast<int>(current_option);

    int total_options = static_cast<int>(Option::COUNT);

    current_index = (current_index - 1 + total_options) % total_options;

    current_option = static_cast<Option>(current_index);
  }
  else if (input.KeyJustReleased(CS230::Input::Keys::Down))
  {
    int current_index = static_cast<int>(current_option);
    int total_options = static_cast<int>(Option::COUNT);

    current_index = (current_index + 1) % total_options;

    current_option = static_cast<Option>(current_index);
  }
  else if (input.KeyJustReleased(CS230::Input::Keys::Enter))
  {
    SelecetOption();
  }

  // --- 2. mouse hover ---
  
  const double text_x = 300.f;
  const double text_width = 300.f; 
  const double text_height = 40.f; 

  if (mouse_pos.x >= text_x && mouse_pos.x <= text_x + text_width &&
      mouse_pos.y >= 350 && mouse_pos.y <= 350 + text_height)
  {
    current_option = Option::dragonic_tactics;
  }
  else if (mouse_pos.x >= text_x && mouse_pos.x <= text_x + text_width &&
      mouse_pos.y >= 400 && mouse_pos.y <= 400 + text_height)
  {
    current_option = Option::test2;
  }
  else if (mouse_pos.x >= text_x && mouse_pos.x <= text_x + text_width &&
      mouse_pos.y >= 450 && mouse_pos.y <= 450 + text_height)
  {
    current_option = Option::exit;
  }

  if (input.MouseJustPressed(0)) // 0 = Left Click
  {
    if (current_option == Option::dragonic_tactics &&
        mouse_pos.x >= text_x && mouse_pos.x <= text_x + text_width &&
        mouse_pos.y >= 350 && mouse_pos.y <= 350 + text_height)
    {
      SelecetOption();
    }
    else if (current_option == Option::test2 &&
        mouse_pos.x >= text_x && mouse_pos.x <= text_x + text_width &&
        mouse_pos.y >= 400 && mouse_pos.y <= 400 + text_height)
    {
      SelecetOption();
    }
    else if (current_option == Option::exit &&
        mouse_pos.x >= text_x && mouse_pos.x <= text_x + text_width &&
        mouse_pos.y >= 450 && mouse_pos.y <= 450 + text_height)
    {
      SelecetOption();
    }
  }
  update_colors();
}

void MainMenu::Unload()
{
}

void MainMenu::Draw()
{
    CS200::RenderingAPI::Clear();
    auto& renderer_2d = Engine::GetRenderer2D();
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    auto& text_manager = Engine::GetTextManager();
    auto  window_size  = Engine::GetWindow().GetSize();
    text_manager.DrawText(
        "Game Porting", Math::vec2{ static_cast<double>(window_size.x / 2 - 300 / 2 - 100), static_cast<double>(Engine::GetWindow().GetSize().y - 69 - 100) }, Fonts::Outlined, { 1.5, 1.5 },
        title_color);

    text_manager.DrawText(
        "Dragonic Tactics", Math::vec2{ static_cast<double>(window_size.x / 2 - 300 / 2), static_cast<double>(window_size.y - 68 * 3 - 200) }, Fonts::Outlined, { 1.0, 1.0 }, dragonic_tactics_color);
    
    text_manager.DrawText(
        "Test2", Math::vec2{ static_cast<double>(window_size.x / 2 - 300 / 2), static_cast<double>(window_size.y - 68 * 4 - 250) }, Fonts::Outlined, { 1.0, 1.0 }, test2_color);
    
    
    text_manager.DrawText("Exit", Math::vec2{ static_cast<double>(window_size.x / 2 - 300 / 2), static_cast<double>(window_size.y - 68 * 4 - 300) }, Fonts::Outlined, { 1.0, 1.0 }, exit_color);

    renderer_2d.EndScene();
}

gsl::czstring MainMenu::GetName() const
{
    return "MainMenu";
}

void MainMenu::update_colors()
{
    switch (current_option)
    {

        case MainMenu::Option::dragonic_tactics:
            dragonic_tactics_color = seleted_color;
            test2_color            = non_seleted_color;
            exit_color             = non_seleted_color;
            break;

       case MainMenu::Option::test2:
            dragonic_tactics_color = non_seleted_color;
            test2_color            = seleted_color;
            exit_color             = non_seleted_color;
            break;

        case MainMenu::Option::exit:
            dragonic_tactics_color = non_seleted_color;
            test2_color            = non_seleted_color;
            exit_color             = seleted_color;
            break;
    }
}
