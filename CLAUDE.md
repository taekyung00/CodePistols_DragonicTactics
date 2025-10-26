# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Recent Work Summary

### Session History & Accomplishments

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

- Created **ENGINE_COMPONENTS.md** - Complete reference of existing CS230 engine capabilities

  - Documented all engine systems (GameObject, Component, GameState, etc.)
  - Asset management (Sprite, Animation, Texture)
  - Input, Camera, Collision systems
  - Helps identify what exists vs. what needs to be built

#### Supporting Documentation

- **architecture/** directory contains supplementary documents:
  - **dragonic_tactics.md** - Korean language game design document (original specifications)
  - **game_architecture_rules.md** - Korean architectural principles (5 core rules for indie game development)
  - **Architecture_Document.md** - Academic milestone template for CS230 course
  - **Rubric-EngineProof.md** - Academic grading rubric

### Project Understanding

**Current State**: CS230 Engine with arcade-style games (Mode1: Ship/Asteroid, Mode2: Cat platformer)

**Future State**: Dragonic Tactics - Turn-based tactical RPG built on CS230 engine

**Key Insight**: Architecture documents plan how to extend existing CS230 GameObject/Component systems for tactical RPG gameplay, NOT replace them.

### Quick Start Commands

- Build: `msbuild CS230_TaekyungHo.sln /p:Configuration=Debug /p:Platform=Win32`
- Run: `Debug/CS230.exe` or `Release/CS230.exe`
- Clean: `msbuild CS230_TaekyungHo.sln /t:Clean`

---

## Build System

This is a Visual Studio C++ project using MSBuild:

### Building

- **Visual Studio GUI**: Open `CS230_TaekyungHo.sln` and build (Ctrl+Shift+B)
- **Command Line**: `msbuild CS230_TaekyungHo.sln /p:Configuration=Debug /p:Platform=Win32`
- **Run**: Set CS230 as startup project and press F5, or run `Debug/CS230.exe` or `Release/CS230.exe`

### Configuration

- **Configurations**: Debug/Release builds for Win32 and x64
- **Language Standard**: C++17 with strict warnings and treat warnings as errors enabled
- **Platform Toolset**: v143 (Visual Studio 2022)
- **Warning Level**: Level 4 with warnings treated as errors
- **Assets**: Post-build step automatically copies `Assets/` folder to output directory
- **Dependencies**: No external libraries - pure Win32/OpenGL implementation

## Architecture Overview

This is a 2D game engine written in C++ with a component-based entity system:

### Core Engine (CS230 namespace)

- **Engine**: Singleton pattern managing all engine subsystems (Window, Logger, GameStateManager, Input, TextureManager, Fonts)
- **GameStateManager**: Handles game state transitions between Splash, MainMenu, Mode1, Mode2
- **GameObject**: Base class for all game entities with component system, collision detection, and state machine support
- **Component**: Interface for GameObject components (sprite rendering, physics, etc.)
- **ComponentManager**: Manages components attached to GameObjects

### Game Architecture

- **Game States**: Each state (Splash, MainMenu, Mode1, Mode2) inherits from GameState and manages its own GameObjectManager
- **Game Objects**: Specific entity types (Cat, Robot, Asteroid, Ship, Crates, Meteor, Floor, Portal, Score, Laser, Particle) inherit from GameObject
- **Asset System**: Sprites (.spt), animations (.anm), textures (.png) managed by TextureManager
- **Physics**: Built-in collision detection, velocity-based movement, and collision resolution
- **Rendering**: Sprite-based rendering with transformation matrices and camera support

### Key Design Patterns

- **Singleton**: Engine instance provides global access to subsystems
- **State Machine**: Both for game states and individual GameObject behavior
- **Component System**: GameObjects can have multiple components for different behaviors
- **Entity-Component**: GameObjects are entities, Components provide behavior

### Memory Management

- Manual memory management with proper cleanup in destructors
- Components owned by ComponentManager
- GameObjects managed by GameObjectManager within each GameState

## File Organization

```
CS230/
├── Engine/          # Core engine systems (CS230 namespace)
│   ├── Engine.h/cpp # Main engine singleton
│   ├── GameObject.h # Base entity class
│   ├── Component.h  # Component interface
│   └── ...          # Other engine systems
├── Game/            # Game-specific implementations
│   ├── States.h     # Game state enumeration
│   ├── Mode1.h/cpp  # Game mode implementations
│   └── ...          # Game entities (Cat, Robot, etc.)
├── Assets/          # Game assets (.spt, .anm, .png)
└── main.cpp         # Entry point
```

## Development Guidelines

### Core Patterns

- All engine code should be in the `CS230` namespace
- Game entities must override `Type()` and `TypeName()` methods returning `GameObjectTypes` enum values
- Access engine subsystems through `Engine::GetX()` static methods (Logger, Window, Input, etc.)
- Use `Math::vec2` for 2D vectors and `Math::TransformationMatrix` for transforms

### GameObject State Machine

GameObjects can define internal State classes for behavior management:

- Define states as nested classes inheriting from `GameObject::State`
- Implement `Enter()`, `Update()`, `CheckExit()`, and `GetName()` methods
- Switch states using `change_state()` method
- See Cat, Ship, or Robot classes for examples

### Components and Collision

- Use the component system for reusable behaviors via `AddGOComponent()` / `GetGOComponent<T>()`
- Collision detection is built into GameObject base class via `IsCollidingWith()` methods
- Override `CanCollideWith()` to filter collision checks by type
- Override `ResolveCollision()` to handle collision responses

### Debugging

- Use `Engine::GetLogger().LogError()` / `LogEvent()` / `LogDebug()` for logging
- Logger output goes to `Logger.txt` in the working directory

## Asset File Formats

- **Sprites (.spt)**: Define sprite sheets with frame information
- **Animations (.anm)**: Define animation sequences using sprite frames
- **Textures (.png)**: Standard PNG image files loaded by TextureManager
- Assets are automatically copied to output directory during build

## Engine Systems

### Frame Rate

- Target FPS: 30.0
- Fixed timestep with delta time passed to Update() methods
- Frame rate monitoring built into Engine class

### Input System

- Access via `Engine::GetInput()`
- Check key states with `IsKeyDown()`, `IsKeyPressed()`, `IsKeyReleased()`
- Key codes defined in `Input.h` (InputKey enum)

### Game State Flow

- States transition through `GameStateManager::SetNextGameState()`
- Current implementation: Splash → MainMenu → Mode1/Mode2
- Each GameState owns its GameObjectManager and handles its own Update/Draw cycles

---

## Project Architecture Documentation

This repository contains architecture documentation for the **Dragonic Tactics** tactical RPG project, which will be built on top of the CS230 engine.

### Required Reading for Context

**NEW: Documentation has been reorganized!** See [docs/index.md](docs/index.md) for complete navigation.

When working on this project, read these files based on your task:

1. **[docs/architecture.md](docs/architecture.md)** - High-level architecture overview
   - Class hierarchies and relationships
   - Architectural patterns (Factory, Singleton, Component)
   - System interaction diagrams

2. **[docs/implementation-plan.md](docs/implementation-plan.md)** - 26-week development timeline (strategic)
   - Phase-by-phase implementation strategy
   - Dependency chains
   - When to build each system

3. **[docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/)** - Week-by-week tactical guides (for active implementation)
   - [week1.md](docs/Detailed%20Implementations/weeks/week1.md) - Week 1: Foundation (5 systems in parallel)
   - [week2.md](docs/Detailed%20Implementations/weeks/week2.md) - Week 2: Dragon + Grid integration
   - [week3.md](docs/Detailed%20Implementations/weeks/week3.md) - Week 3: Combat + Spells
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

7. **[ENGINE_COMPONENTS.md](ENGINE_COMPONENTS.md)** - Existing CS230 engine reference

### Workflow for /init or New Sessions

When starting a new session or receiving `/init`:

1. Start with [docs/index.md](docs/index.md) - Navigation hub
2. Read [docs/architecture.md](docs/architecture.md) for overall project vision
3. **For active implementation (Weeks 1-5)**: Read relevant week file in [docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/)
4. For specific system design questions, navigate to relevant `docs/systems/*.md` file
5. Check [docs/implementation-plan.md](docs/implementation-plan.md) for timeline context
6. Review [ENGINE_COMPONENTS.md](ENGINE_COMPONENTS.md) to understand existing CS230 capabilities

**Key Principle**: Each documentation file now focuses on a single topic area (<1050 lines). Read only what you need for your current task. 

### Dragonic Tactics Project Context

**Project Type**: Year-long tactical RPG development (26 weeks)

**Base Engine**: CS230 custom C++ engine (existing codebase in this repository)

**Game Genre**: Turn-based tactical RPG (inspired by D&D/Baldur's Gate 3)

**Key Systems to Add:**

- Grid-based tactical combat (8x8 battlefield)
- Turn management with speed-based initiative
- Dice-based damage system (D&D style: 3d6, 2d8, etc.)
- Spell system with spell slots and upcasting
- AI system for 4 opponent character types (Fighter, Cleric, Wizard, Rogue)
- Data-driven design with JSON configuration
- Event-driven architecture with EventBus singleton

**Important Context:**

- **Do NOT assume this is the current CS230 assignment** - The current repository contains Mode1/Mode2 arcade-style games (Ship, Asteroid, Cat platforming)
- **Dragonic Tactics is a FUTURE project** that will extend and build upon the existing CS230 engine
- The architecture documents plan how to leverage existing CS230 components (GameObject, Component system, GameState, etc.) while adding tactical RPG-specific systems