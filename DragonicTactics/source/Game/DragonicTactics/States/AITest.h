#pragma once
#include "../../../Engine/GameState.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/Test/TestAI.h"

class AITest : public CS230::GameState
{
public:
	AITest();
	void Load() override;
	void Update(double dt) override;
	void Unload() override;
	void Draw() override;
	void DrawImGui() override;

	gsl::czstring GetName() const override
	{
		return "AI Test";
    }
};
