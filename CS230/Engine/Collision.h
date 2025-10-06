/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Collision.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 18, 2025
*/
#pragma once
#include "Component.h"
#include "Rect.h"
#include "GameObject.h"
#include "Matrix.h"

namespace Math {
    class TransformationMatrix;
}

namespace CS230 {
    class GameObject;

    class Collision : public Component {
    public:
        enum class CollisionShape {
            Rect,
            Circle
        };
        virtual CollisionShape Shape() = 0;
        virtual void Draw(Math::TransformationMatrix display_matrix) = 0;
        virtual bool IsCollidingWith(GameObject* other_object) = 0;
        virtual bool IsCollidingWith(Math::vec2 point) = 0;
    };

    class RectCollision : public Collision {
    public:
        RectCollision(Math::irect boundary, GameObject* object);
        CollisionShape Shape() override {
            return CollisionShape::Rect;
        }
        void Draw(Math::TransformationMatrix display_matrix);
        Math::rect WorldBoundary();
        bool IsCollidingWith(GameObject* other_object) override;
        bool IsCollidingWith(Math::vec2 point) override;
    private:
        GameObject* object;
        Math::irect boundary;
    };

    class CircleCollision : public Collision {
    public:
        CircleCollision(double radius, GameObject* object);
        CollisionShape Shape() override {
            return CollisionShape::Circle;
        }

        void Draw(Math::TransformationMatrix display_matrix);
        double GetRadius();
        bool IsCollidingWith(GameObject* other_object) override;
        bool IsCollidingWith(Math::vec2 point) override;
    private:
        GameObject* object;
        double radius;
    };

}

