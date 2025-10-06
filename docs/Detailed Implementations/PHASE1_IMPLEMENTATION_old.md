# Phase 1: Foundation Layer Implementation (Week 1-12)

**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Foundation Layer + Core Gameplay
**Timeline**: Weeks 1-12
**Strategy**: Domain-based parallel development with stub interfaces

**Last Updated**: 2025-10-04

---

## Overview

Phase 1 establishes the foundation for parallel development across all 5 developer domains. Week 1 creates stub interfaces, enabling independent work from Week 2 onward.

**Critical Success Criteria:**

- ✅ Week 1: All stub interfaces created (enables parallel work)
- ✅ Each developer works independently using stubs for dependencies
- ✅ Weekly integration swaps (stub → real implementations)
- ✅ Each system testable in isolation
- ✅ Weekly builds demonstrating incremental progress

**Phase 1 Coverage (Weeks 1-12):**

This document covers the first 12 weeks of development across all 5 developer domains, organized by developer assignment.

---

## Table of Contents

- [Week 1: Stub Creation (All Developers)](#week-1-stub-creation-all-developers)
- [Developer 1: Character Systems (Weeks 1-12)](#developer-1-character-systems-weeks-1-12)
- [Developer 2: Battle Systems (Weeks 1-12)](#developer-2-battle-systems-weeks-1-12)
- [Developer 3: AI Systems (Weeks 1-12)](#developer-3-ai-systems-weeks-1-12)
- [Developer 4: Data & Services (Weeks 1-12)](#developer-4-data--services-weeks-1-12)
- [Developer 5: Debug & Tools (Weeks 1-12)](#developer-5-debug--tools-weeks-1-12)
- [Integration Checkpoints](#integration-checkpoints)

---

## Week 1: Stub Creation (All Developers)

**Goal**: Create stub interfaces for all systems to enable parallel development starting Week 2

**Critical**: All 5 developers must complete their stubs by end of Week 1!

### Developer 1: Character System Stubs

**Files to Create:**

```
CS230/Game/Characters/Character.h
CS230/Game/Components/GridPosition.h
CS230/Game/Components/ActionPoints.h
CS230/Game/Components/SpellSlots.h
CS230/Game/Components/StatusEffects.h
CS230/Game/Components/DamageCalculator.h
CS230/Game/Components/TargetingSystem.h
CS230/Game/Components/AIMemory.h
```

**Implementation Tasks:**

- [ ] Create `Character.h` with stub methods (all methods return dummy values)
- [ ] Create all 7 GOComponent header files with stub implementations
- [ ] Add to `GameObjectTypes.h`: `Dragon, Fighter, Cleric, Wizard, Rogue` enum values
- [ ] Document stub behavior in comments (e.g., "// STUB: Always returns 100 HP")

**Stub Character.h Template:**

```cpp
// CS230/Game/Characters/Character.h - STUB FOR WEEK 1
#pragma once
#include "../Engine/GameObject.h"

class Character : public CS230::GameObject {
public:
    virtual GameObjectTypes Type() = 0;
    virtual std::string TypeName() = 0;

    // STUB: Health system
    virtual void TakeDamage(int damage) { /* stub */ }
    virtual int GetCurrentHP() const { return 100; }  // STUB: Always 100
    virtual int GetMaxHP() const { return 100; }      // STUB: Always 100
    virtual bool IsAlive() const { return true; }      // STUB: Always alive

    // STUB: Action system
    virtual void PerformAction(ActionType action) { /* stub */ }
    virtual int GetActionPoints() const { return 2; }       // STUB: Always 2
    virtual int GetMovementRange() const { return 3; }      // STUB: Always 3
    virtual int GetSpeed() const { return 3; }              // STUB: Always 3

    // STUB: Grid positioning
    virtual Math::vec2 GetGridPosition() const { return {0, 0}; }  // STUB: Always origin
    virtual void SetGridPosition(Math::vec2 pos) { /* stub */ }

    // Add all other interface methods as stubs...
};
```

---

### Developer 2: Battle Systems Stubs

**Files to Create:**

```
CS230/Game/Systems/GridSystem.h
CS230/Game/Systems/TurnManager.h
CS230/Game/Systems/CombatSystem.h
CS230/Game/Systems/SpellSystem.h
CS230/Game/Systems/BattleManager.h
CS230/Game/Systems/StatusEffectManager.h
```

**Implementation Tasks:**

- [ ] Create all 6 GSComponent header files with stub implementations
- [ ] Each stub method returns safe dummy values (e.g., `IsValidMove() returns true`)
- [ ] Document stub behavior in comments

**Stub GridSystem.h Template:**

```cpp
// CS230/Game/Systems/GridSystem.h - STUB FOR WEEK 1
#pragma once
#include "../../Engine/Component.h"

class GridSystem : public CS230::Component {
public:
    void Update(double dt) override { /* stub */ }

    // STUB: Always returns valid
    bool IsValidMove(Math::vec2 from, Math::vec2 to) const { return true; }

    // STUB: Returns fake reachable tiles
    std::vector<Math::vec2> GetReachableTiles(Character* character) const {
        return {{0,0}, {1,0}, {0,1}};  // STUB: 3 fake tiles
    }

    // STUB: Returns straight line path
    std::vector<Math::vec2> FindPath(Math::vec2 from, Math::vec2 to) const {
        return {from, to};  // STUB: Direct path
    }

    // STUB: Always has line of sight
    bool HasLineOfSight(Math::vec2 from, Math::vec2 to) const { return true; }

    // Add all other interface methods as stubs...
};
```

---

### Developer 3: AI Systems Stubs

**Files to Create:**

```
CS230/Game/Systems/AISystem.h
CS230/Game/Singletons/AIDirector.h
CS230/Game/Types/AITypes.h  (AIDecision struct, etc.)
```

**Implementation Tasks:**

- [ ] Create `AISystem.h` GSComponent stub
- [ ] Create `AIDirector.h` singleton stub
- [ ] Create `AIDecision` struct definition
- [ ] All AI methods return "Wait" action

**Stub AISystem.h Template:**

```cpp
// CS230/Game/Systems/AISystem.h - STUB FOR WEEK 1
#pragma once
#include "../../Engine/Component.h"
#include "../Types/AITypes.h"

class AISystem : public CS230::Component {
public:
    void Update(double dt) override { /* stub */ }

    // STUB: Always returns "Wait" decision
    AIDecision MakeDecision(Character* aiCharacter) {
        return AIDecision{ActionType::Wait, {0,0}, 1, "stub"};
    }

    // Add all other interface methods as stubs...
};
```

---

### Developer 4: Data & Services Stubs

**Files to Create:**

```
CS230/Game/Singletons/EventBus.h
CS230/Game/Singletons/DiceManager.h
CS230/Game/Singletons/DataRegistry.h
CS230/Game/Singletons/EffectManager.h
CS230/Game/Singletons/SaveManager.h
CS230/Game/Factories/CharacterFactory.h
CS230/Game/Types/Events.h  (all event structs)
```

**Implementation Tasks:**

- [ ] Create all 5 singleton header files with stub implementations
- [ ] Create `CharacterFactory.h` with stub (returns nullptr)
- [ ] Define all event structs (DamageTakenEvent, CharacterDiedEvent, etc.)

**Stub DiceManager.h Template:**

```cpp
// CS230/Game/Singletons/DiceManager.h - STUB FOR WEEK 1
#pragma once

class DiceManager {
public:
    static DiceManager& Instance() {
        static DiceManager instance;
        return instance;
    }

    // STUB: Always returns 10
    int RollDice(int count, int sides) { return 10; }
    int RollDiceFromString(const std::string& diceNotation) { return 10; }

    void SetSeed(int seed) { /* stub */ }
};
```

---

### Developer 5: Debug & Tools Stubs

**Files to Create:**

```
CS230/Game/Debug/DebugManager.h
CS230/Game/Debug/DebugConsole.h
CS230/Game/States/BattleState.h
```

**Implementation Tasks:**

- [ ] Create `DebugManager.h` singleton stub
- [ ] Create `DebugConsole.h` stub
- [ ] Create `BattleState.h` minimal stub (empty Load/Update/Draw)

**Stub DebugManager.h Template:**

```cpp
// CS230/Game/Debug/DebugManager.h - STUB FOR WEEK 1
#pragma once

class DebugManager {
public:
    static DebugManager& Instance() {
        static DebugManager instance;
        return instance;
    }

    // STUB: Debug mode off
    bool IsDebugMode() const { return false; }
    void SetDebugMode(bool enabled) { /* stub */ }
    void ExecuteCommand(const std::string& command) { /* stub */ }
};
```

---

### Week 1 Deliverable

**End of Week 1**: All stub header files committed to repository, all 5 developers can start Week 2 work using each other's stubs.

**RIGOROUS VERIFICATION (Week 1 Completion Checklist)**

**Compilation Tests**:

- [ ] Test_AllStubsCompile()
  - Setup: Include all stub headers in single test file
  - Expected: Compiles without errors
  - Verify: No syntax errors, no missing dependencies

**Cross-Developer Integration Tests**:

- [ ] Test_Developer1Stubs_UsedByDeveloper2()
  
  - Setup: GridSystem (Dev 2) includes Character.h (Dev 1)
  - Action: Create stub Character, pass to GridSystem methods
  - Expected: Compiles and runs without crashes

- [ ] Test_Developer2Stubs_UsedByDeveloper1()
  
  - Setup: Character (Dev 1) includes GridSystem.h (Dev 2)
  - Action: Call GridSystem stub methods from Character
  - Expected: Stub methods return safe dummy values

- [ ] Test_Developer4Stubs_UsedByAll()
  
  - Setup: All developers include EventBus, DiceManager, DataRegistry stubs
  - Action: Call singleton Instance() methods
  - Expected: All singletons accessible and functional (returning dummy values)

**Stub Behavior Verification**:

- [ ] Test_StubsReturnSafeValues()
  - Character->GetCurrentHP() returns 100 (not 0, not negative)
  - Character->IsAlive() returns true (not false)
  - GridSystem->IsValidMove() returns true (not false, prevents blocking)
  - DiceManager->RollDice() returns 10 (consistent value)
  - AISystem->MakeDecision() returns Wait action (safe default)

**Documentation Verification**:

- [ ] All stub files have comments marking them as "STUB FOR WEEK 1"
- [ ] All stub methods have comments explaining dummy behavior
- [ ] Each developer has documented which stubs they depend on

**Repository Verification**:

- [ ] All stub files committed to version control
- [ ] No merge conflicts between developers' stub files
- [ ] Build system compiles all stubs successfully

**Acceptance Criteria (Week 1)**:

- [ ] All 5 developers can compile their projects with all stubs included
- [ ] No developer is blocked by missing stub files
- [ ] Stub methods return safe, predictable values
- [ ] All stubs documented and committed to repository

**Week 1 Exit Interview** (5-minute check with each developer):

- **Question 1**: "Can you successfully `#include` all other developers' stub headers?"
- **Question 2**: "Do all stub methods return the values you expect?"
- **Question 3**: "Are you ready to start Week 2 implementation using these stubs?"
- **Expected Answer**: YES to all 3 questions from all 5 developers

**If ANY developer answers NO**: Week 1 NOT complete, fix stubs before proceeding to Week 2.

---

## Developer 1: Character Systems (Weeks 1-12)

**Domain**: All character implementations + character-level components

### Week 1: Stubs (See Above)

### Weeks 2-4: Character Base Class

**Goal**: Complete Character base class implementation with full interface

**Files**:

- `CS230/Game/Characters/Character.h`
- `CS230/Game/Characters/Character.cpp`

**Implementation Tasks:**

- [ ] **Character data members**
  
  ```cpp
  protected:
      int maxHP;
      int currentHP;
      int speed;
      int maxActionPoints;
      int currentActionPoints;
      int baseAttackPower;
      std::string attackDice;
      int baseDefensePower;
      std::string defenseDice;
      int attackRange;
      std::map<int, int> spellSlots;  // level -> count
      Math::vec2 gridPosition;
  ```

- [ ] **Health system methods**
  
  ```cpp
  void TakeDamage(int damage);
  void Heal(int amount);
  int GetCurrentHP() const;
  int GetMaxHP() const;
  bool IsAlive() const;
  void Die();
  ```

- [ ] **Action system methods**
  
  ```cpp
  void PerformAction(ActionType action);
  int GetActionPoints() const;
  void ConsumeActionPoints(int cost);
  void RefreshActionPoints();
  int GetMovementRange() const;
  int GetSpeed() const;
  ```

- [ ] **GameObject overrides**
  
  ```cpp
  void Update(double dt) override;
  void Draw(Math::TransformationMatrix camera_matrix) override;
  GameObjectTypes Type() override = 0;  // Pure virtual
  std::string TypeName() override = 0;   // Pure virtual
  ```

- [ ] **Collision system**
  
  ```cpp
  bool CanCollideWith(GameObjectTypes other_object_type) override;
  void ResolveCollision(GameObject* other_object) override;
  ```

- [ ] **State machine setup**
  
  ```cpp
  class State {
  public:
      virtual void Enter(Character* character) = 0;
      virtual void Update(Character* character, double dt) = 0;
      virtual void CheckExit(Character* character) = 0;
      virtual std::string GetName() const = 0;
  };
  
  State* current_state;
  
  class State_WaitingTurn : public State { ... };
  class State_Acting : public State { ... };
  class State_Dead : public State { ... };
  ```

**Dependencies**: Uses stub GridSystem, stub CombatSystem

**RIGOROUS TESTING (Weeks 2-4: Character Base Class)**

**Unit Tests - Health System**:

- [ ] Test_TakeDamage_Normal()
  
  - Setup: Character with maxHP=100, currentHP=100
  - Input: TakeDamage(30)
  - Expected: currentHP == 70
  - Verify: IsAlive() returns true

- [ ] Test_TakeDamage_Fatal()
  
  - Setup: Character with currentHP=20
  - Input: TakeDamage(25)
  - Expected: currentHP == 0 (NOT -5)
  - Expected: IsAlive() returns false
  - Expected: current_state == &state_dead

- [ ] Test_Heal_Normal()
  
  - Setup: Character with maxHP=100, currentHP=50
  - Input: Heal(30)
  - Expected: currentHP == 80

- [ ] Test_Heal_OverMax()
  
  - Setup: Character with maxHP=100, currentHP=90
  - Input: Heal(30)
  - Expected: currentHP == 100 (NOT 120)

- [ ] Test_Heal_Dead()
  
  - Setup: Character with currentHP=0, IsAlive()==false
  - Input: Heal(50)
  - Expected: currentHP remains 0 (dead characters cannot be healed)

**Unit Tests - Action System**:

- [ ] Test_ConsumeActionPoints_Valid()
  
  - Setup: maxActionPoints=5, currentActionPoints=5
  - Input: ConsumeActionPoints(2)
  - Expected: currentActionPoints == 3

- [ ] Test_ConsumeActionPoints_Insufficient()
  
  - Setup: currentActionPoints=1
  - Input: ConsumeActionPoints(2)
  - Expected: Assert/Log error, currentActionPoints unchanged

- [ ] Test_RefreshActionPoints()
  
  - Setup: maxActionPoints=5, currentActionPoints=0
  - Input: RefreshActionPoints()
  - Expected: currentActionPoints == 5

**Unit Tests - State Machine**:

- [ ] Test_StateTransition_WaitingToActing()
  
  - Setup: current_state = &state_waiting_turn
  - Action: Simulate turn start
  - Expected: current_state == &state_acting

- [ ] Test_StateTransition_ActingToDead()
  
  - Setup: current_state = &state_acting
  - Action: TakeDamage(9999)
  - Expected: current_state == &state_dead

- [ ] Test_State_DeadPersists()
  
  - Setup: current_state = &state_dead
  - Action: Update(dt) multiple times
  - Expected: current_state remains &state_dead (no exit from dead state)

**Acceptance Criteria**:

- [ ] Character can be instantiated without crashing
- [ ] All health methods work correctly with edge cases
- [ ] Action points correctly consume and refresh
- [ ] State machine transitions work as expected
- [ ] GameObject methods (Update/Draw/Type/TypeName) compile and run

**Console Testing Commands** (Week 4+):

```cpp
// Create test character
spawn dragon 4 4

// Test health system
damage dragon 30     // Should reduce HP by 30
heal dragon 20       // Should increase HP by 20
damage dragon 9999   // Should kill dragon (HP=0, state=dead)

// Test action points
setstat dragon actionPoints 0  // Reduce to 0
refresh dragon                 // Should restore to max
```

---

### Week 5-8: Dragon Implementation

**Goal**: Fully playable Dragon character with all 9 spells

**Files**:

- `CS230/Game/Characters/Dragon.h`
- `CS230/Game/Characters/Dragon.cpp`

**Implementation Tasks:**

- [ ] **Dragon class declaration**
  
  ```cpp
  class Dragon : public Character {
  public:
      Dragon(Math::vec2 gridPos);
  
      GameObjectTypes Type() override { return GameObjectTypes::Dragon; }
      std::string TypeName() override { return "Dragon"; }
  
      // Spell methods
      void Spell_CreateWall(int upcastLevel);
      void Spell_Fireball(int upcastLevel);
      void Spell_LavaPool(int upcastLevel);
      void Spell_DragonRoar(int upcastLevel);
      void Spell_TailSwipe(int upcastLevel);
      void Spell_DragonWrath(int upcastLevel);
      void Spell_MeteorStrike(int upcastLevel);
      void Spell_HeatAbsorb(int upcastLevel);
      void Spell_HeatRelease(int upcastLevel);
  };
  ```

- [ ] **Dragon stats (from JSON via DataRegistry)**
  
  ```cpp
  // In Dragon constructor:
  maxHP = DataRegistry::Instance().GetValue<int>("Dragon.maxHP", 140);
  speed = DataRegistry::Instance().GetValue<int>("Dragon.speed", 5);
  maxActionPoints = DataRegistry::Instance().GetValue<int>("Dragon.maxActionPoints", 5);
  // Load attack/defense dice, spell slots, etc.
  ```

- [ ] **Implement all 9 spells** (one per day for 9 days)
  
  - Week 5: CreateWall, Fireball
  - Week 6: LavaPool, DragonRoar, TailSwipe
  - Week 7: DragonWrath, MeteorStrike
  - Week 8: HeatAbsorb, HeatRelease

- [ ] **Spell slot management**
  
  ```cpp
  bool HasSpellSlot(int level) const;
  void ConsumeSpellSlot(int level);
  bool CanUpcast(int baseLevel, int targetLevel) const;
  ```

- [ ] **Dragon state machine**
  
  ```cpp
  class State_PlayerTurn : public State { ... };
  class State_SelectingAction : public State { ... };
  class State_CastingSpell : public State { ... };
  ```

**Dependencies**:

- Uses stub SpellSystem (Weeks 5-7)
- Swaps to real SpellSystem (Week 8+) - **INTEGRATION POINT**

**RIGOROUS TESTING (Weeks 5-8: Dragon + Spells)**

**Unit Tests - Dragon Stats**:

- [ ] Test_DragonConstruction()
  - Input: Dragon(gridPos={4,4})
  - Expected stats from JSON:
    - maxHP == 140
    - speed == 5
    - maxActionPoints == 5
    - baseAttackPower == 5
    - attackDice == "3d6"
    - baseDefensePower == 3
    - defenseDice == "2d8"
    - attackRange == 1
  - Verify: Type() returns GameObjectTypes::Dragon
  - Verify: TypeName() returns "Dragon"

**Unit Tests - Spell Slot Management**:

- [ ] Test_HasSpellSlot_Valid()
  
  - Setup: Dragon with spell slots: {1:4, 2:3, 3:2}
  - Input: HasSpellSlot(1), HasSpellSlot(2), HasSpellSlot(3)
  - Expected: All return true

- [ ] Test_HasSpellSlot_Invalid()
  
  - Setup: Same dragon
  - Input: HasSpellSlot(4), HasSpellSlot(0), HasSpellSlot(-1)
  - Expected: All return false

- [ ] Test_ConsumeSpellSlot()
  
  - Setup: Dragon with level 3 slots: {3:2}
  - Input: ConsumeSpellSlot(3) twice
  - Expected: After first call: {3:1}
  - Expected: After second call: {3:0}
  - Input: ConsumeSpellSlot(3) third time
  - Expected: Assert/Log error, slots remain {3:0}

- [ ] Test_CanUpcast()
  
  - Setup: Dragon with {1:4, 2:3, 3:2}
  - Input: CanUpcast(baseLevel=1, targetLevel=2)
  - Expected: true (has level 2 slot available)
  - Input: CanUpcast(baseLevel=1, targetLevel=4)
  - Expected: false (no level 4 slot)

**Spell Tests - Fireball** (test each spell with similar rigor):

- [ ] Test_Spell_Fireball_BaseLevel()
  
  - Setup: Dragon at (4,4), enemy at (6,4)
  - Input: Spell_Fireball(upcastLevel=1)
  - Expected Actions:
    1. Check HasSpellSlot(1) → true
    2. ConsumeSpellSlot(1)
    3. Calculate damage: 2d6 per target
    4. Target selection: All enemies within 2x2 area around (6,4)
    5. Apply damage to all targets
  - Expected console output: "Dragon cast Fireball (Level 1)! Dealt X damage to Y enemies."

- [ ] Test_Spell_Fireball_Upcast()
  
  - Setup: Same as above
  - Input: Spell_Fireball(upcastLevel=3)
  - Expected Actions:
    1. Check HasSpellSlot(3) → true
    2. ConsumeSpellSlot(3)
    3. Calculate damage: 2d6 + 1d6 + 1d6 = 4d6 per target (upcast bonus)
    4. Apply increased damage
  - Verify: Damage > base level damage

- [ ] Test_Spell_Fireball_NoSlot()
  
  - Setup: Dragon with {1:0} (no level 1 slots)
  - Input: Spell_Fireball(upcastLevel=1)
  - Expected: Assert/Log error "No spell slot available for level 1"
  - Expected: Spell does NOT execute

**Spell Tests - CreateWall**:

- [ ] Test_Spell_CreateWall_BaseLevel()
  
  - Setup: Dragon at (4,4)
  - Input: Spell_CreateWall(upcastLevel=1), select tile (5,4)
  - Expected Actions:
    1. ConsumeSpellSlot(1)
    2. GridSystem->SetTileType((5,4), TileType::Wall)
    3. Wall blocks movement and LOS
  - Verify: GridSystem->IsValidMove(4,4 → 6,4) returns false (wall blocks)

- [ ] Test_Spell_CreateWall_Upcast()
  
  - Input: Spell_CreateWall(upcastLevel=2)
  - Expected: Creates 3x1 wall instead of 1x1
  - Verify: 3 tiles changed to Wall type

**Spell Tests - TailSwipe**:

- [ ] Test_Spell_TailSwipe_SingleTarget()
  
  - Setup: Dragon at (4,4), enemy at (5,4)
  - Input: Spell_TailSwipe(upcastLevel=1)
  - Expected Actions:
    1. Damage enemy: 3d6
    2. Knockback enemy 2 tiles away → enemy moves to (7,4)
  - Verify: Enemy position changes from (5,4) to (7,4)

- [ ] Test_Spell_TailSwipe_KnockbackBlocked()
  
  - Setup: Dragon at (4,4), enemy at (5,4), wall at (6,4)
  - Input: Spell_TailSwipe(upcastLevel=1)
  - Expected: Enemy takes damage but knockback stops at (5,4) (wall blocks)

**Spell Tests - HeatAbsorb & HeatRelease** (linked mechanic):

- [ ] Test_Spell_HeatAbsorb()
  
  - Setup: Dragon at full HP (140/140)
  - Input: Spell_HeatAbsorb(upcastLevel=1)
  - Expected Actions:
    1. ConsumeSpellSlot(1)
    2. Dragon takes 10 self-damage → HP = 130/140
    3. Internal counter: absorbedHeatLevel = 1
  - Input: Spell_HeatAbsorb(upcastLevel=2)
  - Expected: HP = 120/140, absorbedHeatLevel = 3

- [ ] Test_Spell_HeatRelease()
  
  - Setup: Dragon with absorbedHeatLevel = 3
  - Input: Spell_HeatRelease(upcastLevel=1)
  - Expected Actions:
    1. Calculate damage: 3d6 * 3 = 9d6 total
    2. AOE attack in 3x3 area
    3. Restore spell slots: Levels 1-3 fully restored
    4. absorbedHeatLevel = 0
  - Verify: Damage scales with absorbed heat level

**Spell Tests - MeteorStrike** (highest level spell):

- [ ] Test_Spell_MeteorStrike()
  - Setup: Dragon with level 3 spell slot
  - Input: Spell_MeteorStrike(upcastLevel=3), target tile (6,6)
  - Expected Actions:
    1. ConsumeSpellSlot(3)
    2. Create lava pool at (6,6)
    3. Damage all enemies in 3x3 area: 5d6 each
    4. AOE knockback from center
  - Verify: GridSystem->GetTileType((6,6)) == TileType::Lava

**Integration Tests - Spell + Combat**:

- [ ] Test_SpellCombat_FireballKill()
  - Setup: Dragon at (4,4), weak enemy at (6,4) with 10 HP
  - Input: Spell_Fireball(upcastLevel=1) targeting enemy
  - Expected: Enemy HP reduced, if HP <= 0 → enemy dies
  - Verify: EventBus receives CharacterDiedEvent

**Acceptance Criteria**:

- [ ] All 9 spells implemented and compile without errors
- [ ] Each spell correctly consumes spell slots
- [ ] Upcast mechanics increase spell power correctly
- [ ] Spell effects apply to correct targets/areas
- [ ] Edge cases handled (no slot, invalid target, blocked LOS)

**Console Testing Commands**:

```cpp
// Create test scenario
spawn dragon 4 4
spawn fighter 6 4
debug grid on

// Test spell slots
spellslots dragon           // Display: {1:4, 2:3, 3:2}

// Test Fireball
castspell dragon fireball 1 // Cast at level 1
spellslots dragon           // Verify: {1:3, 2:3, 3:2}

// Test upcasting
castspell dragon fireball 3 // Cast at level 3
spellslots dragon           // Verify: {1:3, 2:3, 3:1}

// Test all spells
castspell dragon createwall 1
castspell dragon tailswipe 1
castspell dragon dragonroar 2
castspell dragon meteorstrike 3
// ... test all 9 spells

// Test edge cases
castspell dragon fireball 1  // Repeat until slots empty
castspell dragon fireball 1  // Should fail: "No spell slot available"
```

**Key Milestone (Week 8)**: Dragon fully playable, can cast all spells, player can test basic gameplay loop

---

### Weeks 9-10: GOComponents (Part 1)

**Goal**: Implement GridPosition, ActionPoints, SpellSlots components

**Files**:

- `CS230/Game/Components/GridPosition.h/cpp`
- `CS230/Game/Components/ActionPoints.h/cpp`
- `CS230/Game/Components/SpellSlots.h/cpp`

**GridPosition Component:**

- [ ] **Data members**
  
  ```cpp
  private:
      Math::vec2 tilePosition;      // Grid coordinates (0-7, 0-7)
      Math::vec2 cachedWorldPosition;  // Pixel coordinates for rendering
  ```

- [ ] **Methods**
  
  ```cpp
  void Update(double dt) override;  // Sync world position
  Math::vec2 GetTilePosition() const;
  void SetTilePosition(Math::vec2 pos);
  Math::vec2 GetWorldPosition() const;  // Convert tile to pixels
  ```

**ActionPoints Component:**

- [ ] **Data members**
  
  ```cpp
  private:
      int maxPoints;
      int currentPoints;
  ```

- [ ] **Methods**
  
  ```cpp
  int GetCurrent() const;
  int GetMax() const;
  bool CanAfford(int cost) const;
  void Spend(int cost);
  void Refresh();  // Called by TurnManager at turn start
  ```

**SpellSlots Component:**

- [ ] **Data members**
  
  ```cpp
  private:
      std::map<int, int> maxSlots;      // level -> max count
      std::map<int, int> currentSlots;  // level -> remaining
  ```

- [ ] **Methods**
  
  ```cpp
  bool HasSlot(int level) const;
  void ConsumeSlot(int level);
  void RestoreSlots(int minLevel, int maxLevel);  // For Heat Release spell
  std::map<int, int> GetCurrentSlots() const;
  std::map<int, int> GetMaxSlots() const;
  ```

**Dependencies**: Real GridSystem available (Week 9+) - **INTEGRATION POINT**

**RIGOROUS TESTING (Weeks 9-10: GOComponents Part 1)**

**Unit Tests - GridPosition Component**:

- [ ] Test_GridPosition_TileToWorld()
  
  - Setup: GridPosition at tile (3, 5), TILE_SIZE=64
  - Input: GetWorldPosition()
  - Expected: Math::vec2(3*64 + 32, 5*64 + 32) = (224, 352)
  - Reasoning: Center of tile, not corner

- [ ] Test_GridPosition_SetTilePosition()
  
  - Setup: GridPosition at (0, 0)
  - Input: SetTilePosition({7, 7})
  - Expected: GetTilePosition() == {7, 7}
  - Expected: GetWorldPosition() == {7*64+32, 7*64+32}

- [ ] Test_GridPosition_Update()
  
  - Setup: GridPosition at tile (2, 2)
  - Input: Character moves to different world position
  - Input: Update(dt)
  - Expected: cachedWorldPosition syncs with new position

**Unit Tests - ActionPoints Component**:

- [ ] Test_ActionPoints_CanAfford()
  
  - Setup: ActionPoints with current=3, max=5
  - Input: CanAfford(2)
  - Expected: true
  - Input: CanAfford(4)
  - Expected: false

- [ ] Test_ActionPoints_Spend()
  
  - Setup: current=5
  - Input: Spend(2)
  - Expected: GetCurrent() == 3

- [ ] Test_ActionPoints_SpendTooMuch()
  
  - Setup: current=1
  - Input: Spend(2)
  - Expected: Assert/Log error
  - Expected: GetCurrent() remains 1

- [ ] Test_ActionPoints_Refresh()
  
  - Setup: max=5, current=0
  - Input: Refresh()
  - Expected: GetCurrent() == 5

**Unit Tests - SpellSlots Component**:

- [ ] Test_SpellSlots_HasSlot()
  
  - Setup: SpellSlots with {1:4, 2:3, 3:2}
  - Input: HasSlot(1), HasSlot(2), HasSlot(3)
  - Expected: All return true

- [ ] Test_SpellSlots_ConsumeSlot()
  
  - Setup: currentSlots = {2:3}
  - Input: ConsumeSlot(2)
  - Expected: currentSlots = {2:2}

- [ ] Test_SpellSlots_ConsumeEmptySlot()
  
  - Setup: currentSlots = {2:0}
  - Input: ConsumeSlot(2)
  - Expected: Assert/Log error
  - Expected: currentSlots remains {2:0}

- [ ] Test_SpellSlots_RestoreSlots()
  
  - Setup: maxSlots={1:4,2:3,3:2}, currentSlots={1:0,2:1,3:0}
  - Input: RestoreSlots(minLevel=1, maxLevel=3)
  - Expected: currentSlots == {1:4, 2:3, 3:2} (all fully restored)

- [ ] Test_SpellSlots_PartialRestore()
  
  - Setup: maxSlots={1:4,2:3,3:2}, currentSlots={1:0,2:0,3:0}
  - Input: RestoreSlots(minLevel=1, maxLevel=2)
  - Expected: currentSlots == {1:4, 2:3, 3:0} (only levels 1-2 restored)

**Integration Tests - Components + Character**:

- [ ] Test_ComponentAttachment()
  
  - Setup: Create Dragon character
  - Input: dragon->AddGOComponent(new GridPosition(4, 4))
  - Input: dragon->AddGOComponent(new ActionPoints(5))
  - Input: dragon->AddGOComponent(new SpellSlots({1:4, 2:3}))
  - Expected: All components attach without crashes
  - Verify: dragon->GetGOComponent<GridPosition>() != nullptr

- [ ] Test_Component_CharacterInteraction()
  
  - Setup: Dragon at (4,4) with ActionPoints(5) component
  - Input: Dragon moves 2 tiles
  - Expected: ActionPoints->Spend(2) called
  - Expected: GridPosition updated to new tile
  - Verify: Components interact correctly

**Acceptance Criteria**:

- [ ] All 3 components compile and attach to Characters
- [ ] GridPosition correctly converts tile ↔ world coordinates
- [ ] ActionPoints correctly track spending/refresh
- [ ] SpellSlots correctly manage slot consumption/restoration
- [ ] Components work together without conflicts

**Console Testing Commands**:

```cpp
// Create dragon with components
spawn dragon 4 4

// Test GridPosition
getpos dragon              // Display: "Dragon at tile (4,4), world (288, 288)"
teleport dragon 7 7        // Move dragon
getpos dragon              // Display: "Dragon at tile (7,7), world (480, 480)"

// Test ActionPoints
getstats dragon            // Display: "AP: 5/5"
move dragon 6 7            // Move 1 tile (costs 1 AP)
getstats dragon            // Display: "AP: 4/5"
refresh dragon             // Refresh action points
getstats dragon            // Display: "AP: 5/5"

// Test SpellSlots
spellslots dragon          // Display: {1:4, 2:3, 3:2}
castspell dragon fireball 1
spellslots dragon          // Display: {1:3, 2:3, 3:2}
restoreslots dragon 1 3    // Heat Release spell effect
spellslots dragon          // Display: {1:4, 2:3, 3:2}
```

---

### Weeks 11-12: GOComponents (Part 2)

**Goal**: Implement StatusEffects, DamageCalculator, TargetingSystem components

**Files**:

- `CS230/Game/Components/StatusEffects.h/cpp`
- `CS230/Game/Components/DamageCalculator.h/cpp`
- `CS230/Game/Components/TargetingSystem.h/cpp`

**StatusEffects Component:**

- [ ] **Data members**
  
  ```cpp
  private:
      std::vector<StatusEffect> activeEffects;
  ```

- [ ] **Methods**
  
  ```cpp
  void AddEffect(StatusEffect effect);
  void RemoveEffect(StatusEffectType type);
  std::vector<StatusEffect> GetActiveEffects() const;
  bool HasEffect(StatusEffectType type) const;
  ```

**DamageCalculator Component:**

- [ ] **Data members**
  
  ```cpp
  private:
      int baseDefensePower;
      std::string defenseDice;
  ```

- [ ] **Methods**
  
  ```cpp
  int CalculateAttackDamage(int basePower, const std::string& attackDice);
  int CalculateDefense();
  int GetFinalDamage(int incomingDamage);
  ```

**TargetingSystem Component:**

- [ ] **Data members**
  
  ```cpp
  private:
      int attackRange;
      int spellRange;
  ```

- [ ] **Methods**
  
  ```cpp
  bool IsInRange(Character* target, int range) const;
  std::vector<Character*> GetValidTargets(int range) const;
  Math::vec2 GetRangeIndicator() const;  // For debug visualization
  ```

**Dependencies**: Real StatusEffectManager available (Week 11+) - **INTEGRATION POINT**

**Testing**: Apply status effects, test damage calculations, test targeting

---

## Developer 2: Battle Systems (Weeks 1-12)

**Domain**: Core tactical battle mechanics

### Week 1: Stubs (See Above)

### Weeks 2-5: GridSystem

**Goal**: Complete 8x8 grid system with pathfinding and line-of-sight

**Files**:

- `CS230/Game/Systems/GridSystem.h`
- `CS230/Game/Systems/GridSystem.cpp`

**Implementation Tasks:**

- [ ] **Grid data structure**
  
  ```cpp
  private:
      static const int GRID_WIDTH = 8;
      static const int GRID_HEIGHT = 8;
      static const int TILE_SIZE = 64;  // pixels
  
      enum class TileType { Empty, Wall, Lava, Corpse };
      TileType grid[GRID_HEIGHT][GRID_WIDTH];
  
      std::map<Math::vec2, Character*> occupiedTiles;  // Track character positions
  ```

- [ ] **Grid validation**
  
  ```cpp
  bool IsValidPosition(Math::vec2 pos) const;
  bool IsOccupied(Math::vec2 pos) const;
  bool IsValidMove(Math::vec2 from, Math::vec2 to) const;
  TileType GetTileType(Math::vec2 pos) const;
  ```

- [ ] **Pathfinding (A* algorithm)**
  
  ```cpp
  std::vector<Math::vec2> FindPath(Math::vec2 from, Math::vec2 to) const;
  std::vector<Math::vec2> GetReachableTiles(Character* character) const;
  int ManhattanDistance(Math::vec2 a, Math::vec2 b) const;
  ```

- [ ] **Line of sight (Bresenham's line algorithm)**
  
  ```cpp
  bool HasLineOfSight(Math::vec2 from, Math::vec2 to) const;
  std::vector<Math::vec2> GetLineOfSightPath(Math::vec2 from, Math::vec2 to) const;
  ```

- [ ] **Character placement**
  
  ```cpp
  void PlaceCharacter(Character* character, Math::vec2 pos);
  void RemoveCharacter(Math::vec2 pos);
  Character* GetCharacterAt(Math::vec2 pos) const;
  ```

**Dependencies**: Uses stub Character (Weeks 2-4), Real Character (Week 5+) - **INTEGRATION POINT**

**RIGOROUS TESTING (Weeks 2-5: GridSystem)**

**Unit Tests - Grid Validation**:

- [ ] Test_IsValidPosition_InsideGrid()
  
  - Input: IsValidPosition({3, 4})
  - Expected: true
  - Input: IsValidPosition({0, 0})
  - Expected: true
  - Input: IsValidPosition({7, 7})
  - Expected: true

- [ ] Test_IsValidPosition_OutsideGrid()
  
  - Input: IsValidPosition({-1, 0})
  - Expected: false
  - Input: IsValidPosition({8, 8})
  - Expected: false
  - Input: IsValidPosition({3, -5})
  - Expected: false

- [ ] Test_IsOccupied()
  
  - Setup: PlaceCharacter(dragon, {4,4})
  - Input: IsOccupied({4,4})
  - Expected: true
  - Input: IsOccupied({3,3})
  - Expected: false

- [ ] Test_IsValidMove()
  
  - Setup: Empty grid
  - Input: IsValidMove({0,0}, {3,3})
  - Expected: true
  - Setup: Place wall at (2,2)
  - Input: IsValidMove({0,0}, {2,2})
  - Expected: false (destination is wall)

**Unit Tests - Pathfinding (A* Algorithm)**:

- [ ] Test_FindPath_StraightLine()
  
  - Setup: Empty 8x8 grid
  - Input: FindPath({0,0}, {0,7})
  - Expected path: [{0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}]
  - Expected length: 8 tiles

- [ ] Test_FindPath_Diagonal()
  
  - Input: FindPath({0,0}, {7,7})
  - Expected: Path uses only orthogonal moves (no diagonals)
  - Expected length: 14 tiles (Manhattan distance)

- [ ] Test_FindPath_AroundWall()
  
  - Setup: Place vertical wall at x=3: {(3,2), (3,3), (3,4)}
  - Input: FindPath({2,3}, {4,3})
  - Expected: Path routes around wall (north or south)
  - Expected: Path length > 2 (direct would be 2 tiles)
  - Verify: Path does NOT contain any wall tiles

- [ ] Test_FindPath_NoPath()
  
  - Setup: Surround tile (4,4) with walls on all 4 sides
  - Input: FindPath({0,0}, {4,4})
  - Expected: Returns empty vector (no path exists)

- [ ] Test_GetReachableTiles()
  
  - Setup: Character at (4,4) with speed=3
  - Input: GetReachableTiles(character)
  - Expected: Returns all tiles within 3 Manhattan distance
  - Verify: Includes (4,7), (7,4), (1,4), (4,1)
  - Verify: Does NOT include (0,0) (distance=8), (7,7) (distance=6)

- [ ] Test_GetReachableTiles_WithWalls()
  
  - Setup: Character at (4,4), wall at (5,4)
  - Input: GetReachableTiles(character) with speed=3
  - Expected: Tiles beyond (5,4) in that direction NOT reachable
  - Verify: (6,4) NOT in reachable list

**Unit Tests - Line of Sight (Bresenham)**:

- [ ] Test_HasLineOfSight_ClearPath()
  
  - Setup: Empty grid
  - Input: HasLineOfSight({0,0}, {7,7})
  - Expected: true

- [ ] Test_HasLineOfSight_BlockedByWall()
  
  - Setup: Wall at (4,4)
  - Input: HasLineOfSight({3,3}, {5,5})
  - Expected: false (wall blocks diagonal LOS)

- [ ] Test_HasLineOfSight_AdjacentTiles()
  
  - Input: HasLineOfSight({4,4}, {4,5})
  - Expected: true (adjacent always visible)

- [ ] Test_GetLineOfSightPath()
  
  - Input: GetLineOfSightPath({0,0}, {3,3})
  - Expected: Returns Bresenham line: [{0,0}, {1,1}, {2,2}, {3,3}]
  - Verify: All intermediate tiles returned

**Unit Tests - Character Placement**:

- [ ] Test_PlaceCharacter()
  
  - Setup: Empty grid
  - Input: PlaceCharacter(dragon, {4,4})
  - Expected: occupiedTiles[{4,4}] == dragon
  - Expected: IsOccupied({4,4}) == true
  - Expected: GetCharacterAt({4,4}) == dragon

- [ ] Test_PlaceCharacter_OccupiedTile()
  
  - Setup: dragon already at (4,4)
  - Input: PlaceCharacter(fighter, {4,4})
  - Expected: Assert/Log error "Tile already occupied"
  - Expected: dragon remains at (4,4), fighter NOT placed

- [ ] Test_RemoveCharacter()
  
  - Setup: dragon at (4,4)
  - Input: RemoveCharacter({4,4})
  - Expected: IsOccupied({4,4}) == false
  - Expected: GetCharacterAt({4,4}) == nullptr

**Integration Tests - GridSystem + Character**:

- [ ] Test_CharacterMovement()
  - Setup: Dragon at (0,0), empty grid
  - Input: path = FindPath({0,0}, {3,3})
  - Input: Move dragon along path
  - Expected: Dragon ends at (3,3)
  - Expected: IsOccupied({0,0}) == false
  - Expected: IsOccupied({3,3}) == true

**Performance Tests**:

- [ ] Test_PathfindingPerformance()
  - Input: FindPath({0,0}, {7,7}) on empty grid
  - Expected: Completes in < 10ms
  - Verify: A* is efficient for 8x8 grid

**Acceptance Criteria**:

- [ ] Grid correctly validates positions (in-bounds checks)
- [ ] Pathfinding finds shortest path without diagonals
- [ ] Pathfinding routes around walls
- [ ] Line of sight correctly blocked by walls
- [ ] Character placement prevents duplicates on same tile
- [ ] All methods handle edge cases (invalid positions, null characters)

**Console Testing Commands**:

```cpp
// Create test scenario
spawn dragon 4 4
debug grid on

// Test pathfinding visualization
setdest dragon 0 0          // Pathfind to (0,0), highlight path
move dragon 0 0             // Execute movement

// Test wall creation and pathfinding
createwall 3 3              // Place wall
setdest dragon 5 5          // Path should route around wall

// Test reachable tiles
showreach dragon            // Highlight all tiles within movement range

// Test line of sight
spawn fighter 7 7
checklos dragon fighter     // Display: "Line of Sight: YES/NO"
createwall 5 5              // Block LOS
checklos dragon fighter     // Display: "Line of Sight: NO"

// Test occupancy
getoccupancy 4 4            // Display: "Occupied by Dragon"
getoccupancy 0 0            // Display: "Empty"
```

**Key Milestone (Week 5)**: GridSystem fully functional with pathfinding

---

### Weeks 6-8: TurnManager

**Goal**: Initiative-based turn order with action point management

**Files**:

- `CS230/Game/Systems/TurnManager.h`
- `CS230/Game/Systems/TurnManager.cpp`

**Implementation Tasks:**

- [ ] **Turn order data**
  
  ```cpp
  private:
      std::vector<Character*> allCharacters;
      std::vector<Character*> turnOrder;  // Sorted by speed
      int currentTurnIndex;
      int roundNumber;
  ```

- [ ] **Initiative system**
  
  ```cpp
  void InitializeTurnOrder(const std::vector<Character*>& characters);
  void SortBySpeed();  // Sort by character->GetSpeed(), descending
  void AdvanceToNextTurn();
  Character* GetCurrentCharacter() const;
  bool IsPlayerTurn() const;
  ```

- [ ] **Turn phases**
  
  ```cpp
  enum class TurnPhase { StartPhase, ActionPhase, EndPhase };
  TurnPhase currentPhase;
  
  void StartPhase();   // Refresh action points, clear "moved" flags
  void ActionPhase();  // Player/AI takes actions
  void EndPhase();     // Apply status effects (burn damage, etc.)
  ```

- [ ] **Action point management**
  
  ```cpp
  void RefreshActionPoints(Character* character);
  bool CanTakeAction(Character* character) const;
  void EndTurn();  // Force end current character's turn
  ```

**Dependencies**: Real Character available (Week 6+)

**RIGOROUS TESTING (Weeks 6-8: TurnManager)**

**Unit Tests - Turn Order Initialization**:

- [ ] Test_InitializeTurnOrder()
  
  - Setup: 5 characters with speeds: Dragon(5), Fighter(3), Cleric(3), Wizard(2), Rogue(4)
  - Input: InitializeTurnOrder(allCharacters)
  - Expected turn order: [Dragon(5), Rogue(4), Fighter(3), Cleric(3), Wizard(2)]
  - Verify: Sorted by speed descending

- [ ] Test_TurnOrder_TieBreaker()
  
  - Setup: Fighter(speed=3), Cleric(speed=3) - tied!
  - Input: SortBySpeed()
  - Expected: Consistent order (e.g., alphabetical or by creation order)
  - Verify: Order doesn't randomize each round

**Unit Tests - Turn Advancement**:

- [ ] Test_AdvanceToNextTurn()
  
  - Setup: turnOrder = [Dragon, Rogue, Fighter, Wizard], currentTurnIndex = 0
  - Input: AdvanceToNextTurn()
  - Expected: currentTurnIndex == 1
  - Expected: GetCurrentCharacter() == Rogue

- [ ] Test_AdvanceToNextTurn_WrapAround()
  
  - Setup: currentTurnIndex = 3 (last character), roundNumber = 1
  - Input: AdvanceToNextTurn()
  - Expected: currentTurnIndex == 0 (wraps to first character)
  - Expected: roundNumber == 2
  - Verify: New round starts

- [ ] Test_GetCurrentCharacter()
  
  - Setup: turnOrder = [Dragon, Rogue, Fighter], currentTurnIndex = 1
  - Input: GetCurrentCharacter()
  - Expected: Returns pointer to Rogue

**Unit Tests - Turn Phases**:

- [ ] Test_StartPhase()
  
  - Setup: Dragon has currentActionPoints = 0, maxActionPoints = 5
  - Input: StartPhase()
  - Expected: Dragon->currentActionPoints refreshed to 5
  - Expected: Dragon->hasMoved flag cleared
  - Expected: currentPhase == TurnPhase::ActionPhase

- [ ] Test_ActionPhase()
  
  - Setup: currentPhase = ActionPhase
  - Input: Player takes action (move/attack)
  - Expected: CanTakeAction() returns true if AP > 0
  - Expected: CanTakeAction() returns false if AP == 0

- [ ] Test_EndPhase()
  
  - Setup: Dragon has StatusEffect::Burn (2 damage/turn)
  - Input: EndPhase()
  - Expected Actions:
    1. Apply burn damage: Dragon takes 2 damage
    2. Decrement burn duration
    3. Check for character death
    4. AdvanceToNextTurn()

**Unit Tests - Action Point Management**:

- [ ] Test_RefreshActionPoints()
  
  - Setup: Dragon with maxActionPoints=5, currentActionPoints=1
  - Input: RefreshActionPoints(dragon)
  - Expected: dragon->currentActionPoints == 5

- [ ] Test_CanTakeAction()
  
  - Setup: Dragon with currentActionPoints=2
  - Input: CanTakeAction(dragon)
  - Expected: true
  - Setup: Dragon with currentActionPoints=0
  - Input: CanTakeAction(dragon)
  - Expected: false

- [ ] Test_EndTurn()
  
  - Setup: Dragon's turn, currentActionPoints=3 (still has AP remaining)
  - Input: EndTurn()
  - Expected: Advance to next character regardless of remaining AP
  - Expected: Dragon's AP carries over (not consumed)

**Integration Tests - Full Turn Cycle**:

- [ ] Test_CompleteTurnCycle()
  
  - Setup: 3 characters: Dragon(speed=5), Fighter(speed=3), Wizard(speed=2)
  - Execute:
    1. Initialize turn order
    2. Dragon takes turn → AdvanceToNextTurn()
    3. Fighter takes turn → AdvanceToNextTurn()
    4. Wizard takes turn → AdvanceToNextTurn()
    5. Wrap back to Dragon
  - Expected: roundNumber == 2
  - Expected: currentTurnIndex == 0 (Dragon again)

- [ ] Test_CharacterDeath_TurnOrder()
  
  - Setup: turnOrder = [Dragon, Fighter, Cleric, Wizard]
  - Action: Fighter dies during Dragon's turn
  - Expected: Fighter removed from turnOrder
  - Expected: New turnOrder = [Dragon, Cleric, Wizard]
  - Expected: Turn advancement skips dead character

**Acceptance Criteria**:

- [ ] Turn order correctly sorted by speed
- [ ] Turn phases execute in correct sequence (Start → Action → End)
- [ ] Action points refresh at start of each character's turn
- [ ] End phase applies status effects correctly
- [ ] Dead characters removed from turn order
- [ ] Turn wraps correctly after last character

**Console Testing Commands**:

```cpp
// Create test scenario
spawn dragon 4 4
spawn fighter 0 0
spawn wizard 7 7

// Display turn order
turnorder                  // Display: "1. Dragon (speed 5), 2. Fighter (speed 3), 3. Wizard (speed 2)"

// Advance turns manually
nextturn                   // Advance to next character
whosturn                   // Display: "Current turn: Fighter"

// Test action points
getstats dragon            // Display: "AP: 5/5"
move dragon 5 5            // Costs 1 AP
getstats dragon            // Display: "AP: 4/5"
endturn                    // Force end Dragon's turn
nextturn                   // Come back to Dragon
getstats dragon            // Display: "AP: 5/5" (refreshed)

// Test turn phases
setphase start             // Force StartPhase
setphase action            // Force ActionPhase
setphase end               // Force EndPhase (apply status effects)

// Test status effect application
addeffect dragon burn 5    // Add burn effect (5 damage/turn)
endturn                    // End Dragon's turn
// Verify: Dragon takes 5 burn damage at end phase
```

**Key Milestone (Week 8)**: Turn system functional, characters take turns in speed order

---

### Weeks 9-12: CombatSystem

**Goal**: Dice-based damage calculation and attack resolution

**Files**:

- `CS230/Game/Systems/CombatSystem.h`
- `CS230/Game/Systems/CombatSystem.cpp`

**Implementation Tasks:**

- [ ] **Attack validation**
  
  ```cpp
  bool CanAttack(Character* attacker, Character* target) const;
  bool IsInRange(Character* attacker, Character* target, int range) const;
  bool HasLineOfSight(Character* attacker, Character* target) const;
  ```

- [ ] **Damage calculation (D&D style)**
  
  ```cpp
  int CalculateDamage(Character* attacker, Character* target, AttackType type);
  
  // Formula: (basePower + attackDice) - (baseDefense + defenseDice)
  // Example: (5 + 3d6) - (3 + 2d8)
  //   attacker rolls 3d6 = 12
  //   defender rolls 2d8 = 10
  //   damage = (5 + 12) - (3 + 10) = 4 damage
  ```

- [ ] **Attack types**
  
  ```cpp
  enum class AttackType { Melee, Ranged, Spell, True };
  
  void ExecuteMeleeAttack(Character* attacker, Character* target);
  void ExecuteRangedAttack(Character* attacker, Character* target);
  void ExecuteSpellAttack(Character* attacker, Character* target, int spellDamage);
  void ExecuteTrueAttack(Character* attacker, Character* target, int damage);  // Ignores defense
  ```

- [ ] **Damage application**
  
  ```cpp
  void ApplyDamage(Character* target, int damage);
  void ApplyHealing(Character* target, int amount);
  void CheckDeath(Character* target);  // Publish CharacterDiedEvent if HP <= 0
  ```

- [ ] **Knockback (from Dragon spells)**
  
  ```cpp
  void ApplyKnockback(Character* target, Math::vec2 direction, int distance);
  ```

**Dependencies**:

- Real DiceManager (Week 9+) - **INTEGRATION POINT**
- Real GridSystem (already available)

**RIGOROUS TESTING (Weeks 9-12: CombatSystem)**

**Unit Tests - Attack Validation**:

- [ ] Test_CanAttack_InRange()
  
  - Setup: Dragon at (4,4) with attackRange=1, Fighter at (5,4)
  - Input: CanAttack(dragon, fighter)
  - Expected: true (distance = 1 tile)

- [ ] Test_CanAttack_OutOfRange()
  
  - Setup: Dragon at (4,4) with attackRange=1, Wizard at (7,7)
  - Input: CanAttack(dragon, wizard)
  - Expected: false (distance = 6 tiles)

- [ ] Test_CanAttack_BlockedLOS()
  
  - Setup: Dragon at (4,4), Fighter at (6,4), wall at (5,4)
  - Input: CanAttack(dragon, fighter)
  - Expected: false (wall blocks line of sight)

**Unit Tests - Damage Calculation (D&D Formula)**:

- [ ] Test_CalculateDamage_Basic()
  
  - Setup: DiceManager seed fixed (reproducible results)
  - Setup: Attacker: basePower=5, attackDice="3d6"
  - Setup: Defender: baseDefense=3, defenseDice="2d8"
  - Input: CalculateDamage(attacker, defender, AttackType::Melee)
  - Mock dice rolls: attacker rolls 12, defender rolls 10
  - Expected damage: (5 + 12) - (3 + 10) = 4
  - Verify: Formula correct

- [ ] Test_CalculateDamage_MinimumZero()
  
  - Setup: Attacker power very low, defender defense very high
  - Mock rolls: attacker=5, defender=20
  - Expected damage: 0 (NOT negative damage)
  - Verify: Damage floors at 0

- [ ] Test_CalculateDamage_TrueAttack()
  
  - Setup: TrueAttack ignores defense
  - Input: ExecuteTrueAttack(attacker, defender, 50)
  - Expected: Defender takes exactly 50 damage (no defense roll)

**Unit Tests - Attack Types**:

- [ ] Test_ExecuteMeleeAttack()
  
  - Setup: Dragon at (4,4), Fighter at (5,4), Fighter HP=100
  - Input: ExecuteMeleeAttack(dragon, fighter)
  - Expected Actions:
    1. Check CanAttack() → true
    2. CalculateDamage() → e.g., 15 damage
    3. fighter->TakeDamage(15)
    4. EventBus publishes DamageTakenEvent
  - Verify: fighter HP = 85

- [ ] Test_ExecuteRangedAttack()
  
  - Setup: Dragon at (4,4), Wizard at (7,4), Dragon has range=5
  - Input: ExecuteRangedAttack(dragon, wizard)
  - Expected: Same as melee, but range check different

- [ ] Test_ExecuteSpellAttack()
  
  - Setup: Dragon casts Fireball (2d6 damage)
  - Input: ExecuteSpellAttack(dragon, fighter, 12)
  - Expected: Fighter takes 12 damage (bypasses normal attack/defense rolls)

**Unit Tests - Damage Application**:

- [ ] Test_ApplyDamage()
  
  - Setup: Fighter with currentHP=50
  - Input: ApplyDamage(fighter, 20)
  - Expected: fighter->currentHP == 30

- [ ] Test_ApplyDamage_Fatal()
  
  - Setup: Fighter with currentHP=10
  - Input: ApplyDamage(fighter, 50)
  - Expected: fighter->currentHP == 0
  - Expected: CheckDeath() triggers
  - Expected: EventBus publishes CharacterDiedEvent

- [ ] Test_ApplyHealing()
  
  - Setup: Dragon with currentHP=50, maxHP=140
  - Input: ApplyHealing(dragon, 30)
  - Expected: dragon->currentHP == 80

- [ ] Test_ApplyHealing_OverMax()
  
  - Setup: Dragon with currentHP=130, maxHP=140
  - Input: ApplyHealing(dragon, 50)
  - Expected: dragon->currentHP == 140 (NOT 180)

**Unit Tests - Knockback**:

- [ ] Test_ApplyKnockback()
  
  - Setup: Fighter at (4,4), knockback direction = east, distance = 2
  - Input: ApplyKnockback(fighter, {1,0}, 2)
  - Expected: Fighter moves to (6,4)
  - Verify: GridSystem updated

- [ ] Test_ApplyKnockback_Wall()
  
  - Setup: Fighter at (4,4), wall at (5,4), knockback east 2 tiles
  - Input: ApplyKnockback(fighter, {1,0}, 2)
  - Expected: Fighter stops at (4,4) (wall blocks)
  - Alternative: Fighter stops at edge before wall

- [ ] Test_ApplyKnockback_GridEdge()
  
  - Setup: Fighter at (7,4), knockback east 2 tiles
  - Expected: Fighter stops at (7,4) (edge of grid)

**Integration Tests - Full Combat Sequence**:

- [ ] Test_FullCombat_KillEnemy()
  
  - Setup: Dragon (140 HP), Fighter (100 HP)
  - Execute:
    1. Dragon attacks Fighter → Fighter takes damage
    2. Fighter attacks Dragon → Dragon takes damage
    3. Repeat until Fighter HP <= 0
  - Expected: CharacterDiedEvent published
  - Expected: Fighter removed from turn order
  - Expected: Fighter->IsAlive() == false

- [ ] Test_MultiTargetAttack()
  
  - Setup: Dragon casts Fireball on 3 enemies in AOE
  - Input: ExecuteSpellAttack on all 3 targets
  - Expected: All 3 enemies take damage
  - Expected: 3 DamageTakenEvent published

**Performance Tests**:

- [ ] Test_DamageCalculation_Performance()
  - Execute: 1000 damage calculations
  - Expected: Completes in < 100ms
  - Verify: No performance bottlenecks

**Acceptance Criteria**:

- [ ] All attack types work correctly (melee, ranged, spell, true)
- [ ] Damage formula matches D&D specification
- [ ] Damage never goes below 0
- [ ] Knockback respects walls and grid boundaries
- [ ] Death triggers correctly when HP <= 0
- [ ] EventBus receives all combat events

**Console Testing Commands**:

```cpp
// Create combat scenario
spawn dragon 4 4
spawn fighter 5 4
debug grid on

// Test basic attack
attack dragon fighter      // Dragon attacks Fighter
getstats fighter           // Display HP after damage

// Test with fixed dice seed (reproducible)
setdiceseed 12345
attack dragon fighter      // Same seed = same damage every time
attack dragon fighter      // Verify: Same damage as previous

// Test damage formula step-by-step
setdiceseed 100
attack dragon fighter
// Console output:
// "Dragon attacks Fighter!"
// "Attack roll: 5 + 3d6(12) = 17"
// "Defense roll: 3 + 2d8(10) = 13"
// "Damage: 17 - 13 = 4"
// "Fighter HP: 96/100"

// Test knockback
castspell dragon tailswipe 1   // TailSwipe has knockback
getpos fighter                 // Should show new position after knockback

// Test death
setstat fighter currentHP 1    // Set Fighter to 1 HP
attack dragon fighter          // Should kill Fighter
getstats fighter               // Display: "Dead"
turnorder                      // Fighter not in turn order
```

**Key Milestone (Week 12)**: Complete combat system with dice-based damage

---

## Developer 3: AI Systems (Weeks 1-12)

**Domain**: All AI behavior and coordination

### Week 1: Stubs (See Above)

### Weeks 2-6: AIDirector Foundation

**Goal**: Threat assessment and difficulty scaling singleton

**Files**:

- `CS230/Game/Singletons/AIDirector.h`
- `CS230/Game/Singletons/AIDirector.cpp`

**Implementation Tasks:**

- [ ] **Singleton setup**
  
  ```cpp
  class AIDirector {
  public:
      static AIDirector& Instance();
  private:
      AIDirector() = default;
      AIDirector(const AIDirector&) = delete;
      AIDirector& operator=(const AIDirector&) = delete;
  };
  ```

- [ ] **Threat assessment**
  
  ```cpp
  int CalculateThreatLevel(const Character* target) const;
  
  // Formula: (currentHP / maxHP) * attackPower * speedModifier
  // Example: Dragon at 100% HP, 15 avg attack = 100 * 15 * 1.0 = 1500
  ```

- [ ] **Difficulty scaling**
  
  ```cpp
  enum class DifficultyLevel { Easy, Normal, Hard, Nightmare };
  
  void SetDifficultyLevel(DifficultyLevel difficulty);
  DifficultyLevel GetDifficultyLevel() const;
  
  // Difficulty modifiers:
  // Easy: AI makes suboptimal choices 30% of time
  // Normal: AI plays optimally
  // Hard: AI gets +10% damage bonus
  // Nightmare: AI gets +20% damage, +1 action point
  ```

- [ ] **Target prioritization**
  
  ```cpp
  Character* FindHighestThreatEnemy(const std::vector<Character*>& enemies) const;
  Character* FindWeakestEnemy(const std::vector<Character*>& enemies) const;
  Character* FindMostWoundedAlly(const std::vector<Character*>& allies) const;
  ```

**Dependencies**: Uses stub Character (Weeks 2-5), Real Character (Week 6+) - **INTEGRATION POINT**

**RIGOROUS TESTING (Weeks 2-6: AIDirector Threat Assessment)**

**Unit Tests - Threat Calculation**:

- [ ] Test_CalculateThreatLevel_FullHP()
  
  - Setup: Dragon at 140/140 HP, avgAttack=15, speed=5
  - Input: CalculateThreatLevel(dragon)
  - Expected: (140/140) * 15 * 1.0 = 15.0 threat
  - Reasoning: Full HP multiplier = 1.0

- [ ] Test_CalculateThreatLevel_HalfHP()
  
  - Setup: Dragon at 70/140 HP, avgAttack=15, speed=5
  - Input: CalculateThreatLevel(dragon)
  - Expected: (70/140) * 15 * 1.0 = 7.5 threat
  - Reasoning: Wounded targets are lower threat

- [ ] Test_CalculateThreatLevel_HighSpeed()
  
  - Setup: Rogue at 80/80 HP, avgAttack=12, speed=6 (highest speed)
  - Input: CalculateThreatLevel(rogue)
  - Expected: (80/80) * 12 * 1.2 = 14.4 threat
  - Reasoning: Fast characters get speed multiplier

- [ ] Test_CalculateThreatLevel_LowSpeed()
  
  - Setup: Wizard at 60/60 HP, avgAttack=10, speed=2 (lowest speed)
  - Input: CalculateThreatLevel(wizard)
  - Expected: (60/60) * 10 * 0.8 = 8.0 threat
  - Reasoning: Slow characters get speed penalty

**Unit Tests - Difficulty Scaling**:

- [ ] Test_DifficultyLevel_Easy()
  
  - Input: SetDifficultyLevel(DifficultyLevel::Easy)
  - Expected: GetDifficultyLevel() == Easy
  - Verify: AI makes suboptimal choices 30% of time

- [ ] Test_DifficultyLevel_Normal()
  
  - Input: SetDifficultyLevel(DifficultyLevel::Normal)
  - Expected: No modifiers applied
  - Verify: AI plays optimally

- [ ] Test_DifficultyLevel_Hard()
  
  - Input: SetDifficultyLevel(DifficultyLevel::Hard)
  - Expected: AI damage +10%
  - Test: AI attack deals 110 damage instead of 100

- [ ] Test_DifficultyLevel_Nightmare()
  
  - Input: SetDifficultyLevel(DifficultyLevel::Nightmare)
  - Expected: AI damage +20%, +1 action point
  - Test: AI character has 3 AP instead of 2

**Unit Tests - Target Prioritization**:

- [ ] Test_FindHighestThreatEnemy()
  
  - Setup: 3 enemies:
    - Dragon: threat=15.0
    - Wizard: threat=8.0
    - Fighter: threat=10.0
  - Input: FindHighestThreatEnemy(enemies)
  - Expected: Returns Dragon

- [ ] Test_FindWeakestEnemy()
  
  - Setup: 3 enemies with HP: Dragon(140), Fighter(100), Wizard(60)
  - Input: FindWeakestEnemy(enemies)
  - Expected: Returns Wizard (lowest maxHP)

- [ ] Test_FindMostWoundedAlly()
  
  - Setup: 3 allies:
    - Fighter: 80/100 HP (80%)
    - Cleric: 50/80 HP (62.5%)
    - Wizard: 20/60 HP (33%)
  - Input: FindMostWoundedAlly(allies)
  - Expected: Returns Wizard (lowest HP percentage)

**Integration Tests - Threat Tracking**:

- [ ] Test_ThreatTracking_DynamicUpdate()
  - Setup: Dragon starts at 140/140 HP (threat=15)
  - Action: Dragon takes 70 damage → 70/140 HP
  - Input: CalculateThreatLevel(dragon)
  - Expected: threat = 7.5 (updated)
  - Verify: AI re-evaluates target priority

**Acceptance Criteria**:

- [ ] Threat calculation formula matches specification
- [ ] Difficulty modifiers apply correctly
- [ ] Target prioritization selects correct characters
- [ ] Threat updates dynamically as HP changes
- [ ] Singleton pattern implemented correctly

**Console Testing Commands**:

```cpp
// Create test scenario
spawn dragon 4 4
spawn fighter 0 0
spawn cleric 0 7
spawn wizard 7 7

// Test threat calculation
getthreat dragon           // Display: "Dragon threat: 15.0"
damage dragon 70           // Reduce HP to half
getthreat dragon           // Display: "Dragon threat: 7.5"

// Test difficulty scaling
setdifficulty easy         // Set to Easy mode
setdifficulty nightmare    // Set to Nightmare mode
getstats fighter           // Display: "AP: 3/3" (normally 2, nightmare gives +1)

// Test target prioritization
findtarget fighter highest // Display: "Highest threat: Dragon (15.0)"
findtarget fighter weakest // Display: "Weakest enemy: Wizard (60 HP)"

// Test ally selection
damage cleric 50           // Wound Cleric
findally cleric wounded    // Display: "Most wounded: Cleric (62.5% HP)"
```

---

### Weeks 7-9: AIDirector Team Tactics

**Goal**: Team decision coordination and role assignment

**Files**: (Continue `AIDirector.h/cpp`)

**Implementation Tasks:**

- [ ] **Team coordination**
  
  ```cpp
  void CoordinateTeamActions(const std::vector<Character*>& aiTeam);
  
  // Tactical positions:
  // Fighter: Front line, close to dragon
  // Cleric: Behind fighter, 2-3 tiles back
  // Wizard: Back line, 5-6 tiles from dragon
  // Rogue: Flanking position, side/behind dragon
  ```

- [ ] **Role assignment**
  
  ```cpp
  enum class TacticalRole { Tank, Support, DPS, Flanker };
  
  TacticalRole GetRole(const Character* character) const;
  Math::vec2 GetIdealPosition(const Character* character, TacticalRole role) const;
  ```

- [ ] **Team state tracking**
  
  ```cpp
  struct TeamState {
      int totalHP;
      int avgHP;
      int aliveCount;
      bool hasHealer;
      bool tankEngaged;
  };
  
  TeamState AnalyzeTeamState(const std::vector<Character*>& team) const;
  ```

**Dependencies**:

- Real Character (available)
- Uses stub GridSystem (Weeks 7-8), Real GridSystem (Week 9+) - **INTEGRATION POINT**

**Testing**: Test team coordination with 4 AI characters

---

### Weeks 10-12: AISystem Framework

**Goal**: AI decision-making framework for all AI characters

**Files**:

- `CS230/Game/Systems/AISystem.h`
- `CS230/Game/Systems/AISystem.cpp`

**Implementation Tasks:**

- [ ] **Decision structure**
  
  ```cpp
  struct AIDecision {
      ActionType action;      // What to do (Move, Attack, Spell, Heal, etc.)
      Math::vec2 target;      // Where to do it
      int priority;           // Urgency (1-10, 10 = critical)
      std::string reasoning;  // Debug info
  };
  ```

- [ ] **Decision-making framework**
  
  ```cpp
  AIDecision MakeDecision(Character* aiCharacter);
  std::vector<AIDecision> EvaluateAllActions(Character* aiCharacter);
  
  // Process:
  // 1. Get all possible actions (move, attack, spell, heal)
  // 2. Score each action based on situation
  // 3. Return highest priority action
  ```

- [ ] **Action evaluation**
  
  ```cpp
  int ScoreMove(Character* aiCharacter, Math::vec2 targetPos) const;
  int ScoreAttack(Character* aiCharacter, Character* target) const;
  int ScoreHeal(Character* aiCharacter, Character* target) const;
  int ScoreRetreat(Character* aiCharacter) const;
  ```

- [ ] **Placeholder AI logic** (full AI in Weeks 13-22)
  
  ```cpp
  AIDecision FighterAI(Fighter* fighter);   // Basic: Move toward dragon, attack
  AIDecision ClericAI(Cleric* cleric);     // Basic: Heal wounded ally
  AIDecision WizardAI(Wizard* wizard);     // Basic: Stay at range, attack
  AIDecision RogueAI(Rogue* rogue);        // Basic: Flank dragon, attack
  ```

**Dependencies**:

- Real GridSystem (available Week 9+)
- Uses stub CombatSystem (Weeks 10-11), Real CombatSystem (Week 12+) - **INTEGRATION POINT**

**Testing**: Test basic AI decisions with stub characters

**Key Milestone (Week 12)**: AI framework complete, basic AI logic functional

---

## Developer 4: Data & Services (Weeks 1-12)

**Domain**: Singletons, data management, persistence

### Week 1: Stubs (See Above)

### Weeks 2-3: EventBus

**Goal**: Pub/sub event system for decoupled communication

**Files**:

- `CS230/Game/Singletons/EventBus.h`
- `CS230/Game/Singletons/EventBus.cpp`

**Implementation Tasks:**

- [ ] **Singleton setup**
  
  ```cpp
  class EventBus {
  public:
      static EventBus& Instance();
  private:
      EventBus() = default;
      // ... copy/move deleted
  };
  ```

- [ ] **Event subscription (template-based)**
  
  ```cpp
  template<typename EventType>
  void Subscribe(std::function<void(const EventType&)> handler) {
      auto& handlers = eventHandlers[typeid(EventType).name()];
      handlers.push_back([handler](const void* event) {
          handler(*static_cast<const EventType*>(event));
      });
  }
  ```

- [ ] **Event publishing**
  
  ```cpp
  template<typename EventType>
  void Publish(const EventType& event) {
      auto it = eventHandlers.find(typeid(EventType).name());
      if (it != eventHandlers.end()) {
          for (auto& handler : it->second) {
              handler(&event);
          }
      }
  }
  ```

- [ ] **Unsubscribe (optional for Phase 1)**
  
  ```cpp
  template<typename EventType>
  void Unsubscribe(/* handler ID */) { ... }
  ```

**Dependencies**: None (self-contained)

**Testing**: Create test events, subscribe handlers, verify publish works

---

### Weeks 4-5: DiceManager

**Goal**: Centralized dice rolling with seeding and logging

**Files**:

- `CS230/Game/Singletons/DiceManager.h`
- `CS230/Game/Singletons/DiceManager.cpp`

**Implementation Tasks:**

- [ ] **Singleton setup** (same pattern as EventBus)

- [ ] **Dice rolling**
  
  ```cpp
  int RollDice(int count, int sides);
  
  // Example: RollDice(3, 6) rolls 3d6
  // Returns sum of all rolls (3-18 for 3d6)
  ```

- [ ] **String parsing**
  
  ```cpp
  int RollDiceFromString(const std::string& diceNotation);
  
  // Example: "3d6" -> calls RollDice(3, 6)
  // Example: "2d8+5" -> calls RollDice(2, 8) and adds 5
  ```

- [ ] **Seeding and logging**
  
  ```cpp
  void SetSeed(int seed);  // For reproducible tests
  int GetLastRoll() const;
  std::vector<int> GetRollHistory() const;  // For debugging
  
  void LogRoll(const std::string& context, int result);
  // Example: "Fighter attack: 3d6 = 14"
  ```

**Dependencies**: None (self-contained)

**RIGOROUS TESTING (Weeks 4-5: DiceManager)**

**Unit Tests - Basic Dice Rolling**:

- [ ] Test_RollDice_SingleDie()
  
  - Input: RollDice(1, 6)
  - Expected: Result in range [1, 6]
  - Repeat 1000 times
  - Verify: All results within valid range

- [ ] Test_RollDice_MultipleDice()
  
  - Input: RollDice(3, 6)
  - Expected: Result in range [3, 18]
  - Verify: Sum of three individual rolls

- [ ] Test_RollDice_EdgeCases()
  
  - Input: RollDice(0, 6)
  - Expected: 0 or error
  - Input: RollDice(1, 0)
  - Expected: 0 or error
  - Input: RollDice(-1, 6)
  - Expected: Error/assert

**Unit Tests - String Parsing**:

- [ ] Test_RollDiceFromString_Simple()
  
  - Input: "3d6"
  - Expected: Calls RollDice(3, 6)
  - Expected: Result in range [3, 18]

- [ ] Test_RollDiceFromString_WithBonus()
  
  - Input: "2d8+5"
  - Expected: RollDice(2, 8) + 5
  - Example: Roll=10 → Result=15

- [ ] Test_RollDiceFromString_WithPenalty()
  
  - Input: "1d20-2"
  - Expected: RollDice(1, 20) - 2
  - Example: Roll=5 → Result=3

- [ ] Test_RollDiceFromString_InvalidFormat()
  
  - Input: "abc"
  - Expected: Error or 0
  - Input: "3x6" (wrong separator)
  - Expected: Error or 0

**Unit Tests - Seeding and Reproducibility**:

- [ ] Test_SetSeed_Reproducible()
  
  - Input: SetSeed(12345)
  - Input: result1 = RollDice(3, 6)
  - Input: SetSeed(12345)  // Reset same seed
  - Input: result2 = RollDice(3, 6)
  - Expected: result1 == result2
  - Reasoning: Same seed = same sequence

- [ ] Test_RollSequence_Reproducible()
  
  - Input: SetSeed(100)
  - Execute: 10 rolls
  - Input: SetSeed(100)  // Reset
  - Execute: 10 rolls again
  - Expected: Both sequences identical

**Unit Tests - Roll History**:

- [ ] Test_GetLastRoll()
  
  - Input: RollDice(3, 6) → returns 12
  - Input: GetLastRoll()
  - Expected: 12

- [ ] Test_GetRollHistory()
  
  - Execute: Roll 5 times → [3, 15, 8, 12, 6]
  - Input: GetRollHistory()
  - Expected: Returns vector [3, 15, 8, 12, 6]

**Statistical Tests - Distribution**:

- [ ] Test_DiceDistribution_Fair()
  
  - Execute: RollDice(1, 6) 6000 times
  - Expected distribution (each face ~1000 times ± 10%):
    - Face 1: ~1000 times
    - Face 2: ~1000 times
    - ... Face 6: ~1000 times
  - Verify: No significant bias

- [ ] Test_MultipleDice_MeanValue()
  
  - Execute: RollDice(3, 6) 1000 times
  - Expected mean: 10.5 ± 0.5
  - Reasoning: Average of 3d6 = 3 * 3.5 = 10.5

**Unit Tests - Logging**:

- [ ] Test_LogRoll()
  - Input: LogRoll("Fighter attack", 14)
  - Expected: Engine::Logger receives "Fighter attack: 14"
  - Verify: Log file contains entry

**Performance Tests**:

- [ ] Test_RollDice_Performance()
  - Execute: 10,000 rolls
  - Expected: Completes in < 100ms
  - Verify: No performance bottlenecks

**Acceptance Criteria**:

- [ ] Dice rolls produce values in correct range
- [ ] String parsing handles all D&D notation (XdY+Z)
- [ ] Seeding produces reproducible sequences
- [ ] Roll history tracked correctly
- [ ] Statistical distribution is fair (no bias)
- [ ] All edge cases handled (0 dice, invalid strings)

**Console Testing Commands**:

```cpp
// Test basic rolling
rolldice 3d6               // Display: "Rolled 3d6: 12"
rolldice 2d8+5             // Display: "Rolled 2d8+5: 15"
rolldice 1d20-2            // Display: "Rolled 1d20-2: 8"

// Test seeding
setdiceseed 12345
rolldice 3d6               // Display: "Rolled 3d6: 14"
setdiceseed 12345          // Reset seed
rolldice 3d6               // Display: "Rolled 3d6: 14" (same result)

// Test history
rollhistory                // Display last 10 rolls
lastroll                   // Display last roll value

// Test distribution (statistical test)
testdistribution 1d6 1000  // Roll 1d6 1000 times, show distribution
// Expected output:
// "Face 1: 167 times (16.7%)"
// "Face 2: 172 times (17.2%)"
// ... (all around 16.7% ± 2%)
```

**Key Milestone (Week 5)**: DiceManager complete, ready for CombatSystem integration (Week 9)

---

### Weeks 6-10: DataRegistry

**Goal**: JSON data loading with hot-reload support

**Files**:

- `CS230/Game/Singletons/DataRegistry.h`
- `CS230/Game/Singletons/DataRegistry.cpp`

**Implementation Tasks:**

- [ ] **Singleton setup** (same pattern)

- [ ] **JSON loading (use nlohmann/json library)**
  
  ```cpp
  void LoadFromFile(const std::string& filepath);
  void ReloadFile(const std::string& filepath);
  void ReloadAll();
  
  nlohmann::json GetJSON(const std::string& key) const;
  ```

- [ ] **Value retrieval (template-based)**
  
  ```cpp
  template<typename T>
  T GetValue(const std::string& key, const T& defaultValue = T()) const {
      // Example: GetValue<int>("Dragon.maxHP", 100)
      // Returns 140 if "Dragon.maxHP" exists in JSON, else 100
  }
  ```

- [ ] **Nested key access**
  
  ```cpp
  // Example: "Dragon.spellSlots.level3" -> 2
  // Supports dot notation for nested JSON objects
  ```

- [ ] **File watching (for hot-reload)**
  
  ```cpp
  void StartWatching();
  void StopWatching();
  void CheckFileChanges();  // Called in Update()
  
  std::map<std::string, std::time_t> lastModifiedTimes;
  ```

- [ ] **JSON validation**
  
  ```cpp
  bool ValidateJSON(const std::string& filepath);
  // Returns false if syntax errors, prevents crashes
  ```

**Dependencies**: None (self-contained, uses external nlohmann/json library)

**Testing**: Create test JSON files, test loading, test hot-reload

**Key Milestone (Week 10)**: DataRegistry complete, ready for CharacterFactory (Week 11)

---

### Weeks 11-12: CharacterFactory (Start)

**Goal**: Begin data-driven character creation (complete in Weeks 13-14)

**Files**:

- `CS230/Game/Factories/CharacterFactory.h`
- `CS230/Game/Factories/CharacterFactory.cpp`

**Implementation Tasks (Weeks 11-12):**

- [ ] **Factory interface**
  
  ```cpp
  class CharacterFactory {
  public:
      static Character* CreateCharacter(CharacterType type, Math::vec2 gridPos);
  private:
      static Dragon* CreateDragon(Math::vec2 gridPos);
      static Fighter* CreateFighter(Math::vec2 gridPos);
      static Cleric* CreateCleric(Math::vec2 gridPos);
      static Wizard* CreateWizard(Math::vec2 gridPos);
      static Rogue* CreateRogue(Math::vec2 gridPos);
  };
  ```

- [ ] **Dragon factory (Week 11-12)**
  
  ```cpp
  Dragon* CharacterFactory::CreateDragon(Math::vec2 gridPos) {
      Dragon* dragon = new Dragon(gridPos);
  
      // Load stats from DataRegistry
      dragon->maxHP = DataRegistry::Instance().GetValue<int>("Dragon.maxHP", 140);
      dragon->speed = DataRegistry::Instance().GetValue<int>("Dragon.speed", 5);
      // ... load all other stats
  
      // Attach components
      dragon->AddGOComponent(new GridPosition(gridPos.x, gridPos.y));
      dragon->AddGOComponent(new ActionPoints(dragon->maxActionPoints));
      dragon->AddGOComponent(new SpellSlots(dragon->spellSlots));
  
      return dragon;
  }
  ```

**Dependencies**:

- Real DataRegistry (available Week 10+)
- Real Character (available Week 5+)

**Testing**: Test Dragon creation from JSON data

**Note**: Full CharacterFactory (all 5 character types) completed in Weeks 13-14

---

## Developer 5: Debug & Tools (Weeks 1-12)

**Domain**: Debug tools, testing infrastructure, integration

### Week 1: Stubs (See Above)

### Weeks 2-4: DebugManager + DebugConsole

**Goal**: Core debug infrastructure with console command system

**Files**:

- `CS230/Game/Debug/DebugManager.h/cpp`
- `CS230/Game/Debug/DebugConsole.h/cpp`

**DebugManager Implementation:**

- [ ] **Singleton with debug toggles**
  
  ```cpp
  class DebugManager {
  public:
      static DebugManager& Instance();
  
      void SetDebugMode(bool enabled);
      bool IsDebugMode() const;
  
      void ToggleGridOverlay();
      void ToggleCollisionBoxes();
      void ToggleAIPathVisualization();
      void ToggleStatusInfo();
  };
  ```

**DebugConsole Implementation:**

- [ ] **Console UI**
  
  ```cpp
  class DebugConsole : public CS230::Component {
  public:
      void Update(double dt) override;
      void Draw(Math::TransformationMatrix camera_matrix) override;
  
      void ToggleConsole();  // Tilde (~) key
      bool IsOpen() const;
  };
  ```

- [ ] **Command registration**
  
  ```cpp
  void RegisterCommand(
      const std::string& name,
      std::function<void(std::vector<std::string>)> handler,
      const std::string& helpText
  );
  
  void ExecuteCommand(const std::string& commandLine);
  ```

- [ ] **Input handling**
  
  ```cpp
  void ProcessInput();       // Handle text input
  void AddOutput(const std::string& text, ConsoleColor color);
  void AutoComplete();       // Tab completion
  
  std::string currentInput;
  std::vector<std::string> commandHistory;
  int historyIndex;
  ```

- [ ] **Basic commands (Week 2-4)**
  
  ```cpp
  // Register these commands:
  help                  // Show all commands
  help <command>        // Show detailed help
  clear                 // Clear console output
  log <message>         // Write to game log
  listcommands          // List all registered commands
  ```

**Dependencies**: None (self-contained for now)

**Testing**: Open console, test command registration, test input handling

**Key Milestone (Week 4)**: Console functional, ready for game-specific commands

---

### Weeks 5-7: HotReloadManager

**Goal**: File watching and auto-reload for JSON data

**Files**:

- `CS230/Game/Debug/HotReloadManager.h/cpp`

**Implementation Tasks:**

- [ ] **File watching**
  
  ```cpp
  class HotReloadManager : public CS230::Component {
  public:
      void Update(double dt) override;  // Check for file changes
  
      void StartWatching();
      void StopWatching();
      void SetAutoReload(bool enabled);
  };
  ```

- [ ] **File change detection**
  
  ```cpp
  struct FileWatchData {
      std::string filepath;
      std::time_t lastModified;
  };
  std::vector<FileWatchData> watchedFiles;
  
  void CheckFileChanges();
  void OnFileChanged(const std::string& filepath);
  ```

- [ ] **JSON validation and reload**
  
  ```cpp
  bool ValidateJSON(const std::string& filepath);
  void ReloadFile(const std::string& filepath);
  void ShowReloadNotification(const std::string& filename);
  ```

**Dependencies**: Real DataRegistry (Week 6+) - **INTEGRATION POINT**

**Testing**: Modify JSON file while game running, verify auto-reload

**Key Milestone (Week 7)**: Hot-reload functional, enables rapid data iteration

---

### Weeks 8-10: GridDebugRenderer

**Goal**: Visual grid overlay for debugging pathfinding

**Files**:

- `CS230/Game/Debug/GridDebugRenderer.h/cpp`

**Implementation Tasks:**

- [ ] **Grid visualization**
  
  ```cpp
  class GridDebugRenderer : public CS230::Component {
  public:
      void Draw(Math::TransformationMatrix camera_matrix) override;
  
      void SetShowGrid(bool show);
      void SetShowCoordinates(bool show);
      void SetShowOccupancy(bool show);
  };
  ```

- [ ] **Drawing methods**
  
  ```cpp
  void DrawGridLines();          // 8x8 grid lines
  void DrawTileCoordinates();    // (0,0) to (7,7) labels
  void DrawOccupancyOverlay();   // Red = occupied, green = empty
  void DrawMovementRangeOverlay();  // Blue tiles = reachable
  void DrawAttackRangeOverlay();    // Orange tiles = attack range
  ```

- [ ] **Keyboard shortcuts**
  
  ```cpp
  // F1: Toggle grid overlay
  // F2: Toggle coordinate display
  // F3: Toggle occupancy highlighting
  ```

**Dependencies**: Real GridSystem (Week 9+) - **INTEGRATION POINT**

**Testing**: Enable grid overlay, verify visualization matches GridSystem state

---

### Weeks 11-12: Console Commands (Basic)

**Goal**: Implement basic game manipulation commands

**Commands to Implement:**

- [ ] **Character manipulation**
  
  ```cpp
  spawn <type> <x> <y>          // Spawn character at grid position
  kill <characterName>          // Instantly kill a character
  teleport <name> <x> <y>       // Move character to position
  heal <name> <amount>          // Heal character
  damage <name> <amount>        // Damage character
  setstat <name> <stat> <value> // Modify character stat
  ```

- [ ] **Debug visualization**
  
  ```cpp
  debug grid <on/off>           // Toggle grid overlay
  debug collision <on/off>      // Toggle collision boxes
  showall                       // Enable all debug visuals
  hideall                       // Disable all debug visuals
  ```

**Dependencies**:

- Real CharacterFactory (Week 11+) - **INTEGRATION POINT**
- Real Characters (available Week 5+)

**Testing**: Test each command via console, verify effects

---

## Integration Checkpoints

**Weekly integration meetings** (30 minutes each Friday):

### Week 5 Integration

**Developer 1 → Developer 2**:

- Character base class complete
- GridSystem can now use real Character methods
- **Test**: Create Character, place on grid, test pathfinding

### Week 6 Integration

**Developer 1 → Developer 3**:

- Character complete with stats
- AIDirector can use real Character for threat assessment
- **Test**: Calculate threat for different character states

### Week 8 Integration

**Developer 1 → Developer 2**:

- Dragon complete with spells
- SpellSystem interface defined (even if stub)
- **Test**: Dragon can call spell methods (may use stub SpellSystem)

### Week 9 Integration

**Developer 2 → Developer 4**:

- GridSystem complete
- Developer 1's GOComponents can use real GridSystem
- **Test**: GridPosition component syncs with GridSystem

**Developer 4 → Developer 2**:

- DiceManager complete
- CombatSystem can use real DiceManager
- **Test**: Damage calculation with real dice rolls

### Week 10 Integration

**Developer 4 → Developer 5**:

- DataRegistry complete
- HotReloadManager can use real DataRegistry
- **Test**: Modify JSON, verify hot-reload works

**Developer 5 → All**:

- GridDebugRenderer complete
- All developers can use grid visualization for debugging
- **Test**: Verify grid overlay displays correctly

### Week 11 Integration

**Developer 4 → Developer 1**:

- CharacterFactory starts (Dragon factory functional)
- Characters can be created from JSON data
- **Test**: Create Dragon via factory, verify stats loaded from JSON

**Developer 5 → Developer 1**:

- Console commands can spawn/manipulate characters
- **Test**: `spawn dragon 3 7`, `heal dragon 50`, etc.

### Week 12 Integration

**Developer 2 → All**:

- CombatSystem complete
- All combat interactions now use real damage calculation
- **Test**: Dragon attacks test character, verify damage formula

**Developer 3 → All**:

- AISystem framework complete
- Basic AI decisions functional
- **Test**: AI character makes move/attack decision

---

## Phase 1 Deliverables (End of Week 12)

### Functional Systems

✅ **Character Systems** (Developer 1):

- Character base class complete
- Dragon fully playable with 9 spells
- 6 GOComponents implemented (GridPosition, ActionPoints, SpellSlots, StatusEffects, DamageCalculator, TargetingSystem)

✅ **Battle Systems** (Developer 2):

- GridSystem with pathfinding and LOS
- TurnManager with initiative order
- CombatSystem with dice-based damage

✅ **AI Systems** (Developer 3):

- AIDirector with threat assessment
- AISystem framework
- Basic AI decision logic

✅ **Data & Services** (Developer 4):

- EventBus pub/sub system
- DiceManager with seeding
- DataRegistry with hot-reload
- CharacterFactory (Dragon complete, others in progress)

✅ **Debug & Tools** (Developer 5):

- DebugManager + DebugConsole
- HotReloadManager
- GridDebugRenderer
- Basic console commands

### Playable Prototype

**By end of Week 12**, you should have:

1. Dragon character fully playable
2. Grid-based movement and pathfinding
3. Turn-based system with action points
4. Combat system with dice rolls
5. Basic AI opponents (using placeholder logic)
6. Debug console with 10+ commands
7. Hot-reload for rapid iteration

### End-to-End Testing Scenarios (Phase 1 Validation)

**Test Scenario 1: Basic Character Creation and Movement**

- [ ] **Setup**: Empty 8x8 grid, debug mode on
- [ ] Execute: `spawn dragon 4 4`
- [ ] Verify: Dragon appears at tile (4,4), HP=140/140, AP=5/5
- [ ] Execute: `debug grid on`
- [ ] Verify: Grid overlay shows 8x8 grid with coordinates
- [ ] Execute: `showreach dragon`
- [ ] Verify: Tiles within 5 movement range highlighted (Manhattan distance)
- [ ] Execute: `move dragon 7 7`
- [ ] Verify: Dragon pathfinds to (7,7), AP consumed = 6 (distance)
- [ ] Expected Result: Dragon at (7,7), AP=0/5 (or error if insufficient AP)

**Test Scenario 2: Spell Casting and Damage**

- [ ] **Setup**: Dragon at (4,4), Fighter at (6,4)
- [ ] Execute: `spellslots dragon`
- [ ] Verify: Display shows {1:4, 2:3, 3:2}
- [ ] Execute: `castspell dragon fireball 1`
- [ ] Verify Actions:
  1. Target selection UI appears (2x2 AOE around Fighter)
  2. Fireball deals 2d6 damage to all enemies in AOE
  3. Visual effect plays (fire animation)
  4. Console log: "Dragon cast Fireball (Level 1)! Dealt X damage."
  5. Fighter HP reduced
  6. Spell slot consumed: {1:3, 2:3, 3:2}
- [ ] Execute: `castspell dragon fireball 3` (upcast)
- [ ] Verify: Damage increased (4d6 instead of 2d6), spell slot {3:1}
- [ ] Expected Result: Fireball works at base level and upcast level

**Test Scenario 3: Turn-Based Combat**

- [ ] **Setup**: Dragon (speed=5) at (4,4), Fighter (speed=3) at (0,0), Wizard (speed=2) at (7,7)
- [ ] Execute: `turnorder`
- [ ] Verify: Display "1. Dragon (5), 2. Fighter (3), 3. Wizard (2)"
- [ ] Execute: Dragon's turn - move and attack
  - Move to (5,5), attack Fighter (if in range)
  - AP consumed for movement and attack
- [ ] Execute: `nextturn`
- [ ] Verify: Fighter's turn begins, AP refreshed to max
- [ ] Execute: Fighter's turn - AI makes decision
  - AI evaluates: move toward Dragon, attack if in range
  - Action executes automatically
- [ ] Execute: `nextturn`
- [ ] Verify: Wizard's turn, AP refreshed
- [ ] Execute: `nextturn`
- [ ] Verify: Round 2 starts, Dragon's turn again, AP=5/5
- [ ] Expected Result: Turn order correct, AP refreshes each turn

**Test Scenario 4: Combat with Dice-Based Damage**

- [ ] **Setup**: Dragon at (4,4), Fighter at (5,4)
- [ ] Execute: `setdiceseed 12345` (reproducible results)
- [ ] Execute: `attack dragon fighter`
- [ ] Verify Console Output:
  - "Dragon attacks Fighter!"
  - "Attack roll: 5 (base) + 3d6(12) = 17"
  - "Defense roll: 3 (base) + 2d8(10) = 13"
  - "Damage: 17 - 13 = 4"
  - "Fighter HP: 96/100"
- [ ] Execute: `setdiceseed 12345` (reset)
- [ ] Execute: `attack dragon fighter`
- [ ] Verify: Identical damage result (same seed = same rolls)
- [ ] Expected Result: Damage formula correct, seeding works

**Test Scenario 5: Character Death and Turn Order Update**

- [ ] **Setup**: Dragon at (4,4), Fighter at (5,4) with 10 HP
- [ ] Execute: `setstat fighter currentHP 10`
- [ ] Execute: `attack dragon fighter`
- [ ] Verify Actions:
  1. Fighter takes damage >= 10 → HP = 0
  2. Fighter->Die() called
  3. EventBus publishes CharacterDiedEvent
  4. Console log: "Fighter has died!"
  5. Fighter removed from turn order
  6. Fighter sprite changes (death animation or gray out)
- [ ] Execute: `turnorder`
- [ ] Verify: Fighter NOT in turn order
- [ ] Execute: `getstats fighter`
- [ ] Verify: Display "Fighter (DEAD), HP: 0/100"
- [ ] Expected Result: Death handled correctly, turn order updated

**Test Scenario 6: Hot-Reload JSON Data**

- [ ] **Setup**: Dragon spawned, game running
- [ ] Execute: `getstats dragon`
- [ ] Verify: Display "HP: 140/140, Speed: 5, AP: 5/5"
- [ ] Action: Edit `Assets/Data/Dragon.json` while game running
  - Change maxHP from 140 to 200
  - Change speed from 5 to 7
- [ ] Wait: HotReloadManager detects file change (~1 second)
- [ ] Verify: Console notification "Dragon.json reloaded"
- [ ] Execute: `spawn dragon 0 0` (new dragon with updated stats)
- [ ] Execute: `getstats dragon`
- [ ] Verify: Display "HP: 200/200, Speed: 7, AP: 5/5"
- [ ] Expected Result: JSON hot-reload works without restart

**Test Scenario 7: AI Decision Making (Basic)**

- [ ] **Setup**: Dragon at (4,4), AI Fighter at (0,0)
- [ ] Execute: Fighter's turn (AI controlled)
- [ ] Verify AI Decision Process:
  1. AISystem::MakeDecision(fighter) called
  2. Evaluates possible actions: Move toward Dragon, Attack (if in range), Heal (if wounded)
  3. Selects highest priority action
  4. Console log: "Fighter (AI) decides: Move to (1,1)" (moving toward Dragon)
  5. Action executes
- [ ] Expected Result: AI makes logical decision (moves toward enemy)

**Test Scenario 8: Status Effects Application**

- [ ] **Setup**: Dragon at (4,4) with Burn status effect (5 damage/turn)
- [ ] Execute: `addeffect dragon burn 5`
- [ ] Verify: Dragon has burn icon/indicator
- [ ] Execute: Dragon takes turn, then `endturn`
- [ ] Verify Actions at EndPhase:
  1. StatusEffectManager applies burn damage
  2. Dragon takes 5 damage
  3. Console log: "Dragon takes 5 burn damage. HP: 135/140"
  4. Burn duration decrements
- [ ] Expected Result: Status effects apply at turn end

**Test Scenario 9: Grid Pathfinding with Walls**

- [ ] **Setup**: Dragon at (4,4), wall at (5,4), destination (6,4)
- [ ] Execute: `createwall 5 4`
- [ ] Execute: `move dragon 6 4`
- [ ] Verify Actions:
  1. GridSystem::FindPath((4,4), (6,4)) called
  2. A* algorithm detects wall at (5,4)
  3. Path routes around wall: (4,4) → (4,5) → (5,5) → (6,5) → (6,4)
  4. Dragon follows path (if enough AP)
- [ ] Execute: `debug grid on` during movement
- [ ] Verify: Path highlighted, Dragon moves along path
- [ ] Expected Result: Pathfinding routes around obstacles

**Test Scenario 10: Console Command Edge Cases**

- [ ] Execute: `spawn dragon -1 -1`
- [ ] Verify: Error "Invalid grid position"
- [ ] Execute: `spawn dragon 10 10`
- [ ] Verify: Error "Position out of bounds"
- [ ] Execute: `heal dragon 9999`
- [ ] Verify: Dragon HP = 140 (max, not 9999)
- [ ] Execute: `damage dragon 9999`
- [ ] Verify: Dragon HP = 0, Dragon dies
- [ ] Execute: `castspell dragon fireball 5`
- [ ] Verify: Error "No spell slot for level 5"
- [ ] Execute: `spawn invalidtype 4 4`
- [ ] Verify: Error "Unknown character type"
- [ ] Expected Result: All edge cases handled gracefully with error messages

**Acceptance Criteria for Phase 1 Completion:**

- [ ] All 10 test scenarios pass without crashes
- [ ] All console commands work as specified
- [ ] Debug visualizations display correctly
- [ ] JSON hot-reload works reliably
- [ ] AI makes basic logical decisions
- [ ] Turn system advances correctly
- [ ] Combat damage formula matches specification
- [ ] Pathfinding routes around obstacles
- [ ] Status effects apply correctly
- [ ] Character death handled properly

**Performance Benchmarks:**

- [ ] Pathfinding (empty grid, 0,0 → 7,7): < 10ms
- [ ] Damage calculation (1000 rolls): < 100ms
- [ ] Turn advancement: < 5ms
- [ ] JSON file reload: < 100ms
- [ ] Frame rate: Maintains 30 FPS with all systems active

---

## Next Steps (Weeks 13-26)

See **[implementation-plan.md](../implementation-plan.md)** for full 26-week timeline.

**Weeks 13-20**: Advanced features (complete SpellSystem, complete AI behaviors, more debug tools)

**Weeks 21-26**: Polish, integration, final testing

---

**Related Documentation**:

- **[docs/class-list.md](../class-list.md)** - Complete class reference
- **[docs/implementation-plan.md](../implementation-plan.md)** - Full 26-week timeline
- **[docs/architecture.md](../architecture.md)** - Architecture overview

**Last Updated**: 2025-10-04
