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
#include "GameStateManager.hpp"

namespace CS230
{
    class Particle : public GameObject
    {
    public:
        Particle(const std::filesystem::path& sprite_file);
        void Start(Math::vec2 _position, Math::vec2 _velocity, double max_life);
        void Update(double dt) override;
        void Draw(Math::TransformationMatrix camera_matrix) override;

        bool Alive()
        {
            return life > 0;
        }

        GameObjectTypes Type() override
        {
            return GameObjectTypes::Particle;
        }

    private:
        double life;
    };

    template <typename T>
    class ParticleManager : public Component
    {
    public:
        ParticleManager();
        ~ParticleManager();
        void Emit(size_t count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread);

    private:
        std::vector<T*> particles;
        size_t             index;
    };

    template <typename T>
    inline ParticleManager<T>::ParticleManager() : index(0)
    {
        for (int i = 0; i < T::MaxCount; ++i)
        {
            T* new_particle = new T();

            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(new_particle);

            particles.push_back(new_particle);
        }
    }

    template <typename T>
    inline ParticleManager<T>::~ParticleManager()
    {
        //for (T* particle : particles)
        //{
        //    particle->Destroy();
        //}
        particles.clear();
    }

    template <typename T>
    inline void ParticleManager<T>::Emit(size_t count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if ((particles[i])&&(particles[i]->Alive()))
            {
                Engine::GetLogger().LogEvent("Particle overwritten");
            }
            double angle_variation = 0.0;
            if (spread != 0)
            {
                angle_variation = static_cast<double>((rand() % static_cast<int>(spread * 1024)) / 1024) - spread / 2;
            }
            Math::vec2 random_magnitude  = direction * static_cast<double>(static_cast<float>((rand() % 1024) / 2048) + 0.5f);
            Math::vec2 particle_velocity = Math::RotationMatrix(angle_variation) * random_magnitude + emitter_velocity;
            particles[index]->Start(emitter_position, particle_velocity, T::MaxLife);

            ++index;
            if (index >= particles.size())
            {
                index = 0;
            }
        }
    }

}
