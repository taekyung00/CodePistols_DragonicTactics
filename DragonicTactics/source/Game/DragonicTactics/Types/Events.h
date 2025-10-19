#pragma once
#include <string>
#include <vector>
#include "../../../Engine/Vec2.hpp"
class Character;

// BattleEvents.h - High-level battle events

struct BattleStartedEvent {
    std::vector<Character*> allCombatants;
    std::string mapName;
};

struct BattleEndedEvent {
    bool playerVictory;
    int turnsElapsed;
    Character* lastSurvivor;  // Winning character
};

struct VictoryConditionMetEvent {
    std::string condition;    // "all enemies defeated", "dragon survived 10 turns"
};

// CombatEvents.h - Combat action events


struct CharacterDamagedEvent {
    Character* target;
    int damageAmount;
    int remainingHP;
    Character* attacker;
    bool wasCritical;         // Was it a critical hit?
};

struct CharacterHealedEvent {
    Character* target;
    int healAmount;
    int currentHP;
    int maxHP;
    Character* healer;        // Who cast healing spell (can be self)
};

struct CharacterDeathEvent {
    Character* character;
    Character* killer;        // Who dealt killing blow (optional)
};

struct AttackMissedEvent {
    Character* attacker;
    Character* target;
    std::string reason;       // "out of range", "dodged", etc.
};

// MovementEvents.h - Grid movement events

struct CharacterMovedEvent {
    Character* character;
    Math::ivec2 fromGrid;     // Starting grid position
    Math::ivec2 toGrid;       // Ending grid position
    int actionPointsSpent;    // Cost of movement
};

struct MovementBlockedEvent {
    Character* character;
    Math::ivec2 attemptedGrid;
    std::string reason;       // "occupied", "out of bounds", "insufficient AP"
};

// SpellEvents.h - Magic system events

struct SpellCastEvent {
    Character* caster;
    std::string spellName;
    int spellLevel;           // Level at which spell was cast
    Math::ivec2 targetGrid;   // Target tile
    int spellSlotUsed;        // Which slot level was consumed
};

struct SpellEffectAppliedEvent {
    Character* target;
    std::string effectName;   // "Burn", "Fear", "Blessing", etc.
    int duration;             // Turns remaining
    Character* caster;        // Who applied the effect
};

struct SpellSlotConsumedEvent {
    Character* character;
    int slotLevel;            // Level of spell slot used
    int slotsRemaining;       // How many of that level remain
};

struct StatusEffectAddedEvent {
    Character* target;
    std::string effectName;
    int duration;
    int magnitude;            // Effect strength (e.g., +5 attack, -2 speed)
};

struct StatusEffectRemovedEvent {
    Character* target;
    std::string effectName;
    std::string reason;       // "expired", "dispelled", "death"
};

struct StatusEffectTickEvent {
    Character* target;
    std::string effectName;
    int damageOrHealing;      // Burn damage, regeneration healing
};

// TurnEvents.h - Turn management events
struct TurnStartedEvent {
    Character* character;     // Whose turn started
    int turnNumber;           // Current turn count
    int actionPoints;         // Available action points
};

struct TurnEndedEvent {
    Character* character;     // Whose turn ended
    int actionsUsed;          // How many actions taken
};

struct InitiativeRolledEvent {
    std::vector<Character*> turnOrder;  // Full turn order after initiative
};

// UIEvents.h - User interface events

struct UIActionSelectedEvent {
    std::string actionType;   // "Attack", "Move", "CastSpell", "EndTurn"
    Character* character;     // Who is performing action
};

struct UITileHoveredEvent {
    Math::ivec2 gridPos;
    bool isValidTarget;       // Can current action target this tile?
};

struct UISpellSelectedEvent {
    std::string spellName;
    int upcastLevel;          // Level at which to cast
    Character* caster;
};