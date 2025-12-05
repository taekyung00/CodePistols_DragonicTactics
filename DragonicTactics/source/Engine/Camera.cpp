/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Camera.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes
Created:    March 8, 2023
*/

#include "pch.h"
#include "Camera.h"
#include "Engine.h"
#include "Matrix.h"

namespace
{
  template <typename T, typename FLOAT = double>
  void ease_to_target(T& current, const T& target, FLOAT delta_time, FLOAT weight = 1.0)
  {
	const auto easing = std::min(delta_time * weight, static_cast<FLOAT>(1.0));
	current += easing * (target - current);
  }
} // namespace

CS230::Camera::Camera(Math::rect _player_zone, size_t camera_amount)
{
  cameras.resize(camera_amount);
  for (CameraSettings& cam : cameras)
  {
	cam.player_zone = _player_zone;
	cam.position	= Math::vec2{ 0.0, 0.0 };
	cam.offset		= Math::vec2{ 0.0, 0.0 };
	cam.rotation	= 0.0;
	cam.scale		= Math::vec2{ 1.0, 1.0 };
  }
}

bool& CS230::Camera::SetFirstPersonView(size_t camera_index)
{
  return cameras[camera_index].first_person_view;
}

bool& CS230::Camera::SetAnchoring(size_t camera_index)
{
  return cameras[camera_index].anchoring;
}

bool& CS230::Camera::SetSmoothing(size_t camera_index)
{
  return cameras[camera_index].enable_smoothing;
}

void CS230::Camera::SetPosition(Math::vec2 new_position, size_t camera_index)
{
  cameras[camera_index].position			 = new_position;
  cameras[camera_index].is_position_outdated = true;
}

void CS230::Camera::SetRotation(double new_rotation, size_t camera_index)
{
  if (cameras[camera_index].first_person_view)
  {
	cameras[camera_index].rotation			   = new_rotation;
	cameras[camera_index].is_rotation_outdated = true;
  }
}

void CS230::Camera::SetScale(Math::vec2 new_scale, size_t camera_index)
{
  cameras[camera_index].scale			  = new_scale;
  cameras[camera_index].is_scale_outdated = true;
}

void CS230::Camera::SetPositionOffset(Math::vec2 new_offset, size_t camera_index)
{
  cameras[camera_index].offset				 = new_offset;
  cameras[camera_index].is_position_outdated = true;
}

const Math::vec2& CS230::Camera::GetPosition(size_t camera_index) const
{
  return cameras[camera_index].position;
}

void CS230::Camera::SetLimit(Math::irect new_limit, size_t camera_index)
{
  cameras[camera_index].limit = new_limit;
}

void CS230::Camera::Update(const Math::vec2& player_position, double dt, size_t camera_index)
{
  CameraSettings& cam = cameras[camera_index];

  // 1. Target Position Calculation Step
  //    Start based on current position, adjust target according to player zone
  Math::vec2 targetPos = cam.position;

  if (cam.anchoring)
  {
	targetPos = player_position;
  }
  else
  {
	// Player Zone Check: Adjust target position if player leaves the box
	if (player_position.x > cam.player_zone.Right() + targetPos.x)
	{
	  targetPos.x = player_position.x - cam.player_zone.Right();
	}
	if (player_position.x - targetPos.x < cam.player_zone.Left())
	{
	  targetPos.x = player_position.x - cam.player_zone.Left();
	}

	if (player_position.y > cam.player_zone.Top() + targetPos.y)
	{
	  targetPos.y = player_position.y - cam.player_zone.Top();
	}
	if (player_position.y - targetPos.y < cam.player_zone.Bottom())
	{
	  targetPos.y = player_position.y - cam.player_zone.Bottom();
	}
  }

  // World Limit Check: Ensure target position doesn't go out of map bounds
  if (targetPos.x < cam.limit.Left())
	targetPos.x = cam.limit.Left();
  if (targetPos.x > cam.limit.Right())
	targetPos.x = cam.limit.Right();
  if (targetPos.y < cam.limit.Bottom())
	targetPos.y = cam.limit.Bottom();
  if (targetPos.y > cam.limit.Top())
	targetPos.y = cam.limit.Top();


  // 2. Final Application Step (Use SetPosition)
  if (cam.enable_smoothing)
  {
	// Get current position
	Math::vec2 nextPos = GetPosition(camera_index);

	// Move nextPos slightly towards targetPos using interpolation function (Weight 5.0)
	ease_to_target(nextPos, targetPos, dt, 5.0);

	// Apply calculated position
	SetPosition(nextPos, camera_index);
  }
  else
  {
	// If smoothing is off, move to target position immediately
	// (Call only when values differ to reduce unnecessary operations)
	if (cam.position.x != targetPos.x || cam.position.y != targetPos.y)
	{
	  SetPosition(targetPos, camera_index);
	}
  }
}

Math::TransformationMatrix CS230::Camera::GetMatrix(size_t camera_index)
{
  if (cameras[camera_index].first_person_view)
  {
	if (cameras[camera_index].is_position_outdated || cameras[camera_index].is_rotation_outdated || cameras[camera_index].is_scale_outdated)
	{
	  cameras[camera_index].camera_matrix =
		Math::TranslationMatrix(cameras[camera_index].position - cameras[camera_index].offset) * Math::RotationMatrix(cameras[camera_index].rotation) * Math::ScaleMatrix(cameras[camera_index].scale);
	  cameras[camera_index].view_matrix = cameras[camera_index].camera_matrix;
	  std::swap(cameras[camera_index].view_matrix[0][1], cameras[camera_index].view_matrix[1][0]);
	  cameras[camera_index].view_matrix[0][2] =
		-(cameras[camera_index].camera_matrix[0][0] * cameras[camera_index].camera_matrix[0][2] + cameras[camera_index].camera_matrix[1][0] * cameras[camera_index].camera_matrix[1][2]);
	  cameras[camera_index].view_matrix[1][2] =
		-(cameras[camera_index].camera_matrix[0][1] * cameras[camera_index].camera_matrix[0][2] + cameras[camera_index].camera_matrix[1][1] * cameras[camera_index].camera_matrix[1][2]);
	  cameras[camera_index].is_position_outdated = false;
	  cameras[camera_index].is_rotation_outdated = false;
	  cameras[camera_index].is_scale_outdated	 = false;
	}
  }
  else
  {
	if (cameras[camera_index].is_position_outdated || cameras[camera_index].is_scale_outdated)
	{
	  cameras[camera_index].camera_matrix =
		Math::TranslationMatrix(cameras[camera_index].position - cameras[camera_index].offset) * Math::ScaleMatrix(cameras[camera_index].scale);
	  cameras[camera_index].view_matrix = cameras[camera_index].camera_matrix;
	  std::swap(cameras[camera_index].view_matrix[0][1], cameras[camera_index].view_matrix[1][0]);
	  cameras[camera_index].view_matrix[0][2] =
		-(cameras[camera_index].camera_matrix[0][0] * cameras[camera_index].camera_matrix[0][2] + cameras[camera_index].camera_matrix[1][0] * cameras[camera_index].camera_matrix[1][2]);
	  cameras[camera_index].view_matrix[1][2] =
		-(cameras[camera_index].camera_matrix[0][1] * cameras[camera_index].camera_matrix[0][2] + cameras[camera_index].camera_matrix[1][1] * cameras[camera_index].camera_matrix[1][2]);
	  cameras[camera_index].is_position_outdated = false;
	  cameras[camera_index].is_scale_outdated	 = false;
	}
	// if(cameras[camera_index].is_position_outdated){
	//     cameras[camera_index].camera_matrix = Math::TranslationMatrix(cameras[camera_index].position - cameras[camera_index].offset) ;
	//     cameras[camera_index].view_matrix = cameras[camera_index].camera_matrix;
	//     cameras[camera_index].view_matrix[0][2] = -(cameras[camera_index].camera_matrix[0][0] * cameras[camera_index].camera_matrix[0][2] + cameras[camera_index].camera_matrix[1][0] *
	//     cameras[camera_index].camera_matrix[1][2]); cameras[camera_index].view_matrix[1][2] = -(cameras[camera_index].camera_matrix[0][1] * cameras[camera_index].camera_matrix[0][2] +
	//     cameras[camera_index].camera_matrix[1][1] * cameras[camera_index].camera_matrix[1][2]); cameras[camera_index].is_position_outdated = false;
	// }
  }
  return cameras[camera_index].view_matrix;
}
