# Dragonic Tactics - Architecture Overview

**Purpose**: This document provides the high-level architecture vision for Dragonic Tactics. For detailed implementation specifications, see the [systems/](systems/) directory.

---

## Table of Contents

- [1. Project Overview](#1-project-overview)
- [2. System Architecture](#2-system-architecture)
  - [Character Hierarchy](#character-hierarchy)
  - [Singleton Services](#singleton-services)
  - [GameState Components](#gamestate-components)
  - [GameObject Components](#gameobject-components)
- [3. Architectural Patterns](#3-architectural-patterns)
  - [Interface Design](#interface-design)
  - [Factory Pattern](#factory-pattern)
  - [Component-Based Architecture](#component-based-architecture)
- [4. System Interaction](#4-system-interaction)
- [5. Architecture Evolution](#5-architecture-evolution)
- [6. Documentation Map](#6-documentation-map)

---

## 1. Project Overview

**Game Name**: Dragonic Tactics
**Genre**: Turn-based Tactical RPG
**Engine**: CS230 Custom Engine (Extended)
**Architecture Principles**: Modular, Data-Driven, Component-Based Design
**Development Timeline**: 26 weeks (5 parallel development tracks)

### Core Game Concept

- **Player**: Controls a Dragon (140 HP, speed 5) with powerful spell-casting abilities
- **Opponents**: 4 AI characters with distinct tactical roles:
  - **Fighter**: Melee tank (90 HP, speed 3)
  - **Cleric**: Support healer (90 HP, speed 2)
  - **Wizard**: Glass cannon (55 HP, speed 1, long range)
  - **Rogue**: Agile assassin (65 HP, speed 4, stealth)
- **Battlefield**: 8x8 grid with dice-based combat (D&D style: 3d6, 2d8, etc.)
- **Progression**: Campaign mode with save/load and character advancement

---

## 2. System Architecture

### Character Hierarchy

**Design Principle**: All characters extend `CS230::GameObject` following the existing engine pattern (see Cat.h/Cat.cpp).

**Base Character Class**: Provides common interface for all battlefield entities

- Combat capabilities (attack, defend, take damage)
- Turn-based actions (action points, movement range)
- Grid positioning and targeting
- Status effect management

**Derived Classes**:

- `Dragon` (player character)
- `Fighter`, `Cleric`, `Wizard`, `Rogue` (AI characters)

Each character type specializes the base interface with unique:

- Stats (HP, speed, attack, defense)
- Abilities (spells, attacks, special actions)
- AI behavior (for opponent characters)

**📄 Detailed Specification**: [systems/characters.md](systems/characters.md)

---

### Singleton Services

**Design Principle**: Global services provide centralized functionality accessible throughout the application. Each service is independent and testable.

**Existing CS230 Services**:

- `Logger`: Debug logging and error tracking
- `Input`: Keyboard/mouse input handling
- `TextureManager`: Asset loading and memory management
- `GameStateManager`: State transitions and lifecycle

**🆕 New Tactical RPG Services**:

| Service           | Purpose                    | Key Capability                          |
| ----------------- | -------------------------- | --------------------------------------- |
| **EventBus**      | Inter-system communication | Decoupled event publishing/subscription |
| **DiceManager**   | Random number generation   | Seeded, debuggable dice rolls           |
| **DataRegistry**  | JSON data management       | Hot-reloadable external configuration   |
| **AIDirector**    | Global AI coordination     | Team tactics and difficulty scaling     |
| **EffectManager** | Visual effects             | Spell animations and combat feedback    |
| **SaveManager**   | Campaign persistence       | Save/load game state                    |

**Why Singletons?**

- Centralized access point for shared resources
- Prevent duplicate system instances
- Enable loose coupling through service locator pattern

**📄 Detailed Specification**: [systems/singletons.md](systems/singletons.md)

---

### GameState Components

**Design Principle**: Battle-wide systems that exist at the level of a single game state (e.g., one battle). Added via `AddGSComponent()`.

**Existing CS230 GSComponents**:

- `Camera`: Viewport management
- `GameObjectManager`: Entity lifecycle management
- `Timer`: Time-based events
- `ParticleManager`: Particle effects
- `ShowCollision`: Debug visualization

**🆕 Tactical Battle GSComponents**:

| Component               | Role in Battle                                         |
| ----------------------- | ------------------------------------------------------ |
| **BattleManager**       | Victory conditions, battle flow coordination           |
| **TurnManager**         | Initiative order, action point allocation              |
| **GridSystem**          | 8x8 tile validation, pathfinding, line of sight        |
| **CombatSystem**        | Damage calculation, attack resolution, knockback       |
| **SpellSystem**         | Spell casting, effects, slot management, upcasting     |
| **AISystem**            | AI decision-making, team coordination, difficulty      |
| **StatusEffectManager** | Buff/debuff tracking, stat modification, effect timers |

**How They Interact**:

1. `TurnManager` determines whose turn it is
2. `StatusEffectManager` applies start-of-turn effects (burn, fear, etc.)
3. `AISystem` or player input decides action
4. `GridSystem` validates movement/targeting
5. `CombatSystem`/`SpellSystem` resolves action
6. `StatusEffectManager` modifies damage based on buffs/debuffs
7. `BattleManager` checks victory conditions

**📄 Detailed Specification**: [systems/game-state-components.md](systems/game-state-components.md)

---

### GameObject Components

**Design Principle**: Individual entity behaviors attached to specific GameObjects using `AddGOComponent()`. Enables modular, reusable functionality.

**Existing CS230 GOComponents**:

- `Sprite`: 2D rendering and animation
- `RectCollision`: Collision detection
- `Animation`: Frame-based sprite animation

**🆕 Tactical Character GOComponents**:

| Component            | Attached To            | Purpose                                   |
| -------------------- | ---------------------- | ----------------------------------------- |
| **GridPosition**     | All characters         | Tile-based positioning, pathfinding cache |
| **ActionPoints**     | All characters         | Turn-based action economy                 |
| **SpellSlots**       | Dragon, Wizard, Cleric | Magic resource tracking with upcasting    |
| **StatusEffects**    | All characters         | Individual buff/debuff collection         |
| **AIMemory**         | AI characters          | Tactical decision history                 |
| **DamageCalculator** | All characters         | Dice-based combat resolution              |
| **TargetingSystem**  | All characters         | Attack/spell range validation             |

**Why Components?**

- Composition over inheritance for flexible behavior
- Easy to add/remove capabilities at runtime
- Each component has single responsibility

**📄 Detailed Specification**: [systems/game-object-components.md](systems/game-object-components.md)

---

## 3. Architectural Patterns

### Pure GameObject Inheritance Pattern

**Design Decision**: Following CS230 engine patterns, Dragonic Tactics uses **pure GameObject inheritance WITHOUT interface classes**.

**Character Hierarchy**:

```cpp
CS230::GameObject (engine base class)
└── Character (tactical RPG base class)
    ├── Dragon (player character)
    ├── Fighter (AI tank)
    ├── Cleric (AI support)
    ├── Wizard (AI glass cannon)
    └── Rogue (AI assassin)
```

**NO Interface Inheritance**: This matches existing CS230 code patterns like `Cat.h`:

```cpp
class Cat : public CS230::GameObject { ... }
class Character : public CS230::GameObject { ... }
class Dragon : public Character { ... }
```

**Benefits**:

- **Consistency**: Matches existing CS230 engine patterns
- **Simplicity**: No abstract interface layer to maintain
- **Polymorphism**: Achieved through `Character*` base pointers
- **Extensibility**: Factory pattern provides centralized creation

**📄 Detailed Specification**: [systems/interfaces.md](systems/interfaces.md)

---

### Factory Pattern

**Purpose**: Centralize object creation with data-driven configuration.

**Key Factories**:

| Factory              | Creates                                   | Data Source     |
| -------------------- | ----------------------------------------- | --------------- |
| **CharacterFactory** | Dragon, Fighter, Cleric, Wizard, Rogue    | characters.json |
| **SpellFactory**     | Fireball, Wall, Lava Pool, Healing spells | spells.json     |
| **EffectFactory**    | Burn, Fear, Blessing, Curse effects       | effects.json    |
| **MapFactory**       | 8x8 grid layouts with terrain             | maps.json       |

**Example Usage**:

```cpp
// Instead of: auto dragon = new Dragon({0, 0});
auto dragon = CharacterFactory::Create(CharacterType::Dragon, {0, 0});
// Dragon stats loaded from JSON, components auto-attached
```

**Benefits**:

- **Data-Driven**: Change stats/behavior without recompiling
- **Consistency**: All entities created with proper component setup
- **Hot Reloading**: Modify JSON files during development

**📄 Detailed Specification**: [systems/interfaces.md](systems/interfaces.md#factory-pattern-usage)

---

### Component-Based Architecture

**CS230 Engine Foundation**:

The existing CS230 engine already uses a component-based architecture:

```
GameObject (base class)
├─ ComponentManager (owns components)
├─ State machine (for behavior)
└─ Position, velocity, rotation

GameState (battle level)
├─ GSComponentManager (owns game-state components)
└─ GameObjectManager (owns all GameObjects)
```

**Dragonic Tactics Extension Strategy**:

1. **Character** extends `CS230::GameObject`
   
   - Inherits component system
   - Adds tactical RPG interface methods

2. **BattleState** extends `CS230::GameState`
   
   - Adds tactical GSComponents (TurnManager, GridSystem, etc.)
   - Reuses existing GameObjectManager

3. **New Components** follow existing patterns
   
   - `GridPosition` similar to existing position tracking
   - `ActionPoints` similar to existing `Timer`
   - `StatusEffects` similar to existing `ParticleManager`

**Key Principle**: Extend, don't replace. Leverage CS230's existing architecture.

---

## 4. System Interaction

### Component Interaction Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                        CS230 Engine Layer                       │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │
│  │   Logger    │ │   Input     │ │TextureManager│ │GameState  │ │
│  │             │ │             │ │              │ │Manager    │ │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Dragonic Tactics Layer                       │
│                                                                 │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │               🆕 Singleton Services                        │ │
│  │  EventBus · DiceManager · DataRegistry · AIDirector       │ │
│  │  EffectManager · SaveManager                              │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                   │                             │
│  ┌────────────────────────────────▼────────────────────────────┐ │
│  │               BattleState (GameState)                      │ │
│  │  ┌─────────────────────────────────────────────────────┐   │ │
│  │  │  🆕 Battle GSComponents:                           │   │ │
│  │  │  BattleManager · TurnManager · GridSystem          │   │ │
│  │  │  CombatSystem · SpellSystem · AISystem             │   │ │
│  │  └─────────────────────────────────────────────────────┘   │ │
│  │  ┌─────────────────────────────────────────────────────┐   │ │
│  │  │  GameObjectManager (owns all Characters)           │   │ │
│  │  └─────────────────────────────────────────────────────┘   │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                   │                             │
│  ┌────────────────────────────────▼────────────────────────────┐ │
│  │              Character Layer (GameObjects)                 │ │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │ │
│  │  │  Dragon  │  │ Fighter  │  │ Cleric   │  │ Wizard   │   │ │
│  │  │ (Player) │  │  (AI)    │  │  (AI)    │  │  (AI)    │   │ │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │ │
│  │        Each character has GOComponents:                    │ │
│  │        • GridPosition • ActionPoints • StatusEffects       │ │
│  │        • SpellSlots • DamageCalculator • TargetingSystem   │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────┐
│                    External Data Layer (JSON)                   │
│  characters.json · spells.json · maps.json · ai_behaviors.json  │
└─────────────────────────────────────────────────────────────────┘
```

### Typical Gameplay Flow

This section demonstrates how **all ~50 classes** work together during a complete battle. Each phase shows system interactions and class responsibilities.

---

#### Phase 1: Battle Initialization

**1.1 State Setup** (`BattleState::Load()`):

- **BattleState** creates all GSComponents:
  - **BattleManager** - Coordinates victory conditions
  - **TurnManager** - Manages turn order
  - **GridSystem** - Initializes 8x8 battlefield
  - **CombatSystem** - Prepares damage calculations
  - **SpellSystem** - Loads spell definitions via **SpellFactory**
  - **AISystem** - Initializes AI decision-making
  - **StatusEffectManager** - Prepares effect tracking
  - **GameObjectManager** - Manages character lifecycle

**1.2 Data Loading**:

- **DataRegistry** loads JSON configuration:
  - `characters.json` - Character stats (HP, Speed, Attack, Defense)
  - `spells.json` - Spell data (range, damage, effects)
  - `effects.json` - Status effect definitions
  - `maps.json` - Battlefield layout
  - `ai_behaviors.json` - AI decision trees
- **HotReloadManager** begins watching files for changes (debug mode)

**1.3 Character Creation** (**CharacterFactory**):

- **Dragon** created at grid position (4, 4):
  - GOComponents attached: **GridPosition**, **ActionPoints**(2), **SpellSlots**(4/3/2), **StatusEffects**, **DamageCalculator**, **TargetingSystem**
  - Stats loaded: 140 HP, Speed 5, Attack 3d6, Defense 2d6
  - State machine initialized: `State_WaitingTurn`
- **Fighter** created at (0, 0):
  - GOComponents: Same as Dragon + **AIMemory**
  - Stats: 90 HP, Speed 3, Attack 2d8, Defense 1d6
- **Cleric** created at (0, 7):
  - GOComponents: Same as Fighter
  - Stats: 90 HP, Speed 2, Attack 1d6, Defense 2d6, **SpellSlots**(healing)
- **Wizard** created at (7, 7):
  - GOComponents: Same as Fighter
  - Stats: 55 HP, Speed 1, Attack 3d8 (ranged), Defense 1d4, **SpellSlots**(offensive)
- **Rogue** created at (7, 0):
  - GOComponents: Same as Fighter
  - Stats: 65 HP, Speed 4, Attack 2d6, Defense 1d8

**1.4 Grid Placement**:

- **GridSystem** validates character positions
- **MapFactory** applies terrain (walls, obstacles)
- **GridPosition** component converts tile coords → world coords

**1.5 Initiative Roll**:

- **TurnManager** calculates turn order:
  - **DiceManager** rolls initiative: `RollDice(1, 20)` + Speed modifier
  - **Dragon** (Speed 5): 1d20+5 = 22 (first)
  - **Rogue** (Speed 4): 1d20+4 = 18 (second)
  - **Fighter** (Speed 3): 1d20+3 = 15 (third)
  - **Cleric** (Speed 2): 1d20+2 = 10 (fourth)
  - **Wizard** (Speed 1): 1d20+1 = 8 (fifth)
- **EventBus** publishes `InitiativeRolledEvent` → **StatusInfoOverlay** updates turn order UI

**1.6 Debug Systems Initialize** (if enabled):

- **DebugManager** enables debug mode (F12)
- **DebugConsole** initializes command system (~)
- **GridDebugRenderer** ready to show grid overlay (F1)
- **AIDebugVisualizer** ready to show AI paths (F4)
- **StatusInfoOverlay** displays HP/AP bars (F7)
- **CollisionDebugRenderer** can show hitboxes (F10)
- **GodModeManager** ready for cheat activation (Ctrl+G)
- **DebugUIOverlay** shows turn order panel

**1.7 Battle Start Event**:

- **BattleManager** publishes `BattleStartedEvent{allCombatants, "Arena_01"}`
- **EffectManager** plays battle intro animation
- **Logger** records: "Battle started with 5 combatants"

---

#### Phase 2: Turn Execution (Dragon's Turn - Player)

**2.1 Turn Start**:

- **TurnManager** identifies current character: **Dragon**
- **ActionPoints** component calls `Refresh()` → Dragon gets 2 AP
- **StatusEffects** component processes start-of-turn effects:
  - **StatusEffectManager** applies Burn damage (if any)
  - **StatusEffectManager** decrements effect durations
- **EventBus** publishes `TurnStartedEvent{Dragon, 1, 2}`
  - **StatusInfoOverlay** highlights Dragon portrait
  - **DebugUIOverlay** updates current turn display
  - **AISystem** ignores (not AI character)

**2.2 Player Input** (**Input** system):

- Player hovers tile (5, 5) → **EventBus** publishes `UITileHoveredEvent{(5,5), true}`
  - **GridDebugRenderer** highlights valid movement tiles (if F1 pressed)
  - **TargetingSystem** checks range: `IsInRange((5,5))` → returns true (movement range 2)
- Player clicks tile (5, 5) → **GridSystem** validates:
  - `IsValidMove(Dragon, (5,5))` → checks occupancy, obstacles
  - `FindPath((4,4), (5,5))` → A* pathfinding returns path
- **EventBus** publishes `UIActionSelectedEvent{"Move", Dragon}`

**2.3 Movement Execution**:

- **Dragon** state changes: `State_WaitingTurn` → `State_Acting`
- **GridPosition** updates: `SetTilePosition(5, 5)`
- **ActionPoints** decrements: `Spend(1)` → 1 AP remaining
- **EventBus** publishes `CharacterMovedEvent{Dragon, (4,4), (5,5), 1}`
  - **EffectManager** plays footstep particle effect
  - **Logger** records: "Dragon moved from (4,4) to (5,5)"
  - **AIMemory** on AI characters updates: "Dragon moved closer"

**2.4 Spell Selection**:

- Player presses hotkey "1" → selects **Fireball** spell
- **SpellSlots** component checks: `HasSlot(1)` → returns true (4 level-1 slots)
- **SpellSystem** validates:
  - `CanCastSpell(Dragon, Fireball, targetTile)`
  - **TargetingSystem** checks range (Fireball range: 5 tiles)
  - **GridSystem** checks line of sight: `HasLineOfSight(Dragon, targetTile)` → true
- Player selects target tile (7, 7) - **Wizard** location
- **EventBus** publishes `UISpellSelectedEvent{"Fireball", 1, Dragon}`

**2.5 Spell Casting**:

- **SpellSystem** executes `CastSpell(Dragon, Fireball, (7,7), upcastLevel=1)`:
  - **SpellSlots** consumes slot: `ConsumeSlot(1)` → 3 level-1 slots remaining
  - **SpellFactory** retrieves spell data: `GetSpellData(SpellType::Fireball)`
  - **DiceManager** rolls spell damage: `RollDiceFromString("3d6")` → 12 damage
  - **GridSystem** calculates area of effect: `GetSpellArea(Fireball, (7,7))` → single target
- **EventBus** publishes `SpellCastEvent{Dragon, "Fireball", 1, (7,7), 1}`
  - **EffectManager** spawns fireball projectile animation
  - **Logger** records: "Dragon cast Fireball (level 1) at (7,7)"

**2.6 Spell Resolution**:

- **CombatSystem** applies damage to **Wizard**:
  - **DamageCalculator** (Wizard's component) calculates defense:
    - **DiceManager** rolls defense dice: `RollDiceFromString("1d4")` → 2
    - Total defense: 0 (base) + 2 (roll) = 2
  - Final damage: 12 - 2 = 10 damage
  - **Wizard** HP: 55 → 45
- **EventBus** publishes `CharacterDamagedEvent{Wizard, 10, 45, Dragon, false}`
  - **StatusInfoOverlay** updates Wizard's HP bar
  - **EffectManager** plays explosion particle effect at Wizard's position
  - **AIMemory** (Fighter, Cleric, Rogue) records: "Dragon dealt 10 damage to Wizard"
  - **AIDirector** updates threat table: Dragon threat += 10

**2.7 Turn End**:

- **ActionPoints** component: `GetCurrent()` → 0 AP (both actions used)
- Player presses "End Turn" button or no actions remain
- **Dragon** state changes: `State_Acting` → `State_WaitingTurn`
- **EventBus** publishes `TurnEndedEvent{Dragon, 2}`
  - **TurnManager** advances turn index
  - **BattleManager** checks victory conditions: `CheckVictoryConditions()` → battle continues

---

#### Phase 3: Turn Execution (Rogue's Turn - AI)

**3.1 Turn Start**:

- **TurnManager** identifies current character: **Rogue**
- **ActionPoints** refreshes: 1 AP
- **StatusEffects** processes start-of-turn (none active)
- **EventBus** publishes `TurnStartedEvent{Rogue, 2, 1}`
  - **AISystem** receives event → triggers AI decision

**3.2 AI Decision Making** (**AISystem**):

- **AISystem** calls `RogueAI(Rogue)`:
  - **AIDirector** provides global AI state: `GetTeamDecisions()`
  - **AIMemory** retrieves tactical memory:
    - Last target: Dragon (seen 0 turns ago)
    - Dragon position: (5, 5)
    - Dragon HP: 140/140
  - **AIBehaviorFactory** loads Rogue behavior tree from `ai_behaviors.json`:
    - Priority 1: Stealth approach if not detected
    - Priority 2: Ambush high-value target
    - Priority 3: Flank and attack
  - **GridSystem** calculates path to Dragon:
    - `FindPath((7,0), (6,4))` → path found (2 tiles, costs 1 AP)
  - **AISystem** creates `AIDecision`:
    - `action = Move`, `target = (6,4)`, `priority = High`, `reasoning = "Flank Dragon"`

**3.3 AI Action Execution**:

- **Rogue** moves to (6, 4):
  - **GridPosition** updates position
  - **ActionPoints** spends 1 AP → 0 remaining
- **EventBus** publishes `CharacterMovedEvent{Rogue, (7,0), (6,4), 1}`
  - **AIDebugVisualizer** shows Rogue's movement path (if F4 pressed)
  - **EffectManager** plays movement effect

**3.4 Turn End**:

- **AISystem** publishes `TurnEndedEvent{Rogue, 1}`
- **TurnManager** advances to next character (**Fighter**)

---

#### Phase 4: Turn Execution (Cleric's Turn - AI with Healing)

**4.1 AI Decision** (**AISystem** → `ClericAI()`):

- **AIMemory** checks ally HP:
  - **Wizard**: 45/55 HP (damaged)
  - **Fighter**: 90/90 HP (full)
  - **Rogue**: 65/65 HP (full)
- **AIBehaviorFactory** loads Cleric behavior:
  - Priority 1: Heal ally below 70% HP
  - Priority 2: Apply Blessing buff
  - Priority 3: Attack Dragon
- **AISystem** decides: Cast Healing spell on **Wizard**

**4.2 Spell Casting** (**SpellSystem**):

- **SpellSlots** (Cleric) checks: `HasSlot(1)` → true
- **SpellFactory** loads "Healing Word" spell data
- **DiceManager** rolls healing: `RollDiceFromString("2d6+2")` → 9 HP
- **Wizard** HP: 45 + 9 = 54 (capped at 55 max)
- **EventBus** publishes `CharacterHealedEvent{Wizard, 9, 54, 55, Cleric}`
  - **StatusInfoOverlay** updates Wizard HP bar (green flash)
  - **EffectManager** plays sparkle particle effect at Wizard

**4.3 Turn End**:

- **AISystem** publishes `TurnEndedEvent{Cleric, 1}`

---

#### Phase 5: Status Effect Application (Burn Example)

**5.1 Dragon Casts Lava Pool**:

- **SpellSystem** executes `CastSpell(Dragon, LavaPool, (3,3), level=2)`:
  - **SpellFactory** loads spell: "Creates lava tile, applies Burn on entry"
  - **GridSystem** marks tile (3,3) as `TileType::Lava`
  - **EffectFactory** creates Burn status effect: `CreateBurn(duration=3, magnitude=1d6)`

**5.2 Fighter Moves Into Lava**:

- **GridSystem** validates move: `IsValidMove(Fighter, (3,3))` → true (movement allowed)
- **GridSystem** applies tile effect:
  - **EffectFactory** creates Burn effect
  - **StatusEffects** (Fighter component) adds effect: `AddEffect(Burn, 3 turns, 1d6 damage)`
- **EventBus** publishes `StatusEffectAppliedEvent{Fighter, Burn, 3}`
  - **StatusInfoOverlay** shows Burn icon on Fighter portrait
  - **Logger** records: "Fighter is Burning (3 turns)"

**5.3 Next Turn - Burn Tick**:

- **StatusEffectManager** processes start-of-turn:
  - **DiceManager** rolls burn damage: `RollDice(1, 6)` → 4 damage
  - **Fighter** HP: 90 - 4 = 86
  - **StatusEffects** decrements duration: 3 → 2 turns
- **EventBus** publishes `StatusEffectTickEvent{Fighter, Burn, 4}`
  - **EffectManager** plays fire particle effect

---

#### Phase 6: Character Death & Victory

**6.1 Wizard Takes Fatal Damage**:

- **CombatSystem** applies attack damage:
  - **Wizard** HP: 54 - 60 = -6 → clamped to 0
  - **DamageCalculator** records death
- **EventBus** publishes `CharacterDamagedEvent{Wizard, 60, 0, Dragon, true}`
- **EventBus** publishes `CharacterDeathEvent{Wizard, Dragon}`
  - **StatusInfoOverlay** grays out Wizard portrait
  - **EffectManager** plays death animation
  - **Logger** records: "Wizard has been slain by Dragon"

**6.2 Wizard State Transition**:

- **Wizard** state changes: `State_Acting` → `State_Dead`
- **TurnManager** removes Wizard from turn order
- **GameObjectManager** marks for removal (or keeps corpse)

**6.3 Victory Check** (**BattleManager**):

- **BattleManager** checks victory conditions:
  - Count living enemies: **Fighter** (alive), **Cleric** (alive), **Rogue** (alive) = 3 remaining
  - Victory condition: "All enemies defeated" → NOT MET
  - Battle continues

**6.4 All Enemies Defeated**:

- (Later in battle) **Rogue** dies → 0 enemies remaining
- **BattleManager** triggers `EndBattle(victory=true)`:
  - **EventBus** publishes `BattleEndedEvent{true, 25 turns}`
  - **SaveManager** persists campaign data:
    - `SaveCampaignState(CampaignData{...})`
    - Updates: Dragon HP, spell slots, unlocked spells, score
  - **GameStateManager** transitions to VictoryScreen state

---

#### Phase 7: Debug Tools in Action

**7.1 Debug Console Usage** (**DebugConsole**):

- Player presses `~` → console opens
- Player types: `damage Rogue 50`
  - **DebugConsole** parses command
  - Finds **Rogue** character reference
  - Calls `Rogue->TakeDamage(50)`
  - **EventBus** publishes `CharacterDamagedEvent` (normal flow)
- Player types: `reload characters`
  - **HotReloadManager** triggers `DataRegistry::ReloadFile("characters.json")`
  - **Logger** records: "Reloaded characters.json (25 KB)"

**7.2 God Mode** (**GodModeManager**):

- Player presses `Ctrl+G`
- **GodModeManager** enables invincibility:
  - Intercepts all `CharacterDamagedEvent` for Dragon
  - Sets `event.damageAmount = 0`
- **DebugUIOverlay** shows "GOD MODE ACTIVE" indicator

**7.3 Grid Visualization** (**GridDebugRenderer**):

- Player presses `F1`
- **GridDebugRenderer** draws overlay:
  - Grid lines (8x8)
  - Tile coordinates
  - Movement range (green tiles)
  - Attack range (red tiles)
  - Occupied tiles (yellow outline)

**7.4 AI Debugging** (**AIDebugVisualizer**):

- Player presses `F4`
- **AIDebugVisualizer** draws:
  - AI pathfinding arrows (Fighter → Dragon)
  - Threat values above each character (Dragon: 250 threat)
  - Current AI state text ("Engaging", "Healing", "Flanking")
  - Decision reasoning ("Target: Dragon, Action: Melee Attack, Reason: Highest Threat")

---

#### Phase 8: Hot Reload in Action

**8.1 JSON File Modified**:

- Developer edits `characters.json`:
  - Changes Dragon maxHP: 140 → 200
  - Changes Fireball damage: "3d6" → "4d6"
- **HotReloadManager** detects file change (inotify/file watcher)

**8.2 Reload Triggered**:

- **HotReloadManager** calls `DataRegistry::ReloadFile("characters.json")`
- **DataRegistry** re-parses JSON
- **Logger** records: "Hot reloaded characters.json"

**8.3 Data Applied**:

- **CharacterFactory** queries new data:
  - `DataRegistry::GetValue<int>("Dragon.maxHP")` → returns 200
- Existing Dragon instance keeps current HP, but maxHP updates (game designer decision)
- **DebugConsole** shows: "Reload complete. Restart battle to fully apply changes."

---

#### Phase 9: Save/Load Campaign

**9.1 Battle Victory** → **SaveManager**:

- **BattleManager** calls `SaveManager::SaveCampaignState()`:
  - Creates `CampaignData` struct:
    - `currentLevel = 2`
    - `playerScore = 1250`
    - `dragonCurrentHP = 120`
    - `dragonSpellSlots = {3, 2, 1}` (remaining slots)
    - `unlockedSpells = {"Fireball", "LavaPool", "DragonRoar"}`
    - `levelsCompleted = {"Arena_01"}`
    - `totalPlayTime = 3600` (seconds)
  - Serializes to JSON
  - Writes to `save_slot_1.json`

**9.2 Load Campaign**:

- Player selects "Continue" from main menu
- **SaveManager** calls `LoadCampaignState()`:
  - Reads `save_slot_1.json`
  - Deserializes `CampaignData`
  - Returns to **BattleState**
- **BattleState** restores Dragon state:
  - **Dragon** HP set to 120 (from save)
  - **SpellSlots** restored to {3, 2, 1}
  - Loads level 2 map via **MapFactory**

---

### System Interaction Summary

This gameplay flow demonstrates **47 out of ~50 classes** working together:

**Characters (6)**: Dragon, Fighter, Cleric, Wizard, Rogue, Character
**Singletons (6)**: EventBus, DiceManager, DataRegistry, AIDirector, EffectManager, SaveManager
**GSComponents (8)**: BattleManager, TurnManager, GridSystem, CombatSystem, SpellSystem, AISystem, StatusEffectManager, GameObjectManager
**GOComponents (7)**: GridPosition, ActionPoints, SpellSlots, StatusEffects, AIMemory, DamageCalculator, TargetingSystem
**Factories (5)**: CharacterFactory, SpellFactory, EffectFactory, MapFactory, AIBehaviorFactory
**Debug Tools (9)**: DebugManager, DebugConsole, GridDebugRenderer, AIDebugVisualizer, StatusInfoOverlay, CollisionDebugRenderer, GodModeManager, HotReloadManager, DebugUIOverlay
**States (1)**: BattleState
**Data Structures**: Events, CampaignData, SpellData, AIDecision, StatusEffect

**Key Takeaway**: Every class has a specific role, and the **EventBus** ties them together without tight coupling.

---

## 5. Architecture Evolution

### Current State (Weeks 0-6)

**Foundation Phase**: Core systems with minimal dependencies

- ✅ Character base class + Dragon implementation
- ✅ EventBus for decoupled communication
- ✅ GridSystem for 8x8 battlefield
- ✅ TurnManager for initiative order
- ✅ Basic combat (attack, move actions)
- ✅ Debug console for testing

**Key Principle**: Each system independently testable with stubs

---

### Mid-Development (Weeks 7-16)

**Gameplay Expansion**: Add tactical depth

- ✅ 4 AI character types with distinct behaviors
- ✅ Spell system with 6+ spells
- ✅ Status effects (Fear, Curse, Blessing, Burn)
- ✅ DiceManager for D&D-style combat
- ✅ Advanced AI with threat assessment
- ✅ Visual effects and animations

**Key Principle**: Data-driven balance tuning via JSON hot-reloading

---

### Final State (Weeks 17-26)

**Production Polish**: Campaign mode and advanced features

- ✅ Save/load system for campaign progression
- ✅ Multiple battle scenarios and maps
- ✅ Character advancement and equipment
- ✅ Polished UI/UX with animations
- ✅ Accessibility features (colorblind mode, keybind customization)
- ✅ Performance optimization
- ✅ Comprehensive testing and bug fixes

**Key Principle**: Professional-grade experience with maintainable codebase

---

## 6. Documentation Map

This architecture overview provides the **big picture**. For implementation details, refer to:

### System Specifications

| Document                                                                   | Contents                                                 | When to Read                   |
| -------------------------------------------------------------------------- | -------------------------------------------------------- | ------------------------------ |
| **[systems/characters.md](systems/characters.md)**                         | Character class hierarchy, GameObject integration, stats | Implementing character classes |
| **[systems/singletons.md](systems/singletons.md)**                         | All singleton service implementations                    | Adding global services         |
| **[systems/game-state-components.md](systems/game-state-components.md)**   | Battle system components (TurnManager, GridSystem, etc.) | Building battle logic          |
| **[systems/game-object-components.md](systems/game-object-components.md)** | Character component behaviors                            | Adding character capabilities  |
| **[systems/interfaces.md](systems/interfaces.md)**                         | Factory pattern and character creation                   | Creating game entities         |

### Development Planning

| Document                                             | Contents                                | When to Read                      |
| ---------------------------------------------------- | --------------------------------------- | --------------------------------- |
| **[implementation-plan.md](implementation-plan.md)** | 26-week timeline with 5 parallel tracks | Project planning, task assignment |
| **[debug/tools.md](debug/tools.md)**                 | Debug console, god mode, hot reloading  | Development workflow setup        |
| **[debug/commands.md](debug/commands.md)**           | Console command examples                | Testing and debugging             |
| **[debug/ui.md](debug/ui.md)**                       | Debug UI layout and styling             | Debug visualization               |

### Supporting Documents

| Document                                                                                  | Contents                        |
| ----------------------------------------------------------------------------------------- | ------------------------------- |
| **[architecture/dragonic_tactics.md](../architecture/dragonic_tactics.md)**               | Original game design (Korean)   |
| **[architecture/game_architecture_rules.md](../architecture/game_architecture_rules.md)** | Indie dev principles (Korean)   |
| **[ENGINE_COMPONENTS.md](../ENGINE_COMPONENTS.md)**                                       | Existing CS230 engine reference |
| **[CLAUDE.md](../CLAUDE.md)**                                                             | Project context for Claude Code |

---

## Architecture Principles

### 1. Modular Independence

**Principle**: Each system operates independently with minimal coupling.

**Implementation**:

- Singleton services have no inter-dependencies
- Components communicate via `EventBus`, not direct calls
- Factories use `DataRegistry` for configuration, not hardcoded values

**Benefit**: Parallel development (5 developers can work simultaneously)

---

### 2. Data-Driven Design

**Principle**: External JSON files drive behavior and balance.

**Implementation**:

- Character stats in `characters.json`
- Spell effects in `spells.json`
- AI behavior trees in `ai_behaviors.json`
- Map layouts in `maps.json`

**Benefit**: Game designers can balance without programmer assistance

---

### 3. Component-Based Composition

**Principle**: Behavior built from composable pieces, not inheritance trees.

**Implementation**:

- Characters have components (GridPosition, ActionPoints, SpellSlots)
- Add/remove components at runtime for dynamic behavior
- Each component has single responsibility

**Benefit**: Flexible, testable, maintainable code

---

### 4. Event-Driven Communication

**Principle**: Systems react to events, not direct function calls.

**Implementation**:

- `EventBus` publishes events (TurnStarted, CharacterDamaged, SpellCast)
- Systems subscribe to relevant events
- No circular dependencies

**Benefit**: Loose coupling enables easy feature addition

---

### 5. Test-Driven Development

**Principle**: Every system must be testable in isolation.

**Implementation**:

- Mock implementations for dependencies (e.g., `MockDiceManager` always rolls 6)
- Debug console for runtime testing
- Stub interfaces enable parallel development

**Benefit**: Bugs caught early, systems independently verifiable

---

## Long-Term Maintainability

### Code Organization

```
CS230/
├── Engine/              # CS230 engine (existing)
│   ├── GameObject.h     # Base entity class
│   ├── GameState.h      # State machine
│   └── ...
├── Game/                # Dragonic Tactics (new)
│   ├── Characters/      # Dragon, Fighter, Cleric, Wizard, Rogue
│   ├── Singletons/      # EventBus, DiceManager, DataRegistry, etc.
│   ├── Components/      # GridPosition, ActionPoints, SpellSlots, etc.
│   ├── Systems/         # TurnManager, GridSystem, CombatSystem, etc.
│   ├── Factories/       # CharacterFactory, SpellFactory, etc.
│   └── BattleState.h    # Main battle game state
└── Assets/
    ├── Data/            # JSON configuration files
    ├── Sprites/         # Character and effect sprites
    └── Animations/      # Animation definitions
```

### Future Expansion Paths

**Immediate Extensions** (Post-Week 26):

- Additional character types (Paladin, Ranger, Bard)
- More spells and status effects
- Procedurally generated maps
- Multiplayer support

**Long-Term Possibilities**:

- Modding API for community content
- Scripting system for custom AI behaviors
- Advanced analytics for player engagement tracking
- VR/AR support

**Architecture Supports These** through:

- Factory pattern (easy to add new entity types)
- JSON configuration (external content creation)
- EventBus (plugin system for mods)
- Component system (unlimited behavior combinations)

---

## Summary

Dragonic Tactics extends the CS230 engine with a **modular, data-driven, component-based architecture** optimized for:

✅ **Parallel development** (5 developers, minimal blocking)
✅ **Rapid iteration** (JSON hot-reloading, debug console)
✅ **Extensibility** (factory pattern, pure inheritance, components)
✅ **Maintainability** (loose coupling, single responsibility, testability)

**Next Steps**:

1. Read [implementation-plan.md](implementation-plan.md) for development timeline
2. Choose a development track (Characters, Singletons, Battle Systems, AI, UI/Debug)
3. Read relevant [systems/*.md](systems/) file for detailed specifications
4. Implement with test-driven approach using debug tools

**Questions?** Refer to [CLAUDE.md](../CLAUDE.md) for project context and workflow guidance.
