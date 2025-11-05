#pragma once
#include "./Engine/Component.h"
#include "./Engine/Vec2.hpp"

class GridPosition : public CS230::Component {
public:
	GridPosition(Math::ivec2 start_coordinates);

	void Set(Math::ivec2 new_coordinates);

	const Math::ivec2& Get() const;

	void Update([[maybe_unused]]double dt) override {};

private:
	Math::ivec2 coordinates;
};