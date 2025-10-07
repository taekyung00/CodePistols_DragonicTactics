/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 29, 2025
*/
#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/GameObjectManager.h"
#include "../Engine/Particle.h"


#include "Particles.h"
#include "Score.h"


class Meteor : public CS230::GameObject {
public:
    Meteor(Math::vec2 ship_pos);
    Meteor(Math::vec2 ship_pos, Meteor* parent);
    void Update(double dt) override;

    GameObjectTypes Type() override { return GameObjectTypes::Meteor; }
    std::string TypeName() override { return "Meteor"; }
    bool CanCollideWith(GameObjectTypes other_object_type) override;
    void ResolveCollision(GameObject* other_object) override;
private:
    enum class Animations {
        None,
        Fade
    };

    double health;
    int size;
    static constexpr double default_velocity = 100;
    static constexpr int default_size = 2;
    static constexpr double default_healths[] = { 3, 5, 8 };
    static constexpr double default_scales[] = { 0.5, 0.75, 1.0 };
};
