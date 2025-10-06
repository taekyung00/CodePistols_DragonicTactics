/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Robot.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 29, 2025
*/

#include "Robot.h"
#include "Score.h"

Robot::Robot(Math::vec2 position, Cat* cat, double left_boundary, double right_boundary) :
	CS230::GameObject(position),
	cat_ptr(cat),
	left_boundary(left_boundary),
	right_boundary(right_boundary)
{
	
	AddGOComponent(new CS230::Sprite("Assets/Robot.spt",this));
	//GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
	current_state = &state_walking;
	current_state->Enter(this);
}

void Robot::ResolveCollision(GameObject* other_object) {
	if (other_object->Type() == GameObjectTypes::Cat) {
		Engine::GetGameStateManager().GetGSComponent<Score>()->Add();
		RemoveGOComponent<CS230::RectCollision>();
		//GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Broken));
		change_state(&state_broken);
	}
}

void Robot::State_Broken::Enter(GameObject* object)
{
    Robot* robot = static_cast<Robot*>(object);
    robot->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Broken));
	robot->SetVelocity({ 0,robot->GetVelocity().y });
}

void Robot::State_Broken::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
}

void Robot::State_Broken::CheckExit([[maybe_unused]] GameObject* object)
{
}

void Robot::State_Walking::Enter([[maybe_unused]] GameObject* object) {
    Robot* robot = static_cast<Robot*>(object);
    robot->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Walking));
	if (robot->GetScale().x == -1.0) {
		robot->SetVelocity({ -robot->walking_speed,robot->GetVelocity().y });
	}
	else if (robot->GetScale().x == 1.0) {
		robot->SetVelocity({ -robot->walking_speed,robot->GetVelocity().y });
	}
}

void Robot::State_Walking::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt) {
	//Engine::GetLogger().LogDebug("in walking");

	Robot* robot = static_cast<Robot*>(object);
	//Engine::GetLogger().LogDebug(std::to_string(robot->GetPosition().x));
	
	CS230::RectCollision* robot_rect_collision = robot->GetGOComponent<CS230::RectCollision>();
	if (robot_rect_collision == nullptr) {
		robot->change_state(&robot->state_broken);
		return;
	}
	if ( robot_rect_collision->WorldBoundary().Left() < robot->left_boundary) {
		Engine::GetLogger().LogDebug("skidding to right");
		//robot->SetPosition({ robot->left_boundary + 5,robot->GetPosition().y });
		robot->SetVelocity({ walking_speed,0 });
		robot->SetScale({ 1.0,1.0 });
	}
	if ( robot_rect_collision->WorldBoundary().Right()> robot->right_boundary) {
		Engine::GetLogger().LogDebug("skidding to left");
		//robot->SetPosition({ robot->right_boundary  - 2,robot->GetPosition().y});
		robot->SetVelocity({ -walking_speed,0 });
		robot->SetScale({ -1.0,1.0 });
	}
	
}

void Robot::State_Walking::CheckExit(GameObject* object)
{
	Robot* robot = static_cast<Robot*>(object);
	Math::rect robot_rect = robot->GetGOComponent<CS230::RectCollision>()->WorldBoundary();
	//Cat* cat_ptr = robot->cat_ptr;
	Math::rect cat_rect = robot->cat_ptr->GetGOComponent<CS230::RectCollision>()->WorldBoundary();
	if (cat_rect.Left() >= robot->left_boundary && cat_rect.Right() <= robot->right_boundary) {
		if (cat_rect.Right() <= robot_rect.Left() && robot->GetVelocity().x == -walking_speed) {
			robot->change_state(&robot->state_angry);
		}
		if (cat_rect.Left() >= robot_rect.Right() && robot->GetScale().x == walking_speed) {
			robot->change_state(&robot->state_angry);
		}
	}
}

void Robot::State_Angry::Enter([[maybe_unused]] GameObject* object) {
	Robot* robot = static_cast<Robot*>(object);
	robot->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Angry));
	if (robot->GetScale().x == -1.0) {
		robot->SetVelocity({ -robot->angry_speed,robot->GetVelocity().y });
	}
	else if (robot->GetScale().x == 1.0) {
		robot->SetVelocity({ -robot->angry_speed,robot->GetVelocity().y });
	}
	
}

void Robot::State_Angry::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
	Robot* robot = static_cast<Robot*>(object);
	//Engine::GetLogger().LogDebug(std::to_string(robot->GetPosition().x));

	CS230::RectCollision* robot_rect_collision = robot->GetGOComponent<CS230::RectCollision>();
	if (robot_rect_collision == nullptr) {
		robot->change_state(&robot->state_broken);
		return;
	}
	if (robot_rect_collision->WorldBoundary().Left() < robot->left_boundary) {
		Engine::GetLogger().LogDebug("skidding to right");
		//robot->SetPosition({ robot->left_boundary + 5,robot->GetPosition().y });
		robot->SetVelocity({ angry_speed,0 });
		robot->SetScale({ 1.0,1.0 });
	}
	if (robot_rect_collision->WorldBoundary().Right() > robot->right_boundary) {
		Engine::GetLogger().LogDebug("skidding to left");
		//robot->SetPosition({ robot->right_boundary  - 2,robot->GetPosition().y});
		robot->SetVelocity({ -angry_speed,0 });
		robot->SetScale({ -1.0,1.0 });
	}
}

void Robot::State_Angry::CheckExit([[maybe_unused]] GameObject* object)
{
	Robot* robot = static_cast<Robot*>(object);
	Math::rect robot_rect = robot->GetGOComponent<CS230::RectCollision>()->WorldBoundary();
	Math::rect cat_rect = robot->cat_ptr->GetGOComponent<CS230::RectCollision>()->WorldBoundary();

	if (cat_rect.Left() < robot->left_boundary || cat_rect.Right() > robot->right_boundary) {
		robot->change_state(&robot->state_walking);
	}
}
