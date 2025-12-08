/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Camera.h
Project:    CS230 Engine
Author:     Jonathan Holmes
Created:    March 8, 2023
*/

#pragma once

#include "Component.h"
#include "Matrix.h"
#include "Rect.h"
#include "Vec2.h"
#include <vector>

namespace CS230
{
  class Camera : public Component
  {
public:
	Camera(Math::rect player_zone, size_t camera_amount = 1);
	bool&			  SetFirstPersonView(size_t camera_index = 0);
	bool&			  SetAnchoring(size_t camera_index = 0);
	bool&			  SetSmoothing(size_t camera_index = 0);
	void			  SetPosition(Math::vec2 new_position, size_t camera_index = 0);
	void			  SetRotation(double new_rotation, size_t camera_index = 0);
	void			  SetScale(Math::vec2 new_scale, size_t camera_index = 0);
	void			  SetPositionOffset(Math::vec2 new_offset, size_t camera_index = 0);
	const Math::vec2& GetPosition(size_t camera_index = 0) const;
	void			  SetLimit(Math::irect new_limit, size_t camera_index = 0);
	using Component::Update; // say i'll use this version too, so don't hide anymore
	void					   Update(const Math::vec2& player_position, double dt, size_t camera_index = 0);
	Math::TransformationMatrix GetMatrix(size_t camera_index = 0);

private:
	struct CameraSettings
	{
	  Math::irect				 limit;
	  bool						 first_person_view{ true };
	  bool						 anchoring{ true };
	  bool						 enable_smoothing{ true };
	  Math::vec2				 position;
	  Math::vec2				 offset;
	  bool						 is_position_outdated{ true };
	  double					 rotation;
	  bool						 is_rotation_outdated{ true };
	  Math::vec2				 scale;
	  bool						 is_scale_outdated{ true };
	  Math::rect				 player_zone;
	  Math::TransformationMatrix camera_matrix{};
	  Math::TransformationMatrix view_matrix{};
	};

	std::vector<CameraSettings> cameras;
  };
}
