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

#include "../Engine/Engine.h"
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"

#include "Fonts.h"

class MainMenu : public CS230::GameState {
public:
    MainMenu();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;
    

    std::string GetName() override {
        return "MainMenu";
    }
    static constexpr unsigned int title_color = 0x9A2EFEFF;
    static constexpr unsigned int non_seleted_color = 0x3ADF00FF;
    static constexpr unsigned int seleted_color = 0xFFFFFFFF;
private:
    CS230::Texture* title_texture;
    CS230::Texture* side_scroller_texture;
    CS230::Texture* space_shooter_texture;
    CS230::Texture* exit_texture;

    unsigned int side_scroller_color ;
    unsigned int space_shooter_color ;
    unsigned int exit_color ;

    enum class Option
    {
        side_scroller,
        space_shooter,
        exit
    };
    Option current_option;
    void update_textures();

};
