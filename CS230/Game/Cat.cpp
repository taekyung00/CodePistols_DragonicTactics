/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 24, 2025
*/
#pragma once
#include "..\Engine\Engine.h"

#include "Cat.h"
#include "Mode1.h"

Cat::Cat(Math::vec2 start_position, GameObject* standing_on) :
    start_position(start_position),
    standing_on(standing_on),
    GameObject(start_position)
{
    current_state = &state_idle;
    AddGOComponent(new CS230::Sprite("Assets/Cat.spt",this));
    current_state->Enter(this);
    hurt_timer = new CS230::Timer(0.0);
    AddGOComponent(hurt_timer);
}

void Cat::Update(double dt) {
    GameObject::Update(dt);

    //Engine::GetLogger().LogDebug(std::to_string(hurt_timer->Remaining()));

    // Boundary Check
    CS230::RectCollision* collider = GetGOComponent<CS230::RectCollision>();
    
    if (GetPosition().x <= Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + /*GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2*/(collider->WorldBoundary().Right() - collider->WorldBoundary().Left())/2) {
        double temp_y = GetPosition().y;
        UpdatePosition(-GetPosition());
        UpdatePosition({ Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + /*GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2*/(collider->WorldBoundary().Right() - collider->WorldBoundary().Left()) / 2, temp_y });
        SetVelocity({ 0, GetVelocity().y });
    }
    if (GetPosition().x + (collider->WorldBoundary().Right() - collider->WorldBoundary().Left()) / 2 >= Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + Engine::GetWindow().GetSize().x) {
        double temp_y = GetPosition().y;
        UpdatePosition(-GetPosition());
        UpdatePosition({ Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + Engine::GetWindow().GetSize().x - (collider->WorldBoundary().Right() - collider->WorldBoundary().Left()) / 2, temp_y });
        SetVelocity({ 0, GetVelocity().y });
    }
}

void Cat::Draw(Math::TransformationMatrix camera_matrix) {
    if (hurt_timer->Remaining() == 0.0 || hurt_timer->TickTock() == true) {
        CS230::GameObject::Draw(camera_matrix);
    }
}

bool Cat::CanCollideWith([[maybe_unused]]GameObjectTypes other_object_type)
{
    if (other_object_type == GameObjectTypes::Particle) {
        return false;
    }
    return true;
}

void Cat::ResolveCollision(GameObject* other_object) {
    Math::rect cat_rect = GetGOComponent<CS230::RectCollision>()->WorldBoundary();
    Math::rect other_rect = other_object->GetGOComponent<CS230::RectCollision>()->WorldBoundary();
    switch (other_object->Type())
    {
    case GameObjectTypes::Floor: [[fallthrough]];
    case GameObjectTypes::Crates:
            if (current_state == &state_falling) {
                if (other_object->IsCollidingWith(GetPosition())) {
                    if (cat_rect.Top() > other_rect.Top()) {
                        SetPosition({ GetPosition().x, other_rect.Top() });
                        standing_on = other_object;
                        current_state->CheckExit(this);
                        return;
                    }
                }
            }
            if (cat_rect.Left() < other_rect.Left()) {
                UpdatePosition(Math::vec2{ (other_rect.Left() - cat_rect.Right()), 0.0 });
                SetVelocity({ 0, GetVelocity().y });
            }
            else {
                UpdatePosition(Math::vec2{ (other_rect.Right() - cat_rect.Left()), 0.0 });
                SetVelocity({ 0, GetVelocity().y });
            }
        break;
    case GameObjectTypes::Robot:
        if (current_state == &state_falling && cat_rect.Top() > other_rect.Top() && hurt_timer->Remaining() == 0) {
            Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, GetPosition(), { 0, 0 }, { 0, 100 }, 2 * PI / 3);
            SetPosition({ GetPosition().x, other_rect.Top() });
            SetVelocity({ GetVelocity().x, pop_velocity });
            other_object->ResolveCollision(this);
        }
        else {
            if (cat_rect.Left() < other_rect.Left()) {
                SetVelocity({ -hurt_acceleration, hurt_velocity });
            }
            else {
                SetVelocity({ hurt_acceleration, hurt_velocity });
            }
            change_state(&state_falling);
            hurt_timer->Set(hurt_time);
        }
        standing_on = nullptr;
        break;
    case GameObjectTypes::Asteroid:
        if (current_state == &state_falling && cat_rect.Top() > other_rect.Top() && hurt_timer->Remaining() == 0) {
            other_object->ResolveCollision(this);
            SetPosition({ GetPosition().x, other_rect.Top() });
            SetVelocity({ GetVelocity().x, bounce_velocity });
        }
        else {
            if (cat_rect.Left() < other_rect.Left()) {
                SetVelocity({ -hurt_acceleration, hurt_velocity });
            }
            else {
                SetVelocity({ hurt_acceleration, hurt_velocity });
            }
            change_state(&state_falling);
            hurt_timer->Set(hurt_time);
        }
        standing_on = nullptr;
        break;
    case GameObjectTypes::Portal:
        static_cast<Portal*>(other_object)->GoToState();
        break;
    }
}

void Cat::update_x_velocity(double dt)
{
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        UpdateVelocity({ x_acceleration * dt,0 });
        if (GetVelocity().x  > max_velocity) {
            SetVelocity({ max_velocity,GetVelocity().y });
        }
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        UpdateVelocity({ -x_acceleration * dt,0 });
        if (GetVelocity().x < -max_velocity) {
            SetVelocity({ -max_velocity,GetVelocity().y });
        }
    }
    else {
        if (GetVelocity().x > x_drag * dt) {
            UpdateVelocity({ -x_drag * dt ,0 });
        }
        else if (GetVelocity().x < -x_drag * dt) {
            UpdateVelocity({ x_drag * dt ,0 });
        }
        else {
            SetVelocity({ 0,GetVelocity().y });
        }
    }

}

void Cat::State_Jumping::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Jumping));
    cat->SetVelocity({ cat->GetVelocity().x,Cat::jump_velocity });
    cat->standing_on = nullptr;
}

void Cat::State_Jumping::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->UpdateVelocity({ 0,-Engine::GetGameStateManager().GetGSComponent<Gravity>()->GetValue() * dt });
    cat->update_x_velocity(dt);
}

void Cat::State_Jumping::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Up) == false) {
        cat->change_state(&cat->state_falling);
        cat->SetVelocity({ cat->GetVelocity().x,0 });
    }
    else if (cat->GetVelocity().y <= 0) {
        cat->change_state(&cat->state_falling);
    }
}

void Cat::State_Idle::Enter([[maybe_unused]] GameObject* object){
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Idle));
}

void Cat::State_Idle::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt){}

void Cat::State_Idle::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
        cat->change_state(&cat->state_jumping);
    }

}

void Cat::State_Falling::Enter([[maybe_unused]] GameObject* object){
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Falling));
    delete (cat->falling_height);
    cat->falling_height = new double;
    *(cat->falling_height) = cat->GetPosition().y;
}

void Cat::State_Falling::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->UpdateVelocity({ 0,-Engine::GetGameStateManager().GetGSComponent<Gravity>()->GetValue() * dt });
    cat->update_x_velocity(dt);
}

void Cat::State_Falling::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat->standing_on != nullptr) {
        
        if (cat->GetVelocity().x > 0) {
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
                cat->change_state(&cat->state_skidding);
            }
            else {
                cat->change_state(&cat->state_running);
            }
        }
        else if (cat->GetVelocity().x < 0) {
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
                cat->change_state(&cat->state_skidding);
            }
            else {
                cat->change_state(&cat->state_running);
            }
        }
        else {
            cat->change_state(&cat->state_idle);
        }
        *(cat->falling_height) = *(cat->falling_height)-cat->GetPosition().y;
        if (*(cat->falling_height) >= Cat::LargeFallHeight) {
            Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, cat->GetPosition(), { 0, 0 }, { 0, 100 }, PI / 2);
        }
        cat->SetVelocity({ cat->GetVelocity().x, 0 });
    }

    if (cat->GetPosition().y < -500) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Cat::State_Running::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Running));
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
            cat->SetScale({ 1.0,1.0 });
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
            cat->SetScale({ -1.0,1.0 });
    }
}

void Cat::State_Running::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->update_x_velocity(dt);
}

void Cat::State_Running::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetVelocity().x  == 0) {
        cat->change_state(&cat->state_idle);
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
        cat->change_state(&cat->state_jumping);
    }
    
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        if (cat->GetVelocity().x  < 0) {
            cat->change_state(&cat->state_skidding);
        }
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        if (cat->GetVelocity().x  > 0) {
            cat->change_state(&cat->state_skidding);
        }
    }
    if (cat->standing_on != nullptr && cat->standing_on->IsCollidingWith(cat->GetPosition()) == false) {
        cat->standing_on = nullptr;
        cat->change_state(&cat->state_falling);
    }

}

void Cat::State_Skidding::Enter([[maybe_unused]] GameObject* object){
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Skidding));
    Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, cat->GetPosition(), { 0, 0 }, { 2 * cat->GetVelocity().x, 0 }, PI / 6);
}

void Cat::State_Skidding::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat-> GetVelocity().x  > 0) {
        cat->UpdateVelocity({ -(x_drag + x_acceleration) * dt,0});
    }
    else if (cat->GetVelocity().x < 0) {
        cat->UpdateVelocity({ (x_drag + x_acceleration) * dt,0 });
    }
    cat->update_x_velocity(dt);
}

void Cat::State_Skidding::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat-> GetVelocity().x  == 0) {
        cat->change_state(&cat->state_idle);
    }
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
        cat->change_state(&cat->state_jumping);
    }

    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        if (cat->GetVelocity().x  >= 0) {
            cat->change_state(&cat->state_running);
        }

    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        if (cat->GetVelocity().x <= 0) {
            cat->change_state(&cat->state_running);
        }
    }
}
