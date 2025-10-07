/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 24, 2025
*/

#pragma once
#include "..\Engine\Input.h"
#include "..\Engine\Matrix.h"
#include "..\Engine\GameObject.h"
#include "..\Engine\Collision.h"
#include "..\Engine\ShowCollision.h"
#include "..\Engine\GameObjectManager.h"

#include "..\Game\ScreenWrap.h"
#include "..\Game\Laser.h"

class Ship : public CS230::GameObject {
public:
    Ship(Math::vec2 start_position);
    void Update(double dt)override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
    GameObjectTypes Type() override { return GameObjectTypes::Ship; }
    std::string TypeName() override { return "Ship"; }
    void ResolveCollision(GameObject* other_object) override;
    bool CanCollideWith(GameObjectTypes other_object_type) override;
    const bool GetExploded() const { return exploded; }
    

private:
    enum class Animations {
        None,
        Explosion
    };

    enum class Flame_Animations {
        Off,
        On
    };
    static constexpr double speed = 700;
    static constexpr double drag = 1;


    CS230::Sprite flame_left;
    CS230::Sprite flame_right;
    Math::vec2 start_position;
    static constexpr double rotation_speed = 5;
    static constexpr double scale = 0.75;

    bool isPressed;
    bool exploded;

};
