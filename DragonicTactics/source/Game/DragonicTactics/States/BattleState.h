#pragma once
#include "./Engine/GameState.hpp"
#include "../Objects/Character.h"
#include "../Objects/Dragon.h"
#include "../Objects/Fighter.h"
#include <vector>

class BattleState : public CS230::GameState {
public:
    BattleState();
    ~BattleState();

    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;

    enum class BattlePhase {
        Setup,
        PlayerTurn,
        EnemyTurn,
        BattleEnd
    };

    BattlePhase GetCurrentPhase() const { return currentPhase; }
    void SetPhase(BattlePhase phase);

    Dragon* GetDragon() { return dragon; }
    Fighter* GetFighter() { return fighter; }
    Character* GetCurrentTurnCharacter();

    void EndCurrentTurn();
    void CheckBattleEnd();

private:
    Dragon* dragon;
    Fighter* fighter;

    BattlePhase currentPhase;
    int turnCount;

    Math::vec2 cursorPosition;
    bool selectingTarget;
    std::string selectedAction;

    void HandleInput();
    void HandlePlayerTurnInput();
    void HandleEnemyTurnInput();
    void RenderGrid();
    void RenderUI();
};