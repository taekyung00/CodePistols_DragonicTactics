/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameObjectManager.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 25, 2025
*/

#pragma once
#include <list>
#include <memory>
#include "GameObject.h"
#include "Matrix.h"
#include "Component.h"

namespace Math { class TransformationMatrix; }

namespace CS230 {
    class GameObjectManager : public CS230::Component{
    public:
        void Add(std::unique_ptr<GameObject> object);
        void Unload();

        void UpdateAll(double dt);
        void SortForDraw();
        void DrawAll(Math::TransformationMatrix camera_matrix);

        void CollisionTest();

        const std::list<std::unique_ptr<GameObject>>& GetAll() const { 
            return objects; 
        }

        std::vector<GameObject*> GetAllRaw() const;
    private:
       std::list<std::unique_ptr<GameObject>> objects;
    };
}
