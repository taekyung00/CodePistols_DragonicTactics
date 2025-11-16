#include "Particle.h"
/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Particle.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    June 6, 2025
*/

namespace CS230 {
	Particle::Particle(const std::filesystem::path& sprite_file) : 
		
		GameObject({0,0}),
		life(0.0)
	{
		AddGOComponent(new Sprite(sprite_file, this));
	}
	void Particle::Start(Math::vec2 _position, Math::vec2 _velocity, double max_life)
	{
		SetPosition(_position);
		SetVelocity(_velocity);
		life = max_life;
		GetGOComponent<Sprite>()->PlayAnimation(0);
	}

	void Particle::Update(double dt) {
		if (Alive()) {
			life -= dt;
			//Engine::GetLogger().LogDebug(std::to_string(life));
			GameObject::Update(dt);
		}
	}

	void Particle::Draw(Math::TransformationMatrix camera_matrix) {
		if (Alive()) {
			GameObject::Draw(camera_matrix);
		}
	}
}