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

**Purpose**: Text-based command input for rapid testing and debugging

**Features**:

- Tilde (~) key opens console overlay
- Command history (up/down arrows)
- Tab completion for commands
- Color-coded output (errors in red, success in green, info in white)
- Command help system

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

Debug tools should be implemented **alongside** gameplay features, not after. Each phase includes corresponding debug tools:

### Phase 0.5: Core Debug Infrastructure (Week 0.5)

**Implement BEFORE Phase 1 begins**

- [ ] **DebugManager Singleton** - Master control for all debug features
- [ ] **DebugConsole** - Basic console with command registration
- [ ] **HotReloadManager** - File watching and JSON reload
- [ ] **Basic Debug UI Overlay** - FPS counter and simple text display
- [ ] **Keyboard shortcut system** - F-keys and Ctrl combinations

**Why first?** Debug tools accelerate all future development. Better to have them from day 1.

### Phase 1 Debug Tools (Weeks 1-6)

**Implemented alongside Foundation Layer**

- [ ] **GridDebugRenderer** (Week 4-5) - Implement with GridSystem
- [ ] **Data reload commands** (Week 1) - Implement with DataRegistry
- [ ] **Character spawn commands** (Week 3-4) - Implement with CharacterFactory
- [ ] **God Mode basic features** (Week 6) - Invincibility and infinite resources

**Console Commands Added:**

- `reload`, `spawn`, `kill`, `teleport`, `god`, `debug grid`

### Phase 2 Debug Tools (Weeks 7-12)

**Implemented alongside Core Gameplay Systems**

- [ ] **Turn manipulation commands** (Week 7-8) - Implement with TurnManager
- [ ] **Combat debug commands** (Week 8-9) - Damage, healing, stat modification
- [ ] **CollisionDebugRenderer** (Week 8-9) - Implement with CombatSystem
- [ ] **Movement range visualization** (Week 9-10) - Implement with MovementSystem
- [ ] **StatusInfoOverlay** (Week 10) - HP bars, action points, spell slots

**Console Commands Added:**

- `endturn`, `nextturn`, `damage`, `heal`, `setstat`, `giveap`, `givespell`

### Phase 3 Debug Tools (Weeks 13-20)

**Implemented alongside Advanced Tactical Features**

- [ ] **AIDebugVisualizer** (Week 13-15) - Implement with complete AI system
- [ ] **AI control commands** (Week 13-15) - Enable/disable, force states
- [ ] **Spell visualization** (Week 18-19) - AOE circles, targeting lines
- [ ] **Status effect badges** (Week 16-17) - Visual indicators for buffs/debuffs
- [ ] **Threat value display** (Week 19-20) - AI memory visualization

**Console Commands Added:**

- `disableai`, `enableai`, `aisetstate`, `clearstatus`, `debug aipath`

### Phase 4 Debug Tools (Weeks 21-26)

**Polish and advanced debugging**

- [ ] **Recording/Replay System** (Week 22) - Record battles for bug reproduction
- [ ] **Performance profiler** (Week 23) - Frame time breakdown
- [ ] **Save/Load debugging** (Week 21-22) - Inspect save file contents
- [ ] **Advanced god mode features** (Week 24) - Instant kill, always critical

**Console Commands Added:**

- `startrecording`, `stoprecording`, `playreplay`, `screenshot`

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
