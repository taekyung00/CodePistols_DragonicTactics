#pragma once
#include "./Engine/Component.h"
#include "./Engine/Vec2.h"

class GridPosition : public CS230::Component
{
  public:
  GridPosition(Math::ivec2 start_coordinates);

  void Set(Math::ivec2 new_coordinates);

  const Math::ivec2& Get() const;

  void Update([[maybe_unused]] double dt) override { };

<<<<<<< HEAD
private:
	Math::ivec2 coordinates;
};
=======
  private:
  Math::ivec2 coordinates;
};
>>>>>>> main
