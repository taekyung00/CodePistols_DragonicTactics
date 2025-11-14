#include "TurnManager.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"
#include "../Singletons/DiceManager.h"
#include "../Objects/Components/StatsComponent.h"
#include <algorithm>

TurnManager::TurnManager() : 
    currentTurnIndex{},
    turnNumber{}, 
    roundNumber{},
    combatActive{},
    initiativeMode{InitiativeMode::RollOnce}
{
}

void TurnManager::InitializeTurnOrder(const std::vector<Character*>& characters) {
    if (characters.empty()) {
        Engine::GetLogger().LogError("TurnManager: Cannot initialize with empty character list");
        return;
    }

    // ===== Sangyun: Use initiative system instead of simple array order (NEW) =====
    RollInitiative(characters);

    // Update turnOrder from initiativeOrder for backward compatibility
    turnOrder.clear();
    for (const auto& entry : initiativeOrder) {
        if (entry.character && entry.character->IsAlive()) {
            turnOrder.push_back(entry.character);
        }
    }
    // ===== End Sangyun Initiative Integration =====

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

        // ===== Sangyun: Re-roll initiative if variant mode enabled (NEW) =====
        if (initiativeMode == InitiativeMode::RollEachRound) {
            std::vector<Character*> aliveCharacters;
            for (const auto& entry : initiativeOrder) {
                if (entry.character && entry.character->IsAlive()) {
                    aliveCharacters.push_back(entry.character);
                }
            }

            if (!aliveCharacters.empty()) {
                RollInitiative(aliveCharacters);

                // Update turnOrder from new initiative
                turnOrder.clear();
                for (const auto& entry : initiativeOrder) {
                    if (entry.character && entry.character->IsAlive()) {
                        turnOrder.push_back(entry.character);
                    }
                }

                currentTurnIndex = 0; // Reset to first in new order
            }
        }
        // ===== End Sangyun Initiative Re-roll =====
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
    initiativeOrder.clear();
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
    // ===== Sangyun: Return initiative-based turn order if available (NEW) =====
    if (!initiativeOrder.empty()) {
        std::vector<Character*> order;
        for (const auto& entry : initiativeOrder) {
            if (entry.character && entry.character->IsAlive()) {
                order.push_back(entry.character);
            }
        }
        return order;
    }
    // ===== Sangyun Initiative Return =====

    // Fallback to simple turnOrder
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

// ==========Sangyun: INITIATIVE SYSTEM IMPLEMENTATION (NEW) ==========

int TurnManager::CalculateSpeedModifier(int speed) const {
    // D&D 5e ability modifier formula: (ability_score - 10) / 2
    // For speed 5: (5 - 10) / 2 = -2
    // For speed 10: (10 - 10) / 2 = 0
    // For speed 15: (15 - 10) / 2 = +2
    return (speed - 10) / 2;
}

void TurnManager::RollInitiative(const std::vector<Character*>& characters) {
    // Clear previous initiative
    initiativeOrder.clear();

    Engine::GetLogger().LogEvent("=== ROLLING INITIATIVE ===");

    DiceManager& dice = Engine::GetDiceManager();

    // Roll 1d20 + speed modifier for each character
    for (Character* character : characters) {
        if (!character || !character->IsAlive()) {
            continue;
        }

        // Roll 1d20
        int roll = dice.RollDice(1, 20);

        // Get speed from StatsComponent and calculate modifier
        StatsComponent* stats = character->GetStatsComponent();
        if (!stats) {
            Engine::GetLogger().LogError("Character has no StatsComponent!");
            continue;
        }

        int speed = stats->GetSpeed();
        int modifier = CalculateSpeedModifier(speed);

        // Create initiative entry
        InitiativeEntry entry(character, roll, modifier);
        initiativeOrder.push_back(entry);

        // Log result
        Engine::GetLogger().LogEvent(
            character->TypeName() + " rolls " + std::to_string(roll) +
            " + " + std::to_string(modifier) + " = " + std::to_string(entry.totalInitiative)
        );

        // Publish individual initiative rolled event
        InitiativeRolledEvent event{
            character, roll, modifier, entry.totalInitiative
        };
        Engine::GetEventBus().Publish(event);
    }

    // Sort by initiative (highest first)
    SortInitiativeOrder();

    // Publish turn order established event
    std::vector<Character*> sortedChars;
    for (const auto& entry : initiativeOrder) {
        sortedChars.push_back(entry.character);
    }

    Engine::GetEventBus().Publish(TurnOrderEstablishedEvent{sortedChars});

    Engine::GetLogger().LogEvent("=== TURN ORDER ESTABLISHED ===");
    for (const auto& entry : initiativeOrder) {
        Engine::GetLogger().LogEvent(
            "  " + std::to_string(entry.totalInitiative) + ": " + entry.character->TypeName()
        );
    }
}

void TurnManager::SortInitiativeOrder() {
    // Sort by total initiative (descending)
    std::sort(initiativeOrder.begin(), initiativeOrder.end(),
        [](const InitiativeEntry& a, const InitiativeEntry& b) {
            // Primary sort: Total initiative (higher goes first)
            if (a.totalInitiative != b.totalInitiative) {
                return a.totalInitiative > b.totalInitiative;
            }

            // Tie-breaker 1: Speed stat (higher goes first)
            int speedA = a.character->GetStatsComponent()->GetSpeed();
            int speedB = b.character->GetStatsComponent()->GetSpeed();
            if (speedA != speedB) {
                return speedA > speedB;
            }

            // Tie-breaker 2: Pointer address (deterministic for same pointers)
            return a.character > b.character;
        }
    );
}

int TurnManager::GetInitiativeValue(Character* character) const {
    for (const auto& entry : initiativeOrder) {
        if (entry.character == character) {
            return entry.totalInitiative;
        }
    }
    return 0; // Character not in initiative order
}

void TurnManager::ResetInitiative() {
    initiativeOrder.clear();
    Engine::GetLogger().LogEvent("Initiative reset for new combat");
}

// ==========Sangyun: MOCK CHARACTER SUPPORT FOR TESTING (NEW) ==========

void TurnManager::RollInitiativeMock(const std::vector<MockCharacter*>& characters) {
    // Clear previous initiative
    initiativeOrder.clear();

    Engine::GetLogger().LogEvent("=== ROLLING INITIATIVE (MOCK) ===");

    DiceManager& dice = Engine::GetDiceManager();

    // Roll 1d20 + speed modifier for each mock character
    for (MockCharacter* character : characters) {
        if (!character || !character->IsAlive()) {
            continue;
        }

        // Roll 1d20
        int roll = dice.RollDice(1, 20);

        // Get speed and calculate modifier
        int speed = character->GetSpeed();
        int modifier = CalculateSpeedModifier(speed);

        // Create initiative entry for mock character
        InitiativeEntry entry(character, roll, modifier);
        initiativeOrder.push_back(entry);

        // Log result
        Engine::GetLogger().LogEvent(
            character->TypeName() + " rolls " + std::to_string(roll) +
            " + " + std::to_string(modifier) + " = " + std::to_string(entry.totalInitiative)
        );
    }

    // Sort by initiative (highest first)  
    std::sort(initiativeOrder.begin(), initiativeOrder.end(),
        [](const InitiativeEntry& a, const InitiativeEntry& b) {
            // Primary sort: Total initiative (higher goes first)
            if (a.totalInitiative != b.totalInitiative) {
                return a.totalInitiative > b.totalInitiative;
            }

            // Tie-breaker 1: Speed stat (higher goes first)
            int speedA = a.mockCharacter ? a.mockCharacter->GetSpeed() : 
                        (a.character ? a.character->GetStatsComponent()->GetSpeed() : 0);
            int speedB = b.mockCharacter ? b.mockCharacter->GetSpeed() :
                        (b.character ? b.character->GetStatsComponent()->GetSpeed() : 0);
            if (speedA != speedB) {
                return speedA > speedB;
            }

            // Tie-breaker 2: Pointer address (deterministic)
            if (a.mockCharacter && b.mockCharacter) {
                return a.mockCharacter > b.mockCharacter;
            }
            return a.character > b.character;
        }
    );

    Engine::GetLogger().LogEvent("=== TURN ORDER ESTABLISHED (MOCK) ===");
    for (const auto& entry : initiativeOrder) {
        std::string name = entry.mockCharacter ? entry.mockCharacter->TypeName() :
                          (entry.character ? entry.character->TypeName() : "Unknown");
        Engine::GetLogger().LogEvent(
            "  " + std::to_string(entry.totalInitiative) + ": " + name
        );
    }
}

int TurnManager::GetInitiativeValueMock(MockCharacter* character) const {
    for (const auto& entry : initiativeOrder) {
        if (entry.mockCharacter == character) {
            return entry.totalInitiative;
        }
    }
    return 0; // Character not in initiative order
}