# Playtest 1 Implementation Plan - Week 2

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

**Test Suite 1: Dragon Initialization**

- [ ] **Test_Dragon_Spawn_DefaultStats()**
  
  ```cpp
  // Setup
  Dragon dragon;
  
  // Assertions: Verify all stats initialized correctly
  ASSERT_EQ(dragon.GetMaxHP(), 140);
  ASSERT_EQ(dragon.GetCurrentHP(), 140);
  ASSERT_EQ(dragon.GetSpeed(), 5);
  ASSERT_EQ(dragon.GetActionPoints(), 5);
  ASSERT_EQ(dragon.GetBaseAttackPower(), 5);
  ASSERT_EQ(dragon.GetAttackDice(), "3d6");
  ASSERT_EQ(dragon.GetBaseDefensePower(), 3);
  ASSERT_EQ(dragon.GetDefenseDice(), "2d8");
  ASSERT_EQ(dragon.GetAttackRange(), 1);
  ASSERT_EQ(dragon.GetGridPosition(), Math::vec2{4, 4});  // Center spawn
  ```

- [ ] **Test_Dragon_SpellSlots_Initialized()**
  
  ```cpp
  // Setup
  Dragon dragon;
  
  // Verify spell slots
  ASSERT_EQ(dragon.GetSpellSlots(1), 4);  // Level 1: 4 slots
  ASSERT_EQ(dragon.GetSpellSlots(2), 3);  // Level 2: 3 slots
  ASSERT_EQ(dragon.GetSpellSlots(3), 2);  // Level 3: 2 slots
  ASSERT_EQ(dragon.GetSpellSlots(4), 0);  // Level 4: 0 slots (doesn't exist)
  ```

- [ ] **Test_Dragon_Type()**
  
  ```cpp
  // Setup
  Dragon dragon;
  
  // Assertions
  ASSERT_EQ(dragon.Type(), GameObjectTypes::Dragon);
  ASSERT_EQ(dragon.TypeName(), "Dragon");
  ```

**Test Suite 2: Dragon Movement**

- [ ] **Test_Dragon_MoveToTile_ValidMove()**
  
  ```cpp
  // Setup
  BattleState battleState;
  GridSystem* grid = battleState.GetGSComponent<GridSystem>();
  Dragon dragon;
  grid->PlaceCharacter(&dragon, {4, 4});
  
  ASSERT_EQ(dragon.GetGridPosition(), Math::vec2{4, 4});
  ASSERT_EQ(dragon.GetActionPoints(), 5);
  
  // Subscribe to MovementEvent
  bool eventPublished = false;
  Math::vec2 eventFromTile, eventToTile;
  EventBus::Instance().Subscribe<MovementEvent>([&](const MovementEvent& e) {
      eventPublished = true;
      eventFromTile = e.fromTile;
      eventToTile = e.toTile;
  });
  
  // Action: Move to adjacent tile
  dragon.MoveToTile({5, 4});
  
  // Assertions
  ASSERT_EQ(dragon.GetGridPosition(), Math::vec2{5, 4});
  ASSERT_EQ(dragon.GetActionPoints(), 4);  // Consumed 1 AP
  ASSERT_TRUE(eventPublished);
  ASSERT_EQ(eventFromTile, Math::vec2{4, 4});
  ASSERT_EQ(eventToTile, Math::vec2{5, 4});
  ASSERT_TRUE(grid->IsOccupied({5, 4}));
  ASSERT_FALSE(grid->IsOccupied({4, 4}));  // Old position now empty
  ```

- [ ] **Test_Dragon_MoveToTile_InvalidMove_OutOfBounds()**
  
  ```cpp
  // Setup
  BattleState battleState;
  GridSystem* grid = battleState.GetGSComponent<GridSystem>();
  Dragon dragon;
  grid->PlaceCharacter(&dragon, {4, 4});
  
  Math::vec2 originalPosition = dragon.GetGridPosition();
  int originalAP = dragon.GetActionPoints();
  
  // Action: Try to move out of bounds
  dragon.MoveToTile({10, 10});  // Invalid: grid is only 8x8
  
  // Assertions: Dragon should NOT move
  ASSERT_EQ(dragon.GetGridPosition(), originalPosition);
  ASSERT_EQ(dragon.GetActionPoints(), originalAP);  // AP not consumed
  // Verify error logged
  ```

- [ ] **Test_Dragon_MoveToTile_InvalidMove_ThroughWall()**
  
  ```cpp
  // Setup
  BattleState battleState;
  GridSystem* grid = battleState.GetGSComponent<GridSystem>();
  grid->SetTileType({5, 4}, TileType::Wall);  // Create wall
  Dragon dragon;
  grid->PlaceCharacter(&dragon, {4, 4});
  
  // Action: Try to move to wall tile
  dragon.MoveToTile({5, 4});
  
  // Assertions: Dragon should NOT move
  ASSERT_EQ(dragon.GetGridPosition(), Math::vec2{4, 4});  // Still at original position
  ```

- [ ] **Test_Dragon_MoveToTile_NoActionPoints()**
  
  ```cpp
  // Setup
  Dragon dragon;
  dragon.ConsumeActionPoints(5);  // Consume all AP
  ASSERT_EQ(dragon.GetActionPoints(), 0);
  
  Math::vec2 originalPosition = dragon.GetGridPosition();
  
  // Action: Try to move with 0 AP
  dragon.MoveToTile({5, 4});
  
  // Assertions: Dragon should NOT move
  ASSERT_EQ(dragon.GetGridPosition(), originalPosition);
  ASSERT_EQ(dragon.GetActionPoints(), 0);
  ```

**Test Suite 3: Dragon Combat Integration**

- [ ] **Test_Dragon_TakeDamage_IntegrationWithHealth()**
  
  ```cpp
  // Setup
  Dragon dragon;
  ASSERT_EQ(dragon.GetCurrentHP(), 140);
  
  // Action: Take damage
  dragon.TakeDamage(50);
  
  // Assertions
  ASSERT_EQ(dragon.GetCurrentHP(), 90);
  ASSERT_TRUE(dragon.IsAlive());
  
  // Take fatal damage
  dragon.TakeDamage(100);
  ASSERT_EQ(dragon.GetCurrentHP(), 0);
  ASSERT_FALSE(dragon.IsAlive());
  ```

- [ ] **Test_Dragon_RefreshActionPoints_NewTurn()**
  
  ```cpp
  // Setup
  Dragon dragon;
  dragon.ConsumeActionPoints(3);  // Use 3 AP
  ASSERT_EQ(dragon.GetActionPoints(), 2);
  
  // Action: Refresh AP (new turn)
  dragon.RefreshActionPoints();
  
  // Assertions
  ASSERT_EQ(dragon.GetActionPoints(), 5);  // Back to max
  ```

**Test Suite 4: Dragon Update & Draw**

- [ ] **Test_Dragon_Update()**
  
  ```cpp
  // Setup
  Dragon dragon;
  
  // Action: Update (no crash expected)
  dragon.Update(0.016);  // 60 FPS frame
  
  // If we get here, test passed
  ASSERT_TRUE(true);
  ```

- [ ] **Test_Dragon_Draw()**
  
  ```cpp
  // Setup
  Dragon dragon;
  Math::TransformationMatrix identity;
  
  // Action: Draw (no crash expected)
  dragon.Draw(identity);
  
  // If we get here, test passed
  ASSERT_TRUE(true);
  ```

**Usage Examples**:

**Example 1: Spawning Dragon in BattleState**

```cpp
// In BattleState::Load()
Dragon* dragon = new Dragon();
GridSystem* grid = GetGSComponent<GridSystem>();

// Place dragon at center
grid->PlaceCharacter(dragon, {4, 4});

// Add to GameObjectManager
AddGameObject(dragon);

std::cout << "Dragon spawned at (4, 4)" << std::endl;
std::cout << "HP: " << dragon->GetCurrentHP() << "/" << dragon->GetMaxHP() << std::endl;
std::cout << "AP: " << dragon->GetActionPoints() << std::endl;
// Output:
// Dragon spawned at (4, 4)
// HP: 140/140
// AP: 5
```

**Example 2: Moving Dragon with Input**

```cpp
// In BattleState::Update()
Dragon* dragon = GetActiveCharacter();  // Get dragon from TurnManager

if (Engine::GetInput().IsKeyPressed(InputKey::W)) {
    // Move north
    Math::vec2 targetTile = dragon->GetGridPosition() + Math::vec2{0, -1};
    dragon->MoveToTile(targetTile);
}

if (Engine::GetInput().IsKeyPressed(InputKey::A)) {
    // Move west
    Math::vec2 targetTile = dragon->GetGridPosition() + Math::vec2{-1, 0};
    dragon->MoveToTile(targetTile);
}

if (Engine::GetInput().IsKeyPressed(InputKey::S)) {
    // Move south
    Math::vec2 targetTile = dragon->GetGridPosition() + Math::vec2{0, 1};
    dragon->MoveToTile(targetTile);
}

if (Engine::GetInput().IsKeyPressed(InputKey::D)) {
    // Move east
    Math::vec2 targetTile = dragon->GetGridPosition() + Math::vec2{1, 0};
    dragon->MoveToTile(targetTile);
}
```

**Example 3: Dragon Movement with Pathfinding**

```cpp
Dragon* dragon = /* ... */;
GridSystem* grid = /* ... */;

// Player clicks on target tile
Math::vec2 targetTile = {7, 7};

// Check if tile is reachable
std::vector<Math::vec2> reachableTiles = grid->GetReachableTiles(dragon);
bool canReach = std::find(reachableTiles.begin(), reachableTiles.end(), targetTile) != reachableTiles.end();

if (canReach) {
    // Move to tile
    dragon->MoveToTile(targetTile);

    std::cout << "Dragon moved to (" << targetTile.x << ", " << targetTile.y << ")" << std::endl;
    std::cout << "AP remaining: " << dragon->GetActionPoints() << std::endl;
} else {
    std::cout << "Cannot reach tile (" << targetTile.x << ", " << targetTile.y << ")" << std::endl;
}
```

**Example 4: Subscribing to Dragon Movement Events**

```cpp
// Subscribe to movement events for UI updates
EventBus::Instance().Subscribe<MovementEvent>([](const MovementEvent& e) {
    if (e.character->Type() == GameObjectTypes::Dragon) {
        std::cout << "Dragon moved from (" << e.fromTile.x << ", " << e.fromTile.y << ") "
                  << "to (" << e.toTile.x << ", " << e.toTile.y << ")" << std::endl;

        // Update camera to follow dragon
        Math::vec2 worldPos = e.toTile * 64;  // Tile size = 64 pixels
        Engine::GetCamera().SetPosition(worldPos);
    }
});
```

**Example 5: Dragon Turn Management**

```cpp
// In TurnManager::StartNextTurn()
if (activeCharacter->Type() == GameObjectTypes::Dragon) {
    Dragon* dragon = static_cast<Dragon*>(activeCharacter);

    // Refresh action points
    dragon->RefreshActionPoints();

    // Display turn info
    std::cout << "\n=== Dragon's Turn ===" << std::endl;
    std::cout << "HP: " << dragon->GetCurrentHP() << "/" << dragon->GetMaxHP() << std::endl;
    std::cout << "AP: " << dragon->GetActionPoints() << std::endl;
    std::cout << "Position: (" << dragon->GetGridPosition().x << ", "
              << dragon->GetGridPosition().y << ")" << std::endl;

    // Spell slots
    std::cout << "Spell Slots:" << std::endl;
    for (int level = 1; level <= 3; ++level) {
        int slots = dragon->GetSpellSlots(level);
        std::cout << "  Level " << level << ": " << slots << " slots" << std::endl;
    }
}
```

**Example 6: Dragon Combat Scenario**

```cpp
// Full turn example
Dragon* dragon = /* ... */;
Fighter* enemy = /* ... */;
GridSystem* grid = /* ... */;

// Dragon's turn starts
dragon->RefreshActionPoints();

// Move towards enemy (2 AP)
dragon->MoveToTile({5, 4});
dragon->MoveToTile({5, 5});

std::cout << "AP after movement: " << dragon->GetActionPoints() << std::endl;
// Output: AP after movement: 3

// Attack enemy (1 AP)
// (This will be implemented in Week 3)
// dragon->Attack(enemy);

// End turn
std::cout << "Dragon ends turn" << std::endl;
```

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

**Test Suite 1: Basic Pathfinding**

- [ ] **Test_FindPath_StraightLine_Horizontal()**
  
  ```cpp
  // Setup
  GridSystem grid;
  
  // Action
  std::vector<Math::vec2> path = grid.FindPath({0, 0}, {4, 0});
  
  // Assertions
  ASSERT_EQ(path.size(), 5);  // 5 tiles including start and end
  ASSERT_EQ(path[0], Math::vec2{0, 0});
  ASSERT_EQ(path[1], Math::vec2{1, 0});
  ASSERT_EQ(path[2], Math::vec2{2, 0});
  ASSERT_EQ(path[3], Math::vec2{3, 0});
  ASSERT_EQ(path[4], Math::vec2{4, 0});
  ```

- [ ] **Test_FindPath_StraightLine_Vertical()**
  
  ```cpp
  // Setup
  GridSystem grid;
  
  // Action
  std::vector<Math::vec2> path = grid.FindPath({3, 0}, {3, 5});
  
  // Assertions
  ASSERT_EQ(path.size(), 6);
  ASSERT_EQ(path[0], Math::vec2{3, 0});
  ASSERT_EQ(path[5], Math::vec2{3, 5});
  ```

- [ ] **Test_FindPath_Diagonal()**
  
  ```cpp
  // Setup
  GridSystem grid;
  
  // Action: Path from corner to corner
  std::vector<Math::vec2> path = grid.FindPath({0, 0}, {3, 3});
  
  // Assertions
  ASSERT_FALSE(path.empty());
  ASSERT_EQ(path[0], Math::vec2{0, 0});
  ASSERT_EQ(path.back(), Math::vec2{3, 3});
  // Path length should be 7 (Manhattan distance = 6, +1 for start tile)
  ASSERT_EQ(path.size(), 7);
  ```

- [ ] **Test_FindPath_SameStartAndEnd()**
  
  ```cpp
  // Setup
  GridSystem grid;
  
  // Action: Path to same tile
  std::vector<Math::vec2> path = grid.FindPath({4, 4}, {4, 4});
  
  // Assertions: Should return single-tile path
  ASSERT_EQ(path.size(), 1);
  ASSERT_EQ(path[0], Math::vec2{4, 4});
  ```

**Test Suite 2: Pathfinding Around Obstacles**

- [ ] **Test_FindPath_AroundWall_Horizontal()**
  
  ```cpp
  // Setup: Create vertical wall
  GridSystem grid;
  grid.SetTileType({2, 0}, TileType::Wall);
  grid.SetTileType({2, 1}, TileType::Wall);
  grid.SetTileType({2, 2}, TileType::Wall);
  
  // Action: Find path from left to right (must go around wall)
  std::vector<Math::vec2> path = grid.FindPath({0, 1}, {4, 1});
  
  // Assertions
  ASSERT_FALSE(path.empty());
  ASSERT_EQ(path[0], Math::vec2{0, 1});
  ASSERT_EQ(path.back(), Math::vec2{4, 1});
  
  // Verify path does NOT go through wall tiles
  for (const auto& tile : path) {
      ASSERT_NE(tile, Math::vec2{2, 0});
      ASSERT_NE(tile, Math::vec2{2, 1});
      ASSERT_NE(tile, Math::vec2{2, 2});
  }
  ```

- [ ] **Test_FindPath_AroundWall_LShape()**
  
  ```cpp
  // Setup: Create L-shaped obstacle
  GridSystem grid;
  for (int x = 2; x <= 5; ++x) grid.SetTileType({x, 3}, TileType::Wall);
  for (int y = 3; y <= 5; ++y) grid.SetTileType({5, y}, TileType::Wall);
  
  // Action: Find path
  std::vector<Math::vec2> path = grid.FindPath({1, 4}, {6, 4});
  
  // Assertions
  ASSERT_FALSE(path.empty());
  ASSERT_EQ(path[0], Math::vec2{1, 4});
  ASSERT_EQ(path.back(), Math::vec2{6, 4});
  
  // Verify no wall tiles in path
  for (const auto& tile : path) {
      ASSERT_NE(grid.GetTileType(tile), TileType::Wall);
  }
  ```

- [ ] **Test_FindPath_NoPathAvailable()**
  
  ```cpp
  // Setup: Create impassable barrier (full vertical line)
  GridSystem grid;
  for (int y = 0; y < 8; ++y) {
      grid.SetTileType({4, y}, TileType::Wall);
  }
  
  // Action: Try to find path from left to right (no path exists)
  std::vector<Math::vec2> path = grid.FindPath({0, 4}, {7, 4});
  
  // Assertions: Should return empty path
  ASSERT_TRUE(path.empty());
  ```

**Test Suite 3: Movement Cost & Difficult Terrain**

- [ ] **Test_GetMovementCost_NormalTile()**
  
  ```cpp
  // Setup
  GridSystem grid;
  grid.SetTileType({3, 3}, TileType::Empty);
  
  // Action
  int cost = grid.GetMovementCost({3, 3}, {3, 4});
  
  // Assertions: Normal tile costs 1
  ASSERT_EQ(cost, 1);
  ```

- [ ] **Test_GetMovementCost_DifficultTerrain()**
  
  ```cpp
  // Setup
  GridSystem grid;
  grid.SetTileType({3, 4}, TileType::Difficult);
  
  // Action
  int cost = grid.GetMovementCost({3, 3}, {3, 4});
  
  // Assertions: Difficult terrain costs 2
  ASSERT_EQ(cost, 2);
  ```

- [ ] **Test_GetMovementCost_Wall()**
  
  ```cpp
  // Setup
  GridSystem grid;
  grid.SetTileType({3, 4}, TileType::Wall);
  
  // Action
  int cost = grid.GetMovementCost({3, 3}, {3, 4});
  
  // Assertions: Wall is impassable (cost = infinity or large number)
  ASSERT_GE(cost, 1000);  // Should be very high or INT_MAX
  ```

- [ ] **Test_FindPath_PrefersShorterPath_AvoidsDifficultTerrain()**
  
  ```cpp
  // Setup: Create difficult terrain in straight path
  GridSystem grid;
  grid.SetTileType({2, 0}, TileType::Difficult);
  grid.SetTileType({3, 0}, TileType::Difficult);
  
  // Action: Find path (should prefer going around difficult tiles)
  std::vector<Math::vec2> path = grid.FindPath({0, 0}, {5, 0});
  
  // If path goes through difficult terrain, it should be longer
  // If path avoids it, verify it goes around
  ASSERT_FALSE(path.empty());
  ```

**Test Suite 4: Reachable Tiles Calculation**

- [ ] **Test_GetReachableTiles_Movement3()**
  
  ```cpp
  // Setup
  GridSystem grid;
  TestCharacter character;
  character.speed = 3;  // Movement range = 3
  character.gridPosition = {4, 4};
  
  // Action
  std::vector<Math::vec2> reachable = grid.GetReachableTiles(&character);
  
  // Assertions: Should include all tiles within Manhattan distance 3
  // At (4,4) with range 3:
  // Min: (1,4), (4,1)
  // Max: (7,4), (4,7)
  ASSERT_FALSE(reachable.empty());
  
  // Verify center tile included
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{4, 4}) != reachable.end());
  
  // Verify tiles at distance 3 included
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{1, 4}) != reachable.end());
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{7, 4}) != reachable.end());
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{4, 1}) != reachable.end());
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{4, 7}) != reachable.end());
  
  // Verify tiles beyond distance 3 NOT included
  ASSERT_FALSE(std::find(reachable.begin(), reachable.end(), Math::vec2{0, 4}) != reachable.end());
  ```

- [ ] **Test_GetReachableTiles_WithWalls()**
  
  ```cpp
  // Setup: Character at (4,4), wall blocks path
  GridSystem grid;
  grid.SetTileType({5, 4}, TileType::Wall);
  grid.SetTileType({5, 3}, TileType::Wall);
  grid.SetTileType({5, 5}, TileType::Wall);
  
  TestCharacter character;
  character.speed = 3;
  character.gridPosition = {4, 4};
  
  // Action
  std::vector<Math::vec2> reachable = grid.GetReachableTiles(&character);
  
  // Assertions: Tiles beyond wall should NOT be reachable (if wall blocks)
  ASSERT_FALSE(reachable.empty());
  
  // Tiles on the left should be reachable
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{1, 4}) != reachable.end());
  
  // Tiles directly blocked by wall may not be reachable
  // (Depends on implementation - if wall cuts off entire right side)
  ```

- [ ] **Test_GetReachableTiles_Movement1()**
  
  ```cpp
  // Setup: Character with movement 1 (only adjacent tiles)
  GridSystem grid;
  TestCharacter character;
  character.speed = 1;
  character.gridPosition = {4, 4};
  
  // Action
  std::vector<Math::vec2> reachable = grid.GetReachableTiles(&character);
  
  // Assertions: Should only include current tile + 4 adjacent tiles
  // (4,4), (3,4), (5,4), (4,3), (4,5)
  ASSERT_LE(reachable.size(), 5);
  
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{4, 4}) != reachable.end());
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{3, 4}) != reachable.end());
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{5, 4}) != reachable.end());
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{4, 3}) != reachable.end());
  ASSERT_TRUE(std::find(reachable.begin(), reachable.end(), Math::vec2{4, 5}) != reachable.end());
  ```

**Test Suite 5: Heuristic & Neighbors**

- [ ] **Test_Heuristic_ManhattanDistance()**
  
  ```cpp
  // Setup
  GridSystem grid;
  
  // Test horizontal distance
  int dist1 = grid.Heuristic({0, 0}, {5, 0});
  ASSERT_EQ(dist1, 5);
  
  // Test vertical distance
  int dist2 = grid.Heuristic({0, 0}, {0, 5});
  ASSERT_EQ(dist2, 5);
  
  // Test diagonal distance (Manhattan = x_diff + y_diff)
  int dist3 = grid.Heuristic({0, 0}, {3, 4});
  ASSERT_EQ(dist3, 7);  // 3 + 4 = 7
  ```

- [ ] **Test_GetNeighbors_CenterTile()**
  
  ```cpp
  // Setup
  GridSystem grid;
  
  // Action: Get neighbors of center tile
  std::vector<Math::vec2> neighbors = grid.GetNeighbors({4, 4});
  
  // Assertions: Should have 4 neighbors (up, down, left, right)
  ASSERT_EQ(neighbors.size(), 4);
  ASSERT_TRUE(std::find(neighbors.begin(), neighbors.end(), Math::vec2{3, 4}) != neighbors.end());
  ASSERT_TRUE(std::find(neighbors.begin(), neighbors.end(), Math::vec2{5, 4}) != neighbors.end());
  ASSERT_TRUE(std::find(neighbors.begin(), neighbors.end(), Math::vec2{4, 3}) != neighbors.end());
  ASSERT_TRUE(std::find(neighbors.begin(), neighbors.end(), Math::vec2{4, 5}) != neighbors.end());
  ```

- [ ] **Test_GetNeighbors_CornerTile()**
  
  ```cpp
  // Setup
  GridSystem grid;
  
  // Action: Get neighbors of corner
  std::vector<Math::vec2> neighbors = grid.GetNeighbors({0, 0});
  
  // Assertions: Corner has only 2 neighbors
  ASSERT_EQ(neighbors.size(), 2);
  ASSERT_TRUE(std::find(neighbors.begin(), neighbors.end(), Math::vec2{1, 0}) != neighbors.end());
  ASSERT_TRUE(std::find(neighbors.begin(), neighbors.end(), Math::vec2{0, 1}) != neighbors.end());
  ```

**Usage Examples**:

**Example 1: Basic Pathfinding**

```cpp
GridSystem grid;

// Find path from top-left to bottom-right
Math::vec2 start = {0, 0};
Math::vec2 goal = {7, 7};

std::vector<Math::vec2> path = grid.FindPath(start, goal);

if (!path.empty()) {
    std::cout << "Path found! Length: " << path.size() << std::endl;
    for (const auto& tile : path) {
        std::cout << "(" << tile.x << ", " << tile.y << ") -> ";
    }
    std::cout << "Goal!" << std::endl;
} else {
    std::cout << "No path available!" << std::endl;
}
```

**Example 2: Pathfinding Around Obstacles**

```cpp
GridSystem grid;

// Create wall obstacle
for (int y = 2; y <= 5; ++y) {
    grid.SetTileType({4, y}, TileType::Wall);
}

// Find path (must go around wall)
std::vector<Math::vec2> path = grid.FindPath({3, 3}, {5, 3});

std::cout << "Path navigates around wall:" << std::endl;
for (const auto& tile : path) {
    std::cout << "(" << tile.x << ", " << tile.y << ")" << std::endl;
}
```

**Example 3: Calculating Reachable Tiles for Movement**

```cpp
GridSystem grid;
Dragon dragon;  // speed = 5

// Get all tiles dragon can reach this turn
std::vector<Math::vec2> reachableTiles = grid.GetReachableTiles(&dragon);

std::cout << "Dragon can reach " << reachableTiles.size() << " tiles:" << std::endl;
for (const auto& tile : reachableTiles) {
    std::cout << "  (" << tile.x << ", " << tile.y << ")" << std::endl;
}

// Highlight reachable tiles in UI
for (const auto& tile : reachableTiles) {
    // DrawHighlightedTile(tile);  // Visual feedback
}
```

**Example 4: Movement with Player Input**

```cpp
GridSystem grid;
Dragon* dragon = /* ... */;

// Player clicks on target tile
Math::vec2 clickedTile = {6, 3};

// Check if tile is reachable
std::vector<Math::vec2> reachableTiles = grid.GetReachableTiles(dragon);
bool canReach = std::find(reachableTiles.begin(), reachableTiles.end(), clickedTile) != reachableTiles.end();

if (canReach) {
    // Find and display path
    std::vector<Math::vec2> path = grid.FindPath(dragon->GetGridPosition(), clickedTile);

    std::cout << "Moving dragon along path:" << std::endl;
    for (const auto& tile : path) {
        std::cout << "  -> (" << tile.x << ", " << tile.y << ")" << std::endl;
    }

    // Execute movement
    dragon->MoveToTile(clickedTile);
} else {
    std::cout << "Cannot reach tile (" << clickedTile.x << ", " << clickedTile.y << ")!" << std::endl;
}
```

**Example 5: Difficult Terrain Pathfinding**

```cpp
GridSystem grid;

// Create difficult terrain (mud, forest, etc.)
grid.SetTileType({3, 3}, TileType::Difficult);
grid.SetTileType({4, 3}, TileType::Difficult);
grid.SetTileType({5, 3}, TileType::Difficult);

// Find path (A* will prefer normal tiles over difficult terrain)
std::vector<Math::vec2> path = grid.FindPath({2, 3}, {6, 3});

// Calculate total movement cost
int totalCost = 0;
for (size_t i = 0; i < path.size() - 1; ++i) {
    int cost = grid.GetMovementCost(path[i], path[i + 1]);
    totalCost += cost;
}

std::cout << "Path length: " << path.size() << " tiles" << std::endl;
std::cout << "Total movement cost: " << totalCost << std::endl;
```

**Example 6: Multi-Turn Movement Planning**

```cpp
GridSystem grid;
Dragon* dragon = /* ... */;
Math::vec2 finalGoal = {7, 7};

// Calculate full path to goal
std::vector<Math::vec2> fullPath = grid.FindPath(dragon->GetGridPosition(), finalGoal);

if (!fullPath.empty()) {
    int movementPerTurn = dragon->GetMovementRange();
    int turnsNeeded = (fullPath.size() - 1 + movementPerTurn - 1) / movementPerTurn;  // Ceiling division

    std::cout << "Reaching (" << finalGoal.x << ", " << finalGoal.y << ") requires:" << std::endl;
    std::cout << "  Path length: " << fullPath.size() << " tiles" << std::endl;
    std::cout << "  Turns needed: " << turnsNeeded << std::endl;

    // Show path for this turn
    int tilesThisTurn = std::min(movementPerTurn, static_cast<int>(fullPath.size()) - 1);
    std::cout << "  This turn, can move " << tilesThisTurn << " tiles" << std::endl;
}
```

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

**Test Suite 1: Turn Order Initialization**

- [ ] **Test_InitializeTurnOrder_SingleCharacter()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  
  // Action
  turnManager.InitializeTurnOrder({&dragon});
  
  // Assertions
  ASSERT_EQ(turnManager.GetActiveCharacter(), &dragon);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  ```

- [ ] **Test_InitializeTurnOrder_TwoCharacters()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  Fighter fighter;
  
  bool turnStartedEventFired = false;
  EventBus::Instance().Subscribe<TurnStartedEvent>([&](const TurnStartedEvent& e) {
      turnStartedEventFired = true;
      ASSERT_EQ(e.activeCharacter, &dragon);
      ASSERT_EQ(e.turnNumber, 1);
  });
  
  // Action
  turnManager.InitializeTurnOrder({&dragon, &fighter});
  
  // Assertions
  ASSERT_EQ(turnManager.GetActiveCharacter(), &dragon);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  ASSERT_TRUE(turnStartedEventFired);
  ```

- [ ] **Test_InitializeTurnOrder_FourCharacters()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  Fighter fighter;
  Wizard wizard;
  Cleric cleric;
  
  // Action
  turnManager.InitializeTurnOrder({&dragon, &fighter, &wizard, &cleric});
  
  // Assertions
  ASSERT_EQ(turnManager.GetActiveCharacter(), &dragon);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  ```

**Test Suite 2: Turn Progression**

- [ ] **Test_StartNextTurn_TwoCharacters()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  Fighter fighter;
  turnManager.InitializeTurnOrder({&dragon, &fighter});
  
  ASSERT_EQ(turnManager.GetActiveCharacter(), &dragon);
  
  // Action: Progress turn
  turnManager.StartNextTurn();
  
  // Assertions
  ASSERT_EQ(turnManager.GetActiveCharacter(), &fighter);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);  // Still turn 1
  ```

- [ ] **Test_StartNextTurn_Wraps_IncrementsTurnNumber()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  Fighter fighter;
  turnManager.InitializeTurnOrder({&dragon, &fighter});
  
  // Progress through one full round
  turnManager.StartNextTurn();  // Dragon -> Fighter
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  
  // Action: Wrap to next turn
  turnManager.StartNextTurn();  // Fighter -> Dragon (new turn)
  
  // Assertions
  ASSERT_EQ(turnManager.GetActiveCharacter(), &dragon);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 2);  // Turn 2!
  ```

- [ ] **Test_EndCurrentTurn_PublishesEvent()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  turnManager.InitializeTurnOrder({&dragon});
  
  bool turnEndedEventFired = false;
  EventBus::Instance().Subscribe<TurnEndedEvent>([&](const TurnEndedEvent& e) {
      turnEndedEventFired = true;
      ASSERT_EQ(e.finishedCharacter, &dragon);
  });
  
  // Action
  turnManager.EndCurrentTurn();
  
  // Assertions
  ASSERT_TRUE(turnEndedEventFired);
  ```

- [ ] **Test_TurnProgression_FourCharacters_FullRound()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  Fighter fighter;
  Wizard wizard;
  Cleric cleric;
  turnManager.InitializeTurnOrder({&dragon, &fighter, &wizard, &cleric});
  
  // Turn 1, Character 1
  ASSERT_EQ(turnManager.GetActiveCharacter(), &dragon);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  
  // Turn 1, Character 2
  turnManager.StartNextTurn();
  ASSERT_EQ(turnManager.GetActiveCharacter(), &fighter);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  
  // Turn 1, Character 3
  turnManager.StartNextTurn();
  ASSERT_EQ(turnManager.GetActiveCharacter(), &wizard);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  
  // Turn 1, Character 4
  turnManager.StartNextTurn();
  ASSERT_EQ(turnManager.GetActiveCharacter(), &cleric);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 1);
  
  // Turn 2, Character 1 (wraps)
  turnManager.StartNextTurn();
  ASSERT_EQ(turnManager.GetActiveCharacter(), &dragon);
  ASSERT_EQ(turnManager.GetCurrentTurnNumber(), 2);
  ```

**Test Suite 3: Action Point Refresh**

- [ ] **Test_StartNextTurn_RefreshesActionPoints()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  dragon.ConsumeActionPoints(3);  // Use some AP
  ASSERT_EQ(dragon.GetActionPoints(), 2);
  
  turnManager.InitializeTurnOrder({&dragon});
  
  // Dragon's turn already started, AP should be refreshed
  ASSERT_EQ(dragon.GetActionPoints(), 5);  // Back to max
  ```

- [ ] **Test_StartNextTurn_RefreshesOnlyActiveCharacter()**
  
  ```cpp
  // Setup
  TurnManager turnManager;
  Dragon dragon;
  Fighter fighter;
  
  dragon.ConsumeActionPoints(2);
  fighter.ConsumeActionPoints(3);
  
  turnManager.InitializeTurnOrder({&dragon, &fighter});
  
  // Dragon's turn started, AP refreshed
  ASSERT_EQ(dragon.GetActionPoints(), 5);
  ASSERT_EQ(fighter.GetActionPoints(), 2);  // Fighter AP NOT refreshed yet
  
  // Progress to fighter's turn
  turnManager.StartNextTurn();
  ASSERT_EQ(dragon.GetActionPoints(), 5);  // Dragon AP unchanged
  ASSERT_EQ(fighter.GetActionPoints(), 5);  // Fighter AP NOW refreshed
  ```

**Test Suite 4: Event Publishing**

- [ ] **Test_TurnStartedEvent_PublishedOnInit()**
  
  ```cpp
  // Setup
  bool eventFired = false;
  Character* eventCharacter = nullptr;
  int eventTurnNumber = 0;
  
  EventBus::Instance().Subscribe<TurnStartedEvent>([&](const TurnStartedEvent& e) {
      eventFired = true;
      eventCharacter = e.activeCharacter;
      eventTurnNumber = e.turnNumber;
  });
  
  TurnManager turnManager;
  Dragon dragon;
  
  // Action
  turnManager.InitializeTurnOrder({&dragon});
  
  // Assertions
  ASSERT_TRUE(eventFired);
  ASSERT_EQ(eventCharacter, &dragon);
  ASSERT_EQ(eventTurnNumber, 1);
  ```

- [ ] **Test_TurnStartedEvent_PublishedOnEveryTurn()**
  
  ```cpp
  // Setup
  int eventCount = 0;
  EventBus::Instance().Subscribe<TurnStartedEvent>([&](const TurnStartedEvent& e) {
      eventCount++;
  });
  
  TurnManager turnManager;
  Dragon dragon;
  Fighter fighter;
  
  // Action
  turnManager.InitializeTurnOrder({&dragon, &fighter});  // Event 1
  turnManager.StartNextTurn();  // Event 2
  turnManager.StartNextTurn();  // Event 3
  
  // Assertions
  ASSERT_EQ(eventCount, 3);
  ```

**Usage Examples**:

**Example 1: Basic Turn Management**

```cpp
TurnManager turnManager;
Dragon dragon;
Fighter fighter;

// Initialize turn order
turnManager.InitializeTurnOrder({&dragon, &fighter});

std::cout << "Turn " << turnManager.GetCurrentTurnNumber() << ": "
          << turnManager.GetActiveCharacter()->TypeName() << "'s turn" << std::endl;
// Output: Turn 1: Dragon's turn

// End dragon's turn
turnManager.EndCurrentTurn();

std::cout << "Turn " << turnManager.GetCurrentTurnNumber() << ": "
          << turnManager.GetActiveCharacter()->TypeName() << "'s turn" << std::endl;
// Output: Turn 1: Fighter's turn

// End fighter's turn (wraps to turn 2)
turnManager.EndCurrentTurn();

std::cout << "Turn " << turnManager.GetCurrentTurnNumber() << ": "
          << turnManager.GetActiveCharacter()->TypeName() << "'s turn" << std::endl;
// Output: Turn 2: Dragon's turn
```

**Example 2: Turn Management with UI Updates**

```cpp
TurnManager turnManager;

// Subscribe to turn events for UI
EventBus::Instance().Subscribe<TurnStartedEvent>([](const TurnStartedEvent& e) {
    std::cout << "\n========== TURN " << e.turnNumber << " ==========" << std::endl;
    std::cout << "Active: " << e.activeCharacter->TypeName() << std::endl;
    std::cout << "HP: " << e.activeCharacter->GetCurrentHP() << "/" << e.activeCharacter->GetMaxHP() << std::endl;
    std::cout << "AP: " << e.activeCharacter->GetActionPoints() << std::endl;
    std::cout << "================================" << std::endl;
});

EventBus::Instance().Subscribe<TurnEndedEvent>([](const TurnEndedEvent& e) {
    std::cout << e.finishedCharacter->TypeName() << " ended their turn." << std::endl;
});

// Run game
Dragon dragon;
Fighter fighter;
turnManager.InitializeTurnOrder({&dragon, &fighter});

// Player actions...
turnManager.EndCurrentTurn();
// Player actions...
turnManager.EndCurrentTurn();
```

**Example 3: Integration with BattleState**

```cpp
// In BattleState class
class BattleState : public CS230::GameState {
private:
    TurnManager* turnManager;
    std::vector<Character*> allCharacters;

public:
    void Load() override {
        // Create characters
        Dragon* dragon = new Dragon();
        Fighter* fighter = new Fighter();

        allCharacters = {dragon, fighter};

        // Initialize turn manager
        turnManager = new TurnManager();
        turnManager->InitializeTurnOrder(allCharacters);
    }

    void Update(double dt) override {
        // Get active character
        Character* active = turnManager->GetActiveCharacter();

        // Player input for active character
        if (Engine::GetInput().IsKeyPressed(InputKey::Space)) {
            // End turn
            turnManager->EndCurrentTurn();
        }
    }
};
```

**Example 4: Four-Character Battle**

```cpp
TurnManager turnManager;

Dragon dragon;
Fighter fighter;
Wizard wizard;
Cleric cleric;

std::vector<Character*> party = {&dragon, &fighter, &wizard, &cleric};

turnManager.InitializeTurnOrder(party);

// Simulate 3 full rounds
for (int round = 1; round <= 3; ++round) {
    for (int i = 0; i < party.size(); ++i) {
        Character* active = turnManager.GetActiveCharacter();

        std::cout << "Round " << turnManager.GetCurrentTurnNumber()
                  << " - " << active->TypeName() << "'s turn" << std::endl;

        // Simulate actions
        active->ConsumeActionPoints(active->GetActionPoints());  // Use all AP

        turnManager.EndCurrentTurn();
    }
}
```

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

**Test Suite 1: Roll Command**

- [ ] **Test_RollCommand_Basic_3d6()**
  
  ```cpp
  // Setup
  DebugConsole console;
  console.ExecuteCommand("roll 3d6");
  
  // Verify: Check Logger.txt for output like:
  // "Rolled 3d6 = [4, 6, 2] = 12"
  // (exact values vary, but format should match)
  ```

- [ ] **Test_RollCommand_WithModifier()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action
  console.ExecuteCommand("roll 2d8+5");
  
  // Verify: Logger should show result in range [7, 21]
  // Format: "Rolled 2d8+5 = [3, 5] = 13"
  ```

- [ ] **Test_RollCommand_NoArgs()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action: Execute without args
  console.ExecuteCommand("roll");
  
  // Verify: Logger shows error "Usage: roll <dice notation>"
  ```

- [ ] **Test_RollCommand_InvalidNotation()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action: Invalid notation
  console.ExecuteCommand("roll xyz");
  
  // Verify: Logger shows error (parser should catch invalid format)
  ```

- [ ] **Test_RollCommand_MultipleRolls()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action: Roll multiple times
  console.ExecuteCommand("roll 1d20");
  console.ExecuteCommand("roll 1d20");
  console.ExecuteCommand("roll 1d20");
  
  // Verify: Each roll logged separately with different results
  ```

**Test Suite 2: SetSeed Command**

- [ ] **Test_SetSeedCommand_Reproducibility()**
  
  ```cpp
  // Setup
  DebugConsole console;
  DiceManager& dice = DiceManager::Instance();
  
  // Set seed
  console.ExecuteCommand("setseed 12345");
  
  // First roll
  int result1 = dice.RollDice(3, 6);
  
  // Reset to same seed
  console.ExecuteCommand("setseed 12345");
  
  // Second roll (should match first)
  int result2 = dice.RollDice(3, 6);
  
  // Assertions
  ASSERT_EQ(result1, result2);
  ```

- [ ] **Test_SetSeedCommand_DifferentSeeds()**
  
  ```cpp
  // Setup
  DebugConsole console;
  DiceManager& dice = DiceManager::Instance();
  
  // Seed 1
  console.ExecuteCommand("setseed 111");
  int result1 = dice.RollDice(10, 20);
  
  // Seed 2
  console.ExecuteCommand("setseed 222");
  int result2 = dice.RollDice(10, 20);
  
  // Results should differ (extremely high probability)
  ASSERT_NE(result1, result2);
  ```

- [ ] **Test_SetSeedCommand_NoArgs()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action: No args
  console.ExecuteCommand("setseed");
  
  // Verify: Logger shows error "Usage: setseed <number>"
  ```

- [ ] **Test_SetSeedCommand_InvalidArg()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Action: Non-numeric arg
  console.ExecuteCommand("setseed abc");
  
  // Verify: Should log error or exception message
  ```

**Test Suite 3: Integration Tests**

- [ ] **Test_RollCommand_AfterSetSeed()**
  
  ```cpp
  // Setup
  DebugConsole console;
  
  // Set seed for reproducibility
  console.ExecuteCommand("setseed 99999");
  
  // Roll dice
  console.ExecuteCommand("roll 5d6");
  
  // Verify: Specific reproducible result
  // (With seed 99999, 5d6 should always give same result)
  ```

- [ ] **Test_RollCommand_LogsIndividualDice()**
  
  ```cpp
  // Setup
  DebugConsole console;
  DiceManager& dice = DiceManager::Instance();
  
  // Action
  console.ExecuteCommand("roll 4d6");
  
  // Verify
  std::vector<int> lastRolls = dice.GetLastRolls();
  ASSERT_EQ(lastRolls.size(), 4);
  
  // Verify each die is in range [1, 6]
  for (int roll : lastRolls) {
      ASSERT_GE(roll, 1);
      ASSERT_LE(roll, 6);
  }
  ```

**Usage Examples**:

**Example 1: Testing Dice Rolls in Debug Console**

```cpp
// Open debug console (~)
DebugConsole console;
console.ToggleConsole();

// Roll different dice
console.ExecuteCommand("roll 3d6");
// Output: Rolled 3d6 = [4, 6, 2] = 12

console.ExecuteCommand("roll 1d20");
// Output: Rolled 1d20 = [15] = 15

console.ExecuteCommand("roll 2d8+5");
// Output: Rolled 2d8+5 = [3, 7] = 15
```

**Example 2: Setting Seed for Reproducible Testing**

```cpp
DebugConsole console;

// Set seed
console.ExecuteCommand("setseed 12345");
// Output: Set dice seed to 12345

// Roll dice (reproducible)
console.ExecuteCommand("roll 3d6");
// Output: Rolled 3d6 = [4, 5, 2] = 11

// Reset seed to same value
console.ExecuteCommand("setseed 12345");

// Same roll gives same result
console.ExecuteCommand("roll 3d6");
// Output: Rolled 3d6 = [4, 5, 2] = 11
```

**Example 3: Testing Combat Damage**

```cpp
DebugConsole console;

// Test Dragon's attack dice (3d6)
std::cout << "Testing Dragon attack damage:" << std::endl;
for (int i = 0; i < 5; ++i) {
    console.ExecuteCommand("roll 3d6");
}
// Output: 5 different rolls showing damage range

// Test Fighter's defense dice (2d8)
std::cout << "Testing Fighter defense rolls:" << std::endl;
for (int i = 0; i < 5; ++i) {
    console.ExecuteCommand("roll 2d8");
}
```

**Example 4: Debugging Spell Damage**

```cpp
DebugConsole console;

// Test Fireball damage at different spell levels
console.ExecuteCommand("roll 3d6");    // Level 1 Fireball
console.ExecuteCommand("roll 4d6");    // Level 2 Fireball
console.ExecuteCommand("roll 5d6");    // Level 3 Fireball

// Verify damage scales correctly
```

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

**Test Suite 1: Grid Overlay Toggle**

- [ ] **Test_ToggleGrid_F1Key()**
  
  ```cpp
  // Setup
  GridDebugRenderer renderer;
  ASSERT_FALSE(renderer.GetShowGrid());  // Initially hidden
  
  // Action: Press F1 (simulate in test)
  renderer.SetShowGrid(true);
  
  // Assertions
  ASSERT_TRUE(renderer.GetShowGrid());
  
  // Toggle off
  renderer.SetShowGrid(false);
  ASSERT_FALSE(renderer.GetShowGrid());
  ```

- [ ] **Test_ToggleGrid_MultipleToggles()**
  
  ```cpp
  // Setup
  GridDebugRenderer renderer;
  
  // Toggle 10 times
  for (int i = 0; i < 10; ++i) {
      bool expectedState = (i % 2 == 0);
      renderer.SetShowGrid(expectedState);
      ASSERT_EQ(renderer.GetShowGrid(), expectedState);
  }
  ```

**Test Suite 2: Coordinate Display Toggle**

- [ ] **Test_ToggleCoordinates()**
  
  ```cpp
  // Setup
  GridDebugRenderer renderer;
  ASSERT_FALSE(renderer.GetShowCoordinates());
  
  // Action: Enable coordinates
  renderer.SetShowCoordinates(true);
  
  // Assertions
  ASSERT_TRUE(renderer.GetShowCoordinates());
  
  // Disable
  renderer.SetShowCoordinates(false);
  ASSERT_FALSE(renderer.GetShowCoordinates());
  ```

**Test Suite 3: Drawing (Visual Tests)**

- [ ] **Test_DrawGridLines_NoErrorsOrCrashes()**
  
  ```cpp
  // Setup
  GridDebugRenderer renderer;
  renderer.SetShowGrid(true);
  
  Math::TransformationMatrix identity;
  
  // Action: Draw (should not crash)
  renderer.Draw(identity);
  
  // If we get here, test passed
  ASSERT_TRUE(true);
  ```

- [ ] **Test_DrawTileCoordinates_NoErrorsOrCrashes()**
  
  ```cpp
  // Setup
  GridDebugRenderer renderer;
  renderer.SetShowCoordinates(true);
  
  Math::TransformationMatrix identity;
  
  // Action: Draw (should not crash)
  renderer.Draw(identity);
  
  // If we get here, test passed
  ASSERT_TRUE(true);
  ```

- [ ] **Test_DrawBoth_GridAndCoordinates()**
  
  ```cpp
  // Setup
  GridDebugRenderer renderer;
  renderer.SetShowGrid(true);
  renderer.SetShowCoordinates(true);
  
  Math::TransformationMatrix identity;
  
  // Action: Draw both (should not crash)
  renderer.Draw(identity);
  
  // If we get here, test passed
  ASSERT_TRUE(true);
  ```

**Test Suite 4: Integration with GridSystem**

- [ ] **Test_DrawGrid_MatchesGridDimensions()**
  
  ```cpp
  // Setup
  GridSystem grid;
  GridDebugRenderer renderer;
  renderer.SetShowGrid(true);
  
  // Grid is 8x8, should draw 9 lines horizontally and 9 vertically
  // (This is a visual test - verify manually or with screenshot)
  
  Math::TransformationMatrix identity;
  renderer.Draw(identity);
  
  ASSERT_TRUE(true);
  ```

- [ ] **Test_DrawCoordinates_AllTilesLabeled()**
  
  ```cpp
  // Setup
  GridSystem grid;
  GridDebugRenderer renderer;
  renderer.SetShowCoordinates(true);
  
  // Should draw labels for all 64 tiles (0,0) to (7,7)
  // (This is a visual test)
  
  Math::TransformationMatrix identity;
  renderer.Draw(identity);
  
  ASSERT_TRUE(true);
  ```

**Test Suite 5: Keyboard Shortcuts**

- [ ] **Test_F1Key_TogglesGrid()**
  
  ```cpp
  // Setup
  GridDebugRenderer renderer;
  
  // Simulate F1 press in Update()
  // (Manually test in-game)
  
  // Expected: Grid toggles on/off with each press
  ```

**Usage Examples**:

**Example 1: Enabling Grid Overlay**

```cpp
GridDebugRenderer renderer;

// Enable grid overlay
renderer.SetShowGrid(true);

// In game loop Update()
if (Engine::GetInput().IsKeyPressed(InputKey::F1)) {
    renderer.SetShowGrid(!renderer.GetShowGrid());
    std::cout << "Grid overlay: " << (renderer.GetShowGrid() ? "ON" : "OFF") << std::endl;
}

// In game loop Draw()
renderer.Draw(cameraMatrix);
```

**Example 2: Showing Tile Coordinates**

```cpp
GridDebugRenderer renderer;

// Enable coordinate display
renderer.SetShowCoordinates(true);

// Draw
Math::TransformationMatrix identity;
renderer.Draw(identity);

// Output: Each tile labeled (0,0), (0,1), ..., (7,7)
```

**Example 3: Debug Mode Toggle**

```cpp
class BattleState : public CS230::GameState {
private:
    GridDebugRenderer* debugRenderer;

public:
    void Load() override {
        debugRenderer = new GridDebugRenderer();
        debugRenderer->SetShowGrid(false);
        debugRenderer->SetShowCoordinates(false);
    }

    void Update(double dt) override {
        // F1: Toggle grid
        if (Engine::GetInput().IsKeyPressed(InputKey::F1)) {
            debugRenderer->SetShowGrid(!debugRenderer->GetShowGrid());
            Engine::GetLogger().LogEvent("Grid overlay toggled");
        }

        // F2: Toggle coordinates
        if (Engine::GetInput().IsKeyPressed(InputKey::F2)) {
            debugRenderer->SetShowCoordinates(!debugRenderer->GetShowCoordinates());
            Engine::GetLogger().LogEvent("Coordinate labels toggled");
        }
    }

    void Draw() override {
        // Draw game objects...

        // Draw debug overlay last (on top)
        debugRenderer->Draw(GetCamera().GetMatrix());
    }
};
```

**Example 4: Integration with Debug Console**

```cpp
DebugConsole console;

// Register commands for grid rendering
console.RegisterCommand("grid",
    [](std::vector<std::string> args) {
        GridDebugRenderer* renderer = /* get renderer */;
        renderer->SetShowGrid(!renderer->GetShowGrid());
        Engine::GetLogger().LogEvent("Grid overlay toggled");
    },
    "Toggle grid overlay"
);

console.RegisterCommand("coords",
    [](std::vector<std::string> args) {
        GridDebugRenderer* renderer = /* get renderer */;
        renderer->SetShowCoordinates(!renderer->GetShowCoordinates());
        Engine::GetLogger().LogEvent("Coordinate labels toggled");
    },
    "Toggle tile coordinate labels"
);

// Usage:
// > grid       // Toggle grid
// > coords     // Toggle coordinates
```

**Example 5: Debugging Character Movement**

```cpp
GridDebugRenderer renderer;
renderer.SetShowGrid(true);
renderer.SetShowCoordinates(true);

// Move dragon
Dragon dragon;
dragon.MoveToTile({5, 5});

// With grid enabled, you can visually verify:
// - Grid lines align with tile boundaries
// - Coordinate (5,5) matches dragon's position
// - Pathfinding follows grid correctly
```

**Example 6: Custom Drawing for Highlighted Tiles**

```cpp
class GridDebugRenderer : public CS230::Component {
private:
    std::vector<Math::vec2> highlightedTiles;

public:
    void HighlightTiles(const std::vector<Math::vec2>& tiles) {
        highlightedTiles = tiles;
    }

    void Draw(Math::TransformationMatrix camera_matrix) override {
        if (showGrid) {
            DrawGridLines();
        }

        if (showCoordinates) {
            DrawTileCoordinates();
        }

        // Draw highlighted tiles (e.g., reachable tiles)
        for (const auto& tile : highlightedTiles) {
            DrawHighlightedTile(tile);  // Custom visual (green outline, etc.)
        }
    }
};

// Usage
GridDebugRenderer renderer;
GridSystem grid;
Dragon dragon;

// Highlight reachable tiles
std::vector<Math::vec2> reachable = grid.GetReachableTiles(&dragon);
renderer.HighlightTiles(reachable);

// Draw (reachable tiles shown in green)
renderer.Draw(cameraMatrix);
```

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
