#include "pch.h"

/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "GameObjectManager.h"
#include "Logger.h"

void CS230::GameObjectManager::Add(std::unique_ptr<GameObject> object)
{
  objects.emplace_back(std::move(object));
}

void CS230::GameObjectManager::Unload()
{
  objects.clear();
}

void CS230::GameObjectManager::UpdateAll(double dt)
{
  // std::vector<GameObject*> destroy_objects;
  // for (GameObject* object : objects)
  // {
  // 	object->Update(dt);
  // 	if (object->Destroyed() == true)
  // 	{
  // 		destroy_objects.push_back(object);
  // 	}
  // }
  // for (GameObject* destroy_object : destroy_objects)
  // {
  // 	objects.remove(destroy_object);
  // 	delete destroy_object;
  // }

  std::vector<std::list<std::unique_ptr<GameObject>>::iterator> destroy_iterators;

  for (auto it = objects.begin(); it != objects.end(); ++it)
  {
	(*it)->Update(dt);
	if ((*it)->Destroyed())
	{
	  destroy_iterators.push_back(it);
	}
  }

  for (auto it : destroy_iterators)
  {
	objects.erase(it);
  }
}

void CS230::GameObjectManager::SortForDraw()
{
  // objects.sort([](GameObject* a, GameObject* b) { return a->DrawPriority() < b->DrawPriority(); });
}

void CS230::GameObjectManager::DrawAll(Math::TransformationMatrix camera_matrix)
{
  for (auto& object : objects)
  {
	object->Draw(camera_matrix);
  }
}

void CS230::GameObjectManager::CollisionTest()
{
  // for (GameObject* object1 : objects)
  // {
  // 	for (GameObject* object2 : objects)
  // 	{
  // 		if (object1 != object2 && object1->CanCollideWith(object2->Type()))
  // 		{
  // 			if (object1->IsCollidingWith(object2))
  // 			{
  // 				Engine::GetLogger().LogEvent("Collision Detected: " + object1->TypeName() + " and " + object2->TypeName());
  // 				object1->ResolveCollision(object2);
  // 			}
  // 		}
  // 	}
  // }

  for (const auto& object1 : objects)
  {
	for (const auto& object2 : objects)
	{
	  if (object1.get() != object2.get() && object1->CanCollideWith(object2->Type()))
	  {
		if (object1->IsCollidingWith(object2.get()))
		{
		  Engine::GetLogger().LogEvent("Collision Detected: " + object1->TypeName() + " and " + object2->TypeName());
		  object1->ResolveCollision(object2.get());
		}
	  }
	}
  }
}

std::vector<CS230::GameObject*> CS230::GameObjectManager::GetAllRaw() const
{
  std::vector<CS230::GameObject*> raw_pointers;
  raw_pointers.reserve(objects.size());
  for (const auto& obj_ptr : objects)
  {
	raw_pointers.push_back(obj_ptr.get());
  }
  return raw_pointers;
}
