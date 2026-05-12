#include "pch.h"
#include "Settings.h"
#include "CS200/IRenderer2D.h"
#include "CS200/NDC.h"
#include "CS200/RenderingAPI.h"
#include "Engine/SoundManager.h"
#include "Engine/TextManager.h"
#include "Engine/TextureManager.h"
#include "Engine/Input.h"
#include "Engine/Matrix.h"
#include "Engine/GameStateManager.h"
#include <algorithm>

// [데이터 유지] 초기값 설정 (최초 1회만 실행됨)
Settings::MapSize Settings::s_current_map_size = Settings::MapSize::Medium;
int Settings::s_bgm_volume = 80;
int Settings::s_sfx_volume = 80;
bool Settings::s_is_bgm_muted = false;
bool Settings::s_is_sfx_muted = false;

Settings::Settings() : current_option(Option::MapSize) {}

void Settings::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0F0F0FFF);

    title_pos = { 800.0, 800.0 };
    menu_start_pos = { 800.0, 600.0 };

    rows.clear();
    rows.push_back({ "MAP SIZE", Option::MapSize });
    rows.push_back({ "BGM VOLUME", Option::BGMVolume });
    rows.push_back({ "SFX VOLUME", Option::SFXVolume });
    rows.push_back({ "BGM MUTE", Option::BGMMute });
    rows.push_back({ "SFX MUTE", Option::SFXMute });
    rows.push_back({ "BACK", Option::Back });

    flames.resize(80);
    for (auto& f : flames) InitFlame(f, true);

    // 로드 시 현재 설정 적용
    ApplySettings();
}

void Settings::Update(double dt)
{
    run_time += dt;
    CS230::Input& input = Engine::GetInput();
    Math::vec2 mouse_pos = input.GetMousePos();
    int wheel = static_cast<int>(input.GetMouseScroll());

    // 1. 키보드 수직 이동
    if (input.KeyJustReleased(CS230::Input::Keys::Up) || input.KeyJustReleased(CS230::Input::Keys::W)) {
        current_option = static_cast<Option>((static_cast<int>(current_option) - 1 + static_cast<int>(Option::COUNT)) % static_cast<int>(Option::COUNT));
    }
    if (input.KeyJustReleased(CS230::Input::Keys::Down) || input.KeyJustReleased(CS230::Input::Keys::S)) {
        current_option = static_cast<Option>((static_cast<int>(current_option) + 1) % static_cast<int>(Option::COUNT));
    }

    // 2. 키보드/휠 수평 조절
    int dir = 0;
    if (input.KeyJustReleased(CS230::Input::Keys::Left) || input.KeyJustReleased(CS230::Input::Keys::A)) dir = -1;
    if (input.KeyJustReleased(CS230::Input::Keys::Right) || input.KeyJustReleased(CS230::Input::Keys::D)) dir = 1;
    if (wheel != 0) dir = (wheel > 0) ? 1 : -1; // 휠 위로: 증가, 아래로: 감소

    if (dir != 0) {
        if (current_option == Option::MapSize) {
            s_current_map_size = static_cast<MapSize>((static_cast<int>(s_current_map_size) + dir + (int)MapSize::COUNT) % (int)MapSize::COUNT);
        } else if (current_option == Option::BGMVolume) {
            s_bgm_volume = std::clamp(s_bgm_volume + dir * wheel_sensitivity, 0, 100);
            ApplySettings();
        } else if (current_option == Option::SFXVolume) {
            s_sfx_volume = std::clamp(s_sfx_volume + dir * wheel_sensitivity, 0, 100);
            ApplySettings();
        }
    }

    // 3. 확인 및 마우스 상호작용
    for (int i = 0; i < (int)rows.size(); ++i) {
        Math::vec2 p = { menu_start_pos.x, menu_start_pos.y - i * row_spacing };

        // 마우스 호버 감지 영역 (전체 행)
        if (mouse_pos.y > p.y - 30 && mouse_pos.y < p.y + 30) {
            current_option = rows[i].option;

            // 클릭 조작
            if (input.MouseJustPressed(0)) {
                if (current_option == Option::MapSize) {
                    s_current_map_size = static_cast<MapSize>((static_cast<int>(s_current_map_size) + 1) % (int)MapSize::COUNT);
                } else if (current_option == Option::BGMMute) {
                    s_is_bgm_muted = !s_is_bgm_muted;
                } else if (current_option == Option::SFXMute) {
                    s_is_sfx_muted = !s_is_sfx_muted;
                } else if (current_option == Option::Back) {
                    Engine::GetGameStateManager().PopState();
                    return;
                }
                ApplySettings();
            }

            // 볼륨 바 드래그 조작
            if (input.MouseDown(0)) {
                if (current_option == Option::BGMVolume || current_option == Option::SFXVolume) {
                    float rel_x = std::clamp((float)mouse_pos.x - slider_x_start, 0.0f, slider_width);
                    int new_val = static_cast<int>((rel_x / slider_width) * 100.0f);
                    if (current_option == Option::BGMVolume) s_bgm_volume = new_val;
                    else s_sfx_volume = new_val;
                    ApplySettings();
                }
            }
        }
    }

    if (input.KeyJustReleased(CS230::Input::Keys::Enter) || input.KeyJustReleased(CS230::Input::Keys::Z)) {
        if (current_option == Option::Back) Engine::GetGameStateManager().PopState();
        else if (current_option == Option::BGMMute) { s_is_bgm_muted = !s_is_bgm_muted; ApplySettings(); }
        else if (current_option == Option::SFXMute) { s_is_sfx_muted = !s_is_sfx_muted; ApplySettings(); }
    }

    if (input.KeyJustReleased(CS230::Input::Keys::Escape)) Engine::GetGameStateManager().PopState();
}

void Settings::Draw()
{
    CS200::RenderingAPI::Clear();
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    renderer_2d->BeginScene(CS200::build_ndc_matrix(default_window_size));

    DrawFlames(run_time);
    auto& text_manager = Engine::GetTextManager();

    // 타이틀
    Math::ivec2 title_size = text_manager.CalculateTextSize("SETTINGS", Fonts::Kings);
    Math::vec2 t_adj = { title_pos.x - title_size.x * 0.75, title_pos.y };
    text_manager.DrawText("SETTINGS", t_adj + Math::vec2{5, -5}, Fonts::Kings, {1.5, 1.5}, title_shadow_color);
    text_manager.DrawText("SETTINGS", t_adj, Fonts::Kings, {1.5, 1.5}, title_main_color);

    for (int i = 0; i < (int)rows.size(); ++i) {
        const auto& row = rows[i];
        bool is_sel = (row.option == current_option);
        CS200::RGBA color = is_sel ? selected_color : non_selected_color;
        Math::vec2 p = { menu_start_pos.x, menu_start_pos.y - i * row_spacing };

        // 라벨
        text_manager.DrawText(row.label, { p.x - 500, p.y }, Fonts::Kings, {0.8, 0.8}, color);

        // 값 및 위젯 렌더링
        if (row.option == Option::BGMVolume || row.option == Option::SFXVolume) {
           int vol = (row.option == Option::BGMVolume) ? s_bgm_volume : s_sfx_volume;
            
            // 1. 슬라이더 배경 (어두운 회색)
            Math::vec2 bg_pos = Math::vec2{ static_cast<double>(slider_x_start + slider_width / 2.0f), p.y + 15.0 };
            Math::vec2 bg_scale = Math::vec2{ static_cast<double>(slider_width), static_cast<double>(slider_height) };
            renderer_2d->DrawRectangle(Math::TranslationMatrix(bg_pos) * Math::ScaleMatrix(bg_scale), 0x333333FF);

            // 2. 슬라이더 채우기 (진한 빨강)
            float fill_ratio = vol / 100.0f;
            float fill_width = slider_width * fill_ratio;
            Math::vec2 fill_pos = Math::vec2{ static_cast<double>(slider_x_start + fill_width / 2.0f), p.y + 15.0 };
            Math::vec2 fill_scale = Math::vec2{ static_cast<double>(fill_width), static_cast<double>(slider_height) };
            renderer_2d->DrawRectangle(Math::TranslationMatrix(fill_pos) * Math::ScaleMatrix(fill_scale), title_main_color);

            // 3. [추가] 조절용 포인트 (Knob)
            // 채워진 막대의 끝 지점에 원형 포인트를 그려 현재 위치를 강조합니다.
            float knob_x = slider_x_start + fill_width;
            Math::vec2 knob_pos = Math::vec2{ static_cast<double>(knob_x), p.y + 15.0 };
            float knob_size = slider_height * 1.5f; // 막대보다 조금 더 크게 설정
            
            // 포인트의 테두리는 금색(selected_color), 안쪽은 빨간색으로 포인트 주기
            renderer_2d->DrawCircle(Math::TranslationMatrix(knob_pos) * Math::ScaleMatrix(Math::vec2{knob_size, knob_size}), 
                                    selected_color, // 테두리 또는 전체 색상
                                    title_main_color, 
                                    0, 0);

            // 4. 퍼센트 텍스트
            Math::vec2 text_pos = Math::vec2{ static_cast<double>(slider_x_start + slider_width + 30.0f), p.y };
            text_manager.DrawText(std::to_string(vol) + "%", text_pos, Fonts::Kings, Math::vec2{ 0.6, 0.6 }, color);
        } else {
            std::string val = "";
            switch(row.option) {
                case Option::MapSize:
                    if (s_current_map_size == MapSize::Small) val = "< SMALL >";
                    else if (s_current_map_size == MapSize::Medium) val = "< MEDIUM >";
                    else val = "< LARGE >";
                    break;
                case Option::BGMMute: val = s_is_bgm_muted ? "[MUTED]" : "[ACTIVE]"; break;
                case Option::SFXMute: val = s_is_sfx_muted ? "[MUTED]" : "[ACTIVE]"; break;
                case Option::Back: val = "GO MENU"; break;
            }
            text_manager.DrawText(val, { slider_x_start, p.y }, Fonts::Kings, {0.8, 0.8}, color);
        }

        if (is_sel) {
            double bounce = std::sin(run_time * 8.0) * 10.0;
            text_manager.DrawText(">", { p.x - 560 - bounce, p.y }, Fonts::Kings, {0.8, 0.8}, selected_color);
        }
    }
    renderer_2d->EndScene();
}

void Settings::ApplySettings() {
    auto& sound = Engine::GetSoundManager();
    sound.SetBGMVolume(s_is_bgm_muted ? 0.0f : (s_bgm_volume / 100.0f));
    sound.SetSFXVolume(s_is_sfx_muted ? 0.0f : (s_sfx_volume / 100.0f));
}

void Settings::InitFlame(Flame& f, bool random_y) {
    f.pos.x = (rand() % default_window_size.x);
    f.pos.y = random_y ? (rand() % default_window_size.y) : (-50.0 - (rand() % 150));
    f.size = 2.0 + (rand() % 4);
    f.speed_y = 1.0 + ((rand() % 40) / 10.0);
    f.speed_x = ((rand() % 100) - 50) / 100.0;
    f.flicker_offset = (rand() % 1000) / 10.0;
    int cp = rand() % 3;
    f.color = (cp == 0) ? 0xFFD20000 : (cp == 1 ? 0xFF640000 : 0xFF190000);
}

void Settings::DrawFlames(double time) {
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
    for (auto& f : flames) {
        f.pos.y += f.speed_y;
        f.pos.x += f.speed_x + std::sin(time * 3.0 + f.flicker_offset) * 0.8;
        if (f.pos.y > default_window_size.y + 50) InitFlame(f, false);
        uint8_t a = static_cast<uint8_t>((0.3 + 0.7 * (0.5 + 0.5 * std::sin(time * 15.0 + f.flicker_offset))) * 255.0);
        renderer_2d->DrawCircle(Math::TranslationMatrix(f.pos) * Math::ScaleMatrix(Math::vec2{f.size, f.size}), f.color | a, f.color | a, 0, 0);
    }
}

void Settings::Unload() {}
gsl::czstring Settings::GetName() const { return "Settings"; }

// MapSize 열거형을 MapId 문자열로 변환
std::string Settings::OptionToMapId(MapSize size)
{
    switch (size)
    {
        case MapSize::Small:  return "first_map";  // 8x8
        case MapSize::Medium: return "medium_map"; // 10x10
        case MapSize::Large:  return "large_map";  // 12x12
        default:              return "medium_map";
    }
}

// 현재 설정된 맵 ID를 반환하는 래퍼 함수
std::string Settings::GetCurrentMapId()
{
    return OptionToMapId(s_current_map_size);
}

void Settings::DrawImGui() {}