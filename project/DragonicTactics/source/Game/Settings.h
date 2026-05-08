/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Settings.h
Project:    CS230 Engine
*/

#pragma once

#include "../Engine/Engine.h"
#include "../Engine/Font.h"
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"

#include "../Engine/Fonts.h"

#include <string>
#include <vector>

class Settings : public CS230::GameState
{
  public:
  Settings();
  void						   Load() override;
  void						   Update(double dt) override;
  void						   Unload() override;
  void						   Draw() override;
  void						   DrawImGui() override;
  void						   SelectOption();
  gsl::czstring				   GetName() const override;

  static constexpr CS200::RGBA title_color		  = 0x9A2EFEFF;
  static constexpr CS200::RGBA non_selected_color = 0xFFFFFFFF;
  static constexpr CS200::RGBA hover_color		  = 0x3ADF00FF;
  static constexpr CS200::RGBA active_color		  = 0x00CFFFFF;

  static bool s_bgm_enabled;
  static int  s_bgm_volume_pct; // 10..100, multiples of 10

  private:
  enum class Option
  {
	Small,	// map index 0 (8x8)
	Medium, // map index 1 (10x10)
	Large,	// map index 2 (12x12)
	BGMToggle,
	BGMVolume,
	Back,
	COUNT
  };
  Option current_option;

  static void ApplyBGMSettings();

  struct MenuItem
  {
	std::string text;
	Option		option;
  };

  std::vector<MenuItem> menu_items;

  Math::vec2 title_pos;
  Math::vec2 title_scale;

  Math::vec2 menu_start_pos_bl;
  Math::vec2 menu_item_size;
  double	 menu_item_total_height;

  static constexpr Math::ivec2 default_window_size = { 800, 600 };

  static std::string OptionToMapId(Option opt);
};
