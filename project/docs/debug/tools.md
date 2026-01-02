# Debug Tools & Development Utilities Specification

**Project**: Dragonic Tactics
**Purpose**: Comprehensive debug, testing, and development tools to accelerate development and improve game quality
**Philosophy**: Debug tools should be built alongside gameplay systems, not after

---

## Table of Contents

1. [Debug System Architecture](#debug-system-architecture)
2. [Visual Debug Tools](#visual-debug-tools)
3. [Console Command System](#console-command-system)
4. [God Mode & Cheat System](#god-mode--cheat-system)
5. [Hot Reloading System](#hot-reloading-system)
6. [Debug UI Overlay](#debug-ui-overlay)
7. [Integration Timeline](#integration-timeline)

---

## Debug System Architecture

### Core Debug Manager (Singleton)

```cpp
class DebugManager {
public:
    static DebugManager& Instance();

    // Debug mode toggles
    void SetDebugMode(bool enabled);
    bool IsDebugMode() const;

    // Visual debug toggles
    void ToggleGridOverlay();
    void ToggleCollisionBoxes();
    void ToggleAIPathVisualization();
    void ToggleStatusInfo();

    // God mode features
    void ToggleGodMode();
    bool IsGodModeEnabled() const;
    void SetInvincibility(Character* character, bool enabled);
    void SetInfiniteActionPoints(Character* character, bool enabled);
    void SetInfiniteSpellSlots(Character* character, bool enabled);

    // Console commands
    void ExecuteCommand(const std::string& command);
    void RegisterCommand(const std::string& name, std::function<void(std::vector<std::string>)> handler);

    // Hot reload
    void ReloadAllData();
    void ReloadCharacterData();
    void ReloadSpellData();
    void WatchFileChanges();  // Monitor JSON files for changes
};
```

---

## Visual Debug Tools

### 1. Grid Overlay System

**Purpose**: Visualize 8x8 battlefield grid with tile coordinates and occupation status

**Features**:

- Toggle grid lines on/off (default: ON in debug builds)
- Display tile coordinates (0,0 to 7,7)
- Highlight occupied tiles (red) vs empty tiles (green)
- Show movement range for selected character (blue overlay)
- Show attack/spell range for selected character (orange overlay)

**Implementation**:

```cpp
class GridDebugRenderer : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix);

    void SetShowGrid(bool show);
    void SetShowCoordinates(bool show);
    void SetShowOccupancy(bool show);
    void SetHighlightMovementRange(Character* character);
    void SetHighlightAttackRange(Character* character);

private:
    bool showGrid = true;
    bool showCoordinates = true;
    bool showOccupancy = true;
    Character* highlightedCharacter = nullptr;

    void DrawGridLines();
    void DrawTileCoordinates();
    void DrawOccupancyOverlay();
    void DrawMovementRangeOverlay();
    void DrawAttackRangeOverlay();
};
```

**Keyboard Shortcuts**:

- `F1`: Toggle grid overlay
- `F2`: Toggle coordinate display
- `F3`: Toggle occupancy highlighting

---

### 2. AI Path Visualization

**Purpose**: Show AI decision-making process and pathfinding calculations

**Features**:

- Display current AI character's target (red X marker)
- Draw pathfinding route (yellow dotted line)
- Show considered positions (light blue dots)
- Display threat assessment values (text above enemies)
- Visualize attack range circles for AI characters
- Show AI state (text: "Attacking", "Healing", "Retreating", etc.)

**Implementation**:

```cpp
class AIDebugVisualizer : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix);

    void SetShowPaths(bool show);
    void SetShowTargets(bool show);
    void SetShowThreatValues(bool show);
    void SetShowAIState(bool show);

    // Called by AISystem to log decisions
    void LogAIDecision(Character* aiCharacter, AIDecision decision);
    void SetCurrentPath(Character* aiCharacter, std::vector<Math::vec2> path);
    void SetCurrentTarget(Character* aiCharacter, Character* target);

private:
    bool showPaths = true;
    bool showTargets = true;
    bool showThreatValues = true;
    bool showAIState = true;

    struct AIDebugInfo {
        std::vector<Math::vec2> currentPath;
        Character* currentTarget;
        std::string currentState;
        int threatLevel;
    };
    std::map<Character*, AIDebugInfo> aiDebugData;
};
```

**Keyboard Shortcuts**:

- `F4`: Toggle AI path visualization
- `F5`: Toggle threat value display
- `F6`: Step AI decision frame-by-frame (pause mode)

---

### 3. Status Info Overlay

**Purpose**: Display real-time character stats and game state information

**Features**:

- Character HP bars above each character (green = full, yellow = hurt, red = critical)
- Action points indicator (blue bar below HP)
- Spell slots remaining (small icons for each level)
- Active status effects (icon badges: burn, fear, blessing, etc.)
- Turn order display (side panel showing initiative queue)
- Current turn timer
- Frame rate and performance metrics

**Implementation**:

```cpp
class StatusInfoOverlay : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix);

    void SetShowHPBars(bool show);
    void SetShowActionPoints(bool show);
    void SetShowSpellSlots(bool show);
    void SetShowStatusEffects(bool show);
    void SetShowTurnOrder(bool show);
    void SetShowPerformanceMetrics(bool show);

private:
    bool showHPBars = true;
    bool showActionPoints = true;
    bool showSpellSlots = true;
    bool showStatusEffects = true;
    bool showTurnOrder = true;
    bool showPerformanceMetrics = false;

    void DrawHPBar(Character* character);
    void DrawActionPointsBar(Character* character);
    void DrawSpellSlotsIndicator(Character* character);
    void DrawStatusEffectBadges(Character* character);
    void DrawTurnOrderPanel();
    void DrawPerformancePanel();
};
```

**Keyboard Shortcuts**:

- `F7`: Toggle HP/AP bars
- `F8`: Toggle status effects display
- `F9`: Toggle performance metrics

---

### 4. Collision Box Visualization

**Purpose**: Display collision boundaries for debugging hit detection

**Features**:

- Draw rectangle collision boxes (green = no collision, red = colliding)
- Show attack hitboxes during attacks (pulsing red)
- Display spell area-of-effect circles
- Visualize line-of-sight rays
- Show terrain collision boundaries

**Implementation**:

```cpp
class CollisionDebugRenderer : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix);

    void SetShowCharacterCollision(bool show);
    void SetShowAttackHitboxes(bool show);
    void SetShowSpellAOE(bool show);
    void SetShowLineOfSight(bool show);

private:
    bool showCharacterCollision = true;
    bool showAttackHitboxes = true;
    bool showSpellAOE = true;
    bool showLineOfSight = true;
};
```

**Keyboard Shortcuts**:

- `F10`: Toggle collision boxes
- `F11`: Toggle attack/spell range visualization

---

### 5. Dice Roll History Panel

**Purpose**: Display recent dice rolls for combat debugging and RNG verification

**Features**:

- Show last 10 dice rolls with notation, result, and individual die values
- Color-coded results (green = high roll, red = low roll)
- Roll statistics (min, max, average)

**Implementation**:

```cpp
class DiceHistoryPanel : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix) override;
    void LogRoll(const std::string& notation, int result, std::vector<int> individualRolls);

private:
    struct RollHistory {
        std::string notation;  // "3d6"
        int result;            // 12
        std::vector<int> individualRolls;  // [4, 6, 2]
        double timestamp;
    };
    std::deque<RollHistory> recentRolls;
};
```

**Keyboard Shortcuts**: `F9` - Toggle dice history panel

---

### 6. Combat Formula Inspector

**Purpose**: Break down damage calculations step-by-step

**Features**:

- Show complete damage formula for selected attacker/target
- Display all modifiers (attack bonus, defense, status effects)
- Show min/max damage range

**Implementation**:

```cpp
class CombatFormulaInspector : public CS230::Component {
public:
    void ShowDamageCalculation(Character* attacker, Character* target);

private:
    void DrawFormulaBreakdown();
    void DrawModifierList();
};
```

**Keyboard Shortcuts**: `F12` - Toggle combat formula inspector

---

### 7. Event Bus Tracer

**Purpose**: Monitor EventBus activity for debugging event-driven systems

**Features**:

- Real-time event stream (last 20 events)
- Filter events by type
- Show subscriber counts

**Implementation**:

```cpp
class EventBusTracer : public CS230::Component {
public:
    void LogEvent(const std::string& eventType, const std::string& payload, int subscriberCount);
    void SetFilter(const std::string& eventType);

private:
    struct EventLogEntry {
        std::string eventType;
        std::string payload;
        int subscriberCount;
        double timestamp;
    };
    std::deque<EventLogEntry> eventLog;
};
```

**Keyboard Shortcuts**: `Ctrl+E` - Toggle event tracer

---

## Console Command System

### Console Implementation

**Approach**: Separate Console Window (Option B)

We use a **separate Windows console window** that runs alongside the game window for debug commands. This provides a real, native console experience without needing to implement in-game text rendering.

**Architecture**:

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

**Key Features**:

- Two separate windows (game graphics + debug console)
- Real Windows console using `AllocConsole()` API
- Non-blocking input using `_kbhit()` (game keeps running while you type)
- Only appears in Debug builds (automatic via CMake configuration)
- CMake build system support (Visual Studio + VSCode compatible)
- Color-coded output (errors in red, success in green, info in white)
- Command history and help system

**Implementation**: See [SEPARATE_CONSOLE_WINDOW_GUIDE.md](./SEPARATE_CONSOLE_WINDOW_GUIDE.md) for complete implementation details including:

- Complete source code for `DebugConsole` class
- CMake configuration for Debug/Release builds
- Non-blocking input implementation
- Platform-specific handling (`#ifdef _WIN32`)
- Integration with CS230 Engine
- Troubleshooting guide

**Core Commands**:

#### Character Manipulation

```
spawn <characterType> <x> <y>          # Spawn character at grid position
  Example: spawn dragon 3 7
  Example: spawn fighter 2 0

kill <characterName/ID>                # Instantly kill a character
  Example: kill fighter
  Example: kill 2

teleport <characterName> <x> <y>       # Move character to position
  Example: teleport dragon 5 5

heal <characterName> <amount>          # Heal character by amount (or "full")
  Example: heal dragon 50
  Example: heal dragon full

damage <characterName> <amount>        # Deal damage to character
  Example: damage fighter 30

setstat <characterName> <stat> <value> # Modify character stat
  Example: setstat dragon hp 200
  Example: setstat wizard speed 10
  Stats: hp, maxhp, speed, attack, defense, actionpoints
```

#### Resource Manipulation

```
giveap <characterName> <amount>        # Give action points
  Example: giveap dragon 10

givespell <characterName> <level>      # Give spell slots
  Example: givespell wizard 3

refillslots <characterName>            # Restore all spell slots
  Example: refillslots dragon

clearstatus <characterName>            # Remove all status effects
  Example: clearstatus fighter
```

#### Game State Control

```
endturn                                 # Force end current turn
nextturn <count>                        # Skip forward multiple turns
  Example: nextturn 5

pause                                   # Pause game
resume                                  # Resume game
timescale <value>                       # Set game speed (0.1 to 10.0)
  Example: timescale 0.5               # Half speed
  Example: timescale 2.0               # Double speed

restart                                 # Restart current battle
victory                                 # Force victory condition
defeat                                  # Force defeat condition
```

#### AI Control

```
disableai <characterName>               # Stop AI from taking actions
  Example: disableai fighter

enableai <characterName>                # Re-enable AI
  Example: enableai fighter

disableallai                            # Disable all AI characters
enableallai                             # Re-enable all AI characters

aisetstate <characterName> <state>      # Force AI into specific state
  Example: aisetstate fighter attack
  States: attack, defend, heal, retreat, idle
```

#### Debug Visualization

```
debug <feature> <on/off>                # Toggle debug features
  Example: debug grid on
  Example: debug aipath off
  Features: grid, aipath, collision, status, performance

god <on/off>                            # Toggle god mode
  Example: god on

showall                                 # Enable all debug visualizations
hideall                                 # Disable all debug visualizations
```

#### Data Management

```
reload <datatype>                       # Hot reload JSON data
  Example: reload characters
  Example: reload spells
  Example: reload all
  Types: characters, spells, maps, ai, all

listcommands                            # Show all available commands
help <command>                          # Show detailed help for command
  Example: help spawn

clear                                   # Clear console output
```

#### Testing & Development

```
screenshot <filename>                   # Save screenshot
  Example: screenshot test_battle.png

log <message>                           # Write to game log
  Example: log "Testing dragon spell range"

setvar <varname> <value>                # Set debug variable
  Example: setvar dice_rolls_visible true

startrecording                          # Start recording gameplay for replay
stoprecording                           # Stop recording
playreplay <filename>                   # Play back recorded session
```

**Implementation**:

```cpp
class DebugConsole : public CS230::Component {
public:
    void Update(double dt);
    void Draw(Math::TransformationMatrix camera_matrix);

    void ToggleConsole();
    bool IsOpen() const;

    void ExecuteCommand(const std::string& commandLine);
    void RegisterCommand(const std::string& name,
                        std::function<void(std::vector<std::string>)> handler,
                        const std::string& helpText);

    void AddOutput(const std::string& text, ConsoleColor color = ConsoleColor::White);

private:
    bool isOpen = false;
    std::string currentInput;
    std::vector<std::string> commandHistory;
    std::vector<std::pair<std::string, ConsoleColor>> outputBuffer;
    int historyIndex = -1;

    std::map<std::string, std::function<void(std::vector<std::string>)>> commands;
    std::map<std::string, std::string> helpTexts;

    void DrawConsoleWindow();
    void DrawInputLine();
    void DrawOutputBuffer();
    void ProcessInput();
    void AutoComplete();
};
```

---

## God Mode & Cheat System

### God Mode Features

**Purpose**: Enable rapid testing without dealing with game-over scenarios

**Features**:

```cpp
class GodModeManager : public CS230::Component {
public:
    // Master god mode toggle
    void SetGodMode(bool enabled);
    bool IsGodModeEnabled() const;

    // Individual god mode features (can be toggled separately)
    void SetInvincibility(bool enabled);          // Characters can't die
    void SetInfiniteActionPoints(bool enabled);   // Action points never decrease
    void SetInfiniteSpellSlots(bool enabled);     // Spell slots never consumed
    void SetInstantKill(bool enabled);            // All attacks instantly kill
    void SetInfiniteMovement(bool enabled);       // Move anywhere on grid
    void SetNoFogOfWar(bool enabled);             // See entire battlefield
    void SetAlwaysCritical(bool enabled);         // All attacks are critical hits
    void SetAutoWin(bool enabled);                // Bypass victory conditions

    // Per-character god mode settings
    void SetCharacterInvincible(Character* character, bool enabled);
    void SetCharacterInfiniteResources(Character* character, bool enabled);

    // Called by various systems to check god mode status
    bool ShouldBlockDamage(Character* character) const;
    bool ShouldBlockResourceConsumption(Character* character) const;
    bool ShouldAllowIllegalMove(Character* character) const;

private:
    bool godModeEnabled = false;
    bool invincibility = false;
    bool infiniteActionPoints = false;
    bool infiniteSpellSlots = false;
    bool instantKill = false;
    bool infiniteMovement = false;
    bool noFogOfWar = false;
    bool alwaysCritical = false;
    bool autoWin = false;

    std::set<Character*> invincibleCharacters;
    std::set<Character*> infiniteResourceCharacters;
};
```

**Keyboard Shortcuts**:

- `Ctrl+G`: Toggle god mode (all features)
- `Ctrl+I`: Toggle invincibility only
- `Ctrl+R`: Toggle infinite resources only
- `Ctrl+K`: Toggle instant kill mode

**God Mode Integration Example**:

```cpp
// In CombatSystem::ApplyDamage()
void CombatSystem::ApplyDamage(Character* target, int damage) {
    // Check god mode before applying damage
    if (GodModeManager::Instance().ShouldBlockDamage(target)) {
        Engine::GetLogger().LogDebug("God Mode: Damage blocked for " + target->TypeName());
        return;  // Don't apply damage
    }

    // Normal damage calculation
    target->TakeDamage(damage);
}

// In SpellSystem::CastSpell()
void SpellSystem::CastSpell(Character* caster, SpellType spell) {
    // Check god mode before consuming spell slot
    if (!GodModeManager::Instance().ShouldBlockResourceConsumption(caster)) {
        caster->GetGOComponent<SpellSlots>()->ConsumeSpellSlot(spell.level);
    } else {
        Engine::GetLogger().LogDebug("God Mode: Spell slot consumption skipped");
    }

    // Cast spell normally
    ApplySpellEffect(spell, target);
}
```

---

## Hot Reloading System

### File Watcher & Reload Manager

**Purpose**: Modify JSON data files while game is running and see changes immediately

**Features**:

- Monitor `Assets/Data/` directory for file changes
- Automatically reload modified JSON files
- Validate JSON before applying (don't crash on syntax errors)
- Show notification when data reloaded
- Optional: Require manual confirmation before reload (press R to reload)

**Implementation**:

```cpp
class HotReloadManager : public CS230::Component {
public:
    void Update(double dt);  // Check for file changes each frame

    void StartWatching();
    void StopWatching();

    void SetAutoReload(bool enabled);  // Auto-reload on change vs manual
    void ReloadFile(const std::string& filepath);
    void ReloadAll();

private:
    bool isWatching = true;
    bool autoReload = true;

    struct FileWatchData {
        std::string filepath;
        std::time_t lastModified;
    };
    std::vector<FileWatchData> watchedFiles;

    void CheckFileChanges();
    void OnFileChanged(const std::string& filepath);
    bool ValidateJSON(const std::string& filepath);
    void ShowReloadNotification(const std::string& filename);
};
```

**Supported Hot Reload Data**:

- ✅ `characters.json` - Character stats (HP, speed, attack, defense)
- ✅ `spells.json` - Spell damage, range, costs
- ✅ `ai_behavior.json` - AI decision weights and priorities
- ✅ `balance.json` - Game balance values (dice rolls, modifiers)
- ✅ `maps.json` - Battlefield layouts and terrain
- ❌ Code changes (requires full recompilation)

**Usage Example**:

1. Game is running with Dragon HP at 140
2. Open `Assets/Data/characters.json`
3. Change `"dragon": { "maxHP": 200 }`
4. Save file
5. Game detects change and shows notification: "characters.json reloaded"
6. Dragon's max HP is now 200 (current HP also healed to 200 in debug mode)

---

## Debug UI Overlay

### In-Game Debug Panel

**Purpose**: Display real-time game state information without console

**Layout**:

```
┌─────────────────────────────────────────────────────────────┐
│ DEBUG MODE [F12 to toggle]                    FPS: 30.0    │
├─────────────────────────────────────────────────────────────┤
│ Current Turn: Dragon (Player)                 Turn: 5      │
│ Action Points: 5/5                            God Mode: ON  │
│                                                              │
│ Selected: Dragon                                            │
│   HP: 140/140                                               │
│   Spell Slots: [3,3,2]                                      │
│   Status: None                                              │
│   Grid Pos: (3, 7)                                          │
│                                                              │
│ Enemies Alive: 4/4                                          │
│   Fighter: 90/90 HP @ (2,0)                                 │
│   Cleric: 90/90 HP @ (4,0)                                  │
│   Wizard: 55/55 HP @ (1,0)                                  │
│   Rogue: 65/65 HP @ (5,0)                                   │
│                                                              │
│ Quick Commands:                                             │
│   [G] Toggle God Mode                                       │
│   [~] Open Console                                          │
│   [R] Reload Data                                           │
│   [H] Show/Hide UI                                          │
└─────────────────────────────────────────────────────────────┘
```

**Implementation**:

```cpp
class DebugUIOverlay : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix);

    void SetVisible(bool visible);
    void SetPosition(DebugPanelPosition position);  // TopLeft, TopRight, etc.

private:
    bool visible = true;
    DebugPanelPosition position = DebugPanelPosition::TopLeft;

    void DrawMainPanel();
    void DrawCharacterInfo(Character* character);
    void DrawTurnInfo();
    void DrawQuickCommands();
    void DrawPerformanceMetrics();
};
```

---

## Integration Timeline

### Debug Tools Development Schedule

Debug tools should be implemented **alongside** gameplay features, not after. Each week includes corresponding debug tools that support that week's deliverables.

**Reference**: This timeline aligns with [implementation-plan.md](../implementation-plan.md) Weeks 1-26.

---

### Week 1: Foundation Systems + Core Debug Infrastructure

**Gameplay Systems Built** (implementation-plan.md:250-260):

- Character base class (Dev A)
- GridSystem foundation (Dev B)
- EventBus singleton (Dev C)
- DiceManager singleton (Dev D)
- **DebugConsole framework (Dev E)**

**Debug Tools to Implement**:

- [ ] **DebugManager Singleton** - Master control for all debug features
  
  - `SetDebugMode(bool)`, `ToggleGridOverlay()`, `ExecuteCommand()`
  - Integration: All debug tools register with DebugManager

- [ ] **DebugConsole** - Basic console with command registration
  
  - Tilde (~) key opens/closes console
  - Command registration system
  - Color-coded output (Green = success, Red = error, White = info)
  - **Week 1 Commands**: `help`, `clear`, `listcommands`

- [ ] **EventBusTracer** - Monitor EventBus activity
  
  - Subscribe to ALL events published by EventBus (Dev C's system)
  - Log last 20 events with timestamps
  - **Command**: Ctrl+E to toggle event tracer overlay
  - **Critical**: Needed to verify EventBus pub/sub works in Week 1

- [ ] **DiceManager Test Commands** (Week 1)
  
  - `roll <notation>` - Test DiceManager's RollDiceFromString()
  - `setseed <number>` - Set RNG seed for reproducible tests
  - Example: `roll 3d6` should output result and individual die values

**Why Week 1?** Debug tools enable testing of all 5 foundation systems immediately. Without them, devs can't verify EventBus, DiceManager, or Character systems work.

**Integration Test Support** (implementation-plan.md "End of Week 1"):

```
# Test EventBus (Dev C's system)
> Roll dice to trigger events
> Ctrl+E to open EventBusTracer
> Verify DiceRolledEvent appears with subscriber count

# Test DiceManager (Dev D's system)
> roll 3d6
Expected: "Rolled 3d6 = 12 [4, 5, 3]"
> setseed 42
> roll 3d6
> setseed 42
> roll 3d6
Expected: Same result both times (reproducible RNG)
```

---

### Week 2: Dragon + Grid Integration

**Gameplay Systems Built** (implementation-plan.md:265-279):

- Dragon class (Dev A)
- GridSystem pathfinding with A* (Dev B)
- TurnManager basics (Dev C)
- **GridDebugRenderer (Dev E)**

**Debug Tools to Implement**:

- [ ] **GridDebugRenderer (F1)** - Visualize 8x8 battlefield
  
  - Draw grid lines (light gray)
  - Show tile coordinates (0,0 to 7,7)
  - Highlight occupied vs empty tiles
  - Show movement range for selected character (blue overlay)
  - **Command**: `debug grid on/off`

- [ ] **Character Spawn Command** (Week 2)
  
  - `spawn <type> <x> <y>` - Create characters at grid position
  - Example: `spawn dragon 4 4`
  - Integration: Uses CharacterFactory (Dev D starts this Week 3-4, but spawn command needs it)

- [ ] **Teleport Command** (Week 2)
  
  - `teleport <characterName> <x> <y>` - Move character instantly
  - Example: `teleport dragon 5 5`
  - Critical for testing pathfinding without manual clicking

**Integration Test Support** (implementation-plan.md:274-279):

```
# Test Dragon spawning and movement
> spawn dragon 4 4
Expected: Dragon appears at center of grid (4, 4)

> debug grid on
Expected: F1 grid overlay shows tile coordinates

> teleport dragon 0 0
> teleport dragon 7 7
Expected: Dragon moves to corners, pathfinding calculates route

> endturn
Expected: TurnManager advances to next turn
```

---

### Week 3: Combat + Spells Begin

**Gameplay Systems Built** (implementation-plan.md:283-298):

- Dragon Fireball spell (Dev A)
- CombatSystem basics (Dev B)
- SpellSystem foundation (Dev C)
- Fighter enemy class (Dev D)
- **Combat debug tools (Dev E)**

**Debug Tools to Implement**:

- [ ] **Combat Debug Commands**
  
  - `damage <characterName> <amount>` - Deal damage for testing
  - `heal <characterName> <amount/full>` - Heal character
  - `kill <characterName>` - Instantly kill character
  - Example: `damage fighter 30` → Fighter HP: 90 → 60

- [ ] **Spell Casting Command** (Week 3)
  
  - `castspell <caster> <spell> <level> <targetX> <targetY>`
  - Example: `castspell dragon fireball 1 6 4`
  - **Critical**: Needed to test SpellSystem without UI

- [ ] **CombatFormulaInspector (F12)** - Basic version
  
  - Show dice rolls and damage calculation breakdown
  - Verify DiceManager integration with combat

**Integration Test Support** (implementation-plan.md:292-298):

```
# Test Combat System
> spawn dragon 3 7
> spawn fighter 6 4
> damage fighter 30
Expected: Fighter HP: 90 → 60, damage event published

> heal fighter full
Expected: Fighter HP: 60 → 90

# Test Spell System
> castspell dragon fireball 1 6 4
Expected: Fireball cast at tile (6, 4), damage calculated with dice rolls
Press F12 to see CombatFormulaInspector showing formula breakdown
```

---

### Week 4: More Spells + Turn System

**Gameplay Systems Built** (implementation-plan.md:302-317):

- Dragon CreateWall + LavaPool (Dev A)
- TurnManager initiative system (Dev B)
- Fighter basic AI - manual control (Dev C)
- **DataRegistry basics with hot-reload (Dev D)**
- **Turn debug tools + DiceHistoryPanel (Dev E)**

**Debug Tools to Implement**:

- [ ] **Hot Reload System** (Week 4 - CRITICAL)
  
  - **HotReloadManager** - File watching and JSON reload
  - Monitor `Assets/Data/` directory for changes
  - `reload <datatype>` command (characters, spells, maps, ai, all)
  - Auto-reload when JSON file modified (or manual with `reload` command)
  - **Integration with DataRegistry** (Dev D's system)

- [ ] **Hot Reload Workflow** (NEW - Missing from original doc):
  
  ```
  1. Game running with Dragon HP = 140
  2. Open Assets/Data/characters.json
  3. Change: "dragon": { "maxHP": 200 }
  4. Save file
  5. HotReloadManager detects change (file watcher)
  6. Console shows: "[HotReload] Detected change: characters.json"
  7. Auto-reload enabled: Data reloads immediately
     OR Manual: Type `reload characters` to reload
  8. Console shows: "[HotReload] Reloaded characters.json successfully"
  9. Verify: `spawn dragon 4 4` → Dragon now has 200 HP
  ```

- [ ] **Turn Management Commands** (Week 4)
  
  - `endturn` - Force end current character's turn
  - `nextturn <count>` - Skip forward N turns
  - `showturnorder` - Display initiative queue

- [ ] **DiceHistoryPanel (F9)** - Last 10 dice rolls
  
  - Show dice notation, result, individual die values
  - Color-coded (green = high, red = low, yellow = average)

**Integration Test Support** (implementation-plan.md:312-317):

```
# Test Turn System
> showturnorder
Expected: Display all characters with initiative values

> endturn
Expected: Advance to next character in turn order

> nextturn 5
Expected: Skip 5 turns for faster testing

# Test Hot Reload (CRITICAL for Week 4)
> # Edit characters.json (change Dragon maxHP to 200)
> reload characters
Expected: "Reloaded characters.json successfully" in green
> spawn dragon 4 4
Expected: Dragon has 200 HP (not 140)

# Test Dice History
> roll 3d6
> roll 2d8+5
> Press F9
Expected: DiceHistoryPanel shows last 2 rolls with results
```

---

### Week 5: Polish + PLAYTEST 1 Preparation

**Gameplay Systems Built** (implementation-plan.md:320-342):

- Dragon spell polish (Dev A)
- Combat system complete (Dev B)
- Fighter manual control polish (Dev C)
- Data-driven Dragon stats (Dev D)
- **BattleState integration + Playtest build (Dev E)**

**Debug Tools to Implement**:

- [ ] **God Mode Basic Features** (Week 5)
  
  - `god on/off` - Toggle invincibility, infinite resources
  - Ctrl+G keyboard shortcut
  - **Purpose**: Skip tedious combat for testing late-game scenarios

- [ ] **Full Debug UI Overlay** (Week 5)
  
  - Status panel (top-right) showing current turn, character stats
  - HP bars above characters
  - Console toggle (~ key)
  - All F-key shortcuts working (F1, F9, F12)

**Playtest 1 Debug Checklist** (NEW - Missing from original doc):

For PLAYTEST 1 (Friday Week 5), testers should use these debug commands:

```
# Verify Dragon spawning
> spawn dragon 4 4
Expected: Dragon at center of 8x8 grid

# Verify movement
> debug grid on
> teleport dragon 3 3
> teleport dragon 5 5
Expected: Dragon moves, pathfinding calculates route

# Verify spells
> spawn fighter 6 4
> castspell dragon fireball 1 6 4
> Press F12 (CombatFormulaInspector)
Expected: Fireball damage calculated, Fighter HP decreases

> castspell dragon createwall 1 5 4
Expected: Wall created at (5, 4)

> castspell dragon lavapool 1 6 5
Expected: Lava pool created at (6, 5)

# Verify turn system
> showturnorder
Expected: Dragon and Fighter in initiative order
> endturn
Expected: Turn advances

# Verify god mode (for quick testing)
> god on
> damage dragon 100
Expected: Dragon takes no damage (invincible)
```

---

---

### Week 6: Dragon Spells Expansion

**Gameplay Systems Built** (implementation-plan.md:370-385):

- Dragon spells 4-6: DragonRoar, TailSwipe, DragonWrath (Dev A)
- StatusEffectManager GSComponent foundation (Dev B)
- AIDirector singleton basics (Dev C)
- SpellFactory foundation (Dev D)
- **CombatFormulaInspector (F12) - Dev E**

**Debug Tools to Implement**:

- [ ] **CombatFormulaInspector (F12)** - Damage calculation debugging
  
  - Show dice roll breakdown (3d6 = [4, 5, 3] = 12)
  - Display all modifiers (+5 attack, -3 defense, +2 blessing, etc.)
  - Show final damage calculation formula
  - Display min/max damage range
  - **Purpose**: Verify complex damage formulas with multiple modifiers

- [ ] **Status Effect Tracking Commands** (Week 6)
  
  - Track Fear, Burn, Stun, Curse effects
  - Console should log when status effects applied/removed
  - EventBusTracer (Ctrl+E) shows StatusEffectAppliedEvent

**Integration Test Support** (implementation-plan.md:380-385):

```
# Test DragonRoar spell (applies Fear)
> spawn dragon 3 7
> spawn fighter 6 4
> castspell dragon dragonroar 1 6 4
Expected: Fear effect applied to Fighter

> Ctrl+E (EventBusTracer)
Expected: StatusEffectAppliedEvent logged with effect=Fear, duration=2

# Test CombatFormulaInspector
> castspell dragon fireball 1 6 4
> Press F12
Expected: Shows "BASE: 3d6 = [4,5,3] = 12"
Expected: Shows "MODIFIERS: +5 attack, -3 defense"
Expected: Shows "FINAL DAMAGE: 14"
```

---

### Week 7: Advanced Dragon Spells + Status Effects

**Gameplay Systems Built** (implementation-plan.md:388-404):

- Dragon spells 7-9: MeteorStrike, HeatAbsorb, HeatRelease (Dev A)
- StatusEffectManager integration with stat modification (Dev B)
- AIDirector threat calculation (Dev C)
- EffectFactory foundation (Dev D)
- **StatusInfoOverlay (F7) - Dev E**

**Debug Tools to Implement**:

- [ ] **StatusInfoOverlay (F7)** - Real-time character stats display
  
  - HP bars above all characters (green/yellow/red)
  - Action points bar (blue bar below HP)
  - Spell slots remaining (small icons)
  - Active status effect badges (Fear = purple "F", Burn = red "B")
  - Turn order panel (side display)
  - **Keyboard**: F7 to toggle

- [ ] **Status Effect Commands** (Week 7)
  
  - `clearstatus <characterName>` - Remove all status effects
  - `applystatus <characterName> <effectType> <duration>`
  - Example: `applystatus fighter fear 3` (Fear for 3 turns)

**Integration Test Support** (implementation-plan.md:399-404):

```
# Test all 9 Dragon spells functional
> spawn dragon 3 7
> spawn fighter 6 4

> castspell dragon fireball 1 6 4
> castspell dragon createwall 1 5 4
> castspell dragon lavapool 1 6 5
> castspell dragon dragonroar 1 6 4
> castspell dragon tailswipe 1 6 4
> castspell dragon dragonwrath 2 6 4
> castspell dragon meteorstrike 3 6 4
> castspell dragon heatabsorb 1 3 7
> castspell dragon heatrelease 1 6 4

Expected: All 9 spells cast successfully

# Test status effects modify combat
> applystatus fighter fear 2
> Press F7 (StatusInfoOverlay)
Expected: Fighter has purple "F" badge above HP bar
Expected: Fighter attack stat shows -2 (Fear penalty)

> damage dragon 30
Expected: StatusInfoOverlay updates Dragon HP bar (green → yellow)
```

---

### Week 8: Fighter Character + AI Foundation

**Gameplay Systems Built** (implementation-plan.md:407-422):

- Fighter character class (Dev A)
- Fighter combat abilities: Melee, Shield Bash, Taunt (Dev B)
- AISystem GSComponent foundation (Dev C)
- CharacterFactory foundation (Dev D)
- **AIDebugVisualizer (F4) basics - Dev E**

**Debug Tools to Implement**:

- [ ] **AIDebugVisualizer (F4)** - Basic AI path visualization
  
  - Show AI character's current target (red X marker)
  - Draw pathfinding route (yellow dotted line)
  - Display AI state text above character ("Attacking", "Moving", etc.)
  - **Keyboard**: F4 to toggle

- [ ] **AI Control Commands** (Week 8)
  
  - `disableai <characterName>` - Stop AI from acting
  - `enableai <characterName>` - Re-enable AI
  - `showaitarget <characterName>` - Print AI's current target

**Integration Test Support** (implementation-plan.md:417-422):

```
# Test Fighter spawns via CharacterFactory
> spawn fighter 6 4
Expected: Fighter appears at (6, 4) with HP=90, speed=3

# Test Fighter abilities
> # (Manual control for now - AI comes Week 9)
> castspell fighter shieldbash 1 3 7
Expected: Shield Bash ability works (damages + pushes Dragon)

> Press F4 (AIDebugVisualizer)
Expected: Shows Fighter's current target (Dragon) with red X
Expected: Yellow dotted line shows path Fighter will take
```

---

### Week 9: Fighter AI + DataRegistry Hot Reload

**Gameplay Systems Built** (implementation-plan.md:425-440):

- Fighter AI behavior - manual mode (Dev A)
- GridSystem LOS (line of sight) (Dev B)
- AISystem basic decision tree (Dev C)
- DataRegistry hot-reload system (Dev D)
- **HotReloadManager integration - Dev E**

**Debug Tools to Implement**:

- [ ] **HotReloadManager Polish** (Week 9)
  
  - File watcher for all JSON files in `Assets/Data/`
  - Auto-reload when files change (configurable)
  - Validation before reload (don't crash on invalid JSON)
  - Console notification: "[HotReload] Reloaded characters.json"
  - **Command**: `reload <datatype>` or auto-reload on file save

- [ ] **AI Decision Logging** (Week 9)
  
  - EventBusTracer shows AIDecisionEvent
  - Console logs: "Fighter decided to move toward Dragon (threat=10)"
  - F4 overlay shows threat values above enemies

**Integration Test Support** (implementation-plan.md:435-440):

```
# Test Fighter AI makes basic decisions
> spawn dragon 3 7
> spawn fighter 6 4
> enableai fighter
> endturn  # Dragon's turn ends, Fighter's turn starts

> Press F4 (AIDebugVisualizer)
Expected: Fighter AI shows target = Dragon
Expected: Path drawn toward Dragon
Expected: AI state text = "Attacking"

> Ctrl+E (EventBusTracer)
Expected: AIDecisionEvent logged: "Fighter chose action: MoveToward"

# Test hot reload
> # Edit characters.json: change Fighter maxHP to 100
> reload characters
Expected: "Reloaded characters.json successfully" (green text)

> spawn fighter 6 4
Expected: Fighter HP = 100 (not 90)
```

---

### Week 10: Polish + PLAYTEST 2

**Gameplay Systems Built** (implementation-plan.md:443-451):

- Dragon spell balance tuning (Dev A)
- Combat system polish (Dev B)
- Fighter AI polish (Dev C)
- Data-driven spell system (Dev D)
- **Integration build for Playtest 2 - Dev E**

**Debug Tools to Implement**:

- [ ] **Playtest 2 Debug Checklist** - All 9 spells + Fighter AI
- [ ] **Performance monitoring** (F9) - Frame rate, memory usage
- [ ] **Debug UI refinement** - All overlays polished

**Playtest 2 Debug Checklist** (NEW - implementation-plan.md:453-469):

For PLAYTEST 2 (Friday Week 10), testers should use these commands:

```
# Verify all 9 Dragon spells
> spawn dragon 3 7
> spawn fighter 6 4

# Test each spell (use F12 to verify damage)
> castspell dragon fireball 1 6 4
> castspell dragon createwall 1 5 4
> castspell dragon lavapool 1 6 5
> castspell dragon dragonroar 1 6 4
> castspell dragon tailswipe 1 6 4
> castspell dragon dragonwrath 2 6 4
> castspell dragon meteorstrike 3 6 4
> castspell dragon heatabsorb 1 3 7
> castspell dragon heatrelease 1 6 4
Expected: All 9 spells work without crashes

# Verify Fighter AI
> enableai fighter
> endturn
> Press F4
Expected: Fighter AI moves toward Dragon and attacks

# Verify status effects
> applystatus fighter fear 2
> Press F7
Expected: Fear badge visible, Fighter attack reduced

# Verify hot reload
> # Edit characters.json
> reload characters
Expected: Changes applied without restart

# Verify debug tools
> Press F1 (Grid overlay)
> Press F4 (AI paths)
> Press F7 (Status info)
> Press F9 (Dice history)
> Press F12 (Combat formula)
> Ctrl+E (Event tracer)
Expected: All debug overlays functional
```

---

### Week 11: Cleric Character + Healing System

**Gameplay Systems Built** (implementation-plan.md:491-506):

- Cleric character class (Dev A)
- Healing spell system: Healing Word, Cure Wounds, Mass Healing (Dev B)
- AISystem ally awareness (Dev C)
- SpellSlots GOComponent (Dev D)
- **Spell selection UI - Dev E**

**Debug Tools to Implement**:

- [ ] **Spell Slot Inspector** (Week 11)
  
  - `spellslots <characterName>` - View/modify spell slots
  - Example: `spellslots dragon` shows Level 1: 3/3, Level 2: 3/3, Level 3: 2/2
  - `refillslots <characterName>` - Restore all spell slots
  - `givespell <characterName> <level>` - Add spell slots

- [ ] **Healing Verification** (Week 11)
  
  - Console logs healing events
  - F12 (CombatFormulaInspector) shows healing calculations
  - StatusInfoOverlay (F7) updates HP bars immediately

**Integration Test Support** (implementation-plan.md:501-506):

```
# Test Cleric spawns and has healing spells
> spawn cleric 4 0
Expected: Cleric at (4, 0), HP=90, speed=2

> spellslots cleric
Expected: Shows "Level 1: 3/3, Level 2: 2/2, Level 3: 1/1"

# Test healing spell
> spawn fighter 6 4
> damage fighter 40
Expected: Fighter HP: 90 → 50

> castspell cleric healingword 1 6 4
> Press F12 (CombatFormulaInspector)
Expected: Shows "HEALING: 1d4+4 = [3]+4 = 7"
Expected: Fighter HP: 50 → 57

# Test spell slot consumption
> spellslots cleric
Expected: "Level 1: 2/3" (one slot consumed)

> refillslots cleric
> spellslots cleric
Expected: "Level 1: 3/3" (all slots restored)
```

---

### Week 12: Wizard Character + Offensive Magic

**Gameplay Systems Built** (implementation-plan.md:509-524):

- Wizard character class (Dev A)
- Wizard offensive spells: Magic Missile, Fireball, Lightning Bolt (Dev B)
- AISystem threat assessment (Dev C)
- TargetingSystem GOComponent (Dev D)
- **DiceHistoryPanel (F9) polish - Dev E**

**Debug Tools to Implement**:

- [ ] **TargetingSystem Debugger** (Week 12)
  
  - `showtargets <characterName>` - List all valid targets for spells
  - F11 (CollisionDebugRenderer) shows spell AOE circles
  - F1 (GridDebugRenderer) shows spell range overlay

- [ ] **DiceHistoryPanel (F9) Enhancement** (Week 12)
  
  - Show last 20 dice rolls (increased from 10)
  - Filter by damage type (fire, cold, lightning)
  - Statistics panel (min, max, average)
  - Color-coded: Green (high roll), Yellow (average), Red (low roll)

**Integration Test Support** (implementation-plan.md:519-524):

```
# Test Wizard spawns with offensive spells
> spawn wizard 1 0
Expected: Wizard at (1, 0), HP=55, speed=1

# Test Magic Missile (auto-hit spell)
> spawn dragon 3 7
> castspell wizard magicmissile 1 3 7
> Press F12
Expected: Magic Missile hits automatically (no attack roll)
Expected: Damage: 1d4+1 per missile

# Test Wizard Fireball AOE
> spawn fighter 6 4
> spawn rogue 6 5
> castspell wizard fireball 3 6 4
> Press F11 (CollisionDebugRenderer)
Expected: Orange AOE circle shows 3x3 area
Expected: Both Fighter and Rogue take damage

# Test Dice History
> Press F9 (DiceHistoryPanel)
Expected: Shows last 20 rolls with color coding
Expected: Fireball damage rolls visible with "8d6" notation
```

---

### Week 13: Rogue Character + Mobility

**Gameplay Systems Built** (implementation-plan.md:527-542):

- Rogue character class (Dev A)
- Rogue mobility abilities: Dash, Disengage, Hide (Dev B)
- AIMemory GOComponent (Dev C)
- DamageCalculator GOComponent (Dev D)
- **GridDebugRenderer polish (F1) - Dev E**

**Debug Tools to Implement**:

- [ ] **Movement Range Visualization** (Week 13)
  
  - F1 (GridDebugRenderer) shows character-specific movement ranges
  - Rogue with Dash: Blue overlay shows 8 tiles (double movement)
  - Normal movement: 4 tiles for Rogue
  - **Purpose**: Verify mobility abilities work correctly

- [ ] **AI Memory Inspector** (Week 13)
  
  - `showaimemory <characterName>` - Display AI's tactical memory
  - Example: "Rogue remembers: Dragon last seen at (3,7), threat=10"
  - EventBusTracer shows AIMemoryUpdatedEvent

**Integration Test Support** (implementation-plan.md:537-542):

```
# Test Rogue spawns with mobility
> spawn rogue 5 0
Expected: Rogue at (5, 0), HP=65, speed=4

# Test normal movement
> Press F1 (GridDebugRenderer)
Expected: Blue overlay shows 4-tile movement range

# Test Dash ability (double movement)
> castspell rogue dash 1
> Press F1
Expected: Blue overlay shows 8-tile movement range (4 * 2)

# Test AI Memory
> spawn dragon 3 7
> enableai rogue
> endturn  # Rogue's turn
> showaimemory rogue
Expected: "Dragon last seen: (3, 7), turns ago: 0, threat: 10"

# Test Disengage (no opportunity attacks)
> castspell rogue disengage 1
> # Move rogue away from dragon
Expected: No opportunity attack triggered
```

---

### Week 14: All Characters Integration

**Gameplay Systems Built** (implementation-plan.md:545-560):

- Character balance tuning (Dev A)
- GridPosition GOComponent (Dev B)
- ActionPoints GOComponent (Dev C)
- CharacterFactory complete (Dev D)
- **BattleState multi-character support - Dev E**

**Debug Tools to Implement**:

- [ ] **Multi-Character Testing Commands** (Week 14)
  
  - `spawnall` - Spawn all 5 characters in starting positions
  - `listcharacters` - List all spawned characters with stats
  - `resetbattle` - Clear all characters and restart

- [ ] **Action Points Debugger** (Week 14)
  
  - `showap <characterName>` - Display current/max action points
  - `giveap <characterName> <amount>` - Add action points
  - `setap <characterName> <amount>` - Set action points to specific value
  - StatusInfoOverlay (F7) shows AP bars for all characters

**Integration Test Support** (implementation-plan.md:555-560):

```
# Test spawn all 5 characters
> spawnall
Expected:
  - Dragon at (3, 7)
  - Fighter at (2, 0)
  - Cleric at (4, 0)
  - Wizard at (1, 0)
  - Rogue at (5, 0)

> listcharacters
Expected:
  Dragon: HP=140/140, AP=5/5, Speed=2, Grid=(3,7)
  Fighter: HP=90/90, AP=5/5, Speed=3, Grid=(2,0)
  Cleric: HP=90/90, AP=5/5, Speed=2, Grid=(4,0)
  Wizard: HP=55/55, AP=5/5, Speed=1, Grid=(1,0)
  Rogue: HP=65/65, AP=5/5, Speed=4, Grid=(5,0)

# Test turn order with all characters
> showturnorder
Expected: All 5 characters listed in initiative order

# Test action points system
> showap dragon
Expected: "Dragon: 5/5 AP"

> giveap dragon 3
Expected: "Dragon: 8/5 AP" (can exceed max)

# Test character factory
> spawn fighter 7 7
> listcharacters
Expected: Shows 6 total characters (original 5 + new Fighter)
```

---

### Week 15: Basic AI for All + PLAYTEST 3

**Gameplay Systems Built** (implementation-plan.md:563-572):

- Cleric AI (healing priority) (Dev A)
- Wizard AI (kiting behavior) (Dev B)
- Rogue AI (flanking logic) (Dev C)
- AIBehaviorFactory foundation (Dev D)
- **Integration build for Playtest 3 - Dev E**

**Debug Tools to Implement**:

- [ ] **AI Behavior Inspector** (Week 15)
  
  - `showai behavior <characterName>` - Display AI decision weights
  - F4 (AIDebugVisualizer) shows all 4 AI characters' paths simultaneously
  - Threat values displayed above all enemies

- [ ] **Playtest 3 Debug Checklist** - Full 1v4 battle

**Playtest 3 Debug Checklist** (NEW - implementation-plan.md:574-590):

For PLAYTEST 3 (Friday Week 15), testers should use these commands:

```
# Spawn all 5 characters for full battle
> spawnall
Expected: Dragon vs 4 AI opponents

# Enable all AI
> enableallai
Expected: Fighter, Cleric, Wizard, Rogue all AI-controlled

# Verify each AI makes basic decisions
> endturn  # Dragon's turn ends
> Press F4 (AIDebugVisualizer)
Expected:
  - Fighter: Moving toward Dragon (tank behavior)
  - Cleric: Checking ally HP (healing priority)
  - Wizard: Keeping distance from Dragon (kiting)
  - Rogue: Flanking Dragon from behind

> Ctrl+E (EventBusTracer)
Expected: AIDecisionEvent for each character:
  - "Fighter chose: MoveToward (target=Dragon)"
  - "Cleric chose: HealLowestAlly (target=Fighter)"
  - "Wizard chose: CastSpell (spell=MagicMissile, target=Dragon)"
  - "Rogue chose: Flank (target=Dragon)"

# Test full turn cycle
> nextturn 5
Expected: All characters take turns, no crashes

# Test victory/defeat conditions
> god on  # Enable invincibility for testing
> damage fighter 90
> damage cleric 90
> damage wizard 55
> damage rogue 65
Expected: All enemies dead, battle ends (Dragon wins)

# Test Cleric AI healing
> resetbattle
> spawnall
> enableallai
> damage fighter 40
> endturn  # Wait for Cleric's turn
> Press F4
Expected: Cleric targets Fighter (lowest HP ally) with healing spell

# Verify all debug tools work in full battle
> Press F1 (Grid with all characters)
> Press F4 (All AI paths)
> Press F7 (All HP/AP bars)
> Press F9 (Dice history for all rolls)
> Press F12 (Combat formula for current attack)
> Ctrl+E (Event stream for full battle)
Expected: All overlays functional with 5 characters
```

---

### Week 16: AI Team Coordination

**Gameplay Systems Built** (implementation-plan.md:612-627):

- Fighter AI advanced (protect allies) (Dev A)
- CombatSystem knockback/push mechanics (Dev B)
- AIDirector team tactics coordination (Dev C)
- Event-driven AI communication (Dev D)
- **AI threat visualization (F4) enhancement - Dev E**

**Debug Tools to Implement**:

- [ ] **Threat Value Display** (Week 16)
  
  - F4 (AIDebugVisualizer) shows threat numbers above all enemies
  - Color-coded: Red (high threat), Yellow (medium), Green (low)
  - `showthreat <characterName>` - Display threat calculation breakdown
  - Example: "Dragon threat to Fighter: 10 (HP=140, Damage=high, Range=far)"

- [ ] **Team Coordination Visualizer** (Week 16)
  
  - F4 shows AI coordination lines (dotted lines between allies)
  - Console logs: "Fighter positioning between Dragon and Cleric (protect behavior)"
  - EventBusTracer shows AICoordinationEvent

**Integration Test Support** (implementation-plan.md:622-627):

```
# Test Fighter protects Cleric from Dragon
> spawnall
> enableallai
> damage cleric 40  # Make Cleric vulnerable
> endturn

> Press F4 (AIDebugVisualizer)
Expected: Fighter moves between Dragon and Cleric (protect position)
Expected: Dotted line from Fighter to Cleric (protection link)

# Test threat visualization
> showthreat dragon
Expected: Shows threat values for all enemies:
  "Fighter: 8 (tank, high HP)"
  "Cleric: 6 (healer, medium priority)"
  "Wizard: 7 (damage dealer, low HP)"
  "Rogue: 5 (flanker, fast)"

# Test AIDirector coordination
> Ctrl+E (EventBusTracer)
Expected: AICoordinationEvent:
  "AIDirector: Fighter protects Cleric (threat=6, HP=50/90)"
```

---

### Week 17: Advanced AI Behaviors

**Gameplay Systems Built** (implementation-plan.md:630-645):

- Cleric AI (buff priority) (Dev A)
- Wizard AI (mana management) (Dev B)
- Rogue AI (stealth mechanics) (Dev C)
- AIBehaviorFactory behavior trees (Dev D)
- **EventBusTracer (Ctrl+E) enhancement - Dev E**

**Debug Tools to Implement**:

- [ ] **AI Behavior Tree Visualizer** (Week 17)
  
  - `showbehaviortree <characterName>` - Display AI decision tree
  - Example: Shows Cleric decision tree: "Check ally HP → Cast buff → Heal lowest"
  - F4 shows decision nodes with checkmarks (executed) and X marks (skipped)

- [ ] **EventBusTracer Filtering** (Week 17)
  
  - Ctrl+E opens event tracer with filter dropdown
  - Filter by event type: "Show only AIDecisionEvent"
  - Filter by character: "Show only Dragon events"
  - Export event log to file: `exportevents <filename>`

**Integration Test Support** (implementation-plan.md:640-645):

```
# Test Cleric AI buff priority
> spawnall
> enableallai
> endturn  # Cleric's turn

> showbehaviortree cleric
Expected:
  1. ✓ Check ally HP (all above 50%)
  2. ✓ Cast Blessing on Fighter (buff priority)
  3. ✗ Heal (not needed)

# Test Wizard AI mana management
> spawnall
> enableallai
> # Let Wizard cast several spells
> nextturn 10

> spellslots wizard
Expected: Wizard conserves high-level slots (Level 3: 1/1 unused)
Expected: Wizard uses low-level slots first (Level 1: 0/3)

# Test EventBusTracer filtering
> Ctrl+E
> Set filter: "AIDecisionEvent"
Expected: Shows only AI decision events, no other events

> exportevents playtest3_events.txt
Expected: "Exported 247 events to playtest3_events.txt"
```

---

### Week 18: Status Effect Expansion

**Gameplay Systems Built** (implementation-plan.md:648-663):

- StatusEffects GOComponent complete (Dev A)
- Status effect interactions (Dev B)
- AISystem status awareness (Dev C)
- EffectFactory expansion (Blessing, Curse, Poison) (Dev D)
- **StatusInfoOverlay badges enhancement - Dev E**

**Debug Tools to Implement**:

- [ ] **Status Effect Badge System** (Week 18)
  
  - F7 (StatusInfoOverlay) shows effect icons above HP bars
  - Icons: Fear (purple F), Burn (red B), Blessing (gold +), Poison (green P), Curse (black C)
  - Tooltip on hover: "Fear: -2 attack, 2 turns remaining"
  - Multiple effects stack visually (up to 5 icons)

- [ ] **Status Effect Commands** (Week 18)
  
  - `applystatus <char> <effect> <duration> <intensity>`
  - Example: `applystatus fighter poison 3 2` (Poison for 3 turns, 2 dmg/turn)
  - `liststatus <characterName>` - List all active effects with details

**Integration Test Support** (implementation-plan.md:658-663):

```
# Test multiple status effects simultaneously
> spawn fighter 6 4
> applystatus fighter burn 3 5
> applystatus fighter fear 2 2
> applystatus fighter curse 4 1

> Press F7 (StatusInfoOverlay)
Expected: Fighter has 3 badges: Red "B", Purple "F", Black "C"
Expected: HP bar shows continuous damage ticks from Burn

> liststatus fighter
Expected:
  "Burn: 5 dmg/turn, 3 turns remaining"
  "Fear: -2 attack, 2 turns remaining"
  "Curse: -1 to all rolls, 4 turns remaining"

# Test AI avoids lava
> spawnall
> castspell dragon lavapool 1 4 2
> enableallai
> endturn

> Press F4
Expected: Fighter path avoids lava tile at (4, 2)
Expected: Console log: "Fighter avoided hazard at (4,2)"
```

---

### Week 19: Spell System Polish

**Gameplay Systems Built** (implementation-plan.md:666-681):

- Spell upcasting system (Dev A)
- Spell area of effect (AOE) (Dev B)
- Spell validation (Dev C)
- SpellFactory complete (all spells data-driven) (Dev D)
- **Spell targeting debug commands - Dev E**

**Debug Tools to Implement**:

- [ ] **Spell Range Visualizer** (Week 19)
  
  - F1 (GridDebugRenderer) shows spell range overlay
  - Orange overlay: Attack range
  - Red overlay: Out of range tiles
  - Blue overlay: AOE tiles (when spell selected)

- [ ] **Spell Validation Debugger** (Week 19)
  
  - `validatespell <caster> <spell> <level> <targetX> <targetY>`
  - Shows all validation checks: Range ✓, LOS ✓, AP ✓, Spell Slots ✓
  - If fails, shows reason: "FAILED: Out of range (distance=6, max=5)"

**Integration Test Support** (implementation-plan.md:676-681):

```
# Test spell upcasting
> spawn dragon 3 7
> spawn fighter 6 4
> spellslots dragon
Expected: "Level 1: 3/3, Level 2: 3/3, Level 3: 2/2"

> castspell dragon fireball 3 6 4  # Upcast to level 3
> Press F12 (CombatFormulaInspector)
Expected: "Fireball (Level 3): 8d6 = 32 damage"
Expected: Spell slot consumed: "Level 3: 1/2"

# Test AOE spells hit multiple targets
> spawn fighter 6 4
> spawn rogue 6 5
> spawn cleric 5 4
> castspell dragon fireball 3 6 4

> Press F11 (CollisionDebugRenderer)
Expected: Orange AOE circle shows 3x3 area
Expected: Fighter, Rogue, Cleric all take damage

# Test spell validation
> validatespell dragon fireball 1 0 0
Expected:
  Range: ✗ (distance=9, max=5)
  LOS: ✗ (blocked)
  AP: ✓ (5/5 available)
  Spell Slots: ✓ (3/3 Level 1)
  RESULT: Cannot cast (out of range)

> validatespell dragon fireball 1 6 4
Expected: All checks ✓, RESULT: Can cast
```

---

### Week 20: Full Battle Integration + PLAYTEST 4

**Gameplay Systems Built** (implementation-plan.md:684-692):

- Character ability polish (all 20+ abilities) (Dev A)
- Combat system complete (all damage types, resistances) (Dev B)
- AISystem advanced tactics (focus fire, protect healers, kiting) (Dev C)
- DataRegistry complete (hot reload for all JSON files) (Dev D)
- **Integration build for Playtest 4 - Dev E**

**Debug Tools to Implement**:

- [ ] **Battle State Inspector** (Week 20)
  
  - `showbattlestate` - Display complete battle state
  - Shows: Turn number, active character, all character stats, battle duration
  - `savebattlestate <filename>` - Save current state to JSON for bug reproduction
  - `loadbattlestate <filename>` - Restore battle from saved state

- [ ] **Playtest 4 Debug Checklist** - Advanced AI + Full 1v4 Battle

**Playtest 4 Debug Checklist** (NEW - implementation-plan.md:694-710):

For PLAYTEST 4 (Friday Week 20), testers should use these commands:

```
# Test complete 1v4 battle from start to finish
> spawnall
> enableallai
> showturnorder
Expected: Full initiative order, all 5 characters

# Play 10 full turns
> # Let battle play naturally for 10 turns
> showbattlestate
Expected:
  Turn: 10
  Duration: 5m 23s
  Active: Wizard
  Dragon HP: 95/140
  Enemies: Fighter(70/90), Cleric(90/90), Wizard(55/55), Rogue(45/65)

# Test AI team coordination
> Press F4 (AIDebugVisualizer)
Expected:
  - Fighter protecting Cleric (tank position)
  - Cleric healing lowest HP ally (Rogue)
  - Wizard keeping distance from Dragon (kiting)
  - Rogue flanking Dragon from behind

# Test focus fire tactic
> damage wizard 30  # Make Wizard vulnerable
> endturn  # Wait for AI turns

> Ctrl+E (EventBusTracer)
Expected: Multiple AI characters target Wizard:
  "Fighter chose: Attack Wizard (low HP priority)"
  "Rogue chose: Attack Wizard (focus fire)"

# Test all spells, abilities, status effects
> # Play battle to completion
Expected: No crashes, all 20+ abilities work

# Save battle state for bug reproduction
> showbattlestate
> savebattlestate playtest4_bug1.json
Expected: "Saved battle state to playtest4_bug1.json"

# Test hot reload during battle
> # Edit characters.json (change Dragon maxHP to 200)
> reload characters
Expected: "Warning: Hot reload during battle may cause inconsistencies"
Expected: Dragon HP cap updates (140 → 200)

# Verify all debug tools in full battle
> Press F1 (Grid overlay)
> Press F4 (AI coordination, threat values, paths)
> Press F7 (HP/AP bars, status badges)
> Press F9 (Dice history, last 20 rolls)
> Press F11 (Collision boxes, spell AOE)
> Press F12 (Combat formula)
> Ctrl+E (Event stream, filter by type)
Expected: All overlays functional, no performance degradation
```

---

### Week 21-24: Visual Polish & Production Quality (PLAYTEST 5)

**Gameplay Systems Built** (implementation-plan.md:714-810):

- Character animations (Week 21)
- Spell visual effects (Week 21)
- Campaign & save system (Week 22)
- Visual polish (Week 23)
- Performance optimization (Week 24)

**Debug Tools to Implement** (Weeks 21-24):

---

### Week 21: Visual Effects Foundation

**Gameplay Systems Built** (implementation-plan.md:732-748):

- Character animations (idle, attack, cast, hurt, death) (Dev A)
- Spell visual effects (explosions, beams, sparkles) (Dev B)
- EffectManager particle system (Dev C)
- MapFactory visual elements (Dev D)
- **UI polish - Dev E**

**Debug Tools to Implement**:

- [ ] **Animation Debugger** (Week 21)
  
  - `showanim <characterName>` - Display current animation state
  - `playanim <characterName> <animName>` - Force play animation
  - F10 toggle: Show animation frame numbers
  - Example: `playanim dragon attack` forces attack animation

- [ ] **Effect Spawner** (Week 21)
  
  - `spawneffect <effectName> <x> <y>` - Spawn visual effect
  - Example: `spawneffect explosion 6 4`
  - Purpose: Test particle effects without triggering spells

**Integration Test Support** (implementation-plan.md:743-748):

```
# Test character animations
> spawn dragon 3 7
> showanim dragon
Expected: "Dragon: idle (frame 3/8, loop)"

> playanim dragon attack
Expected: Dragon plays attack animation, returns to idle

> castspell dragon fireball 1 6 4
Expected: Dragon plays "cast" animation, then fireball visual effect

# Test spell visual effects
> spawneffect explosion 6 4
Expected: Explosion particle effect at (6, 4)

> spawneffect healingsparkles 3 7
Expected: Healing sparkles around Dragon

# Test all animations
> playanim dragon idle
> playanim dragon attack
> playanim dragon cast
> playanim dragon hurt
> playanim dragon death
Expected: All animations play without errors

# Verify HP bars animate smoothly
> spawn fighter 6 4
> damage fighter 40
> Press F7 (StatusInfoOverlay)
Expected: HP bar smoothly animates from 90 → 50 (not instant)
```

---

### Week 22: Campaign & Save System

**Gameplay Systems Built** (implementation-plan.md:750-766):

- Campaign progression (3 battle scenarios) (Dev A)
- BattleManager victory conditions (Dev B)
- SaveManager save/load system (Dev C)
- Campaign data structure (Dev D)
- **Save/load debug commands - Dev E**

**Debug Tools to Implement**:

- [ ] **Save System Debugger** (Week 22)
  
  - `save <slotname>` - Save current game state
  - `load <slotname>` - Load saved game state
  - `listsaves` - List all save files with timestamps
  - `showsave <slotname>` - Display save file contents (JSON preview)
  - `deletesave <slotname>` - Delete save file

- [ ] **Campaign Progression Inspector** (Week 22)
  
  - `showcampaign` - Display campaign progress
  - Shows: Battles completed, score, unlocks, stats
  - `setcampaignprogress <battleNumber>` - Jump to specific battle

**Integration Test Support** (implementation-plan.md:761-766):

```
# Test save/load system
> spawnall
> enableallai
> nextturn 5
> showbattlestate
Expected: Turn 5, Dragon HP: 110/140

> save playtest5_save1
Expected: "Saved game to playtest5_save1.sav"

> damage dragon 50
Expected: Dragon HP: 60/140

> load playtest5_save1
Expected: "Loaded game from playtest5_save1.sav"
Expected: Dragon HP restored to 110/140, Turn 5

# Test save file inspection
> listsaves
Expected:
  "playtest5_save1.sav - 2025-10-12 14:35"
  "playtest5_save2.sav - 2025-10-12 15:20"

> showsave playtest5_save1
Expected: JSON preview showing:
  - Turn: 5
  - Characters: Dragon(110/140), Fighter(70/90), ...
  - Campaign Progress: Battle 1, Score: 1250

# Test campaign progression
> showcampaign
Expected:
  Campaign Progress:
  - Battle 1: ✓ Complete (Score: 1500)
  - Battle 2: In Progress
  - Battle 3: Locked

> setcampaignprogress 3
Expected: "Set campaign to Battle 3"
Expected: Battle 3 unlocked for testing
```

---

### Week 23: Visual Polish

**Gameplay Systems Built** (implementation-plan.md:768-783):

- Character sprite polish (high-quality art) (Dev A)
- Spell effect polish (smooth animations) (Dev B)
- UI visual design (polished menus, buttons) (Dev C)
- Map visual polish (detailed terrain) (Dev D)
- **Camera effects - Dev E**

**Debug Tools to Implement**:

- [ ] **Camera Effects Debugger** (Week 23)
  
  - `shakecamera <intensity> <duration>` - Test screen shake
  - `zoomcamera <level>` - Test camera zoom (0.5 = far, 2.0 = close)
  - `focuscamera <characterName>` - Center camera on character
  - `resetcamera` - Reset to default view

- [ ] **UI Inspector** (Week 23)
  
  - Shift+F1: Toggle UI bounding boxes (shows layout)
  - `showui` - List all active UI elements
  - `hideui <elementName>` - Temporarily hide UI element

**Integration Test Support** (implementation-plan.md:778-783):

```
# Test camera effects
> spawn dragon 3 7
> spawn fighter 6 4
> castspell dragon fireball 1 6 4

> shakecamera 5 1.0
Expected: Screen shakes for 1 second (intensity 5)

> zoomcamera 1.5
Expected: Camera zooms in 150%

> focuscamera fighter
Expected: Camera centers on Fighter

> resetcamera
Expected: Camera returns to default battlefield view

# Test UI layout
> Shift+F1
Expected: Red bounding boxes show around all UI elements

> showui
Expected: List of UI elements:
  "HP Bars (visible)"
  "Status Badges (visible)"
  "Turn Order Panel (visible)"
  "Console (hidden)"

> hideui "Turn Order Panel"
Expected: Turn order panel temporarily hidden

# Verify all placeholders replaced
> # Check all characters, spells, effects
Expected: No placeholder art visible, all final sprites loaded
```

---

### Week 24: Performance Optimization + PLAYTEST 5

**Gameplay Systems Built** (implementation-plan.md:786-804):

- Character optimization (Dev A)
- GridSystem optimization (Dev B)
- AI performance tuning (Dev C)
- Asset loading optimization (Dev D)
- **Performance profiling tools - Dev E**

**Debug Tools to Implement**:

- [ ] **Performance Profiler** (Week 24)
  
  - Shift+F12: Toggle performance overlay
  - Shows: FPS, Frame time (ms), Update time, Draw time
  - Memory usage: Heap, GameObject count, Event count
  - `profile <duration>` - Profile for N seconds, show report

- [ ] **Performance Commands** (Week 24)
  
  - `showfps` - Display FPS counter
  - `showmemory` - Display memory usage
  - `profileframe` - Profile single frame, show breakdown
  - `clearprofile` - Reset profiling data

**Integration Test Support** (implementation-plan.md:799-804):

```
# Test performance monitoring
> Shift+F12 (Performance Overlay)
Expected: Shows real-time stats:
  FPS: 60.0 (min: 58.5, max: 60.0)
  Frame Time: 16.6ms
  Update: 8.2ms, Draw: 6.4ms, Input: 0.8ms
  Memory: 45MB / 512MB
  GameObjects: 23 (5 Characters, 12 Effects, 6 UI)
  EventBus: 15 events/sec, 45 subscribers

# Profile full battle
> spawnall
> enableallai
> profile 30
Expected: "Profiling for 30 seconds..."
Expected: After 30 seconds, shows report:
  Average FPS: 59.8
  Frame spikes: 3 (>20ms)
  Memory leaked: 0 bytes
  Slowest function: AISystem::MakeDecision (3.2ms avg)

# Test pathfinding optimization
> spawn rogue 0 0
> teleport rogue 7 7
Expected: Pathfinding calculates instantly (<1ms)

# Test many characters (stress test)
> spawn fighter 0 0
> spawn fighter 1 0
> spawn fighter 2 0
> # ... spawn 20 fighters
> showfps
Expected: FPS remains above 30 (acceptable performance)
```

**Playtest 5 Debug Checklist** (NEW - implementation-plan.md:796-813):

For PLAYTEST 5 (Friday Week 24), testers should use these commands:

```
# Test full campaign (3 battles)
> showcampaign
Expected: Battle 1 available

> # Play Battle 1 to completion
> # (Dragon vs 4 enemies)
Expected: Battle ends, victory screen, score calculated

> save after_battle1
Expected: Progress saved

> # Play Battle 2
Expected: Difficulty increased (enemies stronger)

> # Play Battle 3
Expected: Final battle, hardest difficulty

# Test save/load between battles
> showcampaign
Expected: All 3 battles complete

> load after_battle1
Expected: Campaign restored to after Battle 1

# Test performance in full battle
> spawnall
> enableallai
> Shift+F12 (Performance Overlay)
> # Play 20 turns
Expected: FPS consistently 60, no frame drops
Expected: Memory stable (no leaks)

# Test all visual effects work
> # Trigger all 20+ spells/abilities
Expected: All animations play smoothly
Expected: No placeholder art visible
Expected: Camera shake on impacts

# Verify UI is polished
> # Check all menus, buttons, overlays
Expected: Professional appearance
Expected: No layout bugs, text overflow, or alignment issues

# Test save file integrity
> save final_test
> listsaves
> showsave final_test
Expected: Save file valid JSON
Expected: All campaign data preserved
```

---

### Week 25-26: Final Polish & Release (PLAYTEST 6)

**Gameplay Systems Built** (implementation-plan.md:815-871):

- Final bug fixes (Week 25)
- Balance tuning (Week 25)
- AI polish (Week 25)
- Data validation (Week 25)
- Release build (Week 26)

**Debug Tools to Implement** (Weeks 25-26):

---

### Week 25: Final Integration

**Gameplay Systems Built** (implementation-plan.md:834-849):

- Character final balance (Dev A)
- Combat final polish (Dev B)
- AI final tuning (Dev C)
- Data validation (Dev D)
- **BattleState complete integration - Dev E**

**Debug Tools to Implement**:

- [ ] **Balance Tuning Tools** (Week 25)
  
  - `comparecharacters <char1> <char2>` - Side-by-side stat comparison
  - `testbalance <iterations>` - Simulate N battles, show win rate
  - `adjustdifficulty <level>` - Test difficulty scaling (easy/normal/hard)

- [ ] **Data Validation** (Week 25)
  
  - `validatedata` - Check all JSON files for errors
  - `validatecharacters` - Check character stat consistency
  - `validatespells` - Check spell balance
  - Reports issues: "Warning: Wizard HP too low (55 < recommended 60)"

**Integration Test Support** (implementation-plan.md:844-849):

```
# Test character balance comparison
> comparecharacters dragon fighter
Expected:
  === Character Comparison ===
  Stat          Dragon    Fighter
  HP            140       90
  Speed         2         3
  Attack        +5        +3
  Defense       8         10
  Spell Slots   [3,3,2]   None
  Abilities     9         3

# Test balance simulation
> testbalance 100
Expected: "Running 100 simulated battles..."
Expected: Result:
  Dragon wins: 45 (45%)
  Enemies win: 55 (55%)
  Average turns: 12
  Recommendation: Increase Dragon HP to 160 for 50% win rate

# Test difficulty scaling
> adjustdifficulty easy
Expected: "Difficulty: Easy (enemies -20% stats)"
> spawnall
Expected: Fighter HP: 72 (90 * 0.8)

> adjustdifficulty hard
Expected: "Difficulty: Hard (enemies +30% stats)"
> spawnall
Expected: Fighter HP: 117 (90 * 1.3)

# Validate all data files
> validatedata
Expected:
  ✓ characters.json - Valid
  ✓ spells.json - Valid
  ✗ maps.json - ERROR: map3 references invalid tileset
  ⚠ ai_behaviors.json - WARNING: Rogue behavior tree has unreachable node

> validatecharacters
Expected:
  ✓ Dragon - Balanced
  ⚠ Wizard - HP too low (55 < 60 recommended)
  ✓ Fighter - Balanced
```

---

### Week 26: Final Polish + PLAYTEST 6 (FINAL RELEASE)

**Gameplay Systems Built** (implementation-plan.md:852-871):

- Bug fixes from all playtests (Dev A-C)
- Balance final tuning (Dev A-C)
- Data final polish (Dev D)
- **Release build - Dev E**

**Debug Tools to Implement**:

- [ ] **Release Build Tools** (Week 26)
  
  - `buildreleasemode` - Create final executable (debug tools disabled)
  - `packageassets` - Package all assets for distribution
  - `generatereleasenotes` - Create changelog from git commits

- [ ] **Final Testing Commands** (Week 26)
  
  - `runtestsfull` - Execute full test suite (all integration tests)
  - `checkcompleteness` - Verify all features implemented
  - `finalcheck` - Pre-release validation checklist

**Playtest 6 Debug Checklist** (FINAL - implementation-plan.md:862-879):

For PLAYTEST 6 (Friday Week 26 - FINAL RELEASE), testers should use these commands:

```
# Test complete campaign (start to finish)
> showcampaign
Expected: Campaign starts at Battle 1

> # Play all 3 battles consecutively
Expected: No crashes, smooth progression

> showcampaign
Expected:
  Battle 1: ✓ Complete (Score: 1800)
  Battle 2: ✓ Complete (Score: 2100)
  Battle 3: ✓ Complete (Score: 2450)
  Total Score: 6350

# Test save/load reliability
> save final_playtest
> # Close and reopen game
> load final_playtest
Expected: All progress restored perfectly

# Test difficulty balance
> adjustdifficulty easy
> # Play Battle 1
Expected: Dragon wins easily

> adjustdifficulty hard
> # Play Battle 1
Expected: Battle challenging but fair

# Run final validation
> validatedata
Expected: All data files ✓ Valid

> checkcompleteness
Expected:
  ✓ 5 Characters implemented
  ✓ 20+ Abilities implemented
  ✓ Turn-based combat working
  ✓ AI for all 4 enemies
  ✓ Campaign mode (3 battles)
  ✓ Save/Load system
  ✓ Visual polish complete
  ✓ Performance optimized

> finalcheck
Expected:
  ✓ All playtests passed
  ✓ All critical bugs fixed
  ✓ Balance tuned
  ✓ Assets finalized
  ✓ Ready for release 🎉

# Test release build
> buildreleasemode
Expected: "Building release executable..."
Expected: "DragonicTactics.exe created (debug tools disabled)"

# Verify debug tools disabled in release
> ~ (tilde key)
Expected: Console does NOT open (debug mode disabled)

> F1, F4, F7, F9, F12
Expected: No debug overlays (all disabled in release build)

# Final confirmation
> packageassets
Expected: "Assets packaged successfully"

> generatereleasenotes
Expected: Changelog generated from commits

**FINAL STATUS**: Dragonic Tactics 1.0 Complete! 🎉
```

---

## Summary: Complete Debug Tools Timeline (Weeks 1-26)

### Week 1-5: Foundation + PLAYTEST 1

- Week 1: DebugConsole, EventBusTracer, DiceManager commands
- Week 2: GridDebugRenderer (F1), spawn/teleport commands
- Week 3: CombatFormulaInspector (F12), castspell command
- Week 4: HotReloadManager, turn management commands
- Week 5: God Mode, Playtest 1 Debug Checklist

### Week 6-10: Spell Expansion + PLAYTEST 2

- Week 6: CombatFormulaInspector polish, status effect tracking
- Week 7: StatusInfoOverlay (F7), clearstatus/applystatus commands
- Week 8: AIDebugVisualizer (F4) basics, AI control commands
- Week 9: HotReloadManager polish, AI decision logging
- Week 10: Playtest 2 Debug Checklist, performance monitoring

### Week 11-15: All Characters + PLAYTEST 3

- Week 11: Spell slot inspector, healing verification
- Week 12: TargetingSystem debugger, DiceHistoryPanel enhancement
- Week 13: Movement range visualization, AI memory inspector
- Week 14: Multi-character commands (spawnall, listcharacters)
- Week 15: AI behavior inspector, Playtest 3 Debug Checklist

### Week 16-20: Advanced AI + PLAYTEST 4

- Week 16: Threat value display, team coordination visualizer
- Week 17: Behavior tree visualizer, EventBusTracer filtering
- Week 18: Status effect badges, multi-effect support
- Week 19: Spell range visualizer, spell validation debugger
- Week 20: Battle state inspector, Playtest 4 Debug Checklist

### Week 21-24: Visual Polish + PLAYTEST 5

- Week 21: Animation debugger, effect spawner
- Week 22: Save system debugger, campaign progression inspector
- Week 23: Camera effects debugger, UI inspector
- Week 24: Performance profiler, Playtest 5 Debug Checklist

### Week 25-26: Final Polish + PLAYTEST 6 (RELEASE)

- Week 25: Balance tuning tools, data validation
- Week 26: Release build tools, final testing, PLAYTEST 6

**Total Debug Commands**: 50+ commands
**Total Visual Tools**: 11 overlays (F1, F4, F7, F9, F10, F11, F12, Shift+F1, Shift+F12, Ctrl+E, Ctrl+G)
**Total Integration Tests**: 150+ test cases across 26 weeks

---

## Configuration File

**`Assets/Data/debug_config.json`**:

```json
{
  "debugMode": true,
  "hotReload": {
    "enabled": true,
    "autoReload": true,
    "watchedDirectories": ["Assets/Data"]
  },
  "visual": {
    "showGrid": true,
    "showCollision": false,
    "showAIPaths": true,
    "showStatusInfo": true
  },
  "godMode": {
    "enabled": false,
    "invincibility": false,
    "infiniteActionPoints": false,
    "infiniteSpellSlots": false
  },
  "console": {
    "enabled": true,
    "historySize": 50,
    "fontSize": 14
  },
  "keyboard": {
    "toggleConsole": "Tilde",
    "toggleGodMode": "Ctrl+G",
    "toggleDebugUI": "F12",
    "reloadData": "Ctrl+R"
  }
}
```

---

## Summary

### Key Benefits of This Debug System:

✅ **Rapid Iteration** - Modify JSON and see changes immediately
✅ **Testing Efficiency** - God mode skips tedious combat for testing late-game features
✅ **Bug Reproduction** - Visual debug tools make issues visible
✅ **AI Development** - Path visualization shows AI decision-making
✅ **Balance Tuning** - Console commands enable quick stat adjustments
✅ **Quality Assurance** - Recording system captures bugs for fixing

### Implementation Priority:

1. **Week 0.5** (before Phase 1): Core debug infrastructure
2. **Weeks 1-6**: Grid visualization + basic commands
3. **Weeks 7-12**: Combat debug + status overlay
4. **Weeks 13-20**: AI visualization + advanced features
5. **Weeks 21-26**: Polish + recording system

This comprehensive debug system will make development faster, debugging easier, and testing more thorough throughout the entire 26-week development cycle!
