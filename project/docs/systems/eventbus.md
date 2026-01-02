# EventBus System - Complete Specification

**Purpose**: Decoupled event-driven communication system for Dragonic Tactics. Enables systems to communicate without direct dependencies.

---

## Table of Contents

- [1. Overview](#1-overview)
- [2. Core Concepts](#2-core-concepts)
- [3. Implementation](#3-implementation)
- [4. Event Types](#4-event-types)
- [5. Usage Examples](#5-usage-examples)
- [6. Best Practices](#6-best-practices)
- [7. Common Patterns](#7-common-patterns)

---

## 1. Overview

### What is EventBus?

EventBus is a **singleton service** that implements the **Observer/Publish-Subscribe pattern**. It allows systems to:

- **Publish** events when something happens (e.g., "Dragon took damage")
- **Subscribe** to events they care about (e.g., UI listens for damage events to update HP bar)
- **Remain decoupled** - publishers don't know who's listening, subscribers don't know who's publishing

### Why Use EventBus?

**Without EventBus** (tightly coupled):

```cpp
// ❌ BAD: Direct coupling between systems
void Dragon::TakeDamage(int damage) {
    currentHP -= damage;

    // Dragon knows about UI system - BAD!
    Engine::GetUI().UpdateHealthBar(this, currentHP);

    // Dragon knows about EffectManager - BAD!
    Engine::GetEffectManager().PlayDamageEffect(GetPosition());

    // Dragon knows about BattleManager - BAD!
    if (currentHP <= 0) {
        Engine::GetBattleManager().OnCharacterDeath(this);
    }
}
```

**With EventBus** (decoupled):

```cpp
// ✅ GOOD: Dragon only knows about EventBus
void Dragon::TakeDamage(int damage) {
    currentHP -= damage;

    // Publish event - Dragon doesn't care who's listening
    EventBus::Instance().Publish(CharacterDamagedEvent{this, damage, currentHP});

    if (currentHP <= 0) {
        EventBus::Instance().Publish(CharacterDeathEvent{this});
    }
}

// UI, EffectManager, BattleManager each subscribe independently
```

### Key Benefits

| Benefit                  | Description                                        |
| ------------------------ | -------------------------------------------------- |
| **Loose Coupling**       | Systems don't reference each other directly        |
| **Extensibility**        | Add new listeners without modifying publishers     |
| **Testability**          | Mock event subscriptions for unit tests            |
| **Debugging**            | Log all events in one place for debugging          |
| **Parallel Development** | Teams work independently on publishers/subscribers |

---

## 2. Core Concepts

### Event Structure

All events are **plain structs** with public data members:

```cpp
struct CharacterDamagedEvent {
    Character* target;        // Who took damage
    int damageAmount;         // How much damage
    int remainingHP;          // Current HP after damage
    Character* attacker;      // Who dealt damage (optional)
};
```

**Design Rules**:

- Events are **data-only** (no methods except constructors)
- Events are **copyable** (value semantics)
- Events have **descriptive names** ending in "Event"

### Subscribers (Listeners)

Subscribers are **callback functions** that receive events:

```cpp
// Function signature for CharacterDamagedEvent subscriber
void OnCharacterDamaged(const CharacterDamagedEvent& event) {
    // React to event
    Logger::LogEvent("Character took " + std::to_string(event.damageAmount) + " damage");
}
```

**Subscriber Types**:

1. **Free functions** - `void OnEvent(const Event& e)`
2. **Lambda functions** - `[](const Event& e) { ... }`
3. **Member functions** - `std::bind(&Class::Method, this, std::placeholders::_1)`

### Publishers

Publishers **create and send events** to EventBus:

```cpp
// Publish event
EventBus::Instance().Publish(CharacterDamagedEvent{target, 25, 75, attacker});
```

**When to Publish**:

- After state changes (HP decreased, position moved)
- When actions complete (spell cast, turn ended)
- On important transitions (battle started, character died)

---

## 3. Implementation

### EventBus Header

```cpp
// EventBus.h - Singleton event dispatcher
#pragma once
#include <functional>
#include <map>
#include <vector>
#include <typeindex>
#include <string>

class EventBus {
public:
    // Singleton access
    static EventBus& Instance();

    // Subscribe to event type T with callback function
    template<typename T>
    void Subscribe(std::function<void(const T&)> callback) {
        auto typeIndex = std::type_index(typeid(T));
        subscribers[typeIndex].push_back([callback](const void* data) {
            callback(*static_cast<const T*>(data));
        });
    }

    // Publish event of type T
    template<typename T>
    void Publish(const T& event) {
        auto typeIndex = std::type_index(typeid(T));

        // Optional: Log event for debugging
        if (loggingEnabled) {
            LogEvent(typeid(T).name(), &event);
        }

        // Call all subscribers for this event type
        if (subscribers.find(typeIndex) != subscribers.end()) {
            for (auto& callback : subscribers[typeIndex]) {
                callback(&event);
            }
        }
    }

    // Unsubscribe all listeners (used for cleanup)
    void Clear();

    // Enable/disable event logging
    void SetLogging(bool enabled) { loggingEnabled = enabled; }

private:
    EventBus() = default;
    ~EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    // Type-erased callback wrapper
    using CallbackWrapper = std::function<void(const void*)>;

    // Map of event type -> list of callbacks
    std::map<std::type_index, std::vector<CallbackWrapper>> subscribers;

    bool loggingEnabled = false;

    void LogEvent(const std::string& eventType, const void* eventData);
};
```

### EventBus Implementation

```cpp
// EventBus.cpp
#include "EventBus.h"
#include "../Engine/Engine.h"

EventBus& EventBus::Instance() {
    static EventBus instance;
    return instance;
}

void EventBus::Clear() {
    subscribers.clear();
    Engine::GetLogger().LogEvent("EventBus: All subscriptions cleared");
}

void EventBus::LogEvent(const std::string& eventType, const void* eventData) {
    Engine::GetLogger().LogDebug("EventBus: Publishing " + eventType);
}
```

---

## 4. Event Types

### Battle Events

```cpp
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
```

### Combat Events

```cpp
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
```

### Movement Events

```cpp
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
```

### Spell Events

```cpp
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
```

### Status Effect Events

```cpp
// StatusEffectEvents.h - Buff/debuff events

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
```

### UI Events

```cpp
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
```

### Battle State Events

```cpp
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
```

---

## 5. Usage Examples

### Example 1: Combat System Publishing Damage

```cpp
// CombatSystem.cpp - Attack resolution
void CombatSystem::ResolveAttack(Character* attacker, Character* target) {
    // Calculate damage using dice
    int baseDamage = attacker->baseAttackPower;
    int rolledDamage = DiceManager::Instance().Roll(attacker->attackDice);
    int totalDamage = baseDamage + rolledDamage;

    // Apply damage to target
    int oldHP = target->currentHP;
    target->currentHP -= totalDamage;
    if (target->currentHP < 0) target->currentHP = 0;

    // Publish damage event
    CharacterDamagedEvent event;
    event.target = target;
    event.damageAmount = totalDamage;
    event.remainingHP = target->currentHP;
    event.attacker = attacker;
    event.wasCritical = (rolledDamage == 6); // Example: d6 roll of 6 is crit

    EventBus::Instance().Publish(event);

    // Check for death
    if (target->currentHP == 0) {
        CharacterDeathEvent deathEvent;
        deathEvent.character = target;
        deathEvent.killer = attacker;
        EventBus::Instance().Publish(deathEvent);
    }
}
```

### Example 2: UI Subscribing to Events

```cpp
// BattleUI.cpp - Health bar updates
class BattleUI : public CS230::Component {
public:
    void Init() override {
        // Subscribe to damage events
        EventBus::Instance().Subscribe<CharacterDamagedEvent>(
            [this](const CharacterDamagedEvent& e) {
                this->OnCharacterDamaged(e);
            }
        );

        // Subscribe to healing events
        EventBus::Instance().Subscribe<CharacterHealedEvent>(
            [this](const CharacterHealedEvent& e) {
                this->OnCharacterHealed(e);
            }
        );

        // Subscribe to death events
        EventBus::Instance().Subscribe<CharacterDeathEvent>(
            [this](const CharacterDeathEvent& e) {
                this->OnCharacterDeath(e);
            }
        );
    }

private:
    void OnCharacterDamaged(const CharacterDamagedEvent& event) {
        // Update health bar for damaged character
        auto* healthBar = FindHealthBar(event.target);
        if (healthBar) {
            healthBar->SetHP(event.remainingHP);
            healthBar->PlayDamageAnimation();

            // Show damage number popup
            ShowFloatingText(event.target->GetPosition(),
                           "-" + std::to_string(event.damageAmount),
                           event.wasCritical ? Color::Red : Color::White);
        }
    }

    void OnCharacterHealed(const CharacterHealedEvent& event) {
        auto* healthBar = FindHealthBar(event.target);
        if (healthBar) {
            healthBar->SetHP(event.currentHP);
            ShowFloatingText(event.target->GetPosition(),
                           "+" + std::to_string(event.healAmount),
                           Color::Green);
        }
    }

    void OnCharacterDeath(const CharacterDeathEvent& event) {
        auto* portrait = FindPortrait(event.character);
        if (portrait) {
            portrait->PlayDeathAnimation();
        }
    }

    HealthBar* FindHealthBar(Character* character);
    void ShowFloatingText(Math::vec2 pos, std::string text, Color color);
};
```

### Example 3: TurnManager Coordinating Turns

```cpp
// TurnManager.cpp - Turn-based flow
class TurnManager : public CS230::Component {
public:
    void Update(double dt) override {
        if (!turnInProgress) {
            StartNextTurn();
        }
    }

private:
    std::vector<Character*> turnOrder;
    int currentTurnIndex = 0;
    bool turnInProgress = false;

    void StartNextTurn() {
        Character* currentChar = turnOrder[currentTurnIndex];

        // Reset action points
        auto* actionPoints = currentChar->GetGOComponent<ActionPoints>();
        actionPoints->Reset();

        // Apply start-of-turn status effects
        auto* statusEffects = currentChar->GetGOComponent<StatusEffects>();
        statusEffects->ProcessTurnStart();

        // Publish turn started event
        TurnStartedEvent event;
        event.character = currentChar;
        event.turnNumber = currentTurnIndex;
        event.actionPoints = actionPoints->GetCurrent();

        EventBus::Instance().Publish(event);

        // Subscribe to turn end (one-time subscription)
        turnInProgress = true;

        // AI or Player will eventually publish TurnEndedEvent
    }

    void Init() override {
        // Subscribe to turn ended events
        EventBus::Instance().Subscribe<TurnEndedEvent>(
            [this](const TurnEndedEvent& e) {
                this->OnTurnEnded(e);
            }
        );
    }

    void OnTurnEnded(const TurnEndedEvent& event) {
        turnInProgress = false;
        currentTurnIndex = (currentTurnIndex + 1) % turnOrder.size();
    }
};
```

### Example 4: AI System Reacting to Events

```cpp
// AISystem.cpp - AI decision-making based on events
class AISystem : public CS230::Component {
public:
    void Init() override {
        // Subscribe to turn started for AI characters
        EventBus::Instance().Subscribe<TurnStartedEvent>(
            [this](const TurnStartedEvent& e) {
                if (IsAICharacter(e.character)) {
                    this->MakeAIDecision(e.character);
                }
            }
        );

        // Subscribe to damage events for threat tracking
        EventBus::Instance().Subscribe<CharacterDamagedEvent>(
            [this](const CharacterDamagedEvent& e) {
                this->UpdateThreatTable(e);
            }
        );

        // Subscribe to death events to remove from threat table
        EventBus::Instance().Subscribe<CharacterDeathEvent>(
            [this](const CharacterDeathEvent& e) {
                this->RemoveFromThreat(e.character);
            }
        );
    }

private:
    std::map<Character*, int> threatLevels;  // Track who's dangerous

    void MakeAIDecision(Character* aiChar) {
        // AI logic to choose action
        Character* target = SelectBestTarget(aiChar);

        if (CanAttack(aiChar, target)) {
            // Attack highest threat target
            PerformAttack(aiChar, target);
        } else {
            // Move toward target
            MoveToward(aiChar, target);
        }

        // End turn
        TurnEndedEvent event;
        event.character = aiChar;
        event.actionsUsed = 2;  // Example
        EventBus::Instance().Publish(event);
    }

    void UpdateThreatTable(const CharacterDamagedEvent& event) {
        // If attacker damaged an ally, increase their threat
        if (IsAlly(event.target)) {
            threatLevels[event.attacker] += event.damageAmount;
        }
    }

    Character* SelectBestTarget(Character* aiChar) {
        // Choose highest threat enemy
        Character* bestTarget = nullptr;
        int maxThreat = 0;

        for (auto& [character, threat] : threatLevels) {
            if (threat > maxThreat && character->currentHP > 0) {
                maxThreat = threat;
                bestTarget = character;
            }
        }

        return bestTarget ? bestTarget : FindNearestEnemy(aiChar);
    }

    bool IsAICharacter(Character* c);
    bool IsAlly(Character* c);
    void PerformAttack(Character* attacker, Character* target);
    void MoveToward(Character* mover, Character* target);
};
```

### Example 5: EffectManager Visual Feedback

```cpp
// EffectManager.cpp - Visual effects responding to events
class EffectManager {
public:
    void Initialize() {
        // Subscribe to combat events
        EventBus::Instance().Subscribe<CharacterDamagedEvent>(
            [this](const CharacterDamagedEvent& e) {
                PlayDamageEffect(e.target->GetPosition(), e.wasCritical);
            }
        );

        EventBus::Instance().Subscribe<SpellCastEvent>(
            [this](const SpellCastEvent& e) {
                PlaySpellEffect(e.spellName, e.caster->GetPosition(), GetGridWorldPos(e.targetGrid));
            }
        );

        EventBus::Instance().Subscribe<CharacterHealedEvent>(
            [this](const CharacterHealedEvent& e) {
                PlayHealingEffect(e.target->GetPosition());
            }
        );
    }

    static EffectManager& Instance() {
        static EffectManager instance;
        return instance;
    }

private:
    void PlayDamageEffect(Math::vec2 pos, bool critical) {
        // Spawn particle effect
        if (critical) {
            SpawnParticles("CriticalHit", pos, 20);
            PlaySound("critical_hit.wav");
        } else {
            SpawnParticles("Hit", pos, 10);
            PlaySound("hit.wav");
        }
    }

    void PlaySpellEffect(const std::string& spellName, Math::vec2 from, Math::vec2 to) {
        if (spellName == "Fireball") {
            SpawnProjectile("Fireball", from, to);
        } else if (spellName == "LavaPool") {
            SpawnGroundEffect("Lava", to, 3.0f); // 3 second duration
        }
    }

    void PlayHealingEffect(Math::vec2 pos) {
        SpawnParticles("Sparkles", pos, 15);
        PlaySound("heal.wav");
    }

    void SpawnParticles(const std::string& type, Math::vec2 pos, int count);
    void SpawnProjectile(const std::string& type, Math::vec2 from, Math::vec2 to);
    void SpawnGroundEffect(const std::string& type, Math::vec2 pos, float duration);
    void PlaySound(const std::string& filename);
};
```

### Example 6: Logger Debugging All Events

```cpp
// BattleState.cpp - Enable event logging for debugging
void BattleState::Load() {
    // Enable event logging in debug builds
    #ifdef _DEBUG
        EventBus::Instance().SetLogging(true);
    #endif

    // Subscribe logger to all events for debugging
    EventBus::Instance().Subscribe<CharacterDamagedEvent>(
        [](const CharacterDamagedEvent& e) {
            Engine::GetLogger().LogEvent(
                e.target->TypeName() + " took " + std::to_string(e.damageAmount) + " damage"
            );
        }
    );

    EventBus::Instance().Subscribe<SpellCastEvent>(
        [](const SpellCastEvent& e) {
            Engine::GetLogger().LogEvent(
                e.caster->TypeName() + " cast " + e.spellName + " at level " + std::to_string(e.spellLevel)
            );
        }
    );

    EventBus::Instance().Subscribe<TurnStartedEvent>(
        [](const TurnStartedEvent& e) {
            Engine::GetLogger().LogEvent(
                "Turn " + std::to_string(e.turnNumber) + ": " + e.character->TypeName() + "'s turn"
            );
        }
    );
}
```

---

## 6. Best Practices

### When to Use EventBus

✅ **Use EventBus for**:

- System-to-system communication (TurnManager → UI, CombatSystem → EffectManager)
- Broadcasting state changes (character damaged, spell cast, turn started)
- Decoupling gameplay logic from presentation (combat doesn't know about UI)
- Optional listeners (logging, analytics, achievements)

❌ **Don't Use EventBus for**:

- Component-to-GameObject communication (use `GetGOComponent<T>()` instead)
- Parent-child relationships (use direct method calls)
- Immediate return values (use function calls, not events)
- High-frequency updates (use Update() loop for per-frame logic)

### Event Naming Conventions

| Pattern               | Example                 | Use Case                        |
| --------------------- | ----------------------- | ------------------------------- |
| `<Noun><Verb>Event`   | `CharacterDamagedEvent` | Past tense - something happened |
| `<Noun><State>Event`  | `BattleStartedEvent`    | State transition occurred       |
| `<Action><Noun>Event` | `AttackMissedEvent`     | Action result                   |

### Subscription Management

**Pattern 1: Subscribe in Init(), Never Unsubscribe** (for persistent systems)

```cpp
class BattleManager : public CS230::Component {
public:
    void Init() override {
        EventBus::Instance().Subscribe<CharacterDeathEvent>(
            [this](const CharacterDeathEvent& e) { OnCharacterDeath(e); }
        );
    }
    // Subscription lives for entire battle
};
```

**Pattern 2: Clear All on State Change** (for state cleanup)

```cpp
void BattleState::Unload() {
    EventBus::Instance().Clear();  // Remove all subscriptions when leaving battle
}
```

### Event Data Design

**✅ GOOD - Complete Information**:

```cpp
struct SpellCastEvent {
    Character* caster;
    std::string spellName;
    int spellLevel;
    Math::ivec2 targetGrid;
    int spellSlotUsed;
};
// Listeners have all context they need
```

**❌ BAD - Incomplete Information**:

```cpp
struct SpellCastEvent {
    std::string spellName;  // Who cast it? What level?
};
// Listeners must query global state - defeats decoupling!
```

### Avoiding Event Cascades

**Problem**: Events triggering events triggering events (hard to debug)

**Solution**: Use event flags or batch events

```cpp
void CombatSystem::ResolveAttack(Character* attacker, Character* target) {
    // Collect all events that will occur
    std::vector<Event> pendingEvents;

    // Apply damage
    int damage = CalculateDamage(attacker, target);
    target->currentHP -= damage;
    pendingEvents.push_back(CharacterDamagedEvent{...});

    // Check status effects
    if (attacker->HasBurnEffect()) {
        pendingEvents.push_back(StatusEffectTickEvent{...});
    }

    // Check death
    if (target->currentHP <= 0) {
        pendingEvents.push_back(CharacterDeathEvent{...});
    }

    // Publish all events at once
    for (auto& event : pendingEvents) {
        // Publish (type-erased, requires variant or visitor pattern)
    }
}
```

---

## 7. Common Patterns

### Pattern 1: Turn-Based Flow

```cpp
// TurnManager publishes turn started
TurnStartedEvent → AISystem makes decision → AISystem publishes action events
                 → UI highlights active character
                 → StatusEffectManager applies start-of-turn effects

// Character publishes turn ended
TurnEndedEvent → TurnManager advances to next character
               → BattleManager checks victory conditions
```

### Pattern 2: Combat Resolution

```cpp
// CombatSystem resolves attack
AttackEvent → DiceManager rolls damage → CombatSystem publishes CharacterDamagedEvent
                                       → UI updates health bar
                                       → EffectManager plays hit animation
                                       → Logger logs combat result
                                       → AISystem updates threat table
```

### Pattern 3: Spell Casting

```cpp
// SpellSystem casts spell
SpellCastEvent → SpellSlots component consumes slot → SpellSlotConsumedEvent
               → UI updates spell icons
               → EffectManager plays cast animation

// Spell resolves
SpellEffectAppliedEvent → StatusEffects component adds effect
                        → UI shows status icon
                        → Logger logs spell application
```

### Pattern 4: Victory Detection

```cpp
// CharacterDeathEvent published
CharacterDeathEvent → BattleManager counts remaining enemies
                    → If all enemies dead: BattleEndedEvent
                    → UI shows victory screen
                    → SaveManager saves progress
                    → GameStateManager transitions to Victory state
```

---

## Summary

EventBus is the **communication backbone** of Dragonic Tactics:

✅ **Decouples systems** - No direct dependencies between combat, UI, AI, effects
✅ **Enables parallel development** - Teams implement publishers/subscribers independently
✅ **Simplifies debugging** - Log all events in one place
✅ **Extensible** - Add new listeners without modifying existing code
✅ **Testable** - Mock subscriptions for unit tests

**Key Takeaway**: If two systems need to communicate but shouldn't depend on each other, use EventBus.

**Next Steps**:

1. Implement EventBus singleton using template-based type dispatch
2. Define event structs in `Events/` directory (categorized by domain)
3. Subscribe to events in `Init()` methods of systems
4. Publish events after state changes in gameplay code

**Related Documentation**:

- [singletons.md](singletons.md) - Other singleton services
- [game-state-components.md](game-state-components.md) - Systems that use EventBus
- [debug/tools.md](../debug/tools.md) - EventBus debugging tools
