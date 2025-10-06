# Playtest 1 Implementation Plan - Week 4

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

