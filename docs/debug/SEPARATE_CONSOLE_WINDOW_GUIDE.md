# Separate Console Window Implementation Guide

**Author**: For Student Programmers (Sophomore CS Level)
**Purpose**: How to create a real separate console window for debug commands
**Build System**: CMake (Visual Studio + VSCode compatible)

---

## Table of Contents

1. [What We're Building](#what-were-building)
2. [How It Works](#how-it-works)
3. [Step-by-Step Implementation](#step-by-step-implementation)
4. [CMake Configuration](#cmake-configuration)
5. [Usage Examples](#usage-examples)
6. [Troubleshooting](#troubleshooting)

---

## What We're Building

We're creating a **separate console window** that runs alongside your game window:

```
┌─────────────────┐    ┌─────────────────┐
│  Game Window    │    │ Console Window  │
│                 │    │ > spawn dragon  │
│   [Dragon]      │    │ Dragon spawned! │
│   [Fighter]     │    │ > roll 3d6      │
│                 │    │ Result: 14      │
└─────────────────┘    └─────────────────┘
     (Graphics)           (Debug commands)
```

**Benefits**:
- Two separate windows you can position anywhere
- Real Windows console (no need to code text rendering!)
- Game keeps running while you type commands
- Only appears in Debug builds (automatic!)
- Works with CMake build system

---

## How It Works

### The Magic: `AllocConsole()`

Windows provides a function called `AllocConsole()` that creates a brand new console window for your program.

**Normally**: Your game is a "Windows Application" with no console
**After `AllocConsole()`**: Your game has BOTH a game window AND a console window!

```cpp
#include <windows.h>

// This one line creates the console window!
AllocConsole();
```

### Connecting C++ Streams

After creating the console, we need to "connect" C++ streams (`std::cout`, `std::cin`) to it:

```cpp
FILE* fp;
freopen_s(&fp, "CONOUT$", "w", stdout);  // printf/cout → console
freopen_s(&fp, "CONIN$", "r", stdin);    // cin ← console
freopen_s(&fp, "CONOUT$", "w", stderr);  // errors → console
```

**What this does**:
- `CONOUT$`: Special Windows name for "console output"
- `CONIN$`: Special Windows name for "console input"
- `freopen_s`: Redirects C++ streams to the console window

---

## Step-by-Step Implementation

### Step 1: Create DebugConsole Header

Create file: `CS230/Engine/DebugConsole.h`

```cpp
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace CS230 {

class DebugConsole {
public:
    // Singleton pattern (only one console exists)
    static DebugConsole& Instance();

    // Initialize console window (call at game startup)
    void Initialize();

    // Shutdown console window (call at game exit)
    void Shutdown();

    // Register a debug command
    // Example: RegisterCommand("spawn", SpawnHandler, "spawn <type> <x> <y>")
    void RegisterCommand(
        const std::string& name,
        std::function<void(std::vector<std::string>)> handler,
        const std::string& helpText
    );

    // Check for console input (call every frame)
    void Update();

    // Print message to console
    void Log(const std::string& message);
    void LogError(const std::string& message);

private:
    DebugConsole() = default;
    ~DebugConsole() = default;

    // Disable copy/move (singleton)
    DebugConsole(const DebugConsole&) = delete;
    DebugConsole& operator=(const DebugConsole&) = delete;

    void ProcessCommand(const std::string& input);
    std::vector<std::string> ParseArguments(const std::string& input);

    struct Command {
        std::function<void(std::vector<std::string>)> handler;
        std::string helpText;
    };

    std::map<std::string, Command> commands;
    bool isInitialized = false;

#ifdef _WIN32
    void* consoleHandle = nullptr;  // HANDLE on Windows
#endif
};

} // namespace CS230
```

---

### Step 2: Implement DebugConsole

Create file: `CS230/Engine/DebugConsole.cpp`

```cpp
#include "DebugConsole.h"
#include <iostream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>  // For _kbhit()
#endif

namespace CS230 {

DebugConsole& DebugConsole::Instance() {
    static DebugConsole instance;
    return instance;
}

void DebugConsole::Initialize() {
#ifdef _WIN32
#ifdef _DEBUG
    // Only create console in Debug builds on Windows
    if (AllocConsole()) {
        // Redirect C++ streams to console
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        // Set console title
        SetConsoleTitle(L"Dragonic Tactics - Debug Console");

        // Get console handle for future use
        consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        // Welcome message
        std::cout << "========================================\n";
        std::cout << "  Dragonic Tactics Debug Console\n";
        std::cout << "========================================\n";
        std::cout << "Type 'help' for available commands\n";
        std::cout << "Type 'quit' to exit game\n";
        std::cout << "----------------------------------------\n";

        isInitialized = true;
    }
#endif // _DEBUG
#endif // _WIN32

    // Register built-in commands
    RegisterCommand("help",
        [this](std::vector<std::string> args) {
            std::cout << "\nAvailable Commands:\n";
            std::cout << "-------------------\n";
            for (const auto& [name, cmd] : commands) {
                std::cout << "  " << cmd.helpText << "\n";
            }
            std::cout << "\n";
        },
        "help - Show this message"
    );

    RegisterCommand("quit",
        [](std::vector<std::string> args) {
            std::cout << "Exiting game...\n";
            exit(0);
        },
        "quit - Exit the game"
    );

    RegisterCommand("clear",
        [](std::vector<std::string> args) {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
        },
        "clear - Clear console screen"
    );
}

void DebugConsole::Shutdown() {
#ifdef _WIN32
    if (isInitialized) {
        FreeConsole();
        isInitialized = false;
    }
#endif
}

void DebugConsole::RegisterCommand(
    const std::string& name,
    std::function<void(std::vector<std::string>)> handler,
    const std::string& helpText
) {
    commands[name] = { handler, helpText };
}

void DebugConsole::Update() {
#ifdef _WIN32
    if (!isInitialized) return;

    // Check if user pressed a key (non-blocking!)
    if (_kbhit()) {
        std::string input;
        std::getline(std::cin, input);

        if (!input.empty()) {
            ProcessCommand(input);
        }
    }
#endif
}

void DebugConsole::ProcessCommand(const std::string& input) {
    std::vector<std::string> args = ParseArguments(input);

    if (args.empty()) return;

    std::string commandName = args[0];
    args.erase(args.begin());  // Remove command name from args

    // Find and execute command
    auto it = commands.find(commandName);
    if (it != commands.end()) {
        try {
            it->second.handler(args);
        } catch (const std::exception& e) {
            LogError("Command failed: " + std::string(e.what()));
        }
    } else {
        LogError("Unknown command: " + commandName);
        std::cout << "Type 'help' for available commands\n";
    }
}

std::vector<std::string> DebugConsole::ParseArguments(const std::string& input) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string arg;

    while (iss >> arg) {
        args.push_back(arg);
    }

    return args;
}

void DebugConsole::Log(const std::string& message) {
    if (isInitialized) {
        std::cout << message << std::endl;
    }
}

void DebugConsole::LogError(const std::string& message) {
    if (isInitialized) {
#ifdef _WIN32
        // Print errors in red
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cerr << "[ERROR] " << message << std::endl;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
        std::cerr << "[ERROR] " << message << std::endl;
#endif
    }
}

} // namespace CS230
```

---

### Step 3: Initialize in Game

Update `CS230/Engine/Engine.cpp`:

```cpp
#include "DebugConsole.h"

void Engine::Init(const char* windowName) {
    logger.LogEvent("Engine Init");

    // Initialize window, input, etc...
    window.Init(windowName);

    // Initialize debug console (Debug builds only)
    DebugConsole::Instance().Initialize();

    logger.LogEvent("Engine Init Complete");
}

void Engine::Update(double dt) {
    // Check for console input every frame (non-blocking!)
    DebugConsole::Instance().Update();

    // Normal game update
    GetGameStateManager().Update(dt);
}

void Engine::Shutdown() {
    // Shutdown debug console
    DebugConsole::Instance().Shutdown();

    // Normal shutdown
    GetGameStateManager().Shutdown();
    GetTextureManager().Unload();
}
```

---

### Step 4: Register Game-Specific Commands

In your game state (e.g., `Mode1.cpp`):

```cpp
#include "Engine/DebugConsole.h"
#include "Singletons/DiceManager.h"

void Mode1::Load() {
    // Register debug commands for this game mode

    // Dice rolling test
    DebugConsole::Instance().RegisterCommand("roll",
        [](std::vector<std::string> args) {
            if (args.empty()) {
                std::cout << "Usage: roll <notation>\n";
                std::cout << "Example: roll 3d6\n";
                return;
            }

            int result = DiceManager::Instance().RollDiceFromString(args[0]);
            std::cout << "Rolled " << args[0] << ": " << result << "\n";
        },
        "roll <notation> - Roll dice (e.g., roll 3d6)"
    );

    // Spawn character test
    DebugConsole::Instance().RegisterCommand("spawn",
        [this](std::vector<std::string> args) {
            if (args.size() < 3) {
                std::cout << "Usage: spawn <type> <x> <y>\n";
                std::cout << "Example: spawn dragon 4 4\n";
                return;
            }

            std::string type = args[0];
            int x = std::stoi(args[1]);
            int y = std::stoi(args[2]);

            // Spawn logic here
            std::cout << "Spawned " << type << " at (" << x << ", " << y << ")\n";
        },
        "spawn <type> <x> <y> - Spawn character on grid"
    );
}
```

---

## CMake Configuration

### CMakeLists.txt for Windows Console

Add this to your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.15)
project(DragonicTactics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(ENGINE_SOURCES
    CS230/Engine/Engine.cpp
    CS230/Engine/GameObject.cpp
    CS230/Engine/DebugConsole.cpp
    # ... other engine files
)

set(GAME_SOURCES
    CS230/Game/Mode1.cpp
    CS230/Game/Mode2.cpp
    # ... other game files
)

# Create executable
add_executable(CS230
    CS230/main.cpp
    ${ENGINE_SOURCES}
    ${GAME_SOURCES}
)

# Windows-specific settings
if(WIN32)
    # Debug builds: Show console window
    # Release builds: Hide console window
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Console subsystem (console window visible)
        set_target_properties(CS230 PROPERTIES
            WIN32_EXECUTABLE FALSE
        )
    else()
        # Windows subsystem (no console)
        set_target_properties(CS230 PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
endif()

# Include directories
target_include_directories(CS230 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CS230
)

# Compile definitions
if(WIN32)
    target_compile_definitions(CS230 PRIVATE
        _WIN32
        $<$<CONFIG:Debug>:_DEBUG>
    )
endif()
```

### Building with CMake

#### Visual Studio:
```bash
# Generate Visual Studio solution
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32

# Open CS230.sln in Visual Studio
# Set configuration to Debug
# Press F5 to run
```

#### VSCode:
```bash
# Install CMake Tools extension in VSCode
# Open Command Palette (Ctrl+Shift+P)
# Type: "CMake: Configure"
# Select: "Debug"
# Press F5 to run
```

---

## Usage Examples

### Example 1: Rolling Dice

```
Console Window:
> roll 3d6
Rolled 3d6: 14

> roll 2d8+5
Rolled 2d8+5: 18
```

### Example 2: Spawning Characters

```
Console Window:
> spawn dragon 4 4
Spawned dragon at (4, 4)

> spawn fighter 2 3
Spawned fighter at (2, 3)
```

### Example 3: Getting Help

```
Console Window:
> help

Available Commands:
-------------------
  help - Show this message
  quit - Exit the game
  clear - Clear console screen
  roll <notation> - Roll dice (e.g., roll 3d6)
  spawn <type> <x> <y> - Spawn character on grid
```

---

## Troubleshooting

### Problem: Console window doesn't appear

**Solution**: Make sure you're running a Debug build!

```cmake
# Check your CMake configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build - Console will appear")
endif()
```

### Problem: Game freezes when typing in console

**Solution**: Make sure you're using `_kbhit()` for non-blocking input!

```cpp
// WRONG: This blocks the game!
std::string input;
std::cin >> input;  // Game stops here waiting for input

// CORRECT: Non-blocking check
if (_kbhit()) {
    std::string input;
    std::getline(std::cin, input);
}
```

### Problem: Console closes immediately when game exits

**Solution**: The console closes with your program (normal behavior). If you need to read error messages:

```cpp
// Option 1: Add pause before exit
std::cout << "Press Enter to exit...";
std::cin.get();

// Option 2: Run from existing console
// Open cmd.exe first, then run: CS230.exe
```

### Problem: `_kbhit()` not found

**Solution**: Include the correct header:

```cpp
#ifdef _WIN32
#include <conio.h>  // For _kbhit()
#endif
```

### Problem: Works in Visual Studio but not VSCode

**Solution**: Make sure VSCode is using the same CMake configuration:

1. Open Command Palette (Ctrl+Shift+P)
2. Type: "CMake: Select Variant"
3. Choose: "Debug"
4. Rebuild project

---

## Platform Notes

### Windows
- Uses `AllocConsole()` API
- Uses `_kbhit()` for non-blocking input
- Console colors supported via `SetConsoleTextAttribute()`

### Linux/Mac (Future)
- Would use terminal emulator or ncurses library
- Different non-blocking input method (termios)
- Currently not implemented (Windows-only)

### Why Windows-Only?

`AllocConsole()` is a Windows-specific API. For cross-platform support, you'd need:
- Linux: Fork a terminal emulator or use ncurses
- Mac: Similar to Linux approach

For now, we use `#ifdef _WIN32` to keep code Windows-specific. Future versions can add platform-specific implementations.

---

## Summary

You now have:
- ✅ Separate console window for debug commands
- ✅ Non-blocking input (game keeps running!)
- ✅ CMake build system support
- ✅ Debug/Release automatic toggling
- ✅ Extensible command system

**Key Files Created**:
1. `CS230/Engine/DebugConsole.h` - Console interface
2. `CS230/Engine/DebugConsole.cpp` - Console implementation
3. Updated `CS230/Engine/Engine.cpp` - Initialize console
4. Updated `CMakeLists.txt` - Build configuration

**Next Steps**:
1. Build project in Debug mode
2. Run game - console window appears!
3. Type `help` to see commands
4. Register your own game-specific commands

Happy debugging! 🎮
