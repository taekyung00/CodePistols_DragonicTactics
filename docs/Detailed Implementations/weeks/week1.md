# Playtest 1 Implementation Plan - Week 1

**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Foundation Layer + Dragon Playable + First Playtest
**Timeline**: Weeks 1-5 (First playtest milestone)
**Strategy**: Task-based parallel development - all 5 developers work together on critical path

**Last Updated**: 2025-10-15
**Implementation Status Updated**: 2025-10-15

**Related Documentation**: See [docs/implementation-plan.md](../implementation-plan.md) for complete 26-week timeline

---

## ⚡ IMPLEMENTATION STATUS (As of 2025-10-15)

**Overall Week 1 Completion: ~75%** (3.75 / 5 systems complete)

### Quick Status Overview

| System           | Developer | Status             | Completion | Notes                                                          |
| ---------------- | --------- | ------------------ | ---------- | -------------------------------------------------------------- |
| **EventBus**     | C         | ✅ Complete         | 100%       | Perfect implementation, fully tested, production-ready         |
| **DiceManager**  | D         | ✅ Complete         | 100%       | Full D&D notation support, robust error handling               |
| **GridSystem**   | B         | ✅ Nearly Complete  | 95%        | Needs `Character*` instead of `MockCharacter*`                 |
| **Character**    | A         | ⚠️ Mostly Complete | 85%        | Missing: state machine integration, EventBus publishing, Die() |
| **DebugConsole** | E         | ❌ Not Started      | 0%         | **CRITICAL BLOCKER** for Week 2                                |

### What Works Great ✅

1. **EventBus** - Zero coupling, type-safe templates, 15+ event types defined, 5 passing tests
2. **DiceManager** - Parses "3d6+2", handles edge cases, seed control, GetLastRolls() for debugging
3. **GridSystem** - 8×8 grid, TileType enum, IsValidTile/IsWalkable/IsOccupied all working
4. **Components** - GridPosition, ActionPoints, SpellSlots, StatsComponent all implemented and tested
5. **Characters** - Dragon (250 HP, 3d8 attack) and Fighter (90 HP, 2d6 attack) classes working
6. **Test Infrastructure** - Week1TestMocks.h with MockCharacter, MockLogger, ASSERT macros

### What Needs Work ⚠️

1. **Character State Machine** (2-3 hours) - States defined but not integrated into Character class
2. **EventBus Integration** (1-2 hours) - TakeDamage/Die should publish events
3. **DebugConsole** (4-6 hours) - **HIGHEST PRIORITY** - Blocks Week 2 GridDebugRenderer

### Remaining Work: ~10-15 hours

**Recommended Timeline to 100%:**

- Day 1 (4 hrs): State machine + DebugConsole structure
- Day 2 (4 hrs): EventBus integration + DebugConsole commands
- Day 3 (2 hrs): Integration testing + bug fixes

**Detailed Analysis**: See REPORT.md section below for complete file-by-file assessment

---

## Overview

Playtest 1 delivers the **first playable milestone** (Dragon vs Fighter battle) in just 5 weeks using aggressive task-based parallelization.

**Critical Success Criteria:**

- ✅ **Week 1**: 5 core systems built in parallel (Character, GridSystem, EventBus, DiceManager, DebugConsole)
- ✅ **Week 2**: Dragon playable with movement
- ✅ **Week 3**: Combat + Fireball spell working
- ✅ **Week 4**: All 3 Dragon spells + Fighter enemy
- ✅ **Week 5**: Dragon vs Fighter battle → **PLAYTEST 1** 🧪

**Playtest 1 Goal (Week 5):**

- Dragon can move on 8x8 grid
- Dragon can attack
- Dragon can cast 3 spells (Fireball, CreateWall, LavaPool)
- Fighter enemy can attack back (manual control)
- Turn-based combat works (initiative, turn order)
- Play for 5 minutes without crash

**Meeting Schedule (Weeks 1-5):**

- **Daily Standups**: 10 min each morning (quick blocker resolution)
- **Weekly Integration Meetings**: Friday 45 min (demos + planning)
- **Playtest 1**: Week 5 Friday PM (90 min - full team playthrough)

---

## Table of Contents

- [Week 1: Foundation (All 5 Systems in Parallel)](#week-1-foundation-all-5-systems-in-parallel)
  - [Developer A: Character Base Class](#week-1-developer-a-character-base-class)
  - [Developer B: GridSystem Foundation](#week-1-developer-b-gridsystem-foundation)
  - [Developer C: EventBus Singleton](#week-1-developer-c-eventbus-singleton)
  - [Developer D: DiceManager Singleton](#week-1-developer-d-dicemanager-singleton)
  - [Developer E: DebugConsole Framework](#week-1-developer-e-debugconsole-framework)
  - [Week 1 Deliverable & Verification](#week-1-deliverable--verification)

---

## Week 1: Foundation (All 5 Systems in Parallel)

**Goal**: Build 5 core systems simultaneously - all real implementations, no stubs!

**Philosophy**: Instead of creating stubs → real implementations, we build everything REAL from Day 1. All 5 developers work in parallel on independent systems.

**End of Week 1**: All 5 core systems **fully implemented and functional**

### **🔧 Independent Testing Strategy (CRITICAL FOR WEEK 1)**

**Problem**: Circular dependencies prevent independent testing - EventBus needs Character, GridSystem needs Character, DebugConsole needs Logger, etc.

**Solution**: Use **Mock Classes** for independent testing, then replace with real implementations for integration testing.

#### **Shared Test Infrastructure**

Create this file **FIRST** (before any developer starts testing):

```cpp
// File: CS230/Game/Test/Week1TestMocks.h
#pragma once
#include "../Engine/Vec2.h"
#include <string>
#include <vector>
#include <iostream>

// Mock Character for EventBus and GridSystem testing
class MockCharacter {
public:
    MockCharacter(const std::string& name = "MockChar")
        : name(name), hp(100), maxHP(100), gridPos{0, 0} {}

    // EventBus interface
    std::string TypeName() const { return name; }
    int GetCurrentHP() const { return hp; }
    int GetMaxHP() const { return maxHP; }
    void SetHP(int newHP) { hp = newHP; }

    // GridSystem interface
    Math::vec2 GetGridPosition() const { return gridPos; }
    void SetGridPosition(Math::vec2 pos) { gridPos = pos; }

private:
    std::string name;
    int hp, maxHP;
    Math::vec2 gridPos;
};

// Mock Logger for DebugConsole testing
class MockLogger {
public:
    void LogEvent(const std::string& msg) { events.push_back(msg); }
    void LogError(const std::string& msg) { errors.push_back(msg); }
    void LogDebug(const std::string& msg) { debug.push_back(msg); }

    std::vector<std::string> GetEvents() const { return events; }
    std::vector<std::string> GetErrors() const { return errors; }
    void Clear() { events.clear(); errors.clear(); debug.clear(); }

private:
    std::vector<std::string> events, errors, debug;
};

// Test assertion macros (lightweight)
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cout << "❌ ASSERT_TRUE failed: " << #condition << std::endl; \
        return false; \
    }

#define ASSERT_FALSE(condition) \
    if ((condition)) { \
        std::cout << "❌ ASSERT_FALSE failed: " << #condition << std::endl; \
        return false; \
    }

#define ASSERT_EQ(actual, expected) \
    if ((actual) != (expected)) { \
        std::cout << "❌ ASSERT_EQ failed: " << #actual << " = " << (actual) \
                  << ", expected " << (expected) << std::endl; \
        return false; \
    }

#define ASSERT_NE(actual, expected) \
    if ((actual) == (expected)) { \
        std::cout << "❌ ASSERT_NE failed: " << #actual << " = " << (actual) \
                  << ", expected NOT " << (expected) << std::endl; \
        return false; \
    }

#define ASSERT_GE(actual, minimum) \
    if ((actual) < (minimum)) { \
        std::cout << "❌ ASSERT_GE failed: " << #actual << " = " << (actual) \
                  << ", expected >= " << (minimum) << std::endl; \
        return false; \
    }

#define ASSERT_LE(actual, maximum) \
    if ((actual) > (maximum)) { \
        std::cout << "❌ ASSERT_LE failed: " << #actual << " = " << (actual) \
                  << ", expected <= " << (maximum) << std::endl; \
        return false; \
    }
```

#### **Testing Phases**

**Phase 1 (Days 1-4): Individual Testing**

- Each developer tests their system with mocks **independently**
- No waiting for other developers to finish
- Immediate feedback and iteration

**Phase 2 (Day 5): Integration Testing**

- Replace mocks with real implementations
- Test full system integration
- Fix any interface mismatches

#### **Developer-Specific Mock Usage**

**Developer A (Character)**: ✅ **No dependencies** - test directly as written

**Developer B (GridSystem)**: Use `MockCharacter` instead of real Character class

```cpp
// Example test with MockCharacter
bool Test_GridSystem_Independent() {
    GridSystem grid;
    MockCharacter character("TestChar");

    grid.PlaceCharacter(&character, {4, 4});
    ASSERT_TRUE(grid.IsOccupied({4, 4}));
    ASSERT_EQ(grid.GetCharacterAt({4, 4}), &character);
    return true;
}
```

**Developer C (EventBus)**: Use `MockCharacter` for event testing

```cpp
// Example EventBus test with MockCharacter
bool Test_EventBus_Independent() {
    EventBus::Instance().Clear();

    MockCharacter victim("Dragon");
    MockCharacter attacker("Fighter");

    bool received = false;
    int receivedDamage = 0;

    EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
        received = true;
        receivedDamage = e.damageAmount;
    });

    CharacterDamagedEvent event{&victim, 42, 58, &attacker, true};
    EventBus::Instance().Publish(event);

    ASSERT_TRUE(received);
    ASSERT_EQ(receivedDamage, 42);
    return true;
}
```

**Developer D (DiceManager)**: ✅ **No dependencies** - test directly as written

**Developer E (DebugConsole)**: Use `MockLogger` for testing

```cpp
// Example DebugConsole test with MockLogger
bool Test_DebugConsole_Independent() {
    DebugConsole console;
    MockLogger logger;

    console.RegisterCommand("test",
        [&logger](std::vector<std::string> args) {
            logger.LogEvent("Test command executed");
        },
        "Test command");

    console.ExecuteCommand("test");

    auto events = logger.GetEvents();
    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events[0], "Test command executed");
    return true;
}
```

#### **Benefits of This Approach**

1. **🚀 No Blocking**: Each developer can test immediately without waiting
2. **⚡ Faster Iteration**: Test your system as you build it
3. **🔍 Clear Interfaces**: Mocks define exactly what each system expects
4. **🛠️ Easier Debugging**: Isolate problems to specific systems
5. **📋 Better Planning**: Integration testing reveals interface mismatches early

#### **Week 1 Testing Schedule**

- **Monday-Thursday**: Individual testing with mocks
- **Friday Morning**: Replace mocks with real implementations
- **Friday Afternoon**: Integration testing and fixes

**IMPORTANT**: Every developer should have their system working with mocks before Friday!

---

### Week 1: Developer A - Character Base Class

**Goal**: Complete Character base class with full functionality

**Files to Create**:

```
CS230/Game/Characters/Character.h
CS230/Game/Characters/Character.cpp
CS230/Game/GameObjectTypes.h (update with new character types)
```

**Implementation Tasks**:

- [ ] **Character data members**
  
  ```cpp
  protected:
      int maxHP;
      int currentHP;
      int speed;
      int maxActionPoints;
      int currentActionPoints;
      int baseAttackPower;
      std::string attackDice;          // "3d6"
      int baseDefensePower;
      std::string defenseDice;         // "2d8"
      int attackRange;
      std::map<int, int> spellSlots;   // level -> count
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
  int GetActionPoints() const;
  void ConsumeActionPoints(int cost);
  void RefreshActionPoints();
  int GetMovementRange() const;
  int GetSpeed() const;
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
  
  class State_WaitingTurn : public State { /* ... */ };
  class State_Acting : public State { /* ... */ };
  class State_Dead : public State { /* ... */ };
  ```

- [ ] **Add to GameObjectTypes.h**
  
  ```cpp
  enum class GameObjectTypes {
      // Existing types...
      Dragon,
      Fighter,
      Cleric,
      Wizard,
      Rogue
  };
  ```

**Rigorous Testing**:

**Test Suite 1: Health System**

- [ ] **Test_TakeDamage_Normal()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.maxHP = 100;
  character.currentHP = 100;
  
  // Action
  character.TakeDamage(30);
  
  // Assertions
  ASSERT_EQ(character.GetCurrentHP(), 70);
  ASSERT_EQ(character.GetMaxHP(), 100);
  ASSERT_TRUE(character.IsAlive());
  ASSERT_EQ(character.current_state->GetName(), "Acting");  // Still alive, still in Acting state
  
  // Verify EventBus published DamageTakenEvent
  ASSERT_TRUE(lastDamageTakenEvent.victim == &character);
  ASSERT_EQ(lastDamageTakenEvent.damage, 30);
  ```

- [ ] **Test_TakeDamage_Fatal()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.maxHP = 100;
  character.currentHP = 20;
  
  // Action
  character.TakeDamage(25);
  
  // Critical Assertions
  ASSERT_EQ(character.GetCurrentHP(), 0);  // NOT -5! Must clamp to 0
  ASSERT_FALSE(character.IsAlive());
  ASSERT_EQ(character.current_state->GetName(), "Dead");
  
  // Verify CharacterDiedEvent published
  ASSERT_TRUE(lastCharacterDiedEvent.deadCharacter == &character);
  
  // Verify character cannot act when dead
  int apBefore = character.GetActionPoints();
  character.ConsumeActionPoints(1);
  ASSERT_EQ(character.GetActionPoints(), apBefore);  // No change, dead characters can't consume AP
  ```

- [ ] **Test_TakeDamage_OverkillBy1000()**
  
  ```cpp
  // Edge case: Massive overkill damage
  TestCharacter character;
  character.currentHP = 50;
  
  character.TakeDamage(99999);
  
  ASSERT_EQ(character.GetCurrentHP(), 0);  // NOT -99949! Must clamp to 0
  ASSERT_FALSE(character.IsAlive());
  ```

- [ ] **Test_Heal_Normal()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.maxHP = 100;
  character.currentHP = 50;
  
  // Action
  character.Heal(20);
  
  // Assertions
  ASSERT_EQ(character.GetCurrentHP(), 70);
  ASSERT_TRUE(character.IsAlive());
  ```

- [ ] **Test_Heal_OverMax()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.maxHP = 100;
  character.currentHP = 90;
  
  // Action
  character.Heal(30);
  
  // Critical: Must NOT exceed maxHP
  ASSERT_EQ(character.GetCurrentHP(), 100);  // NOT 120!
  ASSERT_LE(character.GetCurrentHP(), character.GetMaxHP());
  ```

- [ ] **Test_Heal_DeadCharacter()**
  
  ```cpp
  // Edge case: Cannot heal dead characters (no resurrection)
  TestCharacter character;
  character.currentHP = 0;
  character.current_state = &state_dead;
  
  character.Heal(50);
  
  ASSERT_EQ(character.GetCurrentHP(), 0);  // Still dead
  ASSERT_FALSE(character.IsAlive());
  ```

**Test Suite 2: Action Points System**

- [ ] **Test_ActionPoints_Consume()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.maxActionPoints = 5;
  character.currentActionPoints = 5;
  
  // Action 1
  character.ConsumeActionPoints(2);
  ASSERT_EQ(character.GetActionPoints(), 3);
  
  // Action 2
  character.ConsumeActionPoints(1);
  ASSERT_EQ(character.GetActionPoints(), 2);
  
  // Action 3: Consume more than available
  character.ConsumeActionPoints(5);  // Only has 2, tries to consume 5
  ASSERT_EQ(character.GetActionPoints(), 0);  // Goes to 0, NOT -3
  ```

- [ ] **Test_ActionPoints_Refresh()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.maxActionPoints = 5;
  character.currentActionPoints = 0;
  
  // Action
  character.RefreshActionPoints();
  
  // Assertions
  ASSERT_EQ(character.GetActionPoints(), 5);
  ASSERT_EQ(character.GetActionPoints(), character.maxActionPoints);
  ```

- [ ] **Test_ActionPoints_RefreshWhileFull()**
  
  ```cpp
  // Edge case: Refreshing when already at max
  TestCharacter character;
  character.maxActionPoints = 5;
  character.currentActionPoints = 5;
  
  character.RefreshActionPoints();
  
  ASSERT_EQ(character.GetActionPoints(), 5);  // Still 5, NOT 10
  ```

**Test Suite 3: State Machine**

- [ ] **Test_StateMachine_WaitingTurn()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.change_state(&state_waitingturn);
  
  // Verify state
  ASSERT_EQ(character.current_state->GetName(), "WaitingTurn");
  
  // Verify cannot act
  int apBefore = character.GetActionPoints();
  character.ConsumeActionPoints(1);
  ASSERT_EQ(character.GetActionPoints(), apBefore - 1);  // Can still consume AP (for validation)
  ```

- [ ] **Test_StateMachine_Acting()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.change_state(&state_acting);
  
  // Verify state
  ASSERT_EQ(character.current_state->GetName(), "Acting");
  
  // Verify can act
  character.ConsumeActionPoints(1);
  ASSERT_EQ(character.GetActionPoints(), character.maxActionPoints - 1);
  ```

- [ ] **Test_StateMachine_Dead()**
  
  ```cpp
  // Setup
  TestCharacter character;
  character.currentHP = 0;
  character.change_state(&state_dead);
  
  // Verify state
  ASSERT_EQ(character.current_state->GetName(), "Dead");
  ASSERT_FALSE(character.IsAlive());
  
  // Verify cannot change state from Dead (permanent)
  character.change_state(&state_acting);
  ASSERT_EQ(character.current_state->GetName(), "Dead");  // Still dead
  ```

**Usage Examples**:

**Example 1: Creating a Test Character**

```cpp
// Define a concrete test character
class TestCharacter : public Character {
public:
    TestCharacter() {
        maxHP = 100;
        currentHP = 100;
        speed = 5;
        maxActionPoints = 5;
        currentActionPoints = 5;
        baseAttackPower = 10;
        attackDice = "2d6";
        baseDefensePower = 5;
        defenseDice = "1d6";
        attackRange = 1;
        gridPosition = {0, 0};
    }

    GameObjectTypes Type() override { return GameObjectTypes::Dragon; }
    std::string TypeName() override { return "TestCharacter"; }
};

// Usage
TestCharacter character;
character.TakeDamage(30);
std::cout << "HP: " << character.GetCurrentHP() << "/" << character.GetMaxHP() << std::endl;
// Output: HP: 70/100
```

**Example 2: Subscribing to Character Events**

```cpp
// Subscribe to damage events
EventBus::Instance().Subscribe<DamageTakenEvent>([](const DamageTakenEvent& e) {
    std::cout << e.victim->TypeName() << " took " << e.damage << " damage!" << std::endl;
    std::cout << "HP remaining: " << e.victim->GetCurrentHP() << std::endl;
});

// Subscribe to death events
EventBus::Instance().Subscribe<CharacterDiedEvent>([](const CharacterDiedEvent& e) {
    std::cout << e.deadCharacter->TypeName() << " has died!" << std::endl;
    Engine::GetLogger().LogEvent("Character died: " + e.deadCharacter->TypeName());
});

// Now when character takes damage, events fire
TestCharacter character;
character.TakeDamage(30);  // Triggers DamageTakenEvent
character.TakeDamage(80);  // Triggers DamageTakenEvent AND CharacterDiedEvent
```

**Example 3: Using the State Machine**

```cpp
TestCharacter character;

// Start of turn
character.change_state(&state_acting);
character.RefreshActionPoints();

// Perform actions
while (character.GetActionPoints() > 0) {
    // Do something
    character.ConsumeActionPoints(1);
}

// End of turn
character.change_state(&state_waitingturn);
```

**Example 4: Complete Combat Scenario**

```cpp
// Create two characters
TestCharacter hero;
TestCharacter enemy;

// Hero attacks enemy
int damage = 30;
enemy.TakeDamage(damage);

std::cout << "Enemy HP: " << enemy.GetCurrentHP() << "/" << enemy.GetMaxHP() << std::endl;
// Output: Enemy HP: 70/100

if (!enemy.IsAlive()) {
    std::cout << "Enemy defeated!" << std::endl;
    enemy.change_state(&state_dead);
}

// Enemy counterattacks (if alive)
if (enemy.IsAlive() && enemy.GetActionPoints() > 0) {
    hero.TakeDamage(20);
    enemy.ConsumeActionPoints(1);
}
```

**Dependencies**: None (self-contained, uses only CS230 engine)

---

### Week 1: Developer B - GridSystem Foundation

**Goal**: 8x8 grid battlefield with tile validation and basic occupancy tracking

**Files to Create**:

```
CS230/Game/Systems/GridSystem.h
CS230/Game/Systems/GridSystem.cpp
```

**Implementation Tasks**:

- [ ] **Grid data structure**
  
  ```cpp
  private:
      static const int GRID_WIDTH = 8;
      static const int GRID_HEIGHT = 8;
      static const int TILE_SIZE = 64;  // pixels
  
      enum class TileType {
          Empty,
          Wall,
          Lava,
          Difficult  // costs 2 movement
      };
  
      TileType tiles[GRID_WIDTH][GRID_HEIGHT];
      std::map<Math::vec2, Character*> occupiedTiles;
  ```

- [ ] **Tile validation methods**
  
  ```cpp
  bool IsValidTile(Math::vec2 tile) const;
  bool IsWalkable(Math::vec2 tile) const;
  bool IsOccupied(Math::vec2 tile) const;
  TileType GetTileType(Math::vec2 tile) const;
  void SetTileType(Math::vec2 tile, TileType type);
  ```

- [ ] **Character placement (basic)**
  
  ```cpp
  void PlaceCharacter(Character* character, Math::vec2 pos);
  void RemoveCharacter(Math::vec2 pos);
  Character* GetCharacterAt(Math::vec2 pos) const;
  ```

**Note**: Pathfinding (A*) and line-of-sight added in Week 2

**Rigorous Testing** (using MockCharacter for character placement tests):

**Test Suite 1: Tile Validation**

- [ ] **Test_IsValidTile_AllCorners()**
  
  ```cpp
  GridSystem grid;
  
  // Test all 4 corners (should be valid)
  ASSERT_TRUE(grid.IsValidTile({0, 0}));     // Top-left
  ASSERT_TRUE(grid.IsValidTile({7, 0}));     // Top-right
  ASSERT_TRUE(grid.IsValidTile({0, 7}));     // Bottom-left
  ASSERT_TRUE(grid.IsValidTile({7, 7}));     // Bottom-right
  
  // Test center
  ASSERT_TRUE(grid.IsValidTile({4, 4}));
  ```

- [ ] **Test_IsValidTile_OutOfBounds()**
  
  ```cpp
  GridSystem grid;
  
  // Negative coordinates
  ASSERT_FALSE(grid.IsValidTile({-1, 0}));
  ASSERT_FALSE(grid.IsValidTile({0, -1}));
  ASSERT_FALSE(grid.IsValidTile({-1, -1}));
  
  // Beyond grid bounds (8x8 grid, max index is 7)
  ASSERT_FALSE(grid.IsValidTile({8, 0}));
  ASSERT_FALSE(grid.IsValidTile({0, 8}));
  ASSERT_FALSE(grid.IsValidTile({8, 8}));
  ASSERT_FALSE(grid.IsValidTile({100, 100}));
  ```

- [ ] **Test_IsWalkable_EmptyTiles()**
  
  ```cpp
  GridSystem grid;
  
  // All tiles start as Empty (walkable)
  for (int x = 0; x < 8; ++x) {
      for (int y = 0; y < 8; ++y) {
          ASSERT_TRUE(grid.IsWalkable({x, y}));
          ASSERT_EQ(grid.GetTileType({x, y}), TileType::Empty);
      }
  }
  ```

- [ ] **Test_IsWalkable_WallTiles()**
  
  ```cpp
  GridSystem grid;
  
  // Place walls
  grid.SetTileType({3, 3}, TileType::Wall);
  grid.SetTileType({3, 4}, TileType::Wall);
  
  // Walls are NOT walkable
  ASSERT_FALSE(grid.IsWalkable({3, 3}));
  ASSERT_FALSE(grid.IsWalkable({3, 4}));
  
  // Adjacent tiles ARE walkable
  ASSERT_TRUE(grid.IsWalkable({2, 3}));
  ASSERT_TRUE(grid.IsWalkable({4, 3}));
  ASSERT_TRUE(grid.IsWalkable({3, 2}));
  ASSERT_TRUE(grid.IsWalkable({3, 5}));
  ```

- [ ] **Test_IsWalkable_DifferentTileTypes()**
  
  ```cpp
  GridSystem grid;
  
  grid.SetTileType({0, 0}, TileType::Empty);
  grid.SetTileType({1, 0}, TileType::Wall);
  grid.SetTileType({2, 0}, TileType::Lava);
  grid.SetTileType({3, 0}, TileType::Difficult);
  
  ASSERT_TRUE(grid.IsWalkable({0, 0}));   // Empty: walkable
  ASSERT_FALSE(grid.IsWalkable({1, 0}));  // Wall: NOT walkable
  ASSERT_TRUE(grid.IsWalkable({2, 0}));   // Lava: walkable (but damages)
  ASSERT_TRUE(grid.IsWalkable({3, 0}));   // Difficult: walkable (costs 2 movement)
  ```

**Test Suite 2: Character Placement & Occupancy**

- [ ] **Test_PlaceCharacter_SingleCharacter()**
  
  ```cpp
  #include "../Test/Week1TestMocks.h"  // Include mock classes
  
  GridSystem grid;
  MockCharacter character("TestChar");
  
  // Place character
  grid.PlaceCharacter(&character, {4, 4});
  
  // Verify occupancy
  ASSERT_TRUE(grid.IsOccupied({4, 4}));
  ASSERT_EQ(grid.GetCharacterAt({4, 4}), &character);
  
  // Verify other tiles not occupied
  ASSERT_FALSE(grid.IsOccupied({3, 4}));
  ASSERT_FALSE(grid.IsOccupied({5, 4}));
  ASSERT_EQ(grid.GetCharacterAt({3, 4}), nullptr);
  ```

- [ ] **Test_PlaceCharacter_MultipleCharacters()**
  
  ```cpp
  GridSystem grid;
  MockCharacter char1("Char1"), char2("Char2"), char3("Char3");
  
  grid.PlaceCharacter(&char1, {0, 0});
  grid.PlaceCharacter(&char2, {7, 7});
  grid.PlaceCharacter(&char3, {4, 4});
  
  ASSERT_TRUE(grid.IsOccupied({0, 0}));
  ASSERT_TRUE(grid.IsOccupied({7, 7}));
  ASSERT_TRUE(grid.IsOccupied({4, 4}));
  
  ASSERT_EQ(grid.GetCharacterAt({0, 0}), &char1);
  ASSERT_EQ(grid.GetCharacterAt({7, 7}), &char2);
  ASSERT_EQ(grid.GetCharacterAt({4, 4}), &char3);
  ```

- [ ] **Test_PlaceCharacter_Overwrite()**
  
  ```cpp
  GridSystem grid;
  MockCharacter char1("Char1"), char2("Char2");
  
  // Place char1
  grid.PlaceCharacter(&char1, {4, 4});
  ASSERT_EQ(grid.GetCharacterAt({4, 4}), &char1);
  
  // Place char2 at same location (should overwrite)
  grid.PlaceCharacter(&char2, {4, 4});
  ASSERT_EQ(grid.GetCharacterAt({4, 4}), &char2);  // char2 now occupies tile
  ```

- [ ] **Test_RemoveCharacter()**
  
  ```cpp
  GridSystem grid;
  MockCharacter character("TestChar");
  
  // Place then remove
  grid.PlaceCharacter(&character, {4, 4});
  ASSERT_TRUE(grid.IsOccupied({4, 4}));
  
  grid.RemoveCharacter({4, 4});
  ASSERT_FALSE(grid.IsOccupied({4, 4}));
  ASSERT_EQ(grid.GetCharacterAt({4, 4}), nullptr);
  ```

**Test Suite 3: Tile Type Management**

- [ ] **Test_SetTileType_AllTypes()**
  
  ```cpp
  GridSystem grid;
  
  grid.SetTileType({0, 0}, TileType::Empty);
  grid.SetTileType({1, 0}, TileType::Wall);
  grid.SetTileType({2, 0}, TileType::Lava);
  grid.SetTileType({3, 0}, TileType::Difficult);
  
  ASSERT_EQ(grid.GetTileType({0, 0}), TileType::Empty);
  ASSERT_EQ(grid.GetTileType({1, 0}), TileType::Wall);
  ASSERT_EQ(grid.GetTileType({2, 0}), TileType::Lava);
  ASSERT_EQ(grid.GetTileType({3, 0}), TileType::Difficult);
  ```

- [ ] **Test_SetTileType_OutOfBounds()**
  
  ```cpp
  GridSystem grid;
  
  // Should NOT crash, should log error instead
  grid.SetTileType({-1, 0}, TileType::Wall);    // Invalid
  grid.SetTileType({8, 8}, TileType::Wall);     // Invalid
  grid.SetTileType({100, 100}, TileType::Wall); // Invalid
  
  // Verify valid tiles unaffected
  ASSERT_EQ(grid.GetTileType({0, 0}), TileType::Empty);
  ```

**Usage Examples**:

**Example 1: Basic Grid Setup**

```cpp
// Create grid system
GridSystem grid;

// Verify grid dimensions
std::cout << "Grid size: " << GridSystem::GRID_WIDTH << "x" << GridSystem::GRID_HEIGHT << std::endl;
// Output: Grid size: 8x8

// Check tile validity
if (grid.IsValidTile({5, 5})) {
    std::cout << "Tile (5,5) is valid" << std::endl;
}

// Check all tiles walkable
bool allWalkable = true;
for (int x = 0; x < 8; ++x) {
    for (int y = 0; y < 8; ++y) {
        if (!grid.IsWalkable({x, y})) {
            allWalkable = false;
            break;
        }
    }
}
std::cout << "All tiles walkable: " << (allWalkable ? "Yes" : "No") << std::endl;
// Output: All tiles walkable: Yes
```

**Example 2: Building Walls**

```cpp
GridSystem grid;

// Create a wall across the middle
for (int x = 0; x < 8; ++x) {
    grid.SetTileType({x, 4}, TileType::Wall);
}

// Verify walls
for (int x = 0; x < 8; ++x) {
    ASSERT_FALSE(grid.IsWalkable({x, 4}));
    ASSERT_EQ(grid.GetTileType({x, 4}), TileType::Wall);
}

std::cout << "Wall created at row 4" << std::endl;
```

**Example 3: Placing Multiple Characters**

```cpp
GridSystem grid;

// Spawn Dragon and Fighter
Dragon dragon;
Fighter fighter;

grid.PlaceCharacter(&dragon, {4, 4});   // Center
grid.PlaceCharacter(&fighter, {0, 0});  // Top-left corner

// Verify placements
std::cout << "Dragon at: (" << dragon.GetGridPosition().x << ", "
          << dragon.GetGridPosition().y << ")" << std::endl;
// Output: Dragon at: (4, 4)

std::cout << "Fighter at: (" << fighter.GetGridPosition().x << ", "
          << fighter.GetGridPosition().y << ")" << std::endl;
// Output: Fighter at: (0, 0)

// Check occupancy
if (grid.IsOccupied({4, 4})) {
    Character* occupant = grid.GetCharacterAt({4, 4});
    std::cout << "Tile (4,4) occupied by " << occupant->TypeName() << std::endl;
    // Output: Tile (4,4) occupied by Dragon
}
```

**Example 4: Creating Lava Hazards**

```cpp
GridSystem grid;

// Create lava pool pattern
std::vector<Math::vec2> lavaTiles = {
    {3, 3}, {4, 3}, {5, 3},
    {3, 4}, {4, 4}, {5, 4},
    {3, 5}, {4, 5}, {5, 5}
};

for (const auto& tile : lavaTiles) {
    grid.SetTileType(tile, TileType::Lava);
}

// Check if character would step in lava
Math::vec2 proposedMove = {4, 4};
if (grid.GetTileType(proposedMove) == TileType::Lava) {
    std::cout << "Warning: Moving to lava tile! Character will take damage." << std::endl;
}
```

**Example 5: Difficult Terrain**

```cpp
GridSystem grid;

// Create difficult terrain (forest, mud, etc.)
grid.SetTileType({5, 5}, TileType::Difficult);

// Check movement cost
if (grid.GetTileType({5, 5}) == TileType::Difficult) {
    int movementCost = 2;  // Difficult terrain costs 2 movement
    std::cout << "Tile (5,5) costs " << movementCost << " movement to enter" << std::endl;
}
```

**Dependencies**: None (uses only Math::vec2 from CS230 engine)

---

### Week 1: Developer C - EventBus Singleton

**Goal**: Complete event-driven communication system for the entire game

**Files to Create**:

```
CS230/Game/Singletons/EventBus.h
CS230/Game/Singletons/EventBus.cpp
CS230/Game/Events/Events.h
```

**Note**: For Week 1, we start with a single `Events.h` file containing all event types. As the project grows, we can split this into domain-specific files (`CombatEvents.h`, `TurnEvents.h`, etc.) as detailed in [EventBus System Documentation](../../systems/eventbus.md).

**Implementation Tasks**:

- [x] **EventBus singleton class**
  
  ```cpp
  class EventBus {
  public:
      static EventBus& Instance();
  
      template<typename T>
      void Subscribe(std::function<void(const T&)> callback);
  
      template<typename T>
      void Publish(const T& event);
  
      void Clear();  // Clear all subscriptions
      void SetLogging(bool enabled);  // Enable/disable event logging
      bool IsLoggingEnabled() const;
  
  private:
      std::map<std::type_index, std::vector<std::function<void(const void*)>>> subscribers;
      bool loggingEnabled = false;
  
      void LogEvent(const std::string& eventType, const void* eventData);
  };
  ```

- [x] **Define all event types** (in Events.h):
  
  ```cpp
  struct CharacterDamagedEvent {
      Character* target;
      int damageAmount;
      int remainingHP;
      Character* attacker;
      bool wasCritical;
  };
  
  struct CharacterHealedEvent {
      Character* target;
      int healAmount;
      int currentHP;
      int maxHP;
      Character* healer;
  };
  
  struct CharacterDeathEvent {
      Character* character;
      Character* killer;
  };
  
  struct TurnStartedEvent {
      Character* character;
      int turnNumber;
      int actionPoints;
  };
  
  struct TurnEndedEvent {
      Character* character;
      int actionsUsed;
  };
  
  struct SpellCastEvent {
      Character* caster;
      std::string spellName;
      int spellLevel;
      Math::ivec2 targetGrid;
      int spellSlotUsed;
  };
  
  struct CharacterMovedEvent {
      Character* character;
      Math::ivec2 fromGrid;
      Math::ivec2 toGrid;
      int actionPointsSpent;
  };
  
  struct AttackMissedEvent {
      Character* attacker;
      Character* target;
      std::string reason;
  };
  
  // Define 10+ event types total
  ```

**Usage Example**:

```cpp
// Subscribe to events
EventBus::Instance().Subscribe<CharacterDamagedEvent>([](const CharacterDamagedEvent& e) {
    Engine::GetLogger().LogEvent("Character took " + std::to_string(e.damageAmount) + " damage");
});

// Publish events
CharacterDamagedEvent event{victim, 30, 70, attacker, false};
EventBus::Instance().Publish(event);
```

---

#### 🏗️ EventBus Design Philosophy

**IMPORTANT DESIGN DECISION**: Understanding Why Type-Based Subscription is Correct

**Question**: "Should EventBus distinguish individual subscribers, or only event types?"

**Answer**: **EventBus uses type-based subscription (`std::type_index(typeid(T))`) - This is CORRECT by design.**

##### Why Type-Based Subscription Works

**Current Design:**

```cpp
// EventBus internally uses:
std::map<std::type_index, std::vector<CallbackWrapper>> subscribers;

// Multiple systems subscribe to the SAME event type
EventBus::Instance().Subscribe<CharacterDamagedEvent>([](const CharacterDamagedEvent& e) {
    // System A: Update UI
});

EventBus::Instance().Subscribe<CharacterDamagedEvent>([](const CharacterDamagedEvent& e) {
    // System B: Log damage
});

EventBus::Instance().Subscribe<CharacterDamagedEvent>([](const CharacterDamagedEvent& e) {
    // System C: Update AI memory
});

// When event published, ALL subscribers receive it (broadcast semantics)
EventBus::Instance().Publish(CharacterDamagedEvent{dragon, 15, 125, fighter, true});
```

**Key Insight**: Events are **broadcast announcements**, not **direct messages**.

##### Event Data Contains Context for Filtering

Notice that event structs include `target`, `attacker`, `character` fields:

```cpp
struct CharacterDamagedEvent {
    Character* target;      // WHO was damaged (filtering info)
    int damageAmount;
    int remainingHP;
    Character* attacker;    // WHO dealt damage (context)
    bool wasCritical;
};
```

**Subscribers filter by checking event data:**

```cpp
// BattleManager subscribes to ALL death events
EventBus::Instance().Subscribe<CharacterDeathEvent>([this](const CharacterDeathEvent& e) {
    if (e.character->Type() == GameObjectTypes::Dragon) {
        EndBattle(false);  // Dragon died - game over
    }
});

// StatusInfoOverlay subscribes to ALL damage events
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    UpdateHPBar(e.target);  // Update UI for damaged character
});

// Dragon's component filters by target
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    if (e.target == this->owner) {  // Filter: is this event for MY character?
        PlayDamageAnimation();
    }
});
```

##### Why Multiple Systems Need the Same Event

**Example**: When Dragon casts Fireball:

```cpp
EventBus::Instance().Publish(SpellCastEvent{&dragon, "Fireball", 1, {7,7}, 1});
```

**Six different systems react to this ONE event:**

1. ✅ **EffectManager** → Spawn fireball projectile animation
2. ✅ **Logger** → Record: "Dragon cast Fireball at (7,7)"
3. ✅ **AIMemory (all AI characters)** → Update: "Dragon used offensive spell"
4. ✅ **AIDirector** → Update threat: Dragon is aggressive
5. ✅ **DebugUIOverlay** → Display spell cast in event log
6. ✅ **SaveManager** → Track spell usage stats

**If EventBus distinguished subscribers, you'd need:**

```cpp
// ❌ WRONG - This defeats the purpose of EventBus!
Publish(SpellCastEvent, target=EffectManager);
Publish(SpellCastEvent, target=Logger);
Publish(SpellCastEvent, target=AIMemory);
// ... (6 separate publishes - tight coupling!)
```

##### Real-World Example: Fighter Attacks Dragon

```cpp
// CombatSystem publishes ONE event
EventBus::Instance().Publish(CharacterDamagedEvent{
    .target = dragon,
    .damageAmount = 15,
    .remainingHP = 125,
    .attacker = fighter,
    .wasCritical = true
});

// All subscribers react (each checks event data):

// 1. StatusInfoOverlay: Update Dragon's HP bar
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    UpdateHPBar(e.target);  // Dragon's HP bar updates
});

// 2. BattleManager: Check victory conditions
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    if (e.remainingHP <= 0) {
        CheckVictoryConditions();
    }
});

// 3. AIDirector: Update threat assessment
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    UpdateThreat(e.attacker, +10);  // Fighter is dangerous
});

// 4. EffectManager: Show damage particle effect
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    SpawnDamageNumber(e.target->GetPosition(), e.damageAmount);
    if (e.wasCritical) {
        SpawnCriticalFlash(e.target->GetPosition());
    }
});

// 5. Logger: Record event
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    LogEvent(e.attacker->TypeName() + " dealt " +
             std::to_string(e.damageAmount) + " damage to " +
             e.target->TypeName());
});
```

**All 5 systems react to ONE event publish** → Perfect decoupling!

##### Performance Analysis

**Question**: "Won't ALL subscribers get called even if event isn't for them?"

**Answer**: Yes, but this is **intentional and efficient** for turn-based tactical RPG.

**Scenario**: Dragon takes damage

```cpp
EventBus::Instance().Publish(CharacterDamagedEvent{dragon, 10, 130, fighter, false});
```

**Cost per subscriber**: ~5-10 instructions to check `if (e.target == this)`
**Total subscribers**: ~8-10 systems
**Total cost**: ~80 instructions ≈ **0.0001ms on modern CPU**

**For turn-based RPG with ~5 characters:**

- Events published: ~10-50 times per turn
- Total event overhead: **<0.01ms per turn** (negligible)

**Benefit**: Complete decoupling of all systems ✅

##### Why NOT Per-Subscriber Identification

**Anti-Pattern**: Trying to send message to specific subscriber

```cpp
// ❌ BAD: Publisher must know subscriber IDs (tight coupling)
EventBus::Instance().PublishTo(CharacterDamagedEvent{...}, targetSubscriber=dragon);
```

**Problems:**

- ❌ Publisher must know subscribers exist (breaks encapsulation)
- ❌ Cannot broadcast to multiple systems
- ❌ Tight coupling defeats loose coupling principle
- ❌ Wrong abstraction (this is a message queue, not event bus)

**Correct Solution**: Use event data for filtering

```cpp
// ✅ GOOD: Broadcast event, subscribers filter by target
EventBus::Instance().Publish(CharacterDamagedEvent{
    .target = dragon,  // Dragon checks this
    .damageAmount = 10
});

// Dragon's subscription filters by target:
EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
    if (e.target == this) {  // Filter: is this event for ME?
        UpdateHPBar();
    }
});
```

##### Summary: EventBus Design Principles

**✅ CORRECT Design (Type-Based Subscription):**

1. **Event = Broadcast Announcement** (not direct message)
2. **Event data contains context** (`target`, `attacker`, etc.) for filtering
3. **Multiple systems react to same event** (loose coupling)
4. **Performance is negligible** for turn-based RPG (<0.01ms per turn)
5. **Aligns with architecture principles** (see [docs/architecture.md](../../architecture.md))

**❌ INCORRECT Design (Per-Subscriber Channels):**

1. Tight coupling (publisher knows subscribers)
2. Cannot broadcast to multiple systems
3. Complex API requiring subscriber IDs
4. Wrong abstraction (message queue vs. event bus)

**Key Takeaway**: EventBus uses `typeid(T)` to distinguish event **types**, not subscribers. Subscribers filter events by checking event data fields (`.target`, `.attacker`, `.character`). This provides perfect loose coupling while maintaining efficiency.

---

**Rigorous Testing** (using MockCharacter for independence):

**Test Suite 1: Basic Pub/Sub Functionality**

- [x] **Test_Subscribe_Publish_SingleSubscriber()**
  
  ```cpp
  #include "../Test/Week1TestMocks.h"  // Include mock classes
  
  EventBus::Instance().Clear();  // Start fresh
  
  // Setup: Subscribe to CharacterDamagedEvent
  bool callbackInvoked = false;
  int receivedDamage = 0;
  MockCharacter* receivedTarget = nullptr;
  
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      callbackInvoked = true;
      receivedDamage = e.damageAmount;
      receivedTarget = static_cast<MockCharacter*>(e.target);
  });
  
  // Action: Publish event with MockCharacter
  MockCharacter character("TestDragon");
  CharacterDamagedEvent event{&character, 30, 70, nullptr, false};
  EventBus::Instance().Publish(event);
  
  // Assertions
  ASSERT_TRUE(callbackInvoked);
  ASSERT_EQ(receivedDamage, 30);
  ASSERT_EQ(receivedTarget, &character);
  ```

- [x] **Test_MultipleSubscribers_SameEvent()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: 3 subscribers to same event
  int callback1Count = 0;
  int callback2Count = 0;
  int callback3Count = 0;
  
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      callback1Count++;
  });
  
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      callback2Count++;
  });
  
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      callback3Count++;
  });
  
  // Action: Publish event
  MockCharacter character("TestChar");
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 10, 90, nullptr, false});
  
  // All 3 callbacks should be invoked
  ASSERT_EQ(callback1Count, 1);
  ASSERT_EQ(callback2Count, 1);
  ASSERT_EQ(callback3Count, 1);
  ```

- [x] **Test_MultipleDifferentEvents()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: Subscribe to 3 different event types
  bool damageCalled = false;
  bool deathCalled = false;
  bool spellCalled = false;
  
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      damageCalled = true;
  });
  
  EventBus::Instance().Subscribe<CharacterDeathEvent>([&](const CharacterDeathEvent& e) {
      deathCalled = true;
  });
  
  EventBus::Instance().Subscribe<SpellCastEvent>([&](const SpellCastEvent& e) {
      spellCalled = true;
  });
  
  // Publish all 3 events
  MockCharacter character("TestChar");
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 10, 90, nullptr, false});
  EventBus::Instance().Publish(CharacterDeathEvent{&character, nullptr});
  EventBus::Instance().Publish(SpellCastEvent{&character, "Fireball", 1, {0,0}, 1});
  
  // All 3 should be called
  ASSERT_TRUE(damageCalled);
  ASSERT_TRUE(deathCalled);
  ASSERT_TRUE(spellCalled);
  ```

**Test Suite 2: Event Data Integrity**

- [x] **Test_EventData_CompleteTransfer()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: Complex event with all fields
  MockCharacter victim("Victim"), attacker("Attacker");
  CharacterDamagedEvent receivedEvent;
  
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      receivedEvent = e;  // Copy event
  });
  
  // Publish
  CharacterDamagedEvent originalEvent{&victim, 42, 58, &attacker, true};
  EventBus::Instance().Publish(originalEvent);
  
  // Verify all fields transferred correctly
  ASSERT_EQ(receivedEvent.target, &victim);
  ASSERT_EQ(receivedEvent.damageAmount, 42);
  ASSERT_EQ(receivedEvent.remainingHP, 58);
  ASSERT_EQ(receivedEvent.attacker, &attacker);
  ASSERT_TRUE(receivedEvent.wasCritical);
  ```

- [x] **Test_EventData_MultiplePublishes()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup
  std::vector<int> damages;
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      damages.push_back(e.damageAmount);
  });
  
  // Publish multiple events
  MockCharacter character("TestChar");
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 10, 90, nullptr, false});
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 20, 70, nullptr, false});
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 30, 40, nullptr, true});
  
  // Verify all received in order
  ASSERT_EQ(damages.size(), 3);
  ASSERT_EQ(damages[0], 10);
  ASSERT_EQ(damages[1], 20);
  ASSERT_EQ(damages[2], 30);
  ```

**Test Suite 3: Clear & Cleanup**

- [ ] **Test_Clear_RemovesAllSubscribers()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: Subscribe to multiple events
  int damageCount = 0;
  int deathCount = 0;
  
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      damageCount++;
  });
  
  EventBus::Instance().Subscribe<CharacterDeathEvent>([&](const CharacterDeathEvent& e) {
      deathCount++;
  });
  
  // Clear all subscriptions
  EventBus::Instance().Clear();
  
  // Publish events (should NOT trigger callbacks)
  MockCharacter character("TestChar");
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 10, 90, nullptr, false});
  EventBus::Instance().Publish(CharacterDeathEvent{&character, nullptr});
  
  // Callbacks should NOT be invoked
  ASSERT_EQ(damageCount, 0);
  ASSERT_EQ(deathCount, 0);
  ```

- [ ] **Test_Publish_NoSubscribers()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Publish event with NO subscribers (should not crash)
  MockCharacter character("TestChar");
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 10, 90, nullptr, false});
  
  // If we get here, test passed (no crash)
  ASSERT_TRUE(true);
  ```

**Test Suite 4: Stress Testing**

- [ ] **Test_ManySubscribers()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Subscribe 100 callbacks
  std::vector<int> callbackCounts(100, 0);
  for (int i = 0; i < 100; ++i) {
      EventBus::Instance().Subscribe<CharacterDamagedEvent>([&, i](const CharacterDamagedEvent& e) {
          callbackCounts[i]++;
      });
  }
  
  // Publish event
  MockCharacter character("TestChar");
  EventBus::Instance().Publish(CharacterDamagedEvent{&character, 10, 90, nullptr, false});
  
  // All 100 should be invoked
  for (int i = 0; i < 100; ++i) {
      ASSERT_EQ(callbackCounts[i], 1);
  }
  ```

- [ ] **Test_ManyEvents()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: Subscribe
  int count = 0;
  EventBus::Instance().Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
      count++;
  });
  
  // Publish 1000 events
  MockCharacter character("TestChar");
  for (int i = 0; i < 1000; ++i) {
      EventBus::Instance().Publish(CharacterDamagedEvent{&character, i, 100-i, nullptr, false});
  }
  
  // All 1000 should be received
  ASSERT_EQ(count, 1000);
  ```

**Usage Examples**:

**Example 1: Basic Damage Logging**

```cpp
// Subscribe to damage events for logging
EventBus::Instance().Subscribe<CharacterDamagedEvent>([](const CharacterDamagedEvent& e) {
    std::string msg = e.target->TypeName() + " took " + std::to_string(e.damageAmount) + " damage";
    Engine::GetLogger().LogEvent(msg);
});

// Now whenever ANY character takes damage, it's automatically logged
Dragon dragon;
dragon.TakeDamage(30);
// Logger.txt: "Dragon took 30 damage"

Fighter fighter;
fighter.TakeDamage(20);
// Logger.txt: "Fighter took 20 damage"
```

**Example 2: Death Notification System**

```cpp
// Subscribe to character death
EventBus::Instance().Subscribe<CharacterDeathEvent>([](const CharacterDeathEvent& e) {
    std::cout << "💀 " << e.character->TypeName() << " has been slain!" << std::endl;

    if (e.killer != nullptr) {
        std::cout << "   Killer: " << e.killer->TypeName() << std::endl;
    } else {
        std::cout << "   (Environmental death)" << std::endl;
    }
});

// Combat scenario
Fighter fighter;
Dragon dragon;

fighter.TakeDamage(100);  // Dies
// Console output:
// 💀 Fighter has been slain!
//    Killer: Dragon
```

**Example 3: Turn-Based UI Updates**

```cpp
// Subscribe to turn events to update UI
EventBus::Instance().Subscribe<TurnStartedEvent>([](const TurnStartedEvent& e) {
    std::cout << "\n=== Turn " << e.turnNumber << " ===" << std::endl;
    std::cout << "Active Character: " << e.character->TypeName() << std::endl;
    std::cout << "HP: " << e.character->GetCurrentHP() << "/" << e.character->GetMaxHP() << std::endl;
    std::cout << "AP: " << e.actionPoints << std::endl;
});

EventBus::Instance().Subscribe<TurnEndedEvent>([](const TurnEndedEvent& e) {
    std::cout << e.character->TypeName() << " ended their turn." << std::endl;
});

// When TurnManager runs, UI updates automatically!
```

**Example 4: Spell Effects & VFX Triggers**

```cpp
// Subscribe to spell events to trigger VFX
EventBus::Instance().Subscribe<SpellCastEvent>([](const SpellCastEvent& e) {
    std::cout << "✨ " << e.caster->TypeName() << " cast " << e.spellName
              << " (Level " << e.level << ")" << std::endl;

    // Trigger VFX based on spell name
    if (e.spellName == "Fireball") {
        std::cout << "   🔥 FIREBALL VFX at (" << e.targetGrid.x << ", " << e.targetGrid.y << ")" << std::endl;
    } else if (e.spellName == "LavaPool") {
        std::cout << "   🌋 LAVA VFX at (" << e.targetGrid.x << ", " << e.targetGrid.y << ")" << std::endl;
    }
});

// Dragon casts spell
Dragon dragon;
dragon.Spell_Fireball(1, {5, 5});
// Console output:
// ✨ Dragon cast Fireball (Level 1)
//    🔥 FIREBALL VFX at (5, 5)
```

**Example 5: Achievement/Statistics Tracking**

```cpp
// Track statistics using EventBus
class StatsTracker {
public:
    void Init() {
        EventBus::Instance().Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
            totalDamageDealt += e.damageAmount;
        });

        EventBus::Instance().Subscribe<CharacterDeathEvent>([this](const CharacterDeathEvent& e) {
            enemiesKilled++;
        });

        EventBus::Instance().Subscribe<SpellCastEvent>([this](const SpellCastEvent& e) {
            spellsCast++;
        });
    }

    void PrintStats() {
        std::cout << "=== Combat Statistics ===" << std::endl;
        std::cout << "Total Damage Dealt: " << totalDamageDealt << std::endl;
        std::cout << "Enemies Killed: " << enemiesKilled << std::endl;
        std::cout << "Spells Cast: " << spellsCast << std::endl;
    }

private:
    int totalDamageDealt = 0;
    int enemiesKilled = 0;
    int spellsCast = 0;
};

// Usage
StatsTracker tracker;
tracker.Init();

// ... play game ...

tracker.PrintStats();
// Output:
// === Combat Statistics ===
// Total Damage Dealt: 450
// Enemies Killed: 3
// Spells Cast: 12
```

**Example 6: Debugging with EventBus**

```cpp
// Subscribe to ALL events for debugging
EventBus::Instance().Subscribe<CharacterDamagedEvent>([](const CharacterDamagedEvent& e) {
    Engine::GetLogger().LogDebug("[EVENT] CharacterDamaged: " + std::to_string(e.damageAmount));
});

EventBus::Instance().Subscribe<CharacterDeathEvent>([](const CharacterDeathEvent& e) {
    Engine::GetLogger().LogDebug("[EVENT] CharacterDeath: " + e.character->TypeName());
});

EventBus::Instance().Subscribe<TurnStartedEvent>([](const TurnStartedEvent& e) {
    Engine::GetLogger().LogDebug("[EVENT] TurnStarted: Turn " + std::to_string(e.turnNumber));
});

// Now Logger.txt contains complete event log for debugging!
```

**Dependencies**: None (uses C++ STL: `<typeindex>`, `<functional>`, `<map>`)

**Related Documentation**: See [EventBus System Documentation](../../systems/eventbus.md) for complete usage patterns, advanced examples, and architectural best practices.

---

### Week 1: Developer D - DiceManager Singleton

**Goal**: Complete dice rolling system with D&D notation support

**Files to Create**:

```
CS230/Game/Singletons/DiceManager.h
CS230/Game/Singletons/DiceManager.cpp
```

**Implementation Tasks**:

- [ ] **DiceManager singleton class**
  
  ```cpp
  class DiceManager {
  public:
      static DiceManager& Instance();
  
      int RollDice(int count, int sides);
      int RollDiceFromString(const std::string& diceNotation);  // "3d6", "2d8+5"
      void SetSeed(int seed);  // For testing reproducibility
      std::vector<int> GetLastRolls() const;  // For debug logging
  
  private:
      std::mt19937 rng;
      std::vector<int> lastRolls;  // Store individual die results
  };
  ```

- [ ] **Parse dice notation strings**
  
  ```cpp
  // "3d6" → Roll 3 six-sided dice
  // "2d8+5" → Roll 2 eight-sided dice, add 5
  // "1d20-2" → Roll 1 twenty-sided die, subtract 2
  ```

- [ ] **Logging integration**
  
  ```cpp
  // Log all dice rolls to Engine::GetLogger()
  // Example: "DiceManager: Rolled 3d6 = [4, 6, 2] = 12"
  ```

**Usage Example**:

```cpp
// Roll dice for damage
int damage = DiceManager::Instance().RollDiceFromString("3d6+5");
Engine::GetLogger().LogEvent("Rolled damage: " + std::to_string(damage));

// Set seed for testing
DiceManager::Instance().SetSeed(12345);
int result = DiceManager::Instance().RollDice(3, 6);  // Reproducible result
```

**Rigorous Testing**:

**Test Suite 1: Basic Dice Rolling**

- [ ] **Test_RollDice_SingleDie_d6()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Roll 1d6 multiple times
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDice(1, 6);
      ASSERT_GE(result, 1);  // Minimum is 1
      ASSERT_LE(result, 6);  // Maximum is 6
  }
  ```

- [ ] **Test_RollDice_SingleDie_d20()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Roll 1d20 (for initiative)
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDice(1, 20);
      ASSERT_GE(result, 1);
      ASSERT_LE(result, 20);
  }
  ```

- [ ] **Test_RollDice_MultipleDice_3d6()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Roll 3d6
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDice(3, 6);
      ASSERT_GE(result, 3);   // Minimum: 3 × 1 = 3
      ASSERT_LE(result, 18);  // Maximum: 3 × 6 = 18
  }
  ```

- [ ] **Test_RollDice_MultipleDice_10d6()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Roll 10d6 (lots of dice)
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDice(10, 6);
      ASSERT_GE(result, 10);   // Minimum: 10 × 1 = 10
      ASSERT_LE(result, 60);   // Maximum: 10 × 6 = 60
  }
  ```

- [ ] **Test_RollDice_Distribution()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Roll 1d6 many times, verify distribution
  std::map<int, int> distribution;
  int rolls = 6000;
  
  for (int i = 0; i < rolls; ++i) {
      int result = dice.RollDice(1, 6);
      distribution[result]++;
  }
  
  // Each face should appear ~1000 times (6000 / 6)
  // Allow 20% variance
  for (int face = 1; face <= 6; ++face) {
      ASSERT_GE(distribution[face], 800);   // At least 800
      ASSERT_LE(distribution[face], 1200);  // At most 1200
  }
  ```

**Test Suite 2: String Notation Parsing**

- [ ] **Test_RollDiceFromString_Basic_3d6()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDiceFromString("3d6");
      ASSERT_GE(result, 3);
      ASSERT_LE(result, 18);
  }
  ```

- [ ] **Test_RollDiceFromString_WithPositiveModifier()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // 2d8+5
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDiceFromString("2d8+5");
      ASSERT_GE(result, 7);   // 2×1+5 = 7
      ASSERT_LE(result, 21);  // 2×8+5 = 21
  }
  ```

- [ ] **Test_RollDiceFromString_WithNegativeModifier()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // 1d20-2
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDiceFromString("1d20-2");
      ASSERT_GE(result, -1);  // 1-2 = -1 (can be negative!)
      ASSERT_LE(result, 18);  // 20-2 = 18
  }
  ```

- [ ] **Test_RollDiceFromString_LargeModifier()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // 1d6+100
  for (int i = 0; i < 100; ++i) {
      int result = dice.RollDiceFromString("1d6+100");
      ASSERT_GE(result, 101);  // 1+100 = 101
      ASSERT_LE(result, 106);  // 6+100 = 106
  }
  ```

- [ ] **Test_RollDiceFromString_EdgeCases()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // 1d1 (always returns 1)
  ASSERT_EQ(dice.RollDiceFromString("1d1"), 1);
  
  // 1d100 (percentile die)
  int result = dice.RollDiceFromString("1d100");
  ASSERT_GE(result, 1);
  ASSERT_LE(result, 100);
  ```

**Test Suite 3: Seed Reproducibility**

- [ ] **Test_SetSeed_Reproducible_SameSeed()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // First run with seed 12345
  dice.SetSeed(12345);
  int result1 = dice.RollDice(10, 6);
  
  // Second run with SAME seed
  dice.SetSeed(12345);
  int result2 = dice.RollDice(10, 6);
  
  // Results MUST be identical
  ASSERT_EQ(result1, result2);
  ```

- [ ] **Test_SetSeed_Reproducible_Sequence()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Roll 10 dice with seed
  dice.SetSeed(99999);
  std::vector<int> sequence1;
  for (int i = 0; i < 10; ++i) {
      sequence1.push_back(dice.RollDice(1, 20));
  }
  
  // Roll same 10 dice with SAME seed
  dice.SetSeed(99999);
  std::vector<int> sequence2;
  for (int i = 0; i < 10; ++i) {
      sequence2.push_back(dice.RollDice(1, 20));
  }
  
  // Entire sequence MUST match
  ASSERT_EQ(sequence1, sequence2);
  ```

- [ ] **Test_SetSeed_DifferentSeeds()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Seed 1
  dice.SetSeed(111);
  int result1 = dice.RollDice(100, 20);
  
  // Seed 2 (different)
  dice.SetSeed(222);
  int result2 = dice.RollDice(100, 20);
  
  // Results SHOULD be different (not guaranteed but extremely likely)
  // If they're the same, something is wrong with RNG
  ASSERT_NE(result1, result2);
  ```

**Test Suite 4: GetLastRolls() Verification**

- [ ] **Test_GetLastRolls_SingleDie()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  dice.RollDice(1, 6);
  std::vector<int> lastRolls = dice.GetLastRolls();
  
  ASSERT_EQ(lastRolls.size(), 1);
  ASSERT_GE(lastRolls[0], 1);
  ASSERT_LE(lastRolls[0], 6);
  ```

- [ ] **Test_GetLastRolls_MultipleDice()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  int total = dice.RollDice(5, 8);
  std::vector<int> lastRolls = dice.GetLastRolls();
  
  // Should have 5 individual rolls
  ASSERT_EQ(lastRolls.size(), 5);
  
  // Sum of individual rolls should equal total
  int sum = 0;
  for (int roll : lastRolls) {
      ASSERT_GE(roll, 1);
      ASSERT_LE(roll, 8);
      sum += roll;
  }
  ASSERT_EQ(sum, total);
  ```

- [ ] **Test_GetLastRolls_FromString()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  dice.RollDiceFromString("3d6+5");
  std::vector<int> lastRolls = dice.GetLastRolls();
  
  // Should have 3 dice (modifier not included in lastRolls)
  ASSERT_EQ(lastRolls.size(), 3);
  
  for (int roll : lastRolls) {
      ASSERT_GE(roll, 1);
      ASSERT_LE(roll, 6);
  }
  ```

**Test Suite 5: Logging Integration**

- [ ] **Test_Logging_DiceRolls()**
  
  ```cpp
  DiceManager& dice = DiceManager::Instance();
  
  // Roll dice (should log to Engine::GetLogger())
  dice.RollDiceFromString("3d6");
  
  // Verify log contains dice roll information
  // (Check Logger.txt for entry like "Rolled 3d6 = [4, 6, 2] = 12")
  // This is a manual verification test
  ```

**Usage Examples**:

**Example 1: Basic Dice Rolling for Damage**

```cpp
DiceManager& dice = DiceManager::Instance();

// Roll damage for a sword attack (1d8)
int swordDamage = dice.RollDice(1, 8);
std::cout << "Sword deals " << swordDamage << " damage" << std::endl;
// Output: Sword deals 5 damage

// Roll damage for a fireball (3d6)
int fireballDamage = dice.RollDice(3, 6);
std::cout << "Fireball deals " << fireballDamage << " damage" << std::endl;
// Output: Fireball deals 12 damage

// Show individual dice
std::vector<int> rolls = dice.GetLastRolls();
std::cout << "Individual dice: ";
for (int roll : rolls) {
    std::cout << roll << " ";
}
std::cout << std::endl;
// Output: Individual dice: 4 6 2
```

**Example 2: String Notation for Complex Rolls**

```cpp
DiceManager& dice = DiceManager::Instance();

// Attack roll with modifier (1d20 + attack bonus)
int attackRoll = dice.RollDiceFromString("1d20+5");
std::cout << "Attack roll: " << attackRoll << std::endl;
// Output: Attack roll: 18 (rolled 13, +5 bonus)

// Damage with static bonus (2d6+3)
int damage = dice.RollDiceFromString("2d6+3");
std::cout << "Damage: " << damage << std::endl;
// Output: Damage: 10 (rolled [4,3], +3 bonus)

// Healing potion (2d4+2)
int healing = dice.RollDiceFromString("2d4+2");
std::cout << "Healed " << healing << " HP" << std::endl;
// Output: Healed 8 HP
```

**Example 3: Initiative Rolling**

```cpp
DiceManager& dice = DiceManager::Instance();

// Roll initiative for multiple characters
struct InitiativeResult {
    std::string name;
    int speed;
    int roll;
    int total;
};

std::vector<InitiativeResult> initiatives;

// Dragon
int dragonRoll = dice.RollDice(1, 20);
initiatives.push_back({"Dragon", 5, dragonRoll, dragonRoll + 5});

// Fighter
int fighterRoll = dice.RollDice(1, 20);
initiatives.push_back({"Fighter", 3, fighterRoll, fighterRoll + 3});

// Wizard
int wizardRoll = dice.RollDice(1, 20);
initiatives.push_back({"Wizard", 2, wizardRoll, wizardRoll + 2});

// Sort by total (highest first)
std::sort(initiatives.begin(), initiatives.end(),
    [](auto& a, auto& b) { return a.total > b.total; });

// Display turn order
std::cout << "=== Initiative Order ===" << std::endl;
for (const auto& init : initiatives) {
    std::cout << init.name << ": " << init.roll << " + " << init.speed
              << " = " << init.total << std::endl;
}
// Output:
// === Initiative Order ===
// Dragon: 15 + 5 = 20
// Fighter: 12 + 3 = 15
// Wizard: 8 + 2 = 10
```

**Example 4: Reproducible Dice for Testing**

```cpp
DiceManager& dice = DiceManager::Instance();

// Set seed for reproducible testing
dice.SetSeed(12345);

// Now dice rolls are reproducible
int damage1 = dice.RollDiceFromString("3d6");
std::cout << "First test damage: " << damage1 << std::endl;
// Output: First test damage: 12

// Reset seed to same value
dice.SetSeed(12345);

// Same roll gives same result!
int damage2 = dice.RollDiceFromString("3d6");
std::cout << "Second test damage: " << damage2 << std::endl;
// Output: Second test damage: 12

ASSERT_EQ(damage1, damage2);  // Passes!
```

**Example 5: Critical Hits (Natural 20)**

```cpp
DiceManager& dice = DiceManager::Instance();

// Roll attack
int attackRoll = dice.RollDice(1, 20);
std::vector<int> lastRolls = dice.GetLastRolls();
int naturalRoll = lastRolls[0];  // The actual d20 result

if (naturalRoll == 20) {
    std::cout << "🎯 CRITICAL HIT!" << std::endl;

    // Double damage dice on crit
    int normalDamage = dice.RollDiceFromString("2d6");
    int critDamage = dice.RollDiceFromString("2d6");
    int totalDamage = normalDamage + critDamage;

    std::cout << "Critical damage: " << totalDamage << std::endl;
} else if (naturalRoll == 1) {
    std::cout << "💥 CRITICAL FUMBLE!" << std::endl;
} else {
    std::cout << "Normal attack: " << attackRoll << std::endl;
}
```

**Example 6: Verbose Dice Logging**

```cpp
DiceManager& dice = DiceManager::Instance();

// Helper function for detailed logging
auto rollWithLogging = [&dice](const std::string& notation) {
    int result = dice.RollDiceFromString(notation);
    std::vector<int> rolls = dice.GetLastRolls();

    std::cout << "Rolling " << notation << ":" << std::endl;
    std::cout << "  Individual dice: [";
    for (size_t i = 0; i < rolls.size(); ++i) {
        std::cout << rolls[i];
        if (i < rolls.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "  Total: " << result << std::endl;

    return result;
};

// Use it
int damage = rollWithLogging("4d6+2");
// Output:
// Rolling 4d6+2:
//   Individual dice: [5, 3, 6, 2]
//   Total: 18
```

**Example 7: Saving Throw with Difficulty Class (DC)**

```cpp
DiceManager& dice = DiceManager::Instance();

int DC = 15;  // Difficulty Class
int savingThrowBonus = 3;

int roll = dice.RollDice(1, 20);
int total = roll + savingThrowBonus;

std::cout << "Saving Throw: " << roll << " + " << savingThrowBonus
          << " = " << total << " vs DC " << DC << std::endl;

if (total >= DC) {
    std::cout << "✓ SUCCESS! Take half damage." << std::endl;
} else {
    std::cout << "✗ FAILURE! Take full damage." << std::endl;
}
// Output:
// Saving Throw: 14 + 3 = 17 vs DC 15
// ✓ SUCCESS! Take half damage.
```

**Dependencies**: None (uses C++ STL `<random>`)

---

### Week 1: Developer E - DebugConsole Framework

**Goal**: Core debug infrastructure with console command system

**Files to Create**:

```
CS230/Game/Debug/DebugManager.h
CS230/Game/Debug/DebugManager.cpp
CS230/Game/Debug/DebugConsole.h
CS230/Game/Debug/DebugConsole.cpp
```

**Implementation Tasks**:

- [ ] **DebugManager singleton**
  
  ```cpp
  class DebugManager {
  public:
      static DebugManager& Instance();
  
      void SetDebugMode(bool enabled);
      bool IsDebugMode() const;
  
      void ToggleGridOverlay();
      void ToggleCollisionBoxes();
      void ToggleStatusInfo();
  };
  ```

- [ ] **DebugConsole UI**
  
  ```cpp
  class DebugConsole : public CS230::Component {
  public:
      void Update(double dt) override;
      void Draw(Math::TransformationMatrix camera_matrix) override;
  
      void ToggleConsole();  // Tilde (~) key
      bool IsOpen() const;
  };
  ```

- [ ] **Command registration system**
  
  ```cpp
  void RegisterCommand(
      const std::string& name,
      std::function<void(std::vector<std::string>)> handler,
      const std::string& helpText
  );
  
  void ExecuteCommand(const std::string& commandLine);
  ```

- [ ] **Basic commands (Week 1)**
  
  ```cpp
  help                  // Show all commands
  help <command>        // Show detailed help
  clear                 // Clear console output
  log <message>         // Write to game log
  listcommands          // List all registered commands
  ```

**Usage Example**:

```cpp
// Toggle console
~                       // Open/close console

// Execute commands
help                    // Display all available commands
help spawn              // Display help for "spawn" command
clear                   // Clear console output
log Test message        // Write "Test message" to Logger.txt
listcommands            // Display all registered commands
```

**Rigorous Testing**:

**Test Suite 1: Console Toggle & UI**

- [ ] **Test_ToggleConsole_Open()**
  
  ```cpp
  // Setup
  DebugConsole console;
  ASSERT_FALSE(console.IsOpen());
  
  // Action: Simulate ~ key press
  // (Manually trigger toggle method in test)
  console.ToggleConsole();
  
  // Assertions
  ASSERT_TRUE(console.IsOpen());
  Engine::GetLogger().LogEvent("Console opened successfully");
  ```

- [ ] **Test_ToggleConsole_Close()**
  
  ```cpp
  // Setup: Console already open
  DebugConsole console;
  console.ToggleConsole();  // Open it
  ASSERT_TRUE(console.IsOpen());
  
  // Action: Toggle again to close
  console.ToggleConsole();
  
  // Assertions
  ASSERT_FALSE(console.IsOpen());
  Engine::GetLogger().LogEvent("Console closed successfully");
  ```

- [ ] **Test_ToggleConsole_MultipleToggles()**
  
  ```cpp
  // Stress test: Toggle 10 times
  DebugConsole console;
  
  for (int i = 0; i < 10; ++i) {
      bool expectedState = (i % 2 == 1);
      console.ToggleConsole();
      ASSERT_EQ(console.IsOpen(), expectedState);
  }
  ```

**Test Suite 2: Command Registration & Execution**

- [ ] **Test_RegisterCommand_SingleCommand()**
  
  ```cpp
  // Setup
  DebugConsole console;
  bool callbackInvoked = false;
  
  console.RegisterCommand("test",
      [&callbackInvoked](std::vector<std::string> args) {
          callbackInvoked = true;
      },
      "Test command"
  );
  
  // Action: Execute command
  console.ExecuteCommand("test");
  
  // Assertions
  ASSERT_TRUE(callbackInvoked);
  ```

- [ ] **Test_RegisterCommand_MultipleCommands()**
  
  ```cpp
  // Setup
  DebugConsole console;
  int cmd1Count = 0, cmd2Count = 0, cmd3Count = 0;
  
  console.RegisterCommand("cmd1", [&](auto args) { cmd1Count++; }, "Command 1");
  console.RegisterCommand("cmd2", [&](auto args) { cmd2Count++; }, "Command 2");
  console.RegisterCommand("cmd3", [&](auto args) { cmd3Count++; }, "Command 3");
  
  // Execute different commands
  console.ExecuteCommand("cmd1");
  console.ExecuteCommand("cmd2");
  console.ExecuteCommand("cmd3");
  console.ExecuteCommand("cmd1");  // Execute cmd1 again
  
  // Verify correct callbacks invoked
  ASSERT_EQ(cmd1Count, 2);
  ASSERT_EQ(cmd2Count, 1);
  ASSERT_EQ(cmd3Count, 1);
  ```

- [ ] **Test_ExecuteCommand_WithArguments()**
  
  ```cpp
  // Setup
  DebugConsole console;
  std::vector<std::string> receivedArgs;
  
  console.RegisterCommand("testargs",
      [&receivedArgs](std::vector<std::string> args) {
          receivedArgs = args;
      },
      "Test command with arguments"
  );
  
  // Action: Execute command with arguments
  console.ExecuteCommand("testargs arg1 arg2 arg3");
  
  // Assertions
  ASSERT_EQ(receivedArgs.size(), 3);
  ASSERT_EQ(receivedArgs[0], "arg1");
  ASSERT_EQ(receivedArgs[1], "arg2");
  ASSERT_EQ(receivedArgs[2], "arg3");
  ```

- [ ] **Test_ExecuteCommand_UnknownCommand()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action: Execute non-existent command (should log error, not crash)
  console.ExecuteCommand("nonexistent");
  
  // If we get here, test passed (no crash)
  ASSERT_TRUE(true);
  
  // Verify error logged
  // Check Logger.txt contains "Unknown command: nonexistent"
  ```

**Test Suite 3: Built-in Commands**

- [ ] **Test_HelpCommand_NoArgs()**
  
  ```cpp
  // Setup
  DebugConsole console;
  console.RegisterCommand("cmd1", [](auto args) {}, "Help text 1");
  console.RegisterCommand("cmd2", [](auto args) {}, "Help text 2");
  
  // Action
  console.ExecuteCommand("help");
  
  // Verify: Check Logger.txt contains:
  // - "cmd1 - Help text 1"
  // - "cmd2 - Help text 2"
  ```

- [ ] **Test_HelpCommand_WithArg()**
  
  ```cpp
  // Setup
  DebugConsole console;
  console.RegisterCommand("spawn", [](auto args) {}, "Spawn a character at specified position");
  
  // Action: Get help for specific command
  console.ExecuteCommand("help spawn");
  
  // Verify: Check Logger.txt contains detailed help for "spawn" command
  ```

- [ ] **Test_ClearCommand()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Add some console output
  Engine::GetLogger().LogEvent("Line 1");
  Engine::GetLogger().LogEvent("Line 2");
  Engine::GetLogger().LogEvent("Line 3");
  
  // Action
  console.ExecuteCommand("clear");
  
  // Verify: Console output buffer cleared
  // (Check internal console buffer is empty)
  ```

- [ ] **Test_LogCommand()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action
  console.ExecuteCommand("log Test message from console");
  
  // Verify: Check Logger.txt contains "Test message from console"
  ```

- [ ] **Test_ListCommandsCommand()**
  
  ```cpp
  // Setup
  DebugConsole console;
  console.RegisterCommand("cmd1", [](auto args) {}, "Command 1");
  console.RegisterCommand("cmd2", [](auto args) {}, "Command 2");
  console.RegisterCommand("cmd3", [](auto args) {}, "Command 3");
  
  // Action
  console.ExecuteCommand("listcommands");
  
  // Verify: Check Logger.txt lists all registered commands
  // Should include: help, clear, log, listcommands, cmd1, cmd2, cmd3
  ```

**Test Suite 4: Edge Cases**

- [ ] **Test_ExecuteCommand_EmptyString()**
  
  ```cpp
  DebugConsole console;
  
  // Should not crash
  console.ExecuteCommand("");
  
  ASSERT_TRUE(true);
  ```

- [ ] **Test_ExecuteCommand_OnlySpaces()**
  
  ```cpp
  DebugConsole console;
  
  console.ExecuteCommand("   ");
  
  ASSERT_TRUE(true);
  ```

- [ ] **Test_RegisterCommand_DuplicateName()**
  
  ```cpp
  DebugConsole console;
  
  console.RegisterCommand("test", [](auto args) {}, "First registration");
  
  // Register same name again (should log warning)
  console.RegisterCommand("test", [](auto args) {}, "Second registration");
  
  // Verify: Check Logger.txt contains warning about duplicate command
  ```

**Usage Examples**:

**Example 1: Opening and Closing the Console**

```cpp
// In-game usage
// Player presses ~ key to open console
if (Engine::GetInput().IsKeyPressed(InputKey::Tilde)) {
    debugConsole.ToggleConsole();
}

// Check if console is open
if (debugConsole.IsOpen()) {
    std::cout << "Console is open. Type 'help' for commands." << std::endl;
}

// Close console
debugConsole.ToggleConsole();
```

**Example 2: Registering Custom Commands**

```cpp
DebugConsole console;

// Register spawn command
console.RegisterCommand("spawn",
    [](std::vector<std::string> args) {
        if (args.size() != 3) {
            Engine::GetLogger().LogError("Usage: spawn <type> <x> <y>");
            return;
        }

        std::string type = args[0];
        int x = std::stoi(args[1]);
        int y = std::stoi(args[2]);

        // Spawn character at position
        Engine::GetLogger().LogEvent("Spawning " + type + " at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    },
    "Spawn a character at specified position. Usage: spawn <type> <x> <y>"
);

// Usage in console:
// > spawn dragon 4 4
// Output: Spawning dragon at (4, 4)
```

**Example 3: Using Built-in Commands**

```cpp
// Open console and execute commands
DebugConsole console;
console.ToggleConsole();

// Get help
console.ExecuteCommand("help");
// Output: Lists all available commands

// Get help for specific command
console.ExecuteCommand("help spawn");
// Output: Spawn a character at specified position. Usage: spawn <type> <x> <y>

// Clear console
console.ExecuteCommand("clear");
// Output: Console cleared

// Log message
console.ExecuteCommand("log Game started successfully");
// Output: "Game started successfully" written to Logger.txt

// List all commands
console.ExecuteCommand("listcommands");
// Output: help, clear, log, listcommands, spawn, ...
```

**Example 4: Creating a Debug Menu**

```cpp
DebugConsole console;

// Register multiple debug commands
console.RegisterCommand("godmode",
    [](std::vector<std::string> args) {
        // Enable god mode (invincibility)
        Engine::GetLogger().LogEvent("God mode enabled");
    },
    "Enable invincibility"
);

console.RegisterCommand("kill",
    [](std::vector<std::string> args) {
        if (args.size() != 1) {
            Engine::GetLogger().LogError("Usage: kill <character>");
            return;
        }
        Engine::GetLogger().LogEvent("Killing " + args[0]);
    },
    "Instantly kill a character. Usage: kill <character>"
);

console.RegisterCommand("teleport",
    [](std::vector<std::string> args) {
        if (args.size() != 2) {
            Engine::GetLogger().LogError("Usage: teleport <x> <y>");
            return;
        }
        int x = std::stoi(args[0]);
        int y = std::stoi(args[1]);
        Engine::GetLogger().LogEvent("Teleporting to (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    },
    "Teleport player to position. Usage: teleport <x> <y>"
);

// Usage:
// > godmode
// > kill enemy1
// > teleport 5 5
```

**Example 5: Integrating with DebugManager**

```cpp
DebugConsole console;

// Register toggle commands for DebugManager
console.RegisterCommand("togglegrid",
    [](std::vector<std::string> args) {
        DebugManager::Instance().ToggleGridOverlay();
        Engine::GetLogger().LogEvent("Grid overlay toggled");
    },
    "Toggle grid overlay visualization"
);

console.RegisterCommand("togglecollision",
    [](std::vector<std::string> args) {
        DebugManager::Instance().ToggleCollisionBoxes();
        Engine::GetLogger().LogEvent("Collision boxes toggled");
    },
    "Toggle collision box visualization"
);

console.RegisterCommand("togglestatus",
    [](std::vector<std::string> args) {
        DebugManager::Instance().ToggleStatusInfo();
        Engine::GetLogger().LogEvent("Status info toggled");
    },
    "Toggle character status info display"
);

// Usage:
// > togglegrid      // Show/hide grid
// > togglecollision // Show/hide collision boxes
// > togglestatus    // Show/hide character HP/AP
```

**Example 6: Complex Command with Error Handling**

```cpp
DebugConsole console;

console.RegisterCommand("damage",
    [](std::vector<std::string> args) {
        // Validate arguments
        if (args.size() != 2) {
            Engine::GetLogger().LogError("Usage: damage <character> <amount>");
            return;
        }

        std::string characterName = args[0];
        int damageAmount = 0;

        // Parse damage amount with error handling
        try {
            damageAmount = std::stoi(args[1]);
        } catch (const std::exception& e) {
            Engine::GetLogger().LogError("Invalid damage amount: " + args[1]);
            return;
        }

        // Validate damage amount
        if (damageAmount < 0) {
            Engine::GetLogger().LogError("Damage amount must be positive");
            return;
        }

        // Apply damage
        Engine::GetLogger().LogEvent("Dealing " + std::to_string(damageAmount) + " damage to " + characterName);
        // ... actual damage logic ...
    },
    "Deal damage to a character. Usage: damage <character> <amount>"
);

// Usage:
// > damage dragon 30    // Valid
// > damage dragon xyz   // Error: Invalid damage amount
// > damage dragon -10   // Error: Damage must be positive
// > damage dragon       // Error: Usage: damage <character> <amount>
```

**Dependencies**: None (uses CS230::Input)

---

### Week 1 Deliverable & Verification

**End of Week 1**: All 5 core systems **fully implemented and functional**

**🎯 RIGOROUS VERIFICATION (Week 1 Completion Checklist)**

**Compilation Tests**:

- [ ] Test_AllSystemsCompile()
  - Setup: Include all system headers in BattleState test file
  - Expected: Compiles without errors
  - Verify: No syntax errors, no missing dependencies

**Individual System Tests**:

- [ ] Test_CharacterBaseClass() (Dev A)
  
  - Create Character instance (concrete test class)
  - Test TakeDamage(), Heal(), GetHP(), RefreshActionPoints()
  - Expected: All methods work correctly

- [ ] Test_GridSystem() (Dev B)
  
  - Test IsValidTile({0,0}), IsValidTile({8,8})
  - Test PlaceCharacter(), IsOccupied()
  - Expected: Tile validation works, occupancy tracking works

- [ ] Test_EventBus() (Dev C)
  
  - Subscribe to DamageTakenEvent
  - Publish DamageTakenEvent
  - Expected: Subscriber receives event callback

- [ ] Test_DiceManager() (Dev D)
  
  - Test RollDice(3, 6) → result in [3, 18]
  - Test RollDiceFromString("2d8+5") → result in [7, 21]
  - Expected: Dice rolls work correctly

- [ ] Test_DebugConsole() (Dev E)
  
  - Open console with ~ key
  - Execute "help" command
  - Expected: Console opens, help command displays

**Integration Tests** (Friday Week 1 - Replace Mocks with Real Classes):

- [ ] Test_CharacterOnGrid()
  
  - Setup: Create **real Character** (not MockCharacter), place on GridSystem at (4,4)
  - Expected: GridSystem->IsOccupied({4,4}) == true
  - **Note**: This tests the actual Character↔GridSystem interface

- [ ] Test_EventBusWithRealCharacter()
  
  - Setup: Subscribe to DamageTakenEvent
  - Action: **Real Character**->TakeDamage(30)
  - Expected: EventBus publishes event automatically, subscriber receives it
  - **Note**: This tests Character publishing events to EventBus

- [ ] Test_DiceManagerInConsole()
  
  - Setup: **Real DebugConsole** with **real Engine::GetLogger()**
  - Action: Execute "roll 3d6" command
  - Expected: DiceManager called, result logged to Logger.txt
  - **Note**: This tests DebugConsole↔DiceManager↔Logger integration

**Acceptance Criteria (Week 1)**:

- [ ] All 5 systems compile successfully
- [ ] All 5 systems tested individually **with mocks** (Days 1-4)
- [ ] All 5 systems tested individually **with real classes** (Friday)
- [ ] At least 3 integration tests pass (Friday)
- [ ] All code committed to repository
- [ ] No developer blocked for Week 2
- [ ] **Week1TestMocks.h** created and used by all developers

**Week 1 Exit Interview** (5-minute check with each developer):

- **Question 1**: "Does your system compile and run without crashes?"
- **Question 2**: "Have you tested your system's core functionality?"
- **Question 3**: "Are you ready to integrate with other systems in Week 2?"
- **Expected Answer**: YES to all 3 questions from all 5 developers

**If ANY developer answers NO**: Week 1 NOT complete, fix issues before proceeding to Week 2.

---
