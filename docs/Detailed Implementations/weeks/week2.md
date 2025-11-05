# Playtest 1 Implementation Plan - Week 2

**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Dragon + Grid + Combat Foundation
**Timeline**: Week 2 of 5 (Playtest 1 milestone)
**Strategy**: All systems connect - Dragon and Fighter on grid with combat

**Last Updated**: 2025-10-15
**Week 1 Status**: Foundation complete (EventBus, DiceManager, GridSystem, Character base class)

**Related Documentation**:

- [Week 1](week1.md) for foundation systems
- [docs/implementation-plan.md](../../implementation-plan.md) for complete 26-week timeline

---

## Overview

Week 2 builds on the Week 1 foundation to create the first playable characters (Dragon and Fighter) with grid movement and basic combat.

**Critical Success Criteria:**

- ✅ **Dragon class**: Extends Character, basic movement, HP=250, 3d8 attack
- ✅ **Fighter class**: Extends Character, HP=90, 2d6 attack
- ✅ **GridSystem pathfinding**: A* algorithm, GetReachableTiles(), movement validation
- ✅ **TurnManager basics**: InitializeTurnOrder(), turn progression, AP refresh
- ✅ **CombatSystem foundation**: ApplyDamage(), dice-based damage calculation, death handling

**Integration Goal (Friday Week 2)**:

- Dragon and Fighter spawn on grid
- Dragon moves using A* pathfinding
- Basic combat damage calculation works
- Turn order initializes and progresses properly

**Meeting Schedule:**

- **Daily Standups**: 10 min each morning (9:00 AM)
- **Friday Integration Meeting**: 45 min (2:00 PM - demos + Week 3 planning)

---

## Table of Contents

- [Week 2 Overview](#overview)
- [Developer A: Dragon Class](#week-2-developer-a-dragon-class)
  - [Implementation Tasks](#implementation-tasks-dragon)
  - [Rigorous Testing](#rigorous-testing-dragon)
  - [Daily Breakdown](#daily-breakdown-developer-a)
- [Developer B: GridSystem Pathfinding](#week-2-developer-b-gridsystem-pathfinding)
  - [Implementation Tasks](#implementation-tasks-pathfinding)
  - [Rigorous Testing](#rigorous-testing-pathfinding)
  - [Daily Breakdown](#daily-breakdown-developer-b)
- [Developer C: TurnManager Basics](#week-2-developer-c-turnmanager-basics)
  - [Implementation Tasks](#implementation-tasks-turnmanager)
  - [Rigorous Testing](#rigorous-testing-turnmanager)
  - [Daily Breakdown](#daily-breakdown-developer-c)
- [Developer D: Fighter Enemy Class](#week-2-developer-d-fighter-enemy-class)
  - [Implementation Tasks](#implementation-tasks-fighter)
  - [Rigorous Testing](#rigorous-testing-fighter)
  - [Daily Breakdown](#daily-breakdown-developer-d)
- [Developer E: CombatSystem Foundation](#week-2-developer-e-combatsystem-foundation)
  - [Implementation Tasks](#implementation-tasks-combatsystem)
  - [Rigorous Testing](#rigorous-testing-combatsystem)
  - [Daily Breakdown](#daily-breakdown-developer-e)
- [Week 2 Integration Test](#week-2-integration-test-friday-afternoon)
- [Week 2 Deliverable & Verification](#week-2-deliverable--verification)

---

## Week 2: Developer A - Dragon Class

**Goal**: Complete Dragon character class with movement and basic attributes

**Foundation**: Character base class from Week 1 provides HP, AP, state machine, and basic methods

**Files to Create**:

```
CS230/Game/Characters/Dragon.h
CS230/Game/Characters/Dragon.cpp
CS230/Assets/Dragon.spt (sprite - placeholder OK for Week 2)
```

### Implementation Tasks (Dragon)

#### **Task 1: Dragon Class Declaration**

```cpp
// File: CS230/Game/Characters/Dragon.h
#pragma once
#include "Character.h"
#include "../../Engine/Vec2.h"
#include <string>

class Dragon : public Character {
public:
    Dragon(Math::vec2 position);
    ~Dragon() override = default;

    // GameObject overrides
    GameObjectTypes Type() override { return GameObjectTypes::Dragon; }
    std::string TypeName() const override { return "Dragon"; }

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;

    // Dragon-specific stats (inherited from Character)
    // HP: 250
    // Speed: 3 tiles per turn
    // Attack: 3d8 + 5
    // Action Points: 5 per turn

private:
    void InitializeDragonStats();
};
```

#### **Task 2: Dragon Constructor Implementation**

```cpp
// File: CS230/Game/Characters/Dragon.cpp
#include "Dragon.h"
#include "../Engine/Engine.h"

Dragon::Dragon(Math::vec2 position)
    : Character(position)
{
    InitializeDragonStats();

    // Set grid position
    gridPosition = position;

    // Set sprite (placeholder for Week 2)
    AddGOComponent(new CS230::Sprite("Assets/Dragon.spt", this));

    Engine::GetLogger().LogEvent("Dragon created at (" +
        std::to_string((int)position.x) + ", " +
        std::to_string((int)position.y) + ")");
}

void Dragon::InitializeDragonStats() {
    // Health
    maxHP = 250;
    currentHP = 250;

    // Movement
    speed = 3;  // Can move 3 tiles per turn

    // Action Points
    maxActionPoints = 5;
    currentActionPoints = 5;

    // Attack
    baseAttackPower = 5;
    attackDice = "3d8";  // 3d8 + 5 damage
    attackRange = 1;     // Melee range for Week 2 (spells in Week 3)

    // Defense
    baseDefensePower = 2;
    defenseDice = "2d6";

    // Spell slots (for Week 3 - prepare structure now)
    spellSlots[1] = 4;   // 4 level 1 slots
    spellSlots[2] = 3;   // 3 level 2 slots
    spellSlots[3] = 2;   // 2 level 3 slots
}

void Dragon::Update(double dt) {
    Character::Update(dt);  // Call base class Update (state machine)

    // Dragon-specific update logic (placeholder for Week 3+)
}

void Dragon::Draw(Math::TransformationMatrix camera_matrix) {
    Character::Draw(camera_matrix);  // Draw sprite

    // TODO Week 3: Draw spell indicators, HP bar, etc.
}
```

#### **Task 3: Movement Integration with GridSystem**

```cpp
// Add to Dragon class
public:
    // Movement methods (use GridSystem from Week 1)
    bool MoveTo(Math::vec2 targetPosition);
    std::vector<Math::vec2> GetReachableTiles() const;
    int GetMovementRange() const override { return speed; }

private:
    bool IsValidMove(Math::vec2 targetPosition) const;
```

```cpp
// Implementation
#include "../System/GridSystem.h"

bool Dragon::MoveTo(Math::vec2 targetPosition) {
    if (!IsValidMove(targetPosition)) {
        Engine::GetLogger().LogError("Dragon: Invalid move to (" +
            std::to_string((int)targetPosition.x) + ", " +
            std::to_string((int)targetPosition.y) + ")");
        return false;
    }

    // Calculate path length (will use A* in integration)
    int pathLength = GridSystem::Instance().GetPathLength(gridPosition, targetPosition);

    if (pathLength > speed) {
        Engine::GetLogger().LogError("Dragon: Move too far (distance=" +
            std::to_string(pathLength) + ", speed=" + std::to_string(speed) + ")");
        return false;
    }

    // Move character
    GridSystem::Instance().MoveCharacter(this, gridPosition, targetPosition);
    gridPosition = targetPosition;

    // Consume AP (1 AP per tile moved)
    ConsumeActionPoints(pathLength);

    // Publish event
    EventBus::Instance().Publish(CharacterMovedEvent{this, gridPosition, targetPosition});

    Engine::GetLogger().LogEvent("Dragon moved to (" +
        std::to_string((int)targetPosition.x) + ", " +
        std::to_string((int)targetPosition.y) + ")");

    return true;
}

std::vector<Math::vec2> Dragon::GetReachableTiles() const {
    return GridSystem::Instance().GetReachableTiles(gridPosition, speed);
}

bool Dragon::IsValidMove(Math::vec2 targetPosition) const {
    // Check if tile is on the grid
    if (!GridSystem::Instance().IsValidTile(targetPosition)) {
        return false;
    }

    // Check if tile is walkable
    if (!GridSystem::Instance().IsWalkable(targetPosition)) {
        return false;
    }

    // Check if tile is occupied
    if (GridSystem::Instance().IsOccupied(targetPosition)) {
        return false;
    }

    return true;
}
```

### Rigorous Testing (Dragon)

#### **Test Suite 1: Dragon Initialization**

```cpp
// File: CS230/Game/Test/Test_Dragon.cpp
#include "../Characters/Dragon.h"
#include "Week1TestMocks.h"

bool Test_Dragon_Constructor() {
    // Setup
    Math::vec2 spawnPos{4, 4};

    // Action
    Dragon dragon(spawnPos);

    // Assertions - Stats
    ASSERT_EQ(dragon.GetMaxHP(), 250);
    ASSERT_EQ(dragon.GetCurrentHP(), 250);
    ASSERT_EQ(dragon.GetSpeed(), 3);
    ASSERT_EQ(dragon.GetActionPoints(), 5);
    ASSERT_TRUE(dragon.IsAlive());

    // Assertions - Position
    ASSERT_EQ(dragon.GetGridPosition().x, 4);
    ASSERT_EQ(dragon.GetGridPosition().y, 4);

    // Assertions - Type
    ASSERT_EQ(dragon.Type(), GameObjectTypes::Dragon);
    ASSERT_EQ(dragon.TypeName(), "Dragon");

    std::cout << "✅ Test_Dragon_Constructor passed" << std::endl;
    return true;
}

bool Test_Dragon_Attack_Dice() {
    // Setup
    Dragon dragon({0, 0});

    // Action - Get attack dice string
    std::string attackDice = dragon.GetAttackDice();

    // Assertions
    ASSERT_EQ(attackDice, "3d8");
    ASSERT_EQ(dragon.GetBaseAttackPower(), 5);

    // Verify damage range is reasonable (3d8+5 = 8-29 damage)
    // Note: Actual rolling happens in CombatSystem, we just verify stats

    std::cout << "✅ Test_Dragon_Attack_Dice passed" << std::endl;
    return true;
}

bool Test_Dragon_SpellSlots_Prepared() {
    // Setup
    Dragon dragon({0, 0});

    // Action - Check spell slots (for Week 3 spells)
    int level1Slots = dragon.GetSpellSlots(1);
    int level2Slots = dragon.GetSpellSlots(2);
    int level3Slots = dragon.GetSpellSlots(3);

    // Assertions
    ASSERT_EQ(level1Slots, 4);
    ASSERT_EQ(level2Slots, 3);
    ASSERT_EQ(level3Slots, 2);

    std::cout << "✅ Test_Dragon_SpellSlots_Prepared passed" << std::endl;
    return true;
}
```

#### **Test Suite 2: Dragon Movement**

```cpp
bool Test_Dragon_MoveTo_Valid() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({4, 4});
    GridSystem::Instance().PlaceCharacter(&dragon, {4, 4});

    // Action - Move 2 tiles (within speed=3)
    bool success = dragon.MoveTo({4, 6});

    // Assertions
    ASSERT_TRUE(success);
    ASSERT_EQ(dragon.GetGridPosition().x, 4);
    ASSERT_EQ(dragon.GetGridPosition().y, 6);
    ASSERT_EQ(dragon.GetActionPoints(), 3);  // 5 - 2 = 3

    // Verify old position is empty
    ASSERT_FALSE(GridSystem::Instance().IsOccupied({4, 4}));
    // Verify new position is occupied
    ASSERT_TRUE(GridSystem::Instance().IsOccupied({4, 6}));

    std::cout << "✅ Test_Dragon_MoveTo_Valid passed" << std::endl;
    return true;
}

bool Test_Dragon_MoveTo_TooFar() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({4, 4});
    GridSystem::Instance().PlaceCharacter(&dragon, {4, 4});

    // Action - Try to move 5 tiles (speed is only 3)
    bool success = dragon.MoveTo({4, 9});  // Would need to move 5 tiles

    // Assertions - Move should fail
    ASSERT_FALSE(success);
    ASSERT_EQ(dragon.GetGridPosition().x, 4);
    ASSERT_EQ(dragon.GetGridPosition().y, 4);  // Still at original position
    ASSERT_EQ(dragon.GetActionPoints(), 5);    // AP not consumed

    std::cout << "✅ Test_Dragon_MoveTo_TooFar passed" << std::endl;
    return true;
}

bool Test_Dragon_MoveTo_Occupied() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({4, 4});
    Fighter fighter({4, 5});  // Fighter blocks the path
    GridSystem::Instance().PlaceCharacter(&dragon, {4, 4});
    GridSystem::Instance().PlaceCharacter(&fighter, {4, 5});

    // Action - Try to move to occupied tile
    bool success = dragon.MoveTo({4, 5});

    // Assertions - Move should fail
    ASSERT_FALSE(success);
    ASSERT_EQ(dragon.GetGridPosition().x, 4);
    ASSERT_EQ(dragon.GetGridPosition().y, 4);  // Still at original position

    std::cout << "✅ Test_Dragon_MoveTo_Occupied passed" << std::endl;
    return true;
}

bool Test_Dragon_MoveTo_OutOfBounds() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({4, 4});
    GridSystem::Instance().PlaceCharacter(&dragon, {4, 4});

    // Action - Try to move off the grid (assuming 8x8 grid)
    bool success = dragon.MoveTo({10, 10});  // Out of bounds

    // Assertions
    ASSERT_FALSE(success);
    ASSERT_EQ(dragon.GetGridPosition().x, 4);
    ASSERT_EQ(dragon.GetGridPosition().y, 4);

    std::cout << "✅ Test_Dragon_MoveTo_OutOfBounds passed" << std::endl;
    return true;
}

bool Test_Dragon_GetReachableTiles() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({4, 4});
    GridSystem::Instance().PlaceCharacter(&dragon, {4, 4});

    // Action
    std::vector<Math::vec2> reachableTiles = dragon.GetReachableTiles();

    // Assertions
    // With speed=3, should have access to tiles within 3 Manhattan distance
    // Exact count depends on grid obstacles, but should be > 0
    ASSERT_TRUE(reachableTiles.size() > 0);
    ASSERT_TRUE(reachableTiles.size() <= 24);  // Max possible in 3-tile radius

    // Verify current position NOT in reachable tiles
    bool currentPosInReachable = false;
    for (const auto& tile : reachableTiles) {
        if (tile.x == 4 && tile.y == 4) {
            currentPosInReachable = true;
        }
    }
    ASSERT_FALSE(currentPosInReachable);  // Can't move to current position

    std::cout << "✅ Test_Dragon_GetReachableTiles passed" << std::endl;
    return true;
}
```

#### **Test Suite 3: Dragon Combat Integration**

```cpp
bool Test_Dragon_TakeDamage_Integration() {
    // Setup
    Dragon dragon({0, 0});
    ASSERT_EQ(dragon.GetCurrentHP(), 250);

    // Action - Simulate taking damage
    dragon.TakeDamage(50);

    // Assertions
    ASSERT_EQ(dragon.GetCurrentHP(), 200);
    ASSERT_TRUE(dragon.IsAlive());

    std::cout << "✅ Test_Dragon_TakeDamage_Integration passed" << std::endl;
    return true;
}

bool Test_Dragon_Die() {
    // Setup
    Dragon dragon({0, 0});
    dragon.SetCurrentHP(10);

    // Action - Fatal damage
    dragon.TakeDamage(15);

    // Assertions
    ASSERT_EQ(dragon.GetCurrentHP(), 0);
    ASSERT_FALSE(dragon.IsAlive());
    ASSERT_EQ(dragon.GetCurrentState()->GetName(), "Dead");

    std::cout << "✅ Test_Dragon_Die passed" << std::endl;
    return true;
}
```

### Daily Breakdown (Developer A)

**Monday (Day 1)**: Dragon class structure + initialization

- Morning: Create Dragon.h/cpp, implement constructor
- Afternoon: Implement InitializeDragonStats(), test initialization
- **Deliverable**: Dragon spawns with correct stats (HP=250, speed=3)

**Tuesday (Day 2)**: Movement methods

- Morning: Implement MoveTo(), GetReachableTiles()
- Afternoon: Test movement validation (out of bounds, occupied, too far)
- **Deliverable**: Dragon can validate moves

**Wednesday (Day 3)**: GridSystem integration

- Morning: Integrate with GridSystem pathfinding (Developer B coordination)
- Afternoon: Test actual movement on grid
- **Deliverable**: Dragon moves on grid consuming correct AP

**Thursday (Day 4)**: Combat integration

- Morning: Integrate TakeDamage() with EventBus
- Afternoon: Test combat scenarios, death handling
- **Deliverable**: Dragon takes damage, publishes events

**Friday (Day 5)**: Integration testing + polish

- Morning: Integration tests with Fighter (Developer D)
- Afternoon: Bug fixes, Friday demo prep
- **Deliverable**: Dragon vs Fighter demo ready

---

## Week 2: Developer B - GridSystem Pathfinding

**Goal**: Add A* pathfinding to GridSystem for intelligent movement

**Foundation**: GridSystem from Week 1 has 8x8 grid, IsValidTile(), IsWalkable(), IsOccupied()

**Files to Modify**:

```
CS230/Game/System/GridSystem.h (add pathfinding methods)
CS230/Game/System/GridSystem.cpp (implement A*)
```

### Implementation Tasks (Pathfinding)

#### **Task 1: A* Pathfinding Data Structures**

```cpp
// File: CS230/Game/System/GridSystem.h
#pragma once
#include <vector>
#include <queue>
#include <map>
#include "../../Engine/Vec2.h"

class Character;  // Forward declaration

class GridSystem {
public:
    // Week 1 methods (already implemented)
    static GridSystem& Instance();
    void Reset();
    bool IsValidTile(Math::vec2 position) const;
    bool IsWalkable(Math::vec2 position) const;
    bool IsOccupied(Math::vec2 position) const;
    void PlaceCharacter(Character* character, Math::vec2 position);
    void MoveCharacter(Character* character, Math::vec2 from, Math::vec2 to);
    Character* GetCharacterAt(Math::vec2 position) const;

    // Week 2: Pathfinding methods
    std::vector<Math::vec2> FindPath(Math::vec2 start, Math::vec2 goal);
    int GetPathLength(Math::vec2 start, Math::vec2 goal);
    std::vector<Math::vec2> GetReachableTiles(Math::vec2 start, int maxDistance);

    // Week 2: Helper methods
    int ManhattanDistance(Math::vec2 a, Math::vec2 b) const;
    std::vector<Math::vec2> GetNeighbors(Math::vec2 position) const;

private:
    GridSystem() = default;

    // A* pathfinding node
    struct Node {
        Math::vec2 position;
        int gCost;  // Distance from start
        int hCost;  // Heuristic distance to goal
        int fCost() const { return gCost + hCost; }
        Node* parent;

        Node(Math::vec2 pos, int g, int h, Node* p = nullptr)
            : position(pos), gCost(g), hCost(h), parent(p) {}
    };

    // Grid data
    static constexpr int GRID_WIDTH = 8;
    static constexpr int GRID_HEIGHT = 8;
    Character* grid[GRID_HEIGHT][GRID_WIDTH];
    TileType tileTypes[GRID_HEIGHT][GRID_WIDTH];
};
```

#### **Task 2: A* Algorithm Implementation**

```cpp
// File: CS230/Game/System/GridSystem.cpp
#include "GridSystem.h"
#include "../../Engine/Engine.h"
#include <algorithm>
#include <cmath>

std::vector<Math::vec2> GridSystem::FindPath(Math::vec2 start, Math::vec2 goal) {
    // Edge cases
    if (!IsValidTile(start) || !IsValidTile(goal)) {
        Engine::GetLogger().LogError("GridSystem: Invalid start or goal position");
        return {};
    }

    if (!IsWalkable(goal)) {
        Engine::GetLogger().LogError("GridSystem: Goal is not walkable");
        return {};
    }

    if (start.x == goal.x && start.y == goal.y) {
        return {start};  // Already at goal
    }

    // A* algorithm
    std::vector<Node*> openSet;
    std::vector<Node*> closedSet;
    std::map<std::pair<int, int>, Node*> allNodes;

    // Create start node
    Node* startNode = new Node(start, 0, ManhattanDistance(start, goal));
    openSet.push_back(startNode);
    allNodes[{(int)start.x, (int)start.y}] = startNode;

    Node* goalNode = nullptr;

    while (!openSet.empty()) {
        // Find node with lowest fCost
        auto minIt = std::min_element(openSet.begin(), openSet.end(),
            [](Node* a, Node* b) { return a->fCost() < b->fCost(); });

        Node* current = *minIt;
        openSet.erase(minIt);
        closedSet.push_back(current);

        // Check if we reached the goal
        if (current->position.x == goal.x && current->position.y == goal.y) {
            goalNode = current;
            break;
        }

        // Check neighbors
        std::vector<Math::vec2> neighbors = GetNeighbors(current->position);
        for (const Math::vec2& neighborPos : neighbors) {
            // Skip if not walkable or in closed set
            if (!IsWalkable(neighborPos)) continue;
            if (IsOccupied(neighborPos) && !(neighborPos.x == goal.x && neighborPos.y == goal.y)) {
                continue;  // Allow goal tile even if occupied
            }

            bool inClosedSet = false;
            for (Node* closed : closedSet) {
                if (closed->position.x == neighborPos.x && closed->position.y == neighborPos.y) {
                    inClosedSet = true;
                    break;
                }
            }
            if (inClosedSet) continue;

            int newGCost = current->gCost + 1;  // Each tile costs 1

            // Check if neighbor is in open set
            auto nodeKey = std::make_pair((int)neighborPos.x, (int)neighborPos.y);
            Node* neighborNode = nullptr;

            if (allNodes.find(nodeKey) != allNodes.end()) {
                neighborNode = allNodes[nodeKey];
                if (newGCost < neighborNode->gCost) {
                    // Found a better path
                    neighborNode->gCost = newGCost;
                    neighborNode->parent = current;
                }
            } else {
                // Create new node
                neighborNode = new Node(
                    neighborPos,
                    newGCost,
                    ManhattanDistance(neighborPos, goal),
                    current
                );
                openSet.push_back(neighborNode);
                allNodes[nodeKey] = neighborNode;
            }
        }
    }

    // Reconstruct path
    std::vector<Math::vec2> path;
    if (goalNode != nullptr) {
        Node* current = goalNode;
        while (current != nullptr) {
            path.push_back(current->position);
            current = current->parent;
        }
        std::reverse(path.begin(), path.end());
    }

    // Cleanup
    for (auto& pair : allNodes) {
        delete pair.second;
    }

    if (path.empty()) {
        Engine::GetLogger().LogError("GridSystem: No path found from (" +
            std::to_string((int)start.x) + "," + std::to_string((int)start.y) + ") to (" +
            std::to_string((int)goal.x) + "," + std::to_string((int)goal.y) + ")");
    }

    return path;
}

int GridSystem::GetPathLength(Math::vec2 start, Math::vec2 goal) {
    std::vector<Math::vec2> path = FindPath(start, goal);
    return path.empty() ? -1 : (int)path.size() - 1;  // -1 because start tile doesn't count
}

std::vector<Math::vec2> GridSystem::GetReachableTiles(Math::vec2 start, int maxDistance) {
    std::vector<Math::vec2> reachable;

    // Check all tiles on the grid
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            Math::vec2 tile{(double)x, (double)y};

            // Skip start tile
            if (tile.x == start.x && tile.y == start.y) continue;

            // Skip if not walkable
            if (!IsWalkable(tile)) continue;

            // Skip if occupied
            if (IsOccupied(tile)) continue;

            // Check path length
            int pathLength = GetPathLength(start, tile);
            if (pathLength > 0 && pathLength <= maxDistance) {
                reachable.push_back(tile);
            }
        }
    }

    return reachable;
}

int GridSystem::ManhattanDistance(Math::vec2 a, Math::vec2 b) const {
    return (int)(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

std::vector<Math::vec2> GridSystem::GetNeighbors(Math::vec2 position) const {
    std::vector<Math::vec2> neighbors;

    // 4-directional movement (up, down, left, right)
    const Math::vec2 directions[] = {
        {0, 1},   // Up
        {0, -1},  // Down
        {-1, 0},  // Left
        {1, 0}    // Right
    };

    for (const Math::vec2& dir : directions) {
        Math::vec2 neighbor = position + dir;
        if (IsValidTile(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}
```

### Rigorous Testing (Pathfinding)

#### **Test Suite 1: A* Pathfinding Basic**

```cpp
// File: CS230/Game/Test/Test_GridSystem_Pathfinding.cpp
#include "../System/GridSystem.h"
#include "Week1TestMocks.h"

bool Test_Pathfinding_StraightLine() {
    // Setup
    GridSystem::Instance().Reset();
    Math::vec2 start{0, 0};
    Math::vec2 goal{0, 3};

    // Action
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath(start, goal);

    // Assertions
    ASSERT_EQ(path.size(), 4);  // {0,0}, {0,1}, {0,2}, {0,3}
    ASSERT_EQ(path[0].x, 0);
    ASSERT_EQ(path[0].y, 0);
    ASSERT_EQ(path[3].x, 0);
    ASSERT_EQ(path[3].y, 3);

    // Verify path is continuous
    for (size_t i = 1; i < path.size(); ++i) {
        int distance = GridSystem::Instance().ManhattanDistance(path[i-1], path[i]);
        ASSERT_EQ(distance, 1);  // Each step should be exactly 1 tile apart
    }

    std::cout << "✅ Test_Pathfinding_StraightLine passed" << std::endl;
    return true;
}

bool Test_Pathfinding_AroundObstacle() {
    // Setup
    GridSystem::Instance().Reset();

    // Create wall obstacle at (1, 0), (1, 1), (1, 2)
    GridSystem::Instance().SetTileType({1, 0}, TileType::Wall);
    GridSystem::Instance().SetTileType({1, 1}, TileType::Wall);
    GridSystem::Instance().SetTileType({1, 2}, TileType::Wall);

    Math::vec2 start{0, 1};
    Math::vec2 goal{2, 1};

    // Action
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath(start, goal);

    // Assertions - Should find path around the wall
    ASSERT_TRUE(path.size() > 3);  // Must go around, so longer than direct path
    ASSERT_EQ(path[0].x, 0);
    ASSERT_EQ(path[0].y, 1);
    ASSERT_EQ(path[path.size()-1].x, 2);
    ASSERT_EQ(path[path.size()-1].y, 1);

    // Verify path doesn't go through walls
    for (const Math::vec2& tile : path) {
        ASSERT_TRUE(GridSystem::Instance().IsWalkable(tile));
    }

    std::cout << "✅ Test_Pathfinding_AroundObstacle passed" << std::endl;
    return true;
}

bool Test_Pathfinding_NoPath() {
    // Setup
    GridSystem::Instance().Reset();

    // Create impassable wall blocking the goal
    for (int y = 0; y < 8; ++y) {
        GridSystem::Instance().SetTileType({3, y}, TileType::Wall);
    }

    Math::vec2 start{0, 0};
    Math::vec2 goal{5, 5};  // On the other side of the wall

    // Action
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath(start, goal);

    // Assertions - Should return empty path
    ASSERT_EQ(path.size(), 0);

    std::cout << "✅ Test_Pathfinding_NoPath passed" << std::endl;
    return true;
}

bool Test_Pathfinding_AlreadyAtGoal() {
    // Setup
    GridSystem::Instance().Reset();
    Math::vec2 start{4, 4};
    Math::vec2 goal{4, 4};

    // Action
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath(start, goal);

    // Assertions
    ASSERT_EQ(path.size(), 1);
    ASSERT_EQ(path[0].x, 4);
    ASSERT_EQ(path[0].y, 4);

    std::cout << "✅ Test_Pathfinding_AlreadyAtGoal passed" << std::endl;
    return true;
}
```

#### **Test Suite 2: GetPathLength**

```cpp
bool Test_GetPathLength_Direct() {
    // Setup
    GridSystem::Instance().Reset();

    // Action
    int length = GridSystem::Instance().GetPathLength({0, 0}, {0, 3});

    // Assertions
    ASSERT_EQ(length, 3);  // 3 steps: (0,0)->(0,1)->(0,2)->(0,3)

    std::cout << "✅ Test_GetPathLength_Direct passed" << std::endl;
    return true;
}

bool Test_GetPathLength_NoPath() {
    // Setup
    GridSystem::Instance().Reset();

    // Block all paths
    for (int y = 0; y < 8; ++y) {
        GridSystem::Instance().SetTileType({3, y}, TileType::Wall);
    }

    // Action
    int length = GridSystem::Instance().GetPathLength({0, 0}, {5, 5});

    // Assertions
    ASSERT_EQ(length, -1);  // No path found

    std::cout << "✅ Test_GetPathLength_NoPath passed" << std::endl;
    return true;
}
```

#### **Test Suite 3: GetReachableTiles**

```cpp
bool Test_GetReachableTiles_CenterGrid() {
    // Setup
    GridSystem::Instance().Reset();
    Math::vec2 start{4, 4};  // Center of 8x8 grid
    int maxDistance = 2;

    // Action
    std::vector<Math::vec2> reachable = GridSystem::Instance().GetReachableTiles(start, maxDistance);

    // Assertions
    // With maxDistance=2, should reach 12 tiles (4-directional movement)
    // Not exactly 12 because edges of grid may limit it, but should be > 8
    ASSERT_GE(reachable.size(), 8);
    ASSERT_LE(reachable.size(), 12);

    // Verify all tiles are within distance
    for (const Math::vec2& tile : reachable) {
        int pathLength = GridSystem::Instance().GetPathLength(start, tile);
        ASSERT_GE(pathLength, 1);
        ASSERT_LE(pathLength, maxDistance);
    }

    // Verify start tile NOT in reachable tiles
    for (const Math::vec2& tile : reachable) {
        ASSERT_FALSE(tile.x == start.x && tile.y == start.y);
    }

    std::cout << "✅ Test_GetReachableTiles_CenterGrid passed" << std::endl;
    return true;
}

bool Test_GetReachableTiles_CornerGrid() {
    // Setup
    GridSystem::Instance().Reset();
    Math::vec2 start{0, 0};  // Corner
    int maxDistance = 3;

    // Action
    std::vector<Math::vec2> reachable = GridSystem::Instance().GetReachableTiles(start, maxDistance);

    // Assertions - Should have fewer reachable tiles due to corner
    ASSERT_GE(reachable.size(), 6);  // At least 6 tiles reachable from corner

    std::cout << "✅ Test_GetReachableTiles_CornerGrid passed" << std::endl;
    return true;
}

bool Test_GetReachableTiles_WithObstacles() {
    // Setup
    GridSystem::Instance().Reset();

    // Place walls to limit movement
    GridSystem::Instance().SetTileType({2, 0}, TileType::Wall);
    GridSystem::Instance().SetTileType({2, 1}, TileType::Wall);
    GridSystem::Instance().SetTileType({2, 2}, TileType::Wall);

    Math::vec2 start{0, 1};
    int maxDistance = 3;

    // Action
    std::vector<Math::vec2> reachable = GridSystem::Instance().GetReachableTiles(start, maxDistance);

    // Assertions - Should have fewer tiles due to walls
    ASSERT_GE(reachable.size(), 3);  // At least some tiles reachable

    // Verify wall tiles are NOT reachable
    for (const Math::vec2& tile : reachable) {
        ASSERT_FALSE(tile.x == 2 && (tile.y == 0 || tile.y == 1 || tile.y == 2));
    }

    std::cout << "✅ Test_GetReachableTiles_WithObstacles passed" << std::endl;
    return true;
}
```

#### **Test Suite 4: Edge Cases**

```cpp
bool Test_Pathfinding_InvalidStart() {
    // Setup
    GridSystem::Instance().Reset();

    // Action - Invalid start position
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath({-1, -1}, {4, 4});

    // Assertions
    ASSERT_EQ(path.size(), 0);  // Should return empty path

    std::cout << "✅ Test_Pathfinding_InvalidStart passed" << std::endl;
    return true;
}

bool Test_Pathfinding_InvalidGoal() {
    // Setup
    GridSystem::Instance().Reset();

    // Action - Invalid goal position
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath({4, 4}, {10, 10});

    // Assertions
    ASSERT_EQ(path.size(), 0);

    std::cout << "✅ Test_Pathfinding_InvalidGoal passed" << std::endl;
    return true;
}

bool Test_Pathfinding_UnwalkableGoal() {
    // Setup
    GridSystem::Instance().Reset();
    GridSystem::Instance().SetTileType({5, 5}, TileType::Wall);

    // Action - Goal is a wall
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath({4, 4}, {5, 5});

    // Assertions
    ASSERT_EQ(path.size(), 0);

    std::cout << "✅ Test_Pathfinding_UnwalkableGoal passed" << std::endl;
    return true;
}
```

### Daily Breakdown (Developer B)

**Monday (Day 1)**: A* data structures + basic algorithm

- Morning: Add Node struct, pathfinding method signatures
- Afternoon: Implement FindPath() basic A* logic
- **Deliverable**: A* finds straight-line paths

**Tuesday (Day 2)**: Complete A* with obstacle avoidance

- Morning: Add neighbor checking, walkable validation
- Afternoon: Test paths around obstacles
- **Deliverable**: A* navigates around walls

**Wednesday (Day 3)**: Helper methods + optimization

- Morning: Implement GetPathLength(), GetReachableTiles()
- Afternoon: Optimize A* (node pooling, early exit)
- **Deliverable**: All pathfinding methods working

**Thursday (Day 4)**: Edge case testing

- Morning: Test invalid positions, no path scenarios
- Afternoon: Test performance (large grids, complex paths)
- **Deliverable**: Robust pathfinding with error handling

**Friday (Day 5)**: Integration with Dragon/Fighter movement

- Morning: Test Dragon movement using A* (Developer A coordination)
- Afternoon: Bug fixes, Friday demo prep
- **Deliverable**: Characters move using A* pathfinding

---

## Week 2: Developer C - TurnManager Basics

**Goal**: Implement turn management system for turn-based combat

**Foundation**: EventBus from Week 1 for turn events

**Files to Create**:

```
CS230/Game/System/TurnManager.h
CS230/Game/System/TurnManager.cpp
```

### Implementation Tasks (TurnManager)

#### **Task 1: TurnManager Class Structure**

```cpp
// File: CS230/Game/System/TurnManager.h
#pragma once
#include "../Characters/Character.h"
#include "../Types/Events.h"
#include <vector>
#include <queue>

class TurnManager {
public:
    static TurnManager& Instance();

    // Turn management
    void InitializeTurnOrder(const std::vector<Character*>& characters);
    void StartNextTurn();
    void EndCurrentTurn();

    // Turn state
    Character* GetCurrentCharacter() const;
    int GetCurrentTurnNumber() const;
    int GetRoundNumber() const;
    bool IsCombatActive() const;

    // Turn order
    std::vector<Character*> GetTurnOrder() const;
    int GetCharacterTurnIndex(Character* character) const;

    // Combat state
    void StartCombat();
    void EndCombat();
    void Reset();

private:
    TurnManager() = default;
    TurnManager(const TurnManager&) = delete;
    TurnManager& operator=(const TurnManager&) = delete;

    std::vector<Character*> turnOrder;
    int currentTurnIndex;
    int turnNumber;
    int roundNumber;
    bool combatActive;

    void PublishTurnStartEvent();
    void PublishTurnEndEvent();
};
```

#### **Task 2: Turn Order Initialization**

```cpp
// File: CS230/Game/System/TurnManager.cpp
#include "TurnManager.h"
#include "../../Engine/Engine.h"
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
    EventBus::Instance().Publish(CombatEndedEvent{});
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
    EventBus::Instance().Publish(event);
}

void TurnManager::PublishTurnEndEvent() {
    Character* currentChar = GetCurrentCharacter();
    if (currentChar == nullptr) return;

    TurnEndedEvent event{currentChar, turnNumber, roundNumber};
    EventBus::Instance().Publish(event);
}
```

### Rigorous Testing (TurnManager)

#### **Test Suite 1: Turn Order Initialization**

```cpp
// File: CS230/Game/Test/Test_TurnManager.cpp
#include "../System/TurnManager.h"
#include "../Characters/Dragon.h"
#include "../Characters/Fighter.h"
#include "Week1TestMocks.h"

bool Test_TurnManager_Initialize() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({1, 1});

    std::vector<Character*> characters = {&dragon, &fighter};

    // Action
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Assertions
    std::vector<Character*> turnOrder = TurnManager::Instance().GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), 2);
    ASSERT_EQ(turnOrder[0], &dragon);
    ASSERT_EQ(turnOrder[1], &fighter);
    ASSERT_FALSE(TurnManager::Instance().IsCombatActive());

    std::cout << "✅ Test_TurnManager_Initialize passed" << std::endl;
    return true;
}

bool Test_TurnManager_Initialize_Empty() {
    // Setup
    TurnManager::Instance().Reset();
    std::vector<Character*> characters = {};

    // Action
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Assertions - Should handle gracefully
    std::vector<Character*> turnOrder = TurnManager::Instance().GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), 0);

    std::cout << "✅ Test_TurnManager_Initialize_Empty passed" << std::endl;
    return true;
}

bool Test_TurnManager_Initialize_DeadCharacters() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({1, 1});
    fighter.SetCurrentHP(0);  // Fighter is dead

    std::vector<Character*> characters = {&dragon, &fighter};

    // Action
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Assertions - Dead characters should be removed
    std::vector<Character*> turnOrder = TurnManager::Instance().GetTurnOrder();
    ASSERT_EQ(turnOrder.size(), 1);
    ASSERT_EQ(turnOrder[0], &dragon);

    std::cout << "✅ Test_TurnManager_Initialize_DeadCharacters passed" << std::endl;
    return true;
}
```

#### **Test Suite 2: Turn Progression**

```cpp
bool Test_TurnManager_StartCombat() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({1, 1});

    std::vector<Character*> characters = {&dragon, &fighter};
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Action
    TurnManager::Instance().StartCombat();

    // Assertions
    ASSERT_TRUE(TurnManager::Instance().IsCombatActive());
    ASSERT_EQ(TurnManager::Instance().GetCurrentTurnNumber(), 1);
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &dragon);

    std::cout << "✅ Test_TurnManager_StartCombat passed" << std::endl;
    return true;
}

bool Test_TurnManager_EndCurrentTurn() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({1, 1});

    std::vector<Character*> characters = {&dragon, &fighter};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &dragon);

    // Action - End Dragon's turn
    TurnManager::Instance().EndCurrentTurn();

    // Assertions - Should be Fighter's turn now
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &fighter);
    ASSERT_EQ(TurnManager::Instance().GetCurrentTurnNumber(), 2);
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);  // Still round 1

    std::cout << "✅ Test_TurnManager_EndCurrentTurn passed" << std::endl;
    return true;
}

bool Test_TurnManager_RoundProgression() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({1, 1});

    std::vector<Character*> characters = {&dragon, &fighter};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    // Action - Complete one full round
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);

    TurnManager::Instance().EndCurrentTurn();  // Dragon -> Fighter (still round 1)
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 1);

    TurnManager::Instance().EndCurrentTurn();  // Fighter -> Dragon (round 2)
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 2);
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &dragon);

    std::cout << "✅ Test_TurnManager_RoundProgression passed" << std::endl;
    return true;
}

bool Test_TurnManager_ActionPointRefresh() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});

    std::vector<Character*> characters = {&dragon};
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Consume some AP
    dragon.ConsumeActionPoints(3);
    ASSERT_EQ(dragon.GetActionPoints(), 2);  // 5 - 3 = 2

    // Action - Start combat (should refresh AP)
    TurnManager::Instance().StartCombat();

    // Assertions - AP should be refreshed
    ASSERT_EQ(dragon.GetActionPoints(), 5);  // Refreshed to max

    std::cout << "✅ Test_TurnManager_ActionPointRefresh passed" << std::endl;
    return true;
}
```

#### **Test Suite 3: Edge Cases**

```cpp
bool Test_TurnManager_SkipDeadCharacter() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter1({1, 1});
    Fighter fighter2({2, 2});

    std::vector<Character*> characters = {&dragon, &fighter1, &fighter2};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    // Kill fighter1 during Dragon's turn
    fighter1.SetCurrentHP(0);

    // Action - End Dragon's turn
    TurnManager::Instance().EndCurrentTurn();

    // Assertions - Should skip fighter1 (dead) and go to fighter2
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &fighter2);

    std::cout << "✅ Test_TurnManager_SkipDeadCharacter passed" << std::endl;
    return true;
}

bool Test_TurnManager_AllCharactersDead() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({1, 1});

    std::vector<Character*> characters = {&dragon, &fighter};
    TurnManager::Instance().InitializeTurnOrder(characters);
    TurnManager::Instance().StartCombat();

    // Kill both characters
    dragon.SetCurrentHP(0);
    fighter.SetCurrentHP(0);

    // Action - Try to end turn
    TurnManager::Instance().EndCurrentTurn();

    // Assertions - Combat should end
    ASSERT_FALSE(TurnManager::Instance().IsCombatActive());

    std::cout << "✅ Test_TurnManager_AllCharactersDead passed" << std::endl;
    return true;
}

bool Test_TurnManager_GetCharacterTurnIndex() {
    // Setup
    TurnManager::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({1, 1});

    std::vector<Character*> characters = {&dragon, &fighter};
    TurnManager::Instance().InitializeTurnOrder(characters);

    // Action
    int dragonIndex = TurnManager::Instance().GetCharacterTurnIndex(&dragon);
    int fighterIndex = TurnManager::Instance().GetCharacterTurnIndex(&fighter);

    // Assertions
    ASSERT_EQ(dragonIndex, 0);
    ASSERT_EQ(fighterIndex, 1);

    std::cout << "✅ Test_TurnManager_GetCharacterTurnIndex passed" << std::endl;
    return true;
}
```

### Daily Breakdown (Developer C)

**Monday (Day 1)**: TurnManager class structure

- Morning: Create TurnManager.h/cpp, singleton setup
- Afternoon: Implement InitializeTurnOrder(), basic data members
- **Deliverable**: TurnManager can store character list

**Tuesday (Day 2)**: Turn progression logic

- Morning: Implement StartCombat(), StartNextTurn()
- Afternoon: Implement EndCurrentTurn(), round tracking
- **Deliverable**: Turn order cycles through characters

**Wednesday (Day 3)**: Action point refresh + event integration

- Morning: Add AP refresh on turn start
- Afternoon: Integrate TurnStartedEvent, TurnEndedEvent with EventBus
- **Deliverable**: Characters refresh AP each turn, events published

**Thursday (Day 4)**: Dead character handling

- Morning: Implement skip dead characters logic
- Afternoon: Test all-dead scenario, combat end conditions
- **Deliverable**: Turn system handles character death gracefully

**Friday (Day 5)**: Integration testing

- Morning: Test with Dragon/Fighter combat (Developer A/D coordination)
- Afternoon: Bug fixes, Friday demo prep
- **Deliverable**: Full turn-based combat loop working

---

## Week 2: Developer D - Fighter Enemy Class

**Goal**: Create Fighter enemy character class

**Foundation**: Character base class from Week 1

**Files to Create**:

```
CS230/Game/Characters/Fighter.h
CS230/Game/Characters/Fighter.cpp
CS230/Assets/Fighter.spt (sprite - placeholder OK for Week 2)
```

### Implementation Tasks (Fighter)

#### **Task 1: Fighter Class Declaration**

```cpp
// File: CS230/Game/Characters/Fighter.h
#pragma once
#include "Character.h"
#include "../../Engine/Vec2.h"
#include <string>

class Fighter : public Character {
public:
    Fighter(Math::vec2 position);
    ~Fighter() override = default;

    // GameObject overrides
    GameObjectTypes Type() override { return GameObjectTypes::Fighter; }
    std::string TypeName() const override { return "Fighter"; }

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;

    // Fighter-specific stats (inherited from Character)
    // HP: 90
    // Speed: 2 tiles per turn
    // Attack: 2d6 + 3
    // Action Points: 4 per turn

    // Week 2: Basic attack method (AI comes in Week 4)
    bool BasicAttack(Character* target);

private:
    void InitializeFighterStats();
};
```

#### **Task 2: Fighter Constructor Implementation**

```cpp
// File: CS230/Game/Characters/Fighter.cpp
#include "Fighter.h"
#include "../Engine/Engine.h"
#include "../Singletons/DiceManager.h"

Fighter::Fighter(Math::vec2 position)
    : Character(position)
{
    InitializeFighterStats();

    // Set grid position
    gridPosition = position;

    // Set sprite (placeholder for Week 2)
    AddGOComponent(new CS230::Sprite("Assets/Fighter.spt", this));

    Engine::GetLogger().LogEvent("Fighter created at (" +
        std::to_string((int)position.x) + ", " +
        std::to_string((int)position.y) + ")");
}

void Fighter::InitializeFighterStats() {
    // Health
    maxHP = 90;
    currentHP = 90;

    // Movement
    speed = 2;  // Can move 2 tiles per turn (slower than Dragon)

    // Action Points
    maxActionPoints = 4;
    currentActionPoints = 4;

    // Attack
    baseAttackPower = 3;
    attackDice = "2d6";  // 2d6 + 3 damage
    attackRange = 1;     // Melee range

    // Defense
    baseDefensePower = 4;  // Higher defense than Dragon
    defenseDice = "1d8";
}

void Fighter::Update(double dt) {
    Character::Update(dt);  // Call base class Update (state machine)

    // Fighter-specific update logic (placeholder for Week 4 AI)
}

void Fighter::Draw(Math::TransformationMatrix camera_matrix) {
    Character::Draw(camera_matrix);  // Draw sprite

    // TODO Week 3: Draw HP bar, etc.
}

bool Fighter::BasicAttack(Character* target) {
    if (target == nullptr) {
        Engine::GetLogger().LogError("Fighter: Attack target is null");
        return false;
    }

    if (!target->IsAlive()) {
        Engine::GetLogger().LogError("Fighter: Cannot attack dead target");
        return false;
    }

    // Check if target is in range
    int distance = GridSystem::Instance().ManhattanDistance(gridPosition, target->GetGridPosition());
    if (distance > attackRange) {
        Engine::GetLogger().LogError("Fighter: Target out of range (distance=" +
            std::to_string(distance) + ", range=" + std::to_string(attackRange) + ")");
        return false;
    }

    // Check if we have enough AP (attack costs 2 AP)
    int attackCost = 2;
    if (currentActionPoints < attackCost) {
        Engine::GetLogger().LogError("Fighter: Not enough AP (have " +
            std::to_string(currentActionPoints) + ", need " + std::to_string(attackCost) + ")");
        return false;
    }

    // Roll attack damage (will use CombatSystem in Week 2 Developer E)
    // For now, just call target->TakeDamage() directly
    int damageDice = DiceManager::Instance().RollDiceFromString(attackDice);
    int totalDamage = damageDice + baseAttackPower;

    // Apply damage
    target->TakeDamage(totalDamage);

    // Consume AP
    ConsumeActionPoints(attackCost);

    Engine::GetLogger().LogEvent("Fighter attacked " + target->TypeName() +
        " for " + std::to_string(totalDamage) + " damage");

    // Publish attack event
    EventBus::Instance().Publish(CharacterAttackedEvent{this, target, totalDamage});

    return true;
}
```

### Rigorous Testing (Fighter)

#### **Test Suite 1: Fighter Initialization**

```cpp
// File: CS230/Game/Test/Test_Fighter.cpp
#include "../Characters/Fighter.h"
#include "Week1TestMocks.h"

bool Test_Fighter_Constructor() {
    // Setup
    Math::vec2 spawnPos{2, 2};

    // Action
    Fighter fighter(spawnPos);

    // Assertions - Stats
    ASSERT_EQ(fighter.GetMaxHP(), 90);
    ASSERT_EQ(fighter.GetCurrentHP(), 90);
    ASSERT_EQ(fighter.GetSpeed(), 2);
    ASSERT_EQ(fighter.GetActionPoints(), 4);
    ASSERT_TRUE(fighter.IsAlive());

    // Assertions - Position
    ASSERT_EQ(fighter.GetGridPosition().x, 2);
    ASSERT_EQ(fighter.GetGridPosition().y, 2);

    // Assertions - Type
    ASSERT_EQ(fighter.Type(), GameObjectTypes::Fighter);
    ASSERT_EQ(fighter.TypeName(), "Fighter");

    std::cout << "✅ Test_Fighter_Constructor passed" << std::endl;
    return true;
}

bool Test_Fighter_Attack_Dice() {
    // Setup
    Fighter fighter({0, 0});

    // Action
    std::string attackDice = fighter.GetAttackDice();

    // Assertions
    ASSERT_EQ(attackDice, "2d6");
    ASSERT_EQ(fighter.GetBaseAttackPower(), 3);

    // 2d6+3 = 5-15 damage range

    std::cout << "✅ Test_Fighter_Attack_Dice passed" << std::endl;
    return true;
}
```

#### **Test Suite 2: Fighter Combat**

```cpp
bool Test_Fighter_BasicAttack_Valid() {
    // Setup
    GridSystem::Instance().Reset();
    Fighter fighter({0, 0});
    Dragon dragon({0, 1});  // Adjacent target
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 0});
    GridSystem::Instance().PlaceCharacter(&dragon, {0, 1});

    int dragonHPBefore = dragon.GetCurrentHP();
    int fighterAPBefore = fighter.GetActionPoints();

    // Action
    bool success = fighter.BasicAttack(&dragon);

    // Assertions
    ASSERT_TRUE(success);
    ASSERT_TRUE(dragon.GetCurrentHP() < dragonHPBefore);  // Dragon took damage
    ASSERT_EQ(fighter.GetActionPoints(), fighterAPBefore - 2);  // Consumed 2 AP

    std::cout << "✅ Test_Fighter_BasicAttack_Valid passed" << std::endl;
    return true;
}

bool Test_Fighter_BasicAttack_OutOfRange() {
    // Setup
    GridSystem::Instance().Reset();
    Fighter fighter({0, 0});
    Dragon dragon({3, 3});  // Too far away
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 0});
    GridSystem::Instance().PlaceCharacter(&dragon, {3, 3});

    int dragonHPBefore = dragon.GetCurrentHP();
    int fighterAPBefore = fighter.GetActionPoints();

    // Action
    bool success = fighter.BasicAttack(&dragon);

    // Assertions - Attack should fail
    ASSERT_FALSE(success);
    ASSERT_EQ(dragon.GetCurrentHP(), dragonHPBefore);  // No damage dealt
    ASSERT_EQ(fighter.GetActionPoints(), fighterAPBefore);  // No AP consumed

    std::cout << "✅ Test_Fighter_BasicAttack_OutOfRange passed" << std::endl;
    return true;
}

bool Test_Fighter_BasicAttack_NotEnoughAP() {
    // Setup
    GridSystem::Instance().Reset();
    Fighter fighter({0, 0});
    Dragon dragon({0, 1});
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 0});
    GridSystem::Instance().PlaceCharacter(&dragon, {0, 1});

    // Consume AP so fighter doesn't have enough
    fighter.ConsumeActionPoints(3);  // 4 - 3 = 1 AP left (need 2)

    int dragonHPBefore = dragon.GetCurrentHP();

    // Action
    bool success = fighter.BasicAttack(&dragon);

    // Assertions
    ASSERT_FALSE(success);
    ASSERT_EQ(dragon.GetCurrentHP(), dragonHPBefore);

    std::cout << "✅ Test_Fighter_BasicAttack_NotEnoughAP passed" << std::endl;
    return true;
}

bool Test_Fighter_BasicAttack_DeadTarget() {
    // Setup
    GridSystem::Instance().Reset();
    Fighter fighter({0, 0});
    Dragon dragon({0, 1});
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 0});
    GridSystem::Instance().PlaceCharacter(&dragon, {0, 1});

    // Kill the dragon
    dragon.SetCurrentHP(0);

    // Action
    bool success = fighter.BasicAttack(&dragon);

    // Assertions
    ASSERT_FALSE(success);

    std::cout << "✅ Test_Fighter_BasicAttack_DeadTarget passed" << std::endl;
    return true;
}

bool Test_Fighter_BasicAttack_NullTarget() {
    // Setup
    Fighter fighter({0, 0});

    // Action
    bool success = fighter.BasicAttack(nullptr);

    // Assertions
    ASSERT_FALSE(success);

    std::cout << "✅ Test_Fighter_BasicAttack_NullTarget passed" << std::endl;
    return true;
}
```

#### **Test Suite 3: Fighter Movement**

```cpp
bool Test_Fighter_Movement_Speed() {
    // Setup
    GridSystem::Instance().Reset();
    Fighter fighter({0, 0});
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 0});

    // Action - Move 2 tiles (exactly fighter's speed)
    bool success = fighter.MoveTo({0, 2});

    // Assertions
    ASSERT_TRUE(success);
    ASSERT_EQ(fighter.GetGridPosition().x, 0);
    ASSERT_EQ(fighter.GetGridPosition().y, 2);
    ASSERT_EQ(fighter.GetActionPoints(), 2);  // 4 - 2 = 2

    std::cout << "✅ Test_Fighter_Movement_Speed passed" << std::endl;
    return true;
}

bool Test_Fighter_Movement_TooFar() {
    // Setup
    GridSystem::Instance().Reset();
    Fighter fighter({0, 0});
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 0});

    // Action - Try to move 3 tiles (speed is only 2)
    bool success = fighter.MoveTo({0, 3});

    // Assertions
    ASSERT_FALSE(success);
    ASSERT_EQ(fighter.GetGridPosition().x, 0);
    ASSERT_EQ(fighter.GetGridPosition().y, 0);

    std::cout << "✅ Test_Fighter_Movement_TooFar passed" << std::endl;
    return true;
}
```

### Daily Breakdown (Developer D)

**Monday (Day 1)**: Fighter class structure + initialization

- Morning: Create Fighter.h/cpp, implement constructor
- Afternoon: Implement InitializeFighterStats(), test initialization
- **Deliverable**: Fighter spawns with correct stats (HP=90, speed=2)

**Tuesday (Day 2)**: Combat methods

- Morning: Implement BasicAttack() method
- Afternoon: Test attack validation (range, AP, dead targets)
- **Deliverable**: Fighter can attack adjacent targets

**Wednesday (Day 3)**: Movement integration

- Morning: Add MoveTo() method (similar to Dragon)
- Afternoon: Test Fighter movement on grid
- **Deliverable**: Fighter moves on grid with speed=2

**Thursday (Day 4)**: Combat integration with DiceManager

- Morning: Integrate attack damage with DiceManager (2d6+3)
- Afternoon: Test damage output, EventBus integration
- **Deliverable**: Fighter attacks deal correct dice-based damage

**Friday (Day 5)**: Integration testing with Dragon

- Morning: Test Dragon vs Fighter combat (Developer A coordination)
- Afternoon: Bug fixes, Friday demo prep
- **Deliverable**: Dragon vs Fighter combat working

---

## Week 2: Developer E - CombatSystem Foundation

**Goal**: Create CombatSystem for centralized combat logic

**Foundation**: DiceManager from Week 1, Character from Week 1

**Files to Create**:

```
CS230/Game/System/CombatSystem.h
CS230/Game/System/CombatSystem.cpp
```

### Implementation Tasks (CombatSystem)

#### **Task 1: CombatSystem Class Structure**

```cpp
// File: CS230/Game/System/CombatSystem.h
#pragma once
#include "../Characters/Character.h"
#include <string>

enum class DamageType {
    Physical,
    Fire,
    Cold,
    Lightning,
    Poison
};

class CombatSystem {
public:
    static CombatSystem& Instance();

    // Core combat methods
    int CalculateDamage(Character* attacker, Character* defender, const std::string& damageDice, int baseDamage);
    void ApplyDamage(Character* attacker, Character* defender, int damage);

    // Attack execution
    bool ExecuteAttack(Character* attacker, Character* defender);

    // Damage calculation helpers
    int RollAttackDamage(const std::string& damageDice, int baseDamage);
    bool IsCriticalHit();  // Week 4: critical hit system

    // Range checking
    bool IsInRange(Character* attacker, Character* target, int range);
    int GetDistance(Character* char1, Character* char2);

private:
    CombatSystem() = default;
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;
};
```

#### **Task 2: Damage Calculation Implementation**

```cpp
// File: CS230/Game/System/CombatSystem.cpp
#include "CombatSystem.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../System/GridSystem.h"
#include "../../Engine/Engine.h"

CombatSystem& CombatSystem::Instance() {
    static CombatSystem instance;
    return instance;
}

int CombatSystem::CalculateDamage(Character* attacker, Character* defender,
                                    const std::string& damageDice, int baseDamage)
{
    if (attacker == nullptr || defender == nullptr) {
        Engine::GetLogger().LogError("CombatSystem: Null attacker or defender");
        return 0;
    }

    // Roll attack dice
    int diceRoll = DiceManager::Instance().RollDiceFromString(damageDice);
    int totalDamage = diceRoll + baseDamage;

    // Week 2: Simple damage calculation
    // Week 4: Add defense calculation, resistances, etc.

    Engine::GetLogger().LogEvent("CombatSystem: " + attacker->TypeName() +
        " rolled " + damageDice + " = " + std::to_string(diceRoll) +
        " + " + std::to_string(baseDamage) + " = " + std::to_string(totalDamage) + " damage");

    return totalDamage;
}

void CombatSystem::ApplyDamage(Character* attacker, Character* defender, int damage) {
    if (defender == nullptr) {
        Engine::GetLogger().LogError("CombatSystem: Null defender");
        return;
    }

    if (damage < 0) {
        Engine::GetLogger().LogError("CombatSystem: Negative damage (" + std::to_string(damage) + ")");
        damage = 0;
    }

    // Apply damage to defender
    int hpBefore = defender->GetCurrentHP();
    defender->TakeDamage(damage);
    int hpAfter = defender->GetCurrentHP();

    Engine::GetLogger().LogEvent("CombatSystem: " + defender->TypeName() +
        " took " + std::to_string(damage) + " damage (" +
        std::to_string(hpBefore) + " -> " + std::to_string(hpAfter) + " HP)");

    // Publish damage event
    EventBus::Instance().Publish(CharacterDamagedEvent{
        defender,
        damage,
        hpAfter,
        attacker,
        !defender->IsAlive()
    });

    // Check if defender died
    if (!defender->IsAlive()) {
        Engine::GetLogger().LogEvent("CombatSystem: " + defender->TypeName() + " died!");
        EventBus::Instance().Publish(CharacterDiedEvent{defender, attacker});
    }
}

bool CombatSystem::ExecuteAttack(Character* attacker, Character* defender) {
    if (attacker == nullptr || defender == nullptr) {
        Engine::GetLogger().LogError("CombatSystem: Null attacker or defender");
        return false;
    }

    if (!attacker->IsAlive()) {
        Engine::GetLogger().LogError("CombatSystem: Dead attacker cannot attack");
        return false;
    }

    if (!defender->IsAlive()) {
        Engine::GetLogger().LogError("CombatSystem: Cannot attack dead defender");
        return false;
    }

    // Check range
    if (!IsInRange(attacker, defender, attacker->GetAttackRange())) {
        Engine::GetLogger().LogError("CombatSystem: Target out of range");
        return false;
    }

    // Check AP (attack costs 2 AP)
    int attackCost = 2;
    if (attacker->GetActionPoints() < attackCost) {
        Engine::GetLogger().LogError("CombatSystem: Not enough AP");
        return false;
    }

    // Calculate and apply damage
    int damage = CalculateDamage(attacker, defender,
                                  attacker->GetAttackDice(),
                                  attacker->GetBaseAttackPower());
    ApplyDamage(attacker, defender, damage);

    // Consume AP
    attacker->ConsumeActionPoints(attackCost);

    // Publish attack event
    EventBus::Instance().Publish(CharacterAttackedEvent{attacker, defender, damage});

    return true;
}

int CombatSystem::RollAttackDamage(const std::string& damageDice, int baseDamage) {
    int diceRoll = DiceManager::Instance().RollDiceFromString(damageDice);
    return diceRoll + baseDamage;
}

bool CombatSystem::IsCriticalHit() {
    // Week 4: Implement critical hit system (1d20 >= 18)
    return false;
}

bool CombatSystem::IsInRange(Character* attacker, Character* target, int range) {
    if (attacker == nullptr || target == nullptr) return false;

    int distance = GetDistance(attacker, target);
    return distance <= range;
}

int CombatSystem::GetDistance(Character* char1, Character* char2) {
    if (char1 == nullptr || char2 == nullptr) return -1;

    return GridSystem::Instance().ManhattanDistance(
        char1->GetGridPosition(),
        char2->GetGridPosition()
    );
}
```

### Rigorous Testing (CombatSystem)

#### **Test Suite 1: Damage Calculation**

```cpp
// File: CS230/Game/Test/Test_CombatSystem.cpp
#include "../System/CombatSystem.h"
#include "../Characters/Dragon.h"
#include "../Characters/Fighter.h"
#include "Week1TestMocks.h"

bool Test_CombatSystem_CalculateDamage() {
    // Setup
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    // Action - Calculate damage (3d8 + 5)
    int damage = CombatSystem::Instance().CalculateDamage(&dragon, &fighter, "3d8", 5);

    // Assertions - 3d8 + 5 = 8-29 damage
    ASSERT_GE(damage, 8);
    ASSERT_LE(damage, 29);

    std::cout << "✅ Test_CombatSystem_CalculateDamage passed (damage=" << damage << ")" << std::endl;
    return true;
}

bool Test_CombatSystem_CalculateDamage_MinRoll() {
    // Setup
    DiceManager::Instance().SetSeed(1000);  // Fixed seed for minimum rolls
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    // Action - Roll multiple times to find minimum
    int minDamage = 999;
    for (int i = 0; i < 100; ++i) {
        int damage = CombatSystem::Instance().CalculateDamage(&dragon, &fighter, "3d8", 5);
        if (damage < minDamage) minDamage = damage;
    }

    // Assertions - Minimum should be close to 3*1 + 5 = 8
    ASSERT_GE(minDamage, 8);
    ASSERT_LE(minDamage, 10);

    std::cout << "✅ Test_CombatSystem_CalculateDamage_MinRoll passed (min=" << minDamage << ")" << std::endl;
    return true;
}

bool Test_CombatSystem_CalculateDamage_MaxRoll() {
    // Setup
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    // Action - Roll multiple times to find maximum
    int maxDamage = 0;
    for (int i = 0; i < 100; ++i) {
        int damage = CombatSystem::Instance().CalculateDamage(&dragon, &fighter, "3d8", 5);
        if (damage > maxDamage) maxDamage = damage;
    }

    // Assertions - Maximum should be close to 3*8 + 5 = 29
    ASSERT_GE(maxDamage, 25);
    ASSERT_LE(maxDamage, 29);

    std::cout << "✅ Test_CombatSystem_CalculateDamage_MaxRoll passed (max=" << maxDamage << ")" << std::endl;
    return true;
}
```

#### **Test Suite 2: Apply Damage**

```cpp
bool Test_CombatSystem_ApplyDamage() {
    // Setup
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});
    int fighterHPBefore = fighter.GetCurrentHP();

    // Action
    CombatSystem::Instance().ApplyDamage(&dragon, &fighter, 20);

    // Assertions
    ASSERT_EQ(fighter.GetCurrentHP(), fighterHPBefore - 20);
    ASSERT_TRUE(fighter.IsAlive());

    std::cout << "✅ Test_CombatSystem_ApplyDamage passed" << std::endl;
    return true;
}

bool Test_CombatSystem_ApplyDamage_Fatal() {
    // Setup
    Fighter fighter({0, 0});
    fighter.SetCurrentHP(10);

    // Action - Apply fatal damage
    CombatSystem::Instance().ApplyDamage(nullptr, &fighter, 15);

    // Assertions
    ASSERT_EQ(fighter.GetCurrentHP(), 0);
    ASSERT_FALSE(fighter.IsAlive());

    std::cout << "✅ Test_CombatSystem_ApplyDamage_Fatal passed" << std::endl;
    return true;
}

bool Test_CombatSystem_ApplyDamage_Negative() {
    // Setup
    Fighter fighter({0, 0});
    int hpBefore = fighter.GetCurrentHP();

    // Action - Try to apply negative damage (should be clamped to 0)
    CombatSystem::Instance().ApplyDamage(nullptr, &fighter, -10);

    // Assertions - HP should not change
    ASSERT_EQ(fighter.GetCurrentHP(), hpBefore);

    std::cout << "✅ Test_CombatSystem_ApplyDamage_Negative passed" << std::endl;
    return true;
}
```

#### **Test Suite 3: Execute Attack**

```cpp
bool Test_CombatSystem_ExecuteAttack_Valid() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});  // Adjacent
    GridSystem::Instance().PlaceCharacter(&dragon, {0, 0});
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 1});

    int fighterHPBefore = fighter.GetCurrentHP();
    int dragonAPBefore = dragon.GetActionPoints();

    // Action
    bool success = CombatSystem::Instance().ExecuteAttack(&dragon, &fighter);

    // Assertions
    ASSERT_TRUE(success);
    ASSERT_TRUE(fighter.GetCurrentHP() < fighterHPBefore);  // Took damage
    ASSERT_EQ(dragon.GetActionPoints(), dragonAPBefore - 2);  // Consumed 2 AP

    std::cout << "✅ Test_CombatSystem_ExecuteAttack_Valid passed" << std::endl;
    return true;
}

bool Test_CombatSystem_ExecuteAttack_OutOfRange() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({5, 5});  // Too far
    GridSystem::Instance().PlaceCharacter(&dragon, {0, 0});
    GridSystem::Instance().PlaceCharacter(&fighter, {5, 5});

    int fighterHPBefore = fighter.GetCurrentHP();

    // Action
    bool success = CombatSystem::Instance().ExecuteAttack(&dragon, &fighter);

    // Assertions
    ASSERT_FALSE(success);
    ASSERT_EQ(fighter.GetCurrentHP(), fighterHPBefore);  // No damage

    std::cout << "✅ Test_CombatSystem_ExecuteAttack_OutOfRange passed" << std::endl;
    return true;
}

bool Test_CombatSystem_ExecuteAttack_NotEnoughAP() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});
    GridSystem::Instance().PlaceCharacter(&dragon, {0, 0});
    GridSystem::Instance().PlaceCharacter(&fighter, {0, 1});

    // Drain dragon's AP
    dragon.ConsumeActionPoints(4);  // 5 - 4 = 1 AP left (need 2)

    int fighterHPBefore = fighter.GetCurrentHP();

    // Action
    bool success = CombatSystem::Instance().ExecuteAttack(&dragon, &fighter);

    // Assertions
    ASSERT_FALSE(success);
    ASSERT_EQ(fighter.GetCurrentHP(), fighterHPBefore);

    std::cout << "✅ Test_CombatSystem_ExecuteAttack_NotEnoughAP passed" << std::endl;
    return true;
}
```

#### **Test Suite 4: Range Checking**

```cpp
bool Test_CombatSystem_IsInRange_Adjacent() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    // Action
    bool inRange = CombatSystem::Instance().IsInRange(&dragon, &fighter, 1);

    // Assertions
    ASSERT_TRUE(inRange);

    std::cout << "✅ Test_CombatSystem_IsInRange_Adjacent passed" << std::endl;
    return true;
}

bool Test_CombatSystem_IsInRange_TooFar() {
    // Setup
    GridSystem::Instance().Reset();
    Dragon dragon({0, 0});
    Fighter fighter({5, 5});

    // Action
    bool inRange = CombatSystem::Instance().IsInRange(&dragon, &fighter, 1);

    // Assertions
    ASSERT_FALSE(inRange);

    std::cout << "✅ Test_CombatSystem_IsInRange_TooFar passed" << std::endl;
    return true;
}

bool Test_CombatSystem_GetDistance() {
    // Setup
    Dragon dragon({0, 0});
    Fighter fighter({3, 4});

    // Action
    int distance = CombatSystem::Instance().GetDistance(&dragon, &fighter);

    // Assertions - Manhattan distance = |0-3| + |0-4| = 7
    ASSERT_EQ(distance, 7);

    std::cout << "✅ Test_CombatSystem_GetDistance passed" << std::endl;
    return true;
}
```

### Daily Breakdown (Developer E)

**Monday (Day 1)**: CombatSystem class structure

- Morning: Create CombatSystem.h/cpp, singleton setup
- Afternoon: Implement CalculateDamage() with DiceManager integration
- **Deliverable**: Damage calculation working with dice rolls

**Tuesday (Day 2)**: Apply damage + death handling

- Morning: Implement ApplyDamage(), integrate with Character.TakeDamage()
- Afternoon: Test fatal damage, death events
- **Deliverable**: Damage application with death handling

**Wednesday (Day 3)**: Attack execution

- Morning: Implement ExecuteAttack() (range check, AP check, damage)
- Afternoon: Test complete attack flow
- **Deliverable**: Full attack execution working

**Thursday (Day 4)**: Range checking + helpers

- Morning: Implement IsInRange(), GetDistance()
- Afternoon: Test edge cases (null characters, negative damage)
- **Deliverable**: Robust combat system with validation

**Friday (Day 5)**: Integration testing

- Morning: Test with Dragon vs Fighter combat (all developers)
- Afternoon: Bug fixes, Friday demo prep
- **Deliverable**: Complete combat system ready for demo

---

## Week 2 Integration Test (Friday Afternoon)

**Goal**: Verify all Week 2 systems work together

**Test Scenario**: Dragon vs Fighter Battle

### Integration Test Script

```cpp
// File: CS230/Game/Test/Test_Week2_Integration.cpp
#include "../Characters/Dragon.h"
#include "../Characters/Fighter.h"
#include "../System/GridSystem.h"
#include "../System/TurnManager.h"
#include "../System/CombatSystem.h"
#include "Week1TestMocks.h"

bool Test_Week2_Integration_DragonVsFighter() {
    std::cout << "\n=== Week 2 Integration Test: Dragon vs Fighter ===" << std::endl;

    // Setup - Reset all systems
    GridSystem::Instance().Reset();
    TurnManager::Instance().Reset();
    EventBus::Instance().Clear();

    // Create characters
    Dragon dragon({4, 4});
    Fighter fighter({4, 6});  // 2 tiles away

    std::cout << "✅ Characters created" << std::endl;
    std::cout << "   Dragon: HP=" << dragon.GetCurrentHP() << ", Pos=(" << dragon.GetGridPosition().x << "," << dragon.GetGridPosition().y << ")" << std::endl;
    std::cout << "   Fighter: HP=" << fighter.GetCurrentHP() << ", Pos=(" << fighter.GetGridPosition().x << "," << fighter.GetGridPosition().y << ")" << std::endl;

    // Place characters on grid
    GridSystem::Instance().PlaceCharacter(&dragon, {4, 4});
    GridSystem::Instance().PlaceCharacter(&fighter, {4, 6});

    ASSERT_TRUE(GridSystem::Instance().IsOccupied({4, 4}));
    ASSERT_TRUE(GridSystem::Instance().IsOccupied({4, 6}));
    std::cout << "✅ Characters placed on grid" << std::endl;

    // Initialize turn order
    std::vector<Character*> characters = {&dragon, &fighter};
    TurnManager::Instance().InitializeTurnOrder(characters);

    ASSERT_EQ(TurnManager::Instance().GetTurnOrder().size(), 2);
    std::cout << "✅ Turn order initialized" << std::endl;

    // Start combat
    TurnManager::Instance().StartCombat();

    ASSERT_TRUE(TurnManager::Instance().IsCombatActive());
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &dragon);
    std::cout << "✅ Combat started - Dragon's turn" << std::endl;

    // Dragon's Turn - Move closer to Fighter
    std::cout << "\n--- Dragon's Turn ---" << std::endl;
    ASSERT_EQ(dragon.GetActionPoints(), 5);

    // Find path to tile adjacent to Fighter
    std::vector<Math::vec2> path = GridSystem::Instance().FindPath({4, 4}, {4, 5});
    ASSERT_GE(path.size(), 2);  // At least start and end
    std::cout << "✅ Pathfinding found route to Fighter (length=" << path.size() << ")" << std::endl;

    // Move Dragon
    bool moveSuccess = dragon.MoveTo({4, 5});
    ASSERT_TRUE(moveSuccess);
    ASSERT_EQ(dragon.GetGridPosition().x, 4);
    ASSERT_EQ(dragon.GetGridPosition().y, 5);
    ASSERT_EQ(dragon.GetActionPoints(), 4);  // 5 - 1 = 4
    std::cout << "✅ Dragon moved to (4,5), AP=" << dragon.GetActionPoints() << std::endl;

    // Dragon attacks Fighter
    int fighterHPBefore = fighter.GetCurrentHP();
    bool attackSuccess = CombatSystem::Instance().ExecuteAttack(&dragon, &fighter);
    ASSERT_TRUE(attackSuccess);
    ASSERT_TRUE(fighter.GetCurrentHP() < fighterHPBefore);
    ASSERT_EQ(dragon.GetActionPoints(), 2);  // 4 - 2 = 2
    std::cout << "✅ Dragon attacked Fighter for " << (fighterHPBefore - fighter.GetCurrentHP()) << " damage" << std::endl;
    std::cout << "   Fighter HP: " << fighterHPBefore << " -> " << fighter.GetCurrentHP() << std::endl;

    // End Dragon's turn
    TurnManager::Instance().EndCurrentTurn();
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &fighter);
    std::cout << "✅ Dragon's turn ended" << std::endl;

    // Fighter's Turn - Attack Dragon back
    std::cout << "\n--- Fighter's Turn ---" << std::endl;
    ASSERT_EQ(fighter.GetActionPoints(), 4);  // Refreshed

    int dragonHPBefore = dragon.GetCurrentHP();
    bool fighterAttackSuccess = CombatSystem::Instance().ExecuteAttack(&fighter, &dragon);
    ASSERT_TRUE(fighterAttackSuccess);
    ASSERT_TRUE(dragon.GetCurrentHP() < dragonHPBefore);
    ASSERT_EQ(fighter.GetActionPoints(), 2);  // 4 - 2 = 2
    std::cout << "✅ Fighter attacked Dragon for " << (dragonHPBefore - dragon.GetCurrentHP()) << " damage" << std::endl;
    std::cout << "   Dragon HP: " << dragonHPBefore << " -> " << dragon.GetCurrentHP() << std::endl;

    // End Fighter's turn
    TurnManager::Instance().EndCurrentTurn();
    ASSERT_EQ(TurnManager::Instance().GetCurrentCharacter(), &dragon);
    ASSERT_EQ(TurnManager::Instance().GetRoundNumber(), 2);  // Round 2 started
    std::cout << "✅ Fighter's turn ended - Round 2 started" << std::endl;

    // Verify both characters still alive
    ASSERT_TRUE(dragon.IsAlive());
    ASSERT_TRUE(fighter.IsAlive());
    std::cout << "✅ Both characters alive" << std::endl;

    std::cout << "\n=== Week 2 Integration Test PASSED ✅ ===" << std::endl;
    std::cout << "Final state:" << std::endl;
    std::cout << "  Dragon: HP=" << dragon.GetCurrentHP() << "/" << dragon.GetMaxHP() << std::endl;
    std::cout << "  Fighter: HP=" << fighter.GetCurrentHP() << "/" << fighter.GetMaxHP() << std::endl;
    std::cout << "  Turn: " << TurnManager::Instance().GetCurrentTurnNumber() << std::endl;
    std::cout << "  Round: " << TurnManager::Instance().GetRoundNumber() << std::endl;

    return true;
}
```

### Expected Demo Output (Friday 2:00 PM)

```
=== Week 2 Demo: Dragon vs Fighter ===

Initializing systems...
✅ GridSystem ready (8x8 grid)
✅ EventBus ready
✅ DiceManager ready
✅ TurnManager ready
✅ CombatSystem ready

Creating characters...
✅ Dragon created at (4,4) - HP: 250/250, Speed: 3, AP: 5
✅ Fighter created at (4,6) - HP: 90/90, Speed: 2, AP: 4

Starting combat...
✅ Turn order: Dragon, Fighter
✅ Combat started

--- Turn 1: Dragon ---
Dragon finds path to (4,5) - 1 tile
Dragon moves to (4,5) - AP: 5 -> 4
Dragon attacks Fighter!
  Rolled 3d8+5 = (6+3+7)+5 = 21 damage
  Fighter HP: 90 -> 69
Dragon ends turn - AP: 4 -> 2

--- Turn 2: Fighter ---
Fighter AP refreshed: 2 -> 4
Fighter attacks Dragon!
  Rolled 2d6+3 = (4+5)+3 = 12 damage
  Dragon HP: 250 -> 238
Fighter ends turn - AP: 4 -> 2

--- Turn 3: Dragon (Round 2) ---
Dragon AP refreshed: 2 -> 5
...

=== Demo Complete ===
All Week 2 systems working!
- ✅ Dragon and Fighter classes
- ✅ A* pathfinding
- ✅ Turn management
- ✅ Combat system
- ✅ Dice-based damage
```

---

## Week 2 Deliverable & Verification

### Deliverable Checklist

**Developer A: Dragon Class**

- [x] Dragon.h/cpp created
- [x] Dragon spawns with HP=250, speed=3, AP=5
- [x] Dragon can move using GridSystem
- [x] Dragon integrates with TurnManager
- [x] Dragon can take damage and die
- [x] All tests passing (15+ tests)

**Developer B: GridSystem Pathfinding**

- [x] A* algorithm implemented
- [x] FindPath() works around obstacles
- [x] GetPathLength() returns correct distances
- [x] GetReachableTiles() returns valid tiles
- [x] Edge cases handled (invalid positions, no path)
- [x] All tests passing (20+ tests)

**Developer C: TurnManager**

- [x] TurnManager.h/cpp created
- [x] InitializeTurnOrder() works
- [x] Turn progression cycles correctly
- [x] AP refresh on turn start
- [x] Dead character skipping
- [x] Events published (TurnStarted, TurnEnded)
- [x] All tests passing (15+ tests)

**Developer D: Fighter Class**

- [x] Fighter.h/cpp created
- [x] Fighter spawns with HP=90, speed=2, AP=4
- [x] Fighter can move using GridSystem
- [x] Fighter BasicAttack() works
- [x] Fighter integrates with CombatSystem
- [x] All tests passing (15+ tests)

**Developer E: CombatSystem**

- [x] CombatSystem.h/cpp created
- [x] CalculateDamage() uses DiceManager
- [x] ApplyDamage() works with death handling
- [x] ExecuteAttack() validates range, AP, and executes attack
- [x] IsInRange() helper works
- [x] Events published (CharacterAttacked, CharacterDamaged, CharacterDied)
- [x] All tests passing (20+ tests)

### Integration Test Checklist

- [x] Dragon and Fighter spawn on grid
- [x] Characters move using A* pathfinding
- [x] Turn order initializes and progresses
- [x] Combat damage calculated with dice
- [x] Characters can die from damage
- [x] AP refreshes each turn
- [x] Events published correctly
- [x] No crashes during combat
- [x] Friday demo ready

### Known Issues / Tech Debt

- **Week 2 Scope**:
  - No spells yet (Week 3)
  - No AI for Fighter (Week 4)
  - No debug console (postponed to Week 6)
  - Simple damage calculation (no defense/resistance yet)

### Week 3 Preview

**Next Week Focus**: All 3 Dragon Spells + Combat Integration

- Dev A: All 3 Dragon spells (Fireball, CreateWall, LavaPool)
- Dev B: Fighter combat abilities (Shield Bash)
- Dev C: SpellSystem complete
- Dev D: DataRegistry (dragon_stats.json, fighter_stats.json)
- Dev E: BattleState integration

**Goal**: Dragon vs Fighter full combat with spells by Friday Week 3!

---

## Daily Standup Template (Week 2)

```
Date: [Day of Week 2]
Time: 9:00 AM (10 minutes)

Format:
- Each dev: "Yesterday I [completed X], today I'm [working on Y], [blocked by Z]"

Example Monday:
- Dev A: "Starting Dragon class today, will create Dragon.h/cpp and implement stats"
- Dev B: "Starting A* pathfinding, will implement FindPath() algorithm"
- Dev C: "Starting TurnManager, will implement turn order initialization"
- Dev D: "Starting Fighter class, will create Fighter.h/cpp and implement stats"
- Dev E: "Starting CombatSystem, will implement damage calculation"

Example Wednesday (midweek check-in):
- Dev A: "Dragon movement working, today integrating with pathfinding. Need Dev B's GetReachableTiles() API"
- Dev B: "A* working, GetReachableTiles() ready. Will pair with Dev A after standup"
- Dev C: "Turn progression working, today adding AP refresh. No blockers"
- Dev D: "Fighter attacks working, today integrating with CombatSystem. Need Dev E's ExecuteAttack() done"
- Dev E: "ExecuteAttack() done this morning, Dev D can integrate. Working on range checking now"

Example Friday (demo day):
- All devs: "Wrapping up Week 2, integration testing at 2pm"
```

---

**End of Week 2 Plan**

**Next Steps**: Implement all tasks, run tests, pass integration test, demo Friday 2pm!
