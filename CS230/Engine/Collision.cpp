/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Component.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 17, 2025
*/

#include "Collision.h"

namespace CS230 {
    RectCollision::RectCollision(Math::irect boundary, CS230::GameObject* object) :
        boundary(boundary),
        object(object)
    {
    }

    void RectCollision::Draw(Math::TransformationMatrix display_matrix)
    {
        const double render_height = rlGetFramebufferHeight();

        Math::rect world_boundary = WorldBoundary();

        Math::vec2 bottom_left = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Bottom() };
        Math::vec2 bottom_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Bottom() };
        Math::vec2 top_left = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Top() };
        Math::vec2 top_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Top() };

        bottom_left.y = bottom_left.y * -1 + render_height;
        bottom_right.y = bottom_right.y * -1 + render_height;
        top_left.y = top_left.y * -1 + render_height;
        top_right.y = top_right.y * -1 + render_height;


        DrawLine(int(top_left.x), int(top_left.y), int(top_right.x), int(top_right.y), WHITE);
        DrawLine(int(bottom_right.x), int(bottom_right.y), int(top_right.x), int(top_right.y), WHITE);
        DrawLine(int(bottom_right.x), int(bottom_right.y), int(bottom_left.x), int(bottom_left.y), WHITE);
        DrawLine(int(top_left.x), int(top_left.y), int(bottom_left.x), int(bottom_left.y), WHITE);

    }

    Math::rect RectCollision::WorldBoundary() {

        return {
            object->GetMatrix() * static_cast<Math::vec2>(boundary.point_1),
            object->GetMatrix() * static_cast<Math::vec2>(boundary.point_2)
        };
    }
    bool RectCollision::IsCollidingWith(GameObject* other_object)
    {
        Collision* other_collider = other_object->GetGOComponent<Collision>();


        if (other_collider == nullptr) {
            //Engine::GetLogger().LogError("No collision component found");
            return false;
        }


        if (other_collider->Shape() != CollisionShape::Rect) {
            Engine::GetLogger().LogError("Rect vs unsupported type");
            return false;
        }

        Math::rect rectangle_1 = WorldBoundary();
        Math::rect rectangle_2 = dynamic_cast<RectCollision*>(other_collider)->WorldBoundary();

        if (!(
            (rectangle_1.Right() <= rectangle_2.Left())   ||
            (rectangle_1.Left()  >= rectangle_2.Right())  ||
            (rectangle_1.Top()   <= rectangle_2.Bottom()) ||
            (rectangle_1.Bottom()>= rectangle_2.Top())
            )) {
            return true;
        }
        return false;
    }
    bool RectCollision::IsCollidingWith(Math::vec2 point)
    {
        Math::rect rectangle = WorldBoundary();
        if (!(
            (rectangle.Left() > point.x) ||
            (rectangle.Right() < point.x) ||
            (rectangle.Top() < point.y) ||
            (rectangle.Bottom() > point.y)
            )
            ) {
            return true;
        }
        return false;
    }
    CircleCollision::CircleCollision(double radius, GameObject* object) : 
        radius(radius), object(object)
    {
    }
    void CircleCollision::Draw(Math::TransformationMatrix display_matrix)
    {
        const double render_height = rlGetFramebufferHeight();
        Math::vec2 transformed_position = display_matrix * object->GetPosition();
        transformed_position.y = transformed_position.y * -1 + render_height;
        const int num_segments = 36;
        Math::vec2 previous_vertex;
        for (int i = 0; i <= num_segments + 1; i++) {
            double theta = 2.0 * PI * static_cast<double>(i) / static_cast<double>(num_segments);
            Math::vec2 vertex = {
                transformed_position.x + GetRadius() * std::cos(theta),
                transformed_position.y + GetRadius() * std::sin(theta)
            };
            if (i > 0) {
                DrawLine(int(vertex.x), int(vertex.y), int(previous_vertex.x), int(previous_vertex.y), WHITE);
            }
            previous_vertex = vertex;
        }

    }
    double CircleCollision::GetRadius()
    {
        return std::min(object->GetScale().x, object->GetScale().x) * radius;
    }
    bool CircleCollision::IsCollidingWith(GameObject* other_object)
    {
        Collision* other_collider = other_object->GetGOComponent<Collision>();


        if (other_collider == nullptr) {
            //Engine::GetLogger().LogError("No collision component found");
            return false;
        }


        if (other_collider->Shape() != CollisionShape::Circle) {
            Engine::GetLogger().LogError("Circle vs unsupported type");
            return false;
        }
        double _radius = GetRadius();
        Math::vec2 position = object->GetPosition();
        double other_radius = dynamic_cast<CircleCollision*>(other_collider)->GetRadius();
        Math::vec2 other_position = other_object->GetPosition();
        
        if (
            ((position.x - other_position.x) * (position.x - other_position.x) +
             (position.y - other_position.y) * (position.y - other_position.y)) <
            ((_radius + other_radius) * (_radius + other_radius))
            ) {
            return true;
        }       
        return false;
    }

    bool CircleCollision::IsCollidingWith(Math::vec2 point) {
        double _radius = GetRadius();
        Math::vec2 position = object->GetPosition();

        if (
            ((position.x - point.x) * (position.x - point.x) +
                (position.y - point.y) * (position.y - point.y)) <
            (_radius  * _radius )
            ) {
            return true;
        }
        return false;
    }
}

