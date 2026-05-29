/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Settings.h
Project:    CS230 Engine
*/

#pragma once

#include "../Engine/Engine.h"
#include "../Engine/GameState.h"
#include "../Engine/Fonts.h"
#include "../Engine/Vec2.h"
#include <string>
#include <vector>

class Settings : public CS230::GameState
{
  public:
  Settings();
  void          Load() override;
  void          Update(double dt) override;
  void          Unload() override;
  void          Draw() override;
  void          DrawImGui() override;
  gsl::czstring GetName() const override;

  static constexpr CS200::RGBA title_main_color   = 0x960000FF;
  static constexpr CS200::RGBA title_shadow_color = 0xFFC800FF;
  static constexpr CS200::RGBA non_selected_color = 0x787878FF;
  static constexpr CS200::RGBA selected_color     = 0xFFC800FF;

  enum class MapSize { Small, Medium, Large, COUNT };
  static MapSize s_current_map_size;
  static int     s_bgm_volume;
  static int     s_sfx_volume;
  static bool    s_is_bgm_muted;
  static bool    s_is_sfx_muted;

  static std::string GetCurrentMapId();

  private:
  enum class Option
  {
    MapSize,
    BGMVolume,
    SFXVolume,
    BGMMute,
    SFXMute,
    Back,
    COUNT
  };
  Option current_option;
  // 현재 드래그 중인 볼륨 슬라이더 (COUNT = 드래그 없음). 드래그 시작 슬라이더를 캡처해
  // 마우스가 다른 바로 넘어가도 그 슬라이더에만 적용되도록 한다.
  Option m_dragging_slider = Option::COUNT;

  struct SettingRow
  {
    std::string label;
    Option      option;
  };
  std::vector<SettingRow> rows;

  Math::vec2 title_pos;
  Math::vec2 menu_start_pos;
  double     row_spacing = 80.0;

  static constexpr Math::ivec2 default_window_size = { 1600, 900 };

  // 슬라이더 설정
  const float slider_x_start     = 900.0f;
  const float slider_width       = 300.0f;
  const float slider_height      = 15.0f;
  const int   wheel_sensitivity  = 5;

  // 불꽃 파티클
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

  void               ApplySettings();
  static std::string OptionToMapId(MapSize size);
};
