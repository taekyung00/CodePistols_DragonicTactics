# Phase 2: Core Gameplay Systems

**Timeline**: Weeks 7-12 (6 weeks)
**Focus**: Turn-Based Combat Mechanics
**Deliverable**: Playable tactical combat with Dragon vs Fighter

---

## Table of Contents

1. [Phase Overview](#phase-overview)
2. [Week 7-8: Turn Management](#week-7-8-turn-management)
3. [Week 8-9: Combat System](#week-8-9-combat-system)
4. [Week 9-10: Movement System](#week-9-10-movement-system)
5. [Week 10-11: Spell System (Dragon)](#week-10-11-spell-system-dragon)
6. [Week 11-12: Basic AI (Fighter)](#week-11-12-basic-ai-fighter)
7. [Debug Tools](#debug-tools)
8. [Integration Testing](#integration-testing)
9. [File Structure Summary](#file-structure-summary)

---

## Phase Overview

### Objectives

Transform Phase 1 foundation into playable tactical combat:

- ✅ Speed-based turn order with action point economy
- ✅ D&D-style dice combat with armor/resistance
- ✅ Grid movement with pathfinding and AP costs
- ✅ 3 functional Dragon spells (Fireball, Heal, Wall)
- ✅ Fighter AI with basic threat assessment
- ✅ Debug tools for rapid iteration

### Success Criteria

| System       | Success Metric                                                       |
| ------------ | -------------------------------------------------------------------- |
| **Turns**    | Dragon (speed 5) → Rogue (4) → Fighter (3) → Cleric (2) → Wizard (1) |
| **Combat**   | Dice-based damage with armor reduction, critical hits on natural 20  |
| **Movement** | Valid tiles calculated, AP costs enforced, obstacles block paths     |
| **Spells**   | Dragon casts 3 unique spells, spell slots consumed, range validated  |
| **AI**       | Fighter moves toward Dragon, attacks when in range, uses all AP      |
| **Debug**    | Console commands enable instant testing of all scenarios             |

### Architecture Patterns

```
TurnManager (GSComponent)
    ↓ manages
ActionPoints (GOComponent) ← attached to Characters
    ↓ validates
CombatSystem (GSComponent) ← uses DiceManager
    ↓ applies
DamageCalculator (GOComponent) ← armor/resistance
    ↓ publishes
EventBus ← all systems communicate via events
```

---

## Week 7-8: Turn Management

### Purpose

Establish **speed-based initiative** and **action point economy** for turn-based gameplay.

### Components to Build

#### 1. TurnManager (GSComponent)

**Location**: `CS230/Engine/TurnManager.h/cpp`

**Responsibilities**:

- Maintain turn order sorted by character speed (descending)
- Track current character and turn counter
- Advance turns when character finishes or runs out of AP
- Refresh action points at turn start
- Publish `TurnStartEvent` and `TurnEndEvent`

**Key Methods**:

```cpp
void RegisterCharacter(Character* character);
Character* GetCurrentCharacter();
void AdvanceToNextTurn();
void RefreshTurnOrder(); // Re-sort by speed
void RefreshActionPoints(Character* character);
std::vector<Character*> GetTurnOrder();
int GetTurnCounter(); // Round number
```

**Implementation Notes**:

- Sort characters by `GetSpeed()` whenever turn order changes
- Skip dead characters automatically in `Update()`
- Increment turn counter when all characters have acted
- Load base action points from `turn_config.json`

#### 2. ActionPoints (GOComponent)

**Location**: `CS230/Engine/ActionPoints.h`

**Responsibilities**:

- Track current/max action points per character
- Validate action affordability before spending
- Refresh to max at turn start

**Key Methods**:

```cpp
int GetCurrent();
int GetMax();
bool CanAffordAction(int cost);
void SpendActionPoints(int cost);
void RefreshForNewTurn();
```

**Action Point Costs** (from JSON):

```json
{
  "actionCosts": {
    "move": 1,           // 1 AP per tile
    "meleeAttack": 2,
    "rangedAttack": 2,
    "castSpell": 2,
    "useItem": 1,
    "endTurn": 0
  }
}
```

### JSON Configuration

**File**: `Assets/Data/turn_config.json`

```json
{
  "turnSystem": {
    "baseActionPoints": 3,
    "actionCosts": {
      "move": 1,
      "meleeAttack": 2,
      "rangedAttack": 2,
      "castSpell": 2,
      "useItem": 1
    },
    "initiativeRule": "speed_descending"
  },
  "characterActionPoints": {
    "dragon": 3,
    "fighter": 3,
    "cleric": 3,
    "wizard": 2,
    "rogue": 3
  }
}
```

### Code Example: TurnManager Core

```cpp
// TurnManager.cpp
void TurnManager::RegisterCharacter(Character* character) {
    if (character == nullptr) {
        Engine::GetLogger().LogError("Cannot register null character");
        return;
    }

    characters.push_back(character);
    RefreshTurnOrder();

    Engine::GetLogger().LogEvent("Registered: " + character->TypeName());
}

void TurnManager::RefreshTurnOrder() {
    // Sort by speed (highest first)
    std::sort(characters.begin(), characters.end(),
        [](Character* a, Character* b) {
            return a->GetSpeed() > b->GetSpeed();
        }
    );

    // Log turn order for debugging
    Engine::GetLogger().LogEvent("Turn Order:");
    for (size_t i = 0; i < characters.size(); i++) {
        Engine::GetLogger().LogDebug(
            std::to_string(i + 1) + ". " +
            characters[i]->TypeName() + " (speed: " +
            std::to_string(characters[i]->GetSpeed()) + ")"
        );
    }
}

void TurnManager::AdvanceToNextTurn() {
    // End current turn
    Character* current = GetCurrentCharacter();
    if (current != nullptr) {
        EventBus::Instance().Publish(TurnEndEvent{current});
        Engine::GetLogger().LogEvent(current->TypeName() + " turn ended");
    }

    // Move to next character
    currentTurnIndex++;

    // Check for new round
    if (currentTurnIndex >= characters.size()) {
        currentTurnIndex = 0;
        turnCounter++;
        Engine::GetLogger().LogEvent("=== Round " + std::to_string(turnCounter) + " ===");
    }

    // Start next turn
    Character* next = GetCurrentCharacter();
    if (next != nullptr) {
        RefreshActionPoints(next);
        EventBus::Instance().Publish(TurnStartEvent{next});
        Engine::GetLogger().LogEvent(next->TypeName() + " turn started");
    }
}

void TurnManager::Update(double dt) {
    // Auto-skip dead characters
    Character* current = GetCurrentCharacter();
    if (current != nullptr && !current->IsAlive()) {
        Engine::GetLogger().LogEvent(current->TypeName() + " is dead, skipping");
        AdvanceToNextTurn();
    }
}
```

### Verification Tests

| Test              | Expected Result                                              |
| ----------------- | ------------------------------------------------------------ |
| **Turn order**    | Create 5 chars with speeds [5,4,3,2,1], verify order matches |
| **Dragon first**  | Dragon (speed 5) acts before Rogue (4) before Fighter (3)    |
| **Wizard last**   | Wizard (speed 1) always acts last                            |
| **AP refresh**    | Characters start turn with max AP from JSON config           |
| **AP spending**   | Spend 2 AP, verify only remaining points available           |
| **Turn advance**  | Call `AdvanceToNextTurn()`, verify next char is current      |
| **Round counter** | Advance through all chars, verify counter increments         |
| **Events**        | Subscribe to turn events, verify they fire with correct data |

### Event Definitions

```cpp
// TurnEvents.h
struct TurnStartEvent {
    Character* character;
};

struct TurnEndEvent {
    Character* character;
};

struct ActionPointSpentEvent {
    Character* character;
    int pointsSpent;
    int remainingPoints;
};
```

### Usage in BattleState

```cpp
// BattleState::Load()
void BattleState::Load() {
    // Create TurnManager
    turnManager = AddGSComponent(new TurnManager());

    // Create characters
    auto dragon = CharacterFactory::CreateCharacter(CharacterType::Dragon);
    auto fighter = CharacterFactory::CreateCharacter(CharacterType::Fighter);

    // Register with TurnManager
    turnManager->RegisterCharacter(dragon);
    turnManager->RegisterCharacter(fighter);

    // Subscribe to events
    EventBus::Instance().Subscribe<TurnStartEvent>([this](const TurnStartEvent& e) {
        Engine::GetLogger().LogEvent(">>> Turn: " + e.character->TypeName());
    });

    // Start first turn
    turnManager->AdvanceToNextTurn();
}
```

---

## Week 8-9: Combat System

### Purpose

Implement **D&D-style dice combat** with armor reduction and critical hits.

### Components to Build

#### 1. CombatSystem (GSComponent)

**Location**: `CS230/Engine/CombatSystem.h/cpp`

**Responsibilities**:

- Calculate damage using DiceManager (e.g., "3d6+2")
- Apply armor/magic resistance via DamageCalculator
- Validate attack range and action point costs
- Handle critical hits (natural 20 = 2x damage)
- Publish combat events (attack, damage, death)

**Key Methods**:

```cpp
bool ExecuteAttack(Character* attacker, Character* target);
int CalculateDamage(Character* attacker, Character* target, AttackType type);
void ApplyDamage(Character* target, int damage);
void HandleCharacterDeath(Character* character);
```

#### 2. DamageCalculator (GOComponent)

**Location**: `CS230/Engine/DamageCalculator.h`

**Responsibilities**:

- Apply armor reduction to physical damage
- Apply magic resistance to spell damage
- Enforce minimum damage (1)

**Key Methods**:

```cpp
int CalculatePhysicalDamage(int baseDamage);
int CalculateMagicalDamage(int baseDamage);
int GetArmor();
int GetMagicResist();
```

### JSON Configuration

**File**: `Assets/Data/combat_config.json`

```json
{
  "combatSystem": {
    "meleeAttack": {
      "range": 1,
      "actionPointCost": 2,
      "baseDamage": "2d6",
      "attackRoll": "1d20"
    },
    "rangedAttack": {
      "range": 6,
      "actionPointCost": 2,
      "baseDamage": "1d8",
      "attackRoll": "1d20"
    },
    "criticalHit": {
      "naturalRoll": 20,
      "damageMultiplier": 2.0
    },
    "armorSystem": {
      "flatReduction": true,
      "minimumDamage": 1
    }
  },
  "characterCombatStats": {
    "dragon": {
      "armor": 5,
      "magicResist": 3,
      "meleeDamage": "3d6",
      "attackBonus": 2
    },
    "fighter": {
      "armor": 8,
      "magicResist": 1,
      "meleeDamage": "2d8",
      "attackBonus": 3
    }
  }
}
```

### Code Example: Combat Execution

```cpp
// CombatSystem.cpp
bool CombatSystem::ExecuteAttack(Character* attacker, Character* target) {
    // Validate action points
    ActionPoints* ap = attacker->GetGOComponent<ActionPoints>();
    int attackCost = DataRegistry::Instance().GetValue<int>("combatSystem.meleeAttack.actionPointCost");

    if (ap == nullptr || !ap->CanAffordAction(attackCost)) {
        Engine::GetLogger().LogError("Insufficient AP for attack");
        return false;
    }

    // Validate range
    GridSystem* grid = GetOwner()->GetGSComponent<GridSystem>();
    Math::vec2 attackerPos = attacker->GetGOComponent<GridPosition>()->GetTilePosition();
    Math::vec2 targetPos = target->GetGOComponent<GridPosition>()->GetTilePosition();

    int range = DataRegistry::Instance().GetValue<int>("combatSystem.meleeAttack.range");
    double distance = (targetPos - attackerPos).Length();

    if (distance > range) {
        Engine::GetLogger().LogError("Target out of range: " + std::to_string(distance));
        return false;
    }

    // Roll attack roll (d20)
    int attackRoll = DiceManager::Instance().RollDice(1, 20);
    Engine::GetLogger().LogEvent(attacker->TypeName() + " rolls d20: " + std::to_string(attackRoll));

    // Check for critical hit
    bool isCritical = (attackRoll == 20);

    // Calculate damage
    int damage = CalculateDamage(attacker, target, AttackType::Physical);

    if (isCritical) {
        damage *= 2;
        Engine::GetLogger().LogEvent("CRITICAL HIT! Damage doubled to " + std::to_string(damage));
    }

    // Apply damage
    ApplyDamage(target, damage);

    // Spend AP
    ap->SpendActionPoints(attackCost);

    // Publish event
    EventBus::Instance().Publish(AttackEvent{attacker, target, damage, isCritical});

    Engine::GetLogger().LogEvent(
        attacker->TypeName() + " hits " + target->TypeName() +
        " for " + std::to_string(damage) + " damage"
    );

    return true;
}

int CombatSystem::CalculateDamage(Character* attacker, Character* target, AttackType type) {
    // Get damage dice notation
    std::string damageKey = attacker->TypeName() + ".meleeDamage";
    std::string diceNotation = DataRegistry::Instance().GetValue<std::string>(damageKey);

    // Parse "3d6" -> count=3, sides=6
    int count, sides;
    ParseDiceNotation(diceNotation, count, sides);

    // Roll damage
    int baseDamage = DiceManager::Instance().RollDice(count, sides);

    // Add attack bonus
    int bonus = DataRegistry::Instance().GetValue<int>(attacker->TypeName() + ".attackBonus");
    int totalDamage = baseDamage + bonus;

    // Apply target's defense
    DamageCalculator* defense = target->GetGOComponent<DamageCalculator>();
    if (defense != nullptr) {
        if (type == AttackType::Physical) {
            totalDamage = defense->CalculatePhysicalDamage(totalDamage);
        } else {
            totalDamage = defense->CalculateMagicalDamage(totalDamage);
        }
    }

    // Ensure minimum damage
    return std::max(1, totalDamage);
}

void CombatSystem::ApplyDamage(Character* target, int damage) {
    int currentHP = target->GetCurrentHP();
    int newHP = std::max(0, currentHP - damage);

    target->SetCurrentHP(newHP);

    EventBus::Instance().Publish(DamageEvent{target, damage, currentHP, newHP});

    Engine::GetLogger().LogEvent(
        target->TypeName() + " takes " + std::to_string(damage) + " damage (" +
        std::to_string(newHP) + "/" + std::to_string(target->GetMaxHP()) + " HP)"
    );

    // Check for death
    if (newHP <= 0) {
        HandleCharacterDeath(target);
    }
}

void CombatSystem::HandleCharacterDeath(Character* character) {
    Engine::GetLogger().LogEvent(character->TypeName() + " has been defeated!");

    EventBus::Instance().Publish(CharacterDeathEvent{character});

    character->SetDead();
}
```

### DamageCalculator Component

```cpp
// DamageCalculator.h
class DamageCalculator : public CS230::Component {
public:
    DamageCalculator(int armorValue, int magicResistValue)
        : armor(armorValue), magicResist(magicResistValue) {}

    int CalculatePhysicalDamage(int baseDamage) const {
        int reduced = baseDamage - armor;
        if (armor > 0) {
            Engine::GetLogger().LogDebug("Armor reduced damage by " + std::to_string(armor));
        }
        return std::max(1, reduced);
    }

    int CalculateMagicalDamage(int baseDamage) const {
        int reduced = baseDamage - magicResist;
        if (magicResist > 0) {
            Engine::GetLogger().LogDebug("Magic resist reduced damage by " + std::to_string(magicResist));
        }
        return std::max(1, reduced);
    }

    int GetArmor() const { return armor; }
    int GetMagicResist() const { return magicResist; }

private:
    int armor;
    int magicResist;
};
```

### Event Definitions

```cpp
// CombatEvents.h
enum class AttackType {
    Physical,
    Magical
};

struct AttackEvent {
    Character* attacker;
    Character* target;
    int damage;
    bool wasCritical;
};

struct DamageEvent {
    Character* target;
    int damageAmount;
    int hpBefore;
    int hpAfter;
};

struct CharacterDeathEvent {
    Character* character;
};
```

### Verification Tests

| Test                | Expected Result                                      |
| ------------------- | ---------------------------------------------------- |
| **Basic attack**    | Fighter attacks Dragon, verify damage applied        |
| **Dice variance**   | Attack 10 times, verify different damage amounts     |
| **Armor reduction** | Attack char with 5 armor, verify damage reduced by 5 |
| **Range check**     | Attempt attack beyond range 1, verify rejection      |
| **AP cost**         | Attack costs 2 AP, verify deduction                  |
| **Death**           | Reduce char to 0 HP, verify death event fires        |
| **Critical hit**    | Force natural 20 roll, verify 2x damage              |
| **Events**          | Subscribe to damage events, verify all data correct  |

---

## Week 9-10: Movement System

### Purpose

Implement **grid-based movement** with action point costs and pathfinding.

### Components to Build

#### 1. GridSystem Enhancements

**Location**: `CS230/Engine/GridSystem.h/cpp` (MODIFY)

**New Methods**:

```cpp
std::vector<Math::vec2> GetValidMoves(Character* character);
bool IsValidMove(Math::vec2 from, Math::vec2 to, int maxCost);
int CalculatePathCost(Math::vec2 from, Math::vec2 to);
bool IsPathClear(Math::vec2 from, Math::vec2 to);
bool IsPositionOccupied(Math::vec2 position);
```

#### 2. MovementAction

**Location**: `CS230/Engine/MovementAction.h/cpp`

**Responsibilities**:

- Validate movement (range, obstacles, AP cost)
- Execute position change
- Spend action points
- Publish movement events

**Key Method**:

```cpp
static bool ExecuteMove(
    Character* character,
    Math::vec2 targetPosition,
    GridSystem* grid,
    TurnManager* turnMgr
);
```

### Code Example: Movement Validation

```cpp
// GridSystem.cpp
std::vector<Math::vec2> GridSystem::GetValidMoves(Character* character) {
    ActionPoints* ap = character->GetGOComponent<ActionPoints>();
    if (ap == nullptr) return {};

    int availableAP = ap->GetCurrent();
    Math::vec2 currentPos = character->GetGOComponent<GridPosition>()->GetTilePosition();

    std::vector<Math::vec2> validMoves;

    // Check all tiles within movement range
    for (int dx = -availableAP; dx <= availableAP; dx++) {
        for (int dy = -availableAP; dy <= availableAP; dy++) {
            if (dx == 0 && dy == 0) continue; // Skip current position

            Math::vec2 targetPos = currentPos + Math::vec2{
                static_cast<double>(dx),
                static_cast<double>(dy)
            };

            if (IsValidMove(currentPos, targetPos, availableAP)) {
                validMoves.push_back(targetPos);
            }
        }
    }

    Engine::GetLogger().LogDebug(
        character->TypeName() + " has " +
        std::to_string(validMoves.size()) + " valid moves"
    );

    return validMoves;
}

bool GridSystem::IsValidMove(Math::vec2 from, Math::vec2 to, int maxCost) {
    // Check bounds
    if (!IsValidPosition(to)) return false;

    // Check occupied
    if (IsPositionOccupied(to)) return false;

    // Check path cost
    int cost = CalculatePathCost(from, to);
    if (cost > maxCost) return false;

    // Check obstacles (simplified)
    if (!IsPathClear(from, to)) return false;

    return true;
}

int GridSystem::CalculatePathCost(Math::vec2 from, Math::vec2 to) {
    // Manhattan distance (L1 norm)
    int dx = static_cast<int>(std::abs(to.x - from.x));
    int dy = static_cast<int>(std::abs(to.y - from.y));

    return dx + dy; // 1 AP per tile
}

bool GridSystem::IsPositionOccupied(Math::vec2 position) {
    for (Character* character : registeredCharacters) {
        GridPosition* gridPos = character->GetGOComponent<GridPosition>();
        if (gridPos != nullptr) {
            Math::vec2 charPos = gridPos->GetTilePosition();
            if (charPos.x == position.x && charPos.y == position.y) {
                return true;
            }
        }
    }
    return false;
}
```

### MovementAction Implementation

```cpp
// MovementAction.cpp
bool MovementAction::ExecuteMove(
    Character* character,
    Math::vec2 targetPosition,
    GridSystem* grid,
    TurnManager* turnMgr
) {
    // Get current position
    GridPosition* gridPos = character->GetGOComponent<GridPosition>();
    if (gridPos == nullptr) {
        Engine::GetLogger().LogError("Character missing GridPosition component");
        return false;
    }

    Math::vec2 currentPos = gridPos->GetTilePosition();

    // Calculate move cost
    int moveCost = grid->CalculatePathCost(currentPos, targetPosition);

    // Check AP
    ActionPoints* ap = character->GetGOComponent<ActionPoints>();
    if (ap == nullptr || !ap->CanAffordAction(moveCost)) {
        Engine::GetLogger().LogError("Cannot afford move (needs " + std::to_string(moveCost) + " AP)");
        return false;
    }

    // Validate move
    if (!grid->IsValidMove(currentPos, targetPosition, ap->GetCurrent())) {
        Engine::GetLogger().LogError("Invalid move");
        return false;
    }

    // Publish start event
    EventBus::Instance().Publish(MovementStartEvent{character, currentPos, targetPosition});

    // Execute movement
    gridPos->SetTilePosition(
        static_cast<int>(targetPosition.x),
        static_cast<int>(targetPosition.y)
    );

    // Spend AP
    ap->SpendActionPoints(moveCost);

    // Publish end event
    EventBus::Instance().Publish(MovementEndEvent{character, currentPos, targetPosition, moveCost});

    Engine::GetLogger().LogEvent(
        character->TypeName() + " moved from " + VecToString(currentPos) +
        " to " + VecToString(targetPosition) + " (cost: " + std::to_string(moveCost) + " AP)"
    );

    return true;
}
```

### Event Definitions

```cpp
// MovementEvents.h
struct MovementStartEvent {
    Character* character;
    Math::vec2 fromPosition;
    Math::vec2 toPosition;
};

struct MovementEndEvent {
    Character* character;
    Math::vec2 fromPosition;
    Math::vec2 toPosition;
    int actionPointsCost;
};
```

### Verification Tests

| Test                | Expected Result                                            |
| ------------------- | ---------------------------------------------------------- |
| **Basic move**      | Move Dragon 1 tile, verify position updated and 1 AP spent |
| **Range**           | Char with 3 AP can move up to 3 tiles                      |
| **Obstacles**       | Cannot move through occupied tiles                         |
| **Path cost**       | Moving 2 tiles costs 2 AP                                  |
| **Insufficient AP** | Cannot move if not enough AP                               |
| **Valid moves**     | `GetValidMoves()` returns only reachable tiles             |
| **Out of bounds**   | Cannot move outside 8x8 grid                               |
| **Events**          | Movement events fire with correct data                     |

---

## Week 10-11: Spell System (Dragon)

### Purpose

Implement **spell casting** for Dragon with 3 unique spells.

### Components to Build

#### 1. SpellSystem (GSComponent)

**Location**: `CS230/Engine/SpellSystem.h/cpp`

**Responsibilities**:

- Validate spell casting (slots, range, AP, line of sight)
- Execute spell effects (damage, healing, terrain)
- Consume spell slots and action points
- Publish spell events

**Key Methods**:

```cpp
bool CanCastSpell(Character* caster, SpellType spell, Math::vec2 target);
void CastSpell(Character* caster, SpellType spell, Math::vec2 target);
void ExecuteFireball(Character* caster, Math::vec2 target, const SpellData& data);
void ExecuteHeal(Character* caster, Math::vec2 target, const SpellData& data);
void ExecuteWall(Character* caster, Math::vec2 target, const SpellData& data);
```

#### 2. SpellSlots (GOComponent)

**Location**: `CS230/Engine/SpellSlots.h`

**Responsibilities**:

- Track available/max spell slots per level
- Validate slot availability before casting
- Consume slots when spells are cast
- Refresh slots (rest mechanic)

**Key Methods**:

```cpp
bool HasSlotAvailable(int spellLevel);
void ConsumeSpellSlot(int spellLevel);
void RefreshSlots();
int GetAvailableSlots(int spellLevel);
```

### JSON Configuration

**File**: `Assets/Data/spells.json`

```json
{
  "spells": {
    "fireball": {
      "name": "Fireball",
      "level": 1,
      "damage": "3d6",
      "range": 5,
      "aoeRadius": 1,
      "actionPointCost": 2,
      "description": "Explodes on impact, damaging all in area."
    },
    "heal": {
      "name": "Heal",
      "level": 1,
      "healing": "2d8+5",
      "range": 3,
      "aoeRadius": 0,
      "actionPointCost": 2,
      "description": "Restores health to target."
    },
    "wall": {
      "name": "Stone Wall",
      "level": 1,
      "duration": 3,
      "range": 4,
      "wallLength": 3,
      "actionPointCost": 2,
      "description": "Creates impassable stone tiles for 3 turns."
    }
  },
  "characterSpells": {
    "dragon": {
      "spellSlots": {
        "level1": 2,
        "level2": 1,
        "level3": 1
      },
      "knownSpells": ["fireball", "heal", "wall"]
    }
  }
}
```

### Code Example: Spell Casting

```cpp
// SpellSystem.cpp
void SpellSystem::CastSpell(Character* caster, SpellType spell, Math::vec2 target) {
    if (!CanCastSpell(caster, spell, target)) {
        return;
    }

    SpellData spellData = GetSpellData(spell);

    Engine::GetLogger().LogEvent(caster->TypeName() + " casts " + spellData.name + "!");

    // Consume slot
    SpellSlots* slots = caster->GetGOComponent<SpellSlots>();
    slots->ConsumeSpellSlot(spellData.level);

    // Spend AP
    ActionPoints* ap = caster->GetGOComponent<ActionPoints>();
    ap->SpendActionPoints(spellData.actionPointCost);

    // Execute spell effect
    switch (spell) {
        case SpellType::Fireball:
            ExecuteFireball(caster, target, spellData);
            break;
        case SpellType::Heal:
            ExecuteHeal(caster, target, spellData);
            break;
        case SpellType::Wall:
            ExecuteWall(caster, target, spellData);
            break;
    }

    // Publish event
    EventBus::Instance().Publish(SpellCastEvent{caster, spell, target});

    // Visual effect
    EffectManager::Instance().PlaySpellEffect(spell, target);
}

void SpellSystem::ExecuteFireball(Character* caster, Math::vec2 target, const SpellData& spellData) {
    // Parse dice notation "3d6"
    int count, sides;
    ParseDiceNotation(spellData.damage, count, sides);

    // Roll damage
    int baseDamage = DiceManager::Instance().RollDice(count, sides);
    Engine::GetLogger().LogEvent("Fireball rolls " + std::to_string(baseDamage) + " damage");

    // Get all characters in AoE radius
    GridSystem* grid = GetOwner()->GetGSComponent<GridSystem>();
    std::vector<Character*> targets = grid->GetCharactersInRadius(target, spellData.aoeRadius);

    // Apply damage to each target
    CombatSystem* combat = GetOwner()->GetGSComponent<CombatSystem>();
    for (Character* targetChar : targets) {
        // Apply magic resistance
        DamageCalculator* defense = targetChar->GetGOComponent<DamageCalculator>();
        int finalDamage = (defense != nullptr)
            ? defense->CalculateMagicalDamage(baseDamage)
            : baseDamage;

        combat->ApplyDamage(targetChar, finalDamage);

        Engine::GetLogger().LogEvent(
            "Fireball hits " + targetChar->TypeName() +
            " for " + std::to_string(finalDamage) + " damage"
        );
    }
}

void SpellSystem::ExecuteHeal(Character* caster, Math::vec2 target, const SpellData& spellData) {
    // Find character at target
    GridSystem* grid = GetOwner()->GetGSComponent<GridSystem>();
    Character* targetChar = grid->GetCharacterAt(target);

    if (targetChar == nullptr) {
        Engine::GetLogger().LogError("No character at target for heal");
        return;
    }

    // Parse healing dice "2d8+5"
    int count, sides, modifier;
    ParseDiceNotationWithModifier(spellData.healing, count, sides, modifier);

    // Roll healing
    int healAmount = DiceManager::Instance().RollDiceWithModifier(count, sides, modifier);

    // Apply healing
    int currentHP = targetChar->GetCurrentHP();
    int maxHP = targetChar->GetMaxHP();
    int newHP = std::min(maxHP, currentHP + healAmount);
    int actualHealing = newHP - currentHP;

    targetChar->SetCurrentHP(newHP);

    EventBus::Instance().Publish(HealEvent{targetChar, healAmount, actualHealing});

    Engine::GetLogger().LogEvent(
        targetChar->TypeName() + " healed for " + std::to_string(actualHealing) + " HP"
    );
}

void SpellSystem::ExecuteWall(Character* caster, Math::vec2 target, const SpellData& spellData) {
    GridSystem* grid = GetOwner()->GetGSComponent<GridSystem>();

    // Create horizontal line of wall tiles
    for (int i = 0; i < spellData.wallLength; i++) {
        Math::vec2 wallTile = target + Math::vec2{static_cast<double>(i), 0};

        if (grid->IsValidPosition(wallTile) && !grid->IsPositionOccupied(wallTile)) {
            grid->AddObstacle(wallTile, spellData.duration);
            Engine::GetLogger().LogEvent("Wall tile at " + VecToString(wallTile));
        }
    }

    Engine::GetLogger().LogEvent("Stone wall created for " + std::to_string(spellData.duration) + " turns");
}
```

### SpellSlots Component

```cpp
// SpellSlots.h
class SpellSlots : public CS230::Component {
public:
    SpellSlots(std::map<int, int> maxSlots)
        : maxSlotsPerLevel(maxSlots), currentSlotsPerLevel(maxSlots) {}

    bool HasSlotAvailable(int spellLevel) const {
        auto it = currentSlotsPerLevel.find(spellLevel);
        return (it != currentSlotsPerLevel.end() && it->second > 0);
    }

    void ConsumeSpellSlot(int spellLevel) {
        if (HasSlotAvailable(spellLevel)) {
            currentSlotsPerLevel[spellLevel]--;
            Engine::GetLogger().LogDebug(
                "Spell slot consumed (Level " + std::to_string(spellLevel) + "): " +
                std::to_string(currentSlotsPerLevel[spellLevel]) + " remaining"
            );
        }
    }

    void RefreshSlots() {
        currentSlotsPerLevel = maxSlotsPerLevel;
        Engine::GetLogger().LogEvent("Spell slots refreshed");
    }

    int GetAvailableSlots(int spellLevel) const {
        auto it = currentSlotsPerLevel.find(spellLevel);
        return (it != currentSlotsPerLevel.end()) ? it->second : 0;
    }

private:
    std::map<int, int> maxSlotsPerLevel;
    std::map<int, int> currentSlotsPerLevel;
};
```

### Event Definitions

```cpp
// SpellEvents.h
enum class SpellType {
    Fireball,
    Heal,
    Wall,
    None
};

struct SpellData {
    std::string name;
    int level;
    std::string damage;
    std::string healing;
    int range;
    int aoeRadius;
    int duration;
    int wallLength;
    int actionPointCost;
    std::string description;
};

struct SpellCastEvent {
    Character* caster;
    SpellType spell;
    Math::vec2 targetPosition;
};

struct HealEvent {
    Character* target;
    int healRoll;
    int actualHealing;
};
```

### Verification Tests

| Test                 | Expected Result                                |
| -------------------- | ---------------------------------------------- |
| **Fireball**         | Dragon casts at Fighter, verify damage applied |
| **Fireball AoE**     | 2 adjacent enemies, both take damage           |
| **Heal**             | Dragon casts on self, HP restored              |
| **Wall**             | Dragon creates wall, tiles become impassable   |
| **Spell slots**      | Dragon has 2 level-1 slots, can cast 2 spells  |
| **Slot consumption** | Cast spell, verify slot decremented            |
| **Range**            | Attempt cast beyond range, verify rejection    |
| **AP cost**          | Casting costs 2 AP, verify deduction           |
| **Events**           | Spell events fire with correct data            |

---

## Week 11-12: Basic AI (Fighter)

### Purpose

Implement **basic AI decision-making** for Fighter to enable single-player gameplay.

### Components to Build

#### 1. AISystem (GSComponent)

**Location**: `CS230/Engine/AISystem.h/cpp`

**Responsibilities**:

- Execute AI logic on opponent characters' turns
- Make decisions based on character type (Fighter behavior)
- Execute actions (move, attack)
- Add decision delay for player visibility

**Key Methods**:

```cpp
void UpdateAI(Character* aiCharacter, double dt);
AIDecision MakeDecision(Character* aiCharacter);
AIDecision MakeFighterDecision(Character* fighter);
Math::vec2 FindBestMoveToward(Character* character, Math::vec2 target);
void ExecuteDecision(Character* aiCharacter, const AIDecision& decision);
Character* FindPlayerCharacter(); // Find Dragon
```

#### 2. AIDirector (Singleton)

**Location**: `CS230/Engine/AIDirector.h/cpp`

**Responsibilities** (Basic Version):

- Set global AI difficulty level
- Future: Team coordination, dynamic difficulty

**Key Methods**:

```cpp
static AIDirector& Instance();
void SetDifficultyLevel(int level);
int GetDifficultyLevel();
```

### AI Behavior: Fighter

**Strategy**: Aggressive melee combatant

1. **Target Selection**: Always target Dragon (player character)
2. **Decision Logic**:
   - If adjacent to Dragon → Attack
   - If not adjacent → Move closer
   - If no AP remaining → End turn

### Code Example: AI Decision Making

```cpp
// AISystem.cpp
void AISystem::UpdateAI(Character* aiCharacter, double dt) {
    if (aiCharacter == nullptr || !aiCharacter->IsAlive()) {
        return;
    }

    // Check if it's this AI's turn
    TurnManager* turnMgr = GetOwner()->GetGSComponent<TurnManager>();
    if (turnMgr->GetCurrentCharacter() != aiCharacter) {
        return;
    }

    // Add decision delay for player visibility
    aiDecisionTimer += dt;
    if (aiDecisionTimer < AI_DECISION_DELAY) {
        return;
    }
    aiDecisionTimer = 0.0;

    Engine::GetLogger().LogEvent("=== " + aiCharacter->TypeName() + " AI thinking ===");

    // Make decision
    AIDecision decision = MakeDecision(aiCharacter);

    // Execute decision
    ExecuteDecision(aiCharacter, decision);

    // Check if AI has more actions
    ActionPoints* ap = aiCharacter->GetGOComponent<ActionPoints>();
    if (ap->GetCurrent() > 0) {
        Engine::GetLogger().LogDebug(aiCharacter->TypeName() + " has " + std::to_string(ap->GetCurrent()) + " AP remaining");
    } else {
        Engine::GetLogger().LogEvent(aiCharacter->TypeName() + " ending turn");
        turnMgr->AdvanceToNextTurn();
    }
}

AIDecision AISystem::MakeFighterDecision(Character* fighter) {
    // Fighter strategy: Move toward and attack Dragon

    Character* dragon = FindPlayerCharacter();
    if (dragon == nullptr || !dragon->IsAlive()) {
        Engine::GetLogger().LogDebug("Fighter: No valid target");
        return AIDecision{AIActionType::EndTurn};
    }

    GridPosition* fighterPos = fighter->GetGOComponent<GridPosition>();
    GridPosition* dragonPos = dragon->GetGOComponent<GridPosition>();

    Math::vec2 fighterTile = fighterPos->GetTilePosition();
    Math::vec2 dragonTile = dragonPos->GetTilePosition();

    double distance = (dragonTile - fighterTile).Length();

    Engine::GetLogger().LogDebug(
        "Fighter at " + VecToString(fighterTile) +
        ", Dragon at " + VecToString(dragonTile) +
        ", distance: " + std::to_string(distance)
    );

    // Check if in melee range (adjacent = distance ~1.0)
    if (distance <= 1.5) {
        Engine::GetLogger().LogEvent("Fighter: Dragon in range, attacking!");
        return AIDecision{AIActionType::Attack, dragonTile};
    } else {
        Math::vec2 moveTarget = FindBestMoveToward(fighter, dragonTile);
        Engine::GetLogger().LogEvent("Fighter: Moving toward Dragon");
        return AIDecision{AIActionType::Move, moveTarget};
    }
}

Math::vec2 AISystem::FindBestMoveToward(Character* character, Math::vec2 targetTile) {
    GridSystem* grid = GetOwner()->GetGSComponent<GridSystem>();
    Math::vec2 currentPos = character->GetGOComponent<GridPosition>()->GetTilePosition();

    // Get all valid moves
    std::vector<Math::vec2> validMoves = grid->GetValidMoves(character);

    if (validMoves.empty()) {
        Engine::GetLogger().LogDebug("No valid moves available");
        return currentPos;
    }

    // Find move closest to target
    Math::vec2 bestMove = currentPos;
    double bestDistance = (targetTile - currentPos).Length();

    for (Math::vec2 move : validMoves) {
        double distanceToTarget = (targetTile - move).Length();
        if (distanceToTarget < bestDistance) {
            bestDistance = distanceToTarget;
            bestMove = move;
        }
    }

    return bestMove;
}

void AISystem::ExecuteDecision(Character* aiCharacter, const AIDecision& decision) {
    GridSystem* grid = GetOwner()->GetGSComponent<GridSystem>();
    CombatSystem* combat = GetOwner()->GetGSComponent<CombatSystem>();
    TurnManager* turnMgr = GetOwner()->GetGSComponent<TurnManager>();

    switch (decision.actionType) {
        case AIActionType::Move: {
            bool success = MovementAction::ExecuteMove(aiCharacter, decision.targetPosition, grid, turnMgr);
            if (!success) {
                Engine::GetLogger().LogError("AI move failed");
            }
            break;
        }

        case AIActionType::Attack: {
            Character* target = grid->GetCharacterAt(decision.targetPosition);
            if (target != nullptr) {
                combat->ExecuteAttack(aiCharacter, target);
            } else {
                Engine::GetLogger().LogError("No target at attack position");
            }
            break;
        }

        case AIActionType::EndTurn:
            Engine::GetLogger().LogEvent("AI ending turn");
            turnMgr->AdvanceToNextTurn();
            break;
    }
}

Character* AISystem::FindPlayerCharacter() {
    GameObjectManager* objMgr = GetOwner()->GetGSComponent<GameObjectManager>();

    for (int i = 0; i < objMgr->GetObjectCount(); i++) {
        CS230::GameObject* obj = objMgr->GetGameObject(i);
        Character* character = dynamic_cast<Character*>(obj);

        if (character != nullptr && character->GetCharacterType() == GameObjectTypes::Dragon) {
            return character;
        }
    }

    return nullptr;
}
```

### AI Decision Structure

```cpp
// AIDecision.h
enum class AIActionType {
    Move,
    Attack,
    CastSpell,
    UseItem,
    EndTurn,
    Wait
};

struct AIDecision {
    AIActionType actionType;
    Math::vec2 targetPosition{0, 0};
    SpellType spell = SpellType::None;
    int priority = 0;
};
```

### Verification Tests

| Test                 | Expected Result                              |
| -------------------- | -------------------------------------------- |
| **Fighter AI**       | Fighter moves toward Dragon on its turn      |
| **Attack**           | Fighter attacks Dragon when adjacent         |
| **Turn integration** | Fighter waits for its turn in turn order     |
| **Target selection** | Fighter always targets Dragon                |
| **Pathfinding**      | Fighter finds valid path to Dragon           |
| **AP management**    | Fighter uses all available AP efficiently    |
| **Logging**          | AI decisions logged to Logger.txt            |
| **Animation delay**  | AI actions have visible delay for player     |
| **Multiple rounds**  | Fighter continues acting over multiple turns |

---

## Debug Tools

### Overview

Debug tools accelerate Phase 2 development by enabling instant testing of combat scenarios.

### Console Commands

#### Turn Management

| Command               | Description                         |
| --------------------- | ----------------------------------- |
| `endturn`             | Force current character to end turn |
| `nextturn [count]`    | Advance N turns (default 1)         |
| `pause`               | Pause turn progression              |
| `resume`              | Resume turn progression             |
| `timescale <value>`   | Speed up/slow down game time        |
| `setturn <character>` | Force specific character's turn     |

#### Resource Manipulation

| Command                              | Description              |
| ------------------------------------ | ------------------------ |
| `damage <character> <amount>`        | Deal damage to character |
| `heal <character> <amount>`          | Heal character           |
| `setstat <character> <stat> <value>` | Modify character stat    |
| `giveap <character> <amount>`        | Give action points       |
| `givespell <character> <spell>`      | Grant spell slot         |

#### Visualization Toggles

| Command / Key           | Description                     |
| ----------------------- | ------------------------------- |
| `debug collision` / F10 | Toggle collision box display    |
| `debug movement` / F2   | Toggle movement range display   |
| `debug spellrange` / F3 | Toggle spell range display      |
| `debug hpbars`          | Toggle HP bars above characters |
| `debug apinfo`          | Toggle action point display     |

#### God Mode Features

| Command                   | Description                         |
| ------------------------- | ----------------------------------- |
| `god`                     | Toggle invincibility                |
| `god damage <multiplier>` | Multiply all damage dealt by player |
| `god ap`                  | Infinite action points              |
| `god spells`              | Infinite spell slots                |
| `god crit`                | All attacks are critical hits       |

### Implementation Example

```cpp
// DebugConsole.cpp
void DebugConsole::RegisterPhase2Commands() {
    RegisterCommand("damage",
        [](std::vector<std::string> args) {
            if (args.size() < 2) {
                DebugConsole::Instance().AddOutput("Usage: damage <character> <amount>", ConsoleColor::Red);
                return;
            }

            std::string characterName = args[0];
            int damageAmount = std::stoi(args[1]);

            Character* target = FindCharacterByName(characterName);
            if (target != nullptr) {
                CombatSystem* combat = GetCurrentBattleState()->GetGSComponent<CombatSystem>();
                combat->ApplyDamage(target, damageAmount);

                DebugConsole::Instance().AddOutput(
                    "Dealt " + std::to_string(damageAmount) + " damage to " + characterName,
                    ConsoleColor::Yellow
                );
            } else {
                DebugConsole::Instance().AddOutput("Character not found: " + characterName, ConsoleColor::Red);
            }
        },
        "Deal damage to a character"
    );
}
```

### StatusInfoOverlay

Displays HP bars and AP counters above characters.

```cpp
// StatusInfoOverlay.cpp
void StatusInfoOverlay::DrawCharacterInfo(Character* character, Math::TransformationMatrix camera_matrix) {
    Math::vec2 worldPos = character->GetPosition();
    Math::vec2 screenPos = camera_matrix * worldPos;
    screenPos.y -= 50; // Offset above sprite

    // HP bar
    int currentHP = character->GetCurrentHP();
    int maxHP = character->GetMaxHP();
    float hpPercent = static_cast<float>(currentHP) / static_cast<float>(maxHP);

    DrawHealthBar(screenPos, hpPercent, 40, 5);

    // HP text
    std::string hpText = std::to_string(currentHP) + "/" + std::to_string(maxHP);
    Engine::GetFont(0).DrawText(hpText, screenPos + Math::vec2{0, -10});

    // AP info
    ActionPoints* ap = character->GetGOComponent<ActionPoints>();
    if (ap != nullptr) {
        std::string apText = "AP: " + std::to_string(ap->GetCurrent()) + "/" + std::to_string(ap->GetMax());
        Engine::GetFont(0).DrawText(apText, screenPos + Math::vec2{0, -25});
    }
}
```

---

## Integration Testing

### Complete Phase 2 Test Scenario

Create `Phase2TestState` to validate all systems working together:

```cpp
// Phase2TestState.cpp
void Phase2TestState::Load() {
    // Load all configuration
    DataRegistry::Instance().LoadFromFile("Assets/Data/turn_config.json");
    DataRegistry::Instance().LoadFromFile("Assets/Data/combat_config.json");
    DataRegistry::Instance().LoadFromFile("Assets/Data/spells.json");

    // Create game systems
    gridSystem = AddGSComponent(new GridSystem());
    turnManager = AddGSComponent(new TurnManager());
    combatSystem = AddGSComponent(new CombatSystem());
    spellSystem = AddGSComponent(new SpellSystem());
    aiSystem = AddGSComponent(new AISystem());

    // Create characters
    auto dragon = CharacterFactory::CreateCharacter(CharacterType::Dragon);
    auto fighter = CharacterFactory::CreateCharacter(CharacterType::Fighter);

    // Position on grid
    dragon->GetGOComponent<GridPosition>()->SetTilePosition(3, 7);
    fighter->GetGOComponent<GridPosition>()->SetTilePosition(3, 2);

    // Register with systems
    turnManager->RegisterCharacter(dragon);
    turnManager->RegisterCharacter(fighter);
    gridSystem->RegisterCharacter(dragon);
    gridSystem->RegisterCharacter(fighter);

    // Subscribe to events
    EventBus::Instance().Subscribe<TurnStartEvent>([](const TurnStartEvent& e) {
        Engine::GetLogger().LogEvent(">>> Turn: " + e.character->TypeName());
    });

    EventBus::Instance().Subscribe<AttackEvent>([](const AttackEvent& e) {
        Engine::GetLogger().LogEvent(
            ">>> Attack: " + e.attacker->TypeName() + " -> " +
            e.target->TypeName() + " for " + std::to_string(e.damage) + " damage"
        );
    });

    // Start first turn
    turnManager->AdvanceToNextTurn();

    Engine::GetLogger().LogEvent("=== Phase 2 Integration Test Started ===");
}
```

### Success Criteria Checklist

- ✅ Dragon and Fighter take turns in correct order (speed-based)
- ✅ Fighter AI moves toward and attacks Dragon
- ✅ Dragon can move, attack, and cast 3 spells
- ✅ All damage uses dice rolls and armor reduction
- ✅ Action points are spent and tracked correctly
- ✅ Spell slots are consumed when casting
- ✅ All events are published and logged
- ✅ Debug console commands work for testing
- ✅ HP bars and AP display update in real-time
- ✅ System runs at stable 30 FPS

### Common Issues to Watch For

⚠️ **Turn order not updating** → Check `RefreshTurnOrder()` is called after character registration
⚠️ **AP not refreshing** → Verify `RefreshActionPoints()` is called in `AdvanceToNextTurn()`
⚠️ **Damage not varying** → Confirm DiceManager is seeded properly
⚠️ **AI not moving** → Check `GetValidMoves()` returns non-empty list
⚠️ **Spells failing** → Validate spell slots are initialized from JSON
⚠️ **Events not firing** → Ensure EventBus subscriptions happen in `Load()`

---

## File Structure Summary

### New Files Created

```
CS230/
├── Engine/
│   ├── TurnManager.h/cpp              [NEW]
│   ├── ActionPoints.h                 [NEW - GOComponent]
│   ├── TurnEvents.h                   [NEW]
│   ├── CombatSystem.h/cpp             [NEW]
│   ├── DamageCalculator.h             [NEW - GOComponent]
│   ├── CombatEvents.h                 [NEW]
│   ├── MovementAction.h/cpp           [NEW]
│   ├── MovementEvents.h               [NEW]
│   ├── SpellSystem.h/cpp              [NEW]
│   ├── SpellSlots.h                   [NEW - GOComponent]
│   ├── SpellEvents.h                  [NEW]
│   ├── AISystem.h/cpp                 [NEW]
│   ├── AIDirector.h/cpp               [NEW - Singleton]
│   ├── AIDecision.h                   [NEW]
│   ├── CollisionDebugRenderer.h/cpp   [NEW]
│   ├── StatusInfoOverlay.h/cpp        [NEW]
│   └── GridSystem.h/cpp               [MODIFIED - movement methods]
├── Game/
│   └── Phase2TestState.h/cpp          [NEW]
└── Assets/
    └── Data/
        ├── turn_config.json            [NEW]
        ├── combat_config.json          [NEW]
        ├── spells.json                 [NEW]
        ├── ai_config.json              [NEW]
        └── movement_config.json        [NEW]
```

### Modified Files

- **GridSystem.h/cpp**: Added `GetValidMoves()`, `IsValidMove()`, `CalculatePathCost()`, `IsPositionOccupied()`
- **DebugConsole.h/cpp**: Added Phase 2 commands (damage, heal, nextturn, etc.)
- **GodModeManager.h/cpp**: Extended with combat-related god mode features

---

## Next Phase Preview

**Phase 3: AI & Interface Polish (Weeks 13-18)**

After completing Phase 2, you will have:

- Functional turn-based combat
- Dragon vs Fighter gameplay
- Basic spell system
- Simple AI opponent

Phase 3 will add:

- Complete AI for all 4 character types (Cleric, Wizard, Rogue)
- Interface design for polymorphic behavior
- Status effect system (buffs, debuffs, DoT)
- Complete spell system for all characters
- AI memory and threat assessment
- BattleManager for victory/defeat conditions
- Advanced debug tools for AI visualization

---

**Last Updated**: 2025-01-XX
**Status**: ⬜ Not Started | 🟡 In Progress | ✅ Completed
**Current Week**: Week ___
