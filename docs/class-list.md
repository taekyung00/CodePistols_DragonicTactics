# Dragonic Tactics - Complete Class List

**Purpose**: Comprehensive reference of ALL classes to implement for Dragonic Tactics project

**Total Classes**: ~50 classes organized by category

**Last Updated**: 2025-10-04

---

## Table of Contents

- [Character Classes (6)](#character-classes)
- [Singleton Services (6)](#singleton-services)
- [GameState Components (7)](#gamestate-components)
- [GameObject Components (7)](#gameobject-components)
- [Factory Classes (5)](#factory-classes)
- [Debug Tools (9)](#debug-tools)
- [Data Structures & Enums (10+)](#data-structures--enums)
- [GameState Implementation (1)](#gamestate-implementation)
- [Developer Assignment](#developer-assignment)

---

## Character Classes

**Location**: `CS230/Game/Characters/`

All characters extend `CS230::GameObject` through `Character` base class (pure GameObject inheritance, NO interfaces).

| #   | Class         | Type            | Extends             | Description                                 | Stats                 |
| --- | ------------- | --------------- | ------------------- | ------------------------------------------- | --------------------- |
| 1   | **Character** | Abstract Base   | `CS230::GameObject` | Base class for all battlefield entities     | N/A                   |
| 2   | **Dragon**    | Player          | `Character`         | Player-controlled dragon with spell-casting | 140 HP, Speed 5, 2 AP |
| 3   | **Fighter**   | AI Tank         | `Character`         | Melee tank, engages dragon                  | 90 HP, Speed 3, 2 AP  |
| 4   | **Cleric**    | AI Support      | `Character`         | Healing support, extends battles            | 90 HP, Speed 2, 1 AP  |
| 5   | **Wizard**    | AI Glass Cannon | `Character`         | Long-range high damage, fragile             | 55 HP, Speed 1, 1 AP  |
| 6   | **Rogue**     | AI Assassin     | `Character`         | High mobility, flanking, stealth            | 65 HP, Speed 4, 1 AP  |

**Key Methods** (all Characters):

- `Type()`, `TypeName()` - GameObject overrides
- `Update(dt)`, `Draw(camera_matrix)` - Frame loop
- `CanCollideWith()`, `ResolveCollision()` - Collision system
- `TakeDamage(damage)`, `PerformAction(action)` - Combat interface
- `GetActionPoints()`, `GetMovementRange()`, `GetSpeed()` - Turn system
- `GetCurrentHP()`, `GetMaxHP()`, `IsAlive()` - Health system
- `GetGridPosition()`, `SetGridPosition()` - Grid positioning

**State Machines**:

- `State_WaitingTurn`, `State_Acting`, `State_Dead` (all characters)
- AI characters have additional states (e.g., Fighter: `State_Engaging`, `State_Tanking`)

---

## Singleton Services

**Location**: `CS230/Game/Singletons/`

All singletons follow pattern: `static SomeClass& Instance()`

| #   | Class             | Purpose                                          | Key Methods                                                        | Dependencies           |
| --- | ----------------- | ------------------------------------------------ | ------------------------------------------------------------------ | ---------------------- |
| 1   | **EventBus**      | Pub/sub event system for decoupled communication | `Subscribe<T>()`, `Publish<T>()`, `Unsubscribe<T>()`               | None                   |
| 2   | **DiceManager**   | Centralized dice rolling with seeding            | `RollDice(count, sides)`, `RollDiceFromString("3d6")`, `SetSeed()` | None                   |
| 3   | **DataRegistry**  | JSON data loading with hot-reload                | `LoadFromFile()`, `ReloadFile()`, `GetValue<T>()`, `GetJSON()`     | None                   |
| 4   | **AIDirector**    | Global AI coordination and difficulty            | `SetDifficultyLevel()`, `GetTeamDecisions()`, `EvaluateThreat()`   | None                   |
| 5   | **EffectManager** | Visual effects and particles                     | `PlaySpellEffect()`, `PlayAttackEffect()`, `EmitParticles()`       | CS230::ParticleManager |
| 6   | **SaveManager**   | Campaign persistence                             | `SaveCampaignState()`, `LoadCampaignState()`, `SaveFileExists()`   | None                   |

**Usage Pattern**:

```cpp
// Example: Roll dice
int damage = DiceManager::Instance().RollDiceFromString("3d6");

// Example: Publish event
EventBus::Instance().Publish(CharacterDiedEvent{deadCharacter, killer});

// Example: Load data
int dragonHP = DataRegistry::Instance().GetValue<int>("Dragon.maxHP", 100);
```

---

## GameState Components

**Location**: `CS230/Game/Systems/`

All GSComponents attached to BattleState via `AddGSComponent(new Component())`

| #   | Class                   | Purpose                                   | Key Methods                                                                 | Dependencies                          |
| --- | ----------------------- | ----------------------------------------- | --------------------------------------------------------------------------- | ------------------------------------- |
| 1   | **BattleManager**       | Battle coordination, victory conditions   | `StartBattle()`, `CheckVictoryConditions()`, `EndBattle()`                  | TurnManager                           |
| 2   | **TurnManager**         | Turn order, initiative, action points     | `InitializeTurnOrder()`, `AdvanceToNextTurn()`, `GetCurrentCharacter()`     | None                                  |
| 3   | **GridSystem**          | 8x8 grid, pathfinding, tile management    | `IsValidMove()`, `GetReachableTiles()`, `FindPath()`, `HasLineOfSight()`    | None                                  |
| 4   | **CombatSystem**        | Damage calculation, attack resolution     | `ExecuteAttack()`, `CalculateDamage()`, `ApplyDamage()`, `ApplyKnockback()` | DiceManager, GridSystem               |
| 5   | **SpellSystem**         | Spell casting, slot management, effects   | `CastSpell()`, `CanCastSpell()`, `CanUpcast()`, `GetSpellArea()`            | DiceManager, GridSystem, CombatSystem |
| 6   | **AISystem**            | AI decision-making, behavior coordination | `MakeDecision()`, `FighterAI()`, `ClericAI()`, `WizardAI()`, `RogueAI()`    | AIDirector, GridSystem                |
| 7   | **StatusEffectManager** | Status effects, buffs/debuffs tracking    | `ApplyStatusEffect()`, `GetModifiedStat()`, `ApplyCurse()`, `ApplyFear()`   | EventBus                              |

**Usage Pattern**:

```cpp
// In BattleState::Load()
AddGSComponent(new BattleManager());
AddGSComponent(new TurnManager());
AddGSComponent(new GridSystem());

// Access from anywhere
auto* grid = GetGSComponent<GridSystem>();
auto path = grid->FindPath(from, to);
```

---

## GameObject Components

**Location**: `CS230/Game/Components/`

All GOComponents attached to Characters via `AddGOComponent(new Component())`

| #   | Class                | Purpose                                        | Key Methods                                                        | Attached To                            |
| --- | -------------------- | ---------------------------------------------- | ------------------------------------------------------------------ | -------------------------------------- |
| 1   | **GridPosition**     | Tile-based positioning, world coord conversion | `GetTilePosition()`, `SetTilePosition()`, `GetWorldPosition()`     | All characters                         |
| 2   | **ActionPoints**     | Turn-based action economy                      | `GetCurrent()`, `CanAfford(cost)`, `Spend(cost)`, `Refresh()`      | All characters                         |
| 3   | **SpellSlots**       | Magic resource tracking by level               | `HasSlot(level)`, `ConsumeSlot(level)`, `RestoreSlots(min, max)`   | Dragon, Wizard, Cleric, Rogue, Fighter |
| 4   | **StatusEffects**    | Individual character buffs/debuffs             | `AddEffect()`, `RemoveEffect()`, `GetActiveEffects()`              | All characters                         |
| 5   | **AIMemory**         | AI decision history, tactical memory           | `RecordDecision()`, `GetLastTarget()`, `GetTurnsSinceSeen()`       | AI characters only                     |
| 6   | **DamageCalculator** | Character-specific damage calculation          | `CalculateAttackDamage()`, `CalculateDefense()`, `GetAttackDice()` | All characters                         |
| 7   | **TargetingSystem**  | Attack/spell range validation                  | `IsInRange(target)`, `GetValidTargets()`, `GetRangeIndicator()`    | All characters                         |

**Usage Pattern**:

```cpp
// Attach components when creating character
dragon->AddGOComponent(new GridPosition(4, 4));
dragon->AddGOComponent(new ActionPoints(2));
dragon->AddGOComponent(new SpellSlots({{1,4}, {2,3}, {3,2}}));

// Access from character
auto* ap = dragon->GetGOComponent<ActionPoints>();
if (ap->CanAfford(1)) {
    ap->Spend(1);
    Attack();
}
```

---

## Factory Classes

**Location**: `CS230/Game/Factories/`

All factories use static methods for object creation.

| #   | Class                 | Creates             | Key Methods                                                           | Data Source         |
| --- | --------------------- | ------------------- | --------------------------------------------------------------------- | ------------------- |
| 1   | **CharacterFactory**  | All character types | `CreateCharacter(type, gridPos)`, `CreateDragon()`, `CreateFighter()` | `characters.json`   |
| 2   | **SpellFactory**      | Spell instances     | `CreateSpell(spellType)`, `GetSpellData(type)`                        | `spells.json`       |
| 3   | **EffectFactory**     | Status effects      | `CreateEffect(effectType, duration)`, `CreateBurn()`, `CreateFear()`  | `effects.json`      |
| 4   | **MapFactory**        | Battle maps         | `CreateMap(mapName)`, `LoadMapLayout()`                               | `maps.json`         |
| 5   | **AIBehaviorFactory** | AI behavior trees   | `CreateAIBehavior(characterType)`, `LoadBehaviorTree()`               | `ai_behaviors.json` |

**Usage Pattern**:

```cpp
// Create characters
Character* dragon = CharacterFactory::CreateCharacter(CharacterType::Dragon, {4, 4});
Character* fighter = CharacterFactory::CreateCharacter(CharacterType::Fighter, {0, 0});

// All stats loaded from JSON automatically
// All components attached automatically
// Ready to use immediately
```

---

## Debug Tools

**Location**: `CS230/Game/Debug/`

All debug tools implement `CS230::Component` for lifecycle management.

| #   | Class                      | Purpose                   | Key Features                                                   | Hotkey    |
| --- | -------------------------- | ------------------------- | -------------------------------------------------------------- | --------- |
| 1   | **DebugManager**           | Master debug control      | Enable/disable debug mode, coordinate all debug tools          | F12       |
| 2   | **DebugConsole**           | Command-line interface    | 50+ commands (spawn, kill, teleport, heal, damage, etc.)       | ~ (Tilde) |
| 3   | **GridDebugRenderer**      | Grid visualization        | Show grid lines, coordinates, occupancy, movement/attack range | F1        |
| 4   | **AIDebugVisualizer**      | AI path visualization     | Show AI paths, targets, threat values, decision states         | F4        |
| 5   | **StatusInfoOverlay**      | Real-time character stats | HP bars, AP bars, spell slots, status effect badges            | F7        |
| 6   | **CollisionDebugRenderer** | Collision box display     | Show collision boxes, attack hitboxes, spell AOE               | F10       |
| 7   | **GodModeManager**         | Cheat system              | Invincibility, infinite resources, instant kill                | Ctrl+G    |
| 8   | **HotReloadManager**       | Live data reload          | Watch JSON files, auto-reload on change                        | Ctrl+R    |
| 9   | **DebugUIOverlay**         | Debug info panel          | Turn order, performance metrics, quick commands                | Always on |

**Console Commands** (50+ total):

```
spawn <type> <x> <y>           # Spawn character
kill <name>                    # Kill character
teleport <name> <x> <y>        # Move character
heal <name> <amount>           # Heal character
damage <name> <amount>         # Damage character
setstat <name> <stat> <value>  # Modify stat
giveap <name> <amount>         # Give action points
givespell <name> <level>       # Give spell slots
refillslots <name>             # Restore all slots
endturn                        # Force end turn
nextturn <count>               # Skip turns
restart                        # Restart battle
victory / defeat               # Force win/loss
disableai / enableai           # Control AI
reload <type>                  # Hot reload data
god <on/off>                   # Toggle god mode
debug <feature> <on/off>       # Toggle debug visuals
```

---

## Data Structures & Enums

**Location**: `CS230/Game/Types/`

### Enums

| Enum                 | Values                                                                                                    | Usage                        |
| -------------------- | --------------------------------------------------------------------------------------------------------- | ---------------------------- |
| **SpellType**        | CreateWall, Fireball, LavaPool, DragonRoar, TailSwipe, DragonWrath, MeteorStrike, HeatAbsorb, HeatRelease | Spell identification         |
| **StatusEffectType** | Curse, Fear, Bind, Blind, Blessing, Haste, Burn                                                           | Status effect identification |
| **ActionType**       | Move, MeleeAttack, RangedAttack, CastSpell, Heal, Wait, Sprint, Stealth, Ambush                           | Action identification        |
| **Direction**        | North, South, East, West                                                                                  | Facing direction             |
| **GameObjectTypes**  | Dragon, Fighter, Cleric, Wizard, Rogue (extend existing enum)                                             | Type identification          |
| **CharacterType**    | Dragon, Fighter, Cleric, Wizard, Rogue                                                                    | Factory creation             |
| **AttackType**       | Melee, Ranged, Spell, True                                                                                | Combat type                  |
| **BattleResult**     | Victory, Defeat, Escaped                                                                                  | Battle outcome               |
| **DifficultyLevel**  | Easy, Normal, Hard, Nightmare                                                                             | AI difficulty                |
| **TileType**         | Empty, Wall, Lava, Corpse                                                                                 | Grid tile state              |

### Structs

| Struct           | Fields                                                                                                                     | Usage               |
| ---------------- | -------------------------------------------------------------------------------------------------------------------------- | ------------------- |
| **StatusEffect** | `type`, `turnsRemaining`, `magnitude`, `source`                                                                            | Status effect data  |
| **SpellData**    | `level`, `range`, `requiresLineOfSight`, `description`                                                                     | Spell configuration |
| **AIDecision**   | `action`, `target`, `priority`, `reasoning`                                                                                | AI decision output  |
| **CampaignData** | `currentLevel`, `playerScore`, `dragonCurrentHP`, `dragonSpellSlots`, `unlockedSpells`, `levelsCompleted`, `totalPlayTime` | Save file data      |

### Events (for EventBus)

| Event                        | Fields                                            | Published When         |
| ---------------------------- | ------------------------------------------------- | ---------------------- |
| **DamageTakenEvent**         | `target`, `source`, `damageAmount`, `wasCritical` | Character takes damage |
| **CharacterDiedEvent**       | `deadCharacter`, `killer`                         | Character HP reaches 0 |
| **TurnStartedEvent**         | `activeCharacter`, `turnNumber`                   | Turn begins            |
| **TurnEndedEvent**           | `character`, `actionsUsed`                        | Turn ends              |
| **SpellCastEvent**           | `caster`, `spell`, `target`, `spellLevel`         | Spell cast             |
| **BattleStartedEvent**       | `allCombatants`                                   | Battle begins          |
| **BattleEndedEvent**         | `playerWon`, `turnsElapsed`                       | Battle ends            |
| **StatusEffectAppliedEvent** | `target`, `effectType`, `duration`                | Effect applied         |
| **StatusEffectRemovedEvent** | `target`, `effectType`                            | Effect removed         |

---

## GameState Implementation

**Location**: `CS230/Game/States/`

| Class           | Extends            | Purpose                                            | Key Methods                                  |
| --------------- | ------------------ | -------------------------------------------------- | -------------------------------------------- |
| **BattleState** | `CS230::GameState` | Main battle game state, glues all systems together | `Load()`, `Update(dt)`, `Unload()`, `Draw()` |

**BattleState Responsibilities**:

- Create and manage all GSComponents (BattleManager, TurnManager, GridSystem, etc.)
- Create characters via CharacterFactory
- Handle input (player actions, console toggle)
- Coordinate frame loop (update systems, draw characters)
- Manage state transitions (victory -> VictoryScreen, defeat -> GameOverScreen)

---

## Developer Assignment

Based on **domain-based ownership** strategy for parallel development:

### Developer 1: Character Systems Lead

**Classes Owned** (13 total):

- Character (base class)
- Dragon, Fighter, Cleric, Wizard, Rogue (5 character types)
- GridPosition, ActionPoints, SpellSlots, StatusEffects, DamageCalculator, TargetingSystem, AIMemory (7 GOComponents)

### Developer 2: Battle Systems Lead

**Classes Owned** (10 total):

- BattleManager, TurnManager, GridSystem, CombatSystem (4 GSComponents)
- SpellSystem, SpellFactory, EffectFactory (3 classes)
- SpellData, AttackType, BattleResult (3 data structures)

### Developer 3: AI Systems Lead

**Classes Owned** (7 total):

- AISystem (GSComponent)
- AIDirector (Singleton)
- AIBehaviorFactory
- AIDecision struct
- AI-specific logic coordination

### Developer 4: Data & Services Lead

**Classes Owned** (9 total):

- EventBus, DiceManager, DataRegistry, EffectManager, SaveManager (5 Singletons)
- CharacterFactory, MapFactory (2 Factories)
- Events (9 event types)
- CampaignData struct

### Developer 5: Debug & Tools Lead

**Classes Owned** (10 total):

- DebugManager, DebugConsole, GridDebugRenderer, AIDebugVisualizer, StatusInfoOverlay, CollisionDebugRenderer, GodModeManager, HotReloadManager, DebugUIOverlay (9 debug tools)
- BattleState (main GameState implementation)

---

## Implementation Timeline

### Week 1: Interface Stubs (All Developers)

- Create header files with method signatures
- Implement stub methods (empty or return dummy values)
- Enable parallel development with fake dependencies

### Weeks 2-20: Parallel Implementation

- Each developer implements their domain using stubs for dependencies
- Gradual integration as real implementations replace stubs

### Weeks 6-20: Integration Phase

- Swap stub dependencies for real implementations
- Integration testing
- Bug fixes

### Weeks 21-26: Polish Phase

- Balance tuning
- Performance optimization
- Final bug fixes
- Documentation

---

## Class Dependency Graph

**Key Dependencies** (for reference):

```
Character
├─ depends on: GridPosition, ActionPoints, SpellSlots (GOComponents)
└─ used by: BattleManager, TurnManager, AISystem

GridSystem
├─ depends on: Nothing (self-contained)
└─ used by: CombatSystem, SpellSystem, AISystem, all Characters

CombatSystem
├─ depends on: DiceManager, GridSystem
└─ used by: Characters, AISystem

SpellSystem
├─ depends on: DiceManager, GridSystem, CombatSystem
└─ used by: Dragon, Wizard, Cleric

AISystem
├─ depends on: AIDirector, GridSystem, CombatSystem
└─ used by: BattleState (for AI character turns)

TurnManager
├─ depends on: Nothing (self-contained)
└─ used by: BattleManager, BattleState

BattleManager
├─ depends on: TurnManager
└─ used by: BattleState

EventBus
├─ depends on: Nothing (self-contained)
└─ used by: All systems (pub/sub)

DataRegistry
├─ depends on: Nothing (self-contained)
└─ used by: All Factories, HotReloadManager

DiceManager
├─ depends on: Nothing (self-contained)
└─ used by: CombatSystem, SpellSystem, Characters
```

**Note**: Dependencies shown above can be **stubbed** during development to enable parallel work. Real implementations swapped in during integration phase.

---

## File Organization

```
CS230/
├── Game/
│   ├── Characters/
│   │   ├── Character.h/cpp           # Base class
│   │   ├── Dragon.h/cpp              # Player character
│   │   ├── Fighter.h/cpp             # AI tank
│   │   ├── Cleric.h/cpp              # AI healer
│   │   ├── Wizard.h/cpp              # AI glass cannon
│   │   └── Rogue.h/cpp               # AI assassin
│   │
│   ├── Singletons/
│   │   ├── EventBus.h/cpp
│   │   ├── DiceManager.h/cpp
│   │   ├── DataRegistry.h/cpp
│   │   ├── AIDirector.h/cpp
│   │   ├── EffectManager.h/cpp
│   │   └── SaveManager.h/cpp
│   │
│   ├── Systems/                      # GSComponents
│   │   ├── BattleManager.h/cpp
│   │   ├── TurnManager.h/cpp
│   │   ├── GridSystem.h/cpp
│   │   ├── CombatSystem.h/cpp
│   │   ├── SpellSystem.h/cpp
│   │   ├── AISystem.h/cpp
│   │   └── StatusEffectManager.h/cpp
│   │
│   ├── Components/                   # GOComponents
│   │   ├── GridPosition.h/cpp
│   │   ├── ActionPoints.h/cpp
│   │   ├── SpellSlots.h/cpp
│   │   ├── StatusEffects.h/cpp
│   │   ├── AIMemory.h/cpp
│   │   ├── DamageCalculator.h/cpp
│   │   └── TargetingSystem.h/cpp
│   │
│   ├── Factories/
│   │   ├── CharacterFactory.h/cpp
│   │   ├── SpellFactory.h/cpp
│   │   ├── EffectFactory.h/cpp
│   │   ├── MapFactory.h/cpp
│   │   └── AIBehaviorFactory.h/cpp
│   │
│   ├── Debug/
│   │   ├── DebugManager.h/cpp
│   │   ├── DebugConsole.h/cpp
│   │   ├── GridDebugRenderer.h/cpp
│   │   ├── AIDebugVisualizer.h/cpp
│   │   ├── StatusInfoOverlay.h/cpp
│   │   ├── CollisionDebugRenderer.h/cpp
│   │   ├── GodModeManager.h/cpp
│   │   ├── HotReloadManager.h/cpp
│   │   └── DebugUIOverlay.h/cpp
│   │
│   ├── Types/
│   │   ├── SpellTypes.h              # SpellType enum, SpellData struct
│   │   ├── StatusEffectTypes.h       # StatusEffectType enum, StatusEffect struct
│   │   ├── ActionTypes.h             # ActionType enum
│   │   ├── GameObjectTypes.h         # Extend existing enum
│   │   ├── Events.h                  # All event structs
│   │   └── CommonTypes.h             # Direction, CharacterType, etc.
│   │
│   ├── States/
│   │   └── BattleState.h/cpp         # Main game state
│   │
│   └── GameObjectTypes.h             # (existing file - extend it)
│
└── Assets/
    └── Data/                          # JSON configuration files
        ├── characters.json
        ├── spells.json
        ├── effects.json
        ├── maps.json
        ├── ai_behaviors.json
        ├── balance.json
        └── debug_config.json
```

---

## Quick Reference

### Most Important Classes (Start Here)

**Foundation Layer** (Week 1-6):

1. **Character** - Base class for all entities
2. **GridSystem** - 8x8 battlefield management
3. **TurnManager** - Turn order and action points
4. **EventBus** - Event communication
5. **DiceManager** - Dice rolling
6. **DataRegistry** - JSON data loading

**Core Gameplay** (Week 7-12):

1. **Dragon** - Player character implementation
2. **CombatSystem** - Damage calculation and attacks
3. **SpellSystem** - Spell casting and effects
4. **CharacterFactory** - Data-driven character creation

**AI & Polish** (Week 13-26):

1. **AISystem** - AI decision-making
2. **Fighter/Cleric/Wizard/Rogue** - AI character implementations
3. **Debug tools** - Development acceleration
4. **BattleState** - Final integration

---

## Related Documentation

- **[docs/architecture.md](architecture.md)** - High-level architecture overview
- **[docs/implementation-plan.md](implementation-plan.md)** - 26-week development timeline
- **[docs/systems/characters.md](systems/characters.md)** - Character implementation details
- **[docs/systems/singletons.md](systems/singletons.md)** - Singleton service specifications
- **[docs/systems/game-state-components.md](systems/game-state-components.md)** - Battle system details
- **[docs/systems/game-object-components.md](systems/game-object-components.md)** - Component behaviors
- **[docs/debug/tools.md](debug/tools.md)** - Debug system specification

---

**Last Updated**: 2025-10-04
**Total Classes**: ~50 classes to implement
**Development Team**: 5 developers with domain-based ownership
