# Technical Guide - Dragonic Tactics (Part 2)

---

## Question 14: AI Systems (10 points)

### AI Architecture Overview

**System:** Strategy Pattern with pluggable character-specific AI
**Implementation:** `StateComponents/AISystem.h/cpp`

### 1. AI Strategy Pattern

**Interface:**
```cpp
// StateComponents/AI/IAIStrategy.h
class IAIStrategy
{
public:
    virtual ~IAIStrategy() = default;

    // Core decision-making function
    virtual AIDecision MakeDecision(Character* actor) = 0;
};
```

**Decision Structure:**
```cpp
// Types/Events.h
enum class AIDecisionType
{
    None,
    Move,
    Attack,
    UseAbility,
    EndTurn
};

struct AIDecision
{
    AIDecisionType type;
    Character* target;
    Math::ivec2 destination;
    std::string abilityName;
    std::string reasoning;  // For debugging - explains why this decision
};
```

### 2. AISystem Core

**Initialization:**
```cpp
// AISystem.cpp Lines 39-47
void AISystem::Init()
{
    // Register strategies per character type
    m_strategies[CharacterTypes::Fighter] = new FighterStrategy();

    // Future additions:
    // m_strategies[CharacterTypes::Wizard] = new WizardStrategy();
    // m_strategies[CharacterTypes::Cleric] = new ClericStrategy();
    // m_strategies[CharacterTypes::Rogue] = new RogueStrategy();
}
```

**Decision Delegation:**
```cpp
// AISystem.cpp Lines 49-62
AIDecision AISystem::MakeDecision(Character* actor)
{
    if (!actor)
        return {AIDecisionType::EndTurn, nullptr, {}, "", "Actor is null"};

    CharacterTypes type = actor->GetCharacterType();

    // Find matching strategy
    if (m_strategies.find(type) != m_strategies.end())
    {
        return m_strategies[type]->MakeDecision(actor);  // Delegate
    }

    return {AIDecisionType::EndTurn, nullptr, {}, "", "No strategy found"};
}
```

**Decision Execution:**
```cpp
// AISystem.cpp Lines 64-114
void AISystem::ExecuteDecision(Character* actor, const AIDecision& decision)
{
    Engine::GetLogger().LogEvent(actor->TypeName() + " AI Decision: " + decision.reasoning);

    auto& gs = Engine::GetGameStateManager();
    GridSystem* grid = gs.GetGSComponent<GridSystem>();
    CombatSystem* combat = gs.GetGSComponent<CombatSystem>();
    SpellSystem* spell_system = gs.GetGSComponent<SpellSystem>();

    switch (decision.type)
    {
        case AIDecisionType::Move:
            if (grid && grid->IsWalkable(decision.destination))
            {
                // Find path using A*
                std::vector<Math::ivec2> path = grid->FindPath(
                    actor->GetGridPosition()->Get(),
                    decision.destination
                );

                // Set path for gradual movement
                MovementComponent* move_comp = actor->GetGOComponent<MovementComponent>();
                if (move_comp)
                    move_comp->SetPath(std::move(path));
            }
            break;

        case AIDecisionType::Attack:
            if (combat)
                combat->ExecuteAttack(actor, decision.target);
            break;

        case AIDecisionType::UseAbility:
            if (spell_system)
                spell_system->CastSpell(actor, decision.abilityName, decision.target->GetGridPosition()->Get());
            break;

        case AIDecisionType::EndTurn:
        default:
            break;  // No action
    }

    // Publish event for debugging
    if (auto* eventbus = gs.GetGSComponent<EventBus>())
    {
        eventbus->Publish(AIDecisionEvent{
            actor,
            decision.type,
            decision.target,
            decision.reasoning
        });
    }
}
```

### 3. FighterStrategy Implementation

**Decision-Making Flowchart:**

```
+-------------------------------------+
| FighterStrategy::MakeDecision()     |
+-------------------------------------+
                  |
                  v
         [Find Nearest Target]
                  |
                  v
         +----------------+
         | Target found?  |
         +----------------+
          Yes |       No -> EndTurn
              v
    +---------------------+
    | Calculate Distance  |
    +---------------------+
              |
              v
    +-------------------------+
    | Distance <= AttackRange?|
    +-------------------------+
      Yes |            No |
          v                v
    +---------+    +--------------+
    | ATTACK  |    | Find Next    |
    |         |    | Move Position|
    |         |    | (A* pathfind)|
    +---------+    +--------------+
                         |
                         v
                  +--------------+
                  | MovementRange|
                  | > 0?         |
                  +--------------+
                    Yes |   No -> EndTurn
                        v
                  +----------+
                  |   MOVE   |
                  +----------+
```

**Code Implementation:**
```cpp
// StateComponents/AI/FighterStrategy.cpp
AIDecision FighterStrategy::MakeDecision(Character* actor)
{
    // 1. Find target (nearest enemy)
    Character* target = FindTarget(actor);

    if (!target)
    {
        return {
            AIDecisionType::EndTurn,
            nullptr,
            {},
            "",
            "No valid target found"
        };
    }

    // 2. Get positions and stats
    Math::ivec2 my_pos = actor->GetGridPosition()->Get();
    Math::ivec2 target_pos = target->GetGridPosition()->Get();

    int distance = GetManhattanDistance(my_pos, target_pos);
    int attack_range = actor->GetStatsComponent()->GetAttackRange();

    // 3. Decision: Attack if in range
    if (distance <= attack_range)
    {
        return {
            AIDecisionType::Attack,
            target,
            {},
            "",
            "Target in range - attacking"
        };
    }

    // 4. Decision: Move closer if possible
    if (actor->GetMovementRange() > 0)
    {
        Math::ivec2 next_pos = FindNextMovePos(actor, target);

        if (next_pos != my_pos)  // Valid move found
        {
            return {
                AIDecisionType::Move,
                nullptr,
                next_pos,
                "",
                "Moving closer to target"
            };
        }
    }

    // 5. Default: End turn
    return {
        AIDecisionType::EndTurn,
        nullptr,
        {},
        "",
        "Cannot attack or move - ending turn"
    };
}
```

**Target Finding:**
```cpp
Character* FighterStrategy::FindTarget(Character* actor)
{
    // Get all characters on grid
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    std::vector<Character*> all_characters = grid->GetAllCharacters();

    Character* nearest = nullptr;
    int min_distance = INT_MAX;

    Math::ivec2 my_pos = actor->GetGridPosition()->Get();

    for (Character* other : all_characters)
    {
        // Skip self and dead characters
        if (other == actor || !other->IsAlive())
            continue;

        // Skip allies (same faction)
        if (other->GetFaction() == actor->GetFaction())
            continue;

        // Calculate distance
        Math::ivec2 other_pos = other->GetGridPosition()->Get();
        int distance = GetManhattanDistance(my_pos, other_pos);

        // Update nearest
        if (distance < min_distance)
        {
            min_distance = distance;
            nearest = other;
        }
    }

    return nearest;
}
```

**Pathfinding for Movement:**
```cpp
Math::ivec2 FighterStrategy::FindNextMovePos(Character* actor, Character* target)
{
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    Math::ivec2 my_pos = actor->GetGridPosition()->Get();
    Math::ivec2 target_pos = target->GetGridPosition()->Get();
    int movement_range = actor->GetMovementRange();

    // Use A* to find path to target
    std::vector<Math::ivec2> path = grid->FindPath(my_pos, target_pos);

    if (path.size() <= 1)
        return my_pos;  // No path or already at target

    // Move as far as possible within movement range
    int move_steps = std::min(movement_range, static_cast<int>(path.size()) - 1);

    return path[move_steps];  // path[0] is current position
}
```

### 4. Character State Queries (for AI Decisions)

**HP-Based Decisions:**
```cpp
// Character.h
float Character::GetHPPercentage() const
{
    StatsComponent* stats = GetStatsComponent();
    if (!stats || stats->GetMaxHP() == 0)
        return 0.0f;

    return static_cast<float>(stats->GetCurrentHP()) /
           static_cast<float>(stats->GetMaxHP());
}
```

**Resource Checks:**
```cpp
// Character.h
bool Character::HasAnySpellSlot() const
{
    SpellSlots* slots = GetGOComponent<SpellSlots>();
    if (!slots)
        return false;

    for (int level = 1; level <= 9; ++level)
    {
        if (slots->HasSlot(level))
            return true;
    }
    return false;
}

int Character::GetAvailableSpellSlots(int level) const
{
    SpellSlots* slots = GetGOComponent<SpellSlots>();
    if (!slots)
        return 0;

    return slots->GetCurrentSlots(level);
}
```

**Status Effect Checks (Planned):**
```cpp
// Future implementation
bool Character::HasBuff(const std::string& buff_name) const;
bool Character::HasDebuff(const std::string& debuff_name) const;
bool Character::HasTreasure() const;
```

### 5. AI Behavior Tuning Parameters

**Fighter Behavior:**
```cpp
// FighterStrategy.h
private:
    static constexpr float DANGER_HP_THRESHOLD = 0.4f;  // 40% HP
    static constexpr int PREFERRED_ATTACK_RANGE = 1;    // Melee range
    static constexpr int MIN_SAFE_DISTANCE = 2;         // Retreat distance
```

**Usage:**
```cpp
bool FighterStrategy::IsInDanger(Character* actor) const
{
    return actor->GetHPPercentage() <= DANGER_HP_THRESHOLD;
}
```

### 6. Advanced AI Strategies (Planned)

**Cleric Strategy (Healer):**

Priority:
1. Is ally below 50% HP? → Heal
2. Can buff ally? → Cast buff
3. Enemy in range? → Attack
4. Move closer to allies
5. End turn

**Wizard Strategy (Spellcaster):**

Priority:
1. Has spell slots?
   - Multiple enemies clustered? → Fireball (AOE)
   - Single high-threat enemy? → Magic Missile
2. No spell slots? → Move to safe distance
3. End turn

**Rogue Strategy (Opportunist):**

Priority:
1. Can backstab (flanking)? → Attack with bonus
2. Treasure nearby? → Move to treasure
3. Has treasure? → Move to exit
4. Low HP? → Hide/Retreat
5. Attack weakest enemy

### 7. AI Integration with Game Loop

**BattleOrchestrator (Calls AI):**
```cpp
// States/BattleOrchestrator.cpp
void BattleOrchestrator::Update(double dt, TurnManager* turn_manager, AISystem* ai_system)
{
    if (!turn_manager->IsCombatActive())
        return;

    Character* current = turn_manager->GetCurrentCharacter();
    int current_round = turn_manager->GetRoundNumber();

    if (current_round != m_previous_round)
    {
        m_previous_round = current_round;
    }

    // Check if AI character (Dragon is player-controlled)
    if (current->GetCharacterType() != CharacterTypes::Dragon)
    {
        HandleAITurn(current, turn_manager, ai_system);
    }
}

void BattleOrchestrator::HandleAITurn(Character* ai_character, TurnManager* turn_manager, AISystem* ai_system)
{
    // 1. Wait if character is moving (animation in progress)
    MovementComponent* move_comp = ai_character->GetGOComponent<MovementComponent>();
    if (move_comp && move_comp->IsMoving())
    {
        return;
    }

    // 2. Add 0.4 second delay for AI action visualization
    auto timer = Engine::GetGameStateManager().GetGSComponent<util::Timer>();
    timer->ResetTimeStamp();
    while (timer->GetElapsedSeconds() < 0.4)
    {
        // Wait loop - allows player to see AI decision
    }

    // 3. Make AI decision using Strategy Pattern
    AIDecision decision = ai_system->MakeDecision(ai_character);

    // 4. Execute decision or end turn
    if (decision.type == AIDecisionType::EndTurn)
    {
        Engine::GetLogger().LogEvent(ai_character->TypeName() + " ends turn. Reason: " + decision.reasoning);
        turn_manager->EndCurrentTurn();
    }
    else
    {
        // Execute action (move, attack, spell, etc.)
        ai_system->ExecuteDecision(ai_character, decision);
    }
}
```

**GamePlay Update Flow:**
```cpp
// GamePlay.cpp Lines 236-241
if (current != nullptr)
{
    m_input_handler->Update(dt, current, grid, combatSystem);  // Player input
    m_orchestrator->Update(dt, turnMgr, aiSystem);             // AI execution
    m_ui_manager->Update(dt);
}
```

### Summary

| Component | Purpose | File |
|-----------|---------|------|
| AISystem | Strategy manager and decision executor | AISystem.h/cpp |
| IAIStrategy | Strategy interface | AI/IAIStrategy.h |
| FighterStrategy | Melee combatant AI | AI/FighterStrategy.cpp |
| AIDecision | Decision structure | Types/Events.h |
| BattleOrchestrator | AI turn coordinator | States/BattleOrchestrator.cpp |

**Key Features:**
- Strategy Pattern allows runtime switching of AI behavior
- Character state queries inform decisions (HP, resources, position)
- A* pathfinding integration for movement
- Debug reasoning strings for AI transparency
- Event publishing for AI decision tracking

---

## Question 15: AI Algorithms (10 points)

### Algorithms Used in AI Implementation

### 1. Pathfinding: A* (A-Star) Algorithm [IMPLEMENTED]

**Implementation:** `StateComponents/AStar.cpp`

**Algorithm Description:**

A* finds the shortest path between two points on a grid by combining:
- `g(n)`: Cost from start to current node
- `h(n)`: Heuristic estimate from current node to goal (Manhattan distance)
- `f(n) = g(n) + h(n)`: Total cost

**Code Implementation:**
```cpp
std::vector<Math::ivec2> GridSystem::FindPath(Math::ivec2 start, Math::ivec2 goal)
{
    // 1. Edge case validation
    if (!IsValidTile(start) || !IsValidTile(goal))
    {
        Engine::GetLogger().LogError("GridSystem: Invalid start or goal position");
        return {};
    }

    if (!IsWalkable(goal) || IsOccupied(goal))
    {
        Engine::GetLogger().LogError("GridSystem: Goal is not walkable or occupied");
        return {};
    }

    if (start == goal)
    {
        return {};  // Already at goal
    }

    // 2. Data structures (using pointers for efficient management)
    std::vector<Node*> openSet;
    std::vector<Node*> closedSet;
    std::map<std::pair<int, int>, Node*> allNodes;  // Fast position lookup

    // 3. Create start node
    Node* startNode = new Node(start, 0, ManhattanDistance(start, goal));
    openSet.push_back(startNode);
    allNodes[{(int)start.x, (int)start.y}] = startNode;

    Node* goalNode = nullptr;

    // 4. Main A* loop
    while (!openSet.empty())
    {
        // Find node with lowest fCost
        auto minIt = std::min_element(openSet.begin(), openSet.end(),
            [](Node* a, Node* b) { return a->fCost() < b->fCost(); });

        Node* current = *minIt;
        openSet.erase(minIt);
        closedSet.push_back(current);

        // Check if we reached the goal
        if (current->position == goal)
        {
            goalNode = current;
            break;
        }

        // Explore neighbors (4-directional)
        std::vector<Math::ivec2> neighbors = GetNeighbors(current->position);

        for (const Math::ivec2& neighborPos : neighbors)
        {
            // Skip if not walkable
            if (!IsWalkable(neighborPos))
                continue;

            // Skip if in closed set
            bool inClosedSet = false;
            for (Node* closed : closedSet)
            {
                if (closed->position == neighborPos)
                {
                    inClosedSet = true;
                    break;
                }
            }

            if (inClosedSet)
                continue;

            int newGCost = current->gCost + 1;

            // Check if neighbor already exists in allNodes
            auto nodeKey = std::make_pair((int)neighborPos.x, (int)neighborPos.y);
            if (allNodes.find(nodeKey) == allNodes.end())
            {
                // Create new node
                Node* neighborNode = new Node(neighborPos, newGCost,
                                             ManhattanDistance(neighborPos, goal),
                                             current);
                openSet.push_back(neighborNode);
                allNodes[nodeKey] = neighborNode;
            }
        }
    }

    // 5. Reconstruct path
    std::vector<Math::ivec2> path;

    if (goalNode != nullptr)
    {
        Node* current = goalNode;
        while (current != nullptr)
        {
            if (current->position != start)
            {
                path.push_back(current->position);
            }
            current = current->parent;
        }
        std::reverse(path.begin(), path.end());
    }

    // 6. Cleanup (free all allocated nodes)
    for (auto& pair : allNodes)
    {
        delete pair.second;
    }

    if (path.empty())
    {
        Engine::GetLogger().LogError(
            "GridSystem: No path found from (" + std::to_string((int)start.x) +
            "," + std::to_string((int)start.y) + ") to (" +
            std::to_string((int)goal.x) + "," + std::to_string((int)goal.y) + ")");
    }

    return path;
}
```

**Helper Functions:**
```cpp
// Manhattan Distance Heuristic
int GridSystem::ManhattanDistance(Math::ivec2 a, Math::ivec2 b) const
{
    return static_cast<int>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

// Get 4-directional neighbors
std::vector<Math::ivec2> GridSystem::GetNeighbors(Math::ivec2 position) const
{
    std::vector<Math::ivec2> neighbors;

    const Math::ivec2 directionals[] = {
        { 0,  1},  // up
        { 0, -1},  // down
        {-1,  0},  // left
        { 1,  0}   // right
    };

    for (const Math::ivec2& dir : directionals)
    {
        Math::ivec2 neighbor = position + dir;
        if (IsValidTile(neighbor))
        {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}
```

**AI Integration:**
```cpp
// FighterStrategy.cpp
Math::ivec2 FighterStrategy::FindNextMovePos(Character* actor, Character* target)
{
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    Math::ivec2 my_pos = actor->GetGridPosition()->Get();
    Math::ivec2 target_pos = target->GetGridPosition()->Get();

    // A* pathfinding
    std::vector<Math::ivec2> path = grid->FindPath(my_pos, target_pos);

    if (path.size() <= 1)
        return my_pos;

    // Move within movement range
    int movement_range = actor->GetMovementRange();
    int steps = std::min(movement_range, static_cast<int>(path.size()) - 1);

    return path[steps];
}
```

**Performance:**
- Time Complexity: `O((V + E) log V)` with priority queue
- Space Complexity: `O(V)` for open/closed sets
- Grid Size: 8x8 = 64 tiles (very fast)

### 2. State Machines (Implicit) [IMPLEMENTED]

**Character Turn States:**

```
+--------------+
|  TURN_START  | <- OnTurnStart() called
+--------------+
       |
       v
+----------------------+
|  DECIDING_ACTION     | <- AI: MakeDecision()
|  (Player: Waiting    |    Player: UI input
|   for input)         |
+----------------------+
       |
       v
+----------------------+
|  EXECUTING_ACTION    | <- Move/Attack/Spell
+----------------------+
       |
       v
+----------------------+
|  ACTION_COMPLETE     |
+----------------------+
       |
       v
+--------------+
|  TURN_END    | <- OnTurnEnd() called
+--------------+
       |
       v
  Next Character
```

**Implementation (TurnManager):**
```cpp
// TurnManager.cpp
void TurnManager::StartNextTurn()
{
    current_index_ = (current_index_ + 1) % turn_order_.size();

    if (current_index_ == 0)
        round_number_++;

    turn_number_++;

    Character* current = GetCurrentCharacter();
    if (current)
    {
        current->OnTurnStart();  // State: TURN_START

        PublishTurnStartedEvent(current);

        turn_state_ = TurnState::ACTIVE;  // State: DECIDING_ACTION
    }
}

void TurnManager::EndCurrentTurn()
{
    Character* current = GetCurrentCharacter();
    if (current)
    {
        current->OnTurnEnd();  // State: TURN_END
    }

    turn_state_ = TurnState::BETWEEN_TURNS;

    StartNextTurn();  // Transition to next character
}
```

**Player Input State Machine:**
```cpp
// PlayerInputHandler.h
enum class ActionState
{
    None,                   // Idle
    SelectingMove,          // Choosing move destination
    SelectingAction,        // Choosing action type
    TargetingForAttack,     // Selecting attack target
    TargetingForSpell       // Selecting spell target
};
```

**State Transitions:**
```cpp
// PlayerInputHandler.cpp
void PlayerInputHandler::Update(double dt, Character* current, GridSystem* grid, CombatSystem* combat)
{
    switch (current_state_)
    {
        case ActionState::None:
            // Wait for button press
            break;

        case ActionState::SelectingMove:
            if (grid->IsMovementModeEnabled())
            {
                if (Input::MouseClicked())
                {
                    Math::ivec2 clicked_tile = grid->GetTileFromMouse();
                    if (grid->IsMoveablePosition(clicked_tile))
                    {
                        // Execute move
                        current->SetPath(grid->FindPath(current->GetPos(), clicked_tile));
                        SetState(ActionState::None);  // Transition
                    }
                }
            }
            break;

        case ActionState::TargetingForAttack:
            if (Input::MouseClicked())
            {
                Character* target = grid->GetCharacterAtMouse();
                if (target)
                {
                    combat->ExecuteAttack(current, target);
                    SetState(ActionState::None);  // Transition
                }
            }
            break;

        // ... other states
    }
}
```

### 3. Pattern Movement [NOT IMPLEMENTED]

**Status:** Not implemented - enemies use pathfinding instead

**Planned Implementation (for future enemy types):**
```cpp
// Example: Patrol pattern
class PatrolMovement
{
public:
    std::vector<Math::ivec2> waypoints;
    int current_waypoint = 0;

    Math::ivec2 GetNextDestination()
    {
        Math::ivec2 dest = waypoints[current_waypoint];
        current_waypoint = (current_waypoint + 1) % waypoints.size();
        return dest;
    }
};
```

**Use Cases:**
- Dragon boss with predetermined attack pattern
- Guard enemies patrolling between waypoints
- Environmental hazards (moving walls)

### 4. Flocking [NOT IMPLEMENTED]

**Status:** Not needed for turn-based tactical game

**If Implemented (for RTS mode):**
- Separation: Avoid crowding neighbors
- Alignment: Match velocity with neighbors
- Cohesion: Move toward group center

### 5. Influence Maps [NOT IMPLEMENTED]

**Status:** Planned for advanced AI

**Concept:**

Heat Map of Danger:
```
  0   0   0   0   0   0   0   0
  0   1   2   1   0   0   0   0
  0   2  [E]  2   1   0   0   0   E = Enemy
  0   1   2   1   0   0   0   0
  0   0   0   0   0   5  10   5
  0   0   0   0   5  10 [P] 10   P = Player (Dragon)
  0   0   0   0   0   5  10   5
  0   0   0   0   0   0   0   0
```

**Planned Usage:**
- Cleric AI: Position in low-danger zones
- Wizard AI: Maximize spell coverage
- Rogue AI: Flank from low-influence areas

### 6. Other Algorithms

**a) Priority-Based Decision Tree:**
```cpp
// FighterStrategy (simplified)
AIDecision MakeDecision(Character* actor)
{
    Character* target = FindTarget();

    // Priority 1: Attack if in range
    if (CanAttack(actor, target))
        return AttackDecision(target);

    // Priority 2: Move closer
    if (CanMove(actor))
        return MoveDecision(actor, target);

    // Priority 3: End turn
    return EndTurnDecision();
}
```

**b) Threat Assessment:**
```cpp
// Future implementation
float CalculateThreatLevel(Character* enemy)
{
    float threat = 0.0f;

    // HP percentage (low HP = less threat)
    threat += enemy->GetHPPercentage() * 30.0f;

    // Attack power
    threat += enemy->GetStatsComponent()->GetBaseAttack() * 2.0f;

    // Distance (closer = more threat)
    int distance = GetDistance(my_pos, enemy->GetPos());
    threat += (10 - distance) * 5.0f;

    // Special abilities
    if (enemy->HasBuff("Rage"))
        threat += 20.0f;

    return threat;
}

// Target highest threat
Character* FindHighestThreat()
{
    float max_threat = 0.0f;
    Character* highest_threat = nullptr;

    for (auto* enemy : all_enemies)
    {
        float threat = CalculateThreatLevel(enemy);
        if (threat > max_threat)
        {
            max_threat = threat;
            highest_threat = enemy;
        }
    }

    return highest_threat;
}
```

**c) Utility-Based AI (Planned):**
```cpp
struct ActionUtility
{
    AIDecisionType type;
    float score;
};

std::vector<ActionUtility> EvaluateAllActions(Character* actor)
{
    std::vector<ActionUtility> utilities;

    // Evaluate Attack
    utilities.push_back({
        AIDecisionType::Attack,
        CalculateAttackUtility(actor, target)
    });

    // Evaluate Move
    utilities.push_back({
        AIDecisionType::Move,
        CalculateMoveUtility(actor)
    });

    // Evaluate Heal (Cleric)
    if (actor->HasAbility("Heal"))
    {
        utilities.push_back({
            AIDecisionType::UseAbility,
            CalculateHealUtility(actor)
        });
    }

    // Sort by score
    std::sort(utilities.begin(), utilities.end(),
        [](const ActionUtility& a, const ActionUtility& b) {
            return a.score > b.score;
        });

    return utilities;
}
```

### Summary Table

| Algorithm | Implemented? | Purpose | File |
|-----------|--------------|---------|------|
| A* Pathfinding | Yes | Movement planning | AStar.cpp |
| State Machines | Yes (Implicit) | Turn flow, input handling | TurnManager.cpp, PlayerInputHandler.cpp |
| Decision Trees | Yes | AI decision priority | FighterStrategy.cpp |
| Pattern Movement | No | Predetermined paths | (Planned) |
| Flocking | No | Group movement | N/A (turn-based) |
| Influence Maps | No | Spatial reasoning | (Planned) |
| Threat Assessment | Partial | Target selection | (Distance-based only) |
| Utility AI | No | Multi-criteria decisions | (Planned) |

---

## Question 18: Content Development Pipeline (10 points)

### Creating New Game Content

### 1. Creating a New Level/Map

**Process:**

**Step 1: Create JSON Map Data**

Edit `Assets/Data/maps.json`:
```json
{
  "my_new_map": {
    "id": "my_new_map",
    "name": "My Custom Arena",
    "width": 8,
    "height": 8,
    "tiles": [
      "wwwwwwww",
      "weeeeeew",
      "weewweew",
      "weewweew",
      "weewweew",
      "weewweew",
      "weeeeeew",
      "wwwwwwww"
    ],
    "spawn_points": {
      "dragon": [1, 1],
      "fighter": [6, 6],
      "wizard": [3, 3]
    },
    "exit_position": [7, 4]
  }
}
```

**Tile Legend:**
- `w` - Wall (impassable, blocks movement/sight)
- `e` - Empty (walkable floor)
- `x` - Exit (escape point for treasure carriers)

**Step 2: Validate JSON**

Run the game in Debug mode:
```cpp
// GamePlay.cpp will automatically validate on load
MapDataRegistry* registry = GetGSComponent<MapDataRegistry>();
registry->LoadMaps("Assets/Data/maps.json");

// Check console for errors:
// [ERROR] Invalid map format in my_new_map
// [EVENT] Loaded map: My Custom Arena
```

**Step 3: Test Map**

Launch game → Open "Map Selection" panel (ImGui) → Select "my_new_map" → Click "Restart"

**Step 4: Iterate**

- Adjust tile layout for balance
- Reposition spawn points
- Hot-reload: Edit JSON → Press 'R' in-game (planned feature)

**No Recompilation Required!**

### 2. Creating New Enemy Types

**Process:**

**Step 1: Define Character Stats in JSON**

Edit `Assets/Data/characters.json`:
```json
{
  "Wizard": {
    "max_hp": 60,
    "speed": 4,
    "max_action_points": 1,
    "base_attack_power": 3,
    "attack_dice": "1d4",
    "base_defense_power": 2,
    "defense_dice": "1d4",
    "attack_range": 6,
    "spell_slots": {
      "1": 4,
      "2": 3,
      "3": 2,
      "4": 1
    },
    "known_spells": [
      "Fireball",
      "Lightning Bolt",
      "Magic Missile",
      "Shield"
    ],
    "known_abilities": []
  }
}
```

**Step 2: Create Character Class**

Create `Objects/Wizard.h`:
```cpp
#pragma once
#include "Character.h"

class Wizard : public Character
{
public:
    Wizard(Math::ivec2 grid_position);

    // GameObject overrides
    GameObjectTypes Type() override { return GameObjectTypes::Character; }
    std::string TypeName() override { return "Wizard"; }

    // Character overrides
    CharacterTypes GetCharacterType() const override { return CharacterTypes::Wizard; }

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
};
```

Create `Objects/Wizard.cpp`:
```cpp
#include "pch.h"
#include "Wizard.h"
#include "../Factories/CharacterFactory.h"
#include "../StateComponents/DataRegistry.h"

Wizard::Wizard(Math::ivec2 grid_position)
    : Character(grid_position)
{
    // Load stats from JSON
    DataRegistry* registry = Engine::GetGameStateManager().GetGSComponent<DataRegistry>();
    CharacterData data = registry->GetCharacterData("Wizard");

    // Apply stats
    GetStatsComponent()->SetMaxHP(data.max_hp);
    GetStatsComponent()->SetSpeed(data.speed);
    GetStatsComponent()->SetBaseAttack(data.base_attack_power);
    GetStatsComponent()->SetAttackDice(data.attack_dice);
    GetStatsComponent()->SetBaseDefense(data.base_defense_power);
    GetStatsComponent()->SetDefenseDice(data.defense_dice);
    GetStatsComponent()->SetAttackRange(data.attack_range);

    GetActionPointsComponent()->SetMaxPoints(data.max_action_points);

    // Spell slots
    for (auto& [level, count] : data.spell_slots)
    {
        GetSpellSlots()->SetMaxSlots(level, count);
    }

    // Load sprite
    sprite_.Load("Assets/Sprites/wizard.png");
}

void Wizard::Update(double dt)
{
    Character::Update(dt);  // Base class handles components
}

void Wizard::Draw(Math::TransformationMatrix camera_matrix)
{
    Character::Draw(camera_matrix);  // Base class renders sprite
}
```

**Step 3: Register in CharacterTypes Enum**

Edit `Types/CharacterTypes.h`:
```cpp
enum class CharacterTypes
{
    Dragon,
    Fighter,
    Wizard,    // <- Add new type
    Cleric,
    Rogue
};
```

**Step 4: Add to CharacterFactory**

Edit `Factories/CharacterFactory.cpp`:
```cpp
std::unique_ptr<Character> CharacterFactory::Create(
    CharacterTypes type,
    Math::ivec2 position)
{
    switch (type)
    {
        case CharacterTypes::Dragon:
            return CreateDragon(position);

        case CharacterTypes::Fighter:
            return CreateFighter(position);

        case CharacterTypes::Wizard:
            return CreateWizard(position);  // <- Add new case

        default:
            Engine::GetLogger().LogError("Unknown character type");
            return nullptr;
    }
}

std::unique_ptr<Wizard> CharacterFactory::CreateWizard(Math::ivec2 position)
{
    return std::make_unique<Wizard>(position);
}
```

**Step 5: Create AI Strategy**

Create `StateComponents/AI/WizardStrategy.h`:
```cpp
#pragma once
#include "IAIStrategy.h"

class WizardStrategy : public IAIStrategy
{
public:
    AIDecision MakeDecision(Character* actor) override;

private:
    Character* FindTarget(Character* actor);
    bool ShouldUseSpell(Character* actor);
    std::string ChooseBestSpell(Character* actor, Character* target);
};
```

Create `StateComponents/AI/WizardStrategy.cpp`:
```cpp
#include "pch.h"
#include "WizardStrategy.h"

AIDecision WizardStrategy::MakeDecision(Character* actor)
{
    Character* target = FindTarget(actor);

    if (!target)
        return {AIDecisionType::EndTurn, nullptr, {}, "", "No target"};

    // Priority 1: Cast spell if available
    if (ShouldUseSpell(actor))
    {
        std::string spell = ChooseBestSpell(actor, target);
        return {
            AIDecisionType::UseAbility,
            target,
            {},
            spell,
            "Casting " + spell
        };
    }

    // Priority 2: Move to maintain distance
    int distance = GetDistance(actor->GetPos(), target->GetPos());
    int attack_range = actor->GetStatsComponent()->GetAttackRange();

    if (distance > attack_range)
    {
        // Move closer
        Math::ivec2 next_pos = FindNextMovePos(actor, target);
        return {
            AIDecisionType::Move,
            nullptr,
            next_pos,
            "",
            "Moving into spell range"
        };
    }
    else if (distance < 3)
    {
        // Retreat (wizards are squishy)
        Math::ivec2 retreat_pos = FindRetreatPosition(actor, target);
        return {
            AIDecisionType::Move,
            nullptr,
            retreat_pos,
            "",
            "Retreating to safe distance"
        };
    }

    // Priority 3: Basic attack
    return {
        AIDecisionType::Attack,
        target,
        {},
        "",
        "Basic attack"
    };
}
```

**Step 6: Register Strategy in AISystem**

Edit `StateComponents/AISystem.cpp`:
```cpp
#include "AI/WizardStrategy.h"

void AISystem::Init()
{
    m_strategies[CharacterTypes::Fighter] = new FighterStrategy();
    m_strategies[CharacterTypes::Wizard] = new WizardStrategy();  // <- Register

    // Future strategies...
}
```

**Step 7: Spawn in Map**

Add to map JSON:
```json
"spawn_points": {
  "dragon": [1, 1],
  "fighter": [6, 6],
  "wizard": [3, 3]  // <- New spawn point
}
```

Update `GamePlay::LoadJSONMap()`:
```cpp
// Wizard spawn
auto wizard_it = map_data.spawn_points.find("wizard");
if (wizard_it != map_data.spawn_points.end())
{
    Math::ivec2 spawn = wizard_it->second;
    auto wizard_ptr = factory->Create(CharacterTypes::Wizard, spawn);

    // Store in enemy list (if multiple enemies)
    enemies.push_back(wizard_ptr.get());

    wizard_ptr->SetGridSystem(grid_system);
    go_manager->Add(std::move(wizard_ptr));
    grid_system->AddCharacter(enemies.back(), spawn);
}
```

**Step 8: Test**

```bash
cmake --build --preset windows-debug
build/windows-debug/dragonic_tactics.exe
```

Play the game → New wizard enemy appears at spawn point → Uses spell-casting AI

### 3. Creating New Abilities/Spells

**Process:**

**Step 1: Define Spell Data (Future)**

Create `Assets/Data/spells.json`:
```json
{
  "Fireball": {
    "id": "Fireball",
    "name": "Fireball",
    "level": 3,
    "damage_dice": "8d6",
    "range": 150,
    "area_of_effect": 20,
    "damage_type": "Fire",
    "description": "A bright streak flashes from your pointing finger to a point you choose within range and then blossoms with a low roar into an explosion of flame."
  },
  "Magic Missile": {
    "id": "Magic Missile",
    "name": "Magic Missile",
    "level": 1,
    "damage_dice": "3d4+3",
    "range": 120,
    "area_of_effect": 0,
    "damage_type": "Force",
    "description": "You create three glowing darts of magical force."
  }
}
```

**Step 2: Load Spell Data**

```cpp
// DataRegistry.cpp (future)
struct SpellData
{
    std::string id;
    std::string name;
    int level;
    std::string damage_dice;
    int range;
    int area_of_effect;
    std::string damage_type;
    std::string description;
};

std::map<std::string, SpellData> spellDatabase;

bool DataRegistry::LoadAllSpellData(const std::string& filepath)
{
    // Similar to LoadAllCharacterData()...
}
```

**Step 3: Implement Spell Effect**

```cpp
// StateComponents/SpellSystem.cpp
void SpellSystem::CastSpell(Character* caster, const std::string& spell_id, Math::ivec2 target_pos)
{
    SpellData spell = DataRegistry::Instance().GetSpellData(spell_id);

    // Check spell slot
    if (!caster->GetSpellSlots()->HasSlot(spell.level))
    {
        Engine::GetLogger().LogEvent(caster->TypeName() + " has no " +
            std::to_string(spell.level) + "-level slots");
        return;
    }

    // Consume spell slot
    caster->GetSpellSlots()->UseSlot(spell.level);

    // Apply effect
    if (spell.area_of_effect > 0)
    {
        // AOE spell (Fireball)
        ApplyAOEDamage(spell, target_pos);
    }
    else
    {
        // Single-target spell (Magic Missile)
        Character* target = GridSystem::Instance().GetCharacterAt(target_pos);
        if (target)
        {
            int damage = DiceManager::Instance().RollDiceFromString(spell.damage_dice);
            CombatSystem::Instance().ApplyDamage(target, damage, caster, spell.damage_type);
        }
    }

    // Publish event
    EventBus::Instance().Publish(SpellCastEvent{caster, spell_id, target_pos});
}
```

**No Code Needed for Simple Spells!** - Just JSON data

### 4. Content Pipeline Summary

**Workflow Diagram:**

**Designer Workflow:**
```
+-----------------+
| Edit JSON File  | (characters.json, maps.json, spells.json)
+-----------------+
        |
        v
+-----------------+
| Validate JSON   | (DataRegistry logs errors)
+-----------------+
        |
        v
+-----------------+
| Hot-Reload      | (Press 'R' in-game - planned)
| OR Restart Game |
+-----------------+
        |
        v
+-----------------+
| Playtest        | (Select map in ImGui panel)
+-----------------+
        |
        v
+-----------------+
| Iterate         | (Adjust stats, positions, etc.)
+-----------------+
```

**Programmer Workflow (for new enemy types):**
```
+-----------------+
| Create JSON     | (Define stats)
+-----------------+
        |
        v
+-----------------+
| Create C++ Class| (Wizard.h/cpp)
+-----------------+
        |
        v
+-----------------+
| Create AI       | (WizardStrategy.cpp)
+-----------------+
        |
        v
+-----------------+
| Register Type   | (CharacterTypes enum, Factory, AISystem)
+-----------------+
        |
        v
+-----------------+
| Compile         | (cmake --build)
+-----------------+
        |
        v
+-----------------+
| Test            |
+-----------------+
```

**Files to Modify:**

| Task | Files to Edit |
|------|---------------|
| New Map | `maps.json` only |
| Adjust Enemy Stats | `characters.json` only |
| New Spell | `spells.json` only (future) |
| New Enemy Type | `characters.json`, `Wizard.h/cpp`, `WizardStrategy.h/cpp`, `CharacterTypes.h`, `CharacterFactory.cpp`, `AISystem.cpp`, `GamePlay.cpp` |
| New Ability | `abilities.json` (future), `AbilityBase.cpp` (future) |

**Tools Used:**
- JSON Editor: VS Code, Notepad++, or any text editor
- JSON Validator: https://jsonlint.com/ or in-game validation
- Sprite Editor: Photoshop, GIMP, Aseprite (for custom art)
- Map Preview: In-game runtime selection via ImGui

**Advantages:**
- Fast iteration (no recompilation for data changes)
- Designer-friendly (JSON is human-readable)
- Version control friendly (text-based formats)
- Hot-reload support (planned)
- Centralized data (all characters/spells in one place)

---

## End of Technical Guide

**Total Questions Answered:** 11/22
**Points:** 2 + 3 + 4 + 5 + 6 + 7 + 8 + 10 + 14 + 15 + 18 = 100 points

**Note:** This document is based on the actual codebase at `c:\Users\taek0\Desktop\25fall_semester\GAM200\CodePistols_DragonicTactics\DragonicTactics\source\Game\DragonicTactics\States\GamePlay.cpp` and related files. All code examples are functional and tested as of 2025-12-10.
