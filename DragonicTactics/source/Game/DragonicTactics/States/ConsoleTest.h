<<<<<<< HEAD
﻿#pragma once
#include "Engine/GameState.h"
=======
#pragma once
#include "Engine/GameState.h"

>>>>>>> main
class ConsoleTest : public CS230::GameState
{
  public:
  ConsoleTest();
  void Load() override;
  void Update(double dt) override;
  void Unload() override;
  void Draw() override;
  void DrawImGui() override;

<<<<<<< HEAD
	gsl::czstring GetName() const override
	{
		return "Console Test";
	}
};
=======
  gsl::czstring GetName() const override
  {
	return "Console Test";
  }
};
>>>>>>> main
