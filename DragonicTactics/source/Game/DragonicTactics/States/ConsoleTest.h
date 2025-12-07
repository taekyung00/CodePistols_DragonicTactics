#pragma once
#include "Engine/GameState.h"

class ConsoleTest : public CS230::GameState
{
  public:
  ConsoleTest();
  void Load() override;
  void Update(double dt) override;
  void Unload() override;
  void Draw() override;
  void DrawImGui() override;

  gsl::czstring GetName() const override
  {
	return "Console Test";
  }
};