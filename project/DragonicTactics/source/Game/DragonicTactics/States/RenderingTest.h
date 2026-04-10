/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "../../../Engine/GameState.h"
#include "Engine/Vec2.h"
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

  std::unique_ptr<CS230::GameObject> dragon;
  std::unique_ptr<CS230::GameObject> fighter;
  constexpr static float timer_max = 3.f;
  float timer = 0.f;
  Math::fvec2 scale = {1.f,1.f};
	float rotate = 0.f;
	Math::fvec2 translate = {0.f,0.f};
	Math::ivec2 window_size;
};