/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 25, 2025
*/

#pragma once
#include "Asteroid.h"
#include "../Engine/Engine.h"
#include "Mode1.h"

Asteroid::Asteroid(Math::vec2 start_position) :
	GameObject(start_position)
{
	current_state = &state_landing;
	AddGOComponent(new CS230::Sprite("Assets/Asteroid.spt",this));
	current_state->Enter(this);
}

bool Asteroid::CanCollideWith( GameObjectTypes other_object_type)
{
	if (other_object_type == GameObjectTypes::Floor) {
		return true;
	}
	return false;
}

void Asteroid::ResolveCollision(GameObject* other_object) {
	
	if (other_object->Type() == GameObjectTypes::Floor) {
		Math::rect other_rect = other_object->GetGOComponent<CS230::RectCollision>()->WorldBoundary();
		SetPosition({ GetPosition().x, other_rect.Top() });
		SetVelocity({ 0, 0 });
		change_state(&state_landing);
	}
}
void Asteroid::State_Bouncing::Enter(GameObject* object)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Bouncing));
	asteroid->SetVelocity({ asteroid->GetVelocity().x,bounce_velocity });
}

void Asteroid::State_Bouncing::Update(GameObject* object, double dt)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->UpdateVelocity({0,-Engine::GetGameStateManager().GetGSComponent<Gravity>()->GetValue() * dt });
}

void Asteroid::State_Bouncing::CheckExit([[maybe_unused]]GameObject* object)
{
	/*Asteroid* asteroid = static_cast<Asteroid*>(object);
	if (asteroid->GetPosition().y <= Mode1::floor) {
		asteroid->SetPosition({ asteroid->GetPosition().x, Mode1::floor });
		asteroid->SetVelocity({ 0, 0 });
		asteroid->change_state(&asteroid->state_landing);
	}*/
}

void Asteroid::State_Landing::Enter(GameObject* object)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Landing));
	asteroid->SetVelocity({asteroid->GetVelocity().x,0});
	//asteroid->SetPosition({asteroid->GetPosition().x,Mode1::floor }) ;
}

void Asteroid::State_Landing::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt){}

void Asteroid::State_Landing::CheckExit(GameObject* object)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	if (asteroid->GetGOComponent<CS230::Sprite>()->AnimationEnded()) {
		asteroid->change_state(&asteroid->state_bouncing);
	}
	
}
