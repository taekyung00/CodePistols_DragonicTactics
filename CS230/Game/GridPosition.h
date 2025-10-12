#pragma once
#include "../Engine/Component.h"
#include "../Engine/Vec2.h"

class GridPosition : public CS230::Component {
public:
	GridPosition(Math::ivec2 start_coordinates);

	void Set(Math::ivec2 new_coordinates);

	const Math::ivec2& Get() const;

	void Update([[maybe_unused]]double dt) override {};//굳이 안해도됨 안필요하면

private:
	Math::ivec2 coordinates;
};