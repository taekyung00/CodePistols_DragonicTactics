# Playtest 1 Implementation Plan - Week 3

**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Combat + Spells Begin
**Timeline**: Week 3 of 5 (Playtest 1 milestone)
**Strategy**: Full combat system with Dragon spells and Fighter abilities

**Last Updated**: 2025-10-17
**Week 2 Status**: Dragon and Fighter classes complete, GridSystem pathfinding working, TurnManager basics functional

**Related Documentation**:

- [Week 1](week1.md) for foundation systems
- [Week 2](week2.md) for Dragon and Fighter base classes
- [docs/implementation-plan.md](../../implementation-plan.md) for complete 26-week timeline

---

## Overview

Week 3 brings the tactical combat to life by implementing all three Dragon spells (Fireball, CreateWall, LavaPool), Fighter combat abilities, and the complete SpellSystem. This is where the game becomes truly playable.

**Critical Success Criteria:**

- ✅ **All 3 Dragon Spells**: Fireball (damage), CreateWall (terrain), LavaPool (area denial)
- ✅ **Fighter Combat Abilities**: Melee attack, Shield Bash (crowd control)
- ✅ **SpellSystem Complete**: CastSpell(), spell slot management, validation, upcasting
- ✅ **DataRegistry Basics**: Load dragon_stats.json and fighter_stats.json
- ✅ **BattleState Integration**: All systems work together in a complete battle

**Integration Goal (Friday Week 3)**:

- Dragon vs Fighter full combat
- Dragon casts all 3 spells successfully
- Fighter uses melee attack and Shield Bash
- Damage calculated with dice rolls
- Combat ends when character dies
- Data loaded from JSON files

**Meeting Schedule:**

- **Daily Standups**: 10 min each morning (9:00 AM)
- **Friday Integration Meeting**: 60 min (2:00 PM - full combat demo + Week 4 planning)

---

## Table of Contents

- [Week 3 Overview](#overview)
- [Developer A: All 3 Dragon Spells](#week-3-developer-a-all-3-dragon-spells)
  - [Implementation Tasks](#implementation-tasks-dragon-spells)
  - [Rigorous Testing](#rigorous-testing-dragon-spells)
  - [Daily Breakdown](#daily-breakdown-developer-a)
- [Developer B: Fighter Combat Abilities](#week-3-developer-b-fighter-combat-abilities)
  - [Implementation Tasks](#implementation-tasks-fighter-abilities)
  - [Rigorous Testing](#rigorous-testing-fighter-abilities)
  - [Daily Breakdown](#daily-breakdown-developer-b)
- [Developer C: SpellSystem Complete](#week-3-developer-c-spellsystem-complete)
  - [Implementation Tasks](#implementation-tasks-spellsystem)
  - [Rigorous Testing](#rigorous-testing-spellsystem)
  - [Daily Breakdown](#daily-breakdown-developer-c)
- [Developer D: DataRegistry Basics](#week-3-developer-d-dataregistry-basics)
  - [Implementation Tasks](#implementation-tasks-dataregistry)
  - [Rigorous Testing](#rigorous-testing-dataregistry)
  - [Daily Breakdown](#daily-breakdown-developer-d)
- [Developer E: BattleState Integration](#week-3-developer-e-battlestate-integration)
  - [Implementation Tasks](#implementation-tasks-battlestate)
  - [Rigorous Testing](#rigorous-testing-battlestate)
  - [Daily Breakdown](#daily-breakdown-developer-e)
- [Week 3 Integration Test](#week-3-integration-test-friday-afternoon)
- [Week 3 Deliverable & Verification](#week-3-deliverable--verification)

---

## Week 3: Developer A - All 3 Dragon Spells

**Goal**: Implement all three Dragon spells with complete functionality

**Foundation**:

- Dragon class from Week 2 provides spell slots and basic stats
- SpellSystem structure from Developer C (parallel development)
- GridSystem from Week 1 provides tile validation and pathfinding

**Files to Create**:

```
CS230/Game/Spells/Fireball.h
CS230/Game/Spells/Fireball.cpp
CS230/Game/Spells/CreateWall.h
CS230/Game/Spells/CreateWall.cpp
CS230/Game/Spells/LavaPool.h
CS230/Game/Spells/LavaPool.cpp
CS230/Game/Spells/SpellBase.h (interface)
```

### Implementation Tasks (Dragon Spells)

#### **Task 1: SpellBase Interface**

**Why this step**: We need a common interface for all spells to ensure consistent behavior and enable polymorphism in the SpellSystem. This allows the SpellSystem to treat all spells uniformly while each spell implements its unique effects.

```cpp
// File: CS230/Game/Spells/SpellBase.h
#pragma once
#include "../../Engine/Vec2.h"
#include <string>
#include <vector>

class Character;

// Step 1.1: Define spell result structure
// Reason: Spells need to communicate back what happened (damage dealt, tiles affected, etc.)
// This enables proper event publishing and UI feedback
struct SpellResult {
    bool success;                          // Did the spell cast successfully?
    std::vector<Character*> affectedTargets; // Who was hit?
    std::vector<Math::vec2> affectedTiles;   // Which tiles were affected?
    int damageDealt;                         // Total damage (for damage spells)
    std::string failureReason;               // Why it failed (out of range, no slots, etc.)
};

// Step 1.2: Define spell targeting types
// Reason: Different spells target differently (single enemy, area, self, etc.)
// This allows UI to show appropriate targeting cursors
enum class SpellTargetType {
    Single,      // Target one character (e.g., Magic Missile)
    Area,        // Target area centered on a tile (e.g., Fireball)
    Line,        // Target tiles in a line (e.g., Lightning Bolt)
    Self,        // Target caster only (e.g., Shield spell)
    AllEnemies   // Target all enemies (rare, powerful spells)
};

// Step 1.3: Base spell interface
// Reason: All spells share common properties (name, cost, range) and behavior (validation, casting)
// Using pure virtual functions ensures every spell implements required methods
class SpellBase {
public:
    virtual ~SpellBase() = default;

    // Core spell information
    virtual std::string GetName() const = 0;
    virtual int GetLevel() const = 0;
    virtual int GetRange() const = 0;
    virtual SpellTargetType GetTargetType() const = 0;
    virtual std::string GetDescription() const = 0;

    // Spell validation (before casting)
    // Reason: Prevent invalid casts (out of range, invalid target, etc.)
    virtual bool CanCast(Character* caster, Math::vec2 targetTile) const = 0;

    // Spell execution
    // Reason: This is where the spell's unique effect happens
    virtual SpellResult Cast(Character* caster, Math::vec2 targetTile, int upcastLevel = 0) = 0;

    // Area of effect calculation
    // Reason: Some spells affect multiple tiles (Fireball, LavaPool)
    // Returns all tiles affected by the spell
    virtual std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const = 0;

protected:
    // Helper: Check if target is in range
    // Reason: Common logic shared by all spells
    bool IsInRange(Math::vec2 casterPos, Math::vec2 targetPos, int range) const {
        int dx = abs((int)targetPos.x - (int)casterPos.x);
        int dy = abs((int)targetPos.y - (int)casterPos.y);
        return (dx + dy) <= range; // Manhattan distance
    }
};
```

#### **Task 2: Fireball Spell Implementation**

**Why this step**: Fireball is the Dragon's primary damage spell. It's an area-of-effect spell that teaches us how to handle multi-target damage and spell slot consumption.

```cpp
// File: CS230/Game/Spells/Fireball.h
#pragma once
#include "SpellBase.h"

// Step 2.1: Fireball class declaration
// Reason: Fireball is a level 2 spell with area damage and upcasting capability
class Fireball : public SpellBase {
public:
    Fireball();
    ~Fireball() override = default;

    // Spell info
    std::string GetName() const override { return "Fireball"; }
    int GetLevel() const override { return 2; }
    int GetRange() const override { return 6; }
    SpellTargetType GetTargetType() const override { return SpellTargetType::Area; }
    std::string GetDescription() const override {
        return "Explodes at target tile, dealing 8d6 fire damage to all creatures within 2 tiles. +1d6 per spell level above 2nd.";
    }

    // Spell validation
    bool CanCast(Character* caster, Math::vec2 targetTile) const override;

    // Spell execution
    SpellResult Cast(Character* caster, Math::vec2 targetTile, int upcastLevel = 0) override;

    // Area calculation
    std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const override;

private:
    // Step 2.2: Fireball-specific configuration
    // Reason: These values define Fireball's balance and feel
    const int explosionRadius = 2;  // Affects tiles within 2 tiles of center
    const std::string baseDamage = "8d6";  // Base damage dice
    const int upcastBonusDice = 1;  // +1d6 per level above 2nd
};
```

```cpp
// File: CS230/Game/Spells/Fireball.cpp
#include "Fireball.h"
#include "../Character.h"
#include "../System/GridSystem.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../../Engine/Engine.h"

Fireball::Fireball() {
    // Constructor is simple - all config is in the class
}

// Step 2.3: Validate spell can be cast
// Reason: Prevent invalid casts that would waste spell slots or break game state
bool Fireball::CanCast(Character* caster, Math::vec2 targetTile) const {
    // Check 1: Caster exists
    if (!caster) {
        return false;
    }

    // Check 2: Target tile is valid on the grid
    // Reason: Can't cast at tiles that don't exist
    GridSystem& grid = GridSystem::GetInstance();
    if (!grid.IsValidTile(targetTile)) {
        return false;
    }

    // Check 3: Target is in range
    // Reason: Spells have limited range for balance
    Math::vec2 casterPos = caster->GetGridPosition();
    if (!IsInRange(casterPos, targetTile, GetRange())) {
        return false;
    }

    // Check 4: Caster has spell slots
    // Reason: D&D spell slot system - must have available slots
    if (!caster->HasSpellSlot(GetLevel())) {
        return false;
    }

    // Check 5: Line of sight (optional - can cast over walls for now)
    // Reason: Some spells require clear line of sight
    // TODO Week 4: Add line of sight check using GridSystem::HasLineOfSight()

    return true;
}

// Step 2.4: Calculate all tiles affected by explosion
// Reason: Fireball affects a radius around the impact point
std::vector<Math::vec2> Fireball::GetAffectedTiles(Math::vec2 targetTile) const {
    std::vector<Math::vec2> tiles;
    GridSystem& grid = GridSystem::GetInstance();

    // Step 2.4a: Check all tiles within radius
    // Reason: We use a square radius for simplicity (8x8 grid is small)
    for (int dx = -explosionRadius; dx <= explosionRadius; ++dx) {
        for (int dy = -explosionRadius; dy <= explosionRadius; ++dy) {
            Math::vec2 checkTile = targetTile + Math::vec2{(double)dx, (double)dy};

            // Only include valid tiles
            if (grid.IsValidTile(checkTile)) {
                tiles.push_back(checkTile);
            }
        }
    }

    return tiles;
}

// Step 2.5: Execute the spell
// Reason: This is where the spell's effect happens - damage calculation and application
SpellResult Fireball::Cast(Character* caster, Math::vec2 targetTile, int upcastLevel) {
    SpellResult result;
    result.success = false;

    // Step 2.5a: Validate cast (redundant check for safety)
    if (!CanCast(caster, targetTile)) {
        result.failureReason = "Cannot cast Fireball at this location";
        return result;
    }

    // Step 2.5b: Consume spell slot
    // Reason: Spell slots are the resource cost for casting
    int slotLevel = (upcastLevel > 0) ? upcastLevel : GetLevel();
    if (!caster->ConsumeSpellSlot(slotLevel)) {
        result.failureReason = "No spell slots remaining";
        return result;
    }

    // Step 2.5c: Calculate damage (with upcasting)
    // Reason: Fireball does 8d6, +1d6 per level above 2nd
    DiceManager& dice = DiceManager::GetInstance();
    int numDice = 8; // Base 8d6

    if (upcastLevel > GetLevel()) {
        numDice += (upcastLevel - GetLevel()) * upcastBonusDice;
    }

    std::string damageRoll = std::to_string(numDice) + "d6";
    int totalDamage = dice.RollDiceFromString(damageRoll);

    // Step 2.5d: Get all affected tiles
    result.affectedTiles = GetAffectedTiles(targetTile);

    // Step 2.5e: Find all characters in affected area
    // Reason: Fireball hits all creatures in the explosion radius
    GridSystem& grid = GridSystem::GetInstance();
    for (const Math::vec2& tile : result.affectedTiles) {
        Character* target = grid.GetCharacterAt(tile);
        if (target && target != caster) { // Don't hit self
            result.affectedTargets.push_back(target);
        }
    }

    // Step 2.5f: Apply damage to all targets
    result.damageDealt = 0;
    for (Character* target : result.affectedTargets) {
        // Each target takes full damage (no save in Week 3)
        // TODO Week 6: Add Dexterity saving throw for half damage
        target->TakeDamage(totalDamage, caster);
        result.damageDealt += totalDamage;
    }

    // Step 2.5g: Publish spell cast event
    // Reason: UI needs to show spell animation, combat log needs to record it
    EventBus::GetInstance().Publish(SpellCastEvent{
        caster,
        GetName(),
        targetTile,
        result.affectedTargets,
        totalDamage
    });

    // Step 2.5h: Log the spell cast
    Engine::GetLogger().LogEvent(
        caster->TypeName() + " casts Fireball at (" +
        std::to_string((int)targetTile.x) + "," + std::to_string((int)targetTile.y) +
        ") dealing " + std::to_string(totalDamage) + " damage to " +
        std::to_string(result.affectedTargets.size()) + " targets"
    );

    result.success = true;
    return result;
}
```

#### **Task 3: CreateWall Spell Implementation**

**Why this step**: CreateWall introduces terrain manipulation - a tactical spell that doesn't deal damage but controls the battlefield. This teaches us how to modify GridSystem state.

```cpp
// File: CS230/Game/Spells/CreateWall.h
#pragma once
#include "SpellBase.h"

// Step 3.1: CreateWall class declaration
// Reason: CreateWall is a level 1 spell that creates impassable terrain
class CreateWall : public SpellBase {
public:
    CreateWall();
    ~CreateWall() override = default;

    // Spell info
    std::string GetName() const override { return "Create Wall"; }
    int GetLevel() const override { return 1; }
    int GetRange() const override { return 5; }
    SpellTargetType GetTargetType() const override { return SpellTargetType::Line; }
    std::string GetDescription() const override {
        return "Creates a line of impassable wall tiles up to 5 tiles long. +1 tile per spell level above 1st.";
    }

    // Spell validation
    bool CanCast(Character* caster, Math::vec2 targetTile) const override;

    // Spell execution
    SpellResult Cast(Character* caster, Math::vec2 targetTile, int upcastLevel = 0) override;

    // Area calculation
    std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const override;

private:
    const int baseWallLength = 5;  // 5 tiles at level 1
    const int upcastBonusTiles = 1;  // +1 tile per level

    // Step 3.2: Helper to determine wall direction
    // Reason: Walls can be horizontal or vertical based on target position
    enum class WallDirection { Horizontal, Vertical };
    WallDirection DetermineDirection(Math::vec2 casterPos, Math::vec2 targetTile) const;
};
```

```cpp
// File: CS230/Game/Spells/CreateWall.cpp
#include "CreateWall.h"
#include "../Character.h"
#include "../System/GridSystem.h"
#include "../Singletons/EventBus.h"
#include "../../Engine/Engine.h"

CreateWall::CreateWall() {}

// Step 3.3: Determine wall direction based on cast direction
// Reason: If cast horizontally, wall is vertical (blocks horizontal movement)
CreateWall::WallDirection CreateWall::DetermineDirection(Math::vec2 casterPos, Math::vec2 targetTile) const {
    int dx = abs((int)targetTile.x - (int)casterPos.x);
    int dy = abs((int)targetTile.y - (int)casterPos.y);

    // If more horizontal distance, create vertical wall
    if (dx > dy) {
        return WallDirection::Vertical;
    }
    return WallDirection::Horizontal;
}

// Step 3.4: Validate spell can be cast
bool CreateWall::CanCast(Character* caster, Math::vec2 targetTile) const {
    if (!caster) return false;

    GridSystem& grid = GridSystem::GetInstance();
    if (!grid.IsValidTile(targetTile)) return false;

    Math::vec2 casterPos = caster->GetGridPosition();
    if (!IsInRange(casterPos, targetTile, GetRange())) return false;

    if (!caster->HasSpellSlot(GetLevel())) return false;

    // Check 5: Target tile must be empty
    // Reason: Can't create wall on occupied tiles
    if (grid.IsOccupied(targetTile)) {
        return false;
    }

    return true;
}

// Step 3.5: Calculate affected tiles (wall line)
std::vector<Math::vec2> CreateWall::GetAffectedTiles(Math::vec2 targetTile) const {
    std::vector<Math::vec2> tiles;
    tiles.push_back(targetTile); // Center tile

    // For Week 3, just return the single target tile
    // TODO Week 4: Implement full wall line based on direction

    return tiles;
}

// Step 3.6: Execute the spell
SpellResult CreateWall::Cast(Character* caster, Math::vec2 targetTile, int upcastLevel) {
    SpellResult result;
    result.success = false;

    if (!CanCast(caster, targetTile)) {
        result.failureReason = "Cannot create wall at this location";
        return result;
    }

    // Consume spell slot
    int slotLevel = (upcastLevel > 0) ? upcastLevel : GetLevel();
    if (!caster->ConsumeSpellSlot(slotLevel)) {
        result.failureReason = "No spell slots remaining";
        return result;
    }

    // Step 3.6a: Calculate wall length with upcasting
    int wallLength = baseWallLength;
    if (upcastLevel > GetLevel()) {
        wallLength += (upcastLevel - GetLevel()) * upcastBonusTiles;
    }

    // Step 3.6b: Determine wall direction
    WallDirection dir = DetermineDirection(caster->GetGridPosition(), targetTile);

    // Step 3.6c: Create wall tiles on the grid
    GridSystem& grid = GridSystem::GetInstance();
    result.affectedTiles.push_back(targetTile);
    grid.SetTileType(targetTile, TileType::Wall);

    // Step 3.6d: Extend wall in the determined direction
    Math::vec2 offset = (dir == WallDirection::Horizontal) ? Math::vec2{1, 0} : Math::vec2{0, 1};

    for (int i = 1; i < wallLength; ++i) {
        // Try both directions from center
        Math::vec2 tile1 = targetTile + (offset * i);
        Math::vec2 tile2 = targetTile - (offset * i);

        if (grid.IsValidTile(tile1) && !grid.IsOccupied(tile1)) {
            grid.SetTileType(tile1, TileType::Wall);
            result.affectedTiles.push_back(tile1);
        }

        if (i < wallLength && grid.IsValidTile(tile2) && !grid.IsOccupied(tile2)) {
            grid.SetTileType(tile2, TileType::Wall);
            result.affectedTiles.push_back(tile2);
        }
    }

    // Publish event
    EventBus::GetInstance().Publish(SpellCastEvent{
        caster,
        GetName(),
        targetTile,
        {},  // No targets for terrain spell
        0    // No damage
    });

    Engine::GetLogger().LogEvent(
        caster->TypeName() + " creates wall at (" +
        std::to_string((int)targetTile.x) + "," + std::to_string((int)targetTile.y) + ")"
    );

    result.success = true;
    return result;
}
```

#### **Task 4: LavaPool Spell Implementation**

**Why this step**: LavaPool is an area denial spell that creates persistent hazards. This introduces duration-based effects and teaches us about status effects (burning damage over time).

```cpp
// File: CS230/Game/Spells/LavaPool.h
#pragma once
#include "SpellBase.h"

// Step 4.1: LavaPool class declaration
// Reason: LavaPool creates dangerous terrain that deals damage over time
class LavaPool : public SpellBase {
public:
    LavaPool();
    ~LavaPool() override = default;

    // Spell info
    std::string GetName() const override { return "Lava Pool"; }
    int GetLevel() const override { return 3; }
    int GetRange() const override { return 4; }
    SpellTargetType GetTargetType() const override { return SpellTargetType::Area; }
    std::string GetDescription() const override {
        return "Creates a pool of lava that deals 4d8 fire damage to creatures starting their turn in the area. Lasts 3 turns. +1d8 per spell level above 3rd.";
    }

    // Spell validation
    bool CanCast(Character* caster, Math::vec2 targetTile) const override;

    // Spell execution
    SpellResult Cast(Character* caster, Math::vec2 targetTile, int upcastLevel = 0) override;

    // Area calculation
    std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const override;

private:
    const int poolRadius = 2;  // 2x2 area
    const std::string baseDamage = "4d8";  // Damage per turn
    const int baseDuration = 3;  // Lasts 3 turns
    const int upcastBonusDice = 1;  // +1d8 per level
};
```

```cpp
// File: CS230/Game/Spells/LavaPool.cpp
#include "LavaPool.h"
#include "../Character.h"
#include "../System/GridSystem.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../../Engine/Engine.h"

LavaPool::LavaPool() {}

// Step 4.2: Validate spell can be cast
bool LavaPool::CanCast(Character* caster, Math::vec2 targetTile) const {
    if (!caster) return false;

    GridSystem& grid = GridSystem::GetInstance();
    if (!grid.IsValidTile(targetTile)) return false;

    Math::vec2 casterPos = caster->GetGridPosition();
    if (!IsInRange(casterPos, targetTile, GetRange())) return false;

    if (!caster->HasSpellSlot(GetLevel())) return false;

    // Check: Can't cast on walls
    if (grid.GetTileType(targetTile) == TileType::Wall) {
        return false;
    }

    return true;
}

// Step 4.3: Calculate affected tiles (pool area)
std::vector<Math::vec2> LavaPool::GetAffectedTiles(Math::vec2 targetTile) const {
    std::vector<Math::vec2> tiles;
    GridSystem& grid = GridSystem::GetInstance();

    // 2x2 area centered on target
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            Math::vec2 checkTile = targetTile + Math::vec2{(double)dx, (double)dy};

            if (grid.IsValidTile(checkTile)) {
                tiles.push_back(checkTile);
            }
        }
    }

    return tiles;
}

// Step 4.4: Execute the spell
SpellResult LavaPool::Cast(Character* caster, Math::vec2 targetTile, int upcastLevel) {
    SpellResult result;
    result.success = false;

    if (!CanCast(caster, targetTile)) {
        result.failureReason = "Cannot create lava pool at this location";
        return result;
    }

    // Consume spell slot
    int slotLevel = (upcastLevel > 0) ? upcastLevel : GetLevel();
    if (!caster->ConsumeSpellSlot(slotLevel)) {
        result.failureReason = "No spell slots remaining";
        return result;
    }

    // Step 4.4a: Calculate damage (with upcasting)
    DiceManager& dice = DiceManager::GetInstance();
    int numDice = 4; // Base 4d8

    if (upcastLevel > GetLevel()) {
        numDice += (upcastLevel - GetLevel()) * upcastBonusDice;
    }

    std::string damageRoll = std::to_string(numDice) + "d8";

    // Step 4.4b: Get all affected tiles
    result.affectedTiles = GetAffectedTiles(targetTile);

    // Step 4.4c: Mark tiles as lava (hazardous terrain)
    GridSystem& grid = GridSystem::GetInstance();
    for (const Math::vec2& tile : result.affectedTiles) {
        grid.SetTileType(tile, TileType::Lava);

        // TODO Week 4: Store lava pool data (damage, duration, upcast level)
        // For now, just mark the terrain type
    }

    // Step 4.4d: Deal immediate damage to characters in area
    for (const Math::vec2& tile : result.affectedTiles) {
        Character* target = grid.GetCharacterAt(tile);
        if (target && target != caster) {
            int damage = dice.RollDiceFromString(damageRoll);
            target->TakeDamage(damage, caster);
            result.affectedTargets.push_back(target);
            result.damageDealt += damage;
        }
    }

    // Publish event
    EventBus::GetInstance().Publish(SpellCastEvent{
        caster,
        GetName(),
        targetTile,
        result.affectedTargets,
        result.damageDealt
    });

    Engine::GetLogger().LogEvent(
        caster->TypeName() + " creates lava pool at (" +
        std::to_string((int)targetTile.x) + "," + std::to_string((int)targetTile.y) +
        ") dealing " + std::to_string(result.damageDealt) + " immediate damage"
    );

    result.success = true;
    return result;
}
```

### Rigorous Testing (Dragon Spells)

#### **Test Suite 1: Fireball Tests**

```cpp
// File: CS230/Game/Test/FireballTests.cpp
#include "../Spells/Fireball.h"
#include "../Character.h"
#include "../System/GridSystem.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "Week1TestMocks.h"

bool TestFireballBasicCast() {
    // Setup
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(2, 3); // 3 level 2 slots

    Character* target = new Character(Math::vec2{3, 3});
    grid.PlaceCharacter(target, Math::vec2{3, 3});

    Fireball fireball;

    // Test: Cast fireball at target location
    SpellResult result = fireball.Cast(caster, Math::vec2{3, 3});

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.affectedTargets.size() > 0);
    ASSERT_TRUE(result.damageDealt > 0);
    ASSERT_TRUE(target->GetCurrentHP() < target->GetMaxHP());

    // Cleanup
    delete caster;
    delete target;
    grid.Clear();

    return true;
}

bool TestFireballUpcasting() {
    // Setup
    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(3, 2); // 2 level 3 slots

    DiceManager& dice = DiceManager::GetInstance();
    dice.SetSeed(12345); // Fixed seed for reproducibility

    Fireball fireball;

    // Test: Upcast to level 3 (should be 9d6 instead of 8d6)
    std::vector<int> damageResults;
    for (int i = 0; i < 10; ++i) {
        SpellResult result = fireball.Cast(caster, Math::vec2{3, 3}, 3);
        damageResults.push_back(result.damageDealt);
    }

    // Upcasted damage should be higher on average
    // 9d6 avg = 31.5, 8d6 avg = 28
    int avgDamage = 0;
    for (int dmg : damageResults) {
        avgDamage += dmg;
    }
    avgDamage /= damageResults.size();

    ASSERT_TRUE(avgDamage > 25); // Should be around 31.5

    delete caster;
    return true;
}

bool TestFireballAreaDamage() {
    // Setup
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(2, 3);

    // Place 4 targets in a cluster
    Character* target1 = new Character(Math::vec2{4, 4});
    Character* target2 = new Character(Math::vec2{4, 5});
    Character* target3 = new Character(Math::vec2{5, 4});
    Character* target4 = new Character(Math::vec2{5, 5});

    grid.PlaceCharacter(target1, Math::vec2{4, 4});
    grid.PlaceCharacter(target2, Math::vec2{4, 5});
    grid.PlaceCharacter(target3, Math::vec2{5, 4});
    grid.PlaceCharacter(target4, Math::vec2{5, 5});

    Fireball fireball;

    // Test: Cast at center of cluster
    SpellResult result = fireball.Cast(caster, Math::vec2{4, 4});

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.affectedTargets.size() == 4);

    // All targets should be damaged
    ASSERT_TRUE(target1->GetCurrentHP() < target1->GetMaxHP());
    ASSERT_TRUE(target2->GetCurrentHP() < target2->GetMaxHP());
    ASSERT_TRUE(target3->GetCurrentHP() < target3->GetMaxHP());
    ASSERT_TRUE(target4->GetCurrentHP() < target4->GetMaxHP());

    // Cleanup
    delete caster;
    delete target1; delete target2; delete target3; delete target4;
    grid.Clear();

    return true;
}

bool TestFireballOutOfRange() {
    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(2, 3);

    Fireball fireball;

    // Test: Try to cast beyond range (range is 6)
    bool canCast = fireball.CanCast(caster, Math::vec2{7, 7});
    ASSERT_FALSE(canCast);

    delete caster;
    return true;
}

bool TestFireballNoSpellSlots() {
    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(2, 0); // No level 2 slots

    Fireball fireball;

    // Test: Try to cast without slots
    SpellResult result = fireball.Cast(caster, Math::vec2{3, 3});
    ASSERT_FALSE(result.success);
    ASSERT_TRUE(result.failureReason.find("slots") != std::string::npos);

    delete caster;
    return true;
}

void RunFireballTests() {
    std::cout << "\n=== Fireball Tests ===\n";
    std::cout << (TestFireballBasicCast() ? "✅" : "❌") << " Basic Cast\n";
    std::cout << (TestFireballUpcasting() ? "✅" : "❌") << " Upcasting\n";
    std::cout << (TestFireballAreaDamage() ? "✅" : "❌") << " Area Damage\n";
    std::cout << (TestFireballOutOfRange() ? "✅" : "❌") << " Out of Range\n";
    std::cout << (TestFireballNoSpellSlots() ? "✅" : "❌") << " No Spell Slots\n";
}
```

#### **Test Suite 2: CreateWall Tests**

```cpp
// File: CS230/Game/Test/CreateWallTests.cpp
#include "../Spells/CreateWall.h"
#include "../Character.h"
#include "../System/GridSystem.h"

bool TestCreateWallBasic() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(1, 3);

    CreateWall spell;

    // Test: Create wall
    SpellResult result = spell.Cast(caster, Math::vec2{3, 3});

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.affectedTiles.size() > 0);

    // Check that tiles are marked as walls
    for (const Math::vec2& tile : result.affectedTiles) {
        ASSERT_TRUE(grid.GetTileType(tile) == TileType::Wall);
    }

    delete caster;
    grid.Clear();
    return true;
}

bool TestCreateWallBlocksMovement() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(1, 3);

    CreateWall spell;

    // Create wall at (3, 3)
    spell.Cast(caster, Math::vec2{3, 3});

    // Test: Tile should not be walkable
    ASSERT_FALSE(grid.IsWalkable(Math::vec2{3, 3}));

    delete caster;
    grid.Clear();
    return true;
}

bool TestCreateWallCantPlaceOnOccupiedTile() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(1, 3);

    Character* blocker = new Character(Math::vec2{3, 3});
    grid.PlaceCharacter(blocker, Math::vec2{3, 3});

    CreateWall spell;

    // Test: Can't create wall on occupied tile
    bool canCast = spell.CanCast(caster, Math::vec2{3, 3});
    ASSERT_FALSE(canCast);

    delete caster;
    delete blocker;
    grid.Clear();
    return true;
}

void RunCreateWallTests() {
    std::cout << "\n=== CreateWall Tests ===\n";
    std::cout << (TestCreateWallBasic() ? "✅" : "❌") << " Basic Wall Creation\n";
    std::cout << (TestCreateWallBlocksMovement() ? "✅" : "❌") << " Blocks Movement\n";
    std::cout << (TestCreateWallCantPlaceOnOccupiedTile() ? "✅" : "❌") << " Can't Place on Occupied Tile\n";
}
```

#### **Test Suite 3: LavaPool Tests**

```cpp
// File: CS230/Game/Test/LavaPoolTests.cpp
#include "../Spells/LavaPool.h"
#include "../Character.h"
#include "../System/GridSystem.h"

bool TestLavaPoolBasicCast() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(3, 2);

    Character* target = new Character(Math::vec2{4, 4});
    grid.PlaceCharacter(target, Math::vec2{4, 4});

    LavaPool spell;

    // Test: Create lava pool
    SpellResult result = spell.Cast(caster, Math::vec2{4, 4});

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.affectedTiles.size() > 0);
    ASSERT_TRUE(result.affectedTargets.size() > 0);
    ASSERT_TRUE(target->GetCurrentHP() < target->GetMaxHP());

    // Check that tiles are marked as lava
    for (const Math::vec2& tile : result.affectedTiles) {
        ASSERT_TRUE(grid.GetTileType(tile) == TileType::Lava);
    }

    delete caster;
    delete target;
    grid.Clear();
    return true;
}

bool TestLavaPoolAreaEffect() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(3, 2);

    // Place multiple targets in area
    Character* target1 = new Character(Math::vec2{4, 4});
    Character* target2 = new Character(Math::vec2{4, 5});
    grid.PlaceCharacter(target1, Math::vec2{4, 4});
    grid.PlaceCharacter(target2, Math::vec2{4, 5});

    LavaPool spell;

    // Test: Both targets take damage
    SpellResult result = spell.Cast(caster, Math::vec2{4, 4});

    ASSERT_TRUE(result.affectedTargets.size() >= 2);
    ASSERT_TRUE(target1->GetCurrentHP() < target1->GetMaxHP());
    ASSERT_TRUE(target2->GetCurrentHP() < target2->GetMaxHP());

    delete caster;
    delete target1;
    delete target2;
    grid.Clear();
    return true;
}

void RunLavaPoolTests() {
    std::cout << "\n=== LavaPool Tests ===\n";
    std::cout << (TestLavaPoolBasicCast() ? "✅" : "❌") << " Basic Cast\n";
    std::cout << (TestLavaPoolAreaEffect() ? "✅" : "❌") << " Area Effect\n";
}
```

### Daily Breakdown (Developer A)

#### **Monday (6-8 hours)**

- Morning: Implement SpellBase interface (1.5 hrs)
- Mid-morning: Implement Fireball class (2.5 hrs)
- Afternoon: Write Fireball test suite (2 hrs)
- End of day: Run tests, fix bugs (1-2 hrs)
- **Deliverable**: Fireball spell working and tested

#### **Tuesday (6-8 hours)**

- Morning: Implement CreateWall class (2 hrs)
- Mid-morning: Write CreateWall tests (1.5 hrs)
- Afternoon: Implement LavaPool class (2 hrs)
- Late afternoon: Write LavaPool tests (1.5 hrs)
- End of day: Integration testing (1 hr)
- **Deliverable**: All 3 spells implemented and tested

#### **Wednesday (4-6 hours)**

- Morning: Add Dragon spells to Dragon class (1 hr)
- Mid-morning: Create spell selection UI (Week 3 simple version) (2 hrs)
- Afternoon: Integration testing with SpellSystem (Developer C) (2 hrs)
- **Deliverable**: Dragon can select and cast all 3 spells

#### **Thursday (4-6 hours)**

- Morning: Bug fixes from integration testing (2 hrs)
- Afternoon: Balance tuning (spell costs, damage, ranges) (2 hrs)
- Late afternoon: Documentation and code cleanup (1-2 hrs)
- **Deliverable**: All spells polished and ready for Friday demo

#### **Friday (2-4 hours)**

- Morning: Final testing (1 hr)
- Afternoon: Integration demo with full team (2 hrs)
- **Deliverable**: Dragon vs Fighter with all spells working

---

## Week 3: Developer B - Fighter Combat Abilities

**Goal**: Implement Fighter's melee attack and Shield Bash ability

**Foundation**:

- Fighter class from Week 2 provides basic stats and movement
- CombatSystem from Developer E handles damage application
- GridSystem from Week 1 provides adjacency checks

**Files to Create**:

```
CS230/Game/Abilities/AbilityBase.h
CS230/Game/Abilities/MeleeAttack.h
CS230/Game/Abilities/MeleeAttack.cpp
CS230/Game/Abilities/ShieldBash.h
CS230/Game/Abilities/ShieldBash.cpp
```

### Implementation Tasks (Fighter Abilities)

#### **Task 1: AbilityBase Interface**

**Why this step**: Similar to SpellBase, we need a common interface for all abilities (both spell and non-spell actions). This allows Fighter abilities to work alongside Dragon spells in a unified system.

```cpp
// File: CS230/Game/Abilities/AbilityBase.h
#pragma once
#include "../../Engine/Vec2.h"
#include <string>
#include <vector>

class Character;

// Step 1.1: Define ability result structure
// Reason: Abilities need to report what happened (damage, effects, etc.)
struct AbilityResult {
    bool success;
    std::vector<Character*> affectedTargets;
    int damageDealt;
    std::string failureReason;
};

// Step 1.2: Define ability types
// Reason: Different abilities have different resource costs
enum class AbilityType {
    BasicAttack,     // Free action (no AP cost beyond movement)
    SpecialAbility,  // Costs AP
    BonusAction      // D&D bonus action (once per turn)
};

// Step 1.3: Base ability interface
class AbilityBase {
public:
    virtual ~AbilityBase() = default;

    // Core ability information
    virtual std::string GetName() const = 0;
    virtual AbilityType GetAbilityType() const = 0;
    virtual int GetAPCost() const = 0;  // Action point cost
    virtual int GetRange() const = 0;
    virtual std::string GetDescription() const = 0;

    // Ability validation
    virtual bool CanUse(Character* user, Character* target) const = 0;

    // Ability execution
    virtual AbilityResult Use(Character* user, Character* target) = 0;

protected:
    // Helper: Check if target is adjacent
    bool IsAdjacent(Math::vec2 pos1, Math::vec2 pos2) const {
        int dx = abs((int)pos2.x - (int)pos1.x);
        int dy = abs((int)pos2.y - (int)pos1.y);
        return (dx <= 1 && dy <= 1 && (dx + dy) > 0); // Adjacent, not same tile
    }
};
```

#### **Task 2: MeleeAttack Implementation**

**Why this step**: MeleeAttack is the Fighter's basic attack. It teaches us about melee range validation and basic damage calculation using the character's attack stats.

```cpp
// File: CS230/Game/Abilities/MeleeAttack.h
#pragma once
#include "AbilityBase.h"

// Step 2.1: MeleeAttack class declaration
class MeleeAttack : public AbilityBase {
public:
    MeleeAttack();
    ~MeleeAttack() override = default;

    // Ability info
    std::string GetName() const override { return "Melee Attack"; }
    AbilityType GetAbilityType() const override { return AbilityType::BasicAttack; }
    int GetAPCost() const override { return 1; }  // 1 AP to attack
    int GetRange() const override { return 1; }   // Melee range (adjacent)
    std::string GetDescription() const override {
        return "Make a melee weapon attack against an adjacent enemy. Deals 2d6 + STR modifier damage.";
    }

    // Ability validation
    bool CanUse(Character* user, Character* target) const override;

    // Ability execution
    AbilityResult Use(Character* user, Character* target) override;
};
```

```cpp
// File: CS230/Game/Abilities/MeleeAttack.cpp
#include "MeleeAttack.h"
#include "../Character.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../../Engine/Engine.h"

MeleeAttack::MeleeAttack() {}

// Step 2.2: Validate ability can be used
// Reason: Check all prerequisites before allowing the attack
bool MeleeAttack::CanUse(Character* user, Character* target) const {
    // Check 1: User and target exist
    if (!user || !target) {
        return false;
    }

    // Check 2: Target is alive
    if (target->GetCurrentHP() <= 0) {
        return false;
    }

    // Check 3: Target is adjacent (melee range)
    Math::vec2 userPos = user->GetGridPosition();
    Math::vec2 targetPos = target->GetGridPosition();
    if (!IsAdjacent(userPos, targetPos)) {
        return false;
    }

    // Check 4: User has enough AP
    if (user->GetCurrentActionPoints() < GetAPCost()) {
        return false;
    }

    // Check 5: Can't attack self
    if (user == target) {
        return false;
    }

    return true;
}

// Step 2.3: Execute the attack
// Reason: Roll attack dice, calculate damage, apply to target
AbilityResult MeleeAttack::Use(Character* user, Character* target) {
    AbilityResult result;
    result.success = false;

    // Step 2.3a: Validate (redundant safety check)
    if (!CanUse(user, target)) {
        result.failureReason = "Cannot use Melee Attack on this target";
        return result;
    }

    // Step 2.3b: Consume action points
    // Reason: Using an ability should cost resources
    if (!user->ConsumeActionPoints(GetAPCost())) {
        result.failureReason = "Not enough action points";
        return result;
    }

    // Step 2.3c: Roll attack dice
    // Reason: Use character's attackDice stat (e.g., "2d6" for Fighter)
    DiceManager& dice = DiceManager::GetInstance();
    std::string attackRoll = user->GetAttackDice();
    int baseDamage = dice.RollDiceFromString(attackRoll);

    // Step 2.3d: Add attack modifier
    // Reason: Characters have baseAttackPower (e.g., +3 from STR)
    int totalDamage = baseDamage + user->GetBaseAttackPower();

    // Step 2.3e: Apply damage to target
    target->TakeDamage(totalDamage, user);
    result.affectedTargets.push_back(target);
    result.damageDealt = totalDamage;

    // Step 2.3f: Publish attack event
    // Reason: Combat log needs to show "Fighter attacks Dragon for 15 damage"
    EventBus::GetInstance().Publish(AbilityUsedEvent{
        user,
        target,
        GetName(),
        totalDamage
    });

    // Step 2.3g: Log the attack
    Engine::GetLogger().LogEvent(
        user->TypeName() + " attacks " + target->TypeName() +
        " for " + std::to_string(totalDamage) + " damage " +
        "(" + attackRoll + " + " + std::to_string(user->GetBaseAttackPower()) + ")"
    );

    result.success = true;
    return result;
}
```

#### **Task 3: ShieldBash Implementation**

**Why this step**: ShieldBash is a special ability that deals damage AND pushes the target back. This introduces forced movement and teaches us about multi-effect abilities.

```cpp
// File: CS230/Game/Abilities/ShieldBash.h
#pragma once
#include "AbilityBase.h"

// Step 3.1: ShieldBash class declaration
// Reason: ShieldBash is a special ability with damage + knockback
class ShieldBash : public AbilityBase {
public:
    ShieldBash();
    ~ShieldBash() override = default;

    // Ability info
    std::string GetName() const override { return "Shield Bash"; }
    AbilityType GetAbilityType() const override { return AbilityType::SpecialAbility; }
    int GetAPCost() const override { return 2; }  // Costs 2 AP (more expensive)
    int GetRange() const override { return 1; }   // Melee range
    std::string GetDescription() const override {
        return "Bash adjacent enemy with shield, dealing 1d8 + STR damage and pushing them back 1 tile.";
    }

    // Ability validation
    bool CanUse(Character* user, Character* target) const override;

    // Ability execution
    AbilityResult Use(Character* user, Character* target) override;

private:
    const std::string baseDamage = "1d8";  // Less damage than normal attack
    const int pushDistance = 1;  // Push 1 tile back

    // Helper: Calculate push direction
    Math::vec2 CalculatePushDirection(Math::vec2 userPos, Math::vec2 targetPos) const;
};
```

```cpp
// File: CS230/Game/Abilities/ShieldBash.cpp
#include "ShieldBash.h"
#include "../Character.h"
#include "../System/GridSystem.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../../Engine/Engine.h"

ShieldBash::ShieldBash() {}

// Step 3.2: Calculate push direction
// Reason: Target gets pushed away from the user
Math::vec2 ShieldBash::CalculatePushDirection(Math::vec2 userPos, Math::vec2 targetPos) const {
    // Calculate direction vector
    Math::vec2 direction = targetPos - userPos;

    // Normalize to unit direction (1, 0, -1 for each component)
    if (direction.x > 0) direction.x = 1;
    else if (direction.x < 0) direction.x = -1;

    if (direction.y > 0) direction.y = 1;
    else if (direction.y < 0) direction.y = -1;

    return direction;
}

// Step 3.3: Validate ability can be used
bool ShieldBash::CanUse(Character* user, Character* target) const {
    if (!user || !target) return false;
    if (target->GetCurrentHP() <= 0) return false;
    if (user == target) return false;

    // Check melee range
    Math::vec2 userPos = user->GetGridPosition();
    Math::vec2 targetPos = target->GetGridPosition();
    if (!IsAdjacent(userPos, targetPos)) return false;

    // Check AP cost
    if (user->GetCurrentActionPoints() < GetAPCost()) return false;

    // Check if there's space to push
    // Reason: Can't use if target has nowhere to go
    Math::vec2 pushDir = CalculatePushDirection(userPos, targetPos);
    Math::vec2 pushDestination = targetPos + pushDir;

    GridSystem& grid = GridSystem::GetInstance();
    if (!grid.IsValidTile(pushDestination) || !grid.IsWalkable(pushDestination)) {
        return false;  // Can't push into wall or off grid
    }

    return true;
}

// Step 3.4: Execute Shield Bash
AbilityResult ShieldBash::Use(Character* user, Character* target) {
    AbilityResult result;
    result.success = false;

    if (!CanUse(user, target)) {
        result.failureReason = "Cannot use Shield Bash on this target";
        return result;
    }

    // Consume AP
    if (!user->ConsumeActionPoints(GetAPCost())) {
        result.failureReason = "Not enough action points";
        return result;
    }

    // Step 3.4a: Calculate damage
    DiceManager& dice = DiceManager::GetInstance();
    int damage = dice.RollDiceFromString(baseDamage) + user->GetBaseAttackPower();

    // Step 3.4b: Apply damage
    target->TakeDamage(damage, user);
    result.affectedTargets.push_back(target);
    result.damageDealt = damage;

    // Step 3.4c: Push target back
    // Reason: Shield Bash's unique mechanic
    Math::vec2 userPos = user->GetGridPosition();
    Math::vec2 targetPos = target->GetGridPosition();
    Math::vec2 pushDir = CalculatePushDirection(userPos, targetPos);
    Math::vec2 newPos = targetPos + pushDir;

    GridSystem& grid = GridSystem::GetInstance();

    // Move target to new position
    grid.MoveCharacter(target, newPos);
    target->SetGridPosition(newPos);

    // Step 3.4d: Publish events
    EventBus::GetInstance().Publish(AbilityUsedEvent{
        user,
        target,
        GetName(),
        damage
    });

    EventBus::GetInstance().Publish(CharacterMovedEvent{
        target,
        targetPos,
        newPos,
        true  // forcedMovement flag
    });

    // Step 3.4e: Log the ability use
    Engine::GetLogger().LogEvent(
        user->TypeName() + " uses Shield Bash on " + target->TypeName() +
        " for " + std::to_string(damage) + " damage and pushes them to (" +
        std::to_string((int)newPos.x) + "," + std::to_string((int)newPos.y) + ")"
    );

    result.success = true;
    return result;
}
```

### Rigorous Testing (Fighter Abilities)

#### **Test Suite 1: MeleeAttack Tests**

```cpp
// File: CS230/Game/Test/MeleeAttackTests.cpp
#include "../Abilities/MeleeAttack.h"
#include "../Character.h"
#include "../System/GridSystem.h"
#include "../Singletons/DiceManager.h"

bool TestMeleeAttackBasic() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    // Setup: Fighter adjacent to Dragon
    Character* fighter = new Character(Math::vec2{3, 3});
    fighter->SetActionPoints(5);
    fighter->SetAttackDice("2d6");
    fighter->SetBaseAttackPower(3);

    Character* dragon = new Character(Math::vec2{4, 3});
    dragon->SetMaxHP(250);
    dragon->SetCurrentHP(250);

    grid.PlaceCharacter(fighter, Math::vec2{3, 3});
    grid.PlaceCharacter(dragon, Math::vec2{4, 3});

    MeleeAttack attack;

    // Test: Fighter attacks Dragon
    AbilityResult result = attack.Use(fighter, dragon);

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.damageDealt > 0);
    ASSERT_TRUE(dragon->GetCurrentHP() < 250);
    ASSERT_TRUE(fighter->GetCurrentActionPoints() == 4); // Used 1 AP

    delete fighter;
    delete dragon;
    grid.Clear();
    return true;
}

bool TestMeleeAttackOutOfRange() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    // Setup: Fighter NOT adjacent to Dragon
    Character* fighter = new Character(Math::vec2{0, 0});
    fighter->SetActionPoints(5);

    Character* dragon = new Character(Math::vec2{5, 5});

    grid.PlaceCharacter(fighter, Math::vec2{0, 0});
    grid.PlaceCharacter(dragon, Math::vec2{5, 5});

    MeleeAttack attack;

    // Test: Can't attack from range
    bool canUse = attack.CanUse(fighter, dragon);
    ASSERT_FALSE(canUse);

    delete fighter;
    delete dragon;
    grid.Clear();
    return true;
}

bool TestMeleeAttackNoAP() {
    Character* fighter = new Character(Math::vec2{0, 0});
    fighter->SetActionPoints(0); // No AP

    Character* dragon = new Character(Math::vec2{1, 0});

    MeleeAttack attack;

    // Test: Can't attack without AP
    bool canUse = attack.CanUse(fighter, dragon);
    ASSERT_FALSE(canUse);

    delete fighter;
    delete dragon;
    return true;
}

void RunMeleeAttackTests() {
    std::cout << "\n=== MeleeAttack Tests ===\n";
    std::cout << (TestMeleeAttackBasic() ? "✅" : "❌") << " Basic Attack\n";
    std::cout << (TestMeleeAttackOutOfRange() ? "✅" : "❌") << " Out of Range\n";
    std::cout << (TestMeleeAttackNoAP() ? "✅" : "❌") << " No Action Points\n";
}
```

#### **Test Suite 2: ShieldBash Tests**

```cpp
// File: CS230/Game/Test/ShieldBashTests.cpp
#include "../Abilities/ShieldBash.h"
#include "../Character.h"
#include "../System/GridSystem.h"

bool TestShieldBashBasic() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* fighter = new Character(Math::vec2{3, 3});
    fighter->SetActionPoints(5);
    fighter->SetAttackDice("2d6");
    fighter->SetBaseAttackPower(3);

    Character* dragon = new Character(Math::vec2{4, 3});
    dragon->SetMaxHP(250);
    dragon->SetCurrentHP(250);

    grid.PlaceCharacter(fighter, Math::vec2{3, 3});
    grid.PlaceCharacter(dragon, Math::vec2{4, 3});

    ShieldBash bash;

    // Test: Shield Bash pushes target
    Math::vec2 originalPos = dragon->GetGridPosition();
    AbilityResult result = bash.Use(fighter, dragon);

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.damageDealt > 0);
    ASSERT_TRUE(dragon->GetCurrentHP() < 250);
    ASSERT_TRUE(dragon->GetGridPosition() != originalPos); // Moved
    ASSERT_TRUE(fighter->GetCurrentActionPoints() == 3); // Used 2 AP

    delete fighter;
    delete dragon;
    grid.Clear();
    return true;
}

bool TestShieldBashCantPushIntoWall() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    Character* fighter = new Character(Math::vec2{3, 3});
    fighter->SetActionPoints(5);

    Character* dragon = new Character(Math::vec2{4, 3});

    // Place wall behind dragon
    grid.SetTileType(Math::vec2{5, 3}, TileType::Wall);

    grid.PlaceCharacter(fighter, Math::vec2{3, 3});
    grid.PlaceCharacter(dragon, Math::vec2{4, 3});

    ShieldBash bash;

    // Test: Can't use Shield Bash if no room to push
    bool canUse = bash.CanUse(fighter, dragon);
    ASSERT_FALSE(canUse);

    delete fighter;
    delete dragon;
    grid.Clear();
    return true;
}

bool TestShieldBashCosts2AP() {
    Character* fighter = new Character(Math::vec2{0, 0});
    fighter->SetActionPoints(1); // Only 1 AP (needs 2)

    Character* dragon = new Character(Math::vec2{1, 0});

    ShieldBash bash;

    // Test: Can't use with insufficient AP
    bool canUse = bash.CanUse(fighter, dragon);
    ASSERT_FALSE(canUse);

    delete fighter;
    delete dragon;
    return true;
}

void RunShieldBashTests() {
    std::cout << "\n=== ShieldBash Tests ===\n";
    std::cout << (TestShieldBashBasic() ? "✅" : "❌") << " Basic Shield Bash\n";
    std::cout << (TestShieldBashCantPushIntoWall() ? "✅" : "❌") << " Can't Push Into Wall\n";
    std::cout << (TestShieldBashCosts2AP() ? "✅" : "❌") << " Costs 2 AP\n";
}
```

### Daily Breakdown (Developer B)

#### **Monday (6-8 hours)**

- Morning: Implement AbilityBase interface (1.5 hrs)
- Mid-morning: Implement MeleeAttack class (2 hrs)
- Afternoon: Write MeleeAttack test suite (2 hrs)
- End of day: Run tests, fix bugs (1.5 hrs)
- **Deliverable**: MeleeAttack working and tested

#### **Tuesday (6-8 hours)**

- Morning: Implement ShieldBash class (2.5 hrs)
- Mid-morning: Write ShieldBash tests (2 hrs)
- Afternoon: Integration testing with Fighter class (2 hrs)
- End of day: Bug fixes (1 hr)
- **Deliverable**: Both Fighter abilities working

#### **Wednesday (4-6 hours)**

- Morning: Add abilities to Fighter class interface (1 hr)
- Mid-morning: Create ability selection UI (2 hrs)
- Afternoon: Integration testing with CombatSystem (Developer E) (2 hrs)
- **Deliverable**: Fighter can use both abilities in combat

#### **Thursday (4-6 hours)**

- Morning: Bug fixes from integration testing (2 hrs)
- Afternoon: Balance tuning (AP costs, damage) (2 hrs)
- Late afternoon: Documentation (1-2 hrs)
- **Deliverable**: Abilities polished

#### **Friday (2-4 hours)**

- Morning: Final testing (1 hr)
- Afternoon: Integration demo (2 hrs)
- **Deliverable**: Fighter vs Dragon combat demo

---

## Week 3: Developer C - SpellSystem Complete

**Goal**: Complete SpellSystem with spell slot management, validation, and upcasting

**Foundation**:

- EventBus from Week 1 for spell cast events
- Character class from Week 1 has spell slot storage
- SpellBase interface from Developer A

**Files to Create**:

```
CS230/Game/System/SpellSystem.h
CS230/Game/System/SpellSystem.cpp
CS230/Game/Types/Events.h (update with spell events)
```

### Implementation Tasks (SpellSystem)

#### **Task 1: SpellSystem Class Structure**

**Why this step**: SpellSystem is the central manager for all spell-related operations. It validates spell casts, manages spell slots, and coordinates spell effects with other systems.

```cpp
// File: CS230/Game/System/SpellSystem.h
#pragma once
#include "../Spells/SpellBase.h"
#include <map>
#include <string>
#include <vector>
#include <memory>

class Character;

// Step 1.1: SpellSystem singleton class
// Reason: Centralized spell management accessible from anywhere
class SpellSystem {
public:
    // Singleton access
    static SpellSystem& GetInstance();

    // Delete copy constructor and assignment operator
    SpellSystem(const SpellSystem&) = delete;
    SpellSystem& operator=(const SpellSystem&) = delete;

    // Step 1.2: Spell registration
    // Reason: Spells must be registered before they can be used
    void RegisterSpell(const std::string& spellName, SpellBase* spell);
    SpellBase* GetSpell(const std::string& spellName);

    // Step 1.3: Spell casting interface
    // Reason: Main entry point for casting spells
    SpellResult CastSpell(
        Character* caster,
        const std::string& spellName,
        Math::vec2 targetTile,
        int upcastLevel = 0
    );

    // Step 1.4: Spell validation
    // Reason: Check if spell can be cast before committing resources
    bool CanCastSpell(
        Character* caster,
        const std::string& spellName,
        Math::vec2 targetTile,
        int upcastLevel = 0
    ) const;

    // Step 1.5: Spell slot management helpers
    // Reason: Query available spells for UI
    std::vector<std::string> GetAvailableSpells(Character* caster) const;
    int GetSpellSlotCount(Character* caster, int level) const;

    // Step 1.6: Spell preview (for UI)
    // Reason: Show which tiles will be affected before casting
    std::vector<Math::vec2> PreviewSpellArea(
        const std::string& spellName,
        Math::vec2 targetTile
    ) const;

private:
    SpellSystem();
    ~SpellSystem();

    // Step 1.7: Spell registry
    // Reason: Store all registered spells
    std::map<std::string, std::unique_ptr<SpellBase>> spells;

    // Step 1.8: Validation helpers
    bool ValidateSpellSlots(Character* caster, int requiredLevel) const;
    bool ValidateTarget(SpellBase* spell, Character* caster, Math::vec2 targetTile) const;
};
```

```cpp
// File: CS230/Game/System/SpellSystem.cpp
#include "SpellSystem.h"
#include "../Character.h"
#include "../Singletons/EventBus.h"
#include "../../Engine/Engine.h"

// Step 1.9: Singleton implementation
SpellSystem& SpellSystem::GetInstance() {
    static SpellSystem instance;
    return instance;
}

SpellSystem::SpellSystem() {
    Engine::GetLogger().LogEvent("SpellSystem initialized");
}

SpellSystem::~SpellSystem() {
    spells.clear();
}
```

#### **Task 2: Spell Registration System**

**Why this step**: Before spells can be used, they must be registered with the SpellSystem. This allows the system to look up spells by name and manage their lifecycle.

```cpp
// Step 2.1: Register a spell
// Reason: Add spell to the system's registry
void SpellSystem::RegisterSpell(const std::string& spellName, SpellBase* spell) {
    if (!spell) {
        Engine::GetLogger().LogError("SpellSystem: Attempted to register null spell");
        return;
    }

    // Check if spell already registered
    if (spells.find(spellName) != spells.end()) {
        Engine::GetLogger().LogError("SpellSystem: Spell '" + spellName + "' already registered");
        return;
    }

    // Step 2.1a: Store spell in registry
    // Reason: unique_ptr ensures proper cleanup
    spells[spellName] = std::unique_ptr<SpellBase>(spell);

    Engine::GetLogger().LogEvent("SpellSystem: Registered spell '" + spellName + "'");
}

// Step 2.2: Retrieve a spell by name
// Reason: Allow other systems to look up spells
SpellBase* SpellSystem::GetSpell(const std::string& spellName) {
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        Engine::GetLogger().LogError("SpellSystem: Spell '" + spellName + "' not found");
        return nullptr;
    }

    return it->second.get();
}
```

#### **Task 3: Spell Validation System**

**Why this step**: Before casting a spell, we must validate that the caster meets all requirements (spell slots, range, valid target, etc.). This prevents invalid game states and provides clear feedback to the player.

```cpp
// Step 3.1: Validate spell slots
// Reason: Caster must have available spell slots of the required level
bool SpellSystem::ValidateSpellSlots(Character* caster, int requiredLevel) const {
    if (!caster) return false;

    // Check if caster has any slots of the required level or higher
    for (int level = requiredLevel; level <= 9; ++level) {
        if (caster->GetSpellSlotCount(level) > 0) {
            return true;
        }
    }

    return false;
}

// Step 3.2: Validate target
// Reason: Use spell's own validation logic
bool SpellSystem::ValidateTarget(SpellBase* spell, Character* caster, Math::vec2 targetTile) const {
    if (!spell || !caster) return false;

    return spell->CanCast(caster, targetTile);
}

// Step 3.3: Main validation method
// Reason: Check all prerequisites before allowing cast
bool SpellSystem::CanCastSpell(
    Character* caster,
    const std::string& spellName,
    Math::vec2 targetTile,
    int upcastLevel
) const {
    // Check 1: Spell exists
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        return false;
    }

    SpellBase* spell = it->second.get();

    // Check 2: Determine required spell slot level
    int requiredLevel = (upcastLevel > 0) ? upcastLevel : spell->GetLevel();

    // Check 3: Validate spell slots
    if (!ValidateSpellSlots(caster, requiredLevel)) {
        return false;
    }

    // Check 4: Validate target
    if (!ValidateTarget(spell, caster, targetTile)) {
        return false;
    }

    return true;
}
```

#### **Task 4: Spell Casting System**

**Why this step**: This is the core functionality - actually casting spells, consuming resources, and coordinating effects.

```cpp
// Step 4.1: Cast spell
// Reason: Main entry point for spell casting
SpellResult SpellSystem::CastSpell(
    Character* caster,
    const std::string& spellName,
    Math::vec2 targetTile,
    int upcastLevel
) {
    SpellResult result;
    result.success = false;

    // Step 4.1a: Validate spell exists
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        result.failureReason = "Spell '" + spellName + "' not found";
        Engine::GetLogger().LogError("SpellSystem: " + result.failureReason);
        return result;
    }

    SpellBase* spell = it->second.get();

    // Step 4.1b: Validate can cast
    if (!CanCastSpell(caster, spellName, targetTile, upcastLevel)) {
        result.failureReason = "Cannot cast " + spellName;
        Engine::GetLogger().LogError("SpellSystem: " + result.failureReason);
        return result;
    }

    // Step 4.1c: Log spell cast attempt
    Engine::GetLogger().LogEvent(
        "SpellSystem: " + caster->TypeName() + " casting " + spellName +
        " at (" + std::to_string((int)targetTile.x) + "," +
        std::to_string((int)targetTile.y) + ")" +
        (upcastLevel > 0 ? " (upcast to level " + std::to_string(upcastLevel) + ")" : "")
    );

    // Step 4.1d: Execute the spell
    // Reason: Delegate to the spell's own Cast() method
    result = spell->Cast(caster, targetTile, upcastLevel);

    // Step 4.1e: Publish system-level event
    // Reason: Other systems may need to react to spell casts
    if (result.success) {
        EventBus::GetInstance().Publish(SpellCastEvent{
            caster,
            spellName,
            targetTile,
            result.affectedTargets,
            result.damageDealt
        });
    }

    return result;
}
```

#### **Task 5: Helper Methods for UI**

**Why this step**: The UI needs to query available spells, preview areas of effect, and display spell slot counts. These helper methods provide that information.

```cpp
// Step 5.1: Get available spells for a caster
// Reason: UI needs to show which spells the character can cast
std::vector<std::string> SpellSystem::GetAvailableSpells(Character* caster) const {
    std::vector<std::string> available;

    if (!caster) return available;

    // Check each registered spell
    for (const auto& pair : spells) {
        const std::string& spellName = pair.first;
        SpellBase* spell = pair.second.get();

        // Check if caster has spell slots for this spell level
        if (ValidateSpellSlots(caster, spell->GetLevel())) {
            available.push_back(spellName);
        }
    }

    return available;
}

// Step 5.2: Get spell slot count
// Reason: UI displays remaining spell slots
int SpellSystem::GetSpellSlotCount(Character* caster, int level) const {
    if (!caster || level < 1 || level > 9) {
        return 0;
    }

    return caster->GetSpellSlotCount(level);
}

// Step 5.3: Preview spell area
// Reason: Show player which tiles will be affected before casting
std::vector<Math::vec2> SpellSystem::PreviewSpellArea(
    const std::string& spellName,
    Math::vec2 targetTile
) const {
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        return {};
    }

    SpellBase* spell = it->second.get();
    return spell->GetAffectedTiles(targetTile);
}
```

### Rigorous Testing (SpellSystem)

```cpp
// File: CS230/Game/Test/SpellSystemTests.cpp
#include "../System/SpellSystem.h"
#include "../Spells/Fireball.h"
#include "../Character.h"
#include "../System/GridSystem.h"

bool TestSpellRegistration() {
    SpellSystem& spellSys = SpellSystem::GetInstance();

    // Test: Register a spell
    Fireball* fireball = new Fireball();
    spellSys.RegisterSpell("Fireball", fireball);

    // Test: Retrieve the spell
    SpellBase* retrieved = spellSys.GetSpell("Fireball");
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_TRUE(retrieved->GetName() == "Fireball");

    return true;
}

bool TestSpellCasting() {
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    SpellSystem& spellSys = SpellSystem::GetInstance();

    // Register Fireball
    spellSys.RegisterSpell("Fireball", new Fireball());

    // Create caster with spell slots
    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(2, 3); // 3 level 2 slots

    // Create target
    Character* target = new Character(Math::vec2{3, 3});
    grid.PlaceCharacter(target, Math::vec2{3, 3});

    // Test: Cast spell through SpellSystem
    SpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{3, 3});

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.damageDealt > 0);
    ASSERT_TRUE(caster->GetSpellSlotCount(2) == 2); // Used 1 slot

    delete caster;
    delete target;
    grid.Clear();
    return true;
}

bool TestSpellUpcast() {
    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(2, 1); // 1 level 2 slot
    caster->SetSpellSlots(3, 2); // 2 level 3 slots

    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new Fireball());

    // Test: Upcast Fireball to level 3
    SpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{3, 3}, 3);

    ASSERT_TRUE(result.success);
    ASSERT_TRUE(caster->GetSpellSlotCount(2) == 1); // Didn't use level 2 slot
    ASSERT_TRUE(caster->GetSpellSlotCount(3) == 1); // Used level 3 slot

    delete caster;
    return true;
}

bool TestGetAvailableSpells() {
    Character* caster = new Character(Math::vec2{0, 0});
    caster->SetSpellSlots(1, 2);
    caster->SetSpellSlots(2, 3);

    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new Fireball());  // Level 2
    spellSys.RegisterSpell("CreateWall", new CreateWall());  // Level 1

    // Test: Get available spells
    std::vector<std::string> available = spellSys.GetAvailableSpells(caster);

    ASSERT_TRUE(available.size() == 2);
    ASSERT_TRUE(std::find(available.begin(), available.end(), "Fireball") != available.end());
    ASSERT_TRUE(std::find(available.begin(), available.end(), "CreateWall") != available.end());

    delete caster;
    return true;
}

bool TestPreviewSpellArea() {
    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new Fireball());

    // Test: Preview Fireball area
    std::vector<Math::vec2> tiles = spellSys.PreviewSpellArea("Fireball", Math::vec2{4, 4});

    ASSERT_TRUE(tiles.size() > 0);
    // Fireball has radius 2, so should affect multiple tiles
    ASSERT_TRUE(tiles.size() >= 9);  // At least 3x3 area

    return true;
}

void RunSpellSystemTests() {
    std::cout << "\n=== SpellSystem Tests ===\n";
    std::cout << (TestSpellRegistration() ? "✅" : "❌") << " Spell Registration\n";
    std::cout << (TestSpellCasting() ? "✅" : "❌") << " Spell Casting\n";
    std::cout << (TestSpellUpcast() ? "✅" : "❌") << " Spell Upcasting\n";
    std::cout << (TestGetAvailableSpells() ? "✅" : "❌") << " Get Available Spells\n";
    std::cout << (TestPreviewSpellArea() ? "✅" : "❌") << " Preview Spell Area\n";
}
```

### Daily Breakdown (Developer C)

#### **Monday (6-8 hours)**

- Morning: Design SpellSystem class structure (1 hr)
- Mid-morning: Implement spell registration system (2 hrs)
- Afternoon: Implement spell validation methods (2 hrs)
- End of day: Write registration tests (1.5 hrs)
- **Deliverable**: Spell registration working

#### **Tuesday (6-8 hours)**

- Morning: Implement CastSpell() method (2.5 hrs)
- Mid-morning: Implement upcasting logic (2 hrs)
- Afternoon: Write casting tests (2 hrs)
- **Deliverable**: Full spell casting system working

#### **Wednesday (4-6 hours)**

- Morning: Implement helper methods for UI (2 hrs)
- Afternoon: Integration testing with Developer A's spells (2-3 hrs)
- **Deliverable**: SpellSystem integrated with all Dragon spells

#### **Thursday (4-6 hours)**

- Morning: Bug fixes from integration testing (2 hrs)
- Afternoon: Performance optimization, documentation (2-3 hrs)
- **Deliverable**: SpellSystem polished and documented

#### **Friday (2-4 hours)**

- Morning: Final testing (1 hr)
- Afternoon: Integration demo (2 hrs)
- **Deliverable**: Full spell system demo

---

## Week 3: Developer D - DataRegistry Basics

**Goal**: Create data-driven character system with JSON loading for Dragon and Fighter stats

**Foundation**:

- Character base class from Week 1 has stat storage
- Need flexible system for loading data from files
- Prepares for future expansion (Week 4-20: more characters, items, spells)

**Files to Create**:

```
CS230/Game/Singletons/DataRegistry.h
CS230/Game/Singletons/DataRegistry.cpp
CS230/Assets/Data/dragon_stats.json
CS230/Assets/Data/fighter_stats.json
CS230/External/json.hpp (or use existing JSON library)
```

### Implementation Tasks (DataRegistry)

#### **Task 1: Choose JSON Library**

**Why this step**: We need a robust JSON parsing library. For C++, `nlohmann/json` is industry-standard, header-only, and easy to integrate.

```bash
# Step 1.1: Download nlohmann/json
# Reason: Single header file, no compilation needed
# Download from: https://github.com/nlohmann/json/releases
# Place in: CS230/External/json.hpp
```

```cpp
// Step 1.2: Test JSON library integration
// File: CS230/Game/Test/JsonTest.cpp
#include "../../External/json.hpp"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

bool TestJsonParsing() {
    // Create sample JSON
    json testData = {
        {"name", "Dragon"},
        {"hp", 250},
        {"speed", 3}
    };

    // Test: Parse values
    std::string name = testData["name"];
    int hp = testData["hp"];
    int speed = testData["speed"];

    ASSERT_TRUE(name == "Dragon");
    ASSERT_TRUE(hp == 250);
    ASSERT_TRUE(speed == 3);

    return true;
}
```

#### **Task 2: Design JSON Data Schema**

**Why this step**: Define the structure for character data files. Consistent schema makes loading/parsing easier and enables future expansion.

```json
// File: CS230/Assets/Data/dragon_stats.json
// Step 2.1: Dragon stats schema
// Reason: All character stats in one place, easy to tune without recompiling
{
  "character_id": "dragon",
  "display_name": "Ancient Dragon",
  "type": "playable",

  "stats": {
    "max_hp": 250,
    "max_action_points": 5,
    "speed": 3,
    "base_attack_power": 5,
    "base_defense_power": 2
  },

  "combat": {
    "attack_dice": "3d8",
    "defense_dice": "2d6",
    "attack_range": 1
  },

  "spell_slots": {
    "1": 4,
    "2": 3,
    "3": 2
  },

  "abilities": [
    "fireball",
    "create_wall",
    "lava_pool"
  ],

  "description": "An ancient dragon with powerful fire magic and devastating physical attacks."
}
```

```json
// File: CS230/Assets/Data/fighter_stats.json
// Step 2.2: Fighter stats schema
{
  "character_id": "fighter",
  "display_name": "Battle Fighter",
  "type": "enemy",

  "stats": {
    "max_hp": 90,
    "max_action_points": 5,
    "speed": 4,
    "base_attack_power": 3,
    "base_defense_power": 4
  },

  "combat": {
    "attack_dice": "2d6",
    "defense_dice": "1d10",
    "attack_range": 1
  },

  "spell_slots": {},

  "abilities": [
    "melee_attack",
    "shield_bash"
  ],

  "description": "A skilled warrior with high defense and crowd control abilities."
}
```

#### **Task 3: DataRegistry Class Implementation**

**Why this step**: DataRegistry is the central manager for all game data. It loads JSON files, caches data, and provides query methods.

```cpp
// File: CS230/Game/Singletons/DataRegistry.h
#pragma once
#include "../../External/json.hpp"
#include <string>
#include <map>
#include <vector>

using json = nlohmann::json;

// Step 3.1: Character data structure
// Reason: Parsed JSON data stored in C++ structs for fast access
struct CharacterData {
    std::string characterId;
    std::string displayName;
    std::string type;  // "playable" or "enemy"

    // Stats
    int maxHP;
    int maxActionPoints;
    int speed;
    int baseAttackPower;
    int baseDefensePower;

    // Combat
    std::string attackDice;
    std::string defenseDice;
    int attackRange;

    // Spell slots (level -> count)
    std::map<int, int> spellSlots;

    // Abilities
    std::vector<std::string> abilities;

    // Description
    std::string description;
};

// Step 3.2: DataRegistry singleton class
// Reason: Centralized data management accessible from anywhere
class DataRegistry {
public:
    // Singleton access
    static DataRegistry& GetInstance();

    // Delete copy constructor and assignment operator
    DataRegistry(const DataRegistry&) = delete;
    DataRegistry& operator=(const DataRegistry&) = delete;

    // Step 3.3: Data loading methods
    // Reason: Load JSON files from disk
    bool LoadCharacterData(const std::string& filePath);
    bool LoadAllCharacterData();  // Load all JSON files in Assets/Data/

    // Step 3.4: Data query methods
    // Reason: Retrieve loaded data by ID
    CharacterData* GetCharacterData(const std::string& characterId);
    bool HasCharacterData(const std::string& characterId) const;

    // Step 3.5: Helper methods
    // Reason: Query available characters for UI
    std::vector<std::string> GetAllCharacterIds() const;
    std::vector<CharacterData*> GetCharactersByType(const std::string& type);

    // Step 3.6: Data reloading (hot-reload for development)
    // Reason: Change JSON files without restarting game
    void ReloadAllData();

private:
    DataRegistry();
    ~DataRegistry();

    // Step 3.7: Data storage
    // Reason: Cache loaded data in memory
    std::map<std::string, CharacterData> characterDataMap;

    // Step 3.8: JSON parsing helpers
    // Reason: Convert JSON to C++ structs
    CharacterData ParseCharacterJson(const json& j);
    bool ValidateCharacterJson(const json& j);
};
```

```cpp
// File: CS230/Game/Singletons/DataRegistry.cpp
#include "DataRegistry.h"
#include "../../Engine/Engine.h"
#include <fstream>
#include <filesystem>

// Step 3.9: Singleton implementation
DataRegistry& DataRegistry::GetInstance() {
    static DataRegistry instance;
    return instance;
}

DataRegistry::DataRegistry() {
    Engine::GetLogger().LogEvent("DataRegistry initialized");
}

DataRegistry::~DataRegistry() {
    characterDataMap.clear();
}
```

#### **Task 4: JSON Loading Implementation**

**Why this step**: Implement file loading and JSON parsing logic. This is the core functionality that reads data files.

```cpp
// Step 4.1: Load single character data file
// Reason: Read JSON file, parse it, store in memory
bool DataRegistry::LoadCharacterData(const std::string& filePath) {
    // Step 4.1a: Open file
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Engine::GetLogger().LogError("DataRegistry: Failed to open file: " + filePath);
        return false;
    }

    // Step 4.1b: Parse JSON
    // Reason: nlohmann::json handles parsing automatically
    json j;
    try {
        file >> j;
    } catch (const json::exception& e) {
        Engine::GetLogger().LogError("DataRegistry: JSON parse error in " + filePath + ": " + e.what());
        return false;
    }

    // Step 4.1c: Validate JSON structure
    // Reason: Ensure required fields exist before parsing
    if (!ValidateCharacterJson(j)) {
        Engine::GetLogger().LogError("DataRegistry: Invalid JSON structure in " + filePath);
        return false;
    }

    // Step 4.1d: Parse JSON to CharacterData struct
    CharacterData data = ParseCharacterJson(j);

    // Step 4.1e: Store in map
    // Reason: Use character_id as key for fast lookup
    characterDataMap[data.characterId] = data;

    Engine::GetLogger().LogEvent("DataRegistry: Loaded character data for '" + data.characterId + "'");
    return true;
}

// Step 4.2: Load all character data files
// Reason: Scan Assets/Data/ directory and load all .json files
bool DataRegistry::LoadAllCharacterData() {
    std::string dataPath = "Assets/Data/";

    // Step 4.2a: Check if directory exists
    if (!std::filesystem::exists(dataPath)) {
        Engine::GetLogger().LogError("DataRegistry: Data directory not found: " + dataPath);
        return false;
    }

    // Step 4.2b: Iterate through all files
    int loadedCount = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dataPath)) {
        if (entry.path().extension() == ".json") {
            if (LoadCharacterData(entry.path().string())) {
                loadedCount++;
            }
        }
    }

    Engine::GetLogger().LogEvent("DataRegistry: Loaded " + std::to_string(loadedCount) + " character data files");
    return loadedCount > 0;
}

// Step 4.3: Validate JSON structure
// Reason: Check that all required fields exist
bool DataRegistry::ValidateCharacterJson(const json& j) {
    // Check required top-level fields
    if (!j.contains("character_id") || !j.contains("stats") || !j.contains("combat")) {
        return false;
    }

    // Check required stats fields
    const json& stats = j["stats"];
    if (!stats.contains("max_hp") || !stats.contains("speed")) {
        return false;
    }

    // Check required combat fields
    const json& combat = j["combat"];
    if (!combat.contains("attack_dice")) {
        return false;
    }

    return true;
}

// Step 4.4: Parse JSON to CharacterData struct
// Reason: Convert JSON fields to C++ struct
CharacterData DataRegistry::ParseCharacterJson(const json& j) {
    CharacterData data;

    // Step 4.4a: Parse basic info
    data.characterId = j["character_id"];
    data.displayName = j.value("display_name", data.characterId);
    data.type = j.value("type", "enemy");
    data.description = j.value("description", "");

    // Step 4.4b: Parse stats
    const json& stats = j["stats"];
    data.maxHP = stats["max_hp"];
    data.maxActionPoints = stats.value("max_action_points", 5);
    data.speed = stats["speed"];
    data.baseAttackPower = stats.value("base_attack_power", 0);
    data.baseDefensePower = stats.value("base_defense_power", 0);

    // Step 4.4c: Parse combat stats
    const json& combat = j["combat"];
    data.attackDice = combat["attack_dice"];
    data.defenseDice = combat.value("defense_dice", "1d6");
    data.attackRange = combat.value("attack_range", 1);

    // Step 4.4d: Parse spell slots
    // Reason: Spell slots are optional (Fighter has none)
    if (j.contains("spell_slots") && j["spell_slots"].is_object()) {
        for (auto& [key, value] : j["spell_slots"].items()) {
            int level = std::stoi(key);
            int count = value;
            data.spellSlots[level] = count;
        }
    }

    // Step 4.4e: Parse abilities
    // Reason: List of ability IDs this character can use
    if (j.contains("abilities") && j["abilities"].is_array()) {
        for (const auto& ability : j["abilities"]) {
            data.abilities.push_back(ability);
        }
    }

    return data;
}
```

#### **Task 5: Data Query Methods**

**Why this step**: Provide methods for other systems to retrieve loaded data.

```cpp
// Step 5.1: Get character data by ID
// Reason: Main query method - returns pointer to stored data
CharacterData* DataRegistry::GetCharacterData(const std::string& characterId) {
    auto it = characterDataMap.find(characterId);
    if (it == characterDataMap.end()) {
        Engine::GetLogger().LogError("DataRegistry: Character data not found: " + characterId);
        return nullptr;
    }

    return &(it->second);
}

// Step 5.2: Check if character data exists
bool DataRegistry::HasCharacterData(const std::string& characterId) const {
    return characterDataMap.find(characterId) != characterDataMap.end();
}

// Step 5.3: Get all character IDs
// Reason: UI needs list of available characters
std::vector<std::string> DataRegistry::GetAllCharacterIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : characterDataMap) {
        ids.push_back(pair.first);
    }
    return ids;
}

// Step 5.4: Get characters by type
// Reason: Filter playable vs enemy characters
std::vector<CharacterData*> DataRegistry::GetCharactersByType(const std::string& type) {
    std::vector<CharacterData*> results;
    for (auto& pair : characterDataMap) {
        if (pair.second.type == type) {
            results.push_back(&pair.second);
        }
    }
    return results;
}

// Step 5.5: Reload all data
// Reason: Hot-reload during development - change JSON without restart
void DataRegistry::ReloadAllData() {
    Engine::GetLogger().LogEvent("DataRegistry: Reloading all data");
    characterDataMap.clear();
    LoadAllCharacterData();
}
```

#### **Task 6: Integrate with Character Class**

**Why this step**: Character class needs to load stats from DataRegistry instead of hardcoding them.

```cpp
// File: CS230/Game/Character.h
// Add new factory method
class Character : public CS230::GameObject {
public:
    // Step 6.1: Add factory method
    // Reason: Create character from data file
    static Character* CreateFromData(const std::string& characterId, Math::vec2 position);

    // Existing constructors still work for manual creation
    Character(Math::vec2 position);

    // ...existing methods...
};
```

```cpp
// File: CS230/Game/Character.cpp
#include "Singletons/DataRegistry.h"

// Step 6.2: Implement factory method
// Reason: Load stats from DataRegistry and create character
Character* Character::CreateFromData(const std::string& characterId, Math::vec2 position) {
    // Step 6.2a: Get data from registry
    DataRegistry& registry = DataRegistry::GetInstance();
    CharacterData* data = registry.GetCharacterData(characterId);

    if (!data) {
        Engine::GetLogger().LogError("Character: Cannot create from data, ID not found: " + characterId);
        return nullptr;
    }

    // Step 6.2b: Create character instance
    Character* character = new Character(position);

    // Step 6.2c: Apply stats from data
    character->maxHP = data->maxHP;
    character->currentHP = data->maxHP;
    character->maxActionPoints = data->maxActionPoints;
    character->currentActionPoints = data->maxActionPoints;
    character->speed = data->speed;
    character->baseAttackPower = data->baseAttackPower;
    character->baseDefensePower = data->baseDefensePower;
    character->attackDice = data->attackDice;
    character->defenseDice = data->defenseDice;
    character->attackRange = data->attackRange;

    // Step 6.2d: Apply spell slots
    for (const auto& [level, count] : data->spellSlots) {
        character->SetSpellSlots(level, count);
    }

    // Step 6.2e: Store character ID for reference
    character->characterDataId = characterId;

    Engine::GetLogger().LogEvent("Character: Created '" + data->displayName + "' from data");
    return character;
}
```

### Rigorous Testing (DataRegistry)

```cpp
// File: CS230/Game/Test/DataRegistryTests.cpp
#include "../Singletons/DataRegistry.h"
#include <fstream>

bool TestLoadCharacterData() {
    DataRegistry& registry = DataRegistry::GetInstance();

    // Test: Load dragon_stats.json
    bool loaded = registry.LoadCharacterData("Assets/Data/dragon_stats.json");
    ASSERT_TRUE(loaded);

    // Test: Data is accessible
    ASSERT_TRUE(registry.HasCharacterData("dragon"));

    return true;
}

bool TestGetCharacterData() {
    DataRegistry& registry = DataRegistry::GetInstance();
    registry.LoadCharacterData("Assets/Data/dragon_stats.json");

    // Test: Retrieve data
    CharacterData* data = registry.GetCharacterData("dragon");
    ASSERT_TRUE(data != nullptr);

    // Test: Verify stats
    ASSERT_TRUE(data->maxHP == 250);
    ASSERT_TRUE(data->speed == 3);
    ASSERT_TRUE(data->attackDice == "3d8");
    ASSERT_TRUE(data->spellSlots[1] == 4);
    ASSERT_TRUE(data->spellSlots[2] == 3);
    ASSERT_TRUE(data->spellSlots[3] == 2);

    return true;
}

bool TestLoadAllCharacterData() {
    DataRegistry& registry = DataRegistry::GetInstance();

    // Test: Load all files
    bool loaded = registry.LoadAllCharacterData();
    ASSERT_TRUE(loaded);

    // Test: Both dragon and fighter loaded
    ASSERT_TRUE(registry.HasCharacterData("dragon"));
    ASSERT_TRUE(registry.HasCharacterData("fighter"));

    return true;
}

bool TestCreateCharacterFromData() {
    DataRegistry& registry = DataRegistry::GetInstance();
    registry.LoadCharacterData("Assets/Data/dragon_stats.json");

    // Test: Create character from data
    Character* dragon = Character::CreateFromData("dragon", Math::vec2{0, 0});
    ASSERT_TRUE(dragon != nullptr);

    // Test: Stats match data file
    ASSERT_TRUE(dragon->GetMaxHP() == 250);
    ASSERT_TRUE(dragon->GetSpeed() == 3);
    ASSERT_TRUE(dragon->GetAttackDice() == "3d8");
    ASSERT_TRUE(dragon->GetSpellSlotCount(2) == 3);

    delete dragon;
    return true;
}

bool TestInvalidJsonHandling() {
    DataRegistry& registry = DataRegistry::GetInstance();

    // Create invalid JSON file
    std::ofstream invalidFile("Assets/Data/invalid.json");
    invalidFile << "{ invalid json content }";
    invalidFile.close();

    // Test: Should fail gracefully
    bool loaded = registry.LoadCharacterData("Assets/Data/invalid.json");
    ASSERT_FALSE(loaded);

    // Cleanup
    std::filesystem::remove("Assets/Data/invalid.json");
    return true;
}

void RunDataRegistryTests() {
    std::cout << "\n=== DataRegistry Tests ===\n";
    std::cout << (TestLoadCharacterData() ? "✅" : "❌") << " Load Character Data\n";
    std::cout << (TestGetCharacterData() ? "✅" : "❌") << " Get Character Data\n";
    std::cout << (TestLoadAllCharacterData() ? "✅" : "❌") << " Load All Character Data\n";
    std::cout << (TestCreateCharacterFromData() ? "✅" : "❌") << " Create Character From Data\n";
    std::cout << (TestInvalidJsonHandling() ? "✅" : "❌") << " Invalid JSON Handling\n";
}
```

### Daily Breakdown (Developer D)

#### **Monday (6-8 hours)**

- Morning: Integrate nlohmann/json library (1 hr)
- Mid-morning: Design JSON schemas for Dragon and Fighter (2 hrs)
- Afternoon: Implement DataRegistry class structure (2 hrs)
- End of day: Write JSON validation tests (1.5 hrs)
- **Deliverable**: DataRegistry structure complete

#### **Tuesday (6-8 hours)**

- Morning: Implement JSON loading methods (2.5 hrs)
- Mid-morning: Implement data parsing (ParseCharacterJson) (2 hrs)
- Afternoon: Write data loading tests (2 hrs)
- End of day: Test with dragon_stats.json and fighter_stats.json (1 hr)
- **Deliverable**: JSON loading working

#### **Wednesday (4-6 hours)**

- Morning: Implement Character factory method (CreateFromData) (2 hrs)
- Mid-morning: Integration testing with Dragon and Fighter classes (2 hrs)
- Afternoon: Write integration tests (1-2 hrs)
- **Deliverable**: Characters can be created from JSON files

#### **Thursday (4-6 hours)**

- Morning: Bug fixes from integration testing (2 hrs)
- Afternoon: Implement hot-reload functionality (2 hrs)
- Late afternoon: Documentation (1-2 hrs)
- **Deliverable**: Data system polished and documented

#### **Friday (2-4 hours)**

- Morning: Final testing (1 hr)
- Afternoon: Integration demo (2 hrs)
- **Deliverable**: Dragon and Fighter loaded from JSON in demo

---

## Week 3: Developer E - BattleState Integration

**Goal**: Create complete battle state that integrates all Week 1-3 systems into playable Dragon vs Fighter combat

**Foundation**:

- All systems from Week 1 (EventBus, DiceManager, GridSystem, Character, DebugConsole)
- Week 2 systems (Dragon, Fighter, TurnManager, CombatSystem, GridSystem pathfinding)
- Week 3 systems (SpellSystem, all spells, all abilities, DataRegistry)

**Files to Create**:

```
CS230/Game/States/BattleState.h
CS230/Game/States/BattleState.cpp
```

### Implementation Tasks (BattleState)

#### **Task 1: BattleState Class Structure**

**Why this step**: BattleState is the main game state for combat. It owns all game objects, manages turn flow, and coordinates all systems.

```cpp
// File: CS230/Game/States/BattleState.h
#pragma once
#include "../../Engine/GameState.h"
#include "../Character.h"
#include "../Dragon.h"
#include "../Fighter.h"
#include <vector>

class BattleState : public CS230::GameState {
public:
    BattleState();
    ~BattleState() override;

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
```

#### **Task 2: BattleState Initialization**

**Why this step**: Load all systems, create characters, and prepare the battle.

```cpp
// File: CS230/Game/States/BattleState.cpp
#include "BattleState.h"
#include "../System/GridSystem.h"
#include "../System/SpellSystem.h"
#include "../Singletons/DataRegistry.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../Spells/Fireball.h"
#include "../Spells/CreateWall.h"
#include "../Spells/LavaPool.h"
#include "../../Engine/Engine.h"

BattleState::BattleState()
    : dragon(nullptr)
    , fighter(nullptr)
    , currentPhase(BattlePhase::Setup)
    , turnCount(0)
    , cursorPosition{0, 0}
    , selectingTarget(false)
{
}

BattleState::~BattleState() {
    // Objects deleted in Unload()
}

// Step 2.1: Load battle state
// Reason: Initialize all systems and create characters
void BattleState::Load() {
    Engine::GetLogger().LogEvent("BattleState: Loading");

    // Step 2.1a: Initialize GridSystem
    // Reason: 8x8 grid for tactical combat
    GridSystem& grid = GridSystem::GetInstance();
    grid.Initialize(8, 8);

    // Step 2.1b: Load character data
    // Reason: Dragon and Fighter stats from JSON files
    DataRegistry& registry = DataRegistry::GetInstance();
    registry.LoadAllCharacterData();

    // Step 2.1c: Register spells with SpellSystem
    // Reason: Spells must be registered before use
    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new Fireball());
    spellSys.RegisterSpell("CreateWall", new CreateWall());
    spellSys.RegisterSpell("LavaPool", new LavaPool());

    // Step 2.1d: Create Dragon (player controlled)
    // Reason: Load from JSON data
    dragon = static_cast<Dragon*>(Character::CreateFromData("dragon", Math::vec2{1, 1}));
    if (dragon) {
        grid.PlaceCharacter(dragon, Math::vec2{1, 1});
        AddGameObject(dragon);  // Add to GameObjectManager
    }

    // Step 2.1e: Create Fighter (enemy)
    fighter = static_cast<Fighter*>(Character::CreateFromData("fighter", Math::vec2{6, 6}));
    if (fighter) {
        grid.PlaceCharacter(fighter, Math::vec2{6, 6});
        AddGameObject(fighter);
    }

    // Step 2.1f: Initialize turn order
    // Reason: Dragon goes first for Week 3 (initiative system in Week 4)
    currentPhase = BattlePhase::PlayerTurn;
    turnCount = 1;

    // Step 2.1g: Subscribe to battle events
    // Reason: Listen for character death events
    EventBus::GetInstance().Subscribe<CharacterDeathEvent>([this](const CharacterDeathEvent& event) {
        Engine::GetLogger().LogEvent("BattleState: " + event.character->TypeName() + " has died");
        CheckBattleEnd();
    });

    Engine::GetLogger().LogEvent("BattleState: Battle started - Dragon vs Fighter");
}
```

#### **Task 3: Input Handling**

**Why this step**: Handle player input for movement, spell casting, and ability use during Player Turn.

```cpp
// Step 3.1: Main input handler
// Reason: Route input based on current battle phase
void BattleState::HandleInput() {
    if (currentPhase == BattlePhase::PlayerTurn) {
        HandlePlayerTurnInput();
    } else if (currentPhase == BattlePhase::EnemyTurn) {
        HandleEnemyTurnInput();
    }
}

// Step 3.2: Player turn input
// Reason: Dragon movement, spell casting, ability use
void BattleState::HandlePlayerTurnInput() {
    CS230::Input& input = Engine::GetInput();

    // Step 3.2a: Move grid cursor with arrow keys
    // Reason: Player selects tiles for movement/spells
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Up)) {
        cursorPosition.y = std::max(0.0, cursorPosition.y - 1);
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Down)) {
        cursorPosition.y = std::min(7.0, cursorPosition.y + 1);
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Left)) {
        cursorPosition.x = std::max(0.0, cursorPosition.x - 1);
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Right)) {
        cursorPosition.x = std::min(7.0, cursorPosition.x + 1);
    }

    // Step 3.2b: Spell keys (1, 2, 3)
    // Reason: Cast Dragon spells
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Num1)) {
        // Cast Fireball at cursor position
        SpellSystem& spellSys = SpellSystem::GetInstance();
        SpellResult result = spellSys.CastSpell(dragon, "Fireball", cursorPosition);

        if (result.success) {
            Engine::GetLogger().LogEvent("BattleState: Dragon cast Fireball");
        }
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Num2)) {
        // Cast CreateWall at cursor position
        SpellSystem& spellSys = SpellSystem::GetInstance();
        SpellResult result = spellSys.CastSpell(dragon, "CreateWall", cursorPosition);

        if (result.success) {
            Engine::GetLogger().LogEvent("BattleState: Dragon cast CreateWall");
        }
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Num3)) {
        // Cast LavaPool at cursor position
        SpellSystem& spellSys = SpellSystem::GetInstance();
        SpellResult result = spellSys.CastSpell(dragon, "LavaPool", cursorPosition);

        if (result.success) {
            Engine::GetLogger().LogEvent("BattleState: Dragon cast LavaPool");
        }
    }

    // Step 3.2c: End turn (Space)
    // Reason: Player finishes actions
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Space)) {
        EndCurrentTurn();
    }
}

// Step 3.3: Enemy turn input (manual control for Week 3)
// Reason: Manually control Fighter for testing
void BattleState::HandleEnemyTurnInput() {
    CS230::Input& input = Engine::GetInput();

    // Step 3.3a: Move cursor (same as player turn)
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Up)) {
        cursorPosition.y = std::max(0.0, cursorPosition.y - 1);
    }
    // ... (same cursor movement as player turn)

    // Step 3.3b: Fighter abilities (A for melee, S for shield bash)
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::A)) {
        // Melee attack (need adjacent target)
        if (dragon) {
            MeleeAttack attack;
            AbilityResult result = attack.Use(fighter, dragon);

            if (result.success) {
                Engine::GetLogger().LogEvent("BattleState: Fighter used Melee Attack");
            }
        }
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::S)) {
        // Shield Bash (need adjacent target)
        if (dragon) {
            ShieldBash bash;
            AbilityResult result = bash.Use(fighter, dragon);

            if (result.success) {
                Engine::GetLogger().LogEvent("BattleState: Fighter used Shield Bash");
            }
        }
    }

    // Step 3.3c: End turn (Space)
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Space)) {
        EndCurrentTurn();
    }
}
```

#### **Task 4: Turn Management**

**Why this step**: Handle turn transitions and check for battle end conditions.

```cpp
// Step 4.1: End current turn
// Reason: Switch between player and enemy turns
void BattleState::EndCurrentTurn() {
    if (currentPhase == BattlePhase::PlayerTurn) {
        // Step 4.1a: Refresh Dragon's AP
        // Reason: Full AP at start of turn
        dragon->RefreshActionPoints();

        // Step 4.1b: Switch to enemy turn
        currentPhase = BattlePhase::EnemyTurn;
        Engine::GetLogger().LogEvent("BattleState: Fighter's turn");
    }
    else if (currentPhase == BattlePhase::EnemyTurn) {
        // Step 4.1c: Refresh Fighter's AP
        fighter->RefreshActionPoints();

        // Step 4.1d: Switch to player turn
        currentPhase = BattlePhase::PlayerTurn;
        turnCount++;
        Engine::GetLogger().LogEvent("BattleState: Dragon's turn (Turn " + std::to_string(turnCount) + ")");
    }

    // Step 4.1e: Check if battle ended
    CheckBattleEnd();
}

// Step 4.2: Check battle end conditions
// Reason: Detect when one character dies
void BattleState::CheckBattleEnd() {
    if (dragon && dragon->GetCurrentHP() <= 0) {
        currentPhase = BattlePhase::BattleEnd;
        Engine::GetLogger().LogEvent("BattleState: BATTLE END - Fighter wins!");
    }
    else if (fighter && fighter->GetCurrentHP() <= 0) {
        currentPhase = BattlePhase::BattleEnd;
        Engine::GetLogger().LogEvent("BattleState: BATTLE END - Dragon wins!");
    }
}

// Step 4.3: Get current turn character
// Reason: Helper for UI display
Character* BattleState::GetCurrentTurnCharacter() {
    if (currentPhase == BattlePhase::PlayerTurn) {
        return dragon;
    } else if (currentPhase == BattlePhase::EnemyTurn) {
        return fighter;
    }
    return nullptr;
}
```

#### **Task 5: Update and Draw**

**Why this step**: Update game logic and render the battle scene.

```cpp
// Step 5.1: Update battle state
// Reason: Process input and update game objects
void BattleState::Update(double dt) {
    // Step 5.1a: Handle input
    HandleInput();

    // Step 5.1b: Update all game objects
    // Reason: Character animations, particle effects, etc.
    UpdateGameObjects(dt);  // CS230::GameState method

    // Step 5.1c: Check battle end (in case of events)
    CheckBattleEnd();
}

// Step 5.2: Draw battle scene
// Reason: Render grid, characters, UI
void BattleState::Draw() {
    // Step 5.2a: Render grid
    RenderGrid();

    // Step 5.2b: Render characters
    DrawGameObjects(Engine::GetWindow().GetCameraMatrix());  // CS230::GameState method

    // Step 5.2c: Render UI
    RenderUI();
}

// Step 5.3: Render grid (simple visualization for Week 3)
// Reason: Show 8x8 grid with tile types
void BattleState::RenderGrid() {
    GridSystem& grid = GridSystem::GetInstance();

    // Simple grid rendering (Week 3 basic version)
    // TODO Week 4: Use GridDebugRenderer for better visualization

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Math::vec2 tilePos{(double)x, (double)y};

            // Get tile type
            TileType type = grid.GetTileType(tilePos);

            // Draw tile (placeholder - use debug renderer in Week 4)
            // ... (simple rectangle drawing based on tile type)
        }
    }

    // Draw cursor
    // ... (simple highlight at cursorPosition)
}

// Step 5.4: Render UI (simple text display for Week 3)
// Reason: Show HP, AP, turn info
void BattleState::RenderUI() {
    CS230::Font& font = Engine::GetFont(Fonts::Default);

    // Draw Dragon HP/AP
    if (dragon) {
        std::string dragonInfo = "Dragon HP: " + std::to_string(dragon->GetCurrentHP()) + "/" + std::to_string(dragon->GetMaxHP());
        dragonInfo += " | AP: " + std::to_string(dragon->GetCurrentActionPoints()) + "/" + std::to_string(dragon->GetMaxActionPoints());
        font.Draw(dragonInfo, Math::vec2{10, 550});
    }

    // Draw Fighter HP/AP
    if (fighter) {
        std::string fighterInfo = "Fighter HP: " + std::to_string(fighter->GetCurrentHP()) + "/" + std::to_string(fighter->GetMaxHP());
        fighterInfo += " | AP: " + std::to_string(fighter->GetCurrentActionPoints()) + "/" + std::to_string(fighter->GetMaxActionPoints());
        font.Draw(fighterInfo, Math::vec2{10, 530});
    }

    // Draw turn info
    std::string turnInfo = "Turn " + std::to_string(turnCount) + " - " +
        (currentPhase == BattlePhase::PlayerTurn ? "Dragon's Turn" : "Fighter's Turn");
    font.Draw(turnInfo, Math::vec2{10, 10});

    // Draw battle end message
    if (currentPhase == BattlePhase::BattleEnd) {
        std::string winMessage = (dragon && dragon->GetCurrentHP() > 0) ? "DRAGON WINS!" : "FIGHTER WINS!";
        font.Draw(winMessage, Math::vec2{300, 300});
    }
}

// Step 5.5: Unload battle state
// Reason: Clean up resources
void BattleState::Unload() {
    Engine::GetLogger().LogEvent("BattleState: Unloading");

    // Unload game objects
    ClearGameObjects();  // CS230::GameState method (deletes dragon, fighter)

    // Clear grid
    GridSystem::GetInstance().Clear();

    dragon = nullptr;
    fighter = nullptr;
}
```

### Rigorous Testing (BattleState)

```cpp
// File: CS230/Game/Test/BattleStateTests.cpp
#include "../States/BattleState.h"
#include "../System/GridSystem.h"
#include "../Singletons/DataRegistry.h"

bool TestBattleStateLoad() {
    BattleState battle;

    // Test: Load battle state
    battle.Load();

    // Test: Dragon and Fighter exist
    ASSERT_TRUE(battle.GetDragon() != nullptr);
    ASSERT_TRUE(battle.GetFighter() != nullptr);

    // Test: Characters are on grid
    GridSystem& grid = GridSystem::GetInstance();
    ASSERT_TRUE(grid.IsOccupied(Math::vec2{1, 1}));  // Dragon position
    ASSERT_TRUE(grid.IsOccupied(Math::vec2{6, 6}));  // Fighter position

    battle.Unload();
    return true;
}

bool TestTurnTransitions() {
    BattleState battle;
    battle.Load();

    // Test: Starts with player turn
    ASSERT_TRUE(battle.GetCurrentPhase() == BattleState::BattlePhase::PlayerTurn);

    // Test: End player turn
    battle.EndCurrentTurn();
    ASSERT_TRUE(battle.GetCurrentPhase() == BattleState::BattlePhase::EnemyTurn);

    // Test: End enemy turn
    battle.EndCurrentTurn();
    ASSERT_TRUE(battle.GetCurrentPhase() == BattleState::BattlePhase::PlayerTurn);

    battle.Unload();
    return true;
}

bool TestBattleEndCondition() {
    BattleState battle;
    battle.Load();

    // Test: Kill Fighter
    Fighter* fighter = battle.GetFighter();
    fighter->TakeDamage(1000, battle.GetDragon());

    // Test: Battle should end
    battle.CheckBattleEnd();
    ASSERT_TRUE(battle.GetCurrentPhase() == BattleState::BattlePhase::BattleEnd);

    battle.Unload();
    return true;
}

void RunBattleStateTests() {
    std::cout << "\n=== BattleState Tests ===\n";
    std::cout << (TestBattleStateLoad() ? "✅" : "❌") << " Battle State Load\n";
    std::cout << (TestTurnTransitions() ? "✅" : "❌") << " Turn Transitions\n";
    std::cout << (TestBattleEndCondition() ? "✅" : "❌") << " Battle End Condition\n";
}
```

### Daily Breakdown (Developer E)

#### **Monday (6-8 hours)**

- Morning: Design BattleState class structure (1.5 hrs)
- Mid-morning: Implement Load() method (2 hrs)
- Afternoon: Implement basic input handling (2 hrs)
- End of day: Test battle initialization (1.5 hrs)
- **Deliverable**: BattleState loads Dragon and Fighter

#### **Tuesday (6-8 hours)**

- Morning: Implement turn management (2.5 hrs)
- Mid-morning: Implement spell casting input (2 hrs)
- Afternoon: Implement ability input (2 hrs)
- End of day: Test turn flow (1 hr)
- **Deliverable**: Full turn system working

#### **Wednesday (4-6 hours)**

- Morning: Implement basic grid rendering (2 hrs)
- Mid-morning: Implement UI rendering (2 hrs)
- Afternoon: Integration testing with all systems (2 hrs)
- **Deliverable**: Complete playable battle

#### **Thursday (4-6 hours)**

- Morning: Bug fixes from integration testing (3 hrs)
- Afternoon: Polish UI and controls (2 hrs)
- End of day: Prepare Friday demo (1 hr)
- **Deliverable**: Polished battle ready for demo

#### **Friday (2-4 hours)**

- Morning: Final testing and bug fixes (1 hr)
- Afternoon: Full team integration demo (2 hrs)
- **Deliverable**: Dragon vs Fighter full combat demo

---

## Week 3 Integration Test (Friday Afternoon)

**Goal**: Demonstrate full Dragon vs Fighter combat with all Week 3 features

**Participants**: All 5 developers

**Duration**: 60 minutes

### Integration Test Scenario

```cpp
// File: CS230/Game/Test/Week3IntegrationTest.cpp
#include "../States/BattleState.h"
#include "../../Engine/Engine.h"

bool Week3IntegrationTest() {
    std::cout << "\n=== Week 3 Integration Test ===\n";
    std::cout << "Testing: Full Dragon vs Fighter Combat\n\n";

    // Test 1: Battle state loads with data-driven characters
    std::cout << "Test 1: Loading battle...\n";
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    ASSERT_TRUE(dragon != nullptr);
    ASSERT_TRUE(fighter != nullptr);
    std::cout << "✅ Dragon and Fighter loaded from JSON\n";

    // Test 2: Dragon casts Fireball
    std::cout << "\nTest 2: Dragon casts Fireball...\n";
    SpellSystem& spellSys = SpellSystem::GetInstance();
    Math::vec2 fireballTarget = fighter->GetGridPosition();
    SpellResult fireballResult = spellSys.CastSpell(dragon, "Fireball", fireballTarget);

    ASSERT_TRUE(fireballResult.success);
    ASSERT_TRUE(fireballResult.damageDealt > 0);
    ASSERT_TRUE(fighter->GetCurrentHP() < fighter->GetMaxHP());
    std::cout << "✅ Fireball hit Fighter for " << fireballResult.damageDealt << " damage\n";

    // Test 3: Dragon casts CreateWall
    std::cout << "\nTest 3: Dragon casts CreateWall...\n";
    Math::vec2 wallTarget{4, 4};
    SpellResult wallResult = spellSys.CastSpell(dragon, "CreateWall", wallTarget);

    ASSERT_TRUE(wallResult.success);
    GridSystem& grid = GridSystem::GetInstance();
    ASSERT_TRUE(grid.GetTileType(wallTarget) == TileType::Wall);
    std::cout << "✅ CreateWall created wall tiles\n";

    // Test 4: Dragon casts LavaPool
    std::cout << "\nTest 4: Dragon casts LavaPool...\n";
    Math::vec2 lavaTarget = fighter->GetGridPosition();
    int fighterHPBefore = fighter->GetCurrentHP();
    SpellResult lavaResult = spellSys.CastSpell(dragon, "LavaPool", lavaTarget);

    ASSERT_TRUE(lavaResult.success);
    ASSERT_TRUE(fighter->GetCurrentHP() < fighterHPBefore);
    std::cout << "✅ LavaPool dealt " << lavaResult.damageDealt << " damage\n";

    // Test 5: Turn transition
    std::cout << "\nTest 5: End Dragon's turn...\n";
    battle.EndCurrentTurn();
    ASSERT_TRUE(battle.GetCurrentPhase() == BattleState::BattlePhase::EnemyTurn);
    std::cout << "✅ Turn transitioned to Fighter\n";

    // Test 6: Fighter uses Melee Attack
    std::cout << "\nTest 6: Fighter uses Melee Attack...\n";
    // Move Fighter adjacent to Dragon (for testing)
    Math::vec2 dragonPos = dragon->GetGridPosition();
    Math::vec2 adjacentPos{dragonPos.x + 1, dragonPos.y};
    grid.MoveCharacter(fighter, adjacentPos);

    MeleeAttack attack;
    int dragonHPBefore = dragon->GetCurrentHP();
    AbilityResult attackResult = attack.Use(fighter, dragon);

    ASSERT_TRUE(attackResult.success);
    ASSERT_TRUE(dragon->GetCurrentHP() < dragonHPBefore);
    std::cout << "✅ Melee Attack hit Dragon for " << attackResult.damageDealt << " damage\n";

    // Test 7: Fighter uses Shield Bash
    std::cout << "\nTest 7: Fighter uses Shield Bash...\n";
    // Ensure space to push Dragon
    Math::vec2 pushDestination{dragonPos.x + 2, dragonPos.y};
    grid.SetTileType(pushDestination, TileType::Floor);

    ShieldBash bash;
    Math::vec2 dragonPosBefore = dragon->GetGridPosition();
    AbilityResult bashResult = bash.Use(fighter, dragon);

    ASSERT_TRUE(bashResult.success);
    ASSERT_TRUE(dragon->GetGridPosition() != dragonPosBefore);
    std::cout << "✅ Shield Bash pushed Dragon to new position\n";

    // Test 8: Battle end condition
    std::cout << "\nTest 8: Testing battle end...\n";
    fighter->TakeDamage(1000, dragon);  // Kill Fighter
    battle.CheckBattleEnd();
    ASSERT_TRUE(battle.GetCurrentPhase() == BattleState::BattlePhase::BattleEnd);
    std::cout << "✅ Battle ended when Fighter died\n";

    battle.Unload();

    std::cout << "\n=== Week 3 Integration Test PASSED ===\n";
    std::cout << "All systems working together:\n";
    std::cout << "  - DataRegistry (JSON loading)\n";
    std::cout << "  - SpellSystem (all 3 Dragon spells)\n";
    std::cout << "  - Fighter abilities (Melee Attack, Shield Bash)\n";
    std::cout << "  - BattleState (full combat flow)\n";
    std::cout << "  - GridSystem, DiceManager, EventBus (Week 1 foundation)\n";

    return true;
}
```

### Demo Checklist

- [ ] **BattleState loads**: Dragon and Fighter spawn on grid
- [ ] **Dragon spells work**:
  - [ ] Fireball: Deals area damage
  - [ ] CreateWall: Creates impassable terrain
  - [ ] LavaPool: Creates hazardous terrain
- [ ] **Fighter abilities work**:
  - [ ] Melee Attack: Deals damage to adjacent target
  - [ ] Shield Bash: Deals damage and pushes target
- [ ] **Turn system**: Turns transition correctly
- [ ] **Data-driven**: Characters loaded from JSON files
- [ ] **Battle end**: Detects when character dies
- [ ] **No crashes**: Game runs for 5+ minutes without crash

---

## Week 3 Deliverable & Verification

### Final Deliverables (End of Week 3)

**Complete by Friday 5:00 PM**

#### **Developer A Deliverables**

- [ ] SpellBase interface implemented
- [ ] Fireball spell complete and tested (5+ tests passing)
- [ ] CreateWall spell complete and tested (3+ tests passing)
- [ ] LavaPool spell complete and tested (3+ tests passing)
- [ ] All spells integrated with SpellSystem

#### **Developer B Deliverables**

- [ ] AbilityBase interface implemented
- [ ] MeleeAttack complete and tested (3+ tests passing)
- [ ] ShieldBash complete and tested (3+ tests passing)
- [ ] Both abilities integrated with Fighter class

#### **Developer C Deliverables**

- [ ] SpellSystem singleton complete
- [ ] Spell registration system working
- [ ] Spell casting with upcasting working
- [ ] Helper methods for UI implemented
- [ ] 5+ tests passing

#### **Developer D Deliverables**

- [ ] DataRegistry singleton complete
- [ ] JSON loading system working
- [ ] dragon_stats.json and fighter_stats.json created
- [ ] Character factory method (CreateFromData) working
- [ ] 5+ tests passing

#### **Developer E Deliverables**

- [ ] BattleState complete
- [ ] Full combat flow working (player turn, enemy turn, end)
- [ ] All systems integrated
- [ ] Basic UI rendering working
- [ ] 3+ tests passing

### Integration Verification

**Run full integration test**:

```bash
# Build project
msbuild CS230_TaekyungHo.sln /p:Configuration=Debug

# Run integration test
Debug/CS230.exe --test week3_integration
```

**Expected Results**:

```
=== Week 3 Integration Test ===
Testing: Full Dragon vs Fighter Combat

Test 1: Loading battle...
✅ Dragon and Fighter loaded from JSON

Test 2: Dragon casts Fireball...
✅ Fireball hit Fighter for 45 damage

Test 3: Dragon casts CreateWall...
✅ CreateWall created wall tiles

Test 4: Dragon casts LavaPool...
✅ LavaPool dealt 28 damage

Test 5: End Dragon's turn...
✅ Turn transitioned to Fighter

Test 6: Fighter uses Melee Attack...
✅ Melee Attack hit Dragon for 15 damage

Test 7: Fighter uses Shield Bash...
✅ Shield Bash pushed Dragon to new position

Test 8: Testing battle end...
✅ Battle ended when Fighter died

=== Week 3 Integration Test PASSED ===
All systems working together:
  - DataRegistry (JSON loading)
  - SpellSystem (all 3 Dragon spells)
  - Fighter abilities (Melee Attack, Shield Bash)
  - BattleState (full combat flow)
  - GridSystem, DiceManager, EventBus (Week 1 foundation)
```

### Success Criteria

**Week 3 is complete when**:

1. **All Dragon spells work** (Fireball, CreateWall, LavaPool)
2. **All Fighter abilities work** (MeleeAttack, ShieldBash)
3. **SpellSystem manages all spells** (registration, validation, casting)
4. **DataRegistry loads from JSON** (dragon_stats.json, fighter_stats.json)
5. **BattleState runs full combat** (Dragon vs Fighter playable)
6. **All unit tests pass** (20+ tests total across all developers)
7. **Integration test passes** (week3_integration test)
8. **No critical bugs** (game runs 5+ minutes without crash)

### Week 4 Preview

**Next week's focus**:

- **Developer A**: Dragon spell balance and polish
- **Developer B**: TurnManager initiative system (1d20 + speed)
- **Developer C**: Fighter basic AI (autonomous decisions)
- **Developer D**: Data-driven stats complete (hot-reload working)
- **Developer E**: StatusEffectManager foundation (track lava burn, etc.)

**Week 4 goal**: Add initiative system, basic AI, and status effects to make combat more dynamic.

---

**End of Week 3 Implementation Plan**
