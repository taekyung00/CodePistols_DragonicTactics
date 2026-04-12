/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Matrix.h"
#include <list>
#include <memory>

namespace Math
{
  class TransformationMatrix;
}

namespace CS230
{
  class GameObjectManager : public CS230::Component
  {
public:
	void Add(std::unique_ptr<GameObject> object);
	void Unload();

	void UpdateAll(double dt);
	void SortForDraw();
	void DrawAll(Math::TransformationMatrix camera_matrix);

	void CollisionTest();

	const std::list<std::unique_ptr<GameObject>>& GetAll() const
	{
	  return objects;
	}

	std::vector<GameObject*> GetAllRaw() const;

private:
	std::list<std::unique_ptr<GameObject>> objects;
  };
}
