/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Engine/GameObject.h"
#include "Engine/Matrix.h"

class Samurai : public CS230::GameObject
{
public:
	Samurai();

	GameObjectTypes Type() override
	{
		return GameObjectTypes::Samurai;
	}

	std::string TypeName() override
	{
		return "Samurai";
	}

	bool CanCollideWith([[maybe_unused]] GameObjectTypes other_object_type) override
	{
		return true;
	};

	void ResolveCollision([[maybe_unused]] GameObject* other_object) override { };
	void Update(double dt) override;
	void Draw(Math::TransformationMatrix camera_matrix) override;

private:
	Math::vec2 velocity = { 100.0, 100.0 };
	static constexpr double ROTATIONSPEED = 2;
	static constexpr double SPEED		  = 500;
	static constexpr double DRAG		  = 1;
};