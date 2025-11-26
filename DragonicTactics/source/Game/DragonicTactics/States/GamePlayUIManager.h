/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlayUIManager.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/

#pragma once
#include <vector>
#include <string>
#include "Engine/Vec2.hpp"
#include "Engine/Matrix.hpp"

class GamePlayUIManager {
public:
    void ShowDamageText(int damage, Math::vec2 position, Math::vec2 size);
    void Update(double dt);
    void Draw(Math::TransformationMatrix camera_matrix);

private:
    struct DamageText {
        std::string text;
        Math::vec2 position;
        Math::vec2 size;
        double lifetime;
    };

    std::vector<DamageText> m_damage_texts;
};
