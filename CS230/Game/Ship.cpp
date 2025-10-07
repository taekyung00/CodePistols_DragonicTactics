/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 24, 2025
*/

#pragma once
#include "Ship.h"
#include "../Engine/Engine.h"


Ship::Ship(Math::vec2 start_position) :
    start_position(start_position),
    GameObject(start_position,0,{scale,scale}),
    isPressed(false),
    exploded(false),
    flame_left("Assets/Flame.spt",this),
    flame_right("Assets/Flame.spt",this)
{
    AddGOComponent(new CS230::Sprite("Assets/Ship.spt",this));
    AddGOComponent(new ScreenWrap(*this));
    flame_left.PlayAnimation(static_cast<int>(Flame_Animations::Off));
    flame_right.PlayAnimation(static_cast<int>(Flame_Animations::Off));
    isPressed = false;
}

void Ship::Update(double dt) {
    flame_left.Update(dt);
    flame_right.Update(dt);
    if (isPressed == false) {
        flame_left.PlayAnimation(static_cast<int>(Flame_Animations::Off));
        flame_right.PlayAnimation(static_cast<int>(Flame_Animations::Off));
    }

    if (exploded == false) {
        if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Enter) ) {
            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(
                new Laser(
                    GetMatrix() * Math::vec2(GetGOComponent<CS230::Sprite>()->GetHotSpot(3)),
                    GetRotation(),
                    GetScale(),
                    Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, Laser::DefaultVelocity }));
            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(
                new Laser(
                    GetMatrix() * Math::vec2(GetGOComponent<CS230::Sprite>()->GetHotSpot(4)),
                    GetRotation(),
                    GetScale(),
                    Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, Laser::DefaultVelocity }));
        }

        if (Engine::GetInput().KeyDown(CS230::Input::Keys::A)) {
            UpdateRotation(rotation_speed * dt);
        }

        if (Engine::GetInput().KeyDown(CS230::Input::Keys::D)) {
            UpdateRotation(-rotation_speed * dt);
        }
    }
    
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::W) && (exploded == false)) {
        UpdateVelocity(Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, speed * dt });
        if (isPressed == false) {
            flame_left.PlayAnimation(static_cast<int>(Flame_Animations::On));
            flame_right.PlayAnimation(static_cast<int>(Flame_Animations::On));
            isPressed = true;
        }  
    }
    else {
        isPressed = false;
    }
    UpdateVelocity({ -drag*dt*GetVelocity()});
    UpdatePosition(dt * GetVelocity());
    UpdateGOComponents(dt);

}

void Ship::Draw(Math::TransformationMatrix camera_matrix) {
    if (exploded == false) {
        flame_left.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(GetGOComponent<CS230::Sprite>()->GetHotSpot(1)));
        flame_right.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(GetGOComponent<CS230::Sprite>()->GetHotSpot(2)));
    }
    
    GetGOComponent<CS230::Sprite>()->Draw(camera_matrix * GetMatrix());
    CS230::Collision* collision = GetGOComponent<CS230::Collision>();
    CS230::ShowCollision* showcollision = Engine::GetGameStateManager().GetGSComponent<CS230::ShowCollision>();
    if ((collision != nullptr) && (showcollision->Enabled() == true)) {
        collision->Draw(camera_matrix);
    }
}

bool Ship::CanCollideWith([[maybe_unused]]GameObjectTypes other_object_type)
{
    return true;
}

void Ship::ResolveCollision([[maybe_unused]]GameObject* other_object) {
    GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Explosion));
    exploded = true;
    RemoveGOComponent<CS230::Collision>();
}
