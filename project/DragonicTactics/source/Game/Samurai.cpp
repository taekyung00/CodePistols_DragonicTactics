/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Samurai.h"
#include "Engine/Input.h"

Samurai::Samurai() : 
	GameObject({100,100})
{
	AddGOComponent(new CS230::Sprite("Assets/sprites/DemoSceneShowcase/Samurai.spt", this));
	AddGOComponent(new CS230::ShowCollision());
	GetGOComponent<CS230::Sprite>()->PlayAnimation(0);
}

void Samurai::Update([[maybe_unused]] double dt)
{

	if(Engine::GetInput().KeyDown(CS230::Input::Keys::A))
	{
		UpdateRotation(ROTATIONSPEED * dt);
	}
	if(Engine::GetInput().KeyDown(CS230::Input::Keys::D))
	{
		UpdateRotation(-ROTATIONSPEED * dt);
	}

	if (Engine::GetInput().KeyDown(CS230::Input::Keys::W))
	{
		UpdateVelocity(Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, SPEED * dt });
	}
	if (Engine::GetInput().KeyDown(CS230::Input::Keys::S))
	{
		UpdateVelocity(Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, -SPEED * dt });
	}
	UpdateVelocity({ -DRAG * dt * GetVelocity() });
	GameObject::Update(dt);
}

void Samurai::Draw(Math::TransformationMatrix camera_matrix)
{
	GameObject::Draw(camera_matrix);
}
