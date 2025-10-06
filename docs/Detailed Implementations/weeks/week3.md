# Playtest 1 Implementation Plan - Week 3

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

