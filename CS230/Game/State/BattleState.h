#pragma once
#include "../../Engine/GameState.h"
#include "../Character.h"
#include "../Dragon.h"
#include "../Fighter.h"
#include <vector>

class BattleState : public CS230::GameState {
public:
    BattleState();
    ~BattleState();

    // Step 1.1: GameState overrides
    // Reason: Required by CS230 engine architecture
    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;

    // Step 1.2: Battle state management
    // Reason: Track battle progress
    enum class BattlePhase {
        Setup,       // Loading characters, initializing systems
        PlayerTurn,  // Dragon's turn (player controlled)
        EnemyTurn,   // Fighter's turn (manual control for Week 3)
        BattleEnd    // One character died
    };

    BattlePhase GetCurrentPhase() const { return currentPhase; }
    void SetPhase(BattlePhase phase);

    // Step 1.3: Character access
    // Reason: Other systems need access to battle participants
    Dragon* GetDragon() { return dragon; }
    Fighter* GetFighter() { return fighter; }
    Character* GetCurrentTurnCharacter();

    // Step 1.4: Battle flow control
    // Reason: Manual turn advancement for Week 3
    void EndCurrentTurn();
    void CheckBattleEnd();

private:
    // Step 1.5: Battle participants
    Dragon* dragon;
    Fighter* fighter;

    // Step 1.6: Battle state
    BattlePhase currentPhase;
    int turnCount;

    // Step 1.7: UI state (simple for Week 3)
    Math::vec2 cursorPosition;  // Grid cursor for spell/ability targeting
    bool selectingTarget;       // Is player selecting a target?
    std::string selectedAction; // "move", "spell", "ability"

    // Step 1.8: Helper methods
    void HandleInput();
    void HandlePlayerTurnInput();
    void HandleEnemyTurnInput();  // Manual control for Week 3
    void RenderGrid();
    void RenderUI();
};