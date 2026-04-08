# Technical Guide - Dragonic Tactics

**Team Name:** Code Pistols
**Game Name:** Dragonic Tactics
**Milestone:** 4
**Last Updated:** 2025-12-10

---

## Question 2: Major Technologies (5 points)

### Core Technologies

**1. OpenGL 3.3+**
- Modern graphics API using programmable shader pipeline
- Cross-platform rendering (Windows native + WebAssembly via Emscripten)
- Located in: `source/OpenGL/` directory

**2. SDL2 (v2.28.5+)**
- Window management and input handling
- Cross-platform support for Windows and Web
- Event system integration
- Located in: `cmake/dependencies/SDL2.cmake`

**3. CMake 3.21+**
- Build system for C++20 projects
- FetchContent for automatic dependency management
- Multi-platform presets (Windows Debug/Release, WebAssembly)
- Configuration: `CMakeLists.txt`, `cmake/` directory

**4. C++20**
- Modern C++ standard with concepts, ranges, and improved compile-time features
- Compiler: MSVC Platform Toolset v143 (Visual Studio 2022)
- Standard enforced via `CMAKE_CXX_STANDARD 20`

**5. Dear ImGui (Docking Branch)**
- Immediate mode GUI for debug tools
- Real-time game state inspection
- In-game debug panels and console
- Located in: `cmake/dependencies/DearImGUI.cmake`

**6. Emscripten (Web Platform)**
- C++ to WebAssembly compiler
- Enables browser-based gameplay
- OpenGL ES translation layer
- Web presets: `web-debug`, `web-developer-release`, `web-release`

---

## Question 3: 3rd-Party Libraries and Assets (5 points)

### Libraries

**Auto-fetched via CMake FetchContent:**

| Library | Version | Purpose | License | Source |
|---------|---------|---------|---------|--------|
| OpenGL | 3.3+ | Graphics rendering API | Industry Std | Khronos Group |
| GLEW | Latest | OpenGL Extension Wrangler | Modified BSD | http://glew.sourceforge.net/ |
| SDL2 | 2.28.5+ | Window/Input/Platform | zlib License | https://www.libsdl.org/ |
| Dear ImGui | Docking | Debug UI framework | MIT License | https://github.com/ocornut/imgui |
| GSL | 4.0.0 | Guidelines Support Library | MIT License | https://github.com/microsoft/GSL |
| STB | Latest | Image loading (stb_image.h) | MIT/Public | https://github.com/nothings/stb |

**Directly included (header-only):**

| Library | Version | Purpose | License | Location |
|---------|---------|---------|---------|----------|
| nlohmann/json | Latest | JSON parsing for game data | MIT License | `source/Game/DragonicTactics/External/json.hpp` |

**File Location:** FetchContent dependencies are defined in `cmake/Dependencies.cmake` and individual files in `cmake/dependencies/` directory. nlohmann/json is directly included as a single header file in the External folder.

**Copyright Compliance:** All libraries used are permissively licensed (MIT, BSD, zlib) and allow commercial use. Attribution is provided in project documentation.

### Assets

All game assets (sprites, animations) are created in-house by the team. No 3rd-party art assets are currently used.

**Asset Pipeline:**
- PNG format for sprites
- Custom .SPT (sprite sheet) format
- Custom .ANM (animation) format
- Located in: `Assets/` directory

---

## Question 4: Style Conventions (10 points)

### Code Formatting

**Formatter:** clang-format version 14
**Configuration File:** `DragonicTactics/.clang-format`

#### Key Formatting Rules

**Full configuration from `DragonicTactics/.clang-format`:**

```yaml
# clang-format version 14
Language: Cpp
Standard: Latest  # C++20

# Indentation
AccessModifierOffset: -4
IndentWidth: 2
UseTab: true
TabWidth: 4
ContinuationIndentWidth: 4
IndentCaseLabels: true
IndentCaseBlocks: true
IndentPPDirectives: AfterHash
IndentWrappedFunctionNames: true

# Line Length
ColumnLimit: 200

# Braces
BreakBeforeBraces: Allman  # Opening brace on new line
BreakBeforeTernaryOperators: true

# Spacing
SpaceBeforeParens: ControlStatements
SpaceAfterCStyleCast: false
SpaceAfterLogicalNot: false
SpaceAfterTemplateKeyword: true
SpaceBeforeAssignmentOperators: true
SpaceBeforeCpp11BracedList: false
SpaceInEmptyBlock: true

# Alignment
AlignAfterOpenBracket: AlwaysBreak
AlignArrayOfStructures: Right
AlignConsecutiveAssignments: AcrossComments
AlignConsecutiveBitFields: AcrossEmptyLinesAndComments
AlignConsecutiveDeclarations: AcrossComments
AlignConsecutiveMacros: AcrossComments
AlignTrailingComments: true
AlignOperands: Align

# Pointer/Reference
PointerAlignment: Left
ReferenceAlignment: Left

# Namespaces
CompactNamespaces: true
NamespaceIndentation: All
FixNamespaceComments: false

# Functions
AllowShortFunctionsOnASingleLine: None
AllowShortBlocksOnASingleLine: Never
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: false
AllowShortEnumsOnASingleLine: true
AllowShortCaseLabelsOnASingleLine: true
AllowShortLambdasOnASingleLine: Inline
AlwaysBreakTemplateDeclarations: Yes
BreakBeforeConceptDeclarations: true

# Other
SeparateDefinitionBlocks: Always
SortIncludes: CaseSensitive
SortUsingDeclarations: true
BitFieldColonSpacing: Both
Cpp11BracedListStyle: false
MaxEmptyLinesToKeep: 2
```

**Most Important Rules:**
- **IndentWidth: 2** - Use 2 spaces for indentation level
- **UseTab: true** - Use tabs (TabWidth: 4)
- **ColumnLimit: 200** - Very wide line limit
- **BreakBeforeBraces: Allman** - Braces on separate lines
- **PointerAlignment: Left** - `int* ptr` not `int *ptr`
- **AllowShortFunctionsOnASingleLine: None** - Always expand functions

### File Naming Conventions

| Type | Convention | Example |
|------|-----------|---------|
| Header Files | `.h` extension | `Character.h`, `EventBus.h` |
| Source Files | `.cpp` extension | `Character.cpp`, `EventBus.cpp` |
| Precompiled Header | `pch.h` (first include) | `#include "pch.h"` |
| Directories | PascalCase | `StateComponents/`, `Objects/` |
| Assets | lowercase_underscores | `dragon_idle.png`, `characters.json` |

### Naming Conventions

**Classes:** PascalCase
```cpp
class CharacterFactory { };
```

**Member variables:** snake_case with trailing underscore
```cpp
class Example {
private:
    int member_variable_;
    std::string some_data_;
};
```

**Functions:** PascalCase
```cpp
void LoadCharacterData();
bool IsAlive() const;
```

**Constants:** UPPER_SNAKE_CASE
```cpp
constexpr int TILE_SIZE = 100;
constexpr int MAP_WIDTH = 8;
```

**Namespaces:** PascalCase
```cpp
namespace CS230 { }
namespace Math { }
```

**Enums:** PascalCase (both type and values)
```cpp
enum class CharacterTypes {
    Dragon,
    Fighter,
    Wizard
};
```

### Documentation

**In-Code Comments:**
```cpp
// Single-line comments for brief explanations
/* Multi-line comments for detailed explanations */

/**
 * @file FileName.cpp
 * @author Author Name
 * @brief Brief description
 * @date YYYY-MM-DD
 */
```

**External Documentation:**
- Architecture: `docs/architecture.md`
- Implementation plans: `docs/implementation-plan.md`
- Weekly guides: `docs/Detailed Implementations/weeks/`
- Korean language for detailed design documents
- English for code comments and technical summaries

### Precompiled Headers

**Mandatory Rule:** Every `.cpp` file MUST start with:
```cpp
#include "pch.h"
```

**Benefits:**
- 35% faster build times
- Centralizes common includes
- Located in: `source/pch.h`

### Directory Structure Rules

```
DragonicTactics/source/
├── Engine/           # Engine core (CS230 namespace)
├── CS200/            # Rendering abstraction
├── OpenGL/           # OpenGL wrappers
└── Game/DragonicTactics/
    ├── StateComponents/  # GameState components (singletons)
    ├── Objects/          # GameObjects (entities)
    ├── States/           # GameStates (scenes)
    ├── Factories/        # Factory patterns
    ├── Types/            # Shared type definitions
    └── Debugger/         # Debug tools
```

---

## Question 5: Debugging Support (10 points)

### Overview

Our debugging system is comprehensive and integrated into gameplay via the **DebugManager** (`Debugger/DebugManager.h/cpp`), which coordinates three major subsystems.

**Activation:** Press **Tab** key during gameplay to toggle debug tools.

### 1. In-Game Debug Console [IMPLEMENTED]

**Type:** Input-capable console with command history
**Implementation:** `Debugger/DebugConsole.h/cpp`

**Features:**
- Command Execution: Type commands and press Enter
- Command History: Up/Down arrow keys to navigate previous commands
- Auto-completion: Built-in help system
- Syntax Coloring:
  - Green: Executed commands
  - Red: Errors
  - Yellow: Warnings

**Built-in Commands:**
```
help          // List all available commands
help <cmd>    // Show help for specific command
clear         // Clear console output
echo <text>   // Echo text to console (testing)
```

**How to Add Custom Commands:**
```cpp
// In GamePlay.cpp or system initialization
auto* console = GetGSComponent<DebugManager>()->GetConsole();

console->RegisterCommand("spawn",
    [](std::vector<std::string> args) {
        // Command implementation
    },
    "Spawn a character at position"
);
```

**Usage in GamePlay:** Lines 69, 102-105 in `GamePlay.cpp`

### 2. Debug Drawing Systems [IMPLEMENTED]

**Implementation:** `Debugger/DebugVisualizer.h/cpp`

**Grid Overlay System:**
```cpp
// Lines 89-120 in DebugVisualizer.cpp
void DrawGridOverlay(const GridSystem* grid)
{
    // Draws semi-transparent white grid lines
    // Vertical lines: x = 0 to 8
    // Horizontal lines: y = 0 to 8
    // Color: 0xFFFFFF44 (semi-transparent white)
}
```

**Activation:**
- Automatic when Debug Mode is ON
- Controlled via `DebugManager::IsGridOverlayEnabled()`
- Toggle: `DebugManager::ToggleGridOverlay()`

**Visual Elements:**
- Grid Lines: 8x8 tactical grid visualization
- Character Positions: Real-time position tracking
- Damage Numbers: Floating damage text with lifetime (2 seconds)
- Combat Effects: Visual feedback for attacks

**Drawing Pipeline:** `GamePlay::Draw()` line 288
```cpp
GetGSComponent<DebugManager>()->Draw(grid_system);
```

### 3. Assertion System [IMPLEMENTED]

**Type:** Function-based assertion with boolean return values
**File:** `Test/TestAssert.h/cpp`

**Implementation:**
```cpp
// TestAssert.h - Function declarations
bool ASSERT_TRUE(bool condition);
bool ASSERT_FALSE(bool condition);

template <typename T>
bool ASSERT_EQ(T actual, T expected);

template <typename T>
bool ASSERT_NE(T actual, T expected);

template <typename T>
bool ASSERT_GE(T actual, T expected);

template <typename T>
bool ASSERT_LE(T actual, T expected);

// TestAssert.cpp - Function implementations
bool ASSERT_TRUE(bool condition)
{
  if (!(condition))
  {
    std::cout << " ASSERT_TRUE failed " << std::endl;
    return false;
  }
  else
  {
    std::cout << " ASSERT_TRUE successed! " << std::endl;
    return true;
  }
}

bool ASSERT_FALSE(bool condition)
{
  if ((condition))
  {
    std::cout << " ASSERT_FALSE failed" << std::endl;
    return false;
  }
  else
  {
    std::cout << " ASSERT_FALSE successed! " << std::endl;
    return true;
  }
}

// TestAssert.ini - Template implementations
template <typename T>
bool ASSERT_EQ(T actual, T expected)
{
    if ((actual) != (expected))
    {
        std::cout << " ASSERT_EQ failed" << std::endl;
        return false;
    }
    else
    {
        std::cout << " ASSERT_EQ successed! " << std::endl;
        return true;
    }
}
```

**Usage Examples:**
```cpp
// In test functions - returns bool for pass/fail
bool result = ASSERT_TRUE(player != nullptr);
if (!result) {
    // Test failed, but program continues
}

// Testing equality
ASSERT_EQ(player->GetHP(), 100);

// Testing inequality
ASSERT_NE(damage, 0);

// Testing greater/less than
ASSERT_GE(speed, 0);
ASSERT_LE(action_points, max_action_points);
```

**Features:**
- Returns bool (true = pass, false = fail)
- Continues execution on failure (doesn't abort)
- Outputs test results to std::cout
- Template support for type-generic comparisons

### 4. In-Game Performance Visualizer [NOT IMPLEMENTED]

**Status:** Planned for future milestone

**Planned Features:**
- FPS counter
- Frame time graph
- Draw call counter
- Memory usage

**Current Workaround:** ImGui debug panels show some performance metrics manually.

### 5. Variable Watching (In-Game) [IMPLEMENTED]

**Type:** Real-time variable inspection via ImGui panels
**Implementation:** `Debugger/DebugVisualizer.cpp`

**Available Panels:**

**a) Character Stats Panel** (Lines 247-308)
```cpp
void DrawImGuiCharacterStats(const GridSystem* grid)
{
    // For each character on grid:
    // - Position (x, y)
    // - Alive/Dead status
    // - HP: current / max
    // - Attack, Speed, Range
    // - Attack Dice notation
    // - Action Points: current / max
}
```

**b) Event Log Panel** (Lines 177-212)
```cpp
void DrawImGuiEventLog()
{
    // Last 10 seconds of events:
    // - Damage events (red)
    // - Death events (dark red)
    // - Spell events (blue)
    // - Turn events (green)
}
```

**c) Dice History Panel** (Lines 214-244)
```cpp
void DrawImGuiDiceHistory()
{
    // Last 20 dice rolls:
    // - Individual die results
    // - Total sum
    // - Timestamp
}
```

**d) Combat Log Panel** (Lines 310-344)
```cpp
void DrawImGuiCombatLog()
{
    // Filtered combat events:
    // - Damage dealt
    // - Character deaths
    // - Active damage numbers with lifetime
}
```

**e) AI Decisions Panel** (Lines 454-503)
```cpp
void DrawImGuiAIDecisions()
{
    // AI decision history table:
    // - Timestamp
    // - Actor name (blue)
    // - Decision type (color-coded)
    // - Target (if applicable)
    // - Reasoning (why this decision?)
}
```

**Activation:** Toggle via Debug Tools panel (Tab key)

**Implementation in GamePlay:** Line 296
```cpp
GetGSComponent<DebugManager>()->DrawImGui(grid_system);
```

### Debug Manager Control Panel

**Main Interface:** `DebugManager::DrawDebugControlPanel()` (Lines 108-179)

```
+-------------------------+
| DEBUG MODE [ON/OFF]     |  <- Master toggle (green when ON)
+-------------------------+
| Panels:                 |
| [X] Events & Stats      |  <- Multi-tab debug panel
| [X] Console (Tab)       |  <- Command console
+-------------------------+
| Cheats:                 |
| [X] God Mode            |  <- Invincibility (red text when ON)
+-------------------------+
| Tab: Toggle This Panel  |
| ESC: Close Panel        |
+-------------------------+
```

---

## Question 6: Additional Debug Features (+5 points)

### 1. Event Tracing System [IMPLEMENTED]

**File:** `Debugger/DebugVisualizer.cpp`

**Subscribed Events:**
```cpp
// Lines 24-33 in DebugVisualizer::Init()
EventBus::Subscribe<CharacterDamagedEvent>(OnCharacterDamaged);
EventBus::Subscribe<CharacterDeathEvent>(OnCharacterDeath);
EventBus::Subscribe<AIDecisionEvent>(OnAIDecision);
EventBus::Subscribe<TurnStartedEvent>(OnTurnStarted);
```

**Event Handlers:**
- **OnCharacterDamaged** (Lines 348-378): Tracks damage events, creates floating damage numbers
- **OnCharacterDeath** (Lines 380-399): Logs character deaths with killer information
- **OnTurnStarted** (Lines 401-419): Logs turn start with character name and action points
- **OnAIDecision** (Lines 421-452): Records AI decision reasoning for debugging

**Event Log Features:**
- 10-second sliding window
- Color-coded by event type
- Timestamp display
- Detailed reasoning for AI decisions

### 2. Hot-Reload System for JSON Data [IMPLEMENTED]

**File:** `StateComponents/DataRegistry.cpp`

**Implementation:**
```cpp
// Lines 80-120: DataRegistry::ReloadAll()
void ReloadAll()
{
    // Check file modification timestamp
    long long currentModTime = GetFileModifiedTime(charactersFile);
    long long lastModTime = fileTimestamps[charactersFile];

    // Reload if modified
    if (currentModTime > lastModTime) {
        LoadAllCharacterData(charactersFile);
        fileTimestamps[charactersFile] = currentModTime;
    }
}
```

**Features:**
- File timestamp tracking
- Automatic change detection
- Graceful fallback on error
- Supports character and map data
- Logging of reload status

**Usage in GamePlay:**
- **R** key: Reload JSON data (planned keybind)
- **L** key: Log all loaded data (planned keybind)

**Files Tracked:**
- `Assets/Data/characters.json` - Character stats and abilities
- `Assets/Data/maps.json` - Map layouts and spawn points

### 3. Function Call Tracing [IMPLEMENTED]

**Implementation:** Using `__PRETTY_FUNCTION__` macro with Logger

**Example Usage:**
```cpp
void Character::OnTurnStart() {
    Engine::GetLogger().LogEvent(
        std::string(__PRETTY_FUNCTION__) + " called"
    );
    // Turn start logic
}
```

**Benefits:**
- Full function signature in logs
- Call stack reconstruction
- Debugging turn flow and system integration
- Helps identify duplicate calls

**Logger Output Format:**
```
[EVENT] void Character::OnTurnStart() called
[EVENT] TurnManager::StartNextTurn() - Character: Dragon
```

### 4. Map Selection Debug UI [IMPLEMENTED]

**File:** `GamePlay.cpp` Lines 298-362

**Features:**
```cpp
void GamePlay::DrawImGui()
{
    ImGui::Begin("Map Selection");

    // Current map display
    ImGui::Text("Current Map: %s", current_source);

    // First map (hardcoded) button
    if (ImGui::Button("First Map")) {
        s_next_map_source = MapSource::First;
    }

    // JSON maps list
    for (auto& map_id : available_json_maps_) {
        if (ImGui::Button(map_id.c_str())) {
            s_next_map_source = MapSource::Json;
            s_next_map_index = i;
        }
    }

    // Restart button
    if (ImGui::Button("Restart with Selected Map")) {
        s_should_restart = true;
    }

    ImGui::End();
}
```

**Functionality:**
- Runtime map switching without recompilation
- Visual indication of currently selected map (green highlight)
- Support for both hardcoded and JSON-based maps
- Restart on next frame to apply changes

### 5. Combat Status Panel [IMPLEMENTED]

**File:** `GamePlay.cpp` Lines 367-377

**Real-time Display:**
```cpp
if (turnMgr && turnMgr->IsCombatActive())
{
    ImGui::Begin("Combat Status");
    Character* current = turnMgr->GetCurrentCharacter();
    if (current) {
        ImGui::Text("Current Turn: %s", current->TypeName().c_str());
        ImGui::Text("Turn #%d | Round #%d",
            turnMgr->GetCurrentTurnNumber(),
            turnMgr->GetRoundNumber());
    }
    ImGui::End();
}
```

**Information Displayed:**
- Current character's turn
- Turn number (sequential)
- Round number (full rotation count)

### 6. Player Action State Machine Debug [IMPLEMENTED]

**File:** `GamePlay.cpp` Lines 364-480

**Features:**

**State Display:**
```cpp
using ActionState = PlayerInputHandler::ActionState;
ActionState currentState = m_input_handler->GetCurrentState();

// Buttons adapt based on state:
// - "Move" / "Cancel Move"
// - "Action" / "Cancel Action"
// - "End Turn" (disabled during selection)
```

**Button States:**
- Disabled when incompatible with current state
- Label changes based on state (e.g., "Move" <-> "Cancel Move")
- Visual feedback via ImGui styling

**Action List Popup:**
```cpp
if (currentState == ActionState::SelectingAction)
{
    ImGui::Begin("Action List");
    if (ImGui::Button("Attack")) { /* ... */ }
    if (ImGui::Button("Spell")) { /* ... */ }
    ImGui::End();
}
```

### 7. Renderer Performance Metrics [IMPLEMENTED]

**File:** `CS200/IRenderer2D.h`

**Available Metrics:**
```cpp
// All renderer implementations provide:
size_t GetDrawCallCounter();    // Number of draw calls this frame
size_t GetDrawTextureCounter(); // Number of textures drawn
```

**Renderers:**
- **ImmediateRenderer2D:** High draw calls (one per quad)
- **BatchRenderer2D:** Reduced draw calls (batched by texture)
- **InstancedRenderer2D:** Minimal draw calls (GPU instancing)

**Usage:**
```cpp
auto* renderer = Engine::GetTextureManager().GetRenderer2D();
size_t draw_calls = renderer->GetDrawCallCounter();
size_t textures = renderer->GetDrawTextureCounter();

Engine::GetLogger().LogDebug("Draw Calls: " + std::to_string(draw_calls));
```

### 8. Damage Visualization System [IMPLEMENTED]

**File:** `GamePlay.cpp` Lines 146-177

**Damage Number Display:**
```cpp
void GamePlay::DisplayDamageAmount(const CharacterDamagedEvent& event)
{
    // Scale damage text based on damage percentage
    float damage_ratio = event.damageAmount / stats->GetMaxHP();
    Math::vec2 size = {1.0f, 1.0f};

    if (damage_ratio >= 0.5)      size = {2.5, 2.5};  // Critical hit
    else if (damage_ratio >= 0.33) size = {2.0, 2.0};  // Heavy damage
    else if (damage_ratio >= 0.2)  size = {1.5, 1.5};  // Medium damage
    else if (damage_ratio >= 0.1)  size = {1.2, 1.2};  // Light damage

    m_ui_manager->ShowDamageText(event.damageAmount, text_position, size);
}
```

**Damage Log Display:**
```cpp
void GamePlay::DisplayDamageLog(const CharacterDamagedEvent& event)
{
    std::string str = event.target->TypeName() + " took " +
                      std::to_string(event.damageAmount) + " damage from " +
                      event.attacker->TypeName() + "(HP: " +
                      std::to_string(event.remainingHP) + ")";

    m_ui_manager->ShowDamageLog(str, position, size);
}
```

**Features:**
- Size-scaled damage numbers based on severity
- Floating text with lifetime
- Persistent damage log
- HP tracking after damage

---

## Question 7: Enabling/Disabling Debugging (10 points)

### Master Debug Toggle System

**Implementation:** `Debugger/DebugManager.h/cpp`

### 1. Runtime Toggle (In-Game)

**Keyboard Shortcut:**
```cpp
// DebugManager.cpp Lines 58-66
void DebugManager::ProcessInput()
{
    CS230::Input& input = Engine::Instance().GetInput();

    if (input.KeyJustPressed(CS230::Input::Keys::Tab))
    {
        ToggleDebugTools();  // Opens/closes Debug Tools panel
    }
}
```

**Control Flow:**
```
Press Tab -> ToggleDebugTools() -> show_debug_tools_ flag
                                 -> Debug Tools panel appears
                                 -> User clicks "DEBUG MODE" button
                                 -> ToggleDebugMode() -> debug_mode flag
                                 -> All debug features activated
```

**Master Flag:**
```cpp
// DebugManager.h
private:
    bool debug_mode = false;  // Master switch for all debug features
    bool show_debug_tools_ = false;  // Controls panel visibility
```

### 2. Debug Feature Flags

**Individual Feature Control:**
```cpp
// DebugManager.h
private:
    bool grid_overlay = true;
    bool collision_boxes = false;
    bool ai_path_visualization = false;
    bool dice_history = true;
    bool combat_inspector = true;
    bool event_tracer = true;
    bool status_info = true;
    bool god_mode = false;
```

**Toggle Functions:**
```cpp
void ToggleGridOverlay();           // Lines 231-235
void ToggleCollisionBoxes();        // Lines 237-241
void ToggleAIPathVisualization();   // Lines 243-247
void ToggleDiceHistory();           // Lines 249-253
void ToggleCombatInspector();       // Lines 255-259
void ToggleEventTracer();           // Lines 261-265
void ToggleStatusInfo();            // Lines 267-271
void ToggleGodMode();               // Lines 273-277
```

**Enable Check (Requires Both Master + Feature Flag):**
```cpp
// Lines 279-317
bool IsGridOverlayEnabled() const {
    return debug_mode && grid_overlay;  // Both must be true
}

bool IsCollisionBoxesEnabled() const {
    return debug_mode && collision_boxes;
}

bool IsGodModeEnabled() const {
    return debug_mode && god_mode;
}
```

### 3. Build Configuration Flags

**CMake Presets:**

**Debug Build (Debug features ENABLED)**
```bash
cmake --preset windows-debug
```
- Console output: ON
- Assertions: ON
- Optimization: O0
- Debug symbols: Full

**Developer-Release Build (Debug features ENABLED + Optimized)**
```bash
cmake --preset windows-developer-release
```
- Console output: ON
- Assertions: ON
- Optimization: O2
- Debug symbols: Full

**Release Build (Debug features DISABLED)**
```bash
cmake --preset windows-release
```
- Console output: OFF
- Assertions: OFF
- Optimization: O3
- Debug symbols: None

**Preprocessor Definitions:**
```cpp
#ifdef NDEBUG
    // Release mode - disable assertions
    #define ASSERT(condition, message) ((void)0)
#else
    // Debug mode - enable assertions
    #define ASSERT(condition, message) \
        if (!(condition)) { \
            Engine::GetLogger().LogError("ASSERTION: " + std::string(message)); \
            std::abort(); \
        }
#endif
```

### 4. Conditional Drawing

**Grid Overlay:**
```cpp
// DebugVisualizer.cpp Lines 89-95
void DebugVisualizer::DrawGridOverlay(const GridSystem* grid)
{
    auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();

    if (!enabled_ || !debug_mgr || !debug_mgr->IsGridOverlayEnabled() || grid == nullptr)
    {
        return;  // Skip drawing if debug disabled
    }

    // Draw grid lines...
}
```

**ImGui Panels:**
```cpp
// DebugManager.cpp Lines 82-106
void DebugManager::DrawImGui(const GridSystem* grid)
{
    if (!initialized_)
    {
        return;  // Skip if not initialized
    }

    if (show_debug_tools_)
    {
        DrawDebugControlPanel();  // Only draw if tools panel is open
    }

    if (visualizer_)
    {
        visualizer_->DrawImGuiDebugPanel(grid);  // Controlled by panel flags
    }

    if (console_)
    {
        console_->DrawImGui();  // Controlled by console open flag
    }
}
```

### 5. Event Logging Control

**EventBus Logging:**
```cpp
// EventBus.h/cpp
void SetLogging(bool enabled) {
    log_events_ = enabled;
}

template<typename EventType>
void Publish(const EventType& event)
{
    if (log_events_) {
        Engine::GetLogger().LogEvent("Event Published: " + GetEventTypeName<EventType>());
    }

    // Dispatch event...
}
```

**Usage:**
```cpp
// Enable verbose event logging
EventBus::Instance().SetLogging(true);

// Disable to reduce console spam
EventBus::Instance().SetLogging(false);
```

### 6. Debug Mode Benefits

**When Debug Mode is ON:**
- Grid overlay visualization
- ImGui debug panels accessible
- Event tracing to console
- AI decision reasoning logged
- Dice roll history tracked
- Combat inspector active
- God mode available (cheat)

**When Debug Mode is OFF:**
- Clean gameplay view
- No performance overhead from debug drawing
- No ImGui rendering
- Minimal console logging
- Release-like experience in Debug builds

### 7. Complete Toggle Flow Example

**User Workflow:**

1. Launch game → Debug Mode OFF by default
2. Press Tab → Debug Tools panel appears
3. Click "DEBUG MODE" button → Turns green
   - `debug_mode = true`
   - All debug features now available
4. Click "Events & Stats" checkbox
   - `show_debug_panel_ = true`
   - Multi-tab panel appears (Event Log, Dice, Characters, Combat)
5. Click "Console (Tab)" checkbox
   - Console window appears
   - Can type commands
6. Press Tab again → Debug Tools panel closes
   - But `debug_mode` still ON
   - Features remain active
7. Press Tab → Reopen panel
   - Click "DEBUG MODE" again → Turns gray
   - `debug_mode = false`
   - All features disabled

### Summary Table

| Debug Feature | Master Switch | Individual Flag | Build Flag | Runtime Toggle |
|---------------|---------------|-----------------|------------|----------------|
| Grid Overlay | debug_mode | grid_overlay | No | Yes (Tab→Panel) |
| ImGui Panels | show_debug_tools_ | Per-panel flags | No | Yes (Tab key) |
| Console | console_->open_ | No | No | Yes (Checkbox) |
| Assertions | No | No | NDEBUG | No (Compile-time) |
| Event Logging | No | log_events_ | No | Yes (SetLogging()) |
| God Mode | debug_mode | god_mode | No | Yes (Checkbox) |

---

## Question 8: Graphics API (10 points)

### Graphics Architecture Overview

**API:** OpenGL 3.3+ with Programmable Shader Pipeline
**Approach:** Shader-based rendering (NOT fixed-function pipeline)

### 1. Shader Programs

**Implementation:** `OpenGL/Shader.h/cpp`

**Shader Structure:**
```cpp
// OpenGL/Shader.h
namespace OpenGL
{
    using ShaderHandle = Handle;

    struct CompiledShader
    {
        ShaderHandle Shader;  // OpenGL program handle
        std::unordered_map<std::string, GLint> UniformLocations;  // Cached locations
    };

    // Create from files
    CompiledShader CreateShader(
        std::filesystem::path vertex_filepath,
        std::filesystem::path fragment_filepath
    );

    // Create from source strings
    CompiledShader CreateShader(
        std::string_view vertex_source,
        std::string_view fragment_source
    );

    // Cleanup
    void DestroyShader(CompiledShader& shader) noexcept;

    // Uniform buffer binding
    void BindUniformBufferToShader(
        ShaderHandle shader_handle,
        GLuint binding_number,
        Handle uniform_buffer,
        std::string_view uniform_block_name
    );
}
```

**Shader Creation Example:**
```cpp
// CS200/BatchRenderer2D.cpp - Creating shader from files
texturingCombineShader = OpenGL::CreateShader(
    "Assets/Shaders/texture.vert",
    "Assets/Shaders/texture.frag"
);

// Accessing cached uniform locations
GLint viewProjLoc = texturingCombineShader.UniformLocations["view_projection"];
```

**Typical Vertex Shader (Textured Quads):**
```glsl
#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aTintColor;
layout(location = 3) in int aTextureIndex;
layout(location = 4) in float aDepth;

uniform mat3 uViewProjection;

out vec2 vTexCoord;
out vec4 vTintColor;
flat out int vTextureIndex;

void main()
{
    vec3 clip_pos = uViewProjection * vec3(aPosition, 1.0);
    gl_Position = vec4(clip_pos.xy, aDepth, 1.0);

    vTexCoord = aTexCoord;
    vTintColor = aTintColor;
    vTextureIndex = aTextureIndex;
}
```

**Typical Fragment Shader:**
```glsl
#version 330 core

in vec2 vTexCoord;
in vec4 vTintColor;
flat in int vTextureIndex;

uniform sampler2D uTextures[16];  // Multiple texture slots

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(uTextures[vTextureIndex], vTexCoord);
    FragColor = texColor * vTintColor;
}
```

### 2. Rendering Abstraction Layer

**Interface:** `CS200/IRenderer2D.h`

**Renderer Implementations:**
```cpp
class IRenderer2D  // Pure virtual interface
{
public:
    virtual void BeginScene(const Math::TransformationMatrix& view_projection) = 0;
    virtual void EndScene() = 0;

    virtual void DrawQuad(
        const Math::TransformationMatrix& transform,
        OpenGL::TextureHandle texture,
        Math::vec2 texture_coord_bl,
        Math::vec2 texture_coord_tr,
        CS200::RGBA tintColor,
        float depth
    ) = 0;

    virtual void DrawCircle(...) = 0;
    virtual void DrawRectangle(...) = 0;
    virtual void DrawLine(...) = 0;

    virtual size_t GetDrawCallCounter() = 0;
    virtual size_t GetDrawTextureCounter() = 0;
};
```

**Three Implementations:**

| Renderer | Description | Draw Calls | Performance | File |
|----------|-------------|------------|-------------|------|
| ImmediateRenderer2D | Immediate mode - one draw per quad | High | Low | CS200/ImmediateRenderer2D.cpp |
| BatchRenderer2D | Batches quads by texture | Medium | Medium | CS200/BatchRenderer2D.cpp |
| InstancedRenderer2D | GPU instancing for identical meshes | Low | High | CS200/InstancedRenderer2D.cpp |

**Runtime Switching:**
```cpp
// TextureManager.cpp
void SwitchRenderer(RendererType type)
{
    switch (type) {
        case RendererType::Immediate:
            current_renderer_ = immediate_renderer_.get();
            break;
        case RendererType::Batch:
            current_renderer_ = batch_renderer_.get();
            break;
        case RendererType::Instanced:
            current_renderer_ = instanced_renderer_.get();
            break;
    }
}
```

### 3. OpenGL Wrapper Layer

**Purpose:** Type-safe, RAII-based OpenGL object management

**Vertex Array Object (VAO):**
```cpp
// OpenGL/VertexArray.h
class VertexArray
{
public:
    VertexArray();  // glGenVertexArrays
    ~VertexArray(); // glDeleteVertexArrays

    void Bind() const;    // glBindVertexArray
    void Unbind() const;  // glBindVertexArray(0)

    void AddVertexBuffer(const VertexBuffer& vbo);
    void SetIndexBuffer(const IndexBuffer& ebo);

private:
    GLuint vao_id_;
    std::vector<VertexBuffer> vertex_buffers_;
    IndexBuffer index_buffer_;
};
```

**Vertex Buffer Object (VBO):**
```cpp
// OpenGL/VertexBuffer.h
class VertexBuffer
{
public:
    VertexBuffer(const void* data, size_t size, GLenum usage);
    ~VertexBuffer();

    void Bind() const;    // glBindBuffer(GL_ARRAY_BUFFER, ...)
    void Unbind() const;
    void UpdateData(const void* data, size_t size, size_t offset);

private:
    GLuint vbo_id_;
};
```

**Texture2D:**
```cpp
// OpenGL/Texture2D.h
class Texture2D
{
public:
    void LoadFromFile(const std::string& filepath);  // Uses stb_image
    void Bind(GLuint slot = 0) const;  // glBindTexture

    int GetWidth() const;
    int GetHeight() const;
    GLuint GetID() const;

private:
    GLuint texture_id_;
    int width_, height_, channels_;
};
```

### 4. Rendering Pipeline Flow

**GamePlay::Draw()** - Lines 266-291:
```cpp
void GamePlay::Draw()
{
    // 1. Clear framebuffer
    Engine::GetWindow().Clear(0x1a1a1aff);  // Dark gray

    // 2. Get active renderer
    auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();

    // 3. Setup camera matrix (NDC transformation)
    Math::TransformationMatrix camera_matrix =
        CS200::build_ndc_matrix(Engine::GetWindow().GetSize());

    // 4. Begin scene (sets view-projection uniform)
    renderer_2d->BeginScene(camera_matrix);

    // 5. Draw grid background
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    if (grid_system)
        grid_system->Draw();

    // 6. Draw all GameObjects (characters, effects)
    CS230::GameObjectManager* goMgr = GetGSComponent<CS230::GameObjectManager>();
    if (goMgr)
        goMgr->DrawAll(camera_matrix);

    // 7. Draw UI elements
    m_ui_manager->Draw(camera_matrix);

    // 8. Draw debug overlays (if enabled)
    GetGSComponent<DebugManager>()->Draw(grid_system);

    // 9. End scene (flush batches, swap buffers)
    renderer_2d->EndScene();
}
```

### 5. Shader Uniform System

**Transform Hierarchy:**
```
Camera Matrix (view_projection) -> Uploaded once per BeginScene()
    |
    v
Model Matrix (per object) -> Uploaded per DrawQuad()
    |
    v
Final Position = view_projection * model * vertex_position
```

**Example - Drawing a Character:**
```cpp
void Character::Draw(Math::TransformationMatrix camera_matrix)
{
    auto* renderer = Engine::GetTextureManager().GetRenderer2D();

    // Model matrix: translation * rotation * scale
    Math::TransformationMatrix model =
        Math::TranslationMatrix(position) *
        Math::RotationMatrix(angle) *
        Math::ScaleMatrix(scale);

    // Shader will multiply: camera_matrix * model * vertex
    renderer->DrawQuad(
        model,
        texture_handle,
        {0.0f, 0.0f},  // Bottom-left UV
        {1.0f, 1.0f},  // Top-right UV
        0xFFFFFFFF,    // White tint (no color change)
        1.0f           // Depth (for sorting)
    );
}
```

### 6. Texture Management

**TextureManager:** `Engine/TextureManager.h/cpp`

**Features:**
- Centralized texture loading and caching
- Automatic texture ID generation
- Renderer management
- Coordinate system transformation

**Loading a Texture:**
```cpp
// TextureManager.cpp
CS230::Texture& LoadTexture(const std::filesystem::path& file_path)
{
    // Check cache first
    if (textures_.contains(file_path.string()))
        return textures_[file_path.string()];

    // Load new texture
    OpenGL::Texture2D gl_texture;
    gl_texture.LoadFromFile(file_path.string());  // stb_image

    CS230::Texture wrapper(gl_texture);
    textures_[file_path.string()] = wrapper;

    return textures_[file_path.string()];
}
```

**Texture Handle System:**
```cpp
using TextureHandle = GLuint;  // OpenGL texture ID

// Usage
TextureHandle sprite_texture = texture_manager.LoadTexture("Assets/dragon.png").GetHandle();
```

### 7. Coordinate Systems

**NDC Transformation:**
```cpp
// CS200/NDC.h
Math::TransformationMatrix build_ndc_matrix(Math::ivec2 window_size)
{
    // Convert from pixel coordinates to Normalized Device Coordinates
    // Pixel space: (0, 0) top-left, (width, height) bottom-right
    // NDC space: (-1, -1) bottom-left, (1, 1) top-right

    float width = static_cast<float>(window_size.x);
    float height = static_cast<float>(window_size.y);

    return Math::ScaleMatrix({2.0f / width, 2.0f / height}) *
           Math::TranslationMatrix({-width / 2.0f, -height / 2.0f});
}
```

**Camera Matrix Usage:**
```glsl
// Vertex Shader
vec3 world_pos = model * vec3(position, 1.0);
vec3 clip_pos = view_projection * world_pos;  // NDC transform
gl_Position = vec4(clip_pos.xy, 0.0, 1.0);
```

### 8. Alpha Blending

**OpenGL Blending Setup:**
```cpp
// Renderer initialization
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glBlendEquation(GL_FUNC_ADD);
```

**RGBA Color Format:**
```cpp
// CS200/Color.h
using RGBA = uint32_t;

constexpr RGBA WHITE = 0xFFFFFFFF;
constexpr RGBA BLACK = 0x000000FF;
constexpr RGBA CLEAR = 0x00000000;  // Fully transparent

// Color unpacking in shader
vec4 color;
color.r = ((tintColor >> 24) & 0xFF) / 255.0;
color.g = ((tintColor >> 16) & 0xFF) / 255.0;
color.b = ((tintColor >> 8) & 0xFF) / 255.0;
color.a = (tintColor & 0xFF) / 255.0;
```

### 9. Depth Sorting

**Painter's Algorithm (Manual Z-Ordering):**
```cpp
// Draw calls are issued in order:
// 1. Background (depth = 0.0)
grid_system->Draw();

// 2. Game objects (depth = 1.0)
goMgr->DrawAll(camera_matrix);

// 3. UI (depth = 2.0)
ui_manager->Draw(camera_matrix);

// 4. Debug overlay (depth = 3.0)
debug_manager->Draw(grid_system);
```

**Note:** 2D game uses manual draw order instead of `glDepthTest`.

### Summary

| Aspect | Implementation | File Location |
|--------|---------------|---------------|
| Shader Language | GLSL 3.3 Core | Embedded strings in .cpp |
| Shader Programs | Vertex + Fragment pair | OpenGL/ShaderProgram.cpp |
| Rendering API | IRenderer2D interface | CS200/IRenderer2D.h |
| Renderer Types | Immediate / Batch / Instanced | CS200/*Renderer2D.cpp |
| OpenGL Wrappers | VAO, VBO, EBO, Texture2D | OpenGL/*.h/cpp |
| Texture Loading | STB Image library | External/STB/stb_image.h |
| Transform Math | Custom Math library | Engine/Math.h |
| Coordinate System | NDC conversion | CS200/NDC.h |
| Blending | Alpha blending enabled | Renderer initialization |

**Answer:** We use programmable shaders (GLSL 3.3 Core), NOT fixed-function pipeline. All rendering goes through vertex and fragment shaders with custom uniform management.

---

## Question 10: Loading Game Data (10 points)

### Overview

Dragonic Tactics uses a data-driven design where game content is defined in JSON files and loaded at runtime. This allows for rapid iteration without recompilation.

**Core System:** `StateComponents/DataRegistry.h/cpp`

### 1. Character Data Loading

**File Format:** `Assets/Data/characters.json`

**JSON Structure:**
```json
{
  "Dragon": {
    "max_hp": 100,
    "speed": 6,
    "max_action_points": 2,
    "base_attack_power": 8,
    "attack_dice": "2d6",
    "base_defense_power": 5,
    "defense_dice": "1d6",
    "attack_range": 1,
    "spell_slots": {
      "1": 4,
      "2": 3,
      "3": 2
    },
    "known_spells": [
      "Fireball",
      "Magic Missile",
      "Shield"
    ],
    "known_abilities": [
      "DragonBreath",
      "WingAttack"
    ]
  },
  "Fighter": {
    "max_hp": 80,
    "speed": 5,
    "max_action_points": 2,
    "base_attack_power": 10,
    "attack_dice": "1d8+2",
    "base_defense_power": 8,
    "defense_dice": "1d6",
    "attack_range": 1,
    "known_abilities": [
      "ShieldBash",
      "PowerAttack"
    ]
  }
}
```

**Data Structure:**
```cpp
// DataRegistry.h
struct CharacterData
{
    std::string character_type;  // "Dragon", "Fighter", etc.

    // Core stats
    int max_hp;
    int speed;
    int max_action_points;

    // Combat stats
    int base_attack_power;
    std::string attack_dice;      // D&D notation: "2d6", "1d8+2"
    int base_defense_power;
    std::string defense_dice;
    int attack_range;

    // Magic system (optional)
    std::map<int, int> spell_slots;  // {level: count}
    std::vector<std::string> known_spells;
    std::vector<std::string> known_abilities;
};
```

**Loading Process:**
```cpp
// DataRegistry.cpp Lines 169-291
bool DataRegistry::LoadAllCharacterData(const std::string& filepath)
{
    // 1. Open JSON file
    std::ifstream file(assets::locate_asset(filepath));
    if (!file.is_open()) {
        Engine::GetLogger().LogError("Failed to open: " + filepath);
        return false;
    }

    // 2. Parse JSON
    nlohmann::json doc;
    file >> doc;
    file.close();

    // 3. Validate root is object
    if (!doc.is_object()) {
        Engine::GetLogger().LogError("Root must be object");
        return false;
    }

    // 4. Iterate over each character
    for (auto& [characterName, charData] : doc.items())
    {
        CharacterData Cdata;
        Cdata.character_type = characterName;

        // 5. Validate and extract required fields
        if (!charData.contains("max_hp") || !charData["max_hp"].is_number_integer()) {
            Engine::GetLogger().LogError(characterName + ": Missing 'max_hp'");
            continue;  // Skip invalid character
        }
        Cdata.max_hp = charData["max_hp"].get<int>();

        // ... (repeat for all required fields)

        // 6. Extract optional fields
        if (charData.contains("spell_slots") && charData["spell_slots"].is_object())
        {
            for (auto& [levelStr, count] : charData["spell_slots"].items())
            {
                int level = std::stoi(levelStr);
                Cdata.spell_slots[level] = count.get<int>();
            }
        }

        if (charData.contains("known_spells") && charData["known_spells"].is_array())
        {
            Cdata.known_spells = charData["known_spells"].get<std::vector<std::string>>();
        }

        // 7. Store in database
        characterDatabase[characterName] = Cdata;

        Engine::GetLogger().LogEvent(
            "Loaded " + characterName +
            ": HP=" + std::to_string(Cdata.max_hp) +
            ", Speed=" + std::to_string(Cdata.speed)
        );
    }

    return true;
}
```

**Accessing Character Data:**
```cpp
// DataRegistry.cpp Lines 295-305
CharacterData DataRegistry::GetCharacterData(const std::string& name)
{
    auto it = characterDatabase.find(name);
    if (it != characterDatabase.end())
        return it->second;

    Engine::GetLogger().LogError("Character data not found: " + name);
    return CharacterData{};  // Return empty struct
}
```

**Usage in CharacterFactory:**
```cpp
// CharacterFactory.cpp
std::unique_ptr<Dragon> CharacterFactory::Create(CharacterTypes type, Math::ivec2 position)
{
    DataRegistry* registry = Engine::GetGameStateManager().GetGSComponent<DataRegistry>();
    CharacterData data = registry->GetCharacterData("Dragon");

    auto dragon = std::make_unique<Dragon>(position);

    // Apply stats from JSON
    dragon->GetStatsComponent()->SetMaxHP(data.max_hp);
    dragon->GetStatsComponent()->SetSpeed(data.speed);
    dragon->GetStatsComponent()->SetBaseAttack(data.base_attack_power);
    dragon->GetStatsComponent()->SetAttackDice(data.attack_dice);

    // Apply spell slots
    for (auto& [level, count] : data.spell_slots) {
        dragon->GetSpellSlots()->SetMaxSlots(level, count);
    }

    return dragon;
}
```

### 2. Map Data Loading

**File Format:** `Assets/Data/maps.json`

**JSON Structure:**
```json
{
  "tutorial_map": {
    "id": "tutorial_map",
    "name": "Tutorial Arena",
    "width": 8,
    "height": 8,
    "tiles": [
      "wwwwwwww",
      "weeeeeew",
      "weeeeeew",
      "weeeeeew",
      "weeeeeew",
      "weeeeeew",
      "weeeeeew",
      "wwwwwwww"
    ],
    "spawn_points": {
      "dragon": [1, 1],
      "fighter": [6, 6]
    },
    "exit_position": [7, 4]
  },
  "forest_battlefield": {
    "id": "forest_battlefield",
    "name": "Forest Battlefield",
    "width": 8,
    "height": 8,
    "tiles": [
      "wwwwwwww",
      "weeewwew",
      "weeeeeew",
      "wwwweeew",
      "weeeeeew",
      "weewweew",
      "weeeeeew",
      "wwwwwwww"
    ],
    "spawn_points": {
      "dragon": [1, 3],
      "fighter": [6, 4]
    },
    "exit_position": [7, 1]
  }
}
```

**Tile Legend:**
- `w` - Wall (impassable)
- `e` - Empty (walkable)
- `x` - Exit (escape point)

**Data Structure:**
```cpp
// MapDataRegistry.h
struct MapData
{
    std::string id;
    std::string name;
    int width;
    int height;
    std::vector<std::string> tiles;  // Row-major, top to bottom
    std::map<std::string, Math::ivec2> spawn_points;  // {"dragon": {1,1}}
    Math::ivec2 exit_position;
};
```

**Loading Process:**
```cpp
// MapDataRegistry.cpp
void MapDataRegistry::LoadMaps(const std::string& filepath)
{
    std::ifstream file(assets::locate_asset(filepath));
    if (!file.is_open()) {
        Engine::GetLogger().LogError("Failed to open maps file");
        return;
    }

    nlohmann::json doc;
    file >> doc;

    for (auto& [mapId, mapJson] : doc.items())
    {
        MapData map;
        map.id = mapId;
        map.name = mapJson["name"].get<std::string>();
        map.width = mapJson["width"].get<int>();
        map.height = mapJson["height"].get<int>();
        map.tiles = mapJson["tiles"].get<std::vector<std::string>>();

        // Parse spawn points
        for (auto& [spawnerName, coords] : mapJson["spawn_points"].items())
        {
            map.spawn_points[spawnerName] = {
                coords[0].get<int>(),
                coords[1].get<int>()
            };
        }

        // Parse exit
        auto exitCoords = mapJson["exit_position"];
        map.exit_position = {
            exitCoords[0].get<int>(),
            exitCoords[1].get<int>()
        };

        maps_[mapId] = map;
        Engine::GetLogger().LogEvent("Loaded map: " + map.name);
    }
}
```

**Applying Map Data to GridSystem:**
```cpp
// GridSystem.cpp
void GridSystem::LoadMap(const MapData& map_data)
{
    // 1. Set map dimensions
    map_width_ = map_data.width;
    map_height_ = map_data.height;

    // 2. Parse tile grid (top-to-bottom in JSON -> bottom-to-top in engine)
    for (int y = 0; y < map_data.tiles.size(); ++y)
    {
        const std::string& row = map_data.tiles[y];
        for (int x = 0; x < row.length(); ++x)
        {
            char tile_char = row[x];

            // Flip Y coordinate: JSON top-left -> Engine bottom-left
            Math::ivec2 grid_pos = {x, map_data.height - 1 - y};

            switch (tile_char)
            {
                case 'w':
                    SetTileType(grid_pos, TileType::Wall);
                    break;
                case 'e':
                    SetTileType(grid_pos, TileType::Empty);
                    break;
                case 'x':
                    SetTileType(grid_pos, TileType::Exit);
                    SetExitPosition(grid_pos);
                    break;
            }
        }
    }

    // 3. Store exit position
    exit_position_ = map_data.exit_position;

    Engine::GetLogger().LogEvent("Map loaded: " + map_data.name);
}
```

**Usage in GamePlay:**
```cpp
// GamePlay.cpp Lines 542-596
void GamePlay::LoadJSONMap(const std::string& map_id)
{
    // 1. Get map registry
    MapDataRegistry* map_registry = GetGSComponent<MapDataRegistry>();
    MapData map_data = map_registry->GetMapData(map_id);

    if (map_data.id.empty()) {
        Engine::GetLogger().LogError("Map not found: " + map_id);
        LoadFirstMap();  // Fallback to hardcoded map
        return;
    }

    // 2. Load map tiles into GridSystem
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    grid_system->LoadMap(map_data);

    // 3. Spawn characters at defined positions
    CharacterFactory* factory = GetGSComponent<CharacterFactory>();

    // Dragon spawn
    auto dragon_it = map_data.spawn_points.find("dragon");
    if (dragon_it != map_data.spawn_points.end())
    {
        Math::ivec2 spawn = dragon_it->second;
        auto player_ptr = factory->Create(CharacterTypes::Dragon, spawn);
        player = player_ptr.get();
        player->SetGridSystem(grid_system);

        go_manager->Add(std::move(player_ptr));
        grid_system->AddCharacter(player, spawn);
    }

    // Fighter spawn
    auto fighter_it = map_data.spawn_points.find("fighter");
    if (fighter_it != map_data.spawn_points.end())
    {
        Math::ivec2 spawn = fighter_it->second;
        auto enemy_ptr = factory->Create(CharacterTypes::Fighter, spawn);
        enemy = enemy_ptr.get();
        enemy->SetGridSystem(grid_system);

        go_manager->Add(std::move(enemy_ptr));
        grid_system->AddCharacter(enemy, spawn);
    }
}
```

### 3. Hot-Reload System

**Purpose:** Reload JSON files without restarting the game

**File Modification Tracking:**
```cpp
// DataRegistry.h
private:
    std::map<std::string, long long> fileTimestamps;  // filepath -> last modified time
```

**Timestamp Retrieval:**
```cpp
// DataRegistry.cpp Lines 157-165
long long DataRegistry::GetFileModifiedTime(const std::string& filepath) const
{
    struct stat fileInfo;
    if (stat(filepath.c_str(), &fileInfo) == 0)
    {
        return static_cast<long long>(fileInfo.st_mtime);  // UNIX timestamp
    }
    return 0;
}
```

**Reload Function:**
```cpp
// DataRegistry.cpp Lines 80-120
void DataRegistry::ReloadAll()
{
    Engine::GetLogger().LogEvent("=== RELOADING ALL DATA ===");

    std::string charactersFile = "Assets/Data/characters.json";

    // Check if file was modified
    long long currentModTime = GetFileModifiedTime(charactersFile);
    long long lastModTime = fileTimestamps[charactersFile];

    if (currentModTime > lastModTime)
    {
        Engine::GetLogger().LogEvent("Reloading " + charactersFile + " (modified)");

        if (LoadAllCharacterData(charactersFile))
        {
            fileTimestamps[charactersFile] = currentModTime;
            Engine::GetLogger().LogEvent("Reload complete");
        }
        else
        {
            Engine::GetLogger().LogError("Reload failed");
        }
    }
    else
    {
        Engine::GetLogger().LogEvent("No changes detected");
    }
}
```

**Usage:**
```cpp
// In-game: Press 'R' key to reload
if (input.KeyJustPressed(Input::Keys::R))
{
    DataRegistry::Instance().ReloadAll();
}
```

**Benefits:**
- Change character stats during gameplay
- Test balance adjustments without recompilation
- Rapid iteration on game data
- Fallback to previous data on parse error

### 4. Data Validation

**Schema Validation:**
```cpp
// DataRegistry.cpp Lines 338-355
bool DataRegistry::ValidateCharacterJSON(const std::string& filepath)
{
    Engine::GetLogger().LogEvent("Validating: " + filepath);

    // Attempt to load - will log errors for missing/invalid fields
    bool result = LoadAllCharacterData(filepath);

    if (result)
        Engine::GetLogger().LogEvent("Validation passed");
    else
        Engine::GetLogger().LogError("Validation failed");

    return result;
}
```

**Field Validation (during load):**
```cpp
// Required field validation
if (!charData.contains("max_hp") || !charData["max_hp"].is_number_integer())
{
    Engine::GetLogger().LogError(characterName + ": Missing or invalid 'max_hp'");
    continue;  // Skip this character
}

// Type validation
if (!charData["attack_dice"].is_string())
{
    Engine::GetLogger().LogError(characterName + ": 'attack_dice' must be string");
    continue;
}

// Array validation
if (charData.contains("known_spells") && !charData["known_spells"].is_array())
{
    Engine::GetLogger().LogError(characterName + ": 'known_spells' must be array");
}
```

**Error Handling:**
- Missing required field: Skip character, log error, continue loading others
- Invalid type: Skip character, log error
- Parse error: Abort load, log error, return false
- File not found: Log error, use default/fallback data

### 5. Data Access Patterns

**Singleton Access:**
```cpp
// Get DataRegistry from GameStateManager
DataRegistry* registry = Engine::GetGameStateManager().GetGSComponent<DataRegistry>();

// Query character data
CharacterData dragonData = registry->GetCharacterData("Dragon");
```

**Direct Access (Key-Value):**
```cpp
// Legacy system - deprecated in favor of structured access
int dragonHP = registry->GetInt("Dragon.max_hp");
std::string dragonDice = registry->GetString("Dragon.attack_dice");
```

**Database Query:**
```cpp
// DataRegistry.h
private:
    std::map<std::string, CharacterData> characterDatabase;
    std::map<std::string, SpellData> spellDatabase;  // Future
    std::map<std::string, AbilityData> abilityDatabase;  // Future
```

### 6. Loading Flow in GamePlay

**Load Phase** (`GamePlay.cpp` Lines 50-142):

```
GamePlay::Load()
    |
    v
1. AddGSComponent(new DataRegistry())
    |
    v
2. DataRegistry->LoadFromFile("Assets/Data/characters.json")
    |
    v
3. DataRegistry->LoadAllCharacterData("Assets/Data/characters.json")
    -> Parse JSON
    -> Populate characterDatabase
    |
    v
4. AddGSComponent(new MapDataRegistry())
    |
    v
5. MapDataRegistry->LoadMaps("Assets/Data/maps.json")
    -> Parse JSON
    -> Populate maps_ database
    |
    v
6. LoadJSONMap(selected_map_id)
    -> GridSystem->LoadMap(map_data)
    -> Set tile types from map data
    |
    v
7. CharacterFactory->Create(CharacterTypes::Dragon, spawn_position)
    -> Query DataRegistry for "Dragon" stats
    -> Apply stats to Character components
    |
    v
8. TurnManager->InitializeTurnOrder({player, enemy})
    -> Combat ready
```

### Summary Table

| Data Type | File | Format | Loader | Usage |
|-----------|------|--------|--------|-------|
| Character Stats | characters.json | JSON | DataRegistry::LoadAllCharacterData() | CharacterFactory::Create() |
| Map Layout | maps.json | JSON | MapDataRegistry::LoadMaps() | GridSystem::LoadMap() |
| Spells | spells.json | JSON | (Planned) | SpellSystem |
| Abilities | abilities.json | JSON | (Planned) | Ability system |

**Key Benefits:**
- No recompilation needed for content changes
- Easy balancing and iteration
- Data validation with error reporting
- Hot-reload support for rapid testing
- Centralized data access via registries

---

*End of Part 1*
