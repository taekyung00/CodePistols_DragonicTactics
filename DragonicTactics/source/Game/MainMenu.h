/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MainMenu.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 6, 2025
*/

#pragma once

#include "../Engine/Engine.hpp"
#include "../Engine/GameState.hpp"
#include "../Engine/Texture.hpp"
#include "../Engine/Font.hpp"

#include "../Engine/Fonts.h"

class MainMenu : public CS230::GameState {
public:
    MainMenu();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;
    static constexpr CS200::RGBA  title_color       = 0x9A2EFEFF;
    static constexpr CS200::RGBA  non_seleted_color = 0xFFFFFFFF;
    static constexpr CS200::RGBA  seleted_color     = 0x3ADF00FF;

private:
enum class Option
    {
        cs230_final,
        dragonic_tactics,
        exit
    };
    Option current_option;

    CS200::RGBA cs230_final_color ;
    CS200::RGBA dragonic_tactics_color ;
    CS200::RGBA exit_color;

    void update_colors();

    static constexpr Math::ivec2 default_window_size = {800,600};
};
