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
#include "GameCursor.h"
#include "Settings.h"
#include "OpenGL/Environment.h"
#include "States.h"
#include <algorithm>
#include <cmath>

Settings::MapSize Settings::s_current_map_size = Settings::MapSize::Medium;
int              Settings::s_bgm_volume        = 80;
int              Settings::s_sfx_volume        = 80;
bool             Settings::s_is_bgm_muted      = false;
bool             Settings::s_is_sfx_muted      = false;

std::string Settings::OptionToMapId(MapSize size)
{
    switch (size)
    {
        case MapSize::Small:  return "first_map";
        case MapSize::Medium: return "medium_map";
        case MapSize::Large:  return "large_map";
        default:              return "medium_map";
    }
}

std::string Settings::GetCurrentMapId()
{
    return OptionToMapId(s_current_map_size);
}

void Settings::ApplySettings()
{
    auto& sound = Engine::GetSoundManager();
    sound.SetBGMVolume(s_is_bgm_muted ? 0.0f : (static_cast<float>(s_bgm_volume) / 100.0f));
    sound.SetSFXVolume(s_is_sfx_muted ? 0.0f : (static_cast<float>(s_sfx_volume) / 100.0f));
}

void Settings::DrawImGui()
{
}

void Settings::InitFlame(Flame& f, bool random_y)
{
    f.pos.x          = static_cast<double>(rand() % default_window_size.x);
    f.pos.y          = random_y ? static_cast<double>(rand() % default_window_size.y) : (-50.0 - (rand() % 150));
    f.size           = 2.0 + (rand() % 4);
    f.speed_y        = 1.0 + ((rand() % 40) / 10.0);
    f.speed_x        = ((rand() % 100) - 50) / 100.0;
    f.flicker_offset = (rand() % 1000) / 10.0;
    int cp           = rand() % 3;
    f.color          = (cp == 0) ? 0xFFD20000 : (cp == 1 ? 0xFF640000 : 0xFF190000);
}

Settings::Settings() : current_option(Option::MapSize)
{
}

void Settings::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0F0F0FFF);

    title_pos      = { 800.0, 800.0 };
    menu_start_pos = { 800.0, 600.0 };

    rows.clear();
    rows.push_back({ "MAP SIZE",   Option::MapSize });
    rows.push_back({ "BGM VOLUME", Option::BGMVolume });
    rows.push_back({ "SFX VOLUME", Option::SFXVolume });
    rows.push_back({ "BGM MUTE",   Option::BGMMute });
    rows.push_back({ "SFX MUTE",   Option::SFXMute });
    rows.push_back({ "BACK",       Option::Back });

    // GamePlay::s_next_map_id와 동기화
    const std::string& active_id = GamePlay::s_next_map_id;
    if      (active_id == "first_map")  s_current_map_size = MapSize::Small;
    else if (active_id == "medium_map") s_current_map_size = MapSize::Medium;
    else if (active_id == "large_map")  s_current_map_size = MapSize::Large;

    flames.resize(80);
    for (auto& f : flames) InitFlame(f, true);

    ApplySettings();
    GameCursor::Enable();
}

void Settings::Update(double dt)
{
    run_time += dt;

    CS230::Input& input       = Engine::GetInput();
    auto          window_size = Engine::GetWindow().GetSize();
    Math::vec2    mouse_pos   = TacticalCamera::ScreenToVirtual(input.GetMousePos(), window_size);

    int wheel = static_cast<int>(input.GetMouseScroll());

    // 상하 옵션 이동
    if (input.KeyJustReleased(CS230::Input::Keys::Up) || input.KeyJustReleased(CS230::Input::Keys::W))
    {
        current_option = static_cast<Option>(
            (static_cast<int>(current_option) - 1 + static_cast<int>(Option::COUNT)) % static_cast<int>(Option::COUNT));
    }
    if (input.KeyJustReleased(CS230::Input::Keys::Down) || input.KeyJustReleased(CS230::Input::Keys::S))
    {
        current_option = static_cast<Option>(
            (static_cast<int>(current_option) + 1) % static_cast<int>(Option::COUNT));
    }

    // 좌우 / 휠 값 조정
    int dir = 0;
    if (input.KeyJustReleased(CS230::Input::Keys::Left)  || input.KeyJustReleased(CS230::Input::Keys::A)) dir = -1;
    if (input.KeyJustReleased(CS230::Input::Keys::Right) || input.KeyJustReleased(CS230::Input::Keys::D)) dir =  1;
    if (wheel != 0) dir = (wheel > 0) ? 1 : -1;

    if (dir != 0)
    {
        if (current_option == Option::MapSize)
        {
            s_current_map_size = static_cast<MapSize>(
                (static_cast<int>(s_current_map_size) + dir + static_cast<int>(MapSize::COUNT)) % static_cast<int>(MapSize::COUNT));
            GamePlay::s_next_map_id = GetCurrentMapId();
        }
        else if (current_option == Option::BGMVolume)
        {
            s_bgm_volume = std::clamp(s_bgm_volume + dir * wheel_sensitivity, 0, 100);
            ApplySettings();
        }
        else if (current_option == Option::SFXVolume)
        {
            s_sfx_volume = std::clamp(s_sfx_volume + dir * wheel_sensitivity, 0, 100);
            ApplySettings();
        }
    }

    // 마우스 버튼을 떼면 슬라이더 드래그 캡처 해제
    if (!input.MouseDown(0))
        m_dragging_slider = Option::COUNT;

    // 마우스 호버 + 클릭
    for (size_t i = 0; i < rows.size(); ++i)
    {
        Math::vec2 p = { menu_start_pos.x, menu_start_pos.y - static_cast<double>(i) * row_spacing };

        if (mouse_pos.y > p.y - 30.0 && mouse_pos.y < p.y + 30.0)
        {
            // 드래그 중에는 호버로 현재 옵션이 바뀌지 않도록 고정
            if (m_dragging_slider == Option::COUNT)
                current_option = rows[i].option;

            // 클릭: 토글/전환 처리
            if (input.MouseJustPressed(0))
            {
                if (current_option == Option::MapSize)
                {
                    Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
                    s_current_map_size = static_cast<MapSize>(
                        (static_cast<int>(s_current_map_size) + 1) % static_cast<int>(MapSize::COUNT));
                    GamePlay::s_next_map_id = GetCurrentMapId();
                }
                else if (current_option == Option::BGMMute)
                {
                    Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
                    s_is_bgm_muted = !s_is_bgm_muted;
                    ApplySettings();
                }
                else if (current_option == Option::SFXMute)
                {
                    Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
                    s_is_sfx_muted = !s_is_sfx_muted;
                    ApplySettings();
                }
                else if (current_option == Option::Back)
                {
                    Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
                    Engine::GetGameStateManager().PopState();
                    return;
                }
                else if (current_option == Option::BGMVolume || current_option == Option::SFXVolume)
                {
                    // 드래그 시작 슬라이더 캡처 (마우스를 누른 그 슬라이더로 고정)
                    m_dragging_slider = current_option;
                }
            }
        }
    }

    // 볼륨 슬라이더 드래그 — 캡처된 슬라이더에만, 마우스 가로 위치로 적용.
    // 마우스가 다른 볼륨 바로 넘어가도 캡처된 슬라이더만 변경된다.
    if (input.MouseDown(0) &&
        (m_dragging_slider == Option::BGMVolume || m_dragging_slider == Option::SFXVolume))
    {
        float rel_x   = std::clamp(static_cast<float>(mouse_pos.x) - slider_x_start, 0.0f, slider_width);
        int   new_val = static_cast<int>((rel_x / slider_width) * 100.0f);
        if (m_dragging_slider == Option::BGMVolume) { s_bgm_volume = new_val; ApplySettings(); }
        else                                        { s_sfx_volume = new_val; ApplySettings(); }
    }

    // 확인 키
    if (input.KeyJustReleased(CS230::Input::Keys::Enter) || input.KeyJustReleased(CS230::Input::Keys::Z))
    {
        if (current_option == Option::Back)
        {
            Engine::GetGameStateManager().PopState();
            return;
        }
        else if (current_option == Option::BGMMute) { s_is_bgm_muted = !s_is_bgm_muted; ApplySettings(); }
        else if (current_option == Option::SFXMute) { s_is_sfx_muted = !s_is_sfx_muted; ApplySettings(); }
    }

    if (input.KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
    }
}

void Settings::Draw()
{
    CS200::RenderingAPI::Clear();
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    auto win         = Engine::GetWindow().GetSize();

    Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
    Engine::GetTextureManager().SaveCurrentScene(ui_ndc);
    renderer_2d->BeginScene(ui_ndc);

    DrawFlames(run_time);

    auto& text_manager = Engine::GetTextManager();

    // 1. 입력을 확인하기 위해 Input과 마우스 좌표를 가져옵니다.
    CS230::Input& input       = Engine::GetInput();
    Math::vec2    mouse_pos   = TacticalCamera::ScreenToVirtual(input.GetMousePos(), win);

    // 2. 마우스 클릭 및 키보드 눌림 상태를 확인합니다.
    bool is_mouse_down = input.MouseDown(0); 
    // 키보드를 누르고 있는 상태 (엔진에 KeyDown 함수가 있다면 사용, 없다면 생략 가능)
    bool is_key_down   = input.KeyDown(CS230::Input::Keys::Enter) || input.KeyDown(CS230::Input::Keys::Z);

    // 타이틀 (그림자 + 본체)
    Math::ivec2 title_size = text_manager.CalculateTextSize("SETTINGS", Fonts::Kings);
    Math::vec2  t_adj      = { title_pos.x - title_size.x * 0.75, title_pos.y };
    text_manager.DrawText("SETTINGS", Math::vec2{ t_adj.x + 5.0, t_adj.y - 5.0 }, Fonts::Kings, { 1.5, 1.5 }, title_shadow_color);
    text_manager.DrawText("SETTINGS", t_adj,                                      Fonts::Kings, { 1.5, 1.5 }, title_main_color);

    // 설정 항목 렌더링
    for (size_t i = 0; i < rows.size(); ++i)
    {
        const auto& row     = rows[i];
        bool        is_sel  = (row.option == current_option);
        Math::vec2  p       = { menu_start_pos.x, menu_start_pos.y - static_cast<double>(i) * row_spacing };

        // 3. Update 함수와 동일한 호버링 충돌 검사 로직 적용
        bool is_hovering = (mouse_pos.y > p.y - 30.0 && mouse_pos.y < p.y + 30.0);

        // 4. 상호작용(클릭 또는 키보드 누름) 중인지 판별
        bool is_interacting = (is_hovering && is_mouse_down) || (is_sel && is_key_down);

        // 5. 기본 색상을 지정하고, 상호작용 중일 때만 하얀색으로 변경
        CS200::RGBA color = is_sel ? selected_color : non_selected_color;
        if (is_interacting)
        {
            color = 0xFFFFFFFF; // 하얀색
        }

        // 라벨 렌더링 (동적으로 변하는 color 적용)
        text_manager.DrawText(row.label, { p.x - 500.0, p.y }, Fonts::Kings, { 0.8, 0.8 }, color);

        // 값 / 위젯 렌더링
        if (row.option == Option::BGMVolume || row.option == Option::SFXVolume)
        {
            int vol = (row.option == Option::BGMVolume) ? s_bgm_volume : s_sfx_volume;

            // 슬라이더 배경
            const double dslider_x = static_cast<double>(slider_x_start);
            const double dslider_w = static_cast<double>(slider_width);
            const double dslider_h = static_cast<double>(slider_height);
            Math::vec2 bg_pos   = { dslider_x + dslider_w / 2.0, p.y + 15.0 };
            Math::vec2 bg_scale = { dslider_w, dslider_h };
            renderer_2d->DrawRectangle(Math::TranslationMatrix(bg_pos) * Math::ScaleMatrix(bg_scale), 0x333333FF);

            // 슬라이더 채움
            double fill_ratio = static_cast<double>(vol) / 100.0;
            double fill_width = dslider_w * fill_ratio;
            Math::vec2 fill_pos   = { dslider_x + fill_width / 2.0, p.y + 15.0 };
            Math::vec2 fill_scale = { fill_width, dslider_h };
            renderer_2d->DrawRectangle(Math::TranslationMatrix(fill_pos) * Math::ScaleMatrix(fill_scale), title_main_color);

            // 슬라이더 노브
            double     knob_x    = dslider_x + fill_width;
            double     knob_size = dslider_h * 1.5;
            Math::vec2 knob_pos  = { knob_x, p.y + 15.0 };
            renderer_2d->DrawCircle(
                Math::TranslationMatrix(knob_pos) * Math::ScaleMatrix(Math::vec2{ knob_size, knob_size }),
                selected_color, title_main_color, 0, 0);

            // 퍼센트 텍스트 렌더링 (동적으로 변하는 color 적용)
            text_manager.DrawText(std::to_string(vol) + "%",
                Math::vec2{ dslider_x + dslider_w + 30.0, p.y },
                Fonts::Kings, { 0.6, 0.6 }, color);
        }
        else
        {
            std::string val;
            switch (row.option)
            {
                case Option::MapSize:
                    if      (s_current_map_size == MapSize::Small)  val = "< SMALL >";
                    else if (s_current_map_size == MapSize::Medium) val = "< MEDIUM >";
                    else                                            val = "< LARGE >";
                    break;
                case Option::BGMMute: val = s_is_bgm_muted ? "[MUTED]"  : "[ACTIVE]"; break;
                case Option::SFXMute: val = s_is_sfx_muted ? "[MUTED]"  : "[ACTIVE]"; break;
                case Option::Back:    val = "GO MENU"; break;
                default: break;
            }
            // 값 텍스트 렌더링 (동적으로 변하는 color 적용)
            text_manager.DrawText(val, { static_cast<double>(slider_x_start), p.y }, Fonts::Kings, { 0.8, 0.8 }, color);
        }

        // 선택 항목 반동 화살표
        if (is_sel)
        {
            double bounce = std::sin(run_time * 8.0) * 10.0;
            // 화살표도 텍스트와 색상을 맞추고 싶다면 selected_color 대신 color를 넣으시면 됩니다.
            text_manager.DrawText(">", { p.x - 560.0 - bounce, p.y }, Fonts::Kings, { 0.8, 0.8 }, color);
        }
    }

    GameCursor::Draw();
    renderer_2d->EndScene();
}

void Settings::DrawFlames(double time)
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

void Settings::Unload()
{
}

gsl::czstring Settings::GetName() const
{
    return "Settings";
}
