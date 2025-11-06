## 3. GameState Components - Battle Systems

**REASONING: GSComponent Pattern from CS230 Engine**

- Mode1.cpp line 42-51 shows GSComponent usage pattern
- GSComponents are battle-wide systems attached to GameState
- Accessed via `GetGSComponent<Type>()`
- Examples: Camera, GameObjectManager, Timer, ParticleManager

### 3.1 BattleManager GSComponent

**Purpose:** Overall battle coordination and victory conditions
**Reasoning from dragonic_tactics.md:**

- Line 55-59: Battle ends when all enemies dead OR dragon dies OR treasure stolen

```cpp
// BattleManager.h - GSComponent for battle coordination
// REASONING: Centralized battle flow control and win/loss checking

#pragma once
#include "../Engine/Component.h"
#include "Character.h"

enum class BattleResult {
    Victory,    // All enemies defeated
    Defeat,     // Dragon died
    Escaped     // Enemies stole treasure and escaped
};

class BattleManager : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Check victory conditions every frame
    // REASONING: Line 55-59 - Battle ends on certain conditions
    // IMPLEMENTATION:
    //   1. Check if dragon dead -> Defeat
    //   2. Check if all enemies dead -> Victory
    //   3. Check if treasure stolen and enemy at spawn -> Escaped

    // ===== Battle Management =====

    void StartBattle(const std::vector<Character*>& allCombatants);
    // Initialize battle with all characters
    // REASONING: Line 46 - "스테이지 시작 : 드래곤과 플레이어를 정해진 위치에 배치"
    // IMPLEMENTATION:
    //   1. Store references to all characters
    //   2. Initialize turn order via TurnManager
    //   3. Publish BattleStartedEvent

    void CheckVictoryConditions();
    // Evaluate if battle should end
    // REASONING: Line 55-59 - Multiple win/loss conditions
    // IMPLEMENTATION:
    //   if (dragon->IsAlive() == false) -> EndBattle(Defeat)
    //   else if (AllEnemiesDead()) -> EndBattle(Victory)
    //   else if (TreasureStolen() && EnemyEscaped()) -> EndBattle(Escaped)

    void EndBattle(BattleResult result);
    // Terminate battle and transition to results screen
    // REASONING: Clean shutdown, show results, save progress
    // IMPLEMENTATION:
    //   1. Stop turn manager
    //   2. Publish BattleEndedEvent
    //   3. Show victory/defeat screen
    //   4. Save campaign progress (if victory)

    // ===== Query Methods =====

    bool IsPlayerTurn() const;
    // Check if dragon is currently active
    // REASONING: UI needs to know if player input enabled
    // USAGE: if (battleMgr->IsPlayerTurn()) { HighlightValidActions(); }

    bool AreAllEnemiesDefeated() const;
    // Check if all AI characters dead
    // REASONING: Victory condition check
    // IMPLEMENTATION: return std::all_of(enemies, [](c) { return !c->IsAlive(); });

    bool IsPlayerDefeated() const;
    // Check if dragon is dead
    // REASONING: Defeat condition check
    // IMPLEMENTATION: return !dragon->IsAlive();

    std::vector<Character*> GetAllCombatants() const { return allCharacters; }
    std::vector<Character*> GetEnemies() const;
    Character* GetPlayerDragon() const { return dragon; }

private:
    std::vector<Character*> allCharacters;  // All battle participants
    Character* dragon;                       // Player character
    BattleResult finalResult;
    bool battleActive;
};
```

**USAGE EXAMPLE:**

```cpp
// In BattleState.cpp - Set up battle
void BattleState::Load() {
    AddGSComponent(new BattleManager());
    AddGSComponent(new TurnManager());
    AddGSComponent(new GridSystem());
    AddGSComponent(new CombatSystem());

    // Create characters
    Dragon* dragon = new Dragon({4, 4});  // Center of 8x8 grid
    Fighter* fighter = new Fighter({0, 0});
    Cleric* cleric = new Cleric({0, 7});
    Wizard* wizard = new Wizard({7, 7});
    Rogue* rogue = new Rogue({7, 0});

    std::vector<Character*> combatants = {dragon, fighter, cleric, wizard, rogue};

    // Initialize battle
    GetGSComponent<BattleManager>()->StartBattle(combatants);
}

// In BattleState.cpp - Check victory each frame
void BattleState::Update(double dt) {
    UpdateGSComponents(dt);  // Calls BattleManager::Update()

    // BattleManager automatically checks victory conditions
}

// In BattleManager.cpp - Victory condition checking
void BattleManager::Update(double dt) {
    if (!battleActive) return;

    CheckVictoryConditions();
}

void BattleManager::CheckVictoryConditions() {
    // Defeat: Dragon died
    if (!dragon->IsAlive()) {
        EndBattle(BattleResult::Defeat);
        return;
    }

    // Victory: All enemies defeated
    if (AreAllEnemiesDefeated()) {
        EndBattle(BattleResult::Victory);
        return;
    }

    // Escape: Treasure stolen (future feature)
    // if (TreasureStolen() && EnemyReachedSpawn()) {
    //     EndBattle(BattleResult::Escaped);
    // }
}
```

### 3.2 TurnManager GSComponent

**Purpose:** Turn order management and action point allocation
**Reasoning from dragonic_tactics.md:**

- Line 48-53: Turn system with phases (start, character turn, end)
- Line 21-22, 49: Speed determines turn order

```cpp
// TurnManager.h - GSComponent for turn-based combat flow
// REASONING: Manage turn order and action point economy

#pragma once
#include "../Engine/Component.h"
#include "Character.h"
#include <vector>

class TurnManager : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Process current character's turn
    // REASONING: Line 50-52 - Character turn with action execution
    // IMPLEMENTATION:
    //   1. If player turn: Wait for input
    //   2. If AI turn: Execute AI decision
    //   3. Check if turn complete (no action points remaining)
    //   4. If complete: AdvanceToNextTurn()

    // ===== Turn Management =====

    void InitializeTurnOrder(const std::vector<Character*>& allCharacters);
    // Sort characters by speed for initiative order
    // REASONING: Line 21-22, 49 - "속력은 게임이 로드 될 때와 속력이 바뀐 캐릭터가 있을 때 마다 비교되어서 턴의 시작 순서를 결정한다"
    // IMPLEMENTATION:
    //   1. Copy character list
    //   2. Sort descending by speed: [Rogue(4), Dragon(5), Fighter(3), Cleric(2), Wizard(1)]
    //   3. Set currentTurnIndex = 0

    void AdvanceToNextTurn();
    // Move to next character in turn order
    // REASONING: Line 52 - "모든 행동력을 사용하거나 대기를 선택하면 턴 종료"
    // IMPLEMENTATION:
    //   1. Call current character's state_acting.CheckExit()
    //   2. Increment currentTurnIndex (wrap to 0 if end)
    //   3. Refresh action points for new character
    //   4. Publish TurnStartedEvent
    //   5. If completed full round, process end-of-round effects

    void SkipCurrentTurn();
    // Force current character to end turn early
    // REASONING: Line 52 - "대기를 선택하면 턴 종료" - Player can end turn without using all actions
    // USAGE: Player clicks "End Turn" button

    // ===== Query Methods =====

    Character* GetCurrentCharacter() const;
    // Returns character whose turn it is
    // REASONING: UI highlights active character, AI makes decisions
    // USAGE: auto* current = turnMgr->GetCurrentCharacter();

    int GetCurrentTurnIndex() const { return currentTurnIndex; }
    int GetRoundNumber() const { return roundNumber; }

    std::vector<Character*> GetTurnOrder() const { return turnOrder; }
    // Returns full initiative order
    // REASONING: UI can display turn order list

    bool IsPlayerTurn() const;
    // Check if dragon is current character
    // REASONING: Enable/disable player input
    // IMPLEMENTATION: return GetCurrentCharacter()->Type() == GameObjectTypes::Dragon;

    // ===== Action Point Management =====

    void RefreshActionPoints(Character* character);
    // Reset character's action points to max at turn start
    // REASONING: Line 27-28 - "매 턴이 시작될 때마다 행동력은 회복되며, 남은 행동력은 이월되지 않는다"
    // IMPLEMENTATION: character->currentActionPoints = character->maxActionPoints;

    void ConsumeActionPoints(Character* character, int cost);
    // Deduct action points when action performed
    // REASONING: Line 27 - "공격, 주문 사용에는 행동이 소모 된다"
    // IMPLEMENTATION: character->currentActionPoints -= cost;

private:
    std::vector<Character*> turnOrder;  // Sorted by speed (descending)
    int currentTurnIndex;               // Index into turnOrder
    int roundNumber;                    // How many complete rounds elapsed
};
```

**USAGE EXAMPLE:**

```cpp
// In BattleManager.cpp - Initialize turn order at battle start
void BattleManager::StartBattle(const std::vector<Character*>& allCombatants) {
    allCharacters = allCombatants;

    // Find dragon
    for (auto* c : allCharacters) {
        if (c->Type() == GameObjectTypes::Dragon) {
            dragon = static_cast<Dragon*>(c);
            break;
        }
    }

    // Initialize turn order by speed
    GetGSComponent<TurnManager>()->InitializeTurnOrder(allCharacters);

    battleActive = true;
    EventBus::Instance().Publish(BattleStartedEvent{allCharacters});
}

// In TurnManager.cpp - Sort by speed
void TurnManager::InitializeTurnOrder(const std::vector<Character*>& allCharacters) {
    turnOrder = allCharacters;

    // Sort descending by speed
    std::sort(turnOrder.begin(), turnOrder.end(),
        [](Character* a, Character* b) {
            return a->GetSpeed() > b->GetSpeed();
        }
    );

    // Example result: [Dragon(5), Rogue(4), Fighter(3), Cleric(2), Wizard(1)]

    currentTurnIndex = 0;
    roundNumber = 1;

    // Start first character's turn
    RefreshActionPoints(turnOrder[0]);
    turnOrder[0]->change_state(&turnOrder[0]->state_acting);

    EventBus::Instance().Publish(TurnStartedEvent{turnOrder[0], roundNumber});
}

// In BattleState.cpp - Player ends turn
void BattleState::Update(double dt) {
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Space)) {
        if (GetGSComponent<TurnManager>()->IsPlayerTurn()) {
            GetGSComponent<TurnManager>()->SkipCurrentTurn();
        }
    }
}

// In TurnManager.cpp - Advance turn
void TurnManager::AdvanceToNextTurn() {
    // End current character's turn
    Character* current = GetCurrentCharacter();
    current->change_state(&current->state_waiting_turn);

    EventBus::Instance().Publish(TurnEndedEvent{current, current->GetActionPoints()});

    // Move to next character
    currentTurnIndex++;
    if (currentTurnIndex >= turnOrder.size()) {
        currentTurnIndex = 0;
        roundNumber++;
        ProcessEndOfRoundEffects();  // Apply burn damage, etc.
    }

    // Start next character's turn
    Character* next = GetCurrentCharacter();
    RefreshActionPoints(next);
    next->change_state(&next->state_acting);

    EventBus::Instance().Publish(TurnStartedEvent{next, roundNumber});
}
```

---

### 3.3 GridSystem GSComponent

**Purpose:** 8x8 battlefield tile management and pathfinding
**Reasoning from dragonic_tactics.md:**

- Entire game takes place on 8x8 grid
- Line 21-22: Speed determines movement range in tiles
- Line 38-40: Attack range measured in tiles

```cpp
// GridSystem.h - GSComponent for grid-based battlefield
// REASONING: All tactical positioning uses discrete tile coordinates

#pragma once
#include "../Engine/Component.h"
#include "../Engine/Vec2.h"
#include "Character.h"
#include <vector>

enum class TileType {
    Empty,      // Normal passable tile
    Wall,       // Impassable obstacle (from Dragon's wall spell, line 100)
    Lava,       // Damaging terrain (from Dragon's lava spell, line 102)
    Corpse      // Dead character tile (passable but occupied)
};

class GridSystem : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Update tile states, lava durations, etc.
    // REASONING: Line 102 - Lava lasts 5 turns, need to track duration
    // IMPLEMENTATION: Decrement lava timers, remove expired tiles

    // ===== Grid Initialization =====

    GridSystem();
    // Initialize 8x8 empty grid
    // REASONING: Battlefield is 8x8 grid
    // IMPLEMENTATION: Create 8x8 array of Empty tiles

    // ===== Movement Validation =====

    bool IsValidMove(Math::vec2 from, Math::vec2 to) const;
    // Check if movement from->to is legal
    // REASONING: Must validate pathfinding and obstacles
    // IMPLEMENTATION:
    //   1. Check if 'to' in grid bounds (0-7, 0-7)
    //   2. Check if 'to' tile is passable (not Wall, not Occupied)
    //   3. Check if path exists (A* pathfinding)
    //   4. Check if distance <= character speed
    // USAGE: if (grid->IsValidMove(currentPos, targetPos)) { Move(); }

    std::vector<Math::vec2> GetReachableTiles(Character* character) const;
    // Get all tiles character can move to this turn
    // REASONING: UI highlights valid movement options
    // IMPLEMENTATION:
    //   1. Run flood-fill from character position
    //   2. Stop at distance = character->GetMovementRange()
    //   3. Skip impassable tiles (walls, occupied)
    //   4. Return list of valid destinations
    // USAGE: auto tiles = grid->GetReachableTiles(dragon);  // Highlight for player

    std::vector<Math::vec2> FindPath(Math::vec2 from, Math::vec2 to) const;
    // Calculate A* path between two points
    // REASONING: AI needs to plan multi-step movement
    // IMPLEMENTATION: A* algorithm with Manhattan distance heuristic
    // USAGE: auto path = grid->FindPath(rogue->pos, dragonFlankPos);

    // ===== Line of Sight =====

    bool HasLineOfSight(Math::vec2 from, Math::vec2 to) const;
    // Check if clear line of sight exists between tiles
    // REASONING: Spells and ranged attacks may require LOS
    // IMPLEMENTATION: Bresenham's line algorithm, check for wall tiles
    // USAGE: if (grid->HasLineOfSight(wizard->pos, dragon->pos)) { Attack(); }

    std::vector<Character*> GetCharactersInRange(Math::vec2 origin, int range) const;
    // Find all characters within 'range' tiles of origin
    // REASONING: Line 38-40 - Attack range determines valid targets
    // IMPLEMENTATION:
    //   1. Calculate Manhattan distance to all characters
    //   2. Return those with distance <= range
    // USAGE: auto targets = grid->GetCharactersInRange(dragonPos, 3);  // Dragon attack range

    // ===== Tile State Management =====

    TileType GetTileType(Math::vec2 position) const;
    // Get tile type at position
    // REASONING: Check if tile is passable, damaging, etc.
    // USAGE: if (GetTileType(pos) == TileType::Lava) { ApplyBurnDamage(); }

    void SetTileType(Math::vec2 position, TileType type);
    // Change tile type (for spells like wall creation)
    // REASONING: Line 100 - Dragon can create wall tiles
    // USAGE: grid->SetTileType({3, 4}, TileType::Wall);

    bool IsOccupied(Math::vec2 position) const;
    // Check if character is standing on tile
    // REASONING: Two characters can't occupy same tile
    // IMPLEMENTATION: Check if any character's gridPosition == position
    // USAGE: if (!grid->IsOccupied(targetPos)) { MoveHere(); }

    Character* GetCharacterAt(Math::vec2 position) const;
    // Get character at specific tile (null if none)
    // REASONING: Check what character player is targeting
    // USAGE: auto* target = grid->GetCharacterAt(mouseClickPos);

    // ===== Spell Area Effects =====

    std::vector<Math::vec2> GetTilesInArea(Math::vec2 center, int size, bool isSquare) const;
    // Get all tiles in area effect
    // REASONING: Line 102 - Lava creates 2x2 area, need to calculate affected tiles
    // IMPLEMENTATION:
    //   If square: Return size x size tiles around center
    //   If circle: Return tiles within radius (Manhattan distance)
    // USAGE: auto tiles = GetTilesInArea(center, 2, true);  // 2x2 lava pool

    std::vector<Math::vec2> GetTilesInLine(Math::vec2 from, Math::vec2 direction, int maxRange) const;
    // Get all tiles in straight line
    // REASONING: Line 105 - Dragon Wrath hits "first enemy in line"
    // IMPLEMENTATION: Raycast in direction until hit character or wall
    // USAGE: auto tiles = GetTilesInLine(dragonPos, North, 4);

    // ===== Grid-World Coordinate Conversion =====

    Math::vec2 GridToWorldPosition(Math::vec2 gridPos) const;
    // Convert grid coordinates (0-7) to world pixels
    // REASONING: Characters need world positions for rendering
    // IMPLEMENTATION: return gridPos * TILE_SIZE + GRID_OFFSET;
    // USAGE: sprite->SetPosition(GridToWorldPosition(gridPos));

    Math::vec2 WorldToGridPosition(Math::vec2 worldPos) const;
    // Convert world pixels to grid coordinates
    // REASONING: Mouse clicks need to map to tiles
    // IMPLEMENTATION: return (worldPos - GRID_OFFSET) / TILE_SIZE;
    // USAGE: auto clickedTile = WorldToGridPosition(mousePos);

private:
    static constexpr int GRID_SIZE = 8;
    static constexpr int TILE_SIZE = 64;  // Pixels per tile

    TileType grid[GRID_SIZE][GRID_SIZE];  // 8x8 tile array
    std::map<Math::vec2, int> lavaDurations;  // Lava tile timers (line 102: 5 turns)
    std::vector<Character*> allCharacters;    // Cache for fast lookups

    // Pathfinding helpers
    int ManhattanDistance(Math::vec2 a, Math::vec2 b) const;
    std::vector<Math::vec2> GetNeighbors(Math::vec2 pos) const;
};
```

**USAGE EXAMPLE:**

```cpp
// In BattleState.cpp - Highlight valid moves when dragon selected
void BattleState::OnDragonSelected() {
    Dragon* dragon = GetPlayerDragon();
    GridSystem* grid = GetGSComponent<GridSystem>();

    // Get all tiles dragon can move to (speed = 5)
    auto reachableTiles = grid->GetReachableTiles(dragon);

    // Highlight tiles for player
    for (auto tile : reachableTiles) {
        HighlightTile(tile, Color::Green);
    }
}

// In Dragon.cpp - Cast wall spell
void Dragon::Spell_CreateWall(Math::vec2 target) {
    // Line 100: Create 2x1 wall within 5 tiles
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    // Check range
    int distance = grid->ManhattanDistance(GetGridPosition(), target);
    if (distance > 5) {
        Engine::GetLogger().LogError("Wall out of range!");
        return;
    }

    // Create 2x1 wall
    grid->SetTileType(target, TileType::Wall);
    grid->SetTileType(target + Math::vec2{1, 0}, TileType::Wall);

    // Consume spell slot and action point
    ConsumeSpellSlot(1);
    currentActionPoints--;

    EventBus::Instance().Publish(SpellCastEvent{this, SpellType::CreateWall, target, 1});
}

// In Fighter AI - Find path to dragon
void Fighter::UpdateAI(double dt) {
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    Dragon* dragon = FindDragon();

    // Calculate path to dragon
    auto path = grid->FindPath(GetGridPosition(), dragon->GetGridPosition());

    if (path.size() <= GetMovementRange()) {
        // Can reach dragon this turn - move and attack
        MovePath(path);
        if (grid->GetCharactersInRange(GetGridPosition(), 1).size() > 0) {
            Attack(dragon);
        }
    } else {
        // Move as close as possible
        Math::vec2 moveTarget = path[GetMovementRange()];
        SetGridPosition(moveTarget);
    }
}
```

### 3.4 CombatSystem GSComponent

**Purpose:** Damage calculation, attack resolution, combat actions
**Reasoning from dragonic_tactics.md:**

- Line 23-24: Attack = Base + Dice Roll (e.g., 5 + 2d6)
- Line 25-26: Damage = max(0, Attack - Defense)
- Line 90-94: Combat actions consume action points

```cpp
// CombatSystem.h - GSComponent for combat resolution
// REASONING: Centralized damage calculation with dice mechanics

#pragma once
#include "../Engine/Component.h"
#include "Character.h"

enum class AttackType {
    Melee,      // Close-range physical attack (line 90)
    Ranged,     // Long-range attack (Wizard line 227)
    Spell,      // Magical damage with spell effects
    True        // Bypass defense (special effects)
};

class CombatSystem : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Process queued combat actions
    // REASONING: Some attacks may be delayed (projectile travel time)
    // IMPLEMENTATION: Update attack animations, apply damage when complete

    // ===== Combat Resolution =====

    void ExecuteAttack(Character* attacker, Character* target, AttackType type);
    // Resolve complete attack action
    // REASONING: Line 90-91 - "행동을 1 소모해서 공격력 만큼의 피해를 범위 내 대상에게 줌"
    // IMPLEMENTATION:
    //   1. Validate attack (range, action points, line of sight)
    //   2. Calculate attack power (base + dice roll)
    //   3. Calculate defense power (base + dice roll)
    //   4. Calculate final damage (attack - defense, minimum 0)
    //   5. Apply damage to target
    //   6. Consume attacker's action point
    //   7. Publish DamageTakenEvent
    //   8. Check if target died
    // USAGE: combatSys->ExecuteAttack(fighter, dragon, AttackType::Melee);

    int CalculateDamage(Character* attacker, Character* target, AttackType type);
    // Calculate damage before applying
    // REASONING: Line 23-26 - Attack/Defense dice formula
    // IMPLEMENTATION:
    //   1. Roll attacker's attack dice: baseAttack + DiceManager::Roll(attackDice)
    //   2. Roll target's defense dice: baseDefense + DiceManager::Roll(defenseDice)
    //   3. finalDamage = max(0, attackRoll - defenseRoll)
    //   4. Apply type modifiers (spells may ignore defense)
    // FORMULA EXAMPLE (Fighter vs Dragon):
    //   Fighter: 5 + 2d6 = 5 + 7 = 12 attack
    //   Dragon:  0 + 2d8 = 0 + 9 = 9 defense
    //   Damage:  max(0, 12 - 9) = 3 HP damage
    // USAGE: int damage = combatSys->CalculateDamage(attacker, target, AttackType::Melee);

    void ApplyDamage(Character* target, int damage);
    // Reduce target HP by damage amount
    // REASONING: Line 25-26 - "체력에서 뺌"
    // IMPLEMENTATION:
    //   1. target->TakeDamage(damage)
    //   2. Publish DamageTakenEvent
    //   3. If target->IsAlive() == false, trigger death sequence
    // USAGE: combatSys->ApplyDamage(dragon, 15);

    // ===== Attack Validation =====

    bool CanAttack(Character* attacker, Character* target) const;
    // Check if attack is valid
    // REASONING: Must validate range, action points, obstacles
    // IMPLEMENTATION:
    //   1. Check attacker has action points (line 27-28)
    //   2. Check target in range (line 38-40)
    //   3. Check line of sight (walls block attacks)
    //   4. Check target is alive
    // USAGE: if (combatSys->CanAttack(fighter, dragon)) { ShowAttackOption(); }

    int GetAttackRange(Character* attacker) const;
    // Get attacker's range in tiles
    // REASONING: Line 38-40, 141, 170, 201, 227, 254 - Each character has attack range
    // IMPLEMENTATION: return attacker->attackRange;
    // EXAMPLES:
    //   Dragon: 3 tiles (line 141)
    //   Fighter: 1 tile (line 170)
    //   Cleric: 2 tiles (line 201 - heal range)
    //   Wizard: 6 tiles (line 227)
    //   Rogue: 1 tile (line 254)

    // ===== Special Combat Mechanics =====

    void ApplyCriticalHit(Character* attacker, Character* target);
    // Deal extra damage on critical hit
    // REASONING: Add tactical variance beyond dice rolls
    // IMPLEMENTATION: Damage *= 1.5, play special effect
    // USAGE: if (RollCritChance()) { ApplyCriticalHit(rogue, dragon); }

    void ApplyKnockback(Character* target, Math::vec2 direction, int distance);
    // Push target away from attacker
    // REASONING: Line 104 - "1d8의 피해 및 2칸 밀쳐내기" (Dragon Tail Swipe)
    // IMPLEMENTATION:
    //   1. Calculate destination: target->pos + direction * distance
    //   2. Check if path clear (walls stop knockback)
    //   3. Move target to final position
    //   4. If knocked into wall/lava, apply collision damage
    // USAGE: ApplyKnockback(fighter, Math::vec2{1, 0}, 2);  // Push 2 tiles right

    int CalculateHealAmount(Character* healer) const;
    // Calculate healing power for Cleric heal
    // REASONING: Line 201 - "공격력만큼 회복" (Heal equals attack power)
    // IMPLEMENTATION:
    //   return healer->baseAttackPower + DiceManager::Roll(healer->attackDice);
    // USAGE: int healing = CalculateHealAmount(cleric);  // e.g., 5 + 1d6 = 8 HP

private:
    struct QueuedAttack {
        Character* attacker;
        Character* target;
        AttackType type;
        double timeRemaining;  // Animation delay
    };
    std::vector<QueuedAttack> attackQueue;
};
```

**USAGE EXAMPLE:**

```cpp
// In Character.cpp - Execute basic melee attack
void Character::PerformAction(ActionType action) {
    CombatSystem* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    switch (action) {
        case ActionType::MeleeAttack: {
            // Find target in melee range
            auto targets = grid->GetCharactersInRange(GetGridPosition(), attackRange);
            if (targets.empty()) {
                Engine::GetLogger().LogError("No targets in range!");
                return;
            }

            Character* target = targets[0];  // Attack first target

            // Validate and execute
            if (combat->CanAttack(this, target)) {
                combat->ExecuteAttack(this, target, AttackType::Melee);

                // Update facing direction (line 34-36)
                Math::vec2 direction = target->GetGridPosition() - GetGridPosition();
                SetFacingDirection(VectorToDirection(direction));
            }
            break;
        }
        // Other actions...
    }
}

// In CombatSystem.cpp - Damage calculation with dice
int CombatSystem::CalculateDamage(Character* attacker, Character* target, AttackType type) {
    // Roll attack dice
    int attackRoll = attacker->GetAttackPower();  // This calls DiceManager internally
    Engine::GetLogger().LogDebug(
        attacker->TypeName() + " attacks for " + std::to_string(attackRoll)
    );

    // Roll defense dice (unless true damage)
    int defenseRoll = 0;
    if (type != AttackType::True) {
        defenseRoll = target->GetDefensePower();
        Engine::GetLogger().LogDebug(
            target->TypeName() + " defends for " + std::to_string(defenseRoll)
        );
    }

    // Calculate final damage (line 25-26)
    int finalDamage = std::max(0, attackRoll - defenseRoll);

    Engine::GetLogger().LogDebug(
        "Final damage: " + std::to_string(finalDamage) + " HP"
    );

    return finalDamage;
}

// In Cleric.cpp - Healing uses attack power
void Cleric::HealAlly(Character* target) {
    CombatSystem* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();

    // Calculate heal amount (line 201 - equals attack power)
    int healAmount = combat->CalculateHealAmount(this);

    // Apply healing (capped at max HP)
    int currentHP = target->GetCurrentHP();
    int maxHP = target->GetMaxHP();
    int newHP = std::min(currentHP + healAmount, maxHP);

    target->currentHP = newHP;

    // Consume action point
    currentActionPoints--;

    // Visual feedback
    EffectManager::Instance().PlayStatusEffect(target, EffectType::Heal);

    Engine::GetLogger().LogInfo(
        TypeName() + " heals " + target->TypeName() + " for " + std::to_string(healAmount) + " HP"
    );
}
```

### 3.5 SpellSystem GSComponent

**Purpose:** Spell casting, effect application, slot management
**Reasoning from dragonic_tactics.md:**

- Line 29-32: Spell slots with levels, upcasting, no regeneration
- Line 94: Casting costs 1 action + spell slot
- Line 96-109: Dragon spell list with complex effects

```cpp
// SpellSystem.h - GSComponent for spell management
// REASONING: Handle complex spell mechanics and spell slot economy

#pragma once
#include "../Engine/Component.h"
#include "Character.h"
#include "SpellTypes.h"
#include <vector>

// Spell definitions from dragonic_tactics.md line 96-109
enum class SpellType {
    // Level 1
    CreateWall,     // Line 100: 2x1 wall, range 5
    Fireball,       // Line 101: 2d8 damage, upcast +1d6

    // Level 2
    LavaPool,       // Line 102: 2x2 lava, 8 burn/turn for 5 turns
    DragonRoar,     // Line 103: AOE fear, range 3
    TailSwipe,      // Line 104: 1d8 + knockback 2 tiles

    // Level 3
    DragonWrath,    // Line 105: 4d6, first in line, upcast +2d6
    MeteorStrike,   // Line 106: 3d20 AOE, skip next turn

    // Level 4
    HeatAbsorb,     // Line 107: Remove burns/lava, gain stats

    // Level 5
    HeatRelease     // Line 108: Self-damage 50% HP, restore 1st-3rd slots
};

struct SpellData {
    int level;              // Spell level (1-5)
    int range;              // Casting range in tiles
    bool requiresLineOfSight;
    std::string description;
};

class SpellSystem : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Update active spell effects (lava durations, etc.)
    // REASONING: Line 102 - Lava lasts 5 turns, need to track
    // IMPLEMENTATION: Decrement effect timers, remove expired

    // ===== Spell Casting =====

    void CastSpell(Character* caster, SpellType spell, Math::vec2 target, int upcastLevel = 0);
    // Cast spell at target location
    // REASONING: Line 94 - "행동을 1, 주문의 요구 레벨 이상의 주문 슬롯을 1 소모해서 사용"
    // IMPLEMENTATION:
    //   1. Validate caster has spell slot available
    //   2. Validate target in range (line 100-106 specify ranges)
    //   3. Consume spell slot and action point
    //   4. Apply spell effect based on type
    //   5. If upcasted, apply bonus effects (line 101, 102, 103, 105, 106)
    //   6. Publish SpellCastEvent
    // USAGE: spellSys->CastSpell(dragon, SpellType::Fireball, enemyPos, 0);

    bool CanCastSpell(Character* caster, SpellType spell, Math::vec2 target) const;
    // Validate if spell can be cast
    // REASONING: Check resources, range, line of sight
    // IMPLEMENTATION:
    //   1. Check caster has action points (line 94)
    //   2. Check caster has spell slot for spell level
    //   3. Check target in range
    //   4. Check line of sight if required
    // USAGE: if (spellSys->CanCastSpell(dragon, SpellType::Fireball, pos)) { Highlight(); }

    bool CanUpcast(Character* caster, SpellType spell, int targetLevel) const;
    // Check if spell can be upcast to higher slot
    // REASONING: Line 30-31 - "낮은 레벨의 주문을 높은 레벨의 주문 슬롯을 소모해서 사용하는 업캐스팅이 가능하다"
    // IMPLEMENTATION:
    //   SpellData data = GetSpellData(spell);
    //   return targetLevel > data.level && caster->spellSlots[targetLevel] > 0;
    // USAGE: if (CanUpcast(dragon, SpellType::Fireball, 3)) { ShowUpcastOption(); }

    // ===== Spell Slot Management =====

    void ConsumeSpellSlot(Character* caster, int level);
    // Use one spell slot at specified level
    // REASONING: Line 29-32 - Spell slots consumed on use, don't regenerate
    // IMPLEMENTATION: caster->spellSlots[level]--;
    // USAGE: ConsumeSpellSlot(dragon, 1);

    bool HasSpellSlot(Character* caster, int level) const;
    // Check if spell slot available at level
    // REASONING: Must validate before casting
    // IMPLEMENTATION: return caster->spellSlots[level] > 0;
    // USAGE: if (HasSpellSlot(dragon, 3)) { EnableLevel3Spells(); }

    void RestoreSpellSlots(Character* caster, int minLevel, int maxLevel);
    // Restore spell slots in range (for Heat Release spell)
    // REASONING: Line 108 - "3레벨 이하의 주문슬롯을 전부 회복"
    // IMPLEMENTATION:
    //   for level in minLevel..maxLevel:
    //     caster->spellSlots[level] = caster->maxSpellSlots[level]
    // USAGE: RestoreSpellSlots(dragon, 1, 3);  // Heat Release effect

    // ===== Spell Data Access =====

    SpellData GetSpellData(SpellType spell) const;
    // Get spell information (level, range, description)
    // REASONING: UI needs to display spell details
    // USAGE: auto data = GetSpellData(SpellType::Fireball);

    std::vector<SpellType> GetAvailableSpells(Character* caster) const;
    // Get all spells caster can currently cast
    // REASONING: UI shows only castable spells
    // IMPLEMENTATION: Filter by available spell slots
    // USAGE: auto spells = GetAvailableSpells(dragon);

    // ===== Area Targeting =====

    std::vector<Math::vec2> GetSpellArea(SpellType spell, Math::vec2 target) const;
    // Get all tiles affected by spell
    // REASONING: Line 102, 103, 106 - Some spells have AOE
    // EXAMPLES:
    //   CreateWall: 2x1 tiles (line 100)
    //   LavaPool: 2x2 tiles (line 102)
    //   DragonRoar: 3 tile radius (line 103)
    //   MeteorStrike: All even OR all odd tiles (line 106)
    // USAGE: auto tiles = GetSpellArea(SpellType::LavaPool, center);

    std::vector<Character*> GetSpellTargets(SpellType spell, Math::vec2 target) const;
    // Get all characters hit by spell
    // REASONING: AOE spells affect multiple targets
    // IMPLEMENTATION: Get spell area, find characters on those tiles
    // USAGE: auto targets = GetSpellTargets(SpellType::DragonRoar, dragonPos);

private:
    // Spell implementations
    void CastFireball(Character* caster, Math::vec2 target, int upcastLevel);
    void CastLavaPool(Character* caster, Math::vec2 target, int upcastLevel);
    void CastDragonRoar(Character* caster, int upcastLevel);
    void CastTailSwipe(Character* caster);
    void CastDragonWrath(Character* caster, Math::vec2 direction, int upcastLevel);
    void CastMeteorStrike(Character* caster, bool evenTiles, int upcastLevel);
    void CastHeatAbsorb(Character* caster);
    void CastHeatRelease(Character* caster);
    void CastCreateWall(Character* caster, Math::vec2 target);

    // Spell data registry (loaded from DataRegistry)
    std::unordered_map<SpellType, SpellData> spellDatabase;
};
```

**USAGE EXAMPLE:**

```cpp
// In Dragon.cpp - Cast fireball with upcasting
void Dragon::CastSpell(SpellType spell, Math::vec2 target) {
    SpellSystem* spellSys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();

    // Check if we can cast
    if (!spellSys->CanCastSpell(this, spell, target)) {
        Engine::GetLogger().LogError("Cannot cast spell!");
        return;
    }

    // Ask player: upcast to higher slot?
    int upcastLevel = 0;
    if (spell == SpellType::Fireball) {
        // Fireball can be upcast (line 101: +1d6 per level)
        if (spellSys->CanUpcast(this, spell, 3)) {
            // Show UI: "Cast Fireball with level 3 slot for bonus damage?"
            if (PlayerChoosesUpcast()) {
                upcastLevel = 3;
            }
        }
    }

    // Cast spell
    spellSys->CastSpell(this, spell, target, upcastLevel);
}

// In SpellSystem.cpp - Fireball implementation
void SpellSystem::CastFireball(Character* caster, Math::vec2 target, int upcastLevel) {
    // Line 101: "화염탄: 2d8의 피해, 1레벨당 +1d6"
    SpellData data = GetSpellData(SpellType::Fireball);
    int spellLevel = (upcastLevel > 0) ? upcastLevel : data.level;

    // Calculate damage
    int baseDamage = DiceManager::Instance().RollDice(2, 8);  // 2d8

    // Upcast bonus
    if (upcastLevel > data.level) {
        int bonusLevels = upcastLevel - data.level;
        int bonusDamage = DiceManager::Instance().RollDice(bonusLevels, 6);  // +1d6 per level
        baseDamage += bonusDamage;
        Engine::GetLogger().LogInfo("Upcast bonus: +" + std::to_string(bonusDamage) + " damage");
    }

    // Apply damage to target
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    Character* target = grid->GetCharacterAt(target);
    if (target) {
        CombatSystem* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
        combat->ApplyDamage(target, baseDamage);
    }

    // Consume resources
    ConsumeSpellSlot(caster, spellLevel);
    caster->currentActionPoints--;

    // Visual effect
    EffectManager::Instance().PlaySpellEffect(SpellType::Fireball, caster->GetPosition(), target);

    EventBus::Instance().Publish(SpellCastEvent{caster, SpellType::Fireball, target, spellLevel});
}

// In SpellSystem.cpp - Lava pool with duration
void SpellSystem::CastLavaPool(Character* caster, Math::vec2 target, int upcastLevel) {
    // Line 102: "용암 뿌리기: 5턴동안 턴 시작시 8의 피해를 주는 화상을 거는 2*2의 용암을 생성"
    SpellData data = GetSpellData(SpellType::LavaPool);
    int spellLevel = (upcastLevel > 0) ? upcastLevel : data.level;

    // Base: 8 damage per turn, 5 turns
    int damagePerTurn = 8;
    int duration = 5;

    // Upcast bonus: +2 damage, +1 turn per level
    if (upcastLevel > data.level) {
        int bonusLevels = upcastLevel - data.level;
        damagePerTurn += bonusLevels * 2;
        duration += bonusLevels;
    }

    // Create 2x2 lava area
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    std::vector<Math::vec2> lavaPositions = grid->GetTilesInArea(target, 2, true);  // 2x2 square

    for (auto pos : lavaPositions) {
        grid->SetTileType(pos, TileType::Lava);
        lavaTiles[pos] = LavaTile{damagePerTurn, duration};  // Track duration
    }

    // Consume resources
    ConsumeSpellSlot(caster, spellLevel);
    caster->currentActionPoints--;

    EffectManager::Instance().PlaySpellEffect(SpellType::LavaPool, caster->GetPosition(), target);
}

// In SpellSystem.cpp - Update lava durations
void SpellSystem::Update(double dt) {
    TurnManager* turnMgr = Engine::GetGameStateManager().GetGSComponent<TurnManager>();

    // Check if new turn started
    if (turnMgr->IsNewTurn()) {
        GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

        // Apply burn damage to characters on lava
        for (auto& [pos, lavaData] : lavaTiles) {
            Character* victim = grid->GetCharacterAt(pos);
            if (victim) {
                victim->TakeDamage(lavaData.damagePerTurn);
                Engine::GetLogger().LogInfo(
                    victim->TypeName() + " takes " + std::to_string(lavaData.damagePerTurn) + " burn damage!"
                );
            }

            // Decrement duration
            lavaData.turnsRemaining--;
            if (lavaData.turnsRemaining <= 0) {
                // Lava expires
                grid->SetTileType(pos, TileType::Empty);
                lavaTiles.erase(pos);
            }
        }
    }
}
```

---

### 3.6 AISystem GSComponent

**Purpose:** Coordinate AI decision-making and global AI behavior
**Reasoning from dragonic_tactics.md:**

- Line 115-264: Four AI character types with distinct tactical behaviors
- Line 49: AI executes actions during their turn based on character type
- AI needs global coordination for team tactics

```cpp
// AISystem.h - GSComponent for AI coordination
// REASONING: Centralized AI decision-making and team coordination

#pragma once
#include "../Engine/Component.h"
#include "Character.h"
#include <vector>

enum class AIPersonality {
    Aggressive,    // Prioritize damage to dragon
    Defensive,     // Protect low-HP allies
    Tactical,      // Optimize positioning and combo attacks
    Opportunistic  // Exploit weaknesses (low HP, debuffs)
};

// ActionDecision: The communication protocol between Character::UpdateAI() and AISystem
// REASONING: Character asks "What should I do?", AISystem returns this struct
enum class ActionType {
    Move,
    MeleeAttack,
    RangedAttack,
    Heal,
    CastSpell,
    Hide,
    Sprint,
    EndTurn
};

struct ActionDecision {
    ActionType action;        // What action to perform
    Character* target;        // Who to target (nullptr for self-targeted actions)
    Math::vec2 position;      // Where to move/cast spell
    SpellType spellType;      // Which spell to cast (if action == CastSpell)
    int priority;             // Action score (0-100, higher = better)
};

class AISystem : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Process AI character turns
    // REASONING: Line 50-52 - AI executes actions during their turn
    // IMPLEMENTATION:
    //   1. Check if current character is AI
    //   2. If yes, evaluate best action via AIDirector singleton
    //   3. Execute action (move, attack, spell)
    //   4. End turn when action points exhausted
    // USAGE: Called automatically by BattleState::Update()

    // ===== AI Decision Making =====

    void ProcessAITurn(Character* aiCharacter);
    // Execute one AI character's complete turn
    // REASONING: Line 115-264 - Each character type has unique behavior
    // IMPLEMENTATION:
    //   1. Get character type (Fighter, Cleric, Wizard, Rogue)
    //   2. Evaluate available actions (move, attack, spell, heal)
    //   3. Use AIDirector::EvaluateAction() for scoring
    //   4. Execute highest-scoring action
    //   5. Repeat until no action points remain
    // USAGE: aiSys->ProcessAITurn(fighter);

    ActionDecision DecideAction(Character* aiCharacter);
    // Determine best action for AI character
    // REASONING: AI needs to evaluate multiple options and pick best
    // IMPLEMENTATION:
    //   switch (character type):
    //     Fighter: Melee attack priority, move to dragon if out of range
    //     Cleric: Heal lowest HP ally, attack if all healthy
    //     Wizard: Ranged spell attack from safe distance
    //     Rogue: Flank dragon, use Hide action, sneak attack
    //   Use threat assessment and target prioritization
    // USAGE: auto decision = aiSys->DecideAction(wizard);

    // ===== Target Selection =====

    Character* SelectBestTarget(Character* attacker);
    // Choose optimal attack target
    // REASONING: Line 141, 170, 201, 227, 254 - AI prioritizes dragon
    // IMPLEMENTATION:
    //   1. Default: Target dragon (player character)
    //   2. Override: If ally dying and character is Cleric, target ally for heal
    //   3. Override: If opportunity attack available (flanking), prioritize
    //   4. Consider range, line of sight, and action point cost
    // USAGE: auto* target = aiSys->SelectBestTarget(fighter);

    Math::vec2 FindOptimalPosition(Character* mover, Character* target);
    // Calculate best grid position for character
    // REASONING: AI needs tactical positioning for range/flanking
    // IMPLEMENTATION:
    //   1. Get all reachable tiles (GridSystem::GetReachableTiles)
    //   2. Score each position:
    //      - Distance to target (closer = better for melee)
    //      - Cover/obstacle proximity
    //      - Adjacency to allies (formation bonuses)
    //      - Escape routes (avoid corners)
    //   3. Return highest-scoring tile
    // USAGE: auto pos = aiSys->FindOptimalPosition(rogue, dragon);

    // ===== Team Coordination =====

    void CoordinateTeamStrategy();
    // Global AI strategy adjustment
    // REASONING: AI should adapt based on battle state
    // IMPLEMENTATION:
    //   1. Assess dragon HP percentage
    //   2. If dragon < 30% HP: All AI go aggressive
    //   3. If any AI < 20% HP: Cleric prioritizes healing
    //   4. If fighter dead: Rogue switches to tank role
    //   5. Update AIPersonality for each character
    // USAGE: Called at start of each round

    bool ShouldFocus Fire(Character* target);
    // Check if all AI should target same character
    // REASONING: Concentrated attacks more effective
    // IMPLEMENTATION:
    //   if (target->GetCurrentHP() < 30) return true;  // Finish low HP targets
    //   if (target->IsPlayer()) return true;  // Always focus dragon
    //   return false;
    // USAGE: if (ShouldFocusFire(dragon)) { AllAttackDragon(); }

    // ===== Difficulty Scaling =====

    void SetDifficultyLevel(int level);
    // Adjust AI intelligence and reaction time
    // REASONING: Accessibility and player progression
    // IMPLEMENTATION:
    //   Level 1 (Easy): AI makes random mistakes 30% of time
    //   Level 2 (Normal): Standard tactical AI
    //   Level 3 (Hard): Perfect play with optimal positioning
    //   Level 4 (Expert): AI uses advanced combos and baiting
    // USAGE: aiSys->SetDifficultyLevel(2);

    float GetReactionDelay() const;
    // Visual delay before AI acts (for player readability)
    // REASONING: Instant AI turns are confusing for players
    // IMPLEMENTATION: return difficulty == 1 ? 1.5f : 0.5f;  // Seconds
    // USAGE: Used by animation system to pace AI turns

    // ===== Query Methods =====

    bool IsAIControlled(Character* character) const;
    // Check if character is AI-controlled
    // REASONING: Distinguish player vs AI
    // IMPLEMENTATION: return character->Type() != GameObjectTypes::Dragon;
    // USAGE: if (aiSys->IsAIControlled(current)) { ProcessAITurn(current); }

    AIPersonality GetPersonality(Character* character) const;
    // Get current AI behavior mode
    // REASONING: UI can display AI intent (e.g., "Aggressive" icon)
    // USAGE: auto personality = aiSys->GetPersonality(fighter);

private:
    // AI state tracking
    std::unordered_map<Character*, AIPersonality> characterPersonalities;
    int difficultyLevel = 2;  // 1=Easy, 2=Normal, 3=Hard, 4=Expert
    float reactionDelay = 0.5f;  // Seconds before AI acts

    // Decision-making helpers
    float ScoreAction(Character* character, ActionType action, Character* target);
    bool IsSafePosition(Math::vec2 pos, Character* character);
    std::vector<Character*> GetAlliesInRange(Character* character, int range);
};
```

**CRITICAL: How Character::UpdateAI() Interacts with AISystem**

See **[characters.md Section 2: Character-AISystem Interaction Flow](characters.md#2-character-aisystem-interaction-flow)** for complete details on:

- Why characters delegate to AISystem instead of having their own AI logic
- Complete turn execution flow (5-step sequence)
- ActionDecision struct communication protocol
- Team coordination and difficulty scaling examples

**TL;DR**: Characters call `AISystem::DecideAction(this)`, receive an `ActionDecision` struct back, and execute it. All AI strategy lives in AISystem, not in character classes.

---

**USAGE EXAMPLE:**

```cpp
// In BattleState.cpp - Initialize AI system
void BattleState::Load() {
    AddGSComponent(new BattleManager());
    AddGSComponent(new TurnManager());
    AddGSComponent(new GridSystem());
    AddGSComponent(new CombatSystem());
    AddGSComponent(new SpellSystem());
    AddGSComponent(new AISystem());
    AddGSComponent(new StatusEffectManager());

    // Set difficulty based on player choice
    GetGSComponent<AISystem>()->SetDifficultyLevel(2);  // Normal difficulty
}

// In TurnManager.cpp - AI turn processing
void TurnManager::Update(double dt) {
    Character* current = GetCurrentCharacter();
    AISystem* aiSys = Engine::GetGameStateManager().GetGSComponent<AISystem>();

    if (aiSys->IsAIControlled(current)) {
        // Wait for reaction delay (visual pacing)
        aiReactionTimer += dt;
        if (aiReactionTimer >= aiSys->GetReactionDelay()) {
            aiSys->ProcessAITurn(current);
            aiReactionTimer = 0.0f;
        }
    } else {
        // Player turn - wait for input
        ProcessPlayerInput();
    }
}

// In AISystem.cpp - Fighter AI behavior
void AISystem::ProcessAITurn(Character* aiCharacter) {
    // Line 170-180: Fighter behavior - "Move to dragon and attack"
    if (aiCharacter->Type() == GameObjectTypes::Fighter) {
        Dragon* dragon = FindDragon();
        GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
        CombatSystem* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();

        // Check if dragon in melee range
        int distance = grid->ManhattanDistance(aiCharacter->GetGridPosition(), dragon->GetGridPosition());

        if (distance <= 1 && aiCharacter->GetActionPoints() > 0) {
            // Attack dragon
            combat->ExecuteAttack(aiCharacter, dragon, AttackType::Melee);
            Engine::GetLogger().LogInfo("Fighter attacks dragon!");
        } else {
            // Move closer to dragon
            Math::vec2 optimalPos = FindOptimalPosition(aiCharacter, dragon);
            aiCharacter->SetGridPosition(optimalPos);
            aiCharacter->ConsumeActionPoint();
            Engine::GetLogger().LogInfo("Fighter moves toward dragon");
        }

        // End turn if no action points left
        if (aiCharacter->GetActionPoints() == 0) {
            GetGSComponent<TurnManager>()->AdvanceToNextTurn();
        }
    }
}

// In AISystem.cpp - Cleric AI behavior
ActionDecision AISystem::DecideAction(Character* aiCharacter) {
    // Line 201-214: Cleric behavior - "Heal allies or attack"
    if (aiCharacter->Type() == GameObjectTypes::Cleric) {
        // Find lowest HP ally
        Character* lowestHPAlly = nullptr;
        int lowestHP = 999;

        auto allies = GetAlliesInRange(aiCharacter, 2);  // Heal range = 2 tiles
        for (auto* ally : allies) {
            if (ally->GetCurrentHP() < lowestHP && ally->GetCurrentHP() < ally->GetMaxHP()) {
                lowestHP = ally->GetCurrentHP();
                lowestHPAlly = ally;
            }
        }

        // If ally below 50% HP, heal them
        if (lowestHPAlly && lowestHP < lowestHPAlly->GetMaxHP() / 2) {
            return ActionDecision{ActionType::Heal, lowestHPAlly};
        } else {
            // No one needs healing - attack dragon
            Dragon* dragon = FindDragon();
            return ActionDecision{ActionType::RangedAttack, dragon};
        }
    }

    // Default: Attack dragon
    return ActionDecision{ActionType::MeleeAttack, FindDragon()};
}

// In AISystem.cpp - Difficulty scaling
void AISystem::SetDifficultyLevel(int level) {
    difficultyLevel = level;

    switch (level) {
        case 1:  // Easy
            reactionDelay = 1.5f;
            Engine::GetLogger().LogInfo("AI Difficulty: Easy (30% mistake rate)");
            break;
        case 2:  // Normal
            reactionDelay = 0.8f;
            Engine::GetLogger().LogInfo("AI Difficulty: Normal");
            break;
        case 3:  // Hard
            reactionDelay = 0.3f;
            Engine::GetLogger().LogInfo("AI Difficulty: Hard (optimal play)");
            break;
        case 4:  // Expert
            reactionDelay = 0.3f;
            Engine::GetLogger().LogInfo("AI Difficulty: Expert (advanced tactics)");
            break;
    }
}
```

---

### 3.7 StatusEffectManager GSComponent

**Purpose:** Track and update all active status effects across all characters
**Reasoning from dragonic_tactics.md:**

- Line 102: Burn effect from lava (8 damage per turn for 5 turns)
- Line 103: Fear effect from Dragon Roar (skip turn)
- Line 108: Heat Absorb removes burns and lava
- Line 201-202: Blessing buff (+3 attack/defense for 3 turns)
- Line 251-252: Curse debuff (-3 attack/defense for 3 turns)

```cpp
// StatusEffectManager.h - GSComponent for buff/debuff tracking
// REASONING: Centralized status effect lifecycle management

#pragma once
#include "../Engine/Component.h"
#include "Character.h"
#include <vector>

enum class EffectType {
    // Buffs
    Blessing,       // Line 201: +3 attack/defense for 3 turns
    Haste,          // +1 speed (future feature)
    Shield,         // +5 defense for 2 turns

    // Debuffs
    Burn,           // Line 102: 8 damage per turn for 5 turns
    Fear,           // Line 103: Skip next turn
    Curse,          // Line 251: -3 attack/defense for 3 turns
    Slow,           // -1 speed (future feature)

    // Special
    Poison,         // 5 damage per turn, stacks
    Stun,           // Cannot act for 1 turn
    Invisible       // Cannot be targeted (Rogue ability)
};

struct StatusEffect {
    EffectType type;
    int duration;           // Turns remaining
    int magnitude;          // Effect strength (damage, stat modifier, etc.)
    Character* source;      // Who applied this effect
    Character* target;      // Who is affected
};

class StatusEffectManager : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Process status effects at turn boundaries
    // REASONING: Line 102 - "턴 시작시 8의 피해" (Damage at turn start)
    // IMPLEMENTATION:
    //   1. Listen for TurnStartedEvent from TurnManager
    //   2. Apply start-of-turn effects (burn damage, poison)
    //   3. Decrement durations
    //   4. Remove expired effects
    //   5. Apply end-of-turn effects (regeneration, etc.)
    // USAGE: Called automatically by BattleState::UpdateGSComponents()

    // ===== Effect Application =====

    void ApplyEffect(Character* target, EffectType type, int duration, int magnitude, Character* source = nullptr);
    // Add status effect to character
    // REASONING: Line 102, 103, 201, 251 - Various spells apply effects
    // IMPLEMENTATION:
    //   1. Create StatusEffect struct
    //   2. Check if effect already exists (some stack, some don't)
    //   3. If stacks (Burn, Poison): Add new instance
    //   4. If doesn't stack (Fear, Blessing): Refresh duration to max
    //   5. Add to activeEffects list
    //   6. Publish EffectAppliedEvent
    // USAGE: effectMgr->ApplyEffect(dragon, EffectType::Burn, 5, 8, wizard);

    void RemoveEffect(Character* target, EffectType type);
    // Remove all instances of effect from character
    // REASONING: Line 108 - "화상과 용암을 제거" (Remove burns and lava)
    // IMPLEMENTATION:
    //   1. Find all effects of type on target
    //   2. Remove from activeEffects
    //   3. Publish EffectRemovedEvent
    // USAGE: effectMgr->RemoveEffect(dragon, EffectType::Burn);

    void RemoveAllEffects(Character* target);
    // Clear all status effects from character
    // REASONING: Character death should clear effects
    // IMPLEMENTATION: activeEffects.erase_if([target](e) { return e.target == target; });
    // USAGE: effectMgr->RemoveAllEffects(dragon);

    // ===== Effect Processing =====

    void ProcessStartOfTurnEffects(Character* character);
    // Apply effects that trigger at turn start
    // REASONING: Line 102 - Burn damage at turn start
    // IMPLEMENTATION:
    //   for each effect on character:
    //     if (effect.type == Burn || Poison):
    //       ApplyDamage(character, effect.magnitude)
    //     if (effect.type == Fear):
    //       SkipTurn(character)
    //   DecrementDurations(character)
    // USAGE: effectMgr->ProcessStartOfTurnEffects(dragon);

    void ProcessEndOfTurnEffects(Character* character);
    // Apply effects that trigger at turn end
    // REASONING: Some effects may trigger at end of turn
    // IMPLEMENTATION: Process regeneration, decrement durations
    // USAGE: effectMgr->ProcessEndOfTurnEffects(fighter);

    void DecrementDurations(Character* character);
    // Reduce remaining duration by 1 turn
    // REASONING: Line 102, 201, 251 - Effects have limited duration
    // IMPLEMENTATION:
    //   for each effect on character:
    //     effect.duration--
    //     if effect.duration <= 0:
    //       RemoveEffect(character, effect.type)
    // USAGE: Called by ProcessStartOfTurnEffects()

    // ===== Query Methods =====

    bool HasEffect(Character* target, EffectType type) const;
    // Check if character has specific effect
    // REASONING: AI and spell logic needs to check effect status
    // IMPLEMENTATION: return std::any_of(activeEffects, [](e) { return e.target == target && e.type == type; });
    // USAGE: if (effectMgr->HasEffect(dragon, EffectType::Burn)) { DontApplyMoreBurn(); }

    std::vector<StatusEffect> GetEffects(Character* target) const;
    // Get all active effects on character
    // REASONING: UI displays effect icons on character portraits
    // USAGE: auto effects = effectMgr->GetEffects(dragon);

    int GetEffectStacks(Character* target, EffectType type) const;
    // Count how many instances of effect are active
    // REASONING: Burn and Poison stack - UI shows count
    // IMPLEMENTATION: return std::count_if(activeEffects, [](e) { return e.target == target && e.type == type; });
    // USAGE: int burnStacks = effectMgr->GetEffectStacks(dragon, EffectType::Burn);

    int GetRemainingDuration(Character* target, EffectType type) const;
    // Get turns remaining for effect
    // REASONING: UI shows duration counter on effect icons
    // USAGE: int turnsLeft = effectMgr->GetRemainingDuration(dragon, EffectType::Blessing);

    // ===== Stat Modification =====

    int CalculateModifiedAttack(Character* character) const;
    // Get attack power including effect modifiers
    // REASONING: Line 201, 251 - Blessing/Curse modify attack
    // IMPLEMENTATION:
    //   int base = character->GetBaseAttackPower();
    //   for each effect on character:
    //     if (Blessing) base += 3
    //     if (Curse) base -= 3
    //   return max(0, base);
    // USAGE: int attack = effectMgr->CalculateModifiedAttack(dragon);

    int CalculateModifiedDefense(Character* character) const;
    // Get defense power including effect modifiers
    // REASONING: Line 201, 251 - Blessing/Curse modify defense
    // IMPLEMENTATION: Same as CalculateModifiedAttack but for defense
    // USAGE: int defense = effectMgr->CalculateModifiedDefense(fighter);

    int CalculateModifiedSpeed(Character* character) const;
    // Get speed including effect modifiers
    // REASONING: Future Haste/Slow effects
    // IMPLEMENTATION: Apply speed buffs/debuffs
    // USAGE: int speed = effectMgr->CalculateModifiedSpeed(rogue);

    // ===== Special Effect Interactions =====

    bool CanAct(Character* character) const;
    // Check if character can perform actions
    // REASONING: Line 103 - Fear effect skips turn
    // IMPLEMENTATION: return !HasEffect(character, EffectType::Fear) && !HasEffect(character, EffectType::Stun);
    // USAGE: if (!effectMgr->CanAct(fighter)) { SkipTurn(); }

    bool IsVisible(Character* character) const;
    // Check if character can be targeted
    // REASONING: Rogue invisibility prevents targeting
    // IMPLEMENTATION: return !HasEffect(character, EffectType::Invisible);
    // USAGE: if (effectMgr->IsVisible(target)) { AllowTargeting(); }

private:
    std::vector<StatusEffect> activeEffects;  // All active effects in battle

    // Effect stacking rules
    bool CanStack(EffectType type) const;
    // Returns true for Burn, Poison; false for Blessing, Curse, Fear

    // Effect categories
    bool IsBuffEffect(EffectType type) const;
    bool IsDebuffEffect(EffectType type) const;
    bool IsDamageOverTimeEffect(EffectType type) const;
};
```

**USAGE EXAMPLE:**

```cpp
// In BattleManager.cpp - Initialize effect manager
void BattleManager::StartBattle(const std::vector<Character*>& allCombatants) {
    allCharacters = allCombatants;

    // Effect manager tracks all characters' effects
    StatusEffectManager* effectMgr = GetGSComponent<StatusEffectManager>();
    // No initialization needed - automatically tracks effects as they're applied
}

// In SpellSystem.cpp - Apply burn from lava
void SpellSystem::CastLavaPool(Character* caster, Math::vec2 target, int upcastLevel) {
    // Line 102: "5턴동안 턴 시작시 8의 피해를 주는 화상"
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    StatusEffectManager* effectMgr = Engine::GetGameStateManager().GetGSComponent<StatusEffectManager>();

    // Create lava tiles
    std::vector<Math::vec2> lavaPositions = grid->GetTilesInArea(target, 2, true);

    for (auto pos : lavaPositions) {
        grid->SetTileType(pos, TileType::Lava);

        // Apply burn to any character standing on lava
        Character* victim = grid->GetCharacterAt(pos);
        if (victim) {
            effectMgr->ApplyEffect(victim, EffectType::Burn, 5, 8, caster);
            Engine::GetLogger().LogInfo(victim->TypeName() + " is burning!");
        }
    }
}

// In StatusEffectManager.cpp - Process burn damage
void StatusEffectManager::ProcessStartOfTurnEffects(Character* character) {
    // Get all effects on this character
    auto effects = GetEffects(character);

    for (auto& effect : effects) {
        switch (effect.type) {
            case EffectType::Burn:
            case EffectType::Poison:
                // Apply damage
                character->TakeDamage(effect.magnitude);
                Engine::GetLogger().LogInfo(
                    character->TypeName() + " takes " + std::to_string(effect.magnitude) + " burn damage!"
                );
                EventBus::Instance().Publish(DamageOverTimeEvent{character, effect.type, effect.magnitude});
                break;

            case EffectType::Fear:
                // Skip turn
                Engine::GetLogger().LogInfo(character->TypeName() + " is too afraid to act!");
                GetGSComponent<TurnManager>()->SkipCurrentTurn();
                break;

            // Other effects...
        }
    }

    // Decrement durations
    DecrementDurations(character);
}

// In Cleric.cpp - Apply Blessing buff
void Cleric::CastBlessing(Character* target) {
    // Line 201: "3턴동안 공격력과 방어력을 +3"
    StatusEffectManager* effectMgr = Engine::GetGameStateManager().GetGSComponent<StatusEffectManager>();

    effectMgr->ApplyEffect(target, EffectType::Blessing, 3, 3, this);

    Engine::GetLogger().LogInfo(TypeName() + " blesses " + target->TypeName() + " (+3 attack/defense for 3 turns)");

    // Consume resources
    ConsumeSpellSlot(1);
    currentActionPoints--;
}

// In CombatSystem.cpp - Use modified stats
int CombatSystem::CalculateDamage(Character* attacker, Character* target, AttackType type) {
    StatusEffectManager* effectMgr = Engine::GetGameStateManager().GetGSComponent<StatusEffectManager>();

    // Get attack power with effect modifiers
    int attackRoll = effectMgr->CalculateModifiedAttack(attacker);
    attackRoll += DiceManager::Instance().RollDice(attacker->GetAttackDice());

    // Get defense power with effect modifiers
    int defenseRoll = 0;
    if (type != AttackType::True) {
        defenseRoll = effectMgr->CalculateModifiedDefense(target);
        defenseRoll += DiceManager::Instance().RollDice(target->GetDefenseDice());
    }

    int finalDamage = std::max(0, attackRoll - defenseRoll);

    Engine::GetLogger().LogDebug(
        attacker->TypeName() + " attack: " + std::to_string(attackRoll) +
        " vs " + target->TypeName() + " defense: " + std::to_string(defenseRoll) +
        " = " + std::to_string(finalDamage) + " damage"
    );

    return finalDamage;
}

// In TurnManager.cpp - Check if character can act
void TurnManager::Update(double dt) {
    Character* current = GetCurrentCharacter();
    StatusEffectManager* effectMgr = Engine::GetGameStateManager().GetGSComponent<StatusEffectManager>();

    // Process start-of-turn effects
    effectMgr->ProcessStartOfTurnEffects(current);

    // Check if character can act (Fear/Stun check)
    if (!effectMgr->CanAct(current)) {
        Engine::GetLogger().LogInfo(current->TypeName() + " cannot act this turn!");
        AdvanceToNextTurn();
        return;
    }

    // Normal turn processing...
}

// In Dragon.cpp - Heat Absorb removes burns
void Dragon::CastHeatAbsorb() {
    // Line 108: "자신의 화상과 주변 용암을 제거하고..."
    StatusEffectManager* effectMgr = Engine::GetGameStateManager().GetGSComponent<StatusEffectManager>();
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    // Remove all burn effects from dragon
    int burnStacks = effectMgr->GetEffectStacks(this, EffectType::Burn);
    effectMgr->RemoveEffect(this, EffectType::Burn);

    // Remove adjacent lava tiles
    auto neighbors = grid->GetNeighbors(GetGridPosition());
    for (auto pos : neighbors) {
        if (grid->GetTileType(pos) == TileType::Lava) {
            grid->SetTileType(pos, TileType::Empty);
        }
    }

    // Gain stat bonuses based on absorbed heat
    int attackBonus = burnStacks * 2;  // +2 attack per burn stack
    effectMgr->ApplyEffect(this, EffectType::Blessing, 3, attackBonus, this);

    Engine::GetLogger().LogInfo("Dragon absorbs heat! +" + std::to_string(attackBonus) + " attack for 3 turns");

    // Consume resources
    ConsumeSpellSlot(4);  // Level 4 spell
    currentActionPoints--;
}
```

---

## Summary

All seven battle-level GSComponents are now specified:

1. **BattleManager** - Victory conditions and battle flow
2. **TurnManager** - Initiative order and action points
3. **GridSystem** - 8x8 battlefield and pathfinding
4. **CombatSystem** - Damage calculation and attack resolution
5. **SpellSystem** - Spell casting and magical effects
6. **AISystem** - AI decision-making and team coordination
7. **StatusEffectManager** - Buff/debuff tracking and stat modification

These components work together to create the complete tactical RPG battle system, leveraging the existing CS230 engine's `GSComponent` pattern.
