# Debug UI Layout & Visual Design

This document specifies the visual layout, positioning, and styling for all debug UI elements in Dragonic Tactics.

---

## Table of Contents

1. [Screen Layout Overview](#screen-layout-overview)
2. [Debug UI Overlay Panel](#debug-ui-overlay-panel)
3. [Console Window](#console-window)
4. [Visual Debug Overlays](#visual-debug-overlays)
5. [Color Schemes](#color-schemes)
6. [Font & Text Rendering](#font--text-rendering)

---

## Screen Layout Overview

### Full Screen Layout (1280x720 reference)

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ DEBUG MODE [F12]                                              FPS: 30.0      │ ← Debug Header (10, 10)
│ Turn: 5  |  Current: Dragon                            God Mode: ON          │ ← Status Bar (10, 30)
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                               │
│  ┌─────────────────────────────────────────────────────────┐                │
│  │                                                           │                │
│  │                 8x8 GRID BATTLEFIELD                     │                │
│  │         (Center: 640, 360)                               │  ┌───────────┐ │
│  │                                                           │  │ Character │ │
│  │   [Grid overlay with coordinates and highlighting]       │  │ Info      │ │
│  │                                                           │  │ Panel     │ │
│  │                                                           │  │ (Right)   │ │
│  │                                                           │  │           │ │
│  │                                                           │  │ Dragon    │ │
│  │   [Characters with HP bars above them]                   │  │ 140/140HP │ │
│  │                                                           │  │ AP: 5/5   │ │
│  │   [AI path visualization with arrows]                    │  │ Spells:   │ │
│  │                                                           │  │ [3,3,2]   │ │
│  │   [Status effect badges on characters]                   │  │           │ │
│  │                                                           │  │ Enemies:  │ │
│  │                                                           │  │ Fighter   │ │
│  │                                                           │  │ Cleric    │ │
│  │                                                           │  │ Wizard    │ │
│  └─────────────────────────────────────────────────────────┘  │ Rogue     │ │
│                                                                └───────────┘ │
│                                                                               │
│ ┌───────────────────────────────────────────────────────────────────────────┤
│ │ > spawn dragon 3 7                                                        ││ ← Console (Bottom)
│ │ Spawned dragon at (3, 7)                                                  ││   Opens with ~ key
│ │ > damage fighter 30                                                       ││   Height: 200px
│ │ Dealt 30 damage to fighter (90 -> 60 HP)                                 ││
│ │ > _                                                                       ││ ← Input cursor
│ └───────────────────────────────────────────────────────────────────────────┘│
└──────────────────────────────────────────────────────────────────────────────┘
```

### Z-Order Layering (Bottom to Top)

1. **Background** - Game background sprites
2. **Grid Lines** - Debug grid overlay (F1)
3. **Characters** - Character sprites
4. **HP Bars** - Above characters
5. **Status Effect Badges** - Above HP bars
6. **AI Path Visualization** - Overlay on grid
7. **Collision Boxes** - Debug overlay (F10)
8. **Debug UI Panel** - Top-right corner
9. **Console** - Bottom overlay (highest priority when open)

---

## Debug UI Overlay Panel

### Position & Dimensions

```cpp
const Math::vec2 DEBUG_PANEL_POSITION = {1050, 10};  // Top-right corner
const int DEBUG_PANEL_WIDTH = 220;
const int DEBUG_PANEL_HEIGHT = 400;
const int DEBUG_PANEL_PADDING = 10;
const int LINE_HEIGHT = 18;
```

### Visual Layout

```
┌────────────────────────┐
│  DEBUG MODE [F12]      │ ← Header (Yellow)
├────────────────────────┤
│  Turn: 5               │
│  Current: Dragon       │ ← Turn Info (White)
│  God Mode: ON          │ ← God Mode Indicator (Green/Red)
├────────────────────────┤
│  Selected: Dragon      │ ← Selected Character (Cyan)
│    HP: 140/140         │
│    AP: 5/5             │
│    Spells: [3,3,2]     │
│    Grid: (3, 7)        │
│    Status: None        │
├────────────────────────┤
│  Enemies Alive: 4/4    │ ← Enemy Summary (White)
│                        │
│  Fighter   90/90 HP    │ ← Individual Enemy Stats
│  @ (2, 0)              │   (Green = full, Yellow = hurt, Red = critical)
│                        │
│  Cleric    90/90 HP    │
│  @ (4, 0)              │
│                        │
│  Wizard    55/55 HP    │
│  @ (1, 0)              │
│                        │
│  Rogue     65/65 HP    │
│  @ (5, 0)              │
├────────────────────────┤
│  Quick Commands:       │ ← Command Reference (Gray)
│  [G] God Mode          │
│  [~] Console           │
│  [R] Reload Data       │
│  [H] Hide UI           │
└────────────────────────┘
```

### Drawing Code Example

```cpp
void DebugUIOverlay::Draw(Math::TransformationMatrix camera_matrix) {
    if (!visible) return;

    Math::vec2 pos = DEBUG_PANEL_POSITION;

    // Draw semi-transparent background
    DrawRect(pos.x, pos.y, DEBUG_PANEL_WIDTH, DEBUG_PANEL_HEIGHT,
             {0, 0, 0, 180});  // Black with 70% opacity

    // Draw border
    DrawRectOutline(pos.x, pos.y, DEBUG_PANEL_WIDTH, DEBUG_PANEL_HEIGHT,
                    {255, 255, 0, 255});  // Yellow border

    int yOffset = DEBUG_PANEL_PADDING;

    // Header
    DrawText("DEBUG MODE [F12]", pos.x + DEBUG_PANEL_PADDING, pos.y + yOffset,
             {255, 255, 0});  // Yellow
    yOffset += LINE_HEIGHT + 5;

    // Separator line
    DrawLine(pos.x + 5, pos.y + yOffset, pos.x + DEBUG_PANEL_WIDTH - 5, pos.y + yOffset,
             {255, 255, 0, 255});
    yOffset += 10;

    // Turn info
    DrawText("Turn: " + std::to_string(currentTurn),
             pos.x + DEBUG_PANEL_PADDING, pos.y + yOffset, {255, 255, 255});
    yOffset += LINE_HEIGHT;

    DrawText("Current: " + currentCharacterName,
             pos.x + DEBUG_PANEL_PADDING, pos.y + yOffset, {255, 255, 255});
    yOffset += LINE_HEIGHT;

    // God mode indicator
    bool godMode = GodModeManager::Instance().IsGodModeEnabled();
    DrawText("God Mode: " + std::string(godMode ? "ON" : "OFF"),
             pos.x + DEBUG_PANEL_PADDING, pos.y + yOffset,
             godMode ? Color{0, 255, 0} : Color{255, 0, 0});  // Green/Red
    yOffset += LINE_HEIGHT + 5;

    // Continue with selected character info, enemies, etc...
}
```

---

## Console Window

### Position & Dimensions

```cpp
const int CONSOLE_HEIGHT = 200;
const int CONSOLE_WIDTH = 1260;  // Full screen width minus padding
const Math::vec2 CONSOLE_POSITION = {10, 520};  // Bottom of screen
const int CONSOLE_PADDING = 10;
const int CONSOLE_LINE_HEIGHT = 16;
const int CONSOLE_MAX_LINES = 10;
```

### Visual Layout

```
┌────────────────────────────────────────────────────────────────────────┐
│ CONSOLE [~]                                                      [X]   │ ← Header with close button
├────────────────────────────────────────────────────────────────────────┤
│ > help                                                                 │ ← Command history (scrollable)
│ === Available Commands ===                                             │   (Yellow = user input)
│ help [command] - Show available commands                               │   (Green = success output)
│ spawn <type> <x> <y> - Spawn character                                 │   (Red = error output)
│ damage <name> <amount> - Deal damage                                   │   (White = info output)
│ god [on/off] - Toggle god mode                                         │
│                                                                         │
│ > spawn dragon 3 7                                                     │
│ Spawned dragon at (3, 7)                                               │
│                                                                         │
├────────────────────────────────────────────────────────────────────────┤
│ > _                                                                    │ ← Input line with blinking cursor
└────────────────────────────────────────────────────────────────────────┘
```

### Drawing Code Example

```cpp
void DebugConsole::Draw(Math::TransformationMatrix camera_matrix) {
    if (!isOpen) return;

    Math::vec2 pos = CONSOLE_POSITION;

    // Draw semi-transparent black background
    DrawRect(pos.x, pos.y, CONSOLE_WIDTH, CONSOLE_HEIGHT,
             {0, 0, 0, 220});  // 86% opacity

    // Draw border
    DrawRectOutline(pos.x, pos.y, CONSOLE_WIDTH, CONSOLE_HEIGHT,
                    {100, 100, 255, 255});  // Blue border

    int yOffset = CONSOLE_PADDING;

    // Header with close button
    DrawText("CONSOLE [~]", pos.x + CONSOLE_PADDING, pos.y + yOffset,
             {100, 100, 255});  // Blue
    DrawText("[X]", pos.x + CONSOLE_WIDTH - 40, pos.y + yOffset,
             {255, 100, 100});  // Red close button
    yOffset += CONSOLE_LINE_HEIGHT + 5;

    // Separator line
    DrawLine(pos.x + 5, pos.y + yOffset, pos.x + CONSOLE_WIDTH - 5, pos.y + yOffset,
             {100, 100, 255, 255});
    yOffset += 5;

    // Draw output buffer (last N lines, scrolling)
    int startLine = std::max(0, (int)outputBuffer.size() - CONSOLE_MAX_LINES + 1);
    for (int i = startLine; i < outputBuffer.size(); i++) {
        auto& [text, color] = outputBuffer[i];
        DrawText(text, pos.x + CONSOLE_PADDING, pos.y + yOffset, GetRGBColor(color));
        yOffset += CONSOLE_LINE_HEIGHT;
    }

    // Draw input line at bottom
    int inputY = pos.y + CONSOLE_HEIGHT - CONSOLE_PADDING - CONSOLE_LINE_HEIGHT - 10;

    // Input background highlight
    DrawRect(pos.x + 5, inputY - 2, CONSOLE_WIDTH - 10, CONSOLE_LINE_HEIGHT + 4,
             {30, 30, 30, 255});

    // Input prompt and text
    DrawText("> " + currentInput, pos.x + CONSOLE_PADDING, inputY,
             {255, 255, 255});

    // Blinking cursor
    if ((int)(Engine::GetTotalTime() * 2) % 2 == 0) {
        int cursorX = pos.x + CONSOLE_PADDING + GetTextWidth("> " + currentInput);
        DrawText("_", cursorX, inputY, {255, 255, 255});
    }
}
```

### Color Helper

```cpp
Color DebugConsole::GetRGBColor(ConsoleColor color) {
    switch (color) {
        case ConsoleColor::White:  return {255, 255, 255, 255};
        case ConsoleColor::Green:  return {0, 255, 0, 255};
        case ConsoleColor::Red:    return {255, 0, 0, 255};
        case ConsoleColor::Yellow: return {255, 255, 0, 255};
        case ConsoleColor::Blue:   return {100, 100, 255, 255};
        default: return {255, 255, 255, 255};
    }
}
```

---

## Visual Debug Overlays

### Grid Overlay (F1)

**Visual Style**:

- Grid lines: Light gray (128, 128, 128, 128) - 50% opacity
- Tile coordinates: White text (255, 255, 255, 200) - 78% opacity
- Occupied tiles: Red tint (255, 0, 0, 50) - 20% opacity
- Empty tiles: Green tint (0, 255, 0, 30) - 12% opacity
- Movement range: Blue overlay (0, 0, 255, 80) - 31% opacity
- Attack range: Orange overlay (255, 128, 0, 80) - 31% opacity

```cpp
void GridDebugRenderer::DrawGridLines() {
    Color gridColor = {128, 128, 128, 128};

    // Draw vertical lines
    for (int x = 0; x <= 8; x++) {
        Math::vec2 start = GridToWorld({x, 0});
        Math::vec2 end = GridToWorld({x, 8});
        DrawLine(start.x, start.y, end.x, end.y, gridColor);
    }

    // Draw horizontal lines
    for (int y = 0; y <= 8; y++) {
        Math::vec2 start = GridToWorld({0, y});
        Math::vec2 end = GridToWorld({8, y});
        DrawLine(start.x, start.y, end.x, end.y, gridColor);
    }
}

void GridDebugRenderer::DrawTileCoordinates() {
    Color textColor = {255, 255, 255, 200};

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Math::vec2 worldPos = GridToWorld({x, y});
            std::string coordText = std::to_string(x) + "," + std::to_string(y);

            // Draw at center of tile
            DrawText(coordText,
                    worldPos.x + TILE_SIZE / 4,
                    worldPos.y + TILE_SIZE / 4,
                    textColor);
        }
    }
}
```

### HP Bars Above Characters

**Visual Style**:

- Position: Above character sprite, centered
- Size: 40px wide × 6px tall
- Background: Dark gray (50, 50, 50, 200)
- Border: Black outline (0, 0, 0, 255)
- Fill colors:
  - > 66% HP: Green (0, 255, 0, 255)
  - 33-66% HP: Yellow (255, 255, 0, 255)
  - < 33% HP: Red (255, 0, 0, 255)
- Text: White HP numbers (255, 255, 255, 255)

```cpp
void StatusInfoOverlay::DrawHPBar(Character* character) {
    Math::vec2 charPos = character->GetPosition();

    // Position above character sprite (assuming 64x64 sprite)
    float barX = charPos.x - 20;  // Centered on character
    float barY = charPos.y - 40;  // 40 pixels above
    float barWidth = 40;
    float barHeight = 6;

    // Background
    DrawRect(barX, barY, barWidth, barHeight, {50, 50, 50, 200});

    // Border
    DrawRectOutline(barX, barY, barWidth, barHeight, {0, 0, 0, 255});

    // HP fill
    float hpPercent = (float)character->GetCurrentHP() / character->GetMaxHP();
    float fillWidth = barWidth * hpPercent;

    Color fillColor;
    if (hpPercent > 0.66f) {
        fillColor = {0, 255, 0, 255};  // Green
    } else if (hpPercent > 0.33f) {
        fillColor = {255, 255, 0, 255};  // Yellow
    } else {
        fillColor = {255, 0, 0, 255};  // Red
    }

    DrawRect(barX, barY, fillWidth, barHeight, fillColor);

    // HP text (e.g., "90/90")
    std::string hpText = std::to_string(character->GetCurrentHP()) + "/" +
                         std::to_string(character->GetMaxHP());
    DrawText(hpText, barX + 2, barY - 12, {255, 255, 255, 255});
}
```

### AI Path Visualization (F4)

**Visual Style**:

- Path line: Yellow dotted line (255, 255, 0, 200)
- Waypoints: Yellow circles (255, 255, 0, 150)
- Target marker: Red X (255, 0, 0, 255)
- Threat values: Red text above enemies (255, 0, 0, 255)
- AI state text: Cyan above AI character (0, 255, 255, 255)

```cpp
void AIDebugVisualizer::DrawAIPath(Character* aiCharacter) {
    auto& debugInfo = aiDebugData[aiCharacter];

    if (debugInfo.currentPath.empty()) return;

    // Draw path as dotted line
    Color pathColor = {255, 255, 0, 200};
    Math::vec2 previousPos = aiCharacter->GetPosition();

    for (auto& gridPos : debugInfo.currentPath) {
        Math::vec2 worldPos = GridToWorld(gridPos);

        // Draw dotted line (draw every other segment)
        DrawDottedLine(previousPos.x, previousPos.y, worldPos.x, worldPos.y, pathColor);

        // Draw waypoint circle
        DrawCircle(worldPos.x, worldPos.y, 5, {255, 255, 0, 150});

        previousPos = worldPos;
    }

    // Draw target marker (red X)
    if (debugInfo.currentTarget != nullptr) {
        Math::vec2 targetPos = debugInfo.currentTarget->GetPosition();
        float size = 10;
        DrawLine(targetPos.x - size, targetPos.y - size,
                 targetPos.x + size, targetPos.y + size,
                 {255, 0, 0, 255});
        DrawLine(targetPos.x + size, targetPos.y - size,
                 targetPos.x - size, targetPos.y + size,
                 {255, 0, 0, 255});
    }

    // Draw AI state text above character
    Math::vec2 charPos = aiCharacter->GetPosition();
    DrawText(debugInfo.currentState,
             charPos.x - 20, charPos.y - 60,
             {0, 255, 255, 255});  // Cyan
}
```

### Status Effect Badges

**Visual Style**:

- Position: Above HP bar, right-aligned
- Size: 16x16 pixels per badge
- Spacing: 2px between badges
- Icons: Simple colored squares with letters
  - Burn: Red "B" (255, 0, 0)
  - Fear: Purple "F" (128, 0, 255)
  - Blessing: Gold "+" (255, 215, 0)
  - Poison: Green "P" (0, 255, 0)

```cpp
void StatusInfoOverlay::DrawStatusEffectBadges(Character* character) {
    auto statusEffects = character->GetGOComponent<StatusEffects>();
    if (statusEffects == nullptr) return;

    auto& effects = statusEffects->GetActiveEffects();
    if (effects.empty()) return;

    Math::vec2 charPos = character->GetPosition();

    // Position above HP bar, right-aligned
    float badgeX = charPos.x + 20;  // Right side of HP bar
    float badgeY = charPos.y - 52;  // Above HP bar
    const int BADGE_SIZE = 16;
    const int BADGE_SPACING = 2;

    for (int i = 0; i < effects.size(); i++) {
        float x = badgeX - ((i + 1) * (BADGE_SIZE + BADGE_SPACING));

        // Draw badge background
        DrawRect(x, badgeY, BADGE_SIZE, BADGE_SIZE, {0, 0, 0, 180});

        // Draw badge icon/letter
        std::string icon;
        Color iconColor;

        switch (effects[i].type) {
            case EffectType::Burn:
                icon = "B";
                iconColor = {255, 0, 0, 255};
                break;
            case EffectType::Fear:
                icon = "F";
                iconColor = {128, 0, 255, 255};
                break;
            case EffectType::Blessing:
                icon = "+";
                iconColor = {255, 215, 0, 255};
                break;
            case EffectType::Poison:
                icon = "P";
                iconColor = {0, 255, 0, 255};
                break;
        }

        DrawText(icon, x + 4, badgeY + 2, iconColor);

        // Draw border
        DrawRectOutline(x, badgeY, BADGE_SIZE, BADGE_SIZE, iconColor);
    }
}
```

### Collision Boxes (F10)

**Visual Style**:

- No collision: Green rectangles (0, 255, 0, 100)
- Colliding: Red rectangles (255, 0, 0, 150)
- Line thickness: 2px
- Draw over characters but under UI

```cpp
void CollisionDebugRenderer::DrawCharacterCollisionBoxes() {
    auto currentState = Engine::GetGameStateManager().GetCurrentState();
    auto& gameObjects = currentState->GetGameObjectManager().GetAllObjects();

    for (auto* obj : gameObjects) {
        Character* character = dynamic_cast<Character*>(obj);
        if (character == nullptr) continue;

        auto collision = character->GetGOComponent<CS230::RectCollision>();
        if (collision == nullptr) continue;

        Math::rect collisionRect = collision->GetWorldBounds();
        bool isColliding = false;  // Check against other characters

        Color boxColor = isColliding ?
            Color{255, 0, 0, 150} :  // Red if colliding
            Color{0, 255, 0, 100};   // Green if no collision

        DrawRectOutline(collisionRect.point1.x, collisionRect.point1.y,
                       collisionRect.Size().x, collisionRect.Size().y,
                       boxColor, 2);  // 2px thickness
    }
}
```

---

### Dice Roll History Panel (F9)

**Position**: Bottom-right corner (next to character info panel)

**Dimensions**:
- Width: 300px
- Height: 250px
- Position: (970, 460)

**Visual Layout**:

```
┌─────────────────────────────────┐
│ DICE ROLL HISTORY [F9]          │ ← Header (Yellow)
├─────────────────────────────────┤
│ 3d6 = 15 [6, 5, 4]              │ ← Recent roll (Green = high)
│   Dragon attack on Fighter      │
│   0.5s ago                       │
├─────────────────────────────────┤
│ 2d8+5 = 13 [4, 4] +5            │ ← Average roll (Yellow)
│   Fireball damage                │
│   2.1s ago                       │
├─────────────────────────────────┤
│ 1d20 = 3                         │ ← Low roll (Red)
│   Initiative roll                │
│   5.3s ago                       │
├─────────────────────────────────┤
│ Stats: Min:3 Max:15 Avg:10.3    │ ← Statistics
└─────────────────────────────────┘
```

**Color Coding**:
- High roll (>75% of max): Green
- Average roll (25-75%): Yellow
- Low roll (<25%): Red

**Drawing Code**:

```cpp
void DiceHistoryPanel::Draw() {
    const Math::vec2 POS = {970, 460};
    const int WIDTH = 300;
    const int HEIGHT = 250;

    // Background
    DrawFilledRect(POS.x, POS.y, WIDTH, HEIGHT, {0, 0, 0, 200});
    DrawRectOutline(POS.x, POS.y, WIDTH, HEIGHT, {255, 255, 0, 255}, 2);

    // Header
    DrawText("DICE ROLL HISTORY [F9]", POS.x + 10, POS.y + 10, {255, 255, 0, 255});

    // Recent rolls
    int yOffset = 40;
    for (const auto& roll : recentRolls) {
        Color rollColor = GetRollColor(roll.result, roll.minPossible, roll.maxPossible);
        DrawText(roll.notation + " = " + std::to_string(roll.result),
                 POS.x + 10, POS.y + yOffset, rollColor);

        // Show individual dice
        std::string breakdown = "  [";
        for (int die : roll.individualRolls) {
            breakdown += std::to_string(die) + ", ";
        }
        breakdown += "]";
        DrawText(breakdown, POS.x + 20, POS.y + yOffset + 15, {200, 200, 200, 255});

        yOffset += 50;
    }
}
```

---

### Combat Formula Inspector (F12)

**Position**: Left side of screen

**Dimensions**:
- Width: 350px
- Height: 300px
- Position: (10, 150)

**Visual Layout**:

```
┌──────────────────────────────────────┐
│ COMBAT FORMULA [F12]                 │ ← Header (Yellow)
├──────────────────────────────────────┤
│ Attacker: Dragon                     │
│ Target: Fighter                      │
├──────────────────────────────────────┤
│ BASE DAMAGE:                         │
│   3d6 = 12  [4, 6, 2]                │ ← Dice breakdown
│                                      │
│ MODIFIERS:                           │
│   + Attack Bonus: +5                 │ ← Green (positive)
│   + Blessing: +2                     │
│   - Defense: -3  (1d4 = 3)          │ ← Red (negative)
│   - Fear: -1                         │
├──────────────────────────────────────┤
│ FORMULA:                             │
│   3d6(12) +5 +2 -1d4(3) -1 = 15     │ ← Final calculation
│                                      │
│ DAMAGE RANGE: 6 - 24                │ ← Min/max possible
└──────────────────────────────────────┘
```

**Drawing Code**:

```cpp
void CombatFormulaInspector::Draw() {
    if (!isVisible) return;

    const Math::vec2 POS = {10, 150};
    const int WIDTH = 350;

    // Background
    DrawFilledRect(POS.x, POS.y, WIDTH, currentHeight, {0, 0, 0, 220});
    DrawRectOutline(POS.x, POS.y, WIDTH, currentHeight, {255, 255, 0, 255}, 2);

    int yOffset = 10;

    // Header
    DrawText("COMBAT FORMULA [F12]", POS.x + 10, POS.y + yOffset, {255, 255, 0, 255});
    yOffset += 30;

    // Attacker/Target
    DrawText("Attacker: " + attacker->TypeName(), POS.x + 10, POS.y + yOffset, {255, 255, 255, 255});
    yOffset += 20;
    DrawText("Target: " + target->TypeName(), POS.x + 10, POS.y + yOffset, {255, 255, 255, 255});
    yOffset += 30;

    // Base damage
    DrawText("BASE DAMAGE:", POS.x + 10, POS.y + yOffset, {255, 255, 0, 255});
    yOffset += 20;
    DrawText("  " + formula.baseDiceNotation + " = " + std::to_string(formula.baseDiceResult),
             POS.x + 10, POS.y + yOffset, {0, 255, 0, 255});
    yOffset += 25;

    // Modifiers
    DrawText("MODIFIERS:", POS.x + 10, POS.y + yOffset, {255, 255, 0, 255});
    yOffset += 20;

    for (auto& [name, value] : formula.modifiers) {
        Color modColor = (value > 0) ? Color{0, 255, 0, 255} : Color{255, 0, 0, 255};
        std::string sign = (value > 0) ? "+" : "";
        DrawText("  " + sign + std::to_string(value) + " " + name,
                 POS.x + 10, POS.y + yOffset, modColor);
        yOffset += 20;
    }

    yOffset += 10;

    // Final formula
    DrawText("FINAL DAMAGE: " + std::to_string(formula.finalDamage),
             POS.x + 10, POS.y + yOffset, {255, 128, 0, 255});
}
```

---

### Event Bus Tracer (Ctrl+E)

**Position**: Bottom-left corner

**Dimensions**:
- Width: 500px
- Height: 200px
- Position: (10, 510)

**Visual Layout**:

```
┌────────────────────────────────────────────────────────────┐
│ EVENT BUS TRACER [Ctrl+E]  Filter: [All Events ▼]         │
├────────────────────────────────────────────────────────────┤
│ 0.1s │ DamageTakenEvent │ Dragon took 15 dmg │ 3 subs    │
│ 0.5s │ TurnStartedEvent │ Rogue's turn       │ 5 subs    │
│ 1.2s │ SpellCastEvent   │ Fireball Level 1   │ 4 subs    │
│ 2.0s │ CharacterMovedEvent │ Dragon (4,4)→(5,5) │ 2 subs │
├────────────────────────────────────────────────────────────┤
│ Total Events: 247 │ Types: 12 │ Unhandled: 0              │
└────────────────────────────────────────────────────────────┘
```

**Drawing Code**:

```cpp
void EventBusTracer::Draw() {
    if (!isVisible) return;

    const Math::vec2 POS = {10, 510};
    const int WIDTH = 500;
    const int HEIGHT = 200;

    // Background
    DrawFilledRect(POS.x, POS.y, WIDTH, HEIGHT, {0, 0, 0, 230});
    DrawRectOutline(POS.x, POS.y, WIDTH, HEIGHT, {128, 255, 255, 255}, 2);

    // Header
    DrawText("EVENT BUS TRACER [Ctrl+E]", POS.x + 10, POS.y + 10, {128, 255, 255, 255});

    // Event stream (last 10 events)
    int yOffset = 40;
    for (const auto& entry : eventLog) {
        double ageSeconds = GetCurrentTime() - entry.timestamp;

        std::string line = FormatTime(ageSeconds) + " | " +
                          entry.eventType + " | " +
                          entry.payload + " | " +
                          std::to_string(entry.subscriberCount) + " subs";

        Color eventColor = (ageSeconds < 1.0) ? Color{255, 255, 0, 255} : Color{200, 200, 200, 255};
        DrawText(line, POS.x + 10, POS.y + yOffset, eventColor);

        yOffset += 18;
    }

    // Statistics footer
    DrawText("Total Events: " + std::to_string(totalEventCount) +
             " | Types: " + std::to_string(eventCounts.size()),
             POS.x + 10, POS.y + HEIGHT - 20, {255, 255, 255, 255});
}
```

---

## Color Schemes

### Primary Debug Colors

```cpp
// Color Palette for Debug UI
namespace DebugColors {
    const Color HEADER = {255, 255, 0, 255};        // Yellow - Headers
    const Color SUCCESS = {0, 255, 0, 255};         // Green - Success messages
    const Color ERROR = {255, 0, 0, 255};           // Red - Errors
    const Color INFO = {255, 255, 255, 255};        // White - Info
    const Color WARNING = {255, 128, 0, 255};       // Orange - Warnings
    const Color COMMAND = {255, 255, 0, 255};       // Yellow - User input
    const Color SYSTEM = {100, 100, 255, 255};      // Blue - System messages

    // HP Bar Colors
    const Color HP_HIGH = {0, 255, 0, 255};         // Green - > 66% HP
    const Color HP_MID = {255, 255, 0, 255};        // Yellow - 33-66% HP
    const Color HP_LOW = {255, 0, 0, 255};          // Red - < 33% HP

    // Grid Colors
    const Color GRID_LINE = {128, 128, 128, 128};   // Light gray - Grid lines
    const Color TILE_COORD = {255, 255, 255, 200};  // White - Coordinates
    const Color TILE_OCCUPIED = {255, 0, 0, 50};    // Red tint - Occupied
    const Color TILE_EMPTY = {0, 255, 0, 30};       // Green tint - Empty
    const Color MOVE_RANGE = {0, 0, 255, 80};       // Blue - Movement range
    const Color ATTACK_RANGE = {255, 128, 0, 80};   // Orange - Attack range

    // AI Debug Colors
    const Color AI_PATH = {255, 255, 0, 200};       // Yellow - Path line
    const Color AI_WAYPOINT = {255, 255, 0, 150};   // Yellow - Waypoints
    const Color AI_TARGET = {255, 0, 0, 255};       // Red - Target marker
    const Color AI_STATE = {0, 255, 255, 255};      // Cyan - State text
    const Color THREAT_VALUE = {255, 0, 0, 255};    // Red - Threat numbers
}
```

---

## Font & Text Rendering

### Font Selection

- **Primary Font**: Use `Engine::GetFont(0)` for all debug text
- **Font Size**: 14px for most text, 12px for small labels, 16px for headers
- **Style**: Monospace preferred for console, sans-serif OK for overlay

### Text Shadow for Readability

```cpp
void DrawTextWithShadow(const std::string& text, float x, float y, Color color) {
    // Draw shadow (offset 1px down-right, 50% opacity black)
    Color shadowColor = {0, 0, 0, 128};
    Engine::GetFont(0).DrawText(text, {x + 1, y + 1}, shadowColor);

    // Draw main text
    Engine::GetFont(0).DrawText(text, {x, y}, color);
}
```

### Line Wrapping for Console

```cpp
std::vector<std::string> WrapText(const std::string& text, int maxWidth) {
    std::vector<std::string> lines;
    std::string currentLine;

    for (char c : text) {
        currentLine += c;

        if (GetTextWidth(currentLine) > maxWidth) {
            // Find last space to break at word boundary
            size_t lastSpace = currentLine.find_last_of(' ');
            if (lastSpace != std::string::npos) {
                lines.push_back(currentLine.substr(0, lastSpace));
                currentLine = currentLine.substr(lastSpace + 1);
            } else {
                // No space found, force break
                lines.push_back(currentLine);
                currentLine.clear();
            }
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}
```

---

## Summary

This debug UI layout provides:

✅ **Clear Visual Hierarchy** - Headers, content, separators clearly distinct
✅ **Consistent Color Coding** - Same colors mean same things everywhere
✅ **Non-Intrusive Design** - Semi-transparent, can see game underneath
✅ **Readable Text** - Shadows, high contrast, appropriate sizing
✅ **Organized Layout** - Everything has its place, nothing overlaps badly
✅ **Professional Appearance** - Looks like a real dev tool, not placeholder UI

All constants and colors are defined in one place for easy tweaking. The layout scales reasonably well to different resolutions (test at 1280x720, 1920x1080).

**Implementation Priority**:

1. **Week 0.5**: Basic overlay + console skeleton
2. **Weeks 1-6**: HP bars, grid overlay, status panel
3. **Weeks 7-12**: Collision boxes, action point bars
4. **Weeks 13-20**: AI visualization, status effect badges

This polished debug UI will make development feel professional and efficient!
