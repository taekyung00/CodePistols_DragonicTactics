#pragma once
#include "../../../Engine/GameState.hpp"

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

    bool TestAITargetsClosestEnemy();
    bool TestAIMovesCloserWhenOutOfRange();
    bool TestAIAttacksWhenInRange();
    bool TestAIUsesShieldBashWhenAdjacent();
    bool TestAIEndsTurnWhenNoActions();
    void RunFighterAITests();
};