/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MainMenu.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 6, 2025
*/

#pragma once

#include "../Engine/Engine.h"
#include "../Engine/GameState.h"
#include "../Engine/Fonts.h"
#include "../Engine/Vec2.h"
#include <string>
#include <vector>

class MainMenu : public CS230::GameState
{
  public:
  MainMenu();
  void            Load() override;
  void            Update(double dt) override;
  void            Unload() override;
  void            Draw() override;
  void            DrawImGui() override;
  void            SelecetOption();
  gsl::czstring   GetName() const override;

  static constexpr CS200::RGBA title_main_color   = 0x960000FF;
  static constexpr CS200::RGBA title_shadow_color = 0xFFC800FF;
  static constexpr CS200::RGBA non_selected_color = 0x787878FF;
  static constexpr CS200::RGBA selected_color     = 0xFFC800FF;

  private:
  enum class Option
  {
    Settings,
#if defined(DEVELOPER_VERSION)
    DragonicTactics,
#endif
    LevelGame,
    Exit,
#if defined(DEVELOPER_VERSION)
    ConsoleTest,
    RenderingTest,
#endif
    COUNT
  };
  Option current_option;

  struct MenuItem
  {
    std::string text;
    Option      option;
  };
  std::vector<MenuItem> main_menu_items;
#if defined(DEVELOPER_VERSION)
  std::vector<MenuItem> dev_menu_items;
#endif

  Math::vec2 title_pos;
  Math::vec2 title_scale;
  Math::vec2 menu_center_pos;
  double     menu_item_spacing;
  Math::vec2 menu_item_size;

  static constexpr Math::ivec2 default_window_size = { 1600, 900 };

  struct Flame
  {
    Math::vec2  pos;
    double      size;
    double      speed_y;
    double      speed_x;
    double      flicker_offset;
    CS200::RGBA color;
  };
  std::vector<Flame> flames;
  void InitFlame(Flame& f, bool random_y);
  void DrawFlames(double time);

  double run_time = 0.0;
};
