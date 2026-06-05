/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  LevelSelect.h
Project:    CS230 Engine
*/

#pragma once

#include "../Engine/Engine.h"
#include "../Engine/GameState.h"
#include "../Engine/Fonts.h"
#include "../Engine/Vec2.h"
#include <string>
#include <vector>

class LevelSelect : public CS230::GameState
{
  public:
	LevelSelect();
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
	static constexpr CS200::RGBA locked_color       = 0x3A3A3AFF;

	// 릴리즈 모드에서 순차 잠금용 — 1=레벨1만 해금, 2=1·2 해금, 3=전체 해금
	static int s_max_unlocked_level;

  private:
	enum class Option
	{
		Level1,  // level number = enum index + 1
		Level2,
		Level3,
		Back,
		COUNT
	};
	Option current_option;

	// 릴리즈 모드에서 잠긴 레벨인지 판단
	static bool IsLocked([[maybe_unused]] Option opt)
	{
#if defined(DEVELOPER_VERSION)
		return false;
#else
		if (opt == Option::Back) return false;
		return static_cast<int>(opt) + 1 > s_max_unlocked_level;
#endif
	}

	void SelectOption();

	struct MenuItem
	{
		std::string text;
		Option      option;
	};
	std::vector<MenuItem> menu_items;

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
