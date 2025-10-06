/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 25, 2025
*/

#pragma once
#include "..\Engine\GameObject.h"

class Asteroid : public CS230::GameObject {
public:
    Asteroid(Math::vec2 start_position);
    GameObjectTypes Type() override { return GameObjectTypes::Asteroid; }
    std::string TypeName() override { return "Asteroid"; }
    bool CanCollideWith(GameObjectTypes other_object_type) override;
    void ResolveCollision(GameObject* other_object) override;
private:
    enum class Animations {
        Bouncing,
        Landing
    };

    Math::vec2 start_position;

    static constexpr double bounce_velocity = 500;

    class State_Bouncing : public CS230::GameObject::State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() { return "Bouncing"; }
    };

    State_Bouncing state_bouncing;

    class State_Landing : public CS230::GameObject::State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() { return "Landing"; }
    };

    State_Landing state_landing;
};

