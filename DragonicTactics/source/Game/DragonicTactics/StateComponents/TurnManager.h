#pragma once
#include "../Objects/Character.h"
#include "../Types/Events.h"
#include "../Singletons/EventBus.h"
#include "../Test/Week1TestMocks.h"
#include <vector>
#include <queue>

class TurnManager {
public:
    static TurnManager& Instance();

    // Turn management
    void InitializeTurnOrder(const std::vector<Character*>& characters);
    void InitializeTurnOrder(const std::vector<MockCharacter*>& characters);
    void StartNextTurn();
    void EndCurrentTurn();

    // Turn state
    Character* GetCurrentCharacter() const;
    int GetCurrentTurnNumber() const;
    int GetRoundNumber() const;
    bool IsCombatActive() const;

    // Turn order
    std::vector<Character*> GetTurnOrder() const;
    int GetCharacterTurnIndex(Character* character) const;

    // Combat state
    void StartCombat();
    void EndCombat();
    void Reset();

private:
    TurnManager() = default;
    TurnManager(const TurnManager&) = delete;
    TurnManager& operator=(const TurnManager&) = delete;

    std::vector<Character*> turnOrder;
    int currentTurnIndex;
    int turnNumber;
    int roundNumber;
    bool combatActive;

    void PublishTurnStartEvent();
    void PublishTurnEndEvent();
};