#pragma once
#include "../../../Engine/GameState.h"

class RenderingTest : public CS230::GameState
{
  public:
  RenderingTest();
  void Load() override;
  void Update(double dt) override;
  void Unload() override;
  void Draw() override;
  void DrawImGui() override;

  gsl::czstring GetName() const override
  {
	return "Rendering Test";
  }
};