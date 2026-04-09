/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Particle.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    June 6, 2025
*/
#pragma once
#include "../Game/GameObjectTypes.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "GameStateManager.h"
#include "DrawDepth.h"

namespace CS230
{
	class Particle : public GameObject
	{
	public:
		Particle(const std::filesystem::path& sprite_file);
		void Start(Math::vec2 _position, Math::vec2 _velocity, double max_life, CS200::RGBA _color = CS200::WHITE);
		void Update(double dt) override;
        //NOTE : draw from gameobjectmanager -> draw all, we have lower hierarchy than objects, so we have to use DrawDepth::PARTICLE in draw function right away
		void Draw(Math::TransformationMatrix camera_matrix, unsigned int color = 0xFFFFFFFF, float depth = DrawDepth::PARTICLE) override;

		int DrawPriority() const override
		{
			return 70;
		}

		bool Alive()
		{
			return life > 0;
		}

		GameObjectTypes Type() override
		{
			return GameObjectTypes::Particle;
		}

	private:
		double		life;
		CS200::RGBA real_color;
	};

	template <typename T>
	class ParticleManager : public Component
	{
	public:
		ParticleManager();
		~ParticleManager();
		void Emit(size_t count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread, CS200::RGBA = CS200::WHITE);

	private:
		std::vector<T*> particles;
		size_t			index;
	};

	template <typename T>
	inline ParticleManager<T>::ParticleManager() : index(0)
	{
		for (int i = 0; i < T::MaxCount; ++i)
		{
			std::unique_ptr<T> new_particle = std::make_unique<T>();
			T*				   particle_ptr = new_particle.get();

			Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(std::move(new_particle));

			particles.push_back(particle_ptr);
		}
	}

	template <typename T>
	inline ParticleManager<T>::~ParticleManager()
	{
		// for (T* particle : particles)
		//{
		//     particle->Destroy();
		// }
		particles.clear();
	}

	template <typename T>
	inline void ParticleManager<T>::Emit(size_t count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread, CS200::RGBA color)
	{
		for (size_t i = 0; i < count; ++i)
		{
			if ((particles[i]) && (particles[i]->Alive()))
			{
				Engine::GetLogger().LogEvent("Particle overwritten");
			}
			double angle_variation = 0.0;
			if (spread != 0)
			{
				angle_variation = static_cast<double>((rand() % static_cast<int>(spread * 1024)) / 1024) - spread / 2;
			}
			Math::vec2 random_magnitude	 = direction * static_cast<double>(static_cast<float>((rand() % 1024) / 2048) + 0.5f);
			Math::vec2 particle_velocity = Math::RotationMatrix(angle_variation) * random_magnitude + emitter_velocity;
			particles[index]->Start(emitter_position, particle_velocity, T::MaxLife, color);

			++index;
			if (index >= particles.size())
			{
				index = 0;
			}
		}
	}

}
