#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameObjectManager.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 25, 2025
*/
#include "GameObjectManager.h"
#include "Logger.h"

void CS230::GameObjectManager::Add(GameObject* object){
	objects.emplace_back(object);
}

void CS230::GameObjectManager::Unload(){
	objects.clear();
}

void CS230::GameObjectManager::UpdateAll(double dt){
	for (auto& object : objects) 
	{
        object->Update(dt);
	}
}

void CS230::GameObjectManager::SortForDraw()
{
    //objects.sort([](GameObject* a, GameObject* b) { return a->DrawPriority() < b->DrawPriority(); });
}

void CS230::GameObjectManager::DrawAll(Math::TransformationMatrix camera_matrix){
	for (auto& object : objects ){
		object->Draw(camera_matrix);		
	}
}

void CS230::GameObjectManager::CollisionTest()
{
	using namespace std::literals;
    for (auto& object1 : objects) {
        for (auto& object2 : objects) {
            if (object1.get() != object2.get() && object1->CanCollideWith(object2->Type())) {
                if (object1->IsCollidingWith(object2.get())) {
                    Engine::GetLogger().LogEvent("Collision Detected: "s + object1->TypeName() + " and "s + object2->TypeName());
                    object1->ResolveCollision(object2.get());
                }
    	    }
        }
    }
}
