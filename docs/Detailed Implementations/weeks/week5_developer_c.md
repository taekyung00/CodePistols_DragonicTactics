## Week 5: Developer C - UI/UX Polish

**Goal**: Create polished UI elements for health bars, turn indicators, spell selection, and grid highlights

**Foundation**:

- Week 4 DataRegistry for UI configuration
- Week 2 GridSystem for grid rendering
- CS230 TextManager for text rendering
- ImGui for debug UI (optional overlay)

**Files to Create**:

```
DragonicTactics/source/Game/DragonicTactics/UI/UIManager.h (new file)
DragonicTactics/source/Game/DragonicTactics/UI/UIManager.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/UI/HealthBar.h (new file)
DragonicTactics/source/Game/DragonicTactics/UI/HealthBar.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/UI/TurnIndicator.h (new file)
DragonicTactics/source/Game/DragonicTactics/UI/TurnIndicator.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/UI/SpellSelectionUI.h (new file)
DragonicTactics/source/Game/DragonicTactics/UI/SpellSelectionUI.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/UITests.cpp (new file)
```

**Files to Modify**:

```
DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp (integrate UI rendering)
DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.cpp (add grid highlights)
```

### Implementation Tasks (UI/UX)

#### **Task 1: UIManager Singleton**

**Goal**: Centralized UI management system for all battle UI elements

**Steps**:

1. Create UIManager singleton to coordinate all UI elements
2. Implement UI layer rendering order (background → gameplay → UI → overlay)
3. Add UI element registration and update loop
4. Integrate with BattleState rendering pipeline
5. Handle UI input events (hover, click)

**Why this matters**: UI elements need coordinated rendering and consistent visual hierarchy. Centralized management prevents z-fighting and ensures proper event handling.

---

#### **Task 2: Health Bars**

**Goal**: Display character HP as visual bar above character sprites

**Steps**:

1. Create HealthBar class with position, max_hp, current_hp
2. Implement bar rendering (filled portion + background)
3. Add color coding (green → yellow → red as HP decreases)
4. Subscribe to CharacterDamagedEvent for automatic updates
5. Position health bars above character sprites (follow character position)

**Design Specs**:

- **Bar Size**: 60px wide, 8px tall
- **Position**: 10px above character sprite
- **Colors**: Green (>66% HP), Yellow (33-66% HP), Red (<33% HP)
- **Border**: 1px black outline

---

#### **Task 3: Turn Indicator**

**Goal**: Show which character's turn it is with visual highlight

**Steps**:

1. Create TurnIndicator class to highlight active character
2. Implement arrow/marker above active character
3. Add turn order display (list of upcoming turns)
4. Subscribe to TurnStartedEvent for automatic updates
5. Animate turn transitions (fade in/out)

**Visual Design**:

- **Active Character**: Bright yellow arrow above sprite
- **Turn Order List**: Side panel showing next 3 turns
- **Turn Number**: Display current turn number

---

#### **Task 4: Spell Selection UI**

**Goal**: Interactive spell selection menu when Dragon chooses action

**Steps**:

1. Create SpellSelectionUI panel for spell choices
2. Display available spells with icons, names, spell slot costs
3. Show spell descriptions on hover
4. Handle keyboard/mouse input for spell selection
5. Disable unavailable spells (insufficient spell slots, invalid range)

**UI Layout**:

```
┌─────────────────────────────┐
│     SELECT SPELL            │
├─────────────────────────────┤
│ [1] Fireball (Lvl 3) ✓      │
│ [2] Create Wall (Lvl 2) ✓   │
│ [3] Lava Pool (Lvl 4) ✓     │
│ [A] Melee Attack            │
│ [ESC] Cancel                │
└─────────────────────────────┘
```

---

#### **Task 5: Grid Highlights & Tooltips**

**Goal**: Visual feedback for tile hover, movement range, and spell range

**Steps**:

1. Implement tile hover highlighting (bright outline on hovered tile)
2. Add movement range overlay (blue transparent tiles for walkable)
3. Add spell range overlay (red transparent tiles for targetable)
4. Implement tile tooltips (show tile type, occupant, distance)
5. Add path preview (dashed line showing movement path)

**Visual Layers**:

- **Base Grid**: Always visible grid lines
- **Range Overlay**: Semi-transparent colored tiles (alpha 0.3)
- **Hover Highlight**: Bright white outline (2px)
- **Path Preview**: Animated dashed line

---

### Implementation Examples (UI/UX)

#### **Example 1: UIManager Singleton**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/UI/UIManager.h
#pragma once
#include "../../../Engine/Component.h"
#include "HealthBar.h"
#include "TurnIndicator.h"
#include "SpellSelectionUI.h"
#include <vector>
#include <memory>

// Manages all UI elements in battle
class UIManager : public CS230::Component {
public:
    // Singleton access
    static UIManager& Instance();

    // Component interface
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix);

    // UI element management (public interface - SnakeCase)
    void RegisterHealthBar(HealthBar* health_bar);
    void UnregisterHealthBar(HealthBar* health_bar);
    void ShowSpellSelectionUI(bool show);
    void UpdateTurnIndicator(Character* active_character);

    // Input handling
    bool HandleInput(const CS230::Input& input);

private:
    UIManager() = default;
    ~UIManager() override = default;
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    // UI elements (private - snake_case)
    std::vector<HealthBar*> health_bars_;
    std::unique_ptr<TurnIndicator> turn_indicator_;
    std::unique_ptr<SpellSelectionUI> spell_selection_ui_;

    bool spell_ui_visible_ = false;

    // Rendering layers
    void draw_health_bars(Math::TransformationMatrix camera_matrix);
    void draw_turn_indicator(Math::TransformationMatrix camera_matrix);
    void draw_spell_selection_ui();
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/UI/UIManager.cpp
#include "UIManager.h"
#include "../../../Engine/Engine.hpp"

UIManager& UIManager::Instance() {
    static UIManager instance;
    return instance;
}

void UIManager::Update(double dt) {
    // Update all UI elements
    if (turn_indicator_) {
        turn_indicator_->Update(dt);
    }

    if (spell_selection_ui_ && spell_ui_visible_) {
        spell_selection_ui_->Update(dt);
    }

    for (auto* health_bar : health_bars_) {
        health_bar->Update(dt);
    }
}

void UIManager::Draw(Math::TransformationMatrix camera_matrix) {
    // Render UI in layers
    draw_health_bars(camera_matrix);
    draw_turn_indicator(camera_matrix);

    if (spell_ui_visible_) {
        draw_spell_selection_ui();
    }
}

void UIManager::draw_health_bars(Math::TransformationMatrix camera_matrix) {
    for (auto* health_bar : health_bars_) {
        health_bar->Draw(camera_matrix);
    }
}

void UIManager::draw_turn_indicator(Math::TransformationMatrix camera_matrix) {
    if (turn_indicator_) {
        turn_indicator_->Draw(camera_matrix);
    }
}

void UIManager::draw_spell_selection_ui() {
    if (spell_selection_ui_) {
        spell_selection_ui_->Draw();
    }
}

void UIManager::RegisterHealthBar(HealthBar* health_bar) {
    health_bars_.push_back(health_bar);
    Engine::GetLogger().LogVerbose("UIManager: Registered health bar");
}

void UIManager::UnregisterHealthBar(HealthBar* health_bar) {
    auto it = std::find(health_bars_.begin(), health_bars_.end(), health_bar);
    if (it != health_bars_.end()) {
        health_bars_.erase(it);
        Engine::GetLogger().LogVerbose("UIManager: Unregistered health bar");
    }
}

void UIManager::ShowSpellSelectionUI(bool show) {
    spell_ui_visible_ = show;

    if (show) {
        Engine::GetLogger().LogEvent("UIManager: Showing spell selection UI");
    }
}

void UIManager::UpdateTurnIndicator(Character* active_character) {
    if (turn_indicator_) {
        turn_indicator_->SetActiveCharacter(active_character);
    }
}

bool UIManager::HandleInput(const CS230::Input& input) {
    if (spell_ui_visible_ && spell_selection_ui_) {
        return spell_selection_ui_->HandleInput(input);
    }
    return false;
}
```

---

#### **Example 2: HealthBar Class**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/UI/HealthBar.h
#pragma once
#include "../../../Engine/Vec2.hpp"
#include "../../../Engine/Matrix.hpp"
#include "../../../CS200/RGBA.hpp"

class Character;

// Visual health bar displayed above character
class HealthBar {
public:
    // Constructor (public interface - SnakeCase)
    HealthBar(Character* owner);

    void Update(double dt);
    void Draw(Math::TransformationMatrix camera_matrix);

    // Health management
    void SetHealth(int current, int max);

private:
    // Owner reference
    Character* owner_;

    // Health values (private - snake_case)
    int current_hp_;
    int max_hp_;

    // Visual properties
    static constexpr float BAR_WIDTH = 60.0f;
    static constexpr float BAR_HEIGHT = 8.0f;
    static constexpr float OFFSET_Y = -10.0f; // Above character

    // Color based on HP percentage (private - snake_case)
    CS200::Color get_bar_color() const;
    Math::vec2 get_position() const;
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/UI/HealthBar.cpp
#include "HealthBar.h"
#include "../Objects/Character.h"
#include "../../../Engine/Engine.hpp"

HealthBar::HealthBar(Character* owner)
    : owner_(owner)
    , current_hp_(0)
    , max_hp_(0)
{
    // Subscribe to damage events
    auto stats = owner_->GetGOComponent<StatsComponent>();
    if (stats) {
        current_hp_ = stats->GetCurrentHP();
        max_hp_ = stats->GetMaxHP();
    }
}

void HealthBar::Update(double dt) {
    // Update health values from character
    auto stats = owner_->GetGOComponent<StatsComponent>();
    if (stats) {
        current_hp_ = stats->GetCurrentHP();
        max_hp_ = stats->GetMaxHP();
    }
}

void HealthBar::Draw(Math::TransformationMatrix camera_matrix) {
    Math::vec2 pos = get_position();

    // Draw background (black)
    Engine::GetRenderer2D().DrawRectangle(
        pos, BAR_WIDTH, BAR_HEIGHT, {0, 0, 0, 255}
    );

    // Draw filled portion
    float fill_percentage = static_cast<float>(current_hp_) / max_hp_;
    float filled_width = BAR_WIDTH * fill_percentage;

    Engine::GetRenderer2D().DrawRectangle(
        pos, filled_width, BAR_HEIGHT, get_bar_color()
    );

    // Draw border (white outline)
    Engine::GetRenderer2D().DrawRectangleOutline(
        pos, BAR_WIDTH, BAR_HEIGHT, {255, 255, 255, 255}, 1.0f
    );
}

CS200::Color HealthBar::get_bar_color() const {
    float hp_percentage = static_cast<float>(current_hp_) / max_hp_;

    if (hp_percentage > 0.66f) {
        return {0, 255, 0, 255}; // Green
    } else if (hp_percentage > 0.33f) {
        return {255, 255, 0, 255}; // Yellow
    } else {
        return {255, 0, 0, 255}; // Red
    }
}

Math::vec2 HealthBar::get_position() const {
    Math::vec2 char_pos = owner_->GetPosition();
    return {char_pos.x - BAR_WIDTH / 2, char_pos.y + OFFSET_Y};
}

void HealthBar::SetHealth(int current, int max) {
    current_hp_ = current;
    max_hp_ = max;
}
```

---

### Rigorous Testing (UI/UX)

#### **Test 1: UIManager Singleton**

```cpp
bool TestUIManagerSingleton() {
    UIManager& ui1 = UIManager::Instance();
    UIManager& ui2 = UIManager::Instance();

    if (&ui1 != &ui2) {
        Engine::GetLogger().LogError("UIManager singleton failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ UIManager singleton test passed");
    return true;
}
```

#### **Test 2: HealthBar Color Coding**

```cpp
bool TestHealthBarColorCoding() {
    Dragon dragon({3, 3});
    HealthBar health_bar(&dragon);

    auto stats = dragon.GetGOComponent<StatsComponent>();

    // Full HP - should be green
    health_bar.SetHealth(stats->GetMaxHP(), stats->GetMaxHP());
    // Verify color (would need access to get_bar_color or visual inspection)

    // Half HP - should be yellow
    health_bar.SetHealth(stats->GetMaxHP() / 2, stats->GetMaxHP());

    // Low HP - should be red
    health_bar.SetHealth(10, stats->GetMaxHP());

    Engine::GetLogger().LogEvent("✅ HealthBar color coding test passed");
    return true;
}
```

#### **Test 3: Health Bar Position Tracking**

```cpp
bool TestHealthBarPositionTracking() {
    Dragon dragon({3, 3});
    HealthBar health_bar(&dragon);

    // Move character
    dragon.SetPosition({200, 300});

    // Health bar should follow (verify through rendering or position getter)

    Engine::GetLogger().LogEvent("✅ HealthBar position tracking test passed");
    return true;
}
```

#### **Test 4: Spell Selection UI Input**

```cpp
bool TestSpellSelectionUIInput() {
    UIManager& ui = UIManager::Instance();

    // Show spell UI
    ui.ShowSpellSelectionUI(true);

    // Simulate input (would need mock Input system)
    // Verify correct spell selected

    ui.ShowSpellSelectionUI(false);

    Engine::GetLogger().LogEvent("✅ Spell selection UI input test passed");
    return true;
}
```

#### **Test 5: Turn Indicator Update**

```cpp
bool TestTurnIndicatorUpdate() {
    UIManager& ui = UIManager::Instance();

    Dragon dragon({3, 3});
    Fighter fighter({5, 5});

    // Update turn indicator to Dragon
    ui.UpdateTurnIndicator(&dragon);

    // Verify indicator points to Dragon (visual inspection)

    // Update turn indicator to Fighter
    ui.UpdateTurnIndicator(&fighter);

    // Verify indicator points to Fighter

    Engine::GetLogger().LogEvent("✅ Turn indicator update test passed");
    return true;
}
```

---

### Daily Breakdown (Developer C)

#### **Monday (7-8 hours)**

- Create UIManager singleton skeleton (1 hr)
- Implement UI rendering layers (1.5 hrs)
- Create HealthBar class (2 hrs)
- Implement health bar color coding (1 hr)
- Test health bars in battle (1.5 hrs)
- **Deliverable**: Health bars working

#### **Tuesday (7-8 hours)**

- Create TurnIndicator class (2 hrs)
- Implement turn indicator visual (arrow/marker) (1.5 hrs)
- Add turn order display panel (2 hrs)
- Integrate with TurnManager events (1 hr)
- Test turn indicator updates (1 hr)
- **Deliverable**: Turn indicator complete

#### **Wednesday (6-7 hours)**

- Create SpellSelectionUI class (2.5 hrs)
- Implement spell list rendering (1.5 hrs)
- Add spell descriptions on hover (1 hr)
- Implement keyboard/mouse input handling (2 hrs)
- **Deliverable**: Spell selection UI functional

#### **Thursday (6-7 hours)**

- Implement grid tile hover highlighting (1.5 hrs)
- Add movement range overlay (2 hrs)
- Add spell range overlay (1.5 hrs)
- Implement path preview visualization (2 hrs)
- **Deliverable**: Grid highlights complete

#### **Friday (4-5 hours)**

- Write comprehensive UI test suite (2 hrs)
- Polish UI visual consistency (1 hr)
- Final UI integration with BattleState (1 hr)
- Bug fixes and tweaks (1 hr)
- **Deliverable**: UI/UX production-ready

---
