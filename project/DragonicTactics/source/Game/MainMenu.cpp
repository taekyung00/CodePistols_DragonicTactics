#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MainMenu.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 6, 2025
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
#if defined(DEVELOPER_VERSION)
#include "Game/DragonicTactics/States/ConsoleTest.h"
#include "Game/DragonicTactics/States/RenderingTest.h"
#endif
#include "OpenGL/Environment.h"
#include "States.h"
#include <cmath>

MainMenu::MainMenu() : current_option(Option::DragonicTactics)
{
}

void MainMenu::DrawImGui()
{
}

void MainMenu::SelecetOption()
{
    switch (current_option)
    {
        case Option::DragonicTactics:
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<GamePlay>();
            break;

        case Option::Settings:
            Engine::GetGameStateManager().PushState<Settings>();
            break;

        case Option::Exit:
            Engine::GetGameStateManager().PopState();
            break;

#if defined(DEVELOPER_VERSION)
        case Option::ConsoleTest:
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<ConsoleTest>();
            break;

        case Option::RenderingTest:
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<RenderingTest>();
            break;
#endif

        default:
            break;
    }
}

void MainMenu::InitFlame(Flame& f, bool random_y)
{
    f.pos.x        = static_cast<double>(rand() % default_window_size.x);
    f.pos.y        = random_y ? static_cast<double>(rand() % default_window_size.y) : (-50.0 - (rand() % 150));
    f.size         = 2.0 + (rand() % 4);
    f.speed_y      = 1.0 + ((rand() % 40) / 10.0);
    f.speed_x      = ((rand() % 100) - 50) / 100.0;
    f.flicker_offset = (rand() % 1000) / 10.0;

    int color_pick = rand() % 3;
    if      (color_pick == 0) f.color = 0xFFD20000;
    else if (color_pick == 1) f.color = 0xFF640000;
    else                      f.color = 0xFF190000;
}

void MainMenu::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0F0F0FFF);
    if (!OpenGL::IsWebGL)
    {
        Engine::GetWindow().ForceResize(default_window_size.x, default_window_size.y);
        Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    main_menu_items.push_back({ "SETTINGS",   Option::Settings });
    main_menu_items.push_back({ "GAME START", Option::DragonicTactics });
    main_menu_items.push_back({ "QUIT",       Option::Exit });

#if defined(DEVELOPER_VERSION)
    dev_menu_items.push_back({ "Console test",   Option::ConsoleTest });
    dev_menu_items.push_back({ "Rendering test", Option::RenderingTest });
#endif

    title_pos          = Math::vec2{ default_window_size.x * 0.5, default_window_size.y * 0.75 };
    title_scale        = Math::vec2{ 1.5, 1.5 };
    menu_center_pos    = Math::vec2{ default_window_size.x * 0.5, default_window_size.y * 0.33 };
    menu_item_spacing  = 400.0;
    menu_item_size     = Math::vec2{ 320.0, 80.0 };

    flames.resize(100);
    for (auto& f : flames) InitFlame(f, true);

    Engine::GetSoundManager().PlayBGM(SoundManager::BGM_MAIN_MENU);
}

void MainMenu::Update(double dt)
{
    run_time += dt;

    CS230::Input& input       = Engine::GetInput();
    auto          window_size = Engine::GetWindow().GetSize();
    Math::vec2    mouse_pos   = TacticalCamera::ScreenToVirtual(input.GetMousePos(), window_size);

    if (input.KeyJustReleased(CS230::Input::Keys::Left)  || input.KeyJustReleased(CS230::Input::Keys::A) ||
        input.KeyJustReleased(CS230::Input::Keys::Up)    || input.KeyJustReleased(CS230::Input::Keys::W))
    {
        int count      = static_cast<int>(Option::COUNT);
        current_option = static_cast<Option>((static_cast<int>(current_option) - 1 + count) % count);
    }
    else if (input.KeyJustReleased(CS230::Input::Keys::Right) || input.KeyJustReleased(CS230::Input::Keys::D) ||
             input.KeyJustReleased(CS230::Input::Keys::Down)  || input.KeyJustReleased(CS230::Input::Keys::S))
    {
        int count      = static_cast<int>(Option::COUNT);
        current_option = static_cast<Option>((static_cast<int>(current_option) + 1) % count);
    }
    else if (input.KeyJustReleased(CS230::Input::Keys::Enter) ||
             input.KeyJustReleased(CS230::Input::Keys::Space) ||
             input.KeyJustReleased(CS230::Input::Keys::Z))
    {
        Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
        SelecetOption();
    }

    bool mouse_is_hovering = false;
    auto check_hover = [&](const std::vector<MenuItem>& items, double base_y)
    {
        double offset_start = -(static_cast<double>(items.size()) - 1.0) / 2.0;
        for (size_t i = 0; i < items.size(); ++i)
        {
            double     item_x   = menu_center_pos.x + ((offset_start + static_cast<double>(i)) * menu_item_spacing);
            Math::vec2 rect_bl  = { item_x - (menu_item_size.x / 2.0), base_y - (menu_item_size.y / 2.0) };
            if (mouse_pos.x >= rect_bl.x && mouse_pos.x <= rect_bl.x + menu_item_size.x &&
                mouse_pos.y >= rect_bl.y && mouse_pos.y <= rect_bl.y + menu_item_size.y)
            {
                current_option    = items[i].option;
                mouse_is_hovering = true;
            }
        }
    };

    check_hover(main_menu_items, menu_center_pos.y);
#if defined(DEVELOPER_VERSION)
    check_hover(dev_menu_items, menu_center_pos.y - 120.0);
#endif

    if (input.MouseJustReleased(0) && mouse_is_hovering)
    {
        Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
        SelecetOption();
    }
}

void MainMenu::Draw()
{
    CS200::RenderingAPI::Clear();
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    auto win         = Engine::GetWindow().GetSize();

    Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
    Engine::GetTextureManager().SaveCurrentScene(ui_ndc);
    renderer_2d->BeginScene(ui_ndc);

    DrawFlames(run_time);

    auto& text_manager = Engine::GetTextManager();

    // 타이틀 (그림자 + 본체)
    Math::vec2  shadow_offset = { 5.0, -5.0 };
    Math::ivec2 title_size    = text_manager.CalculateTextSize("DRAGONIC TACTICS", Fonts::Kings);
    Math::vec2  t_bl = {
        title_pos.x - (title_size.x * title_scale.x) / 2.0,
        title_pos.y - (title_size.y * title_scale.y) / 2.0
    };

    text_manager.DrawText("DRAGONIC TACTICS",
        Math::vec2{ t_bl.x + shadow_offset.x, t_bl.y + shadow_offset.y },
        Fonts::Kings, title_scale, title_shadow_color);
    text_manager.DrawText("DRAGONIC TACTICS", t_bl, Fonts::Kings, title_scale, title_main_color);

    // 메뉴 항목 (가로 배치, 선택 항목에 반동 화살표)
    auto draw_items = [&](const std::vector<MenuItem>& items, double base_y)
    {
        // 1. Update 함수에서 사용하신 것과 동일하게 Input과 마우스 좌표를 가져옵니다.
        CS230::Input& input       = Engine::GetInput();
        auto          window_size = Engine::GetWindow().GetSize();
        Math::vec2    mouse_pos   = TacticalCamera::ScreenToVirtual(input.GetMousePos(), window_size);

        // 2. 키보드와 마우스가 '눌려있는(Down)' 상태인지 확인합니다.
        // [중요] 프로젝트의 Input 클래스에 구현된 '누르고 있는 상태' 검사 함수명에 맞게 변경해 주세요.
        // (예: MouseIsDown, IsMouseDown, KeyIsDown, IsKeyDown 등)
        bool is_mouse_down = input.MouseDown(0); 
        bool is_key_down   = input.KeyDown(CS230::Input::Keys::Enter) || 
                            input.KeyDown(CS230::Input::Keys::Space) || 
                            input.KeyDown(CS230::Input::Keys::Z);

        double offset_start = -(static_cast<double>(items.size()) - 1.0) / 2.0;
        for (size_t i = 0; i < items.size(); ++i)
        {
            const auto& item        = items[i];
            Math::vec2  center_pos  = {
                menu_center_pos.x + ((offset_start + static_cast<double>(i)) * menu_item_spacing),
                base_y
            };
            Math::ivec2 text_size   = text_manager.CalculateTextSize(item.text, Fonts::Kings);
            Math::vec2  text_bl     = {
                center_pos.x - text_size.x / 2.0,
                center_pos.y - text_size.y / 2.0
            };

            bool is_selected = (item.option == current_option);

            // 3. Update와 동일한 방식의 마우스 호버링 충돌 검사입니다.
            Math::vec2 rect_bl = { center_pos.x - (menu_item_size.x / 2.0), base_y - (menu_item_size.y / 2.0) };
            bool is_hovering = (mouse_pos.x >= rect_bl.x && mouse_pos.x <= rect_bl.x + menu_item_size.x &&
                                mouse_pos.y >= rect_bl.y && mouse_pos.y <= rect_bl.y + menu_item_size.y);

            // 4. 마우스를 버튼 위에 올리고 누르고 있거나, 항목이 선택된 상태에서 결정 키를 누르고 있다면 상호작용 중입니다.
            bool is_interacting = (is_hovering && is_mouse_down) || (is_selected && is_key_down);

            // 5. 색상 결정 로직 (기본은 선택/비선택 색상, 누를 때는 하얀색)
            CS200::RGBA item_color = is_selected ? selected_color : non_selected_color;
            if (is_interacting)
            {
                item_color = 0xFFFFFFFF; // 0xFFFFFFFF = 완전한 하얀색
            }

            // 결정된 색상으로 텍스트를 화면에 그립니다.
            text_manager.DrawText(item.text, text_bl, Fonts::Kings, { 1.0, 1.0 }, item_color);

            if (is_selected)
            {
                Math::ivec2 ptr_size  = text_manager.CalculateTextSize(">", Fonts::Kings);
                double      padding   = 40.0;
                double      bounce    = std::sin(run_time * 5.0) * 10.0;

                Math::vec2 left_pos  = {
                    center_pos.x - (text_size.x / 2.0) - padding - bounce - ptr_size.x,
                    text_bl.y
                };
                Math::vec2 right_pos = {
                    center_pos.x + (text_size.x / 2.0) + padding + bounce,
                    text_bl.y
                };

                // 양옆의 반동 화살표(>, <)도 텍스트와 같이 하얀색으로 반응하게 하려면 item_color를 전달합니다.
                text_manager.DrawText(">", left_pos,  Fonts::Kings, { 1.0, 1.0 }, item_color);
                text_manager.DrawText("<", right_pos, Fonts::Kings, { 1.0, 1.0 }, item_color);
            }
        }
    };

    draw_items(main_menu_items, menu_center_pos.y);
#if defined(DEVELOPER_VERSION)
    draw_items(dev_menu_items, menu_center_pos.y - 120.0);
#endif

    renderer_2d->EndScene();
}

void MainMenu::DrawFlames(double time)
{
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    for (auto& f : flames)
    {
        f.pos.y += f.speed_y;
        f.pos.x += f.speed_x + std::sin(time * 3.0 + f.flicker_offset) * 0.8;

        if (f.pos.y > default_window_size.y + 50) InitFlame(f, false);

        double  flicker      = 0.5 + 0.5 * std::sin(time * 15.0 + f.flicker_offset);
        uint8_t a            = static_cast<uint8_t>((0.3 + 0.7 * flicker) * 255.0);
        CS200::RGBA render_color = f.color | a;

        renderer_2d->DrawCircle(
            Math::TranslationMatrix(f.pos) * Math::ScaleMatrix(Math::vec2{ f.size, f.size }),
            render_color, render_color, 0, 0);
    }
}

void MainMenu::Unload()
{
    Engine::GetSoundManager().StopBGM();
}

gsl::czstring MainMenu::GetName() const
{
    return "MainMenu";
}
