# Dragonic Tactics

## Summary

- Name of game: dragonic tactics

- Version: 0.1.3524

- Developers: joonyoung ki, seungju song, sangyun lee, taekyung ho, ginam park

- Team: code pistols

- Small description of game : This game reinterprets the battles of traditional DND campaigns, allowing players to become dragons and fight against a party of adventurers. Each character in the game has a turn. You play as a character called a dragon, and you have to chase away the adventurers who invade your house. Click the buttons to decide your actions, move and attack. You can do everything with a click of the mouse.

## Before Starting / Installation Instructions

- Setup your [Development Environment](docs/DevEnvironment.md)

## How to Build and Run

### Command Line Build

**Windows**

```sh
# configure / generate build files
cmake --preset windows-debug
cmake --preset windows-developer-release
cmake --preset windows-release
cmake --preset web-debug-on-windows

# Build exe
cmake --build --preset windows-debug
cmake --build --preset windows-developer-release
cmake --build --preset windows-release
cmake --build --preset web-debug-on-windows
```

**Linux**
*Causion : even if we can build linux version, linux version can be relatively slow if you run this build file on virtual machine like WSL. It is recommended to build and run windows version or web version on such environment.*

```sh
# configure / generate build files
cmake --preset linux-debug
cmake --preset linux-developer-release
cmake --preset linux-release
cmake --preset web-debug
cmake --preset web-developer-release
cmake --preset web-release

# Build exe
cmake --build --preset linux-debug
cmake --build --preset linux-developer-release
cmake --build --preset linux-release
cmake --build --preset web-debug
cmake --build --preset web-developer-release
cmake --build --preset web-release
```

### Automated Build Script

For convenience, use the automated build script that handles all configurations:

```sh
python3 scripts/scan_build_project.py
```

See the script help for filtering options:

```sh
python3 scripts/scan_build_project.py --help
```

## Build Configurations

This project supports three distinct build configurations, each designed for different stages of development and deployment:

### **Debug Configuration**

- **Purpose**: Active development and debugging
- **Features**: 
  - Allows setting breakpoints to step through code line-by-line
  - No compiler optimizations (preserves variable values for debugging)
  - Console window for viewing debug output
  - All developer helper features enabled
- **Performance**: Slower execution, larger executable size
- **When to use**: When you need to debug issues or understand code flow

### **Developer-Release Configuration**

- **Purpose**: Testing with optimized performance while keeping developer tools
- **Features**:
  - Compiler optimizations enabled (faster execution)
  - Cannot set breakpoints (optimizations interfere with debugging)
  - Console window for viewing output
  - Verbose logging and error checking enabled
  - Developer assertions active
- **Performance**: Fast execution, optimized code
- **When to use**: Performance testing while retaining diagnostic capabilities

### **Release Configuration**

- **Purpose**: Final builds for end users
- **Features**:
  - Full compiler optimizations
  - No console window (clean user experience)
  - Minimal logging (only critical errors)
  - No developer features or extra error checking
  - Smallest executable size
- **Performance**: Maximum speed, production-ready
- **When to use**: Final builds for distribution to users

## CMake Presets

CMake presets simplify the build process by pre-configuring complex settings. Instead of manually specifying:

- Compiler flags and optimization levels
- Output directories and build paths  
- Platform-specific configurations
- Dependency locations
- Debug/release-specific settings

The presets handle all of this automatically. For example, `cmake --preset linux-debug` is equivalent to running:

```sh
cmake -S . -B build/linux-debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DDEVELOPER_VERSION=ON \
  [... many more flags ...]
```

This saves time and reduces configuration errors, especially when working across different platforms and build types.

## Play



New Things

- We are working on Spells, Now only some of the dragon's spell is working.

- Please Let us know other spell's are not working

#### Spell Table

- **ID: S_DEB_020**
  
  - **Name**: Fearful Cry
  
  - **Category**: (N/A)
  
  - **Classes**: Dragon, Fighter
  
  - **Required Slot Level**: 1
  
  - **Targeting**: Enemy:Around:3
  
  - **Upcasting**: False
  
  - **Effect**: Deals 0 damage. Applies "Fear" status for 3 turns. Move to current location. Summons NULL at current location.

**ID: S_ATK_010**

- **Name**: Fire Bolt

- **Category**: Attack

- **Classes**: Dragon, Wizard

- **Required Slot Level**: 1

- **Targeting**: Enemy:Single:4

- **Upcasting**: True

- **Effect**: Deals 2d8 + (\text{Spell Level} - \text{Required Spell Level})d6 damage. Applies "Basic" status for 0 turns. Move to current location. Summons NULL at current location.

**ID: S_ENH_040**

- **Name**: Mana Conversion

- **Category**: Buff

- **Classes**: Dragon, Wizard

- **Required Slot Level**: 0

- **Targeting**: Self:Single:0

- **Upcasting**: True

- **Effect**: Deals 0 + (\text{Spell Level} - \text{Required Spell Level} + 1)d10 damage. Applies "Basic" status for 0 turns. Move to current location. Summons NULL at current location. Special: Recover a Spell Slot of (\text{Spell Level} - \text{Required Spell Level} + 1) level.

**ID: S_ATK_020**

- **Name**: Tail Swipe

- **Category**: Attack

- **Classes**: Dragon

- **Required Slot Level**: 2

- **Targeting**: Enemy:Around:2

- **Upcasting**: False

- **Effect**: Deals 1d8 damage. Applies "Basic" status for 0 turns. Move to target:knockback:2. Summons NULL at current location.

**ID: S_ATK_030**

- **Name**: Dragon's Fury

- **Category**: Attack

- **Classes**: Dragon

- **Required Slot Level**: 3

- **Targeting**: Enemy:Line:4

- **Upcasting**: True

- **Effect**: Deals 4d6 + (\text{Spell Level} - \text{Required Spell Level}) \times 2d6 damage. Applies "Basic" status for 0 turns. Move to current location. Summons NULL at current location.

**ID: S_ATK_040**

- **Name**: Meteor

- **Category**: Attack

- **Classes**: Dragon

- **Required Slot Level**: 3

- **Targeting**: Any:OddEven:-1

- **Upcasting**: True

- **Effect**: Deals 3d20 + (\text{Spell Level} - \text{Required Spell Level}) \times 1d20 damage. Applies "Exhaustion" status for 1 turn. Move to current location. Summons NULL at current location.

**ID: S_ENH_050**

- **Name**: Purify

- **Category**: Buff

- **Classes**: Dragon

- **Required Slot Level**: 1

- **Targeting**: Any:Around:4

- **Upcasting**: False

- **Effect**: Deals 0 damage. Applies "Purify" status for 1 turn. Move to current location. Summons NULL at current location.

**ID: S_GEO_010**

- **Name**: ~~Magma Blast~~ << not working

- **Category**: Terrain Change

- **Classes**: Dragon

- **Required Slot Level**: 2

- **Targeting**: Empty:Point:6

- **Upcasting**: True

- **Effect**: Deals 0 damage. Applies "Basic" status for 0 turns. Move to current location. Summons Lava Zone at selected location.

**ID: S_GEO_020**

- **Name**: ~~Wall Creation~~ << not working

- **Category**: Terrain Change

- **Classes**: Dragon

- **Required Slot Level**: 1

- **Targeting**: Empty:Point:5

- **Upcasting**: True

- **Effect**: Deals 0 damage. Applies "Basic" status for 0 turns. Move to current location. Summons Wall at selected location.



- After building the project, run the executable file located in the build directory.
- For example, if you built the windows-debug preset, run ./build/windows-debug/dragonic_tactics.exe
- At the main menu, click "Dragonic Tactics" to start the game.
- When the game starts, turn order is determined by each character's speed stat.
- we can see the turn order at Combat Status window.
- We can choose player's action : Move, Action, End Turn.
- Move : click the grid where you want to move. Each player can move according to their speed stat. For example, if a player's speed stat is 4, the player can move up to 4 grids. Player can also move less than their speed stat, or mix move and action like move 2 grids, action, move 2 grids. Each character's speed stat is shown at right UI.
- Action - basic attack : click the "Action" button, then click attack to use basic attack. For now, only basic attack is implemented. If you click spell button, click right button to cancel. If enemy is in range of dragon(red one) - now it is 2 grids - click the enemy to attack. Damage is calculated by attacker's dice-based calculated attack amount minus defender's dice-based calculated defense amount. If enemy isn't in range, nothing happens. Dragon can attack based on its attack point. Attack point(AP) is shown at right UI. Each basic attack consumes 1 AP. 
- End Turn : click the "End Turn" button to end your turn. Even if you have remaining speed or AP, you can end your turn.
- If one of characters' HP reaches 0, game ends.

### 

## Feedback

- if you have any feedbacks, contact to here:
  - Email: taek020422@gamil.com
  - Discord: taekyung.ho