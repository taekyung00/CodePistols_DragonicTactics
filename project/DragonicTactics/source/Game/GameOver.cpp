#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameOver.cpp
Project:    CS230 Engine
*/
#include "CS200/IRenderer2D.h"
#include "CS200/NDC.h"
#include "CS200/RenderingAPI.h"
#include "Engine/Engine.h"
#include "Engine/Fonts.h"
#include "Engine/GameStateManager.h"
#include "Engine/Input.h"
#include "Engine/Matrix.h"
#include "Engine/SoundManager.h"
#include "Engine/TextManager.h"
#include "Engine/Window.h"
#include "Game/DragonicTactics/States/GamePlay.h"
#include "GameCursor.h"
#include "GameOver.h"
#include "MainMenu.h"
#include "OpenGL/Environment.h"
#include <cmath>

bool GameOver::s_player_won    = false;
int  GameOver::s_current_level_id = 0;

void GameOver::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0F0F0FFF);
    if (!OpenGL::IsWebGL)
    {
        Engine::GetWindow().ForceResize(WINDOW_SIZE.x, WINDOW_SIZE.y);
        Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    menu_items_.clear();
    bool show_next_level = s_player_won && s_current_level_id > 0 && s_current_level_id < 3;
    if (show_next_level)
        menu_items_.push_back({ "NEXT LEVEL", Option::NextLevel });
    menu_items_.push_back({ "PLAY AGAIN", Option::PlayAgain });
    menu_items_.push_back({ "MAIN MENU",  Option::MainMenu  });
    current_option = show_next_level ? Option::NextLevel : Option::PlayAgain;

    title_pos_         = { WINDOW_SIZE.x * 0.5, WINDOW_SIZE.y * 0.72 };
    menu_center_pos_   = { WINDOW_SIZE.x * 0.5, WINDOW_SIZE.y * 0.35 };
    menu_item_spacing_ = 400.0;
    menu_item_size_    = { 320.0, 80.0 };

    flames_.resize(100);
    for (auto& f : flames_) InitFlame(f, true);
    GameCursor::Enable();
    auto& snd = Engine::GetSoundManager();
    snd.LoadBGM(SoundManager::BGM_WIN);
    snd.LoadBGM(SoundManager::BGM_LOSE);
    snd.PlayBGM(s_player_won ? SoundManager::BGM_WIN : SoundManager::BGM_LOSE);
}


void GameOver::Update(double dt)
{
    run_time_ += dt;

    CS230::Input& input       = Engine::GetInput();
    auto          window_size = Engine::GetWindow().GetSize();
    Math::vec2    mouse_pos   = TacticalCamera::ScreenToVirtual(input.GetMousePos(), window_size);

    // 키보드 좌우/상하 이동 — menu_items_ 기반 순환 (조건부 버튼 대응)
    auto find_cur_idx = [&]() -> int {
        for (int i = 0; i < static_cast<int>(menu_items_.size()); ++i)
            if (menu_items_[static_cast<size_t>(i)].option == current_option) return i;
        return 0;
    };
    if (input.KeyJustReleased(CS230::Input::Keys::Left)  || input.KeyJustReleased(CS230::Input::Keys::A) ||
        input.KeyJustReleased(CS230::Input::Keys::Up)    || input.KeyJustReleased(CS230::Input::Keys::W))
    {
        int count      = static_cast<int>(menu_items_.size());
        int cur        = find_cur_idx();
        current_option = menu_items_[static_cast<size_t>((cur - 1 + count) % count)].option;
    }
    else if (input.KeyJustReleased(CS230::Input::Keys::Right) || input.KeyJustReleased(CS230::Input::Keys::D) ||
             input.KeyJustReleased(CS230::Input::Keys::Down)  || input.KeyJustReleased(CS230::Input::Keys::S))
    {
        int count      = static_cast<int>(menu_items_.size());
        int cur        = find_cur_idx();
        current_option = menu_items_[static_cast<size_t>((cur + 1) % count)].option;
    }
    else if (input.KeyJustReleased(CS230::Input::Keys::Enter) ||
             input.KeyJustReleased(CS230::Input::Keys::Space) ||
             input.KeyJustReleased(CS230::Input::Keys::Z))
    {
        SelectOption();
        return;
    }

    // 마우스 호버
    bool mouse_is_hovering = false;
    double offset_start = -(static_cast<double>(menu_items_.size()) - 1.0) / 2.0;
    for (size_t i = 0; i < menu_items_.size(); ++i)
    {
        double     item_x  = menu_center_pos_.x + ((offset_start + static_cast<double>(i)) * menu_item_spacing_);
        Math::vec2 rect_bl = { item_x - menu_item_size_.x * 0.5, menu_center_pos_.y - menu_item_size_.y * 0.5 };
        if (mouse_pos.x >= rect_bl.x && mouse_pos.x <= rect_bl.x + menu_item_size_.x &&
            mouse_pos.y >= rect_bl.y && mouse_pos.y <= rect_bl.y + menu_item_size_.y)
        {
            current_option    = menu_items_[i].option;
            mouse_is_hovering = true;
        }
    }

    if (input.MouseJustReleased(0) && mouse_is_hovering)
    {
        SelectOption();
    }
}

void GameOver::SelectOption()
{
    switch (current_option)
    {
        case Option::NextLevel:
            GamePlay::s_level_id = s_current_level_id + 1;
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<GamePlay>();
            break;
        case Option::PlayAgain:
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<GamePlay>();
            break;
        case Option::MainMenu:
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<MainMenu>();
            break;
        default:
            break;
    }
}

void GameOver::Draw()
{
    CS200::RenderingAPI::Clear();
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    auto win         = Engine::GetWindow().GetSize();

    Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
    Engine::GetTextureManager().SaveCurrentScene(ui_ndc);
    renderer_2d->BeginScene(ui_ndc);

    DrawFlames(run_time_);

    auto& tm = Engine::GetTextManager();

    // ── 타이틀 ──
    const std::string title_str   = s_player_won ? "PLAYER WIN" : "INVADER WIN";
    CS200::RGBA       title_color = 0xCC2200FF;
    CS200::RGBA       shadow_color = 0xFFC800FF;
    Math::vec2        title_scale = { 1.5, 1.5 };

    Math::ivec2 title_size = tm.CalculateTextSize(title_str, Fonts::Kings);
    Math::vec2  t_bl = {
        title_pos_.x - (title_size.x * title_scale.x) * 0.5,
        title_pos_.y - (title_size.y * title_scale.y) * 0.5
    };
    // 그림자
    tm.DrawText(title_str, Math::vec2{ t_bl.x + 5.0, t_bl.y - 5.0 }, Fonts::Kings, title_scale, shadow_color);
    // 본체
    tm.DrawText(title_str, t_bl, Fonts::Kings, title_scale, title_color);

    // ── 메뉴 항목 ──
    CS230::Input& input       = Engine::GetInput();
    Math::vec2    mouse_pos   = TacticalCamera::ScreenToVirtual(input.GetMousePos(), win);
    bool          mouse_down  = input.MouseDown(0);
    bool          key_down    = input.KeyDown(CS230::Input::Keys::Enter) ||
                                input.KeyDown(CS230::Input::Keys::Space) ||
                                input.KeyDown(CS230::Input::Keys::Z);

    double offset_start = -(static_cast<double>(menu_items_.size()) - 1.0) / 2.0;
    for (size_t i = 0; i < menu_items_.size(); ++i)
    {
        const auto& item      = menu_items_[i];
        bool        is_sel    = (item.option == current_option);
        double      item_x    = menu_center_pos_.x + ((offset_start + static_cast<double>(i)) * menu_item_spacing_);
        Math::vec2  center    = { item_x, menu_center_pos_.y };

        Math::ivec2 text_size = tm.CalculateTextSize(item.text, Fonts::Kings);
        Math::vec2  text_bl   = { center.x - text_size.x * 0.5, center.y - text_size.y * 0.5 };

        Math::vec2 rect_bl    = { center.x - menu_item_size_.x * 0.5, center.y - menu_item_size_.y * 0.5 };
        bool is_hovering      = (mouse_pos.x >= rect_bl.x && mouse_pos.x <= rect_bl.x + menu_item_size_.x &&
                                 mouse_pos.y >= rect_bl.y && mouse_pos.y <= rect_bl.y + menu_item_size_.y);
        bool is_interacting   = (is_hovering && mouse_down) || (is_sel && key_down);

        CS200::RGBA item_color = is_sel ? static_cast<CS200::RGBA>(0xFFC800FF)
                                        : static_cast<CS200::RGBA>(0x787878FF);
        if (is_interacting) item_color = 0xFFFFFFFF;

        tm.DrawText(item.text, text_bl, Fonts::Kings, { 1.0, 1.0 }, item_color);

        if (is_sel)
        {
            double      bounce   = std::sin(run_time_ * 5.0) * 10.0;
            Math::ivec2 ptr_size = tm.CalculateTextSize(">", Fonts::Kings);
            double      padding  = 40.0;
            tm.DrawText(">",
                Math::vec2{ center.x - text_size.x * 0.5 - padding - bounce - ptr_size.x, text_bl.y },
                Fonts::Kings, { 1.0, 1.0 }, item_color);
            tm.DrawText("<",
                Math::vec2{ center.x + text_size.x * 0.5 + padding + bounce, text_bl.y },
                Fonts::Kings, { 1.0, 1.0 }, item_color);
        }
    }

    GameCursor::Draw();
    renderer_2d->EndScene();
}

void GameOver::DrawImGui() {}

void GameOver::Unload()
{
    Engine::GetSoundManager().StopBGM();
}

gsl::czstring GameOver::GetName() const { return "GameOver"; }

// ── 불꽃 파티클 (MainMenu와 동일) ──

void GameOver::InitFlame(Flame& f, bool random_y)
{
    f.pos.x        = static_cast<double>(rand() % WINDOW_SIZE.x);
    f.pos.y        = random_y ? static_cast<double>(rand() % WINDOW_SIZE.y) : (-50.0 - (rand() % 150));
    f.size         = 2.0 + (rand() % 4);
    f.speed_y      = 1.0 + ((rand() % 40) / 10.0);
    f.speed_x      = ((rand() % 100) - 50) / 100.0;
    f.flicker_offset = (rand() % 1000) / 10.0;
    int cp         = rand() % 3;
    f.color        = (cp == 0) ? 0xFFD20000u : (cp == 1 ? 0xFF640000u : 0xFF190000u);
}

void GameOver::DrawFlames(double time)
{
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    for (auto& f : flames_)
    {
        f.pos.y += f.speed_y;
        f.pos.x += f.speed_x + std::sin(time * 3.0 + f.flicker_offset) * 0.8;
        if (f.pos.y > WINDOW_SIZE.y + 50) InitFlame(f, false);

        double   flicker      = 0.5 + 0.5 * std::sin(time * 15.0 + f.flicker_offset);
        uint8_t  a            = static_cast<uint8_t>((0.3 + 0.7 * flicker) * 255.0);
        unsigned render_color = f.color | a;

        renderer_2d->DrawCircle(
            Math::TranslationMatrix(f.pos) * Math::ScaleMatrix(Math::vec2{ f.size, f.size }),
            render_color, render_color, 0, 0);
    }
}
