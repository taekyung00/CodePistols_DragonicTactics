# Dragonic Tactics - 26-Week Implementation Plan

**Purpose**: Collaborative task-based development with balanced workload and early playtesting

**Strategy**: All 5 developers work together on critical path - similar workload, frequent playtests

**Last Updated**: 2025-10-10

**Architecture Alignment**: This plan follows the **pure GameObject inheritance pattern** (NO interfaces) as documented in [systems/interfaces.md](systems/interfaces.md). All characters inherit directly from `Character : public CS230::GameObject`.

**EventBus-Driven**: Systems communicate via EventBus (detailed in [systems/eventbus.md](systems/eventbus.md)) for loose coupling.

---

## Developer Workload Summary (26-Week Total)

**Total Project Duration**: 26 weeks × 5 developers = 130 developer-weeks

**Per Developer Breakdown**:

- **Developer A** (Character & Gameplay): 26 weeks
- **Developer B** (Systems & Algorithms): 26 weeks
- **Developer C** (Logic & Architecture): 26 weeks
- **Developer D** (Data & Services): 26 weeks
- **Developer E** (Tools & Integration): 26 weeks

**Weekly Time Commitment** (per developer):

- Development work: ~35-38 hours/week
- Meetings (standups + integration + playtests): ~2.5 hours/week
- **Total**: ~37.5-40.5 hours/week

**Phase Breakdown**:

- **Phase 1 (Weeks 1-5)**: Foundation & Dragon Playable → **PLAYTEST 1**
- **Phase 2 (Weeks 6-10)**: Full Spell Arsenal → **PLAYTEST 2**
- **Phase 3 (Weeks 11-15)**: All Characters → **PLAYTEST 3**
- **Phase 4 (Weeks 16-20)**: Advanced AI & Team Tactics → **PLAYTEST 4**
- **Phase 5 (Weeks 21-24)**: Visual Polish & Production Quality → **PLAYTEST 5**
- **Phase 6 (Weeks 25-26)**: Final Polish & Release → **PLAYTEST 6**

**Workload Distribution**:

- All developers work similar hours every week (balanced workload)
- No developer sits idle waiting for dependencies
- Task-based division ensures equal contribution across all phases

---

## Table of Contents

- [Detailed Week-by-Week Guides](#detailed-week-by-week-implementation-guides)
- [Development Philosophy](#development-philosophy)
- [Developer Skill Profiles](#developer-skill-profiles-flexible---not-domain-locked)
- [26-Week Development Organized by Playtests](#26-week-development-organized-by-playtests)
- [Phase 1: Foundation & Dragon Playable (Weeks 1-5)](#-phase-1-foundation--dragon-playable-weeks-1-5)
- [Phase 2: Full Spell Arsenal (Weeks 6-10)](#-phase-2-full-spell-arsenal-weeks-6-10)
- [Phase 3: All Characters (Weeks 11-15)](#-phase-3-all-characters-weeks-11-15)
- [Phase 4: Advanced AI & Team Tactics (Weeks 16-20)](#-phase-4-advanced-ai--team-tactics-weeks-16-20)
- [Phase 5: Visual Polish & Production Quality (Weeks 21-24)](#-phase-5-visual-polish--production-quality-weeks-21-24)
- [Phase 6: Final Polish & Release (Weeks 25-26)](#-phase-6-final-polish--release-weeks-25-26)
- [Integration Strategy](#integration-strategy-post-week-5)
- [Milestone Tracking](#milestone-tracking-week-1-5-foundation)
- [Playtesting Schedule](#6-mandatory-playtesting-milestones-weeks-5-10-15-20-24-26)

---

## Detailed Week-by-Week Implementation Guides

**This document provides strategic overview**. For tactical implementation details with code examples and test cases, see:

### **📋 Playtest 1 Implementation (Weeks 1-5)**

**Location**: [Detailed Implementations/weeks/](Detailed%20Implementations/weeks/)

**Total Content**: 3,921 lines of detailed implementation guidance

| Week       | Focus                           | File                                                  | Key Deliverables                                           |
| ---------- | ------------------------------- | ----------------------------------------------------- | ---------------------------------------------------------- |
| **Week 1** | Foundation (5 Systems Parallel) | [week1.md](Detailed%20Implementations/weeks/week1.md) | Character, GridSystem, EventBus, DiceManager, DebugConsole |
| **Week 2** | Dragon + Grid Integration       | [week2.md](Detailed%20Implementations/weeks/week2.md) | Dragon class, A* pathfinding, TurnManager                  |
| **Week 3** | Combat + Spells Begin           | [week3.md](Detailed%20Implementations/weeks/week3.md) | Fireball spell, CombatSystem, SpellSystem, Fighter        |
| **Week 4** | More Spells + Turn System       | [week4.md](Detailed%20Implementations/weeks/week4.md) | CreateWall + LavaPool, Initiative, DataRegistry            |
| **Week 5** | Polish + PLAYTEST 1 🧪          | [week5.md](Detailed%20Implementations/weeks/week5.md) | Balance tuning, BattleState, **First Playtest**           |

**Each week file includes:**
- Day-by-day task breakdowns for all 5 developers
- Complete code examples with mock testing strategy
- File structure and naming conventions
- Rigorous test suites with edge cases
- Friday integration tests
- Deliverable verification checklists

**When to use these guides:**
- You are actively implementing Weeks 1-5
- You need specific code examples and test cases
- You want to understand daily task breakdowns
- You're doing code reviews (test requirements included)

**Relationship to this document:**
- **implementation-plan.md** (this file): "WHAT" and "WHEN" - Strategic 26-week overview
- **weeks/*.md** files: "HOW" - Tactical day-by-day implementation with code

---

## Development Philosophy

### Core Principles

**1. Work Together, Not in Silos**

- All 5 developers collaborate on critical path tasks
- Divide work by what needs to be done **this week**, not by permanent domains
- Everyone contributes from Day 1, no idle time

**2. Balanced Workload**

- Each developer has similar amount of work every week
- No one person carries the whole team
- No one sits idle waiting for dependencies

**3. Playtest Early and Often**

- **Week 5**: First playtest (Dragon playable)
- **Weeks 10, 15, 20, 24, 26**: Additional playtests
- Find problems early when there's time to fix them
- Validate game design before building too much

**4. Task-Based Division (Not Role-Based)**

- Forget permanent "ownership" of systems
- Each week: divide tasks among 5 developers efficiently
- Developers can work on any system based on skills and workload

### Why This Approach Works

✅ **Equal Contribution**: Everyone works similar hours, similar value
✅ **Early Validation**: Dragon playable at Week 5 validates core design
✅ **Risk Mitigation**: 6 playtests catch problems early (21 weeks to fix before delivery)
✅ **True Parallelism**: 5 developers build 5 systems simultaneously
✅ **Flexibility**: Adapt task assignments based on actual progress

---

## Developer Skill Profiles (Flexible - Not Domain-Locked)

**IMPORTANT**: Developers are NOT locked to specific systems. These are skill preferences to guide task assignment, not permanent roles.

### Developer A: Character & Gameplay Specialist

**Strengths**: GameObject inheritance, state machines, gameplay mechanics, spell design
**Week 1-5 Tasks**: Character base class, Dragon class, Dragon spells (Fireball, CreateWall, LavaPool)
**Can also work on**: Any gameplay system, combat mechanics, abilities

### Developer B: Systems & Algorithms Specialist

**Strengths**: Data structures, algorithms, grid systems, pathfinding, turn-based mechanics
**Week 1-5 Tasks**: GridSystem (8x8 grid, A* pathfinding), TurnManager, CombatSystem basics
**Can also work on**: Any system requiring algorithms, optimization, math

### Developer C: Logic & Architecture Specialist

**Strengths**: System design, singletons, event-driven architecture, AI logic
**Week 1-5 Tasks**: EventBus singleton, TurnManager, SpellSystem, Fighter manual control
**Can also work on**: Any architectural system, managers, coordinators

### Developer D: Data & Services Specialist

**Strengths**: Singletons, factories, JSON data, data-driven design
**Week 1-5 Tasks**: DiceManager, DataRegistry, Fighter enemy class, dragon_stats.json
**Can also work on**: Any service, factory, data loading system

### Developer E: Tools & Integration Specialist

**Strengths**: Debug tools, UI, system integration, testing, build systems
**Week 1-5 Tasks**: DebugConsole, GridDebugRenderer, debug commands, BattleState integration
**Can also work on**: Any debug tool, integration work, testing infrastructure

---

## 26-Week Development Organized by Playtests

**Development Philosophy**: Each playtest validates a major milestone. Work builds incrementally toward each playtest goal.

**6 Playtest Milestones**:

- **PT1 (Week 5)**: Dragon Playable - Core gameplay loop
- **PT2 (Week 10)**: Full Spell Arsenal - All Dragon abilities
- **PT3 (Week 15)**: All Characters - 1v4 battle basics
- **PT4 (Week 20)**: Advanced AI - Team coordination
- **PT5 (Week 24)**: Visual Polish - Production quality
- **PT6 (Week 26)**: Final Release - Complete game

---

## 🧪 Phase 1: Foundation & Dragon Playable (Weeks 1-5)

**Goal**: Dragon playable with core combat mechanics

**Target**: PLAYTEST 1 - Dragon vs Fighter Battle

> **📋 For detailed implementation guide, see [Detailed Implementations/weeks/week1.md](Detailed%20Implementations/weeks/week1.md)**
>
> The detailed guide includes:
> - Day-by-day task breakdowns for all 5 developers
> - Complete code examples with mock testing strategy
> - File structure, naming conventions, and integration tests
> - Playtest 1 execution procedures

### Why Week 5 for First Playtest?

**CRITICAL CHANGE**: Original plan had Playtest 1 (Dragon playable) at Week 8. This is too slow!

**NEW GOAL**: Dragon playable by **Week 5** (3 weeks faster)

#### Why the Original Plan Was Slow

**Problem 1**: Role-based division created idle developers

- Dev 3 (AI Lead) had nothing to do Weeks 1-7 because AI needs characters first
- Sequential dependencies blocked parallel work

**Problem 2**: Too many systems before first playtest

- Trying to build everything "properly" before testing
- No early validation that Dragon mechanics work

**Problem 3**: Playtest 1 at Week 8 is too late

- 2 months before first playable demo
- High risk of discovering design problems late

### New Approach: Task-Based Parallelization

**Philosophy**: Forget "domain ownership". All 5 developers work together on the **critical path** to make Dragon playable.

**Critical Path for Dragon Playable**:

1. Character base class + Dragon
2. GridSystem (8x8 grid, movement)
3. EventBus + DiceManager
4. TurnManager (basic turns)
5. CombatSystem (basic damage)
6. SpellSystem (3 Dragon spells)
7. Fighter (1 enemy)
8. Debug tools (console commands)

**Week-by-Week Task Distribution** (ALL 5 developers work in parallel):

#### Week 1: Foundation (5 developers, 5 systems in parallel)

| Developer | Task                   | Deliverable                                         |
| --------- | ---------------------- | --------------------------------------------------- |
| **Dev A** | Character base class   | Character.h/cpp with HP, movement, state machine    |
| **Dev B** | GridSystem foundation  | 8x8 grid, tile validation, IsValidTile()            |
| **Dev C** | EventBus singleton     | Publish/Subscribe system, 10 event types            |
| **Dev D** | DiceManager singleton  | RollDice(), RollDiceFromString("3d6")               |
| **Dev E** | DebugConsole framework | Console UI, command registration, ~5 basic commands |

**End of Week 1**: 5 core systems exist (stubs → real implementations)

---

#### Week 2: Dragon + Grid + Dice (All systems connect)

| Developer | Task                   | Deliverable                                      |
| --------- | ---------------------- | ------------------------------------------------ |
| **Dev A** | **Dragon class**       | Dragon extends Character, basic movement, HP=140 |
| **Dev B** | GridSystem pathfinding | A* pathfinding, GetReachableTiles()              |
| **Dev C** | TurnManager basics     | InitializeTurnOrder(), turn progression          |
| **Dev D** | DiceManager testing    | `roll` command, `setseed` for reproducibility    |
| **Dev E** | GridDebugRenderer      | F1 grid overlay, tile coordinates                |

**Integration Test (Friday Week 2)**:

- Spawn Dragon on grid: `spawn dragon 4 4`
- Move Dragon: Select tile, pathfinding shows route
- End turn: `endturn` command works

---

#### Week 3: Combat + Spells Begin

| Developer | Task                    | Deliverable                                    |
| --------- | ----------------------- | ---------------------------------------------- |
| **Dev A** | Dragon Fireball spell   | Spell_Fireball(), uses DiceManager for damage  |
| **Dev B** | CombatSystem basics     | ApplyDamage(), dice-based damage calculation   |
| **Dev C** | SpellSystem foundation  | CastSpell(), spell slot management             |
| **Dev D** | **Fighter enemy class** | Fighter extends Character, HP=90, basic attack |
| **Dev E** | Combat debug tools      | `damage`, `heal`, `castspell` commands         |

**Integration Test (Friday Week 3)**:

- Dragon vs Fighter on grid
- Dragon casts Fireball: `castspell dragon fireball 1 6 4`
- Damage calculated with dice rolls
- Fighter HP decreases

---

#### Week 4: More Spells + Turn System

| Developer | Task                           | Deliverable                                       |
| --------- | ------------------------------ | ------------------------------------------------- |
| **Dev A** | Dragon CreateWall + LavaPool   | 2 more Dragon spells complete                     |
| **Dev B** | TurnManager initiative         | Initiative rolling (1d20 + speed), turn order     |
| **Dev C** | Fighter basic AI (manual mode) | Fighter can be controlled via console for testing |
| **Dev D** | DataRegistry basics            | Load dragon_stats.json, hot-reload support        |
| **Dev E** | Turn debug tools               | `showturnorder`, `endturn`, DiceHistoryPanel (F9) |

**Integration Test (Friday Week 4)**:

- Full combat loop: Dragon vs Fighter
- Initiative determines turn order
- Dragon uses 3 spells (Fireball, CreateWall, LavaPool)
- Manual control of Fighter for testing

---

#### Week 5: Polish + PLAYTEST 1 🧪

| Developer | Task                           | Deliverable                                  |
| --------- | ------------------------------ | -------------------------------------------- |
| **Dev A** | Dragon polish + balance        | All 3 spells working, balanced damage/costs  |
| **Dev B** | Combat system complete         | Damage formula finalized, death handling     |
| **Dev C** | Fighter manual control polish  | Console commands for Fighter actions         |
| **Dev D** | Data-driven Dragon stats       | Dragon stats loaded from JSON                |
| **Dev E** | **Playtest build integration** | BattleState prototype, all systems connected |

**🧪 PLAYTEST 1 (Friday Week 5)**: Dragon vs Fighter Battle

**Success Criteria**:

- ✅ Dragon spawns on 8x8 grid
- ✅ Dragon can move using pathfinding
- ✅ Dragon has 3 working spells (Fireball, CreateWall, LavaPool)
- ✅ Spells use dice-based damage
- ✅ Turn-based combat works (initiative, turn order)
- ✅ Fighter can attack back (manual control)
- ✅ Combat ends when Dragon or Fighter dies
- ✅ Debug console allows testing (`spawn`, `damage`, `castspell`)

**What We Validated**:

- Core gameplay loop works
- Dragon mechanics feel good
- Dice combat is satisfying
- Grid movement works
- Spell system is extensible

---

## 🧪 Phase 2: Full Spell Arsenal (Weeks 6-10)

**Goal**: Dragon has all 9 spells + status effects working

**Target**: PLAYTEST 2 - Dragon vs Fighter (Full Combat System)

**Foundation Built (Week 5)**: Dragon playable, 3 spells, turn-based combat, Fighter manual control

**This Phase Adds**:

- 6 more Dragon spells (total 9)
- Status effect system (Fear, Burn, Stun, Curse)
- AI foundation (manual control → basic AI)
- Data-driven systems (hot reload, JSON configuration)
- Advanced debug tools (F4, F7, F12)

---

### Week 6: Dragon Spells Expansion

| Developer | Task                                                   | Deliverable                                          |
| --------- | ------------------------------------------------------ | ---------------------------------------------------- |
| **Dev A** | Dragon spells 4-6 (DragonRoar, TailSwipe, DragonWrath) | 3 additional spells, spell slot integration          |
| **Dev B** | StatusEffectManager GSComponent foundation             | Track Fear, Burn, Stun, Curse effects                |
| **Dev C** | AIDirector singleton basics                            | Threat table structure, difficulty scaling framework |
| **Dev D** | SpellFactory foundation                                | Load spell definitions from spells.json              |
| **Dev E** | CombatFormulaInspector (F12)                           | Debug tool showing damage calculations in real-time  |

**Integration Test (Friday Week 6)**:

- Dragon casts DragonRoar → applies Fear to enemies
- StatusEffectManager tracks Fear duration
- CombatFormulaInspector shows dice rolls

---

### Week 7: Advanced Dragon Spells + Status Effects

| Developer | Task                                                      | Deliverable                                |
| --------- | --------------------------------------------------------- | ------------------------------------------ |
| **Dev A** | Dragon spells 7-9 (MeteorStrike, HeatAbsorb, HeatRelease) | All 9 Dragon spells complete               |
| **Dev B** | StatusEffectManager integration                           | Effects modify stats (Fear → -2 attack)    |
| **Dev C** | AIDirector threat calculation                             | Calculate threat values for all characters |
| **Dev D** | EffectFactory foundation                                  | Create visual effects for spells           |
| **Dev E** | StatusInfoOverlay (F7)                                    | Display active effects, HP/AP bars         |

**Integration Test (Friday Week 7)**:

- Dragon has all 9 spells functional
- Status effects modify combat (Fear reduces attack)
- F7 shows all character stats + active effects

---

### Week 8: Fighter Character + AI Foundation

| Developer | Task                            | Deliverable                               |
| --------- | ------------------------------- | ----------------------------------------- |
| **Dev A** | Fighter character class         | Fighter extends Character, HP=90, speed 3 |
| **Dev B** | Fighter combat abilities        | Melee attack, Shield Bash, Taunt          |
| **Dev C** | AISystem GSComponent foundation | AI decision loop, action selection        |
| **Dev D** | CharacterFactory foundation     | Spawn characters from JSON data           |
| **Dev E** | AIDebugVisualizer (F4) basics   | Show AI decision reasoning                |

**Integration Test (Friday Week 8)**:

- Fighter spawns via CharacterFactory
- Fighter controlled manually via console
- Fighter uses all 3 combat abilities

---

### Week 9: Fighter AI + DataRegistry

| Developer | Task                              | Deliverable                             |
| --------- | --------------------------------- | --------------------------------------- |
| **Dev A** | Fighter AI behavior (manual mode) | Console-controlled Fighter for testing  |
| **Dev B** | GridSystem LOS (line of sight)    | Calculate visibility for ranged attacks |
| **Dev C** | AISystem basic decision tree      | Fighter prioritizes nearest enemy       |
| **Dev D** | DataRegistry hot-reload system    | Watch JSON files, reload on change      |
| **Dev E** | HotReloadManager integration      | Trigger reloads, log changes            |

**Integration Test (Friday Week 9)**:

- Fighter AI makes basic decisions (move toward Dragon)
- Modify characters.json → hot reload updates stats
- F4 shows Fighter AI path + reasoning

---

### Week 10: Polish + PLAYTEST 2 🧪

| Developer | Task                           | Deliverable                                   |
| --------- | ------------------------------ | --------------------------------------------- |
| **Dev A** | Dragon spell balance tuning    | Adjust damage, costs, ranges                  |
| **Dev B** | Combat system polish           | Finalize damage formulas, status interactions |
| **Dev C** | Fighter AI polish              | Improve decision-making, edge cases           |
| **Dev D** | Data-driven spell system       | All spells load from spells.json              |
| **Dev E** | Integration build for playtest | BattleState with all systems connected        |

**🧪 PLAYTEST 2 (Friday Week 10)**: Dragon vs Fighter (Full Spell Arsenal)

**Success Criteria**:

- ✅ Dragon has all 9 spells working
- ✅ Fighter AI engages Dragon (manual control optional)
- ✅ Status effects apply correctly (Fear, Burn)
- ✅ Hot reload works for characters.json, spells.json
- ✅ Debug tools functional (F1, F4, F7, F12)

**What We Validated**:

- Dragon spell variety is engaging
- Status effects add tactical depth
- Data-driven design enables rapid balancing
- AI foundation works (even if basic)
- Debug tools accelerate testing

---

## 🧪 Phase 3: All Characters (Weeks 11-15)

**Goal**: All 5 characters exist with basic AI

**Target**: PLAYTEST 3 - Dragon vs 4 AI Characters (Full Team Battle)

**Phase 2 Complete**: Dragon has 9 spells, status effects working, Fighter AI functional

**This Phase Adds**:

- 3 more characters (Cleric, Wizard, Rogue)
- All GOComponents (GridPosition, ActionPoints, SpellSlots, StatusEffects, AIMemory, DamageCalculator, TargetingSystem)
- Basic AI for all 4 enemy characters
- CharacterFactory (data-driven character creation)
- Multi-character battle support

---

### Week 11: Cleric Character + Healing System

| Developer | Task                    | Deliverable                                 |
| --------- | ----------------------- | ------------------------------------------- |
| **Dev A** | Cleric character class  | Cleric extends Character, HP=90, speed 2    |
| **Dev B** | Healing spell system    | Healing Word, Cure Wounds, Mass Healing     |
| **Dev C** | AISystem ally awareness | AI tracks ally HP, positioning              |
| **Dev D** | SpellSlots GOComponent  | Track spell slots per character (upcasting) |
| **Dev E** | Spell selection UI      | Show available spells, slot consumption     |

**Integration Test (Friday Week 11)**:

- Cleric spawns and has 3 healing spells
- Cleric manually casts Healing Word on Fighter
- SpellSlots tracks consumption correctly

---

### Week 12: Wizard Character + Offensive Magic

| Developer | Task                         | Deliverable                              |
| --------- | ---------------------------- | ---------------------------------------- |
| **Dev A** | Wizard character class       | Wizard extends Character, HP=55, speed 1 |
| **Dev B** | Wizard offensive spells      | Magic Missile, Fireball, Lightning Bolt  |
| **Dev C** | AISystem threat assessment   | AI evaluates Dragon threat level         |
| **Dev D** | TargetingSystem GOComponent  | Validate spell ranges, LOS               |
| **Dev E** | DiceHistoryPanel (F9) polish | Show last 20 dice rolls with context     |

**Integration Test (Friday Week 12)**:

- Wizard casts Magic Missile at Dragon
- TargetingSystem validates range/LOS
- F9 shows Wizard's spell damage rolls

---

### Week 13: Rogue Character + Mobility

| Developer | Task                          | Deliverable                               |
| --------- | ----------------------------- | ----------------------------------------- |
| **Dev A** | Rogue character class         | Rogue extends Character, HP=65, speed 4   |
| **Dev B** | Rogue mobility abilities      | Dash, Disengage, Hide                     |
| **Dev C** | AIMemory GOComponent          | AI tracks last seen positions, threats    |
| **Dev D** | DamageCalculator GOComponent  | Dice-based damage with modifiers          |
| **Dev E** | GridDebugRenderer polish (F1) | Show movement/attack ranges per character |

**Integration Test (Friday Week 13)**:

- Rogue uses Dash (double movement)
- AIMemory stores tactical decisions
- F1 shows Rogue's 4-tile movement range

---

### Week 14: All Characters Integration

| Developer | Task                                | Deliverable                               |
| --------- | ----------------------------------- | ----------------------------------------- |
| **Dev A** | Character balance tuning            | Adjust HP, speed, damage for all 5        |
| **Dev B** | GridPosition GOComponent            | Tile-based positioning for all characters |
| **Dev C** | ActionPoints GOComponent            | AP refresh per turn, action costs         |
| **Dev D** | CharacterFactory complete           | Spawn all 5 characters from JSON          |
| **Dev E** | BattleState multi-character support | Handle 5 characters in turn order         |

**Integration Test (Friday Week 14)**:

- Spawn all 5 characters: Dragon, Fighter, Cleric, Wizard, Rogue
- All characters take turns correctly
- CharacterFactory creates characters from JSON data

---

### Week 15: Basic AI for All + PLAYTEST 3 🧪

| Developer | Task                           | Deliverable                              |
| --------- | ------------------------------ | ---------------------------------------- |
| **Dev A** | Cleric AI (healing priority)   | Heal lowest HP ally                      |
| **Dev B** | Wizard AI (kiting behavior)    | Keep distance from Dragon                |
| **Dev C** | Rogue AI (flanking logic)      | Move behind Dragon for attacks           |
| **Dev D** | AIBehaviorFactory foundation   | Load AI behaviors from ai_behaviors.json |
| **Dev E** | Integration build for playtest | All 5 characters with basic AI           |

**🧪 PLAYTEST 3 (Friday Week 15)**: Dragon vs 4 AI Characters

**Success Criteria**:

- ✅ All 5 characters exist and take turns
- ✅ Each AI character makes basic decisions
- ✅ Cleric heals allies, Wizard keeps distance, Rogue flanks
- ✅ Combat ends with victory/defeat
- ✅ No crashes during full battle

**What We Validated**:

- 1v4 battle is achievable and fun
- Each character feels unique (role differentiation)
- AI makes sensible basic decisions
- CharacterFactory enables rapid iteration
- Turn-based combat scales to 5 characters

---

## 🧪 Phase 4: Advanced AI & Team Tactics (Weeks 16-20)

**Goal**: AI demonstrates intelligent team coordination

**Target**: PLAYTEST 4 - Dragon vs 4 Advanced AI (Full Tactical Battle)

**Phase 3 Complete**: All 5 characters, basic AI, GOComponents, CharacterFactory

**This Phase Adds**:

- Advanced AI behaviors (protect allies, focus fire, kiting)
- AIDirector team coordination
- AIBehaviorFactory (JSON-driven AI)
- Status effect expansion (more interactions)
- Spell system polish (upcasting, AOE)
- Event-driven AI communication

---

### Week 16: AI Team Coordination

| Developer | Task                                 | Deliverable                         |
| --------- | ------------------------------------ | ----------------------------------- |
| **Dev A** | Fighter AI advanced (protect allies) | Position between Dragon and Cleric  |
| **Dev B** | CombatSystem knockback/push          | Attacks can reposition enemies      |
| **Dev C** | AIDirector team tactics              | Coordinate 4 AI characters          |
| **Dev D** | Event-driven AI communication        | AI reacts to CharacterDamagedEvent  |
| **Dev E** | AI threat visualization (F4)         | Show threat values above characters |

**Integration Test (Friday Week 16)**:

- Fighter protects Cleric from Dragon
- AIDirector coordinates team focus fire
- F4 shows threat calculations

---

### Week 17: Advanced AI Behaviors

| Developer | Task                             | Deliverable                               |
| --------- | -------------------------------- | ----------------------------------------- |
| **Dev A** | Cleric AI (buff priority)        | Apply Blessing before healing             |
| **Dev B** | Wizard AI (mana management)      | Conserve spell slots for critical moments |
| **Dev C** | Rogue AI (stealth mechanics)     | Hide when low HP                          |
| **Dev D** | AIBehaviorFactory behavior trees | JSON-defined decision trees               |
| **Dev E** | EventBusTracer (Ctrl+E)          | Show all events in real-time              |

**Integration Test (Friday Week 17)**:

- AI uses complex behaviors (Cleric buffs before healing)
- Wizard conserves high-level spells
- Ctrl+E shows event flow during combat

---

### Week 18: Status Effect Expansion

| Developer | Task                               | Deliverable                                  |
| --------- | ---------------------------------- | -------------------------------------------- |
| **Dev A** | StatusEffects GOComponent complete | Track multiple effects per character         |
| **Dev B** | Status effect interactions         | Fear prevents movement, Burn deals damage    |
| **Dev C** | AISystem status awareness          | AI avoids lava, prioritizes removing debuffs |
| **Dev D** | EffectFactory expand               | Create Blessing, Curse, Poison effects       |
| **Dev E** | StatusInfoOverlay badges           | Show effect icons on character portraits     |

**Integration Test (Friday Week 18)**:

- Characters have multiple status effects simultaneously
- Fear prevents movement, Burn ticks damage
- AI reacts to status effects (avoid lava tiles)

---

### Week 19: Spell System Polish

| Developer | Task                           | Deliverable                                           |
| --------- | ------------------------------ | ----------------------------------------------------- |
| **Dev A** | Spell upcasting system         | Higher-level slots increase spell power               |
| **Dev B** | Spell area of effect (AOE)     | Fireball hits 3x3 area                                |
| **Dev C** | Spell validation               | Check range, LOS, AP, spell slots                     |
| **Dev D** | SpellFactory complete          | All spells data-driven from JSON                      |
| **Dev E** | Spell selection debug commands | `castspell dragon fireball 3 5 5` (upcast to level 3) |

**Integration Test (Friday Week 19)**:

- Dragon upcasts Fireball (level 1 → level 3)
- AOE spells hit multiple targets
- Spell validation prevents invalid casts

---

### Week 20: Full Battle Integration + PLAYTEST 4 🧪

| Developer | Task                           | Deliverable                         |
| --------- | ------------------------------ | ----------------------------------- |
| **Dev A** | Character ability polish       | All 20+ abilities working           |
| **Dev B** | Combat system complete         | All damage types, resistances       |
| **Dev C** | AISystem advanced tactics      | Focus fire, protect healers, kiting |
| **Dev D** | DataRegistry complete          | Hot reload for all JSON files       |
| **Dev E** | Integration build for playtest | Full 1v4 battle ready               |

**🧪 PLAYTEST 4 (Friday Week 20)**: Dragon vs 4 Advanced AI

**Success Criteria**:

- ✅ Complete 1v4 battle from start to finish
- ✅ AI demonstrates team coordination
- ✅ All spells, abilities, status effects work
- ✅ Combat is balanced and engaging
- ✅ No major bugs or crashes

**What We Validated**:

- AI feels intelligent and challenging
- Team tactics make combat dynamic
- Battle is balanced (Dragon can win/lose)
- All systems work together seamlessly
- Game is fundamentally fun

---

## 🧪 Phase 5: Visual Polish & Production Quality (Weeks 21-24)

**Goal**: Game looks and performs at production quality

**Target**: PLAYTEST 5 - Visual Polish + Performance

**Phase 4 Complete**: Full 1v4 battle with advanced AI, all systems functional

**This Phase Adds**:

- Character animations (idle, attack, cast, hurt, death)
- Spell visual effects (particles, explosions, beams)
- UI polish (HP bars, spell indicators, menus)
- Campaign & save system
- Performance optimization
- Map visual design

---

### Week 21: Visual Effects Foundation

| Developer | Task                          | Deliverable                                          |
| --------- | ----------------------------- | ---------------------------------------------------- |
| **Dev A** | Character animations          | Idle, attack, cast, hurt, death animations           |
| **Dev B** | Spell visual effects          | Fireball explosion, Lightning bolt, Healing sparkles |
| **Dev C** | EffectManager particle system | Reusable particle effects                            |
| **Dev D** | MapFactory visual elements    | Terrain sprites, obstacles                           |
| **Dev E** | UI polish                     | HP bars, spell slot indicators, turn order display   |

**Integration Test (Friday Week 21)**:

- Characters have animations for all actions
- Spells show visual effects
- UI displays all combat info clearly

---

### Week 22: Campaign & Save System

| Developer | Task                             | Deliverable                                   |
| --------- | -------------------------------- | --------------------------------------------- |
| **Dev A** | Campaign progression             | 3 battle scenarios with increasing difficulty |
| **Dev B** | BattleManager victory conditions | Win/loss detection, score calculation         |
| **Dev C** | SaveManager save/load            | Serialize CampaignData to JSON                |
| **Dev D** | Campaign data structure          | Track progress, unlocks, stats                |
| **Dev E** | Save/load debug commands         | `save slot1`, `load slot1`, `showsave`        |

**Integration Test (Friday Week 22)**:

- Win battle → save game → load → progress persists
- SaveManager serializes all game state correctly

---

### Week 23: Visual Polish

| Developer | Task                    | Deliverable                               |
| --------- | ----------------------- | ----------------------------------------- |
| **Dev A** | Character sprite polish | High-quality sprites for all 5 characters |
| **Dev B** | Spell effect polish     | Smooth animations, particle effects       |
| **Dev C** | UI visual design        | Polished menus, buttons, overlays         |
| **Dev D** | Map visual polish       | Detailed terrain, atmospheric effects     |
| **Dev E** | Camera effects          | Screen shake, zoom, focus effects         |

**Integration Test (Friday Week 23)**:

- Game looks visually complete
- All placeholders replaced with final art
- Camera effects enhance combat feel

---

### Week 24: Performance + PLAYTEST 5 🧪

| Developer | Task                        | Deliverable                                 |
| --------- | --------------------------- | ------------------------------------------- |
| **Dev A** | Character optimization      | Reduce memory usage, improve state machines |
| **Dev B** | GridSystem optimization     | Pathfinding caching, LOS optimization       |
| **Dev C** | AI performance tuning       | Reduce computation time per turn            |
| **Dev D** | Asset loading optimization  | Lazy loading, texture atlases               |
| **Dev E** | Performance profiling tools | Frame rate monitor, memory tracker          |

**🧪 PLAYTEST 5 (Friday Week 24)**: Visual Polish + Performance

**Success Criteria**:

- ✅ Game runs at 60 FPS consistently
- ✅ All visual effects and animations complete
- ✅ UI is polished and intuitive
- ✅ Save/load works reliably
- ✅ No performance issues during battles

**What We Validated**:

- Game looks professional and polished
- Animations enhance combat feel
- UI communicates all information clearly
- Campaign progression works
- Performance is smooth and responsive

---

## 🧪 Phase 6: Final Polish & Release (Weeks 25-26)

**Goal**: Ship-ready game with final bug fixes and balance

**Target**: PLAYTEST 6 - Final Release Candidate

**Phase 5 Complete**: Fully polished visuals, campaign mode, optimized performance

**This Phase Adds**:

- Final bug fixes from all playtests
- Balance tuning based on feedback
- AI polish (fix edge cases, improve decisions)
- Data validation (all JSON files clean)
- Release build preparation

---

### Week 25: Final Integration

| Developer | Task                             | Deliverable                                 |
| --------- | -------------------------------- | ------------------------------------------- |
| **Dev A** | Character final balance          | Tune all stats, abilities for fair gameplay |
| **Dev B** | Combat final polish              | Edge case fixes, formula refinement         |
| **Dev C** | AI final tuning                  | Difficulty scaling, smart behaviors         |
| **Dev D** | Data validation                  | Ensure all JSON files valid, complete       |
| **Dev E** | BattleState complete integration | All systems working together seamlessly     |

**Integration Test (Friday Week 25)**:

- Play 3 consecutive battles without issues
- All systems integrated perfectly
- Game feels complete and polished

---

### Week 26: Final Polish + PLAYTEST 6 🧪

| Developer | Task                     | Deliverable                             |
| --------- | ------------------------ | --------------------------------------- |
| **Dev A** | Bug fixes from playtests | Fix all critical/major bugs             |
| **Dev B** | Balance final tuning     | Adjust difficulty based on feedback     |
| **Dev C** | AI final polish          | Fix any dumb AI decisions               |
| **Dev D** | Data final polish        | Clean up JSON files, validate all data  |
| **Dev E** | Release build            | Create final executable, package assets |

**🧪 PLAYTEST 6 (Friday Week 26)**: Final Release Playtest

**Success Criteria**:

- ✅ Complete campaign (3 battles) without crashes
- ✅ Balanced difficulty (challenging but fair)
- ✅ All features functional
- ✅ Professional polish and presentation
- ✅ Ready for release 🎉

**What We Validated**:

- Game is stable and bug-free
- Difficulty is tuned for target audience
- All features work as designed
- Presentation is professional quality
- Game is ready for players

**Final Milestone**: Dragonic Tactics 1.0 Complete! 🎉

---

## Integration Strategy (Post Week 5)

**Foundation Built (Week 5)**: Character, GridSystem, EventBus, DiceManager, TurnManager, SpellSystem (3 spells), DebugConsole

**Integration Philosophy**: Build on solid foundation, add systems incrementally

### Key Integration Points

**Week 8**: SpellSystem expansion

- Add remaining 6 Dragon spells using existing SpellSystem framework
- Test: All 9 spells castable with proper validation

**Week 10**: StatusEffectManager integration

- Connect to EventBus for status effect events
- Test: Spells apply status effects (Dragon Roar → Fear)

**Week 12**: CharacterFactory + DataRegistry

- Characters created from JSON instead of hardcoded
- Test: Spawn characters with `spawn fighter 4 4` console command

**Week 15**: AISystem + All Characters

- AI takes control of enemy characters during their turns
- Test: Enemies make decisions without player input

**Week 20**: Full Battle Integration (PLAYTEST 2)

- All systems working together (combat, AI, spells, turns, grid)
- Test: Complete 1v4 battle from start to victory/defeat

**Week 25**: BattleState Complete Integration

- All visual effects, UI, animations, save/load
- Test: Full game experience with polish

---

## EventBus Integration Strategy

**Philosophy**: EventBus is the communication backbone that decouples all systems (see [systems/eventbus.md](systems/eventbus.md))

### Week 1: EventBus Foundation

EventBus is built in Week 1 alongside other core systems, enabling event-driven development from the start:

**Week 1**: Basic event framework

- EventBus singleton with publish/subscribe
- Define initial event types: `CharacterDamagedEvent`, `TurnStartedEvent`, `TurnEndedEvent`, `SpellCastEvent`
- All developers can publish/subscribe immediately

**Week 2-3**: Core event integration

- Character publishes `CharacterDamagedEvent` when HP changes
- TurnManager publishes `TurnStartedEvent`, `TurnEndedEvent`
- SpellSystem publishes `SpellCastEvent` for visual effects
- DebugConsole subscribes to all events for logging

**Week 4-5**: Battle event flow (for Playtest 1)

- GridSystem publishes `CharacterMovedEvent`
- CombatSystem publishes damage and death events
- EventBusTracer debug tool shows event flow
- All core systems communicate via events

**Week 6+**: Expand event types

- Add AI decision events, status effect events, UI events
- Advanced debug tools (EventBusTracer filtering)
- Full event-driven architecture across all systems

### Event Publishing Guidelines

**Pattern**: Publish events AFTER state changes, never before

```cpp
// ✅ CORRECT: Publish after state change
void Character::TakeDamage(int damage) {
    int oldHP = currentHP;
    currentHP -= damage;
    if (currentHP < 0) currentHP = 0;

    // State changed, NOW publish
    EventBus::Instance().Publish(CharacterDamagedEvent{this, damage, currentHP, attacker, false});
}

// ❌ WRONG: Publish before state change
void Character::TakeDamage(int damage) {
    EventBus::Instance().Publish(...);  // State not changed yet!
    currentHP -= damage;
}
```

### Event Subscription Guidelines

**Pattern**: Subscribe in `Init()`, never in constructor

```cpp
// ✅ CORRECT: Subscribe in Init()
class BattleUI : public CS230::Component {
public:
    void Init() override {
        EventBus::Instance().Subscribe<CharacterDamagedEvent>(
            [this](const CharacterDamagedEvent& e) { OnDamaged(e); }
        );
    }
};

// ❌ WRONG: Subscribe in constructor (Init() may not be called yet)
class BattleUI : public CS230::Component {
public:
    BattleUI() {
        EventBus::Instance().Subscribe<CharacterDamagedEvent>(...);  // Too early!
    }
};
```

### Event-Driven Testing

**Benefit**: Test systems in isolation by mocking events

```cpp
// Test CombatSystem without real Characters
TEST(CombatSystem, DamageCalculation) {
    // Create stub character
    Character* stubTarget = new StubCharacter();

    // Subscribe to damage events
    int damageReceived = 0;
    EventBus::Instance().Subscribe<CharacterDamagedEvent>(
        [&](const CharacterDamagedEvent& e) {
            damageReceived = e.damageAmount;
        }
    );

    // Trigger combat
    CombatSystem::Instance().CalculateDamage(attacker, stubTarget, AttackType::Melee);

    // Verify event was published
    ASSERT_EQ(damageReceived, 10);  // Expected damage
}
```

---

## Milestone Tracking (Week 1-5 Foundation)

### Critical Path Milestones

These are the "make or break" milestones that must hit on time:

| Week   | Milestone                          | Blocker For              | Status |
| ------ | ---------------------------------- | ------------------------ | ------ |
| **1**  | Foundation systems complete        | All Week 2+ work         | ☐      |
| **3**  | Character + GridSystem working     | Dragon movement          | ☐      |
| **4**  | TurnManager + SpellSystem ready    | Turn-based spell casting | ☐      |
| **5**  | **PLAYTEST 1: Dragon playable** 🧪 | Validate game design     | ☐      |
| **10** | Dragon all 9 spells + Fighter      | Enemy combat testing     | ☐      |
| **15** | All 5 characters + basic AI        | Team combat              | ☐      |
| **20** | **PLAYTEST 2: Full 1v4 battle** 🧪 | Validate AI & balance    | ☐      |
| **24** | **PLAYTEST 3: Visual polish** 🧪   | Final iteration          | ☐      |
| **26** | **Project complete** 🎉            | Delivery                 | ☐      |

### Feature Completion Checklist

**Week 1-5: Foundation (PLAYTEST 1)**:

- ☐ Character base class with HP, movement, actions
- ☐ Dragon class with 3 spells (Fireball, CreateWall, LavaPool)
- ☐ GridSystem with 8x8 grid, pathfinding, LOS
- ☐ EventBus pub/sub system
- ☐ DiceManager with dice rolling and string parsing
- ☐ TurnManager with initiative and turn order
- ☐ SpellSystem with spell slot management
- ☐ DebugConsole with basic commands (`spawn`, `damage`, `castspell`)
- ☐ GridDebugRenderer (F1) for grid visualization
- ☐ BattleState minimal framework

**Week 6-10: Spell Expansion**:

- ☐ Dragon spells 4-9 (DragonRoar, TailSwipe, DragonWrath, MeteorStrike, HeatAbsorb, HeatRelease)
- ☐ Fighter character with basic combat
- ☐ StatusEffectManager for buffs/debuffs
- ☐ CombatFormulaInspector (F12) for damage debugging

**Week 11-15: Character Expansion**:

- ☐ Cleric, Wizard, Rogue characters
- ☐ GOComponents (GridPosition, ActionPoints, SpellSlots, StatusEffects, DamageCalculator, TargetingSystem, AIMemory)
- ☐ CharacterFactory for data-driven character creation
- ☐ DataRegistry with hot-reload
- ☐ AISystem framework

**Week 16-20: AI Intelligence (PLAYTEST 2)**:

- ☐ Fighter AI (tank logic)
- ☐ Cleric AI (healing priority)
- ☐ Wizard AI (kiting)
- ☐ Rogue AI (flanking)
- ☐ AIBehaviorFactory for JSON-driven AI
- ☐ AIDebugVisualizer (F4) for AI debugging

**Week 21-24: Visual Polish (PLAYTEST 3)**:

- ☐ EffectManager for particle effects and animations
- ☐ MapFactory for JSON-based maps
- ☐ SaveManager for campaign progress
- ☐ BattleManager for victory conditions
- ☐ All visual effects implemented

**Week 25-26: Final Integration**:

- ☐ BattleState complete integration
- ☐ All debug tools functional (11 visual tools, 50+ commands)
- ☐ Bug fixes and balance tuning
- ☐ Performance optimization
- ☐ Final playtesting and polish

---

## Risk Management

### High-Risk Dependencies

These are critical dependencies that could block progress:

| Risk                             | Impact                          | Mitigation                                                                        | Timeline   |
| -------------------------------- | ------------------------------- | --------------------------------------------------------------------------------- | ---------- |
| **Week 1 foundation incomplete** | Blocks all Week 2+ work         | All 5 developers focus on Week 1 critical path, daily standups, pair programming  | Week 1     |
| **Integration issues Week 3-4**  | Dragon can't move/cast spells   | Continuous integration testing, debug tools built alongside systems               | Week 3-4   |
| **Playtest 1 fails (Week 5)**    | Game design validation delayed  | Build minimal viable features first, cut scope if needed (fewer spells initially) | Week 5     |
| **AI systems delayed**           | Can't test 1v4 battles          | Fighter with manual control first (Week 10), gradual AI rollout                   | Week 10-20 |
| **Performance issues late**      | Frame rate drops, memory leaks  | Profile early (Week 10), optimize incrementally, use debug tools to monitor       | Week 10-26 |
| **Scope creep**                  | Features expand beyond 26 weeks | Strict feature freeze after Week 20, playtests validate scope cuts                | Week 20+   |

### Sync Meetings & Playtesting Schedule

**Strategy**: Frequent integration and playtesting to catch issues early

---

#### Daily Standups (5-10 minutes) - Week 1-5 CRITICAL

**Purpose**: Quick synchronization during foundation phase (Week 1-5)

**Frequency**: Daily (especially critical during Week 1-5)

**Format**:

- Each dev: "Yesterday I completed X, today I'm working on Y, blocked by Z"
- Quick problem-solving for blockers (< 2 min per issue)
- Week 1: Extra focus on coordination (all 5 devs building foundation together)

**Example Daily Standup (Week 3, Day 2)**:

- Dev A: "Character movement working. Integrating with GridSystem today. Need GridSystem API clarification."
- Dev B: "GridSystem pathfinding done. Testing with Character today. Will clarify API with Dev A after standup."
- Dev C: "EventBus pub/sub working. Adding event type definitions. No blockers."
- Dev D: "DiceManager complete. Writing unit tests. No blockers."
- Dev E: "DebugConsole command registration working. Adding `spawn` and `damage` commands. No blockers."
- **Quick Action**: Dev A and Dev B pair program on GridSystem integration for 30 min after standup

---

#### Weekly Integration Meetings (45 minutes)

**Purpose**: Coordinate system integration and resolve issues

**Frequency**: Weekly (every Friday)

**Agenda**:

1. Review this week's completions (10 min)
2. Demo new features (15 min - each dev shows progress)
3. Plan next week's integration points (10 min)
4. Identify integration risks (5 min)
5. Assign integration tasks (5 min)

**Example Week 4 Integration Meeting** (preparing for Playtest 1):

**Completions**:

- Character base class with movement ✅
- GridSystem with pathfinding and LOS ✅
- EventBus fully functional ✅
- DiceManager with string parsing ✅
- TurnManager with initiative system ✅
- SpellSystem framework (50% complete) ⚠️

**Demos**:

- Dev A: Dragon moves on grid using pathfinding (3 min)
- Dev B: GridDebugRenderer shows reachable tiles (3 min)
- Dev C: EventBus logs all game events in console (3 min)
- Dev D: DiceManager rolls "3d6" and displays results (3 min)
- Dev E: DebugConsole spawns characters with `spawn dragon 4 4` (3 min)

**Next Week Focus** (Playtest 1 Week 5):

- Finish SpellSystem (3 Dragon spells: Fireball, CreateWall, LavaPool)
- Integrate all systems in BattleState
- Fighter character with manual control for testing
- Full combat loop (Dragon vs Fighter)

**Risks**:

- SpellSystem behind schedule → All devs help finish critical spell methods
- BattleState integration unknown → Dev E starts integration Monday, pair with others as needed

---

#### Playtesting Sessions (90 minutes)

**Philosophy**: Playtest early (Week 5) to validate design before building too much

**Purpose**: Full-team playthrough to catch gameplay bugs, balance issues, integration problems

**Frequency**: Weeks 5, 10, 15, 20, 24, 26 (6 total playtests)

**Format**:

1. **Build Integration** (15 min before session): Merge all dev branches, resolve conflicts
2. **Playtest** (30 min): All 5 devs play the game, try to break it
3. **Bug Triage** (30 min): Categorize bugs (critical/major/minor), assign owners
4. **Balance Discussion** (15 min): Is Dragon too strong? Are AI opponents too easy?
5. **Action Items** (15 min): Each dev commits to fixes for next playtest

---

### 6 Mandatory Playtesting Milestones (Weeks 5, 10, 15, 20, 24, 26)

| Playtest   | Week   | Focus                  | Expected State                                                        | Success Criteria                                               |
| ---------- | ------ | ---------------------- | --------------------------------------------------------------------- | -------------------------------------------------------------- |
| **PT1** 🧪 | **5**  | Dragon Playable        | Dragon can move, cast 3 spells, turn-based combat vs Fighter (manual) | ✅ Dragon playable for 5 minutes without crash, core loop works |
| **PT2** 🧪 | **10** | All Dragon Spells      | Dragon has all 9 spells, Fighter with basic AI                        | ✅ All spells castable, Fighter manual control works            |
| **PT3** 🧪 | **15** | All Characters         | All 5 characters exist, basic AI for all enemies                      | ✅ Characters spawn correctly, basic AI functional              |
| **PT4** 🧪 | **20** | Full 1v4 AI Battle     | Dragon vs 4 AI opponents with advanced behaviors                      | ✅ Complete 1v4 battle, AI coordination visible                 |
| **PT5** 🧪 | **24** | Visual Polish          | All systems + visual effects + animations                             | ✅ Game visually complete, no placeholder graphics              |
| **PT6** 🧪 | **26** | Final Release Playtest | Campaign mode, save/load, all features complete                       | ✅ 3 consecutive battles without crash, balanced difficulty     |

---

#### Playtest 1: Week 5 - Dragon Playable

**Goal**: Verify Dragon character is playable

**Test Cases**:

1. Start battle, Dragon spawns at (4, 4)
2. Move Dragon 2 tiles (test GridPosition, ActionPoints)
3. Cast Fireball at dummy target (test SpellSystem integration)
4. Cast CreateWall (test grid obstacle creation)
5. Cast LavaPool (test AOE spell)
6. End turn (test TurnManager)

**Expected Bugs**: Pathfinding edge cases, spell validation issues, animation glitches

**Bug Assignment**: Movement → Dev 1+2, Spells → Dev 1+2, Turns → Dev 2

---

#### Playtest 2: Week 10 - All Dragon Spells

**Goal**: Verify all 9 Dragon spells work correctly

**Test Cases**:

1. Cast all 9 spells at least once
2. Verify spell slot consumption (level 1-5 slots)
3. Test spell upcasting (higher level slots)
4. Verify spell effects (damage, walls, AOE, buffs)
5. Test spell edge cases (out of range, no LOS, no AP)

**Expected Bugs**: Spell validation broken, slot tracking wrong, effects don't apply, crash on upcast

**Bug Assignment**: Spell logic → all devs (system owner), Slot tracking → Dev (SpellSystem owner)

---

#### Playtest 3: Week 15 - All Characters

**Goal**: Verify all 5 characters exist and basic AI works

**Test Cases**:

1. Spawn all 5 characters (Dragon, Fighter, Cleric, Wizard, Rogue)
2. Verify each character has correct stats (HP, speed, AP)
3. Test basic AI for each enemy (make at least 1 decision)
4. Verify turn order works with 5 characters
5. Test combat between all character types

**Expected Bugs**: Character stats wrong, AI doesn't activate, turn order broken, missing animations

**Bug Assignment**: Character stats → Dev (CharacterFactory owner), AI → Dev (AISystem owner), Turn order → Dev (TurnManager owner)

---

#### Playtest 4: Week 20 - Full 1v4 AI Battle

**Goal**: Complete Dragon vs 4 AI opponents battle

**Test Cases**:

1. Dragon vs Fighter + Cleric + Wizard + Rogue (full team)
2. Verify AI coordination (Cleric heals, Wizard kites, etc.)
3. Play battle to completion (victory or defeat)
4. Test all status effects (Fear, Burn, Blessing, Curse, etc.)
5. Verify AI uses abilities intelligently

**Expected Bugs**: No AI coordination, AI makes dumb decisions, softlocks, performance issues, status effects broken

**Bug Assignment**: AI → Dev (AI owner), Status → Dev (StatusEffectManager owner), Performance → all devs

---

#### Playtest 5: Week 24 - Visual Polish

**Goal**: Verify all visual effects, animations, and polish are complete

**Test Cases**:

1. Play Dragon vs 4 AI opponents full battle
2. Verify all spells have visual effects (particles, animations)
3. Verify all attacks have animations
4. Test all status effect badges (icons, timers)
5. Verify UI polish (HP bars, spell slot display, turn order)

**Expected Bugs**: Missing effects, placeholder graphics, animation glitches, UI alignment issues

**Bug Assignment**: Effects → Dev (EffectManager owner), Animations → Dev (Animation owner), UI → Dev (UI owner)

---

#### Playtest 6: Week 26 - Final Polish

**Goal**: Play 3 consecutive battles without crashes, verify save/load

**Test Cases**:

1. Start campaign → Battle 1 → win
2. Save game
3. Battle 2 → lose intentionally
4. Load saved game → Battle 2 → win
5. Battle 3 → win
6. Verify campaign progression (score, unlocks, etc.)

**Expected Bugs**: Save/load corruption, progression doesn't persist, balance issues, polish needed

**Bug Assignment**: Save/load → Dev 4, Balance → all devs, Polish → Dev 5+all

---

### Playtesting Best Practices

**Before Playtest** (Day of, 12pm deadline):

1. Each dev merges branch to `develop`
2. Dev 5 does integration build, resolves conflicts
3. All devs pull latest `develop` and verify local build

**During Playtest**:

1. One dev streams screen (rotate each session)
2. All devs take notes in shared doc
3. Bug format: `[Category] Description - Steps to reproduce - Severity`
4. Example: `[Combat] Dragon takes negative damage - Cast Fireball on self - Major`

**After Playtest**:

1. Triage: Critical (blocks gameplay) → Major (annoying) → Minor (polish)
2. Assign owner to each bug
3. Create GitHub issues/tickets
4. Each dev commits to fix assigned bugs by next sync

**Bug Severity**:

- **Critical**: Crash, softlock, unplayable → Fix within 1 day
- **Major**: Broken feature, balance issue → Fix within 1 week
- **Minor**: Visual glitch, typo → Fix when time permits

---

### Example Bug Triage (Week 12 Playtest)

**Bugs Found**:

1. `[Combat] Dragon deals 0 damage with 1d6+5` - **Critical**
2. `[Grid] Dragon walks through walls` - **Critical**
3. `[UI] HP bar doesn't update` - **Major**
4. `[Spell] Fireball animation plays twice` - **Minor**
5. `[AI] Fighter doesn't attack when adjacent` - **Critical**

**Assignments**:

- Bug 1 → Dev 2 + Dev 4 (fix by Tuesday)
- Bug 2 → Dev 2 (fix by Wednesday)
- Bug 3 → Dev 5 + Dev 1 (fix by Friday)
- Bug 4 → Dev 4 (fix Week 13+)
- Bug 5 → Dev 3 (fix by Monday) **CRITICAL**

**Follow-up**: Daily standups track fix progress, next Friday integration meeting verifies all Critical/Major bugs fixed

---

### Meeting Time Commitment Summary

| Meeting Type          | Frequency  | Duration | Weekly Time  |
| --------------------- | ---------- | -------- | ------------ |
| Daily Standup         | 5x/week    | 10 min   | 50 min       |
| Integration Meeting   | 1x/week    | 45 min   | 45 min       |
| Playtesting Session   | Every 2 wk | 90 min   | 45 min avg   |
| **Total Weekly Time** |            |          | **~2.5 hrs** |

**ROI**: 2.5 hours/week of meetings prevents 10+ hours/week of rework from late bug discovery

---

## Summary: Collaborative Task-Based Plan

This implementation plan enables **collaborative parallel development** for 5 developers across 26 weeks.

### Key Features

✅ **Work Together**: All 5 developers collaborate on critical path, not isolated in silos
✅ **Balanced Workload**: Similar amount of work per developer every week, no one idle
✅ **Task-Based Division**: Divide work by what needs to be done this week, not permanent roles
✅ **Foundation-First**: Core systems built together in Week 1 (Character, GridSystem, EventBus, DiceManager, TurnManager, SpellSystem, DebugConsole)
✅ **Early Playtesting**: 6 playtests throughout development (Weeks 5, 10, 15, 20, 24, 26)
✅ **Daily Standups**: Quick coordination during Week 1-5 foundation phase
✅ **Weekly Integration**: Coordinate system integration and demos every Friday
✅ **Clear Milestones**: Critical path tracked, risks identified early

### Playtest Schedule

| Week   | Playtest | Focus           | Success Criteria                                      |
| ------ | -------- | --------------- | ----------------------------------------------------- |
| **5**  | 🧪 PT1   | Dragon playable | Dragon moves, casts 3 spells, turn-based combat works |
| **10** | 🧪 PT2   | All spells      | All 9 Dragon spells functional                        |
| **15** | 🧪 PT3   | All characters  | 5 characters exist with basic AI                      |
| **20** | 🧪 PT4   | Full battle     | Complete 1v4 AI battle (Dragon vs 4 enemies)          |
| **24** | 🧪 PT5   | Visual polish   | All effects, animations, UI complete                  |
| **26** | 🧪 PT6   | Final release   | Campaign mode, save/load, balanced difficulty         |

### Time Commitment

- **Development**: ~35-38 hours/week per developer
- **Meetings**: ~2.5 hours/week (daily standups + weekly integration + playtesting)
- **Total**: ~37.5-40.5 hours/week

### Why This Works

✅ **Equal Contribution**: Everyone works similar hours with similar value contribution
✅ **Early Validation**: Dragon playable at Week 5 validates core design early
✅ **Risk Mitigation**: 6 playtests catch problems with 21 weeks remaining to fix
✅ **True Parallelism**: 5 developers build 5 systems simultaneously (Week 1)
✅ **High Efficiency**: ~90% developer productivity through balanced task distribution
