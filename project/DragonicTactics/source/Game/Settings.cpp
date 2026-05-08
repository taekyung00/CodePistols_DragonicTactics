#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Settings.cpp
Project:    CS230 Engine
*/
#include "CS200/IRenderer2D.h"
#include "CS200/NDC.h"
#include "CS200/RenderingAPI.h"
#include "Engine/Engine.h"
#include "Engine/GameStateManager.h"
#include "Engine/Input.h"
#include "Engine/Matrix.h"
#include "Engine/SoundManager.h"
#include "Engine/TextManager.h"
#include "Engine/Window.h"
#include "Game/DragonicTactics/States/GamePlay.h"
#include "MainMenu.h"
#include "Settings.h"
#include "OpenGL/Environment.h"
#include "States.h"

bool Settings::s_bgm_enabled	= true;
int	 Settings::s_bgm_volume_pct = 100;

namespace
{
	const double TITLE_X_RATIO			   = 0.25;
	const double TITLE_Y_RATIO_FROM_BOTTOM = 0.8;
	const double TITLE_SCALE_VAL		   = 1.5;

	const double MENU_CENTER_X_RATIO	 = 0.5;
	const double MENU_WIDTH_RATIO		 = 0.4;
	const double MENU_ITEM_HEIGHT_RATIO	 = 0.05;
	const double MENU_ITEM_SPACING_RATIO = 0.03;
	const double MENU_START_Y_RATIO		 = 0.4;
}

Settings::Settings() : current_option(Option::Small)
{
}

std::string Settings::OptionToMapId(Option opt)
{
	switch (opt)
	{
		case Option::Small:	 return "first_map";  // 8x8
		case Option::Medium: return "medium_map"; // 10x10
		case Option::Large:	 return "large_map";  // 12x12
		default:			 return "";
	}
}

void Settings::ApplyBGMSettings()
{
	float vol = s_bgm_enabled ? (static_cast<float>(s_bgm_volume_pct) / 100.0f) : 0.0f;
	Engine::GetSoundManager().SetBGMVolume(vol);
}

void Settings::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
	// ... ImGui 코드
#endif
}

void Settings::SelectOption()
{
	switch (current_option)
	{
		case Option::Small:
		case Option::Medium:
		case Option::Large:
			GamePlay::s_next_map_id = OptionToMapId(current_option);
			break;

		case Option::BGMToggle:
			s_bgm_enabled = !s_bgm_enabled;
			ApplyBGMSettings();
			break;

		case Option::BGMVolume:
			// 좌/우 방향키 전용 — Enter 무시
			break;

		case Option::Back:
			Engine::GetGameStateManager().PopState();
			Engine::GetGameStateManager().PushState<MainMenu>();
			break;

		case Option::COUNT:
			break;
	}
}

void Settings::Load()
{
	CS200::RenderingAPI::SetClearColor(0x000000FF);
	if (!OpenGL::IsWebGL)
	{
		Engine::GetWindow().ForceResize(default_window_size.x, default_window_size.y);
		Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}

	menu_items.push_back({ "Small  (8x8)", Option::Small });
	menu_items.push_back({ "Medium (10x10)", Option::Medium });
	menu_items.push_back({ "Large  (12x12)", Option::Large });
	menu_items.push_back({ "", Option::BGMToggle }); // text built dynamically in Draw()
	menu_items.push_back({ "", Option::BGMVolume }); // text built dynamically in Draw()
	menu_items.push_back({ "Back", Option::Back });

	const std::string& active_id = GamePlay::s_next_map_id;
	if (active_id == "first_map")		current_option = Option::Small;
	else if (active_id == "medium_map") current_option = Option::Medium;
	else if (active_id == "large_map")	current_option = Option::Large;
	else								current_option = Option::Small;

	const auto window_size = default_window_size;

	title_pos.x = window_size.x * TITLE_X_RATIO;
	title_pos.y = window_size.y * TITLE_Y_RATIO_FROM_BOTTOM;
	title_scale = Math::vec2{ TITLE_SCALE_VAL, TITLE_SCALE_VAL };

	const double text_width		  = window_size.x * MENU_WIDTH_RATIO;
	const double text_x			  = (window_size.x * MENU_CENTER_X_RATIO) - (text_width / 2.0);
	const double text_height	  = window_size.y * MENU_ITEM_HEIGHT_RATIO;
	const double start_y_from_top = window_size.y * MENU_START_Y_RATIO;

	const double item_bottom_y_from_top	   = start_y_from_top + text_height;
	const double item_bottom_y_from_bottom = window_size.y - item_bottom_y_from_top;

	menu_start_pos_bl	   = Math::vec2{ text_x, item_bottom_y_from_bottom };
	menu_item_size		   = Math::vec2{ text_width, text_height };
	menu_item_total_height = text_height + (window_size.y * MENU_ITEM_SPACING_RATIO);
}

void Settings::Update([[maybe_unused]] double dt)
{
	CS230::Input&		  input		  = Engine::GetInput();
	Math::vec2			  mouse_pos	  = input.GetMousePos();
	[[maybe_unused]] auto window_size = Engine::GetWindow().GetSize();

	if (input.KeyJustReleased(CS230::Input::Keys::Up))
	{
		int current_index = static_cast<int>(current_option);
		int total_options = static_cast<int>(Option::COUNT);
		current_index	  = (current_index - 1 + total_options) % total_options;
		current_option	  = static_cast<Option>(current_index);
	}
	else if (input.KeyJustReleased(CS230::Input::Keys::Down))
	{
		int current_index = static_cast<int>(current_option);
		int total_options = static_cast<int>(Option::COUNT);
		current_index	  = (current_index + 1) % total_options;
		current_option	  = static_cast<Option>(current_index);
	}
	else if (input.KeyJustReleased(CS230::Input::Keys::Enter))
	{
		SelectOption();
	}
	else if (current_option == Option::BGMVolume)
	{
		if (input.KeyJustReleased(CS230::Input::Keys::Left))
		{
			s_bgm_volume_pct -= 10;
			if (s_bgm_volume_pct < 10) s_bgm_volume_pct = 100;
			ApplyBGMSettings();
		}
		else if (input.KeyJustReleased(CS230::Input::Keys::Right))
		{
			s_bgm_volume_pct += 10;
			if (s_bgm_volume_pct > 100) s_bgm_volume_pct = 10;
			ApplyBGMSettings();
		}
	}

	bool mouse_is_hovering = false;
	int	 total_options	   = static_cast<int>(Option::COUNT);
	for (int i = 0; i < total_options; ++i)
	{
		double item_bottom_y = menu_start_pos_bl.y - (i * menu_item_total_height);

		Math::vec2 rect_pos{ menu_start_pos_bl.x, item_bottom_y };
		Math::vec2 rect_size = menu_item_size;

		if (IsPointInRect(mouse_pos, rect_pos, rect_size))
		{
			current_option	  = static_cast<Option>(i);
			mouse_is_hovering = true;
			break;
		}
	}

	if (input.MouseJustPressed(0) && mouse_is_hovering)
	{
		// Volume 항목은 키보드 좌/우 방향키 전용 — 클릭 무시
		if (current_option != Option::BGMVolume)
		{
			SelectOption();
		}
	}
}

void Settings::Unload()
{
}

void Settings::Draw()
{
	CS200::RenderingAPI::Clear();
	auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
	renderer_2d->BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

	auto& text_manager = Engine::GetTextManager();

	text_manager.DrawText("Settings", title_pos, Fonts::Kings, title_scale, title_color);

	const std::string& active_id = GamePlay::s_next_map_id;

	for (size_t i = 0; i < menu_items.size(); ++i)
	{
		const auto& item = menu_items[i];

		double	   current_item_y = menu_start_pos_bl.y - (i * menu_item_total_height);
		Math::vec2 item_pos		  = { menu_start_pos_bl.x, current_item_y };
		Math::vec2 item_size	  = menu_item_size;

		bool is_hovered		= (item.option == current_option);
		std::string opt_id	= OptionToMapId(item.option);
		bool is_active		= !opt_id.empty() && (opt_id == active_id);

		CS200::RGBA item_color;
		if (is_hovered)		 item_color = hover_color;
		else if (is_active)	 item_color = active_color;
		else				 item_color = non_selected_color;

		static constexpr double Y_OFFSET_RATIO = 0.5;
		double					y_offset	   = item_size.y * Y_OFFSET_RATIO;

		Math::vec2 text_draw_pos = item_pos;
		text_draw_pos.y -= y_offset;

		std::string label;
		if (item.option == Option::BGMToggle)
		{
			label = std::string("BGM: ") + (s_bgm_enabled ? "ON" : "OFF");
		}
		else if (item.option == Option::BGMVolume)
		{
			// 키보드 좌/우 방향키 전용 (마우스 클릭 미지원) — 화살표로 힌트
			label = is_hovered
				? "Volume: < " + std::to_string(s_bgm_volume_pct) + "% >"
				: "Volume:   " + std::to_string(s_bgm_volume_pct) + "%";
		}
		else
		{
			label = item.text;
			if (is_active)
			{
				label += "  *";
			}
		}

		text_manager.DrawText(label, text_draw_pos, Fonts::Kings, { 1.0, 1.0 }, item_color);
	}

	renderer_2d->EndScene();
}

gsl::czstring Settings::GetName() const
{
	return "Settings";
}
