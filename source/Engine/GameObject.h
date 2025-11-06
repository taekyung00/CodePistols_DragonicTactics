/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameObject.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 25, 2025
*/

#pragma once
#include "../Game/GameObjectTypes.h"
#include "ComponentManager.h"
#include "ShowCollision.h"
#include "Sprite.h"

namespace Math
{
    class TransformationMatrix;
}

namespace CS230
{
    class Component;

    class GameObject
    {
    public:
        friend class Sprite;
        GameObject(Math::vec2 position);
        GameObject(Math::vec2 position, double rotation, Math::vec2 scale);

        virtual ~GameObject()
        {
        }

        virtual GameObjectTypes Type()     = 0;
        virtual std::string     TypeName() = 0;

        virtual int DrawPriority() const
        {
            return 10;
        }

        bool         IsCollidingWith(GameObject* other_object);
        bool         IsCollidingWith(Math::vec2 point);
        virtual bool CanCollideWith(GameObjectTypes other_object_type);
        virtual void ResolveCollision([[maybe_unused]] GameObject* other_object) { };

        virtual void Update(double dt);
        virtual void Draw(Math::TransformationMatrix camera_matrix);

        const Math::TransformationMatrix& GetMatrix();
        const Math::vec2&                 GetPosition() const;
        const Math::vec2&                 GetVelocity() const;
        const Math::vec2&                 GetScale() const;
        double                            GetRotation() const;

        template <typename T>
        T* GetGOComponent()
        {
            return componentmanager.GetComponent<T>();
        }

        void SetPosition(Math::vec2 new_position);
        void SetVelocity(Math::vec2 new_velocity);
        void UpdateVelocity(Math::vec2 delta);

        const bool& Destroyed() const
        {
            return destroy;
        }

        void Destroy()
        {
            destroy = true;
        }

    protected:

        void UpdatePosition(Math::vec2 delta);


        void SetScale(Math::vec2 new_scale);
        void UpdateScale(Math::vec2 delta);
        void SetRotation(double new_rotation);
        void UpdateRotation(double delta);

        class State
        {
        public:
            virtual void        Enter(GameObject* object)             = 0;
            virtual void        Update(GameObject* object, double dt) = 0;
            virtual void        CheckExit(GameObject* object)         = 0;
            virtual std::string GetName()                             = 0;

            virtual ~State()
            {
            }
        };

        State* current_state;
        void   change_state(State* new_state);

        bool matrix_outdated;

        void AddGOComponent(Component* component)
        {
            componentmanager.AddComponent(component);
        }

        template <typename T>
        void RemoveGOComponent()
        {
            componentmanager.RemoveComponent<T>();
        }

        void ClearGOComponents()
        {
            componentmanager.Clear();
        }

        void UpdateGOComponents(double dt)
        {
            componentmanager.UpdateAll(dt);
        }


    private:
        bool destroy;

        class State_None : public State
        {
        public:
            void Enter(GameObject*) override
            {
            }

            void Update(GameObject*, double) override
            {
            }

            void CheckExit(GameObject*) override
            {
            }

            std::string GetName() override
            {
                return "";
            }

            ~State_None()
            {
            }
        };

        State_None state_none;

        Math::TransformationMatrix object_matrix;


        Math::vec2 position;
        Math::vec2 velocity;
        Math::vec2 scale;
        double     rotation;

        ComponentManager componentmanager;
    };
}
