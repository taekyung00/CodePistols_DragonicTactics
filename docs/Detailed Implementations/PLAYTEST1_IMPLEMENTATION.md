# Playtest 1 Implementation Plan (Weeks 1-5)

**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Foundation Layer + Dragon Playable + First Playtest
**Timeline**: Weeks 1-5 (First playtest milestone)
**Strategy**: Task-based parallel development - all 5 developers work together on critical path

**Last Updated**: 2025-10-06

**Related Documentation**: See [docs/implementation-plan.md](../implementation-plan.md) for complete 26-week timeline

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
- [Week 2: Dragon + Grid + Dice Integration](#week-2-dragon--grid--dice-integration)
  - [Developer A: Dragon Class](#week-2-developer-a-dragon-class)
  - [Developer B: GridSystem Pathfinding](#week-2-developer-b-gridsystem-pathfinding)
  - [Developer C: TurnManager Basics](#week-2-developer-c-turnmanager-basics)
  - [Developer D: DiceManager Testing](#week-2-developer-d-dicemanager-testing)
  - [Developer E: GridDebugRenderer](#week-2-developer-e-griddebugrenderer)
  - [Week 2 Integration Test](#week-2-integration-test)
- [Week 3: Combat + Spells Begin](#week-3-combat--spells-begin)
  - [Developer A: Dragon Fireball Spell](#week-3-developer-a-dragon-fireball-spell)
  - [Developer B: CombatSystem Basics](#week-3-developer-b-combatsystem-basics)
  - [Developer C: SpellSystem Foundation](#week-3-developer-c-spellsystem-foundation)
  - [Developer D: Fighter Enemy Class](#week-3-developer-d-fighter-enemy-class)
  - [Developer E: Combat Debug Tools](#week-3-developer-e-combat-debug-tools)
  - [Week 3 Integration Test](#week-3-integration-test)
- [Week 4: More Spells + Turn System](#week-4-more-spells--turn-system)
  - [Developer A: Dragon CreateWall + LavaPool](#week-4-developer-a-dragon-createwall--lavapool)
  - [Developer B: TurnManager Initiative](#week-4-developer-b-turnmanager-initiative)
  - [Developer C: Fighter Manual Control](#week-4-developer-c-fighter-manual-control)
  - [Developer D: DataRegistry Basics](#week-4-developer-d-dataregistry-basics)
  - [Developer E: Turn Debug Tools](#week-4-developer-e-turn-debug-tools)
  - [Week 4 Integration Test](#week-4-integration-test)
- [Week 5: Polish + PLAYTEST 1](#week-5-polish--playtest-1)
  - [Developer A: Dragon Polish + Balance](#week-5-developer-a-dragon-polish--balance)
  - [Developer B: Combat System Complete](#week-5-developer-b-combat-system-complete)
  - [Developer C: Fighter Manual Control Polish](#week-5-developer-c-fighter-manual-control-polish)
  - [Developer D: Data-Driven Dragon Stats](#week-5-developer-d-data-driven-dragon-stats)
  - [Developer E: Playtest Build Integration](#week-5-developer-e-playtest-build-integration)
  - [Playtest 1 Execution](#playtest-1-execution)
- [Playtest 1 Deliverables (End of Week 5)](#playtest-1-deliverables-end-of-week-5)

---

## Week 1: Foundation (All 5 Systems in Parallel)

**Goal**: Build 5 core systems simultaneously - all real implementations, no stubs!

**Philosophy**: Instead of creating stubs → real implementations, we build everything REAL from Day 1. All 5 developers work in parallel on independent systems.

**End of Week 1**: All 5 core systems **fully implemented and functional**

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

**Rigorous Testing**:

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
  GridSystem grid;
  TestCharacter character;
  
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
  TestCharacter char1, char2, char3;
  
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
  TestCharacter char1, char2;
  
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
  TestCharacter character;
  
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
CS230/Game/Types/Events.h
```

**Implementation Tasks**:

- [ ] **EventBus singleton class**
  
  ```cpp
  class EventBus {
  public:
      static EventBus& Instance();
  
      template<typename T>
      void Subscribe(std::function<void(const T&)> callback);
  
      template<typename T>
      void Publish(const T& event);
  
      void Clear();  // Clear all subscriptions
  
  private:
      std::map<std::type_index, std::vector<std::function<void(const void*)>>> subscribers;
  };
  ```

- [ ] **Define all event types** (in Events.h):
  
  ```cpp
  struct DamageTakenEvent {
      Character* victim;
      int damage;
      Character* attacker;
  };
  
  struct CharacterDiedEvent {
      Character* deadCharacter;
      Character* killer;
  };
  
  struct TurnStartedEvent {
      Character* activeCharacter;
      int turnNumber;
  };
  
  struct TurnEndedEvent {
      Character* finishedCharacter;
  };
  
  struct SpellCastEvent {
      Character* caster;
      std::string spellName;
      int level;
      Math::vec2 targetTile;
  };
  
  struct MovementEvent {
      Character* character;
      Math::vec2 fromTile;
      Math::vec2 toTile;
  };
  
  // Define 10+ event types total
  ```

**Usage Example**:

```cpp
// Subscribe to events
EventBus::Instance().Subscribe<DamageTakenEvent>([](const DamageTakenEvent& e) {
    Engine::GetLogger().LogEvent("Character took " + std::to_string(e.damage) + " damage");
});

// Publish events
DamageTakenEvent event{victim, 30, attacker};
EventBus::Instance().Publish(event);
```

**Rigorous Testing**:

**Test Suite 1: Basic Pub/Sub Functionality**

- [ ] **Test_Subscribe_Publish_SingleSubscriber()**
  
  ```cpp
  EventBus::Instance().Clear();  // Start fresh
  
  // Setup: Subscribe to DamageTakenEvent
  bool callbackInvoked = false;
  int receivedDamage = 0;
  Character* receivedVictim = nullptr;
  
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      callbackInvoked = true;
      receivedDamage = e.damage;
      receivedVictim = e.victim;
  });
  
  // Action: Publish event
  TestCharacter character;
  DamageTakenEvent event{&character, 30, nullptr};
  EventBus::Instance().Publish(event);
  
  // Assertions
  ASSERT_TRUE(callbackInvoked);
  ASSERT_EQ(receivedDamage, 30);
  ASSERT_EQ(receivedVictim, &character);
  ```

- [ ] **Test_MultipleSubscribers_SameEvent()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: 3 subscribers to same event
  int callback1Count = 0;
  int callback2Count = 0;
  int callback3Count = 0;
  
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      callback1Count++;
  });
  
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      callback2Count++;
  });
  
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      callback3Count++;
  });
  
  // Action: Publish event
  TestCharacter character;
  EventBus::Instance().Publish(DamageTakenEvent{&character, 10, nullptr});
  
  // All 3 callbacks should be invoked
  ASSERT_EQ(callback1Count, 1);
  ASSERT_EQ(callback2Count, 1);
  ASSERT_EQ(callback3Count, 1);
  ```

- [ ] **Test_MultipleDifferentEvents()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: Subscribe to 3 different event types
  bool damageCalled = false;
  bool deathCalled = false;
  bool spellCalled = false;
  
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      damageCalled = true;
  });
  
  EventBus::Instance().Subscribe<CharacterDiedEvent>([&](const CharacterDiedEvent& e) {
      deathCalled = true;
  });
  
  EventBus::Instance().Subscribe<SpellCastEvent>([&](const SpellCastEvent& e) {
      spellCalled = true;
  });
  
  // Publish all 3 events
  TestCharacter character;
  EventBus::Instance().Publish(DamageTakenEvent{&character, 10, nullptr});
  EventBus::Instance().Publish(CharacterDiedEvent{&character, nullptr});
  EventBus::Instance().Publish(SpellCastEvent{&character, "Fireball", 1, {0,0}});
  
  // All 3 should be called
  ASSERT_TRUE(damageCalled);
  ASSERT_TRUE(deathCalled);
  ASSERT_TRUE(spellCalled);
  ```

**Test Suite 2: Event Data Integrity**

- [ ] **Test_EventData_CompleteTransfer()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup: Complex event with all fields
  TestCharacter victim, attacker;
  DamageTakenEvent receivedEvent;
  
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      receivedEvent = e;  // Copy event
  });
  
  // Publish
  DamageTakenEvent originalEvent{&victim, 42, &attacker};
  EventBus::Instance().Publish(originalEvent);
  
  // Verify all fields transferred correctly
  ASSERT_EQ(receivedEvent.victim, &victim);
  ASSERT_EQ(receivedEvent.damage, 42);
  ASSERT_EQ(receivedEvent.attacker, &attacker);
  ```

- [ ] **Test_EventData_MultiplePublishes()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Setup
  std::vector<int> damages;
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      damages.push_back(e.damage);
  });
  
  // Publish multiple events
  TestCharacter character;
  EventBus::Instance().Publish(DamageTakenEvent{&character, 10, nullptr});
  EventBus::Instance().Publish(DamageTakenEvent{&character, 20, nullptr});
  EventBus::Instance().Publish(DamageTakenEvent{&character, 30, nullptr});
  
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
  
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      damageCount++;
  });
  
  EventBus::Instance().Subscribe<CharacterDiedEvent>([&](const CharacterDiedEvent& e) {
      deathCount++;
  });
  
  // Clear all subscriptions
  EventBus::Instance().Clear();
  
  // Publish events (should NOT trigger callbacks)
  TestCharacter character;
  EventBus::Instance().Publish(DamageTakenEvent{&character, 10, nullptr});
  EventBus::Instance().Publish(CharacterDiedEvent{&character, nullptr});
  
  // Callbacks should NOT be invoked
  ASSERT_EQ(damageCount, 0);
  ASSERT_EQ(deathCount, 0);
  ```

- [ ] **Test_Publish_NoSubscribers()**
  
  ```cpp
  EventBus::Instance().Clear();
  
  // Publish event with NO subscribers (should not crash)
  TestCharacter character;
  EventBus::Instance().Publish(DamageTakenEvent{&character, 10, nullptr});
  
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
      EventBus::Instance().Subscribe<DamageTakenEvent>([&, i](const DamageTakenEvent& e) {
          callbackCounts[i]++;
      });
  }
  
  // Publish event
  TestCharacter character;
  EventBus::Instance().Publish(DamageTakenEvent{&character, 10, nullptr});
  
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
  EventBus::Instance().Subscribe<DamageTakenEvent>([&](const DamageTakenEvent& e) {
      count++;
  });
  
  // Publish 1000 events
  TestCharacter character;
  for (int i = 0; i < 1000; ++i) {
      EventBus::Instance().Publish(DamageTakenEvent{&character, i, nullptr});
  }
  
  // All 1000 should be received
  ASSERT_EQ(count, 1000);
  ```

**Usage Examples**:

**Example 1: Basic Damage Logging**

```cpp
// Subscribe to damage events for logging
EventBus::Instance().Subscribe<DamageTakenEvent>([](const DamageTakenEvent& e) {
    std::string msg = e.victim->TypeName() + " took " + std::to_string(e.damage) + " damage";
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
EventBus::Instance().Subscribe<CharacterDiedEvent>([](const CharacterDiedEvent& e) {
    std::cout << "💀 " << e.deadCharacter->TypeName() << " has been slain!" << std::endl;

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
    std::cout << "Active Character: " << e.activeCharacter->TypeName() << std::endl;
    std::cout << "HP: " << e.activeCharacter->GetCurrentHP() << "/" << e.activeCharacter->GetMaxHP() << std::endl;
    std::cout << "AP: " << e.activeCharacter->GetActionPoints() << std::endl;
});

EventBus::Instance().Subscribe<TurnEndedEvent>([](const TurnEndedEvent& e) {
    std::cout << e.finishedCharacter->TypeName() << " ended their turn." << std::endl;
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
        std::cout << "   🔥 FIREBALL VFX at (" << e.targetTile.x << ", " << e.targetTile.y << ")" << std::endl;
    } else if (e.spellName == "LavaPool") {
        std::cout << "   🌋 LAVA VFX at (" << e.targetTile.x << ", " << e.targetTile.y << ")" << std::endl;
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
        EventBus::Instance().Subscribe<DamageTakenEvent>([this](const DamageTakenEvent& e) {
            totalDamageDealt += e.damage;
        });

        EventBus::Instance().Subscribe<CharacterDiedEvent>([this](const CharacterDiedEvent& e) {
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
EventBus::Instance().Subscribe<DamageTakenEvent>([](const DamageTakenEvent& e) {
    Engine::GetLogger().LogDebug("[EVENT] DamageTaken: " + std::to_string(e.damage));
});

EventBus::Instance().Subscribe<CharacterDiedEvent>([](const CharacterDiedEvent& e) {
    Engine::GetLogger().LogDebug("[EVENT] CharacterDied: " + e.deadCharacter->TypeName());
});

EventBus::Instance().Subscribe<TurnStartedEvent>([](const TurnStartedEvent& e) {
    Engine::GetLogger().LogDebug("[EVENT] TurnStarted: Turn " + std::to_string(e.turnNumber));
});

// Now Logger.txt contains complete event log for debugging!
```

**Dependencies**: None (uses C++ STL: `<typeindex>`, `<functional>`, `<map>`)

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

- [ ] Test_ToggleConsole()
  
  - Input: Press ~ key
  - Expected: Console opens
  - Input: Press ~ again
  - Expected: Console closes

- [ ] Test_HelpCommand()
  
  - Input: help
  - Expected: Displays all registered commands

- [ ] Test_LogCommand()
  
  - Input: log Test message
  - Expected: "Test message" written to Logger.txt

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

**Integration Tests** (Friday Week 1):

- [ ] Test_CharacterOnGrid()
  
  - Setup: Create Character, place on GridSystem at (4,4)
  - Expected: GridSystem->IsOccupied({4,4}) == true

- [ ] Test_EventBusWithCharacter()
  
  - Setup: Subscribe to DamageTakenEvent
  - Action: Character->TakeDamage(30)
  - Expected: Event published and received

- [ ] Test_DiceManagerInConsole()
  
  - Setup: DebugConsole open
  - Action: Execute "roll 3d6" command
  - Expected: DiceManager called, result displayed in console

**Acceptance Criteria (Week 1)**:

- [ ] All 5 systems compile successfully
- [ ] All 5 systems tested individually
- [ ] At least 3 integration tests pass
- [ ] All code committed to repository
- [ ] No developer blocked for Week 2

**Week 1 Exit Interview** (5-minute check with each developer):

- **Question 1**: "Does your system compile and run without crashes?"
- **Question 2**: "Have you tested your system's core functionality?"
- **Question 3**: "Are you ready to integrate with other systems in Week 2?"
- **Expected Answer**: YES to all 3 questions from all 5 developers

**If ANY developer answers NO**: Week 1 NOT complete, fix issues before proceeding to Week 2.

---

## Week 2: Dragon + Grid + Dice Integration

**Goal**: All systems connect, Dragon becomes playable with movement

**End of Week 2**: Dragon can spawn on grid, move using pathfinding, and end turn

---

### Week 2: Developer A - Dragon Class

**Goal**: Create Dragon character class extending Character base class

**Files to Create**:

```
CS230/Game/Characters/Dragon.h
CS230/Game/Characters/Dragon.cpp
```

**Implementation Tasks**:

- [ ] **Dragon class definition**
  
  ```cpp
  class Dragon : public Character {
  public:
      Dragon();
  
      GameObjectTypes Type() override { return GameObjectTypes::Dragon; }
      std::string TypeName() override { return "Dragon"; }
  
      void Update(double dt) override;
      void Draw(Math::TransformationMatrix camera_matrix) override;
  
      void MoveToTile(Math::vec2 targetTile);
  };
  ```

- [ ] **Dragon stats (hardcoded for now, JSON in Week 4)**
  
  ```cpp
  Dragon::Dragon() {
      maxHP = 140;
      currentHP = 140;
      speed = 5;
      maxActionPoints = 5;
      currentActionPoints = 5;
      baseAttackPower = 5;
      attackDice = "3d6";
      baseDefensePower = 3;
      defenseDice = "2d8";
      attackRange = 1;
      spellSlots = {{1, 4}, {2, 3}, {3, 2}};  // Level 1: 4 slots, etc.
      gridPosition = {4, 4};  // Start at center
  }
  ```

- [ ] **Dragon movement integration**
  
  ```cpp
  void Dragon::MoveToTile(Math::vec2 targetTile) {
      // Get GridSystem from GameState
      GridSystem* grid = GetGSComponent<GridSystem>();
  
      // Validate move
      if (!grid->IsValidMove(gridPosition, targetTile)) {
          Engine::GetLogger().LogError("Invalid move!");
          return;
      }
  
      // Get path
      std::vector<Math::vec2> path = grid->FindPath(gridPosition, targetTile);
  
      // Move along path (for now, instant teleport)
      Math::vec2 oldPos = gridPosition;
      gridPosition = targetTile;
      grid->PlaceCharacter(this, gridPosition);
  
      // Publish movement event
      EventBus::Instance().Publish(MovementEvent{this, oldPos, gridPosition});
  
      // Consume action points
      ConsumeActionPoints(1);
  }
  ```

**Rigorous Testing**:

- [ ] Test_Dragon_Spawn()
  
  - Action: Create Dragon
  - Expected: HP=140, AP=5, speed=5, gridPosition={4,4}

- [ ] Test_Dragon_MoveToTile()
  
  - Setup: Dragon at (4,4)
  - Input: MoveToTile({5,4})
  - Expected: gridPosition == {5,4}, AP == 4, MovementEvent published

**Dependencies**: Character (Week 1), GridSystem (Week 1), EventBus (Week 1)

---

### Week 2: Developer B - GridSystem Pathfinding

**Goal**: Implement A* pathfinding algorithm for movement

**Files to Update**:

```
CS230/Game/Systems/GridSystem.h
CS230/Game/Systems/GridSystem.cpp
```

**Implementation Tasks**:

- [ ] **A* pathfinding algorithm**
  
  ```cpp
  std::vector<Math::vec2> FindPath(Math::vec2 from, Math::vec2 to) const;
  
  // Helper methods
  int GetMovementCost(Math::vec2 from, Math::vec2 to) const;
  int Heuristic(Math::vec2 from, Math::vec2 to) const;  // Manhattan distance
  std::vector<Math::vec2> GetNeighbors(Math::vec2 tile) const;
  ```

- [ ] **Reachable tiles calculation**
  
  ```cpp
  std::vector<Math::vec2> GetReachableTiles(Character* character) const {
      int movementRange = character->GetMovementRange();
      std::vector<Math::vec2> reachable;
  
      // Flood fill from character position within movement range
      // Account for walls, occupied tiles, difficult terrain
  
      return reachable;
  }
  ```

**Rigorous Testing**:

- [ ] Test_FindPath_StraightLine()
  
  - Input: FindPath({0,0}, {4,0})
  - Expected: Path = [{0,0}, {1,0}, {2,0}, {3,0}, {4,0}]

- [ ] Test_FindPath_AroundWall()
  
  - Setup: Wall at (2,0), (2,1), (2,2)
  - Input: FindPath({0,1}, {4,1})
  - Expected: Path goes around wall (not through it)

- [ ] Test_GetReachableTiles()
  
  - Setup: Character with movement=3 at (4,4)
  - Expected: Returns all tiles within 3 tile distance

**Dependencies**: GridSystem (Week 1)

---

### Week 2: Developer C - TurnManager Basics

**Goal**: Create TurnManager GSComponent for turn order management

**Files to Create**:

```
CS230/Game/Systems/TurnManager.h
CS230/Game/Systems/TurnManager.cpp
```

**Implementation Tasks**:

- [ ] **TurnManager GSComponent**
  
  ```cpp
  class TurnManager : public CS230::Component {
  public:
      void Update(double dt) override;
  
      void InitializeTurnOrder(std::vector<Character*> characters);
      void StartNextTurn();
      void EndCurrentTurn();
  
      Character* GetActiveCharacter() const;
      int GetCurrentTurnNumber() const;
  
  private:
      std::vector<Character*> turnOrder;
      int currentTurnIndex = 0;
      int turnNumber = 1;
      Character* activeCharacter = nullptr;
  };
  ```

- [ ] **Basic turn progression (no initiative yet, Week 4)**
  
  ```cpp
  void TurnManager::InitializeTurnOrder(std::vector<Character*> characters) {
      turnOrder = characters;  // Simple order for now
      currentTurnIndex = 0;
      activeCharacter = turnOrder[0];
      turnNumber = 1;
  
      EventBus::Instance().Publish(TurnStartedEvent{activeCharacter, turnNumber});
  }
  
  void TurnManager::EndCurrentTurn() {
      EventBus::Instance().Publish(TurnEndedEvent{activeCharacter});
      StartNextTurn();
  }
  
  void TurnManager::StartNextTurn() {
      currentTurnIndex = (currentTurnIndex + 1) % turnOrder.size();
  
      if (currentTurnIndex == 0) {
          turnNumber++;
      }
  
      activeCharacter = turnOrder[currentTurnIndex];
      activeCharacter->RefreshActionPoints();
  
      EventBus::Instance().Publish(TurnStartedEvent{activeCharacter, turnNumber});
  }
  ```

**Rigorous Testing**:

- [ ] Test_TurnOrder_Basic()
  
  - Setup: 2 characters (Dragon, Fighter)
  - Input: InitializeTurnOrder([dragon, fighter])
  - Expected: activeCharacter == dragon, turnNumber == 1

- [ ] Test_TurnProgression()
  
  - Action: EndCurrentTurn()
  - Expected: activeCharacter changes to Fighter
  - Action: EndCurrentTurn()
  - Expected: Wraps to Dragon, turnNumber == 2

**Dependencies**: Character (Week 1), EventBus (Week 1)

---

### Week 2: Developer D - DiceManager Testing

**Goal**: Add console command for testing dice rolls

**Files to Update**:

```
CS230/Game/Debug/DebugConsole.cpp (add roll command)
```

**Implementation Tasks**:

- [ ] **Register "roll" command**
  
  ```cpp
  void DebugConsole::Init() {
      RegisterCommand("roll",
          [](std::vector<std::string> args) {
              if (args.size() != 1) {
                  Engine::GetLogger().LogError("Usage: roll <dice notation>");
                  return;
              }
  
              std::string notation = args[0];
              int result = DiceManager::Instance().RollDiceFromString(notation);
              std::vector<int> rolls = DiceManager::Instance().GetLastRolls();
  
              std::string rollsStr = "[";
              for (int i = 0; i < rolls.size(); ++i) {
                  rollsStr += std::to_string(rolls[i]);
                  if (i < rolls.size() - 1) rollsStr += ", ";
              }
              rollsStr += "]";
  
              Engine::GetLogger().LogEvent("Rolled " + notation + " = " + rollsStr + " = " + std::to_string(result));
          },
          "Roll dice using D&D notation (e.g., roll 3d6, roll 2d8+5)"
      );
  }
  ```

- [ ] **Register "setseed" command**
  
  ```cpp
  RegisterCommand("setseed",
      [](std::vector<std::string> args) {
          if (args.size() != 1) {
              Engine::GetLogger().LogError("Usage: setseed <number>");
              return;
          }
  
          int seed = std::stoi(args[0]);
          DiceManager::Instance().SetSeed(seed);
          Engine::GetLogger().LogEvent("Set dice seed to " + std::to_string(seed));
      },
      "Set RNG seed for reproducible dice rolls"
  );
  ```

**Rigorous Testing**:

- [ ] Test_RollCommand()
  
  - Input: roll 3d6
  - Expected: Console displays "Rolled 3d6 = [4, 6, 2] = 12" (example)

- [ ] Test_SetSeedCommand()
  
  - Input: setseed 12345
  - Input: roll 3d6
  - Expected: Reproducible result

**Dependencies**: DiceManager (Week 1), DebugConsole (Week 1)

---

### Week 2: Developer E - GridDebugRenderer

**Goal**: Visual grid overlay for debugging

**Files to Create**:

```
CS230/Game/Debug/GridDebugRenderer.h
CS230/Game/Debug/GridDebugRenderer.cpp
```

**Implementation Tasks**:

- [ ] **GridDebugRenderer component**
  
  ```cpp
  class GridDebugRenderer : public CS230::Component {
  public:
      void Draw(Math::TransformationMatrix camera_matrix) override;
  
      void SetShowGrid(bool show);
      void SetShowCoordinates(bool show);
  
  private:
      bool showGrid = false;
      bool showCoordinates = false;
  };
  ```

- [ ] **Drawing methods**
  
  ```cpp
  void DrawGridLines();         // 8x8 grid lines overlay
  void DrawTileCoordinates();   // (0,0) to (7,7) labels on each tile
  ```

- [ ] **Keyboard shortcuts**
  
  ```cpp
  // F1: Toggle grid overlay
  if (Engine::GetInput().IsKeyPressed(InputKey::F1)) {
      showGrid = !showGrid;
  }
  ```

**Rigorous Testing**:

- [ ] Test_GridOverlay()
  - Input: Press F1
  - Expected: Grid lines visible
  - Input: Press F1 again
  - Expected: Grid lines hidden

**Dependencies**: GridSystem (Week 1)

---

### Week 2 Integration Test

**Integration Test (Friday Week 2)**:

**Test Cases**:

1. **Spawn Dragon on grid**
   
   - Console: `spawn dragon 4 4`
   - Expected: Dragon appears at center of grid

2. **Move Dragon**
   
   - Console: `move dragon 5 5`
   - Expected: Dragon moves to (5,5), path shown via GridDebugRenderer

3. **End turn**
   
   - Console: `endturn`
   - Expected: Turn progresses (if Fighter exists, switches to Fighter turn)

**Success Criteria**:

- [ ] Dragon spawns without crash
- [ ] Dragon moves correctly
- [ ] TurnManager progresses turns
- [ ] Grid overlay shows movement

---

## Week 3: Combat + Spells Begin

**Goal**: Dragon can cast Fireball spell, combat damage works

**End of Week 3**: Dragon vs Fighter combat with Fireball spell functional

---

### Week 3: Developer A - Dragon Fireball Spell

**Goal**: Implement Dragon's first spell (Fireball)

**Files to Update**:

```
CS230/Game/Characters/Dragon.h
CS230/Game/Characters/Dragon.cpp
```

**Implementation Tasks**:

- [ ] **Fireball spell method**
  
  ```cpp
  void Dragon::Spell_Fireball(int upcastLevel, Math::vec2 targetTile) {
      // 1. Validate spell slot
      if (spellSlots[upcastLevel] <= 0) {
          Engine::GetLogger().LogError("No spell slot for level " + std::to_string(upcastLevel));
          return;
      }
  
      // 2. Get GridSystem
      GridSystem* grid = GetGSComponent<GridSystem>();
  
      // 3. Calculate damage (base 2d6, +1d6 per upcast level)
      std::string damageRoll = "2d6";
      for (int i = 1; i < upcastLevel; ++i) {
          damageRoll += "+1d6";
      }
  
      int damage = DiceManager::Instance().RollDiceFromString(damageRoll);
  
      // 4. Get targets in AOE (2x2 square for now)
      std::vector<Math::vec2> aoeTiles = grid->GetTilesInSquare(targetTile, 2);
      std::vector<Character*> targets = grid->GetCharactersInArea(aoeTiles);
  
      // 5. Apply damage to each target
      for (Character* target : targets) {
          target->TakeDamage(damage);
          EventBus::Instance().Publish(DamageTakenEvent{target, damage, this});
      }
  
      // 6. Consume resources
      spellSlots[upcastLevel]--;
      ConsumeActionPoints(1);
  
      // 7. Log and publish event
      Engine::GetLogger().LogEvent("Dragon cast Fireball (Level " + std::to_string(upcastLevel) + ") for " + std::to_string(damage) + " damage");
      EventBus::Instance().Publish(SpellCastEvent{this, "Fireball", upcastLevel, targetTile});
  }
  ```

**Rigorous Testing**:

- [ ] Test_Spell_Fireball_BaseLevel()
  
  - Setup: Dragon at (4,4), Fighter at (6,4) with HP=90
  - Input: Spell_Fireball(1, {6,4})
  - Expected: Fighter HP reduced by 2d6 damage, spell slot consumed

- [ ] Test_Spell_Fireball_NoSlot()
  
  - Setup: Dragon with 0 level 1 spell slots
  - Input: Spell_Fireball(1, {6,4})
  - Expected: Error logged, spell does NOT execute

**Dependencies**: DiceManager (Week 1), GridSystem (Week 2), EventBus (Week 1)

---

### Week 3: Developer B - CombatSystem Basics

**Goal**: Create CombatSystem GSComponent for damage calculation

**Files to Create**:

```
CS230/Game/Systems/CombatSystem.h
CS230/Game/Systems/CombatSystem.cpp
```

**Implementation Tasks**:

- [ ] **CombatSystem GSComponent**
  
  ```cpp
  class CombatSystem : public CS230::Component {
  public:
      void Update(double dt) override;
  
      int CalculateDamage(Character* attacker, Character* target, const std::string& attackDice);
      void ApplyDamage(Character* attacker, Character* target, int damage);
  };
  ```

- [ ] **Damage calculation**
  
  ```cpp
  int CombatSystem::CalculateDamage(Character* attacker, Character* target, const std::string& attackDice) {
      // Roll attack dice
      int attackRoll = DiceManager::Instance().RollDiceFromString(attackDice);
      int attackPower = attacker->GetBaseAttackPower() + attackRoll;
  
      // Roll defense dice
      int defenseRoll = DiceManager::Instance().RollDiceFromString(target->GetDefenseDice());
      int defensePower = target->GetBaseDefensePower() + defenseRoll;
  
      // Calculate final damage (minimum 0)
      int damage = std::max(0, attackPower - defensePower);
  
      Engine::GetLogger().LogEvent("Attack: " + std::to_string(attackPower) + " vs Defense: " + std::to_string(defensePower) + " = " + std::to_string(damage) + " damage");
  
      return damage;
  }
  
  void CombatSystem::ApplyDamage(Character* attacker, Character* target, int damage) {
      target->TakeDamage(damage);
      EventBus::Instance().Publish(DamageTakenEvent{target, damage, attacker});
  }
  ```

**Rigorous Testing**:

- [ ] Test_CalculateDamage()
  - Setup: Attacker (attack=5, dice="3d6"), Target (defense=3, dice="2d8")
  - Action: CalculateDamage(attacker, target, "3d6")
  - Expected: Damage = (5 + 3d6) - (3 + 2d8), minimum 0

**Dependencies**: DiceManager (Week 1), Character (Week 1), EventBus (Week 1)

---

### Week 3: Developer C - SpellSystem Foundation

**Goal**: Create SpellSystem GSComponent for spell management

**Files to Create**:

```
CS230/Game/Systems/SpellSystem.h
CS230/Game/Systems/SpellSystem.cpp
```

**Implementation Tasks**:

- [ ] **SpellSystem GSComponent**
  
  ```cpp
  class SpellSystem : public CS230::Component {
  public:
      void Update(double dt) override;
  
      bool CanCastSpell(Character* caster, int spellLevel);
      void CastSpell(Character* caster, const std::string& spellName, int upcastLevel, Math::vec2 targetTile);
  };
  ```

- [ ] **Spell validation**
  
  ```cpp
  bool SpellSystem::CanCastSpell(Character* caster, int spellLevel) {
      // Check spell slots
      if (caster->GetSpellSlots()[spellLevel] <= 0) {
          return false;
      }
  
      // Check action points
      if (caster->GetActionPoints() < 1) {
          return false;
      }
  
      return true;
  }
  ```

**Rigorous Testing**:

- [ ] Test_CanCastSpell()
  - Setup: Character with level 1 slots = 0
  - Expected: CanCastSpell(character, 1) == false

**Dependencies**: Character (Week 1)

---

### Week 3: Developer D - Fighter Enemy Class

**Goal**: Create Fighter character class (first enemy)

**Files to Create**:

```
CS230/Game/Characters/Fighter.h
CS230/Game/Characters/Fighter.cpp
```

**Implementation Tasks**:

- [ ] **Fighter class definition**
  
  ```cpp
  class Fighter : public Character {
  public:
      Fighter();
  
      GameObjectTypes Type() override { return GameObjectTypes::Fighter; }
      std::string TypeName() override { return "Fighter"; }
  
      void Update(double dt) override;
      void Draw(Math::TransformationMatrix camera_matrix) override;
  
      void BasicAttack(Character* target);
  };
  ```

- [ ] **Fighter stats (hardcoded for now)**
  
  ```cpp
  Fighter::Fighter() {
      maxHP = 90;
      currentHP = 90;
      speed = 3;
      maxActionPoints = 3;
      currentActionPoints = 3;
      baseAttackPower = 4;
      attackDice = "2d6";
      baseDefensePower = 4;
      defenseDice = "2d6";
      attackRange = 1;  // Melee only
      gridPosition = {0, 0};  // Set via spawn command
  }
  ```

- [ ] **Basic attack method**
  
  ```cpp
  void Fighter::BasicAttack(Character* target) {
      CombatSystem* combat = GetGSComponent<CombatSystem>();
      int damage = combat->CalculateDamage(this, target, attackDice);
      combat->ApplyDamage(this, target, damage);
      ConsumeActionPoints(1);
  }
  ```

**Rigorous Testing**:

- [ ] Test_Fighter_Spawn()
  
  - Action: Create Fighter
  - Expected: HP=90, AP=3, speed=3

- [ ] Test_Fighter_BasicAttack()
  
  - Setup: Fighter at (0,0), Dragon at (1,0)
  - Input: BasicAttack(dragon)
  - Expected: Damage calculated, Dragon HP decreases

**Dependencies**: Character (Week 1), CombatSystem (Week 3)

---

### Week 3: Developer E - Combat Debug Tools

**Goal**: Add console commands for testing combat

**Files to Update**:

```
CS230/Game/Debug/DebugConsole.cpp
```

**Implementation Tasks**:

- [ ] **Register "damage" command**
  
  ```cpp
  RegisterCommand("damage",
      [](std::vector<std::string> args) {
          if (args.size() != 2) {
              Engine::GetLogger().LogError("Usage: damage <character> <amount>");
              return;
          }
  
          std::string characterName = args[0];
          int amount = std::stoi(args[1]);
  
          Character* character = FindCharacter(characterName);
          if (character) {
              character->TakeDamage(amount);
              Engine::GetLogger().LogEvent(characterName + " took " + std::to_string(amount) + " damage");
          }
      },
      "Deal damage to a character (e.g., damage dragon 30)"
  );
  ```

- [ ] **Register "heal" command**
  
  ```cpp
  RegisterCommand("heal",
      [](std::vector<std::string> args) {
          if (args.size() != 2) {
              Engine::GetLogger().LogError("Usage: heal <character> <amount>");
              return;
          }
  
          std::string characterName = args[0];
          int amount = std::stoi(args[1]);
  
          Character* character = FindCharacter(characterName);
          if (character) {
              character->Heal(amount);
              Engine::GetLogger().LogEvent(characterName + " healed " + std::to_string(amount) + " HP");
          }
      },
      "Heal a character (e.g., heal fighter 20)"
  );
  ```

- [ ] **Register "castspell" command**
  
  ```cpp
  RegisterCommand("castspell",
      [](std::vector<std::string> args) {
          // Usage: castspell dragon fireball 1 6 4
          // (character, spell, level, targetX, targetY)
          if (args.size() != 5) {
              Engine::GetLogger().LogError("Usage: castspell <character> <spell> <level> <x> <y>");
              return;
          }
  
          std::string characterName = args[0];
          std::string spellName = args[1];
          int level = std::stoi(args[2]);
          int x = std::stoi(args[3]);
          int y = std::stoi(args[4]);
  
          Character* character = FindCharacter(characterName);
          Dragon* dragon = dynamic_cast<Dragon*>(character);
          if (dragon && spellName == "fireball") {
              dragon->Spell_Fireball(level, {x, y});
          }
      },
      "Cast a spell (e.g., castspell dragon fireball 1 6 4)"
  );
  ```

**Rigorous Testing**:

- [ ] Test_DamageCommand()
  
  - Input: damage dragon 30
  - Expected: Dragon HP decreases by 30

- [ ] Test_CastSpellCommand()
  
  - Input: castspell dragon fireball 1 6 4
  - Expected: Fireball cast at tile (6,4)

**Dependencies**: Dragon (Week 2), DebugConsole (Week 1)

---

### Week 3 Integration Test

**Integration Test (Friday Week 3)**:

**Test Cases**:

1. **Dragon vs Fighter on grid**
   
   - Console: `spawn dragon 4 4`
   - Console: `spawn fighter 6 4`
   - Expected: Both characters on grid

2. **Dragon casts Fireball**
   
   - Console: `castspell dragon fireball 1 6 4`
   - Expected:
     - Damage calculated with dice rolls (2d6)
     - Fighter HP decreases
     - Console shows damage roll details

3. **Manual damage/heal**
   
   - Console: `damage fighter 20`
   - Expected: Fighter HP -20
   - Console: `heal fighter 10`
   - Expected: Fighter HP +10

**Success Criteria**:

- [ ] Fireball spell casts successfully
- [ ] Dice-based damage works
- [ ] Combat debug commands functional
- [ ] Fighter takes damage correctly

---

## Week 4: More Spells + Turn System

**Goal**: Dragon has all 3 spells, turn-based combat with initiative works

**End of Week 4**: Full combat loop (Dragon vs Fighter) with turn order

---

### Week 4: Developer A - Dragon CreateWall + LavaPool

**Goal**: Implement Dragon's remaining 2 spells

**Files to Update**:

```
CS230/Game/Characters/Dragon.cpp
```

**Implementation Tasks**:

- [ ] **CreateWall spell**
  
  ```cpp
  void Dragon::Spell_CreateWall(int upcastLevel, Math::vec2 targetTile) {
      if (spellSlots[upcastLevel] <= 0) return;
  
      GridSystem* grid = GetGSComponent<GridSystem>();
  
      // Base wall length = 1 tile, +2 tiles per upcast level
      int wallLength = 1 + (upcastLevel - 1) * 2;
  
      // Create horizontal wall
      for (int i = 0; i < wallLength; ++i) {
          Math::vec2 wallTile = {targetTile.x + i, targetTile.y};
          if (grid->IsValidTile(wallTile)) {
              grid->SetTileType(wallTile, TileType::Wall);
          }
      }
  
      spellSlots[upcastLevel]--;
      ConsumeActionPoints(1);
  
      Engine::GetLogger().LogEvent("Dragon cast CreateWall (Level " + std::to_string(upcastLevel) + ")");
      EventBus::Instance().Publish(SpellCastEvent{this, "CreateWall", upcastLevel, targetTile});
  }
  ```

- [ ] **LavaPool spell**
  
  ```cpp
  void Dragon::Spell_LavaPool(int upcastLevel, Math::vec2 targetTile) {
      if (spellSlots[upcastLevel] <= 0) return;
  
      GridSystem* grid = GetGSComponent<GridSystem>();
  
      // Create lava tile
      grid->SetTileType(targetTile, TileType::Lava);
  
      // Lava deals 1d6 damage per turn to characters standing on it
      // (damage handling in GridSystem Update, Week 5)
  
      spellSlots[upcastLevel]--;
      ConsumeActionPoints(1);
  
      Engine::GetLogger().LogEvent("Dragon cast LavaPool (Level " + std::to_string(upcastLevel) + ")");
      EventBus::Instance().Publish(SpellCastEvent{this, "LavaPool", upcastLevel, targetTile});
  }
  ```

**Rigorous Testing**:

- [ ] Test_Spell_CreateWall()
  
  - Input: Spell_CreateWall(1, {5,4})
  - Expected: 1 wall tile at (5,4)
  - Verify: IsWalkable({5,4}) == false

- [ ] Test_Spell_LavaPool()
  
  - Input: Spell_LavaPool(1, {2,2})
  - Expected: Tile (2,2) becomes TileType::Lava

**Dependencies**: GridSystem (Week 2), EventBus (Week 1)

---

### Week 4: Developer B - TurnManager Initiative

**Goal**: Add initiative rolling (1d20 + speed) to TurnManager

**Files to Update**:

```
CS230/Game/Systems/TurnManager.cpp
```

**Implementation Tasks**:

- [ ] **Initiative rolling**
  
  ```cpp
  void TurnManager::InitializeTurnOrder(std::vector<Character*> characters) {
      // Roll initiative for each character
      std::vector<std::pair<Character*, int>> initiatives;
  
      for (Character* character : characters) {
          int initiativeRoll = DiceManager::Instance().RollDice(1, 20);
          int initiative = character->GetSpeed() + initiativeRoll;
          initiatives.push_back({character, initiative});
  
          Engine::GetLogger().LogEvent(
              character->TypeName() + " rolled " + std::to_string(initiativeRoll) +
              " + " + std::to_string(character->GetSpeed()) + " = " + std::to_string(initiative)
          );
      }
  
      // Sort by initiative (highest first)
      std::sort(initiatives.begin(), initiatives.end(),
          [](auto& a, auto& b) { return a.second > b.second; });
  
      // Build turn order
      turnOrder.clear();
      for (auto& [character, init] : initiatives) {
          turnOrder.push_back(character);
      }
  
      currentTurnIndex = 0;
      activeCharacter = turnOrder[0];
      turnNumber = 1;
  
      EventBus::Instance().Publish(TurnStartedEvent{activeCharacter, turnNumber});
  }
  ```

**Rigorous Testing**:

- [ ] Test_Initiative_Rolling()
  - Setup: Dragon (speed=5), Fighter (speed=3)
  - Action: InitializeTurnOrder([dragon, fighter])
  - Expected: Turn order sorted by initiative (highest first)

**Dependencies**: DiceManager (Week 1), Character (Week 1)

---

### Week 4: Developer C - Fighter Manual Control

**Goal**: Allow manual control of Fighter via console commands for testing

**Files to Update**:

```
CS230/Game/Debug/DebugConsole.cpp
```

**Implementation Tasks**:

- [ ] **Register "attack" command**
  
  ```cpp
  RegisterCommand("attack",
      [](std::vector<std::string> args) {
          if (args.size() != 2) {
              Engine::GetLogger().LogError("Usage: attack <attacker> <target>");
              return;
          }
  
          std::string attackerName = args[0];
          std::string targetName = args[1];
  
          Character* attacker = FindCharacter(attackerName);
          Character* target = FindCharacter(targetName);
  
          Fighter* fighter = dynamic_cast<Fighter*>(attacker);
          if (fighter && target) {
              fighter->BasicAttack(target);
              Engine::GetLogger().LogEvent(attackerName + " attacked " + targetName);
          }
      },
      "Attack a target (e.g., attack fighter dragon)"
  );
  ```

- [ ] **Register "move" command**
  
  ```cpp
  RegisterCommand("move",
      [](std::vector<std::string> args) {
          if (args.size() != 3) {
              Engine::GetLogger().LogError("Usage: move <character> <x> <y>");
              return;
          }
  
          std::string characterName = args[0];
          int x = std::stoi(args[1]);
          int y = std::stoi(args[2]);
  
          Character* character = FindCharacter(characterName);
          Dragon* dragon = dynamic_cast<Dragon*>(character);
          if (dragon) {
              dragon->MoveToTile({x, y});
          }
      },
      "Move character to tile (e.g., move dragon 5 5)"
  );
  ```

**Rigorous Testing**:

- [ ] Test_AttackCommand()
  - Input: attack fighter dragon
  - Expected: Fighter attacks Dragon, damage applied

**Dependencies**: Fighter (Week 3), DebugConsole (Week 1)

---

### Week 4: Developer D - DataRegistry Basics

**Goal**: Create DataRegistry singleton for JSON data loading (basic version)

**Files to Create**:

```
CS230/Game/Singletons/DataRegistry.h
CS230/Game/Singletons/DataRegistry.cpp
Assets/Data/dragon_stats.json
```

**Implementation Tasks**:

- [ ] **DataRegistry singleton (basic, no hot-reload yet)**
  
  ```cpp
  class DataRegistry {
  public:
      static DataRegistry& Instance();
  
      void LoadFromFile(const std::string& filepath);
  
      template<typename T>
      T GetValue(const std::string& key, const T& defaultValue = T()) const;
  
  private:
      std::map<std::string, nlohmann::json> loadedFiles;
  };
  ```

- [ ] **Create dragon_stats.json**
  
  ```json
  {
    "Dragon": {
      "maxHP": 140,
      "speed": 5,
      "maxActionPoints": 5,
      "baseAttackPower": 5,
      "attackDice": "3d6",
      "baseDefensePower": 3,
      "defenseDice": "2d8",
      "attackRange": 1,
      "spellSlots": {
        "level1": 4,
        "level2": 3,
        "level3": 2
      }
    }
  }
  ```

**Note**: Dragon will load stats from JSON in Week 5

**Rigorous Testing**:

- [ ] Test_LoadFromFile()
  
  - Action: LoadFromFile("Assets/Data/dragon_stats.json")
  - Expected: JSON parsed successfully

- [ ] Test_GetValue()
  
  - Setup: Load dragon_stats.json
  - Input: GetValue<int>("Dragon.maxHP", 100)
  - Expected: Returns 140

**Dependencies**: None (uses nlohmann/json library)

---

### Week 4: Developer E - Turn Debug Tools

**Goal**: Add debug tools for turn management

**Files to Update**:

```
CS230/Game/Debug/DebugConsole.cpp
```

**Implementation Tasks**:

- [ ] **Register "showturnorder" command**
  
  ```cpp
  RegisterCommand("showturnorder",
      [](std::vector<std::string> args) {
          TurnManager* turnMgr = GetGSComponent<TurnManager>();
          std::vector<Character*> turnOrder = turnMgr->GetTurnOrder();
  
          Engine::GetLogger().LogEvent("=== Turn Order ===");
          for (int i = 0; i < turnOrder.size(); ++i) {
              std::string marker = (i == turnMgr->GetCurrentTurnIndex()) ? " <-- ACTIVE" : "";
              Engine::GetLogger().LogEvent(std::to_string(i+1) + ". " + turnOrder[i]->TypeName() + marker);
          }
      },
      "Display current turn order"
  );
  ```

- [ ] **Register "endturn" command**
  
  ```cpp
  RegisterCommand("endturn",
      [](std::vector<std::string> args) {
          TurnManager* turnMgr = GetGSComponent<TurnManager>();
          turnMgr->EndCurrentTurn();
          Engine::GetLogger().LogEvent("Turn ended, next character's turn begins");
      },
      "End current turn and move to next character"
  );
  ```

**Rigorous Testing**:

- [ ] Test_ShowTurnOrder()
  
  - Input: showturnorder
  - Expected: Console displays turn order with active character marked

- [ ] Test_EndTurn()
  
  - Input: endturn
  - Expected: Turn progresses to next character

**Dependencies**: TurnManager (Week 2)

---

### Week 4 Integration Test

**Integration Test (Friday Week 4)**:

**Test Cases**:

1. **Full combat loop: Dragon vs Fighter**
   
   - Console: `spawn dragon 4 4`
   - Console: `spawn fighter 6 6`
   - Console: `startbattle`
   - Expected: Initiative rolled, turn order displayed

2. **Initiative determines turn order**
   
   - Expected console output:
     
     ```
     Dragon rolled 15 + 5 = 20
     Fighter rolled 12 + 3 = 15
     Turn Order: Dragon, Fighter
     Dragon's turn (AP: 5)
     ```

3. **Dragon uses 3 spells**
   
   - Console: `castspell dragon fireball 1 6 6`
   - Console: `castspell dragon createwall 1 5 5`
   - Console: `castspell dragon lavapool 1 3 3`
   - Expected: All 3 spells work

4. **Manual control of Fighter**
   
   - Console: `endturn` (switch to Fighter)
   - Console: `move fighter 5 5`
   - Console: `attack fighter dragon`
   - Expected: Fighter moves and attacks

**Success Criteria**:

- [ ] Full combat loop functional
- [ ] Initiative rolling works
- [ ] All 3 Dragon spells work
- [ ] Fighter can be controlled manually

---

## Week 5: Polish + PLAYTEST 1

**Goal**: Dragon vs Fighter battle ready for full playtest

**End of Week 5**: PLAYTEST 1 - Full team plays Dragon vs Fighter battle

---

### Week 5: Developer A - Dragon Polish + Balance

**Goal**: Balance Dragon spells and fix bugs

**Tasks**:

- [ ] Adjust spell damage values (test with Fighter HP)
- [ ] Adjust spell costs (AP and spell slot consumption)
- [ ] Fix any Dragon movement bugs
- [ ] Test all 3 spells against Fighter

**Deliverable**: Dragon balanced and polished

---

### Week 5: Developer B - Combat System Complete

**Goal**: Finalize damage formulas and death handling

**Tasks**:

- [ ] Finalize damage formula (ensure minimum 0 damage)
- [ ] Add death handling (character dies when HP reaches 0)
- [ ] Subscribe to CharacterDiedEvent, remove from grid
- [ ] Test edge cases (0 damage, overkill damage)

**Deliverable**: Combat system fully functional

---

### Week 5: Developer C - Fighter Manual Control Polish

**Goal**: Polish Fighter console commands

**Tasks**:

- [ ] Improve console command error messages
- [ ] Add validation (range checks, AP checks)
- [ ] Add "showstats" command (display HP, AP, position)

**Deliverable**: Fighter fully controllable via console

---

### Week 5: Developer D - Data-Driven Dragon Stats

**Goal**: Dragon loads stats from JSON

**Tasks**:

- [ ] Update Dragon constructor to load from dragon_stats.json
- [ ] Test hot-reload (modify JSON, reload game)
- [ ] Validate JSON data

**Deliverable**: Dragon stats loaded from JSON

---

### Week 5: Developer E - Playtest Build Integration

**Goal**: Integrate all systems in BattleState for playtest

**Files to Create**:

```
CS230/Game/States/BattleState.h
CS230/Game/States/BattleState.cpp
```

**Tasks**:

- [ ] Create BattleState GameState
- [ ] Add all GSComponents (GridSystem, TurnManager, CombatSystem, SpellSystem)
- [ ] Spawn Dragon and Fighter on Load()
- [ ] Initialize turn order
- [ ] Integrate DebugConsole
- [ ] Test full battle loop

**Deliverable**: Playtest build ready (all systems connected)

---

### Playtest 1 Execution

**When**: Week 5 Friday, 2:00 PM - 3:30 PM (90 minutes)

**Format**:

1. **Build Integration** (30 min before, 1:30 PM): Dev E final build
2. **Playtest** (30 min, 2:00-2:30 PM): All 5 devs play
3. **Bug Triage** (30 min, 2:30-3:00 PM): Categorize and assign bugs
4. **Balance Discussion** (15 min, 3:00-3:15 PM): Is Dragon too strong/weak?
5. **Action Items** (15 min, 3:15-3:30 PM): Commit to bug fixes

**Test Plan**:

1. **Start Battle**
   
   - [ ] Dragon spawns at (4, 4)
   - [ ] Fighter spawns at (6, 6)
   - [ ] HP bars show correctly (Dragon: 140/140, Fighter: 90/90)

2. **Movement Test**
   
   - [ ] Console: `move dragon 5 5`
   - [ ] Expected: Dragon moves, AP decreases to 4

3. **Fireball Test**
   
   - [ ] Console: `castspell dragon fireball 1 6 6`
   - [ ] Expected: Fireball hits Fighter, damage shown, HP decreases

4. **CreateWall Test**
   
   - [ ] Console: `castspell dragon createwall 1 5 4`
   - [ ] Expected: Wall created, blocks movement

5. **LavaPool Test**
   
   - [ ] Console: `castspell dragon lavapool 1 3 3`
   - [ ] Expected: Lava appears

6. **Fighter Attack Test**
   
   - [ ] Console: `endturn` (switch to Fighter)
   - [ ] Console: `move fighter 5 5`
   - [ ] Console: `attack fighter dragon`
   - [ ] Expected: Fighter attacks, Dragon HP decreases

7. **Turn Progression Test**
   
   - [ ] Console: `endturn`
   - [ ] Expected: Turn wraps to Dragon, turn counter increments

**Success Criteria**:

- ✅ Dragon spawns without crash
- ✅ Dragon can move
- ✅ Dragon can cast 3 spells
- ✅ Fighter can attack
- ✅ Turn-based combat works
- ✅ Play for 5 minutes without crash

**Bug Triage**:

- **Critical**: Crash, softlock → Fix within 1 day
- **Major**: Broken feature → Fix within 1 week
- **Minor**: Visual glitch → Fix when time permits

---

## Playtest 1 Deliverables (End of Week 5)

**Summary**: By end of Week 5, after Playtest 1, the following systems are complete:

### ✅ Core Systems Complete

| System                | Owner | Status     | Description                                             |
| --------------------- | ----- | ---------- | ------------------------------------------------------- |
| **Character (base)**  | Dev A | ✅ Complete | Base class with HP, movement, AP, state machine         |
| **Dragon**            | Dev A | ✅ Complete | 3 spells (Fireball, CreateWall, LavaPool), movement     |
| **Fighter**           | Dev D | ✅ Complete | Basic attack, manual control                            |
| **GridSystem**        | Dev B | ✅ Complete | 8x8 grid, pathfinding (A*), tile validation             |
| **TurnManager**       | Dev C | ✅ Complete | Initiative rolling, turn order, AP refresh              |
| **CombatSystem**      | Dev B | ✅ Complete | Dice-based damage, death handling                       |
| **SpellSystem**       | Dev C | ✅ Complete | Spell slot management, spell validation                 |
| **EventBus**          | Dev C | ✅ Complete | Pub/sub system, 10+ event types                         |
| **DiceManager**       | Dev D | ✅ Complete | Dice rolling, string parsing ("3d6")                    |
| **DataRegistry**      | Dev D | ✅ Complete | JSON loading (basic)                                    |
| **DebugConsole**      | Dev E | ✅ Complete | Console commands (spawn, damage, heal, castspell, etc.) |
| **GridDebugRenderer** | Dev E | ✅ Complete | F1 grid overlay                                         |
| **BattleState**       | Dev E | ✅ Complete | All systems integrated                                  |

### 📦 Deliverable Artifacts

**Code**:

- [ ] All Week 1-5 systems implemented
- [ ] Dragon character playable with 3 spells
- [ ] Fighter enemy with manual control
- [ ] Debug tools functional

**Testing**:

- [ ] Playtest 1 executed successfully
- [ ] Bug triage completed
- [ ] Critical bugs assigned

### 🎯 Success Criteria (Week 5)

**Playtest 1 is considered SUCCESSFUL if**:

- ✅ All 5 developers completed their Week 1-5 tasks
- ✅ Playtest 1 executed without critical crashes
- ✅ Dragon playable for 5+ minutes
- ✅ All 3 spells working
- ✅ Bug triage completed with all bugs assigned

**Next Steps**: Week 6+ (see [docs/implementation-plan.md](../implementation-plan.md) for Weeks 6-26)

---

## Document Changelog

- **2025-10-06**: Complete rewrite for Week 5 Playtest 1 timeline
  - Removed all content beyond Week 5
  - Reorganized as week-by-week, developer-by-developer plan
  - Added detailed implementation tasks, testing, and deliverables for each developer
  - Streamlined from 2300+ lines to focused Weeks 1-5 implementation guide
  - Aligned with implementation-plan.md Week 5 Playtest 1 milestone

---

**For complete project timeline (Weeks 6-26)**, see:

- [docs/implementation-plan.md](../implementation-plan.md)
