# Debug Command Implementation Examples

This document provides complete implementation examples for debug console commands used throughout Dragonic Tactics development.

**Reference**: Command implementation timeline aligns with [implementation-plan.md](../implementation-plan.md) Weeks 1-26.

---

## Table of Contents

1. [Command Registration System](#command-registration-system)
2. [Week 1 Commands - Foundation Testing](#week-1-commands---foundation-testing)
3. [Week 2-3 Commands - Character & Combat](#week-2-3-commands---character--combat)
4. [Week 4-5 Commands - Turn System & Hot Reload](#week-4-5-commands---turn-system--hot-reload)
5. [Week 6-10 Commands - Spell Expansion](#week-6-10-commands---spell-expansion)
6. [Week 11-20 Commands - AI & Advanced Features](#week-11-20-commands---ai--advanced-features)
7. [God Mode Integration](#god-mode-integration)

---

## Command Registration System

### DebugConsole Class Structure

```cpp
// DebugConsole.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>

enum class ConsoleColor {
    White,
    Green,
    Red,
    Yellow,
    Blue
};

class DebugConsole {
public:
    static DebugConsole& Instance() {
        static DebugConsole instance;
        return instance;
    }

    void Update(double dt);
    void Draw(Math::TransformationMatrix camera_matrix);

    void ToggleConsole();
    bool IsOpen() const { return isOpen; }

    // Register a console command
    void RegisterCommand(const std::string& name,
                        std::function<void(std::vector<std::string>)> handler,
                        const std::string& helpText);

    // Execute command from input string
    void ExecuteCommand(const std::string& commandLine);

    // Add colored output to console
    void AddOutput(const std::string& text, ConsoleColor color = ConsoleColor::White);

private:
    DebugConsole();

    bool isOpen = false;
    std::string currentInput;
    std::vector<std::string> commandHistory;
    std::vector<std::pair<std::string, ConsoleColor>> outputBuffer;
    int historyIndex = -1;

    // Map of command name -> handler function
    std::map<std::string, std::function<void(std::vector<std::string>)>> commands;
    // Map of command name -> help text
    std::map<std::string, std::string> helpTexts;

    void DrawConsoleWindow();
    void DrawInputLine();
    void DrawOutputBuffer();
    void ProcessInput();
    void AutoComplete();
    std::vector<std::string> ParseCommandLine(const std::string& line);
};
```

### Command Registration Example

```cpp
// In DebugManager or main initialization
void InitializeDebugCommands() {
    // Register help command
    DebugConsole::Instance().RegisterCommand("help",
        [](std::vector<std::string> args) {
            if (args.empty()) {
                DebugConsole::Instance().AddOutput("=== Available Commands ===", ConsoleColor::Yellow);
                // List all commands...
            } else {
                // Show help for specific command
                std::string cmd = args[0];
                // Look up and display helpText
            }
        },
        "help [command] - Show available commands or help for specific command"
    );

    // Register clear command
    DebugConsole::Instance().RegisterCommand("clear",
        [](std::vector<std::string> args) {
            DebugConsole::Instance().ClearOutput();
        },
        "clear - Clear console output"
    );
}
```

---

## Week 1 Commands - Foundation Testing

**Context**: Week 1 builds 5 foundation systems in parallel (implementation-plan.md:250-260). Debug commands must test each system immediately.

**Systems Built Week 1**:

- Character base class (Dev A)
- GridSystem foundation (Dev B)
- EventBus singleton (Dev C)
- DiceManager singleton (Dev D)
- DebugConsole framework (Dev E)

**Commands Needed**:

1. `help` - List all available commands
2. `clear` - Clear console output
3. `roll` - Test DiceManager
4. `setseed` - Set RNG seed for reproducible testing

---

### 1. help - Show Available Commands

**Purpose**: List all registered commands or show help for specific command

**When to Implement**: Week 1 (alongside DebugConsole framework)

**Implementation**:

```cpp
// Register in Week 1 during DebugConsole initialization
DebugConsole::Instance().RegisterCommand("help",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            // List all commands
            DebugConsole::Instance().AddOutput("=== Available Commands ===", ConsoleColor::Yellow);

            auto& commands = DebugConsole::Instance().GetAllCommands();
            for (const auto& [name, handler] : commands) {
                DebugConsole::Instance().AddOutput("  " + name, ConsoleColor::White);
            }

            DebugConsole::Instance().AddOutput("\nType 'help <command>' for details", ConsoleColor::Green);
        } else {
            // Show help for specific command
            std::string cmdName = args[0];
            std::string helpText = DebugConsole::Instance().GetHelpText(cmdName);

            if (!helpText.empty()) {
                DebugConsole::Instance().AddOutput("=== " + cmdName + " ===", ConsoleColor::Yellow);
                DebugConsole::Instance().AddOutput(helpText, ConsoleColor::White);
            } else {
                DebugConsole::Instance().AddOutput("Unknown command: " + cmdName, ConsoleColor::Red);
            }
        }
    },
    "help [command] - Show available commands or help for specific command"
);
```

**Week 1 Integration Test**:

```
> help
Expected: List all Week 1 commands (help, clear, roll, setseed)

> help roll
Expected: Show "roll <notation> - Test dice rolling (e.g., roll 3d6)"
```

---

### 2. clear - Clear Console Output

**Purpose**: Clear console output buffer

**When to Implement**: Week 1 (basic console feature)

**Implementation**:

```cpp
DebugConsole::Instance().RegisterCommand("clear",
    [](std::vector<std::string> args) {
        DebugConsole::Instance().ClearOutputBuffer();
        DebugConsole::Instance().AddOutput("Console cleared", ConsoleColor::Green);
    },
    "clear - Clear console output"
);
```

---

### 3. roll - Test Dice Rolling

**Purpose**: Test DiceManager's RollDiceFromString() function

**When to Implement**: Week 1 (tests Dev D's DiceManager system)

**Critical For**: Verifying dice system works before combat integration (Week 3)

**Implementation**:

```cpp
// Register in Week 1 after DiceManager implementation
DebugConsole::Instance().RegisterCommand("roll",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: roll <dice notation>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Examples: roll 3d6, roll 2d8+5, roll 1d20-2", ConsoleColor::Yellow);
            return;
        }

        std::string diceNotation = args[0];

        // Call DiceManager (Dev D's system)
        int result = DiceManager::Instance().RollDiceFromString(diceNotation);

        DebugConsole::Instance().AddOutput(
            "Rolled " + diceNotation + " = " + std::to_string(result),
            ConsoleColor::Green
        );

        // Show individual die results (helps verify distribution)
        auto lastRolls = DiceManager::Instance().GetLastRolls();
        if (!lastRolls.empty()) {
            std::string breakdown = "Individual rolls: [";
            for (size_t i = 0; i < lastRolls.size(); ++i) {
                breakdown += std::to_string(lastRolls[i]);
                if (i < lastRolls.size() - 1) breakdown += ", ";
            }
            breakdown += "]";
            DebugConsole::Instance().AddOutput(breakdown, ConsoleColor::Yellow);
        }
    },
    "roll <notation> - Test dice rolling (e.g., roll 3d6, roll 2d8+5)"
);
```

**Week 1 Integration Test** (implementation-plan.md:789-796):

```
> roll 3d6
Expected: "Rolled 3d6 = 12"
Expected: "Individual rolls: [4, 5, 3]"

> roll 2d8+5
Expected: Result between 7-21, shows [die1, die2] + modifier

> roll 1d20
Expected: Result between 1-20
```

---

### 4. setseed - Set RNG Seed

**Purpose**: Set DiceManager's RNG seed for reproducible testing

**When to Implement**: Week 1 (critical for deterministic tests)

**Why Critical**: Needed to reproduce bugs with specific dice rolls

**Implementation**:

```cpp
// Register in Week 1 after DiceManager implementation
DebugConsole::Instance().RegisterCommand("setseed",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: setseed <number>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Use same seed for reproducible RNG", ConsoleColor::Yellow);
            return;
        }

        int seed = std::stoi(args[0]);
        DiceManager::Instance().SetSeed(seed);

        DebugConsole::Instance().AddOutput(
            "Set RNG seed to " + std::to_string(seed) + " (results now reproducible)",
            ConsoleColor::Green
        );
    },
    "setseed <number> - Set dice RNG seed for reproducible testing"
);
```

**Week 1 Integration Test** (implementation-plan.md:792-796):

```
> setseed 42
> roll 3d6
Result: 15 [6, 5, 4]

> setseed 42
> roll 3d6
Expected: Same result (15 [6, 5, 4]) - proves RNG is reproducible
```

---

## Week 2-3 Commands - Character & Combat

**Context**: Week 2-3 adds Dragon, GridSystem, and basic combat (implementation-plan.md:265-298).

**Systems Built Week 2-3**:

- Dragon class (Week 2)
- GridSystem pathfinding (Week 2)
- CombatSystem basics (Week 3)
- SpellSystem foundation (Week 3)
- Fighter enemy class (Week 3)

**Commands Needed**:

1. `spawn` - Create characters on grid (Week 2)
2. `teleport` - Move characters instantly (Week 2)
3. `damage` - Test combat damage (Week 3)
4. `heal` - Test healing (Week 3)
5. `kill` - Remove character (Week 3)
6. `castspell` - Test spell casting (Week 3)
7. `debug grid` - Toggle grid overlay (Week 2)

---

## Week 4-5 Commands - Turn System & Hot Reload

**Context**: Week 4-5 completes foundation for PLAYTEST 1 (implementation-plan.md:302-342).

**Systems Built Week 4-5**:

- TurnManager with initiative (Week 4)
- DataRegistry with hot-reload (Week 4)
- BattleState integration (Week 5)
- God Mode basics (Week 5)

**Commands Needed**:

1. `reload` - Hot reload JSON data (Week 4 - CRITICAL)
2. `endturn` - Force end turn (Week 4)
3. `nextturn` - Skip N turns (Week 4)
4. `showturnorder` - Display initiative queue (Week 4)
5. `god` - Toggle god mode (Week 5)

---

### reload - Hot Reload JSON Data

**Purpose**: Reload data files without restarting the game

**When to Implement**: Week 4 (alongside DataRegistry - Dev D's system)

**Critical For**: Week 4 integration test (implementation-plan.md:945-950) - MUST verify hot reload works

```cpp
// Register in Week 1 after DataRegistry implementation
DebugConsole::Instance().RegisterCommand("reload",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: reload <datatype>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Types: characters, spells, maps, ai, all", ConsoleColor::Yellow);
            return;
        }

        std::string dataType = args[0];

        if (dataType == "all") {
            DataRegistry::Instance().LoadFromFile("Assets/Data/characters.json");
            DataRegistry::Instance().LoadFromFile("Assets/Data/spells.json");
            DataRegistry::Instance().LoadFromFile("Assets/Data/maps.json");
            DebugConsole::Instance().AddOutput("Reloaded all data files", ConsoleColor::Green);
        }
        else if (dataType == "characters") {
            DataRegistry::Instance().LoadFromFile("Assets/Data/characters.json");
            DebugConsole::Instance().AddOutput("Reloaded characters.json", ConsoleColor::Green);
        }
        else if (dataType == "spells") {
            DataRegistry::Instance().LoadFromFile("Assets/Data/spells.json");
            DebugConsole::Instance().AddOutput("Reloaded spells.json", ConsoleColor::Green);
        }
        else {
            DebugConsole::Instance().AddOutput("Unknown data type: " + dataType, ConsoleColor::Red);
        }
    },
    "reload <datatype> - Hot reload JSON data (types: characters, spells, maps, ai, all)"
);
```

### 2. spawn - Create Characters at Grid Position

**Purpose**: Spawn characters for testing without full battle setup

```cpp
// Register in Week 3-4 after CharacterFactory and GridSystem implementation
DebugConsole::Instance().RegisterCommand("spawn",
    [](std::vector<std::string> args) {
        if (args.size() < 3) {
            DebugConsole::Instance().AddOutput("Usage: spawn <type> <x> <y>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Types: dragon, fighter, cleric, wizard, rogue", ConsoleColor::Yellow);
            return;
        }

        std::string typeName = args[0];
        int gridX = std::stoi(args[1]);
        int gridY = std::stoi(args[2]);

        // Convert string to CharacterType enum
        CharacterType type;
        if (typeName == "dragon") type = CharacterType::Dragon;
        else if (typeName == "fighter") type = CharacterType::Fighter;
        else if (typeName == "cleric") type = CharacterType::Cleric;
        else if (typeName == "wizard") type = CharacterType::Wizard;
        else if (typeName == "rogue") type = CharacterType::Rogue;
        else {
            DebugConsole::Instance().AddOutput("Unknown character type: " + typeName, ConsoleColor::Red);
            return;
        }

        // Validate grid position
        if (gridX < 0 || gridX >= 8 || gridY < 0 || gridY >= 8) {
            DebugConsole::Instance().AddOutput("Invalid grid position (must be 0-7)", ConsoleColor::Red);
            return;
        }

        // Create character using factory
        auto character = CharacterFactory::CreateCharacter(type);

        // Set grid position
        character->GetGOComponent<GridPosition>()->SetTilePosition(gridX, gridY);

        // Add to current game state's object manager
        Engine::GetGameStateManager().GetCurrentState()->AddGameObject(character.release());

        DebugConsole::Instance().AddOutput(
            "Spawned " + typeName + " at (" + std::to_string(gridX) + ", " + std::to_string(gridY) + ")",
            ConsoleColor::Green
        );
    },
    "spawn <type> <x> <y> - Spawn character at grid position"
);
```

### 3. kill - Remove Character from Battle

**Purpose**: Instantly kill a character for testing death/victory conditions

```cpp
// Register in Week 3-4 after Character implementation
DebugConsole::Instance().RegisterCommand("kill",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: kill <characterName>", ConsoleColor::Red);
            return;
        }

        std::string targetName = args[0];

        // Get current battle state
        auto currentState = Engine::GetGameStateManager().GetCurrentState();
        auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

        // Find character by name
        Character* target = nullptr;
        for (auto* obj : gameObjects) {
            Character* character = dynamic_cast<Character*>(obj);
            if (character && character->TypeName() == targetName) {
                target = character;
                break;
            }
        }

        if (target == nullptr) {
            DebugConsole::Instance().AddOutput("Character not found: " + targetName, ConsoleColor::Red);
            return;
        }

        // Mark for destruction
        target->Destroy();

        DebugConsole::Instance().AddOutput("Killed " + targetName, ConsoleColor::Green);
    },
    "kill <characterName> - Instantly kill a character"
);
```

### 4. teleport - Move Character to Grid Position

**Purpose**: Move characters instantly for testing positioning

```cpp
// Register in Week 4-5 after GridSystem implementation
DebugConsole::Instance().RegisterCommand("teleport",
    [](std::vector<std::string> args) {
        if (args.size() < 3) {
            DebugConsole::Instance().AddOutput("Usage: teleport <characterName> <x> <y>", ConsoleColor::Red);
            return;
        }

        std::string targetName = args[0];
        int gridX = std::stoi(args[1]);
        int gridY = std::stoi(args[2]);

        // Validate grid position
        if (gridX < 0 || gridX >= 8 || gridY < 0 || gridY >= 8) {
            DebugConsole::Instance().AddOutput("Invalid grid position (must be 0-7)", ConsoleColor::Red);
            return;
        }

        // Find character
        auto currentState = Engine::GetGameStateManager().GetCurrentState();
        auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

        Character* target = nullptr;
        for (auto* obj : gameObjects) {
            Character* character = dynamic_cast<Character*>(obj);
            if (character && character->TypeName() == targetName) {
                target = character;
                break;
            }
        }

        if (target == nullptr) {
            DebugConsole::Instance().AddOutput("Character not found: " + targetName, ConsoleColor::Red);
            return;
        }

        // Teleport character
        target->GetGOComponent<GridPosition>()->SetTilePosition(gridX, gridY);

        DebugConsole::Instance().AddOutput(
            "Teleported " + targetName + " to (" + std::to_string(gridX) + ", " + std::to_string(gridY) + ")",
            ConsoleColor::Green
        );
    },
    "teleport <characterName> <x> <y> - Teleport character to grid position"
);
```

### 5. god - Toggle God Mode

**Purpose**: Enable/disable god mode for testing

```cpp
// Register in Week 6 after GodModeManager basic implementation
DebugConsole::Instance().RegisterCommand("god",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            // Toggle
            bool current = GodModeManager::Instance().IsGodModeEnabled();
            GodModeManager::Instance().SetGodMode(!current);
            DebugConsole::Instance().AddOutput(
                "God Mode: " + std::string(!current ? "ON" : "OFF"),
                !current ? ConsoleColor::Green : ConsoleColor::Yellow
            );
        } else {
            // Set explicitly
            std::string mode = args[0];
            if (mode == "on") {
                GodModeManager::Instance().SetGodMode(true);
                DebugConsole::Instance().AddOutput("God Mode: ON", ConsoleColor::Green);
            } else if (mode == "off") {
                GodModeManager::Instance().SetGodMode(false);
                DebugConsole::Instance().AddOutput("God Mode: OFF", ConsoleColor::Yellow);
            } else {
                DebugConsole::Instance().AddOutput("Usage: god [on/off]", ConsoleColor::Red);
            }
        }
    },
    "god [on/off] - Toggle or set god mode"
);
```

### 6. debug - Toggle Debug Visualizations

**Purpose**: Toggle visual debug features

```cpp
// Register in Week 4-5 after GridDebugRenderer implementation
DebugConsole::Instance().RegisterCommand("debug",
    [](std::vector<std::string> args) {
        if (args.size() < 2) {
            DebugConsole::Instance().AddOutput("Usage: debug <feature> <on/off>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Features: grid, collision, aipath, status", ConsoleColor::Yellow);
            return;
        }

        std::string feature = args[0];
        std::string state = args[1];
        bool enable = (state == "on");

        if (feature == "grid") {
            GridDebugRenderer::Instance().SetShowGrid(enable);
            DebugConsole::Instance().AddOutput(
                "Grid overlay: " + std::string(enable ? "ON" : "OFF"),
                ConsoleColor::Green
            );
        }
        else if (feature == "collision") {
            CollisionDebugRenderer::Instance().SetShowCharacterCollision(enable);
            DebugConsole::Instance().AddOutput(
                "Collision boxes: " + std::string(enable ? "ON" : "OFF"),
                ConsoleColor::Green
            );
        }
        else {
            DebugConsole::Instance().AddOutput("Unknown feature: " + feature, ConsoleColor::Red);
        }
    },
    "debug <feature> <on/off> - Toggle debug visualization (features: grid, collision, aipath, status)"
);
```

---

### 7. roll - Test Dice Rolling

**Purpose**: Test dice notation and RNG behavior

```cpp
// Register in Week 5 after DiceManager implementation
DebugConsole::Instance().RegisterCommand("roll",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: roll <dice notation>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Examples: roll 3d6, roll 2d8+5, roll 1d20-2", ConsoleColor::Yellow);
            return;
        }

        std::string diceNotation = args[0];
        int result = DiceManager::Instance().RollDiceFromString(diceNotation);

        DebugConsole::Instance().AddOutput(
            "Rolled " + diceNotation + " = " + std::to_string(result),
            ConsoleColor::Green
        );

        // Show individual die results
        auto lastRolls = DiceManager::Instance().GetLastRolls();
        if (!lastRolls.empty()) {
            std::string breakdown = "Individual rolls: ";
            for (int roll : lastRolls) {
                breakdown += std::to_string(roll) + " ";
            }
            DebugConsole::Instance().AddOutput(breakdown, ConsoleColor::Yellow);
        }
    },
    "roll <notation> - Test dice rolling (e.g., roll 3d6, roll 2d8+5)"
);
```

---

### 8. setseed - Set RNG Seed

**Purpose**: Enable reproducible dice rolls for testing

```cpp
// Register in Week 5 after DiceManager implementation
DebugConsole::Instance().RegisterCommand("setseed",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: setseed <number>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Use same seed for reproducible RNG", ConsoleColor::Yellow);
            return;
        }

        int seed = std::stoi(args[0]);
        DiceManager::Instance().SetSeed(seed);

        DebugConsole::Instance().AddOutput(
            "Set RNG seed to " + std::to_string(seed) + " (results now reproducible)",
            ConsoleColor::Green
        );
    },
    "setseed <number> - Set dice RNG seed for reproducible testing"
);
```

---

### 9. castspell - Test Spell Casting

**Purpose**: Cast spells for testing without full gameplay

```cpp
// Register in Week 6-7 after SpellSystem implementation
DebugConsole::Instance().RegisterCommand("castspell",
    [](std::vector<std::string> args) {
        // Usage: castspell dragon fireball 1 6 4
        if (args.size() < 5) {
            DebugConsole::Instance().AddOutput(
                "Usage: castspell <caster> <spell> <level> <targetX> <targetY>",
                ConsoleColor::Red
            );
            DebugConsole::Instance().AddOutput(
                "Example: castspell dragon fireball 1 6 4",
                ConsoleColor::Yellow
            );
            return;
        }

        std::string casterName = args[0];
        std::string spellName = args[1];
        int level = std::stoi(args[2]);
        int targetX = std::stoi(args[3]);
        int targetY = std::stoi(args[4]);

        // Find character
        Character* caster = FindCharacterByName(casterName);
        if (!caster) {
            DebugConsole::Instance().AddOutput(
                "Character not found: " + casterName,
                ConsoleColor::Red
            );
            return;
        }

        // Cast spell through SpellSystem
        SpellSystem* spellSys = GetGSComponent<SpellSystem>();
        if (spellSys) {
            bool success = spellSys->CastSpellDebug(caster, spellName, level, {targetX, targetY});
            if (success) {
                DebugConsole::Instance().AddOutput(
                    casterName + " cast " + spellName + " (Level " + std::to_string(level) + ")",
                    ConsoleColor::Green
                );
            } else {
                DebugConsole::Instance().AddOutput(
                    "Failed to cast spell (check spell slots and range)",
                    ConsoleColor::Red
                );
            }
        }
    },
    "castspell <caster> <spell> <level> <x> <y> - Cast a spell for testing"
);
```

---

### 10. spellslots - Inspect/Modify Spell Slots

**Purpose**: View and modify spell slot counts

```cpp
// Register in Week 6-7 after SpellSlots component implementation
DebugConsole::Instance().RegisterCommand("spellslots",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: spellslots <character> [add/set level count]", ConsoleColor::Red);
            return;
        }

        std::string charName = args[0];
        Character* character = FindCharacterByName(charName);
        if (!character) {
            DebugConsole::Instance().AddOutput("Character not found: " + charName, ConsoleColor::Red);
            return;
        }

        SpellSlots* slots = character->GetGOComponent<SpellSlots>();
        if (!slots) {
            DebugConsole::Instance().AddOutput(charName + " has no spell slots", ConsoleColor::Red);
            return;
        }

        // If just viewing
        if (args.size() == 1) {
            auto currentSlots = slots->GetCurrentSlots();
            auto maxSlots = slots->GetMaxSlots();

            DebugConsole::Instance().AddOutput("=== " + charName + " Spell Slots ===", ConsoleColor::Yellow);
            for (auto& [level, count] : maxSlots) {
                int current = currentSlots[level];
                DebugConsole::Instance().AddOutput(
                    "Level " + std::to_string(level) + ": " + std::to_string(current) + "/" + std::to_string(count),
                    ConsoleColor::White
                );
            }
            return;
        }

        // Modify slots: spellslots dragon add 1 2
        if (args.size() >= 4) {
            std::string action = args[1];  // "add" or "set"
            int level = std::stoi(args[2]);
            int count = std::stoi(args[3]);

            if (action == "add") {
                for (int i = 0; i < count; ++i) {
                    slots->RestoreSlot(level);
                }
                DebugConsole::Instance().AddOutput(
                    "Added " + std::to_string(count) + " level " + std::to_string(level) + " slots",
                    ConsoleColor::Green
                );
            } else if (action == "set") {
                slots->SetSlots(level, count);
                DebugConsole::Instance().AddOutput(
                    "Set level " + std::to_string(level) + " slots to " + std::to_string(count),
                    ConsoleColor::Green
                );
            }
        }
    },
    "spellslots <character> [add/set level count] - View or modify spell slots"
);
```

---

### 11. endturn - Force End Current Turn

**Purpose**: Skip to next character's turn for faster testing

```cpp
// Register in Week 7-8 after TurnManager implementation
DebugConsole::Instance().RegisterCommand("endturn",
    [](std::vector<std::string> args) {
        TurnManager* turnMgr = GetGSComponent<TurnManager>();
        if (!turnMgr) {
            DebugConsole::Instance().AddOutput("TurnManager not found", ConsoleColor::Red);
            return;
        }

        Character* current = turnMgr->GetActiveCharacter();
        if (!current) {
            DebugConsole::Instance().AddOutput("No active character", ConsoleColor::Red);
            return;
        }

        std::string currentName = current->TypeName();
        turnMgr->EndCurrentTurn();

        Character* next = turnMgr->GetActiveCharacter();
        DebugConsole::Instance().AddOutput(
            "Ended " + currentName + "'s turn. Now: " + next->TypeName(),
            ConsoleColor::Green
        );
    },
    "endturn - Force end the current character's turn"
);
```

---

### 12. showturnorder - Display Turn Order

**Purpose**: View initiative queue and active character

```cpp
// Register in Week 7-8 after TurnManager implementation
DebugConsole::Instance().RegisterCommand("showturnorder",
    [](std::vector<std::string> args) {
        TurnManager* turnMgr = GetGSComponent<TurnManager>();
        if (!turnMgr) {
            DebugConsole::Instance().AddOutput("TurnManager not found", ConsoleColor::Red);
            return;
        }

        auto turnOrder = turnMgr->GetTurnOrder();
        int currentIndex = turnMgr->GetCurrentTurnIndex();

        DebugConsole::Instance().AddOutput(
            "=== Turn Order (Turn " + std::to_string(turnMgr->GetCurrentTurnNumber()) + ") ===",
            ConsoleColor::Yellow
        );

        for (int i = 0; i < turnOrder.size(); ++i) {
            Character* character = turnOrder[i];
            std::string marker = (i == currentIndex) ? " <-- CURRENT" : "";
            std::string line = std::to_string(i + 1) + ". " + character->TypeName() +
                              " (HP: " + std::to_string(character->GetCurrentHP()) + "/" +
                              std::to_string(character->GetMaxHP()) + ")" + marker;

            ConsoleColor color = (i == currentIndex) ? ConsoleColor::Green : ConsoleColor::White;
            DebugConsole::Instance().AddOutput(line, color);
        }
    },
    "showturnorder - Display current turn order and active character"
);
```

---

## Phase 2 Commands (Weeks 7-12)

### 7. damage - Deal Damage to Character

**Purpose**: Test damage system without combat

```cpp
// Register in Week 8-9 after CombatSystem implementation
DebugConsole::Instance().RegisterCommand("damage",
    [](std::vector<std::string> args) {
        if (args.size() < 2) {
            DebugConsole::Instance().AddOutput("Usage: damage <characterName> <amount>", ConsoleColor::Red);
            return;
        }

        std::string targetName = args[0];
        int damageAmount = std::stoi(args[1]);

        // Find character
        auto currentState = Engine::GetGameStateManager().GetCurrentState();
        auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

        Character* target = nullptr;
        for (auto* obj : gameObjects) {
            Character* character = dynamic_cast<Character*>(obj);
            if (character && character->TypeName() == targetName) {
                target = character;
                break;
            }
        }

        if (target == nullptr) {
            DebugConsole::Instance().AddOutput("Character not found: " + targetName, ConsoleColor::Red);
            return;
        }

        // Apply damage
        int hpBefore = target->GetCurrentHP();
        target->TakeDamage(damageAmount);
        int hpAfter = target->GetCurrentHP();

        DebugConsole::Instance().AddOutput(
            "Dealt " + std::to_string(damageAmount) + " damage to " + targetName +
            " (" + std::to_string(hpBefore) + " -> " + std::to_string(hpAfter) + " HP)",
            ConsoleColor::Green
        );
    },
    "damage <characterName> <amount> - Deal damage to character"
);
```

### 8. heal - Heal Character

**Purpose**: Test healing system

```cpp
// Register in Week 8-9 after CombatSystem implementation
DebugConsole::Instance().RegisterCommand("heal",
    [](std::vector<std::string> args) {
        if (args.size() < 2) {
            DebugConsole::Instance().AddOutput("Usage: heal <characterName> <amount/full>", ConsoleColor::Red);
            return;
        }

        std::string targetName = args[0];
        std::string healArg = args[1];

        // Find character
        auto currentState = Engine::GetGameStateManager().GetCurrentState();
        auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

        Character* target = nullptr;
        for (auto* obj : gameObjects) {
            Character* character = dynamic_cast<Character*>(obj);
            if (character && character->TypeName() == targetName) {
                target = character;
                break;
            }
        }

        if (target == nullptr) {
            DebugConsole::Instance().AddOutput("Character not found: " + targetName, ConsoleColor::Red);
            return;
        }

        int hpBefore = target->GetCurrentHP();
        int healAmount;

        if (healArg == "full") {
            healAmount = target->GetMaxHP() - target->GetCurrentHP();
            target->SetHP(target->GetMaxHP());
        } else {
            healAmount = std::stoi(healArg);
            target->SetHP(std::min(target->GetCurrentHP() + healAmount, target->GetMaxHP()));
        }

        int hpAfter = target->GetCurrentHP();

        DebugConsole::Instance().AddOutput(
            "Healed " + targetName + " for " + std::to_string(healAmount) +
            " HP (" + std::to_string(hpBefore) + " -> " + std::to_string(hpAfter) + ")",
            ConsoleColor::Green
        );
    },
    "heal <characterName> <amount/full> - Heal character"
);
```

### 9. nextturn - Skip Turns for Testing

**Purpose**: Fast-forward through turns

```cpp
// Register in Week 7-8 after TurnManager implementation
DebugConsole::Instance().RegisterCommand("nextturn",
    [](std::vector<std::string> args) {
        int count = 1;
        if (!args.empty()) {
            count = std::stoi(args[0]);
        }

        auto battleState = dynamic_cast<BattleState*>(Engine::GetGameStateManager().GetCurrentState());
        if (battleState == nullptr) {
            DebugConsole::Instance().AddOutput("Not in battle", ConsoleColor::Red);
            return;
        }

        auto turnManager = battleState->GetGSComponent<TurnManager>();
        if (turnManager == nullptr) {
            DebugConsole::Instance().AddOutput("TurnManager not found", ConsoleColor::Red);
            return;
        }

        for (int i = 0; i < count; i++) {
            turnManager->AdvanceToNextTurn();
        }

        DebugConsole::Instance().AddOutput(
            "Advanced " + std::to_string(count) + " turn(s)",
            ConsoleColor::Green
        );
    },
    "nextturn [count] - Skip forward N turns (default: 1)"
);
```

### 10. giveap - Give Action Points

**Purpose**: Test action point system

```cpp
// Register in Week 7-8 after ActionPoints implementation
DebugConsole::Instance().RegisterCommand("giveap",
    [](std::vector<std::string> args) {
        if (args.size() < 2) {
            DebugConsole::Instance().AddOutput("Usage: giveap <characterName> <amount>", ConsoleColor::Red);
            return;
        }

        std::string targetName = args[0];
        int amount = std::stoi(args[1]);

        // Find character
        auto currentState = Engine::GetGameStateManager().GetCurrentState();
        auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

        Character* target = nullptr;
        for (auto* obj : gameObjects) {
            Character* character = dynamic_cast<Character*>(obj);
            if (character && character->TypeName() == targetName) {
                target = character;
                break;
            }
        }

        if (target == nullptr) {
            DebugConsole::Instance().AddOutput("Character not found: " + targetName, ConsoleColor::Red);
            return;
        }

        auto actionPoints = target->GetGOComponent<ActionPoints>();
        if (actionPoints == nullptr) {
            DebugConsole::Instance().AddOutput("Character has no ActionPoints component", ConsoleColor::Red);
            return;
        }

        int apBefore = actionPoints->GetCurrentPoints();
        actionPoints->AddActionPoints(amount);
        int apAfter = actionPoints->GetCurrentPoints();

        DebugConsole::Instance().AddOutput(
            "Gave " + std::to_string(amount) + " AP to " + targetName +
            " (" + std::to_string(apBefore) + " -> " + std::to_string(apAfter) + ")",
            ConsoleColor::Green
        );
    },
    "giveap <characterName> <amount> - Give action points to character"
);
```

---

## Phase 3 Commands (Weeks 13-20)

### 11. disableai - Disable AI Character

**Purpose**: Stop AI from taking actions for testing

```cpp
// Register in Week 13-15 after complete AI implementation
DebugConsole::Instance().RegisterCommand("disableai",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: disableai <characterName/all>", ConsoleColor::Red);
            return;
        }

        std::string targetName = args[0];

        if (targetName == "all") {
            AISystem::Instance().DisableAllAI();
            DebugConsole::Instance().AddOutput("Disabled all AI", ConsoleColor::Green);
            return;
        }

        // Find character
        auto currentState = Engine::GetGameStateManager().GetCurrentState();
        auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

        Character* target = nullptr;
        for (auto* obj : gameObjects) {
            Character* character = dynamic_cast<Character*>(obj);
            if (character && character->TypeName() == targetName) {
                target = character;
                break;
            }
        }

        if (target == nullptr) {
            DebugConsole::Instance().AddOutput("Character not found: " + targetName, ConsoleColor::Red);
            return;
        }

        AISystem::Instance().DisableAI(target);
        DebugConsole::Instance().AddOutput("Disabled AI for " + targetName, ConsoleColor::Green);
    },
    "disableai <characterName/all> - Disable AI for character or all AI"
);
```

### 12. clearstatus - Remove Status Effects

**Purpose**: Clear all status effects from character

```cpp
// Register in Week 16-17 after StatusEffectManager implementation
DebugConsole::Instance().RegisterCommand("clearstatus",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: clearstatus <characterName>", ConsoleColor::Red);
            return;
        }

        std::string targetName = args[0];

        // Find character
        auto currentState = Engine::GetGameStateManager().GetCurrentState();
        auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

        Character* target = nullptr;
        for (auto* obj : gameObjects) {
            Character* character = dynamic_cast<Character*>(obj);
            if (character && character->TypeName() == targetName) {
                target = character;
                break;
            }
        }

        if (target == nullptr) {
            DebugConsole::Instance().AddOutput("Character not found: " + targetName, ConsoleColor::Red);
            return;
        }

        auto statusEffects = target->GetGOComponent<StatusEffects>();
        if (statusEffects == nullptr) {
            DebugConsole::Instance().AddOutput("Character has no StatusEffects component", ConsoleColor::Red);
            return;
        }

        statusEffects->ClearAllEffects();
        DebugConsole::Instance().AddOutput("Cleared all status effects from " + targetName, ConsoleColor::Green);
    },
    "clearstatus <characterName> - Remove all status effects from character"
);
```

---

## God Mode Integration

### GodModeManager Implementation

```cpp
// GodModeManager.h
#pragma once
#include <set>

class Character;

class GodModeManager {
public:
    static GodModeManager& Instance() {
        static GodModeManager instance;
        return instance;
    }

    // Master god mode toggle
    void SetGodMode(bool enabled) {
        godModeEnabled = enabled;
        if (enabled) {
            // Enable all god mode features
            invincibility = true;
            infiniteActionPoints = true;
            infiniteSpellSlots = true;
        }
    }

    bool IsGodModeEnabled() const { return godModeEnabled; }

    // Individual feature toggles
    void SetInvincibility(bool enabled) { invincibility = enabled; }
    void SetInfiniteActionPoints(bool enabled) { infiniteActionPoints = enabled; }
    void SetInfiniteSpellSlots(bool enabled) { infiniteSpellSlots = enabled; }

    // Query methods for systems to check
    bool ShouldBlockDamage(Character* character) const {
        return invincibility || invincibleCharacters.count(character) > 0;
    }

    bool ShouldBlockResourceConsumption(Character* character) const {
        return (infiniteActionPoints || infiniteSpellSlots) ||
               infiniteResourceCharacters.count(character) > 0;
    }

private:
    GodModeManager() = default;

    bool godModeEnabled = false;
    bool invincibility = false;
    bool infiniteActionPoints = false;
    bool infiniteSpellSlots = false;

    std::set<Character*> invincibleCharacters;
    std::set<Character*> infiniteResourceCharacters;
};
```

### Integration Example in CombatSystem

```cpp
// In CombatSystem::ApplyDamage()
void CombatSystem::ApplyDamage(Character* target, int damage) {
    // Check god mode before applying damage
    if (GodModeManager::Instance().ShouldBlockDamage(target)) {
        Engine::GetLogger().LogDebug("God Mode: Damage blocked for " + target->TypeName());

        // Show visual feedback that god mode is active
        DebugUIOverlay::Instance().ShowNotification("GOD MODE: Damage Blocked");

        return;  // Don't apply damage
    }

    // Normal damage calculation
    target->TakeDamage(damage);

    // Log and publish damage event
    Engine::GetLogger().LogEvent(target->TypeName() + " took " + std::to_string(damage) + " damage");
    EventBus::Instance().Publish(DamageEvent{nullptr, target, damage});
}
```

---

## Summary

This document provides complete, copy-paste-ready implementations for all debug console commands. Each command includes:

- ✅ Input validation and error handling
- ✅ Colored console output for feedback
- ✅ Integration with game systems
- ✅ Clear help text
- ✅ Practical examples

**Usage Pattern**:

1. Register commands during debug system initialization
2. Commands automatically available in console
3. User types command, sees colored feedback
4. Systems integrate god mode checks naturally

These debug commands will dramatically accelerate development and testing throughout the 26-week project!
