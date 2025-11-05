#include "TurnManager.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"
#include <algorithm>

TurnManager& TurnManager::Instance() {
    static TurnManager instance;
    return instance;
}

void TurnManager::InitializeTurnOrder(const std::vector<Character*>& characters) {
    if (characters.empty()) {
        Engine::GetLogger().LogError("TurnManager: Cannot initialize with empty character list");
        return;
    }

    // Clear existing turn order
    turnOrder.clear();

    // Copy characters
    turnOrder = characters;

    // For Week 2: Simple turn order by position in array
    // Week 4: Will add initiative system (1d20 + speed)

    // Remove dead characters
    turnOrder.erase(
        std::remove_if(turnOrder.begin(), turnOrder.end(),
            [](Character* c) { return !c->IsAlive(); }),
        turnOrder.end()
    );

    if (turnOrder.empty()) {
        Engine::GetLogger().LogError("TurnManager: All characters are dead");
        return;
    }

    // Reset turn counters
    currentTurnIndex = 0;
    turnNumber = 1;
    roundNumber = 1;
    combatActive = false;

    Engine::GetLogger().LogEvent("TurnManager: Turn order initialized with " +
        std::to_string(turnOrder.size()) + " characters");

    // Log turn order
    for (size_t i = 0; i < turnOrder.size(); ++i) {
        Engine::GetLogger().LogEvent("  " + std::to_string(i+1) + ". " +
            turnOrder[i]->TypeName());
    }
}

void TurnManager::StartCombat() {
    if (turnOrder.empty()) {
        Engine::GetLogger().LogError("TurnManager: Cannot start combat without turn order");
        return;
    }

    combatActive = true;
    currentTurnIndex = 0;
    turnNumber = 1;
    roundNumber = 1;

    Engine::GetLogger().LogEvent("TurnManager: Combat started");

    // Start first turn
    StartNextTurn();
}

void TurnManager::StartNextTurn() {
    if (!combatActive) {
        Engine::GetLogger().LogError("TurnManager: Combat not active");
        return;
    }

    if (turnOrder.empty()) {
        Engine::GetLogger().LogError("TurnManager: No characters in turn order");
        return;
    }

    // Get current character
    Character* currentChar = turnOrder[currentTurnIndex];

    // Skip dead characters
    while (!currentChar->IsAlive()) {
        currentTurnIndex = (currentTurnIndex + 1) % turnOrder.size();
        currentChar = turnOrder[currentTurnIndex];

        // Check if we've cycled through all characters (all dead)
        if (currentTurnIndex == 0) {
            Engine::GetLogger().LogEvent("TurnManager: All characters dead, ending combat");
            EndCombat();
            return;
        }
    }

    // Refresh character's action points
    currentChar->RefreshActionPoints();

    // Publish turn start event
    PublishTurnStartEvent();

    Engine::GetLogger().LogEvent("TurnManager: Turn " + std::to_string(turnNumber) +
        " - " + currentChar->TypeName() + "'s turn");
}

void TurnManager::EndCurrentTurn() {
    if (!combatActive) {
        Engine::GetLogger().LogError("TurnManager: Combat not active");
        return;
    }

    // Publish turn end event
    PublishTurnEndEvent();

    // Advance to next character
    currentTurnIndex = (currentTurnIndex + 1) % turnOrder.size();
    turnNumber++;

    // Check if we completed a round (all characters had a turn)
    if (currentTurnIndex == 0) {
        roundNumber++;
        Engine::GetLogger().LogEvent("TurnManager: Round " + std::to_string(roundNumber) + " started");
    }

    // Start next turn
    StartNextTurn();
}

void TurnManager::EndCombat() {
    combatActive = false;
    Engine::GetLogger().LogEvent("TurnManager: Combat ended after " +
        std::to_string(turnNumber) + " turns (" +
        std::to_string(roundNumber) + " rounds)");

    // Publish combat end event
    Engine::GetEventBus().Publish(CombatEndedEvent{});
}

void TurnManager::Reset() {
    turnOrder.clear();
    currentTurnIndex = 0;
    turnNumber = 0;
    roundNumber = 0;
    combatActive = false;

    Engine::GetLogger().LogEvent("TurnManager: Reset");
}

Character* TurnManager::GetCurrentCharacter() const {
    if (turnOrder.empty() || currentTurnIndex >= turnOrder.size()) {
        return nullptr;
    }
    return turnOrder[currentTurnIndex];
}

int TurnManager::GetCurrentTurnNumber() const {
    return turnNumber;
}

int TurnManager::GetRoundNumber() const {
    return roundNumber;
}

bool TurnManager::IsCombatActive() const {
    return combatActive;
}

std::vector<Character*> TurnManager::GetTurnOrder() const {
    return turnOrder;
}

int TurnManager::GetCharacterTurnIndex(Character* character) const {
    for (size_t i = 0; i < turnOrder.size(); ++i) {
        if (turnOrder[i] == character) {
            return (int)i;
        }
    }
    return -1;
}

void TurnManager::PublishTurnStartEvent() {
    Character* currentChar = GetCurrentCharacter();
    if (currentChar == nullptr) return;

    TurnStartedEvent event{currentChar, turnNumber, roundNumber};
    Engine::GetEventBus().Publish(event);
}

void TurnManager::PublishTurnEndEvent() {
    Character* currentChar = GetCurrentCharacter();
    if (currentChar == nullptr) return;

    TurnEndedEvent event{ currentChar, turnNumber };
    Engine::GetEventBus().Publish(event);
}