/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/

#pragma once
#include <vector>
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Matrix.h"
#include "../Engine/Camera.h"
#include "../Engine/GameObjectManager.h"
#include "../Engine/Font.h"
#include "../Engine/Timer.h"
#include "../Engine/Particle.h"

#include "Fonts.h"
#include "Background.h"
#include "Gravity.h"
#include "Floor.h"
#include "Particles.h"

class Cat;
class Mode1 : public CS230::GameState {
public:
    Mode1();
    void Load() override;
    void Update(double) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "Mode1";
    }
    static constexpr double gravity = 800;
    static constexpr double floor = 80;
    static constexpr double timer_max = 60;
private:
    Cat* cat_ptr;
    Floor* starting_floor_ptr ;
    CS230::Texture* timer_texture;
    CS230::Texture* score_texture;
    int last_timer;
    void update_timer_text(int value);
    void update_score_text();
    int score;
};
