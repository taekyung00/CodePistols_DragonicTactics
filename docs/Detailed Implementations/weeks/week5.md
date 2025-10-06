# Playtest 1 Implementation Plan - Week 5

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
