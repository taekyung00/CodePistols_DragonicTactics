#include "pch.h"
#include "MainMenu.h"
#include "CS200/RenderingAPI.h"
#include "CS200/NDC.h"
#include "Engine/TextManager.h"
#include "Engine/TextureManager.h"
#include "Engine/Input.h"
#include "Engine/Window.h"
#include "Game/DragonicTactics/States/GamePlay.h"
#include "Settings.h"
#if defined(DEVELOPER_VERSION)
#include "Game/DragonicTactics/States/ConsoleTest.h"
#include "Game/DragonicTactics/States/RenderingTest.h"
#endif
#include "OpenGL/Environment.h"
#include "States.h"
#include <cmath>
#include <algorithm>

MainMenu::MainMenu() : current_option(Option::DragonicTactics) {}

void MainMenu::DrawImGui() {}

void MainMenu::SelecetOption()
{
    switch (current_option)
    {
        case Option::DragonicTactics: Engine::GetGameStateManager().PopState(); Engine::GetGameStateManager().PushState<GamePlay>(); break;
        case Option::Settings:        Engine::GetGameStateManager().PopState(); Engine::GetGameStateManager().PushState<Settings>(); break;
        case Option::Exit:            Engine::GetGameStateManager().PopState(); break;
#if defined(DEVELOPER_VERSION)
        case Option::ConsoleTest:     Engine::GetGameStateManager().PopState(); Engine::GetGameStateManager().PushState<ConsoleTest>(); break;
        case Option::RenderingTest:   Engine::GetGameStateManager().PopState(); Engine::GetGameStateManager().PushState<RenderingTest>(); break;
#endif
        default: break;
    }
}

void MainMenu::InitFlame(Flame& f, bool random_y)
{
    f.pos.x = (rand() % default_window_size.x);
    f.pos.y = random_y ? (rand() % default_window_size.y) : (-50.0 - (rand() % 150));
    f.size = 2.0 + (rand() % 4);
    f.speed_y = 1.0 + ((rand() % 40) / 10.0);
    f.speed_x = ((rand() % 100) - 50) / 100.0;
    f.flicker_offset = (rand() % 1000) / 10.0;

    int color_pick = rand() % 3;
    if (color_pick == 0) f.color = 0xFFD20000;
    else if (color_pick == 1) f.color = 0xFF640000;
    else f.color = 0xFF190000;
}

void MainMenu::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0F0F0FFF);
    if (!OpenGL::IsWebGL) {
        Engine::GetWindow().ForceResize(default_window_size.x, default_window_size.y);
        Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    main_menu_items.push_back({ "SETTINGS", Option::Settings });
    main_menu_items.push_back({ "GAME START", Option::DragonicTactics });
    main_menu_items.push_back({ "QUIT", Option::Exit });

#if defined(DEVELOPER_VERSION)
    dev_menu_items.push_back({ "Console test", Option::ConsoleTest });
    dev_menu_items.push_back({ "Rendering test", Option::RenderingTest });
#endif

    title_pos = Math::vec2{ default_window_size.x * 0.5, default_window_size.y * 0.75 };
    title_scale = Math::vec2{ 1.5, 1.5 };
    menu_center_pos = Math::vec2{ default_window_size.x * 0.5, default_window_size.y * 0.33 };
    menu_item_spacing = 400.0;
    menu_item_size = Math::vec2{ 320.0, 80.0 };

    flames.resize(100);
    for (auto& f : flames) InitFlame(f, true);
}

void MainMenu::Update(double dt)
{
    run_time += dt;

    CS230::Input& input = Engine::GetInput();
    Math::vec2 mouse_pos = input.GetMousePos();

    // 키보드 조작
    if (input.KeyJustReleased(CS230::Input::Keys::Left) || input.KeyJustReleased(CS230::Input::Keys::A) ||
        input.KeyJustReleased(CS230::Input::Keys::Up) || input.KeyJustReleased(CS230::Input::Keys::W))
    {
        int count = static_cast<int>(Option::COUNT);
        current_option = static_cast<Option>((static_cast<int>(current_option) - 1 + count) % count);
    }
    else if (input.KeyJustReleased(CS230::Input::Keys::Right) || input.KeyJustReleased(CS230::Input::Keys::D) ||
             input.KeyJustReleased(CS230::Input::Keys::Down) || input.KeyJustReleased(CS230::Input::Keys::S))
    {
        int count = static_cast<int>(Option::COUNT);
        current_option = static_cast<Option>((static_cast<int>(current_option) + 1) % count);
    }
    else if (input.KeyJustReleased(CS230::Input::Keys::Enter) || input.KeyJustReleased(CS230::Input::Keys::Space))
    {
        SelecetOption();
    }

    // 마우스 호버 처리
    bool mouse_is_hovering = false;
    auto check_hover = [&](const std::vector<MenuItem>& items, double base_y) {
        double offset_start = -(static_cast<double>(items.size()) - 1.0) / 2.0;
        for (size_t i = 0; i < items.size(); ++i) {
            double item_x = menu_center_pos.x + ((offset_start + i) * menu_item_spacing);
            Math::vec2 rect_bl = Math::vec2{ item_x - (menu_item_size.x / 2.0), base_y - (menu_item_size.y / 2.0) };
            if (mouse_pos.x >= rect_bl.x && mouse_pos.x <= rect_bl.x + menu_item_size.x &&
                mouse_pos.y >= rect_bl.y && mouse_pos.y <= rect_bl.y + menu_item_size.y) {
                current_option = items[i].option;
                mouse_is_hovering = true;
            }
        }
    };

    check_hover(main_menu_items, menu_center_pos.y);
#if defined(DEVELOPER_VERSION)
    check_hover(dev_menu_items, menu_center_pos.y - 120.0);
#endif

    // 마우스 클릭 시 메뉴 이동
    if (input.MouseJustPressed(0) && mouse_is_hovering)
    {
        SelecetOption();
    }
}

void MainMenu::Draw()
{
    CS200::RenderingAPI::Clear();
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    renderer_2d->BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    // 배경 불꽃 효과
    DrawFlames(run_time);

    auto& text_manager = Engine::GetTextManager();
    Math::vec2 shadow_offset = Math::vec2{ 5.0, -5.0 };

    // 타이틀 렌더링
    Math::ivec2 title_size = text_manager.CalculateTextSize("DRAGONIC TACTICS", Fonts::Kings);
    Math::vec2 t_adjusted = Math::vec2{ title_pos.x - (title_size.x * title_scale.x) / 2.0, title_pos.y - (title_size.y * title_scale.y) / 2.0 };

    text_manager.DrawText("DRAGONIC TACTICS", Math::vec2{t_adjusted.x + shadow_offset.x, t_adjusted.y + shadow_offset.y}, Fonts::Kings, title_scale, title_shadow_color);
    text_manager.DrawText("DRAGONIC TACTICS", t_adjusted, Fonts::Kings, title_scale, title_main_color);

    // 메뉴 버튼 렌더링
    auto draw_items = [&](const std::vector<MenuItem>& items, double base_y) {
        double offset_start = -(static_cast<double>(items.size()) - 1.0) / 2.0;
        for (size_t i = 0; i < items.size(); ++i) {
            const auto& item = items[i];

            Math::vec2 center_pos = Math::vec2{ menu_center_pos.x + ((offset_start + i) * menu_item_spacing), base_y };
            Math::ivec2 text_size = text_manager.CalculateTextSize(item.text, Fonts::Kings);
            Math::vec2 text_bl = Math::vec2{ center_pos.x - text_size.x / 2.0, center_pos.y - text_size.y / 2.0 };

            bool is_selected = (item.option == current_option);
            CS200::RGBA item_color = is_selected ? selected_color : non_selected_color;

            text_manager.DrawText(item.text, text_bl, Fonts::Kings, Math::vec2{ 1.0, 1.0 }, item_color);

            if (is_selected) {
                Math::ivec2 ptr_l_size = text_manager.CalculateTextSize(">", Fonts::Kings);
                double padding = 40.0;
                double bounce = std::sin(run_time * 5.0) * 10.0;

                Math::vec2 left_pos = Math::vec2{ center_pos.x - (text_size.x / 2.0) - padding - bounce - ptr_l_size.x, text_bl.y };
                Math::vec2 right_pos = Math::vec2{ center_pos.x + (text_size.x / 2.0) + padding + bounce, text_bl.y };

                text_manager.DrawText(">", left_pos, Fonts::Kings, Math::vec2{ 1.0, 1.0 }, selected_color);
                text_manager.DrawText("<", right_pos, Fonts::Kings, Math::vec2{ 1.0, 1.0 }, selected_color);
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
    for (auto& f : flames) {
        f.pos.y += f.speed_y;
        f.pos.x += f.speed_x + std::sin(time * 3.0 + f.flicker_offset) * 0.8;

        if (f.pos.y > default_window_size.y + 50) InitFlame(f, false);

        double flicker = 0.5 + 0.5 * std::sin(time * 15.0 + f.flicker_offset);
        uint8_t a = static_cast<uint8_t>((0.3 + 0.7 * flicker) * 255.0);
        CS200::RGBA render_color = f.color | a;

        renderer_2d->DrawCircle(Math::TranslationMatrix(f.pos) * Math::ScaleMatrix(Math::vec2{f.size, f.size}), render_color, render_color, 0, 0);
    }
}

void MainMenu::Unload() {}

gsl::czstring MainMenu::GetName() const { return "MainMenu"; }