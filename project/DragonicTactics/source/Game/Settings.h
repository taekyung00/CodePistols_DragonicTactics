#pragma once

#include "../Engine/Engine.h"
#include "../Engine/GameState.h"
#include "../Engine/Fonts.h"
#include "../Engine/Vec2.h"
#include <vector>
#include <string>

class Settings : public CS230::GameState
{
public:
    Settings();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;
    void DrawImGui() override;
    gsl::czstring GetName() const override;

    static constexpr CS200::RGBA title_main_color   = 0x960000FF;
    static constexpr CS200::RGBA title_shadow_color = 0xFFC800FF;
    static constexpr CS200::RGBA non_selected_color = 0x787878FF;
    static constexpr CS200::RGBA selected_color     = 0xFFC800FF;

    // [데이터 유지] static 선언으로 상태 유지
    enum class MapSize { Small, Medium, Large, COUNT };
    static MapSize s_current_map_size;
    static int s_bgm_volume;
    static int s_sfx_volume;
    static bool s_is_bgm_muted;
    static bool s_is_sfx_muted;

private:
    enum class Option {
        MapSize,
        BGMVolume,
        SFXVolume,
        BGMMute,
        SFXMute,
        Back,
        COUNT
    };
    Option current_option;

    struct SettingRow {
        std::string label;
        Option option;
    };
    std::vector<SettingRow> rows;

    Math::vec2 title_pos;
    Math::vec2 menu_start_pos;
    double row_spacing = 80.0;

    static constexpr Math::ivec2 default_window_size = { 1600, 900 };

    // 볼륨 슬라이더 설정
    const float slider_x_start = 900.0f;
    const float slider_width = 300.0f;
    const float slider_height = 15.0f;
    const int wheel_sensitivity = 5;

    // 불꽃 효과
    struct Flame { Math::vec2 pos; double size; double speed_y; double speed_x; double flicker_offset; CS200::RGBA color; };
    std::vector<Flame> flames;
    void InitFlame(Flame& f, bool random_y);
    void DrawFlames(double time);

    double run_time = 0.0;
    void ApplySettings();
    static std::string GetCurrentMapId();
    static std::string OptionToMapId(MapSize opt);
};