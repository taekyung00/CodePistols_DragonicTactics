/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/

#pragma once
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/GameObjectManager.h"
#include "../Engine/Particle.h"
#include "../Engine/Timer.h"



#include "Fonts.h"
#include "Ship.h"
#include "Particles.h"
#include "Score.h"

class Mode2 : public CS230::GameState {
public:
    Mode2();
    void Load() override;
    void Update(double) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "Mode2";
    }

private:
    Ship* ship_ptr;
    CS230::Texture* gameover_texture;
    CS230::Texture* reload_texture;

    static constexpr double meteor_timer = 4.5;
    CS230::Texture* score_texture;
    void update_score_text();
    int temp_score;
};
