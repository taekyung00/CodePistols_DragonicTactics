# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Recent Work Summary

### Session History & Accomplishments

#### New Engine Implementation (CURRENT)

**MAJOR UPDATE**: The project has transitioned from the old CS230 MSBuild-based engine to a **brand new CMake-based OpenGL engine** with SDL2, modern C++20, and multi-platform support (Windows + WebAssembly).

**Current Implementation Status**:
- **Week 1 Foundation Systems**: IMPLEMENTED
  - EventBus (type-safe event system)
  - DiceManager (D&D dice notation)
  - Character base class with grid positioning
  - GridSystem (8x8 tactical grid)
  - TurnManager (speed-based initiative)
  - StatsComponent (combat stats tracking)

- **Week 2 Systems**: IMPLEMENTED
  - Dragon character (player)
  - Fighter character (opponent)
  - Grid integration with character movement
  - Character sprite rendering

- **Active Development**: Weeks 3-5 (Combat, Spells, Polish, Playtest)

#### Architecture Documentation & Planning

- **Reorganized documentation structure** into focused, topic-based files under `docs/`:

  - **[docs/index.md](docs/index.md)** - Main navigation hub for all documentation
  - **[docs/architecture.md](docs/architecture.md)** - High-level architecture overview (703 lines)
  - **[docs/implementation-plan.md](docs/implementation-plan.md)** - 26-week development timeline (strategic overview)
  - **[docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/)** - Week-by-week tactical guides (3,921 lines total):
    - [week1.md](docs/Detailed%20Implementations/weeks/week1.md) - Foundation (5 systems in parallel) - 1,940 lines
    - [week2.md](docs/Detailed%20Implementations/weeks/week2.md) - Dragon + Grid integration - 540 lines
    - [week3.md](docs/Detailed%20Implementations/weeks/week3.md) - Combat + Spells - 542 lines
    - [week4.md](docs/Detailed%20Implementations/weeks/week4.md) - Spell expansion - 528 lines
    - [week5.md](docs/Detailed%20Implementations/weeks/week5.md) - Polish + Playtest 1 - 371 lines
  - **[docs/systems/](docs/systems/)** - Detailed system specifications:
    - [characters.md](docs/systems/characters.md) - Character implementations (705 lines)
    - [singletons.md](docs/systems/singletons.md) - Global services (755 lines)
    - [game-state-components.md](docs/systems/game-state-components.md) - Battle systems (1040 lines)
    - [game-object-components.md](docs/systems/game-object-components.md) - Character components (693 lines)
    - [interfaces.md](docs/systems/interfaces.md) - Patterns & factories (296 lines)
  - **[docs/debug/](docs/debug/)** - Debug tools documentation:
    - [tools.md](docs/debug/tools.md) - Debug system spec (782 lines)
    - [commands.md](docs/debug/commands.md) - Command examples (783 lines)
    - [ui.md](docs/debug/ui.md) - UI layout design (648 lines)

#### Supporting Documentation

- **architecture/** directory contains supplementary documents:
  - **dragonic_tactics.md** - Korean language game design document (original specifications)
  - **game_architecture_rules.md** - Korean architectural principles (5 core rules for indie game development)
  - **Architecture_Document.md** - Academic milestone template for CS230 course
  - **Rubric-EngineProof.md** - Academic grading rubric

### Project Understanding

**Current State**: NEW custom OpenGL engine with CMake build system, implementing Dragonic Tactics tactical RPG

**Project Type**: Year-long tactical RPG development (26 weeks)

**Game Genre**: Turn-based tactical RPG (inspired by D&D/Baldur's Gate 3)

**Key Insight**: This is a complete custom engine built from scratch with tactical RPG systems integrated from the ground up.

### Quick Start Commands

#### Building
```bash
# Configure build (Windows Debug)
cmake --preset windows-debug

# Build
cmake --build --preset windows-debug

# Run executable
./build/windows-debug/dragonic_tactics.exe
```

#### Alternative Build Presets
- `windows-debug` - Debug build with console output
- `windows-developer-release` - Optimized with debug symbols
- `windows-release` - Full optimization, no console
- `web-debug-on-windows` - WebAssembly/Emscripten build

---

## Build System

This project uses **CMake 3.21+** with modern C++20:

### Building

#### Visual Studio 2022
1. Open folder in Visual Studio (File → Open → Folder)
2. CMake will auto-configure using presets
3. Build: Ctrl+Shift+B
4. Run: F5 (debugger) or Ctrl+F5 (no debugger)

#### Command Line
```bash
# Configure
cmake --preset windows-debug

# Build
cmake --build --preset windows-debug

# Run
./build/windows-debug/dragonic_tactics.exe
```

#### CMake GUI
```bash
cmake-gui .
# Select preset: windows-debug, windows-developer-release, or windows-release
# Configure → Generate → Open Project
```

### Configuration

- **Language Standard**: C++20 (CMAKE_CXX_STANDARD 20, no extensions)
- **Platform Toolset**: v143 (Visual Studio 2022)
- **Warning Level**: Level 4 with warnings treated as errors
- **Configurations**: Debug, Developer Release, Release
- **Platforms**: Windows (x86/x64) + WebAssembly (Emscripten)
- **Assets**: CMake post-build automatically copies `Assets/` folder to output directory

### External Dependencies (via CMake FetchContent)

All dependencies are **automatically downloaded and configured** by CMake:

| Library | Purpose | Version |
|---------|---------|---------|
| **OpenGL** | Graphics API | System |
| **GLEW** | OpenGL extensions | Latest |
| **SDL2** | Window, input, platform abstraction | 2.28.5+ |
| **Dear ImGui** | Debug UI overlay | docking branch |
| **GSL** | Guidelines Support Library (lifetime safety) | v4.0.0 |
| **STB** | Image loading (stb_image.h) | Latest |

No manual dependency installation required - CMake handles everything!

---

## Architecture Overview

This is a **modern 2D tactical RPG engine** written in C++20 with OpenGL rendering, built using component-based architecture and event-driven design.

### Core Engine (CS230 namespace)

#### Engine Singleton ([source/Engine/Engine.hpp](DragonicTactics/source/Engine/Engine.hpp))
- **Pattern**: Singleton with Pimpl idiom (private implementation)
- **Purpose**: Central orchestrator for all engine subsystems
- **Key Methods**:
  - `Start(window_title)` - Initialize all subsystems, create window
  - `Update()` - Main game loop iteration (input, logic, render, ImGui)
  - `Stop()` - Graceful shutdown and cleanup
  - `HasGameEnded()` - Check for termination condition

#### Core Subsystems

| Subsystem | File | Purpose |
|-----------|------|---------|
| **Window** | [Window.hpp](DragonicTactics/source/Engine/Window.hpp) | SDL2 window management, OpenGL context (800x600 default) |
| **Input** | [Input.hpp](DragonicTactics/source/Engine/Input.hpp) | Keyboard input (A-Z, arrows, space, enter, escape, tab) |
| **Logger** | [Logger.hpp](DragonicTactics/source/Engine/Logger.hpp) | Debug logging (Verbose/Debug/Event/Error) to `Logger.txt` |
| **GameStateManager** | [GameStateManager.hpp](DragonicTactics/source/Engine/GameStateManager.hpp) | State machine (Splash → MainMenu → Battle) |
| **TextureManager** | [TextureManager.hpp](DragonicTactics/source/Engine/TextureManager.hpp) | Asset loading and caching (PNG textures) |
| **Renderer2D** | [IRenderer2D.hpp](DragonicTactics/source/CS200/IRenderer2D.hpp) | 2D graphics rendering abstraction (CS200 namespace) |
| **TextManager** | [TextManager.hpp](DragonicTactics/source/Engine/TextManager.hpp) | Text rendering system |

### GameObject Framework (CS230 namespace)

#### GameObject ([source/Engine/GameObject.h](DragonicTactics/source/Engine/GameObject.h))
Base entity class for all game objects:

**Core Virtual Methods**:
- `Type()` - Returns GameObjectTypes enum value
- `TypeName()` - Returns string name for debugging
- `Update(dt)` - Per-frame logic update
- `Draw(transform)` - Rendering with transformation matrix

**Properties**:
- Position (`Math::vec2`)
- Velocity, Rotation, Scale
- Collision bounds and detection
- Component system integration
- State machine support (nested `State` classes)

**State Machine Pattern**:
```cpp
class MyGameObject : public CS230::GameObject {
    class StateIdle : public State { /* ... */ };
    class StateMoving : public State { /* ... */ };
    void change_state(State* new_state);
};
```

**Collision System**:
- `IsCollidingWith(GameObject*)` - Collision test
- `CanCollideWith(GameObjectTypes)` - Filter collision checks
- `ResolveCollision(GameObject*)` - Handle collision response

#### Component System ([Component.h](DragonicTactics/source/Engine/Component.h), [ComponentManager.h](DragonicTactics/source/Engine/ComponentManager.h))

**Component Interface**:
```cpp
class Component {
    virtual void Update(double dt) = 0;
    virtual ~Component() = default;
};
```

**ComponentManager**:
- Manages vector of `Component*` pointers
- Owned by both `GameObject` and `GameState`
- Template methods: `GetComponent<T>()` for type-safe retrieval

**Usage Example**:
```cpp
auto stats = character->GetGOComponent<StatsComponent>();
stats->TakeDamage(10);
```

#### GameState ([source/Engine/GameState.hpp](DragonicTactics/source/Engine/GameState.hpp))

Abstract base for game states:

**Virtual Lifecycle Methods**:
- `Load()` - Initialize state resources
- `Update(dt)` - Per-frame game logic
- `Draw()` - Render game objects
- `DrawImGui()` - Debug UI overlay
- `Unload()` - Cleanup resources

**Features**:
- Component system support (like GameObject)
- `GameObjectManager` ownership
- State transition via `GameStateManager::SetNextGameState()`

---

## Dragonic Tactics Systems

### 1. Character System

#### Character Base Class ([Game/DragonicTactics/Objects/Character.h](DragonicTactics/source/Game/DragonicTactics/Objects/Character.h))

**Extends**: `CS230::GameObject`

**Character Types**:
```cpp
enum class CharacterTypes {
    Dragon,   // Player character
    Fighter,  // Opponent (melee)
    Rogue,    // Opponent (ranged/dex)
    Cleric,   // Opponent (healer/support)
    Wizard    // Opponent (spellcaster)
};
```

**Core Components**:
- `GridPosition` - Tile coordinates on 8x8 grid ([GridPosition.h](DragonicTactics/source/Game/DragonicTactics/Objects/Components/GridPosition.h))
- `StatsComponent` - HP, attack, defense, speed ([StatsComponent.h](DragonicTactics/source/Game/DragonicTactics/Objects/Components/StatsComponent.h))
- `ActionPoints` - Movement/action budget per turn ([ActionPoints.h](DragonicTactics/source/Game/DragonicTactics/Objects/Components/ActionPoints.h))
- `SpellSlots` - Magic system (spell levels 1-9) ([SpellSlots.h](DragonicTactics/source/Game/DragonicTactics/Objects/Components/SpellSlots.h))

**Key Methods**:
```cpp
// Turn management
void OnTurnStart();
void OnTurnEnd();
int GetActionPoints() const;
void RefreshActionPoints();

// Combat
void PerformAttack(Character* target);
void PerformAction(Action* action, Character* target, Math::ivec2 tile_pos);
void TakeDamage(int amount, DamageType type);
void ReceiveHeal(int amount);

// Movement
void SetPathTo(Math::ivec2 destination);  // Pathfinding
bool IsAlive() const;
```

#### Character Stats ([Game/DragonicTactics/Types/CharacterTypes.h](DragonicTactics/source/Game/DragonicTactics/Types/CharacterTypes.h))

```cpp
struct CharacterStats {
    int max_hp = 10;
    int current_hp = 10;
    int base_attack = 1;
    std::string attack_dice = "1d6";    // D&D dice notation
    int base_defend = 1;
    std::string defend_dice = "1d4";
    int speed = 5;                      // Initiative/turn order
    int attack_range = 1;               // Melee vs ranged
};

enum class DamageType {
    Physical, Fire, Cold, Lightning, Poison
};
```

#### Concrete Characters

**Dragon** ([Dragon.h](DragonicTactics/source/Game/DragonicTactics/Objects/Dragon.h)):
- Player-controlled character
- Higher HP and balanced stats
- Full spell system access

**Fighter** ([Fighter.h](DragonicTactics/source/Game/DragonicTactics/Objects/Fighter.h)):
- AI opponent
- High HP, melee focus
- Limited spellcasting

### 2. Grid System ([Game/DragonicTactics/StateComponents/GridSystem.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.h))

**Type**: GameState Component (extends `CS230::Component`)

**Grid Configuration**:
- **Size**: 8x8 tiles
- **Tile Size**: 64 pixels
- **Total Area**: 512x512 pixels

**Tile Types**:
```cpp
enum class TileType {
    Empty,       // Walkable, no penalties
    Wall,        // Impassable obstacle
    Lava,        // Damage over time
    Difficult,   // Extra movement cost
    Invalid      // Out of bounds
};
```

**Key Methods**:
```cpp
// Tile queries
bool IsWalkable(Math::ivec2 tile) const;
bool IsOccupied(Math::ivec2 tile) const;
bool IsValidTile(Math::ivec2 tile) const;
TileType GetTileType(Math::ivec2 tile) const;

// Character management
void AddCharacter(Character* character, Math::ivec2 position);
void RemoveCharacter(Character* character);
void MoveCharacter(Character* character, Math::ivec2 new_position);
Character* GetCharacterAt(Math::ivec2 position) const;

// Rendering
void Draw(Math::TransformationMatrix camera_matrix);
```

### 3. Turn Management ([Game/DragonicTactics/StateComponents/TurnManager.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.h))

**Type**: Singleton pattern

**Turn Flow**:
1. `InitializeTurnOrder(characters)` - Sort by speed stat (initiative)
2. `StartNextTurn()` - Activate next character in order
3. Character performs actions (move, attack, cast spell)
4. `EndCurrentTurn()` - Deactivate character, refresh resources
5. Repeat until combat ends

**Key Methods**:
```cpp
void InitializeTurnOrder(std::vector<Character*> characters);
void StartNextTurn();
void EndCurrentTurn();

Character* GetCurrentCharacter() const;
int GetCurrentTurnNumber() const;
int GetRoundNumber() const;
bool IsCombatActive() const;

// Event publishing
void PublishTurnStartEvent(Character* character);
void PublishTurnEndEvent(Character* character);
```

### 4. Event System ([Game/DragonicTactics/Singletons/EventBus.h](DragonicTactics/source/Game/DragonicTactics/Singletons/EventBus.h))

**Type**: Singleton with type-erased event dispatch

**Pattern**: Publisher-Subscriber (Pub/Sub)

**Usage Example**:
```cpp
// Subscribe to events
EventBus::Instance().Subscribe<CharacterDamagedEvent>(
    [](const CharacterDamagedEvent& event) {
        Engine::GetLogger().LogEvent("Character took " +
            std::to_string(event.damage) + " damage");
    }
);

// Publish events
CharacterDamagedEvent event{target, damage, attacker, damage_type};
EventBus::Instance().Publish(event);
```

**Event Categories** ([Game/DragonicTactics/Types/Events.h](DragonicTactics/source/Game/DragonicTactics/Types/Events.h)):

**Battle Events**:
- `BattleStartedEvent` - Combat initialization
- `BattleEndedEvent` - Combat conclusion
- `VictoryConditionMetEvent` - Win condition triggered

**Combat Events**:
- `CharacterDamagedEvent` - Damage applied
- `CharacterHealedEvent` - Healing applied
- `CharacterDeathEvent` - Character eliminated
- `AttackMissedEvent` - Attack failed

**Movement Events**:
- `CharacterMovedEvent` - Position changed
- `MovementBlockedEvent` - Movement prevented

**Spell Events**:
- `SpellCastEvent` - Spell execution
- `SpellEffectAppliedEvent` - Effect resolution
- `SpellSlotConsumedEvent` - Resource depletion
- `StatusEffectAddedEvent`, `StatusEffectRemovedEvent`, `StatusEffectTickEvent`

**Turn Events**:
- `TurnStartedEvent`, `TurnEndedEvent`
- `InitiativeRolledEvent` - Turn order established

**UI Events**:
- `UIActionSelectedEvent`
- `UITileHoveredEvent`
- `UISpellSelectedEvent`

### 5. Dice & Combat Systems

#### DiceManager ([Game/DragonicTactics/Singletons/DiceManager.h](DragonicTactics/source/Game/DragonicTactics/Singletons/DiceManager.h))

**Type**: Singleton with Mersenne Twister RNG

**D&D Dice Notation Support**:
```cpp
// Roll 3d6 (3 six-sided dice)
int result = DiceManager::Instance().RollDiceFromString("3d6");

// Roll 2d8+5 (2 eight-sided dice plus 5)
int result = DiceManager::Instance().RollDiceFromString("2d8+5");

// Manual roll
int result = DiceManager::Instance().RollDice(3, 6); // 3d6

// Get individual roll results
std::vector<int> rolls = DiceManager::Instance().GetLastRolls();
```

**Methods**:
- `RollDice(count, sides)` - Basic dice roll
- `RollDiceFromString(notation)` - Parse "3d6+2" format
- `SetSeed(seed)` - Deterministic testing
- `GetLastRolls()` - Roll history for UI display

#### CombatSystem ([Game/DragonicTactics/Singletons/CombatSystem.h](DragonicTactics/source/Game/DragonicTactics/Singletons/CombatSystem.h))

**Type**: Singleton pattern

**Combat Resolution**:
```cpp
// Full attack sequence
CombatSystem::Instance().ExecuteAttack(attacker, defender);

// Damage calculation
int damage = CombatSystem::Instance().CalculateDamage(
    attacker, defender, damage_type
);

// Apply damage
CombatSystem::Instance().ApplyDamage(target, damage, damage_type);

// Range/positioning
bool in_range = CombatSystem::Instance().IsInRange(
    attacker, defender
);
int distance = CombatSystem::Instance().GetDistance(
    attacker->GetGridPosition(), defender->GetGridPosition()
);
```

**Key Methods**:
- `ExecuteAttack(attacker, defender)` - Full attack resolution
- `CalculateDamage(attacker, defender, type)` - Roll attack dice
- `ApplyDamage(target, amount, type)` - Apply damage with events
- `RollAttackDamage(character)` - Parse attack_dice string
- `IsCriticalHit()` - Critical hit detection (Week 4 feature)
- `IsInRange(attacker, target)` - Range validation
- `GetDistance(pos1, pos2)` - Grid distance calculation

### 6. Action System

#### Action Base Class ([Game/DragonicTactics/Objects/Actions/Action.h](DragonicTactics/source/Game/DragonicTactics/Objects/Actions/Action.h))

**Purpose**: Encapsulate character actions (attacks, spells, items)

**Concrete Actions**:
- `ActionAttack` ([ActionAttack.h](DragonicTactics/source/Game/DragonicTactics/Objects/Actions/ActionAttack.h)) - Physical attack action

**Usage Pattern**:
```cpp
// Character performs action
ActionAttack attack_action;
character->PerformAction(&attack_action, target, target_tile);
```

---

## File Organization

```
CodePistols_DragonicTactics/
├── CMakeLists.txt                              # Root CMake config
├── CMakePresets.json                           # Build presets
├── CLAUDE.md                                   # This file
├── README.md                                   # Project README
├── docs/                                       # Architecture documentation
│   ├── index.md                                # Documentation navigation
│   ├── architecture.md                         # High-level architecture
│   ├── implementation-plan.md                  # 26-week timeline
│   ├── systems/                                # System specifications
│   ├── debug/                                  # Debug tools docs
│   └── Detailed Implementations/weeks/         # Week-by-week guides
│
├── architecture/                               # Supporting documents
│   ├── dragonic_tactics.md                     # Original design (Korean)
│   ├── game_architecture_rules.md              # Architecture principles
│   ├── Architecture_Document.md                # Academic template
│   └── Rubric-EngineProof.md                   # Grading rubric
│
├── DragonicTactics/                            # Main project root
│   ├── CMakeLists.txt                          # Project CMake config
│   ├── cmake/                                  # CMake modules
│   │   ├── Dependencies.cmake                  # FetchContent dependencies
│   │   ├── CompilerWarnings.cmake              # Warning settings
│   │   └── StandardProjectSettings.cmake       # Build configuration
│   │
│   ├── source/                                 # All source code
│   │   ├── main.cpp                            # Application entry point
│   │   ├── CMakeLists.txt                      # Source build config
│   │   │
│   │   ├── Engine/                             # Core engine (CS230 namespace)
│   │   │   ├── Engine.hpp/cpp                  # Main engine singleton
│   │   │   ├── GameObject.h/cpp                # Entity base class
│   │   │   ├── GameState.hpp                   # State interface
│   │   │   ├── GameStateManager.hpp/cpp        # State machine
│   │   │   ├── Component.h                     # Component interface
│   │   │   ├── ComponentManager.h/cpp          # Component management
│   │   │   ├── GameObjectManager.h/cpp         # Entity list management
│   │   │   ├── Window.hpp/cpp                  # SDL window
│   │   │   ├── Input.hpp/cpp                   # Keyboard input
│   │   │   ├── Logger.hpp/cpp                  # Logging system
│   │   │   ├── TextureManager.hpp/cpp          # Asset management
│   │   │   ├── TextManager.hpp/cpp             # Text rendering
│   │   │   ├── Vec2.hpp/cpp                    # Math::vec2
│   │   │   ├── Matrix.hpp/cpp                  # Math::TransformationMatrix
│   │   │   ├── Rect.hpp                        # Rectangle bounds
│   │   │   ├── Random.hpp/cpp                  # RNG utilities
│   │   │   ├── Sprite.h/cpp                    # Sprite rendering
│   │   │   ├── Animation.h/cpp                 # Frame-based animation
│   │   │   ├── Texture.hpp/cpp                 # Texture wrapper
│   │   │   ├── Camera.h/cpp                    # Camera transformation
│   │   │   ├── Particle.h/cpp                  # Particle effects
│   │   │   └── ShowCollision.h/cpp             # Debug collision viz
│   │   │
│   │   ├── Game/                               # Game-specific code
│   │   │   ├── States.h                        # Game state enum
│   │   │   ├── Splash.h/cpp                    # Splash screen state
│   │   │   ├── MainMenu.h/cpp                  # Main menu state
│   │   │   │
│   │   │   ├── CS230_Final/                    # Legacy final project
│   │   │   │   ├── Cat.h/cpp                   # Platformer character
│   │   │   │   └── Robot.h/cpp                 # Enemy character
│   │   │   │
│   │   │   └── DragonicTactics/                # Dragonic Tactics game
│   │   │       │
│   │   │       ├── Abilities/                  # Character abilities
│   │   │       │   ├── AbilityBase.h           # Ability interface
│   │   │       │   ├── MeleeAttack.h/cpp       # Melee attack ability
│   │   │       │   └── ShieldBash.h/cpp        # Fighter shield bash
│   │   │       │
│   │   │       ├── Objects/                    # Game entities
│   │   │       │   ├── Character.h/cpp         # Base character class
│   │   │       │   ├── Dragon.h/cpp            # Player dragon
│   │   │       │   ├── Fighter.h/cpp           # Opponent fighter
│   │   │       │   │
│   │   │       │   ├── Components/             # Character components
│   │   │       │   │   ├── GridPosition.h/cpp  # Grid coordinates
│   │   │       │   │   ├── StatsComponent.h/cpp # Combat stats
│   │   │       │   │   ├── ActionPoints.h/cpp  # Movement budget
│   │   │       │   │   └── SpellSlots.h/cpp    # Magic system
│   │   │       │   │
│   │   │       │   └── Actions/                # Action system
│   │   │       │       ├── Action.h/cpp        # Action interface
│   │   │       │       └── ActionAttack.h/cpp  # Attack action
│   │   │       │
│   │   │       ├── Singletons/                 # Global services
│   │   │       │   ├── EventBus.h/cpp          # Event system
│   │   │       │   ├── DiceManager.h/cpp       # Dice rolling
│   │   │       │   ├── CombatSystem.h/cpp      # Combat resolution
│   │   │       │   ├── SpellSystem.h/cpp       # Spell casting system
│   │   │       │   ├── TurnManager.h/cpp       # Turn management singleton
│   │   │       │   └── DataRegistry.h/cpp      # Config data storage
│   │   │       │
│   │   │       ├── StateComponents/            # GameState components
│   │   │       │   ├── GridSystem.h/cpp        # 8x8 tactical grid
│   │   │       │   ├── TurnManager.h/cpp       # Turn management component
│   │   │       │   └── AStar.cpp               # A* pathfinding implementation
│   │   │       │
│   │   │       ├── States/                     # Game states
│   │   │       │   ├── BattleState.h/cpp       # Main combat state
│   │   │       │   ├── GamePlay.h/cpp          # Gameplay state
│   │   │       │   ├── RenderingTest.h/cpp     # Rendering test state
│   │   │       │   ├── ConsoleTest.h/cpp       # Console test state
│   │   │       │   ├── Test.h                  # Test interface
│   │   │       │   ├── Test_Ginam.cpp          # Ginam's test state
│   │   │       │   ├── Test_Junyoung.cpp       # Junyoung's test state
│   │   │       │   ├── Test_Seungju.cpp        # Seungju's test state
│   │   │       │   ├── Test_SY.cpp             # SY's test state
│   │   │       │   └── Test_Taekyung.cpp       # Taekyung's test state
│   │   │       │
│   │   │       ├── Types/                      # Shared types
│   │   │       │   ├── GameObjectTypes.h       # Entity type enum
│   │   │       │   ├── CharacterTypes.h        # Character types/stats
│   │   │       │   ├── GameTypes.h             # Shared game types
│   │   │       │   └── Events.h                # Event definitions
│   │   │       │
│   │   │       ├── Debugger/                   # Debug tools
│   │   │       │   ├── DebugManager.h/cpp      # Debug system manager
│   │   │       │   └── DebugConsole.h/cpp      # Console interface
│   │   │       │
│   │   │       ├── External/                   # External integrations
│   │   │       │   └── (placeholder)
│   │   │       │
│   │   │       └── Test/                       # Test implementations
│   │   │           ├── TestAssert.h/cpp        # Test assertion utilities
│   │   │           ├── TestAStar.h/cpp         # A* pathfinding tests
│   │   │           ├── Week1TestMocks.h/cpp    # Week 1 mock characters
│   │   │           └── Week3TestMocks.h/cpp    # Week 3 mock objects (spells, treasure)
│   │   │
│   │   ├── CS200/                              # Rendering abstraction
│   │   │   ├── IRenderer2D.hpp                 # 2D rendering interface
│   │   │   ├── ImmediateRenderer2D.hpp/cpp     # Immediate mode impl
│   │   │   ├── RenderingAPI.hpp/cpp            # OpenGL wrapper
│   │   │   ├── RGBA.hpp                        # Color type
│   │   │   ├── Image.hpp/cpp                   # Image loading
│   │   │   └── ImGuiHelper.hpp/cpp             # ImGui integration
│   │   │
│   │   ├── OpenGL/                             # Low-level OpenGL
│   │   │   ├── GL.hpp/cpp                      # OpenGL initialization
│   │   │   ├── Shader.hpp/cpp                  # Shader management
│   │   │   ├── VertexArray.hpp/cpp             # VAO wrapper
│   │   │   ├── Buffer.hpp/cpp                  # VBO/IBO wrapper
│   │   │   ├── Framebuffer.hpp/cpp             # FBO wrapper
│   │   │   └── Texture.hpp/cpp                 # Texture management
│   │   │
│   │   └── Demo/                               # Graphics demos
│   │       ├── DemoShapes.hpp/cpp
│   │       ├── DemoTexturing.hpp/cpp
│   │       └── [Other demos]
│   │
│   ├── Assets/                                 # Game assets
│   │   ├── *.png                               # Texture images
│   │   ├── *.spt                               # Sprite definitions
│   │   └── *.anm                               # Animation sequences
│   │
│   ├── app_resources/                          # Application resources
│   │   └── windows/icon.ico                    # App icon
│   │
│   └── build/                                  # CMake output (gitignored)
│       ├── windows-debug/
│       ├── windows-developer-release/
│       └── windows-release/
```

---

## Development Guidelines

### Core Patterns

#### Namespaces
- **CS230**: Core engine systems (Engine, GameObject, GameState, Component)
- **CS200**: Rendering abstraction layer (IRenderer2D, RenderingAPI)
- **OpenGL**: Low-level OpenGL wrappers
- **Math**: Math utilities (vec2, ivec2, TransformationMatrix)

#### GameObject Development
```cpp
class MyCharacter : public CS230::GameObject {
public:
    GameObjectTypes Type() override { return GameObjectTypes::Character; }
    std::string TypeName() override { return "MyCharacter"; }

    void Update(double dt) override {
        // Game logic
    }

    void Draw(Math::TransformationMatrix camera_matrix) override {
        // Rendering
    }
};
```

#### Component Development
```cpp
class MyComponent : public CS230::Component {
public:
    void Update(double dt) override {
        // Component logic
    }
};

// Usage in GameObject
void MyGameObject::Load() {
    AddGOComponent(new MyComponent());
}

void MyGameObject::Update(double dt) {
    auto comp = GetGOComponent<MyComponent>();
    comp->DoSomething();
}
```

#### GameState Development
```cpp
class MyGameState : public CS230::GameState {
public:
    void Load() override {
        // Initialize resources
        AddComponent(new GridSystem());
    }

    void Update(double dt) override {
        UpdateComponents(dt);
        game_object_manager_.UpdateAll(dt);
    }

    void Draw() override {
        game_object_manager_.DrawAll(GetCamera());
    }

    void Unload() override {
        // Cleanup
    }
};
```

### Event-Driven Development

**Publishing Events**:
```cpp
// Create event
CharacterDamagedEvent event{
    .target = target_character,
    .damage = damage_amount,
    .attacker = attacker_character,
    .damage_type = DamageType::Physical
};

// Publish to all subscribers
EventBus::Instance().Publish(event);
```

**Subscribing to Events**:
```cpp
// In Load() or constructor
EventBus::Instance().Subscribe<CharacterDamagedEvent>(
    [this](const CharacterDamagedEvent& event) {
        // Handle event
        UpdateHealthBar(event.target);
        PlayDamageAnimation(event.target);
    }
);
```

### Singleton Access

All singletons use `Instance()` static method:
```cpp
EventBus::Instance().Publish(event);
DiceManager::Instance().RollDice(3, 6);
CombatSystem::Instance().ExecuteAttack(attacker, defender);
DataRegistry::Instance().LoadCharacterData("dragon.json");
```

### Engine Subsystem Access

Access via `Engine::GetX()` static methods:
```cpp
Engine::GetLogger().LogEvent("Message");
Engine::GetInput().IsKeyPressed(InputKey::Space);
Engine::GetWindow().GetSize();
Engine::GetGameStateManager().SetNextGameState<BattleState>();
```

### Math Utilities

**2D Vectors**:
```cpp
Math::vec2 position{100.0f, 200.0f};
Math::vec2 velocity{1.0f, 0.0f};
position += velocity * dt;
```

**Integer Vectors (Grid Coordinates)**:
```cpp
Math::ivec2 tile{3, 5};  // Row 3, Column 5
auto grid_pos = character->GetGOComponent<GridPosition>();
grid_pos->SetPosition(tile);
```

**Transformation Matrices**:
```cpp
Math::TransformationMatrix transform;
transform = Math::TranslationMatrix(position) *
            Math::RotationMatrix(rotation) *
            Math::ScaleMatrix(scale);
sprite.Draw(transform);
```

### Collision Detection

**GameObject-level Collision**:
```cpp
bool CanCollideWith(GameObjectTypes other_type) override {
    return other_type == GameObjectTypes::Character;
}

void ResolveCollision(CS230::GameObject* other) override {
    // Handle collision response
}
```

**Grid-level Collision**:
```cpp
GridSystem* grid = GetComponent<GridSystem>();
if (grid->IsWalkable(target_tile) && !grid->IsOccupied(target_tile)) {
    grid->MoveCharacter(character, target_tile);
}
```

### Debugging

**Logging**:
```cpp
Engine::GetLogger().LogVerbose("Detailed info");
Engine::GetLogger().LogDebug("Debug info");
Engine::GetLogger().LogEvent("Important event");
Engine::GetLogger().LogError("Error message");
```

**ImGui Debug UI**:
```cpp
void MyGameState::DrawImGui() override {
    ImGui::Begin("Debug Window");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Character HP: %d", character->GetHP());
    if (ImGui::Button("Damage Character")) {
        character->TakeDamage(10, DamageType::Physical);
    }
    ImGui::End();
}
```

**Test Keyboard Shortcuts** (in Test states):
- **F**: Test EventBus
- **E**: Test DiceManager
- **T/Y**: Display character status
- **D/H**: Attack/Heal actions
- **S**: Show grid visualization
- **P**: Test TurnManager
- **J/R/L**: JSON data loading
- **Enter**: Run full combat test

### Asset Management

**Loading Textures**:
```cpp
// Automatic loading via TextureManager
Texture* texture = Engine::GetTextureManager().Load("Assets/character.png");
```

**Sprite Definitions** (.spt files):
```
Assets/character.png
frame_count frame_width frame_height
```

**Animation Definitions** (.anm files):
```
Assets/character.spt
frame_duration
frame1 frame2 frame3 ...
```

---

## Engine Systems Reference

### Frame Rate
- **Target FPS**: Not fixed (uses delta time for frame-independent logic)
- **Delta Time**: Passed to `Update(dt)` methods for time-based calculations
- **Frame Stats**: Displayed in ImGui overlay (if enabled)

### Input System
```cpp
Input& input = Engine::GetInput();

// Key states
if (input.IsKeyDown(InputKey::W)) { /* Held down */ }
if (input.IsKeyPressed(InputKey::Space)) { /* Just pressed this frame */ }
if (input.IsKeyReleased(InputKey::Escape)) { /* Just released this frame */ }
```

**Available Keys** (InputKey enum):
- A-Z (letter keys)
- Num0-Num9 (number keys)
- Space, Enter, Escape, Tab
- Left, Right, Up, Down (arrow keys)

### Game State Flow

**State Transitions**:
```cpp
Engine::GetGameStateManager().SetNextGameState<BattleState>();
```

**Current Implementation**:
- Splash → MainMenu → BattleState (Dragonic Tactics combat)

**State Lifecycle**:
1. `Load()` - Initialize resources
2. `Update(dt)` - Frame-by-frame logic
3. `Draw()` - Render game objects
4. `DrawImGui()` - Debug UI overlay
5. `Unload()` - Cleanup on state change

---

## Project Architecture Documentation

This repository contains comprehensive architecture documentation for the **Dragonic Tactics** tactical RPG project.

### Required Reading for Context

**Documentation Navigation**: See [docs/index.md](docs/index.md) for complete documentation map.

**Read these files based on your task**:

1. **[docs/architecture.md](docs/architecture.md)** - High-level architecture overview
   - Class hierarchies and relationships
   - Architectural patterns (Factory, Singleton, Component)
   - System interaction diagrams

2. **[docs/implementation-plan.md](docs/implementation-plan.md)** - 26-week development timeline (strategic)
   - Phase-by-phase implementation strategy
   - Dependency chains between systems
   - When to build each system

3. **[docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/)** - Week-by-week tactical guides (for active implementation)
   - [week1.md](docs/Detailed%20Implementations/weeks/week1.md) - Week 1: Foundation (5 systems in parallel) - IMPLEMENTED
   - [week2.md](docs/Detailed%20Implementations/weeks/week2.md) - Week 2: Dragon + Grid integration - IMPLEMENTED
   - [week3.md](docs/Detailed%20Implementations/weeks/week3.md) - Week 3: Combat + Spells - IN PROGRESS
   - [week4.md](docs/Detailed%20Implementations/weeks/week4.md) - Week 4: Spell expansion
   - [week5.md](docs/Detailed%20Implementations/weeks/week5.md) - Week 5: Polish + Playtest 1
   - Complete code examples, test cases, daily task breakdowns

4. **[docs/systems/](docs/systems/)** - Detailed system specifications (read as needed):
   - [characters.md](docs/systems/characters.md) - Character class implementations
   - [singletons.md](docs/systems/singletons.md) - Global service specifications
   - [game-state-components.md](docs/systems/game-state-components.md) - Battle system details
   - [game-object-components.md](docs/systems/game-object-components.md) - Component behaviors
   - [interfaces.md](docs/systems/interfaces.md) - Interface design & factory patterns

5. **[docs/debug/](docs/debug/)** - Debug tools (for development workflow):
   - [tools.md](docs/debug/tools.md) - Debug system specification
   - [commands.md](docs/debug/commands.md) - Console command examples
   - [ui.md](docs/debug/ui.md) - Debug UI layout

6. **[architecture/dragonic_tactics.md](architecture/dragonic_tactics.md)** - Original game design (Korean)

### Workflow for /init or New Sessions

When starting a new session or receiving `/init`:

1. Start with [docs/index.md](docs/index.md) - Documentation navigation hub
2. Read [docs/architecture.md](docs/architecture.md) for overall project vision
3. **Check this CLAUDE.md** to understand current implementation status (Weeks 1-2 complete)
4. **For active implementation**: Read relevant week file in [docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/)
5. For specific system design questions, navigate to relevant `docs/systems/*.md` file
6. Check [docs/implementation-plan.md](docs/implementation-plan.md) for timeline context

**Key Principle**: Each documentation file focuses on a single topic area (<1050 lines). Read only what you need for your current task.

### Dragonic Tactics Project Context

**Project Type**: Year-long tactical RPG development (26 weeks)

**Base Engine**: Custom C++20 OpenGL engine with CMake (THIS REPOSITORY)

**Game Genre**: Turn-based tactical RPG (inspired by D&D/Baldur's Gate 3)

**Key Systems Implemented (Weeks 1-2)**:
- Grid-based tactical combat (8x8 battlefield)
- Turn management with speed-based initiative
- Dice-based damage system (D&D style: 3d6, 2d8, etc.)
- Event-driven architecture with EventBus singleton
- Character base class with components (GridPosition, Stats, ActionPoints, SpellSlots)
- Dragon (player) and Fighter (opponent) characters

**Key Systems In Development (Weeks 3-5)**:
- Full spell system with spell slots and upcasting
- AI system for 4 opponent character types (Fighter, Cleric, Wizard, Rogue)
- Data-driven design with JSON configuration
- Combat polish and balance
- UI/UX improvements

**Implementation Status**:
- ✅ Week 1: Foundation systems (EventBus, DiceManager, Character, Grid, TurnManager)
- ✅ Week 2: Dragon + Fighter characters, Grid integration
- 🚧 Week 3: Combat system refinement, Spell system
- ⏳ Week 4: Spell expansion, Advanced combat
- ⏳ Week 5: Polish, Playtest 1

---

## Platform Support

### Windows (Native)
- Primary development platform
- MSVC v143 (Visual Studio 2022)
- Direct OpenGL rendering
- Console output in Debug/Developer builds
- Windowed application in Release builds

### WebAssembly (Emscripten)
- Cross-compile via `web-debug-on-windows` preset
- Outputs single HTML file with embedded assets
- Uses SDL2 + OpenGL ES
- Browser compatibility for demos/playtesting

---

## Additional Resources

- **Build Issues**: Check [DragonicTactics/README.md](DragonicTactics/README.md)
- **Contributing**: Follow C++20 Core Guidelines (GSL integrated)
- **Code Style**: 4-space indentation, descriptive naming, comments for complex logic
- **Version Control**: Git with main branch, feature branches for major systems
- **Testing**: Test states in [Game/DragonicTactics/States/Test_*.cpp](DragonicTactics/source/Game/DragonicTactics/States/)

---

## Summary for Claude Code

**CRITICAL CONTEXT**:
- This is a **NEW custom engine**, not the old CS230 MSBuild engine
- Build system: **CMake 3.21+** (not MSBuild)
- Language: **C++20** (not C++17)
- Platform: **SDL2 + OpenGL** (not pure Win32)
- Dependencies: **Automated via FetchContent** (OpenGL, GLEW, SDL2, ImGui, GSL, STB)
- **Weeks 1-2 systems are IMPLEMENTED** - refer to source code, not just documentation
- **Active development**: Week 3 (Combat + Spells)

When working on this project:
1. Use CMake build commands (not msbuild)
2. Reference actual implementation files in `DragonicTactics/source/`
3. Follow C++20 standards and modern practices
4. Use EventBus for inter-system communication
5. Leverage existing singletons (DiceManager, CombatSystem, etc.)
6. Write tests in Test states for new features
7. Log events/errors via Engine::GetLogger()
8. Use ImGui for debug visualization

**Entry Point**: [DragonicTactics/source/main.cpp](DragonicTactics/source/main.cpp)

**Core Headers to Review**:
- [Engine.hpp](DragonicTactics/source/Engine/Engine.hpp) - Engine singleton
- [GameObject.h](DragonicTactics/source/Engine/GameObject.h) - Entity base
- [Character.h](DragonicTactics/source/Game/DragonicTactics/Objects/Character.h) - Character base
- [EventBus.h](DragonicTactics/source/Game/DragonicTactics/Singletons/EventBus.h) - Event system
- [GridSystem.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.h) - Grid system
- [TurnManager.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.h) - Turn management

**Documentation Hub**: [docs/index.md](docs/index.md)
