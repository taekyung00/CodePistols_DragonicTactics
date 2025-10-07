/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 29, 2025
*/

#include "Meteor.h"
//Meteor::Meteor(Math::ivec2 window_size) : 
//	CS230::GameObject({
//        (static_cast<double>(rand()) / RAND_MAX) * window_size.x,
//        (static_cast<double>(rand()) / RAND_MAX) * window_size.y })
//{
//    AddGOComponent(new CS230::Sprite("Assets/Meteor.spt",this));
//    SetPosition({
//        (static_cast<double>(rand()) / RAND_MAX) * window_size.x,
//        (static_cast<double>(rand()) / RAND_MAX) * window_size.y });
//	SetVelocity({
//		(static_cast<double>(rand()) / RAND_MAX) * 2 * default_velocity - (default_velocity) ,
//        (static_cast<double>(rand()) / RAND_MAX) * 2 * default_velocity - (default_velocity) });
//	SetRotation((static_cast<double>(rand()) / RAND_MAX) * 2 * PI);
//    Engine::GetLogger().LogDebug("Rotation: " + std::to_string(GetRotation()));
//}

Meteor::Meteor(Math::vec2 ship_pos) : Meteor(ship_pos,nullptr){}

Meteor::Meteor(Math::vec2 ship_pos, Meteor* parent) : CS230::GameObject({ 0, 0 }) {
    AddGOComponent(new CS230::Sprite("Assets/Meteor.spt", this));
    if (parent == nullptr) {
        while (1)
        {
            Math::vec2 temp = {
            ((double)rand() / RAND_MAX) * (Engine::GetWindow().GetSize().x + 300) - 150,
            ((double)rand() / RAND_MAX) * (Engine::GetWindow().GetSize().y + 300) - 150
            };
            if (
                (temp.x > GetScreenWidth() + GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2) ||
                (temp.x < -GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2) ||
                (temp.y > GetScreenHeight() + GetGOComponent<CS230::Sprite>()->GetFrameSize().y / 2) ||
                (temp.y < -GetGOComponent<CS230::Sprite>()->GetFrameSize().y / 2)
                ) {
                SetPosition(temp);
                
                break;
            }
        }
        
        Math::vec2 directional_vec = (GetPosition() - ship_pos);
        directional_vec.Normalize();
        SetVelocity(Math::vec2{
            directional_vec.x*(((double)rand() / RAND_MAX) * default_velocity /** 2 - default_velocity*/),
            directional_vec.y*(((double)rand() / RAND_MAX) * default_velocity /** 2 - default_velocity*/)
            });
        
        SetRotation(((double)rand() / RAND_MAX) * 2 * PI);

        size = default_size;
    }
    else {
        SetVelocity(parent->GetVelocity());
        SetPosition(parent->GetPosition());
        SetRotation(parent->GetRotation());

        size = parent->size - 1;
    }
    health = default_healths[size];
    SetScale(Math::vec2{ default_scales[size], default_scales[size] });
}


void Meteor::Update(double dt){
	GameObject::Update(dt);
    if (GetPosition().x > GetScreenWidth() + GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2) {
        SetPosition({ -static_cast<double>(GetGOComponent<CS230::Sprite>()->GetFrameSize().x) / 2 ,GetPosition().y });
    }

    if (GetPosition().x < -GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2) {
        SetPosition({ static_cast<double>(GetScreenWidth()) + static_cast<double>(GetGOComponent<CS230::Sprite>()->GetFrameSize().x) / 2 ,GetPosition().y });
    }

    if (GetPosition().y > GetScreenHeight() + GetGOComponent<CS230::Sprite>()->GetFrameSize().y / 2) {
        SetPosition({ GetPosition().x,-static_cast<double>(GetGOComponent<CS230::Sprite>()->GetFrameSize().y) / 2 });
    }

    if (GetPosition().y < -GetGOComponent<CS230::Sprite>()->GetFrameSize().y / 2) {
        SetPosition({ GetPosition().x,static_cast<double>(GetScreenHeight()) + static_cast<double>(GetGOComponent<CS230::Sprite>()->GetFrameSize().y) / 2 });
    }
    if (GetGOComponent<CS230::Sprite>()->CurrentAnimation() == static_cast<int>(Animations::Fade)) {
        if (GetGOComponent<CS230::Sprite>()->AnimationEnded() == true) {
            Destroy();
            Engine::GetGameStateManager().GetGSComponent<Score>()->Add(10);
        }
    }
}

bool Meteor::CanCollideWith(GameObjectTypes other_object_type) {
    if (other_object_type == GameObjectTypes::Laser) {
        return true;
    }
    return false;
}

void Meteor::ResolveCollision(GameObject* other_object) {
    if (other_object->Type() == GameObjectTypes::Laser) {
        double radius = GetGOComponent<CS230::CircleCollision>()->GetRadius();

        Math::vec2 contact_point = (other_object->GetPosition() - GetPosition());
        contact_point.Normalize();
        contact_point = contact_point * radius /*/ std::min(GetScale().x, GetScale().y)*/ + GetPosition();
        //??
        Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Hit>>()->Emit(1, contact_point, GetVelocity(), {0, 50}, 0);
        Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::MeteorBit>>()->Emit(5, contact_point, { 0,0 }, { 0, 50 }, PI / 3);
        UpdateVelocity(0.01 * other_object->GetVelocity());
        health -= 1;
        if (health < 0) {
            
            RemoveGOComponent<CS230::Collision>();
            GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Fade));
            if (size > 0) {
                Meteor* meteor_1 = new Meteor({0,0}, this);
                meteor_1->SetVelocity(Math::RotationMatrix(PI / 6) * GetVelocity());

                Meteor* meteor_2 = new Meteor({0,0}, this);
                meteor_2->SetVelocity(Math::RotationMatrix(-PI / 6) * GetVelocity());

                Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(meteor_1);
                Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(meteor_2);
            }
        }

    }
}
