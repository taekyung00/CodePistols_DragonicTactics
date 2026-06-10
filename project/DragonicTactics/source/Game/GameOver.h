/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameOver.h
Project:    CS230 Engine
*/

#pragma once

#include "../Engine/GameState.h"
#include "../Engine/Vec2.h"
#include <string>
#include <vector>

namespace CS200 { using RGBA = unsigned int; }

class GameOver : public CS230::GameState
{
  public:
  void          Load() override;
  void          Update(double dt) override;
  void          Unload() override;
  void          Draw() override;
  void          DrawImGui() override;
  gsl::czstring GetName() const override;

  // GamePlay이 전환 전에 설정
  static bool s_player_won;
  static int  s_current_level_id;  // 0 = 자유 모드, 1~3 = 레벨 모드

  private:
  enum class Option { NextLevel, PlayAgain, MainMenu, COUNT };
  Option current_option = Option::PlayAgain;

  struct MenuItem { std::string text; Option option; };
  std::vector<MenuItem> menu_items_;

  // 메뉴 레이아웃
  Math::vec2 title_pos_        = {};
  Math::vec2 menu_center_pos_  = {};
  double     menu_item_spacing_ = 0.0;
  Math::vec2 menu_item_size_   = {};

  static constexpr Math::ivec2 WINDOW_SIZE = { 1600, 900 };

  // 불꽃 파티클 (MainMenu와 동일)
  struct Flame
  {
    Math::vec2  pos;
    double      size;
    double      speed_y;
    double      speed_x;
    double      flicker_offset;
    unsigned int color;
  };
  std::vector<Flame> flames_;
  void InitFlame(Flame& f, bool random_y);
  void DrawFlames(double time);

  double run_time_ = 0.0;

  void SelectOption();
};
