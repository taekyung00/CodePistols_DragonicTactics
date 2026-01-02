/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ComponentManager.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 17, 2025
*/
#pragma once
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Component.h"

namespace CS230
{
  class ComponentManager
  {
public:

	void UpdateAll(double dt)
	{
	  for (const auto& component : components)
	  {
		component->Update(dt);
	  }
	}

	void AddComponent(Component* component)
	{
	  components.emplace_back(component);
	}

	template <typename T>
	T* GetComponent()
	{
	  for (const auto& component : components)
	  {
		T* ptr = dynamic_cast<T*>(component.get());
		if (ptr != nullptr)
		{
		  return ptr;
		}
	  }
	  return nullptr;
	}

	template <typename T>
	void RemoveComponent()
	{
	  auto it = std::find_if(components.begin(), components.end(), [](const std::unique_ptr<Component>& component) { return dynamic_cast<T*>(component.get()) != nullptr; });

	  if (it != components.end())
	  {
		components.erase(it);
	  }
	}

	void Clear()
	{
	  components.clear();
	}

private:
	std::vector<std::unique_ptr<Component>> components;
  };
}
