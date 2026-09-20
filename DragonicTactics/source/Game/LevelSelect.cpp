#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  LevelSelect.cpp
Project:    CS230 Engine
*/
#include "CS200/IRenderer2D.h"
#include "CS200/RenderingAPI.h"
#include "Engine/Engine.h"
#include "Engine/GameStateManager.h"
#include "Engine/Input.h"
#include "Engine/Matrix.h"
#include "Engine/SoundManager.h"
#include "Engine/TextManager.h"
#include "Engine/Window.h"
#include "Game/DragonicTactics/States/GamePlay.h"
#include "GameCursor.h"
#include "LevelSelect.h"
#include "MainMenu.h"
#include "OpenGL/Environment.h"
#include <cmath>

int LevelSelect::s_max_unlocked_level = 1;

LevelSelect::LevelSelect() : current_option(Option::Level1)
{
}

void LevelSelect::DrawImGui()
{
}

void LevelSelect::SelectOption()
{
	if (IsLocked(current_option)) return;

	switch (current_option)
	{
		case Option::Level1:
			GamePlay::s_level_id = 1;
			Engine::GetGameStateManager().PopState();
			Engine::GetGameStateManager().PushState<GamePlay>();
			break;

		case Option::Level2:
			GamePlay::s_level_id = 2;
			Engine::GetGameStateManager().PopState();
			Engine::GetGameStateManager().PushState<GamePlay>();
			break;

		case Option::Level3:
			GamePlay::s_level_id = 3;
			Engine::GetGameStateManager().PopState();
			Engine::GetGameStateManager().PushState<GamePlay>();
			break;

		case Option::Back:
			Engine::GetGameStateManager().PopState();
			Engine::GetGameStateManager().PushState<MainMenu>();
			break;

		default:
			break;
	}
}

void LevelSelect::InitFlame(Flame& f, bool random_y)
{
	f.pos.x          = static_cast<double>(rand() % default_window_size.x);
	f.pos.y          = random_y ? static_cast<double>(rand() % default_window_size.y) : (-50.0 - (rand() % 150));
	f.size           = 2.0 + (rand() % 4);
	f.speed_y        = 1.0 + ((rand() % 40) / 10.0);
	f.speed_x        = ((rand() % 100) - 50) / 100.0;
	f.flicker_offset = (rand() % 1000) / 10.0;

	int color_pick = rand() % 3;
	if      (color_pick == 0) f.color = 0xFFD20000;
	else if (color_pick == 1) f.color = 0xFF640000;
	else                      f.color = 0xFF190000;
}

void LevelSelect::Load()
{
	CS200::RenderingAPI::SetClearColor(0x0F0F0FFF);
	if (!OpenGL::IsWebGL)
	{
		Engine::GetWindow().ForceResize(default_window_size.x, default_window_size.y);
		Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}

	menu_items.push_back({ "LEVEL 1", Option::Level1 });
	menu_items.push_back({ "LEVEL 2", Option::Level2 });
	menu_items.push_back({ "LEVEL 3", Option::Level3 });
	menu_items.push_back({ "BACK",    Option::Back });

	title_pos         = Math::vec2{ default_window_size.x * 0.5, default_window_size.y * 0.75 };
	title_scale       = Math::vec2{ 1.5, 1.5 };
	menu_center_pos   = Math::vec2{ default_window_size.x * 0.5, default_window_size.y * 0.33 };
	menu_item_spacing = 300.0;
	menu_item_size    = Math::vec2{ 260.0, 80.0 };

	flames.resize(100);
	for (auto& f : flames) InitFlame(f, true);
	GameCursor::Enable();
}

void LevelSelect::Update(double dt)
{
	run_time += dt;

	CS230::Input& input       = Engine::GetInput();
	auto          window_size = Engine::GetWindow().GetSize();
	Math::vec2    mouse_pos   = TacticalCamera::ScreenToVirtual(input.GetMousePos(), window_size);

	if (input.KeyJustReleased(CS230::Input::Keys::Left)  || input.KeyJustReleased(CS230::Input::Keys::A) ||
	    input.KeyJustReleased(CS230::Input::Keys::Up)    || input.KeyJustReleased(CS230::Input::Keys::W))
	{
		int count = static_cast<int>(Option::COUNT);
		int next  = (static_cast<int>(current_option) - 1 + count) % count;
		if (!IsLocked(static_cast<Option>(next)))
			current_option = static_cast<Option>(next);
	}
	else if (input.KeyJustReleased(CS230::Input::Keys::Right) || input.KeyJustReleased(CS230::Input::Keys::D) ||
	         input.KeyJustReleased(CS230::Input::Keys::Down)  || input.KeyJustReleased(CS230::Input::Keys::S))
	{
		int count = static_cast<int>(Option::COUNT);
		int next  = (static_cast<int>(current_option) + 1) % count;
		if (!IsLocked(static_cast<Option>(next)))
			current_option = static_cast<Option>(next);
	}
	else if (input.KeyJustReleased(CS230::Input::Keys::Enter) ||
	         input.KeyJustReleased(CS230::Input::Keys::Space) ||
	         input.KeyJustReleased(CS230::Input::Keys::Z))
	{
		Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
		SelectOption();
	}
	else if (input.KeyJustReleased(CS230::Input::Keys::Escape))
	{
		Engine::GetGameStateManager().PopState();
		Engine::GetGameStateManager().PushState<MainMenu>();
	}

	bool mouse_is_hovering = false;
	double offset_start    = -(static_cast<double>(menu_items.size()) - 1.0) / 2.0;
	for (size_t i = 0; i < menu_items.size(); ++i)
	{
		double     item_x  = menu_center_pos.x + ((offset_start + static_cast<double>(i)) * menu_item_spacing);
		Math::vec2 rect_bl = { item_x - (menu_item_size.x / 2.0), menu_center_pos.y - (menu_item_size.y / 2.0) };
		if (mouse_pos.x >= rect_bl.x && mouse_pos.x <= rect_bl.x + menu_item_size.x &&
		    mouse_pos.y >= rect_bl.y && mouse_pos.y <= rect_bl.y + menu_item_size.y)
		{
			if (!IsLocked(menu_items[i].option))
			{
				current_option    = menu_items[i].option;
				mouse_is_hovering = true;
			}
		}
	}

	if (input.MouseJustReleased(0) && mouse_is_hovering)
	{
		Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
		SelectOption();
	}
}

void LevelSelect::Draw()
{
	CS200::RenderingAPI::Clear();
	auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
	auto win         = Engine::GetWindow().GetSize();

	Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
	Engine::GetTextureManager().SaveCurrentScene(ui_ndc);
	renderer_2d->BeginScene(ui_ndc);

	DrawFlames(run_time);

	auto& text_manager = Engine::GetTextManager();

	// 타이틀
	Math::vec2  shadow_offset = { 5.0, -5.0 };
	Math::ivec2 title_size    = text_manager.CalculateTextSize("SELECT LEVEL", Fonts::Kings);
	Math::vec2  t_bl          = {
		title_pos.x - (title_size.x * title_scale.x) / 2.0,
		title_pos.y - (title_size.y * title_scale.y) / 2.0
	};

	text_manager.DrawText("SELECT LEVEL",
	    Math::vec2{ t_bl.x + shadow_offset.x, t_bl.y + shadow_offset.y },
	    Fonts::Kings, title_scale, title_shadow_color);
	text_manager.DrawText("SELECT LEVEL", t_bl, Fonts::Kings, title_scale, title_main_color);

	// 메뉴 항목
	bool is_mouse_down = Engine::GetInput().MouseDown(0);
	bool is_key_down   = Engine::GetInput().KeyDown(CS230::Input::Keys::Enter) ||
	                     Engine::GetInput().KeyDown(CS230::Input::Keys::Space) ||
	                     Engine::GetInput().KeyDown(CS230::Input::Keys::Z);

	double offset_start = -(static_cast<double>(menu_items.size()) - 1.0) / 2.0;
	for (size_t i = 0; i < menu_items.size(); ++i)
	{
		const auto& item       = menu_items[i];
		Math::vec2  center_pos = {
			menu_center_pos.x + ((offset_start + static_cast<double>(i)) * menu_item_spacing),
			menu_center_pos.y
		};
		Math::ivec2 text_size = text_manager.CalculateTextSize(item.text, Fonts::Kings);
		Math::vec2  text_bl   = {
			center_pos.x - text_size.x / 2.0,
			center_pos.y - text_size.y / 2.0
		};

		bool is_locked   = IsLocked(item.option);
		bool is_selected = !is_locked && (item.option == current_option);

		Math::vec2 rect_bl     = { center_pos.x - (menu_item_size.x / 2.0), menu_center_pos.y - (menu_item_size.y / 2.0) };
		auto       window_size = Engine::GetWindow().GetSize();
		Math::vec2 virt_mp     = TacticalCamera::ScreenToVirtual(Engine::GetInput().GetMousePos(), window_size);
		bool       is_hovering = !is_locked &&
		                         (virt_mp.x >= rect_bl.x && virt_mp.x <= rect_bl.x + menu_item_size.x &&
		                          virt_mp.y >= rect_bl.y && virt_mp.y <= rect_bl.y + menu_item_size.y);

		bool is_interacting = (is_hovering && is_mouse_down) || (is_selected && is_key_down);

		CS200::RGBA item_color = is_locked ? locked_color : (is_selected ? selected_color : non_selected_color);
		if (!is_locked && is_interacting) item_color = 0xFFFFFFFF;

		text_manager.DrawText(item.text, text_bl, Fonts::Kings, { 1.0, 1.0 }, item_color);

		// 잠긴 레벨에 LOCKED 표시
		if (is_locked)
		{
			Math::ivec2 lock_size = text_manager.CalculateTextSize("LOCKED", Fonts::Kings);
			Math::vec2  lock_bl   = {
				center_pos.x - lock_size.x / 2.0,
				center_pos.y - text_size.y / 2.0 - lock_size.y - 8.0
			};
			text_manager.DrawText("LOCKED", lock_bl, Fonts::Kings, { 0.7, 0.7 }, locked_color);
		}

		if (is_selected)
		{
			Math::ivec2 ptr_size = text_manager.CalculateTextSize(">", Fonts::Kings);
			double      padding  = 30.0;
			double      bounce   = std::sin(run_time * 5.0) * 10.0;

			Math::vec2 left_pos  = {
				center_pos.x - (text_size.x / 2.0) - padding - bounce - ptr_size.x,
				text_bl.y
			};
			Math::vec2 right_pos = {
				center_pos.x + (text_size.x / 2.0) + padding + bounce,
				text_bl.y
			};

			text_manager.DrawText(">", left_pos,  Fonts::Kings, { 1.0, 1.0 }, item_color);
			text_manager.DrawText("<", right_pos, Fonts::Kings, { 1.0, 1.0 }, item_color);
		}
	}

	GameCursor::Draw();
	renderer_2d->EndScene();
}

void LevelSelect::DrawFlames(double time)
{
	auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
	for (auto& f : flames)
	{
		f.pos.y += f.speed_y;
		f.pos.x += f.speed_x + std::sin(time * 3.0 + f.flicker_offset) * 0.8;

		if (f.pos.y > default_window_size.y + 50) InitFlame(f, false);

		double      flicker      = 0.5 + 0.5 * std::sin(time * 15.0 + f.flicker_offset);
		uint8_t     a            = static_cast<uint8_t>((0.3 + 0.7 * flicker) * 255.0);
		CS200::RGBA render_color = f.color | a;

		renderer_2d->DrawCircle(
		    Math::TranslationMatrix(f.pos) * Math::ScaleMatrix(Math::vec2{ f.size, f.size }),
		    render_color, render_color, 0, 0);
	}
}

void LevelSelect::Unload()
{
}

gsl::czstring LevelSelect::GetName() const
{
	return "LevelSelect";
}
