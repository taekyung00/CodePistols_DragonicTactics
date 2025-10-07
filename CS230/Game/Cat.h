/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 24, 2025
*/

#pragma once
#include "..\Engine\Input.h"
#include "..\Engine\Camera.h"
#include "..\Engine\Matrix.h"
#include "..\Engine\GameObject.h"
#include "..\Engine\Collision.h"
#include "..\Engine\Timer.h"
#include "Portal.h"
#include "States.h"


class Cat : public CS230::GameObject{
public:
    
    Cat(Math::vec2 start_position,GameObject* standing_on);
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
    GameObjectTypes Type() override { return GameObjectTypes::Cat; }
    std::string TypeName() override { return "Cat"; }
    bool CanCollideWith(GameObjectTypes other_object_type) override;
    const Math::vec2& GetPosition() const { return GameObject::GetPosition(); }
    void ResolveCollision(GameObject* other_object) override;
    static constexpr double  LargeFallHeight = 250.0;
private:
    static constexpr double hurt_time = 2.0;
    static constexpr double hurt_velocity = 350;
    static constexpr double hurt_acceleration = 300;

    static constexpr double bounce_velocity = 700;
    static constexpr double pop_velocity = 400;

    CS230::Timer* hurt_timer;
    GameObject* standing_on;
    enum class Animations {
        Idle,
        Running,
        Jumping,
        Falling,
        Skidding
    };

    Math::vec2 start_position;


    static constexpr double x_acceleration = 200.0;
    static constexpr double x_drag = 450.0;
    static constexpr double max_velocity = 300.0;
    static constexpr double jump_velocity = 650.0;

    double* falling_height = nullptr;
    

    void update_x_velocity(double dt);

    class State_Jumping : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Jumping"; }
    };

    State_Jumping state_jumping;

    class State_Idle : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Idle"; }
    };

    State_Idle state_idle;

    class State_Falling : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Falling"; }
    };

    State_Falling state_falling;

    class State_Running : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Running"; }
    };

    State_Running state_running;

    class State_Skidding : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Skidding"; }
    };

    State_Skidding state_skidding;
};

