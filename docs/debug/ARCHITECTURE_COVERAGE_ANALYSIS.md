# Debug Tools Architecture Coverage Analysis

**Date**: 2025-10-06
**Purpose**: Verify debug tools properly support all systems defined in [architecture.md](../architecture.md)

---

## Executive Summary

**Status**: ⚠️ **PARTIAL COVERAGE** - Debug tools support core functionality but are missing several key systems

### Coverage Overview

| System Type         | Total in Architecture | Covered in Debug Tools | Coverage % | Status      |
| ------------------- | --------------------- | ---------------------- | ---------- | ----------- |
| **Singletons**      | 6                     | 2-3                    | ~40%       | ⚠️ Missing  |
| **GSComponents**    | 7                     | 3-4                    | ~50%       | ⚠️ Partial  |
| **GOComponents**    | 7                     | 3                      | ~43%       | ⚠️ Missing  |
| **Character Types** | 5                     | 5                      | 100%       | ✅ Complete  |
| **Debug Features**  | N/A                   | Strong                 | N/A        | ✅ Excellent |

---

## Detailed Analysis

### 1. Singleton Services (6 Total)

#### ✅ Well-Covered Singletons (2)

1. **DataRegistry**
   
   - Debug tools: `reload` command in commands.md
   - Hot-reload functionality documented
   - File watching explained
   - **Status**: ✅ **COMPLETE**

2. **DebugManager** (itself)
   
   - Extensively documented in tools.md
   - Toggle functions, god mode, command execution
   - **Status**: ✅ **COMPLETE**

#### ⚠️ Partially Covered Singletons (1)

3. **EventBus**
   - Architecture.md: Central communication hub for all systems
   - Debug tools: Only mentioned once in commands.md
   - **Missing**:
     - No debug commands to inspect event subscriptions
     - No event logging/tracing tools
     - No way to manually publish events for testing
   - **Status**: ⚠️ **NEEDS EVENT INSPECTION TOOLS**

#### ❌ Missing Singleton Coverage (3)

4. **DiceManager**
   
   - Architecture.md: "Seeded, debuggable dice rolls"
   - Debug tools: **NOT MENTIONED** in tools.md or commands.md
   - **Missing**:
     - No `roll` command to test dice notation ("3d6", "2d8+5")
     - No `setseed` command for reproducible testing
     - No dice history viewer
   - **Status**: ❌ **CRITICAL MISSING**

5. **AIDirector**
   
   - Architecture.md: "Global AI coordination, team tactics, difficulty scaling"
   - Debug tools: Mentioned in AIDebugVisualizer but no direct commands
   - **Missing**:
     - No command to adjust AI difficulty
     - No command to inspect threat tables
     - No team coordination visualization
   - **Status**: ⚠️ **NEEDS AI INSPECTION COMMANDS**

6. **SaveManager**
   
   - Architecture.md: "Campaign persistence, save/load game state"
   - Debug tools: **NOT MENTIONED**
   - **Missing**:
     - No save/load debug commands
     - No save file inspection tools
     - No campaign state viewer
   - **Status**: ❌ **COMPLETELY MISSING**

---

### 2. GameState Components (7 Total)

#### ✅ Well-Covered GSComponents (3)

1. **GridSystem**
   
   - Debug tools: GridDebugRenderer class in tools.md
   - F1-F3 keyboard shortcuts
   - Grid overlay, coordinates, occupancy visualization
   - Console commands for pathfinding visualization
   - **Status**: ✅ **EXCELLENT COVERAGE**

2. **AISystem**
   
   - Debug tools: AIDebugVisualizer class in tools.md
   - Path visualization, target markers, threat values
   - F4-F6 keyboard shortcuts
   - **Status**: ✅ **GOOD COVERAGE**

3. **StatusEffectManager**
   
   - Debug tools: StatusInfoOverlay shows active effects
   - Status effect badges rendered
   - **Status**: ✅ **BASIC COVERAGE** (could use more commands)

#### ⚠️ Partially Covered GSComponents (2)

4. **TurnManager**
   
   - Architecture.md: "Initiative order, action point allocation"
   - Debug tools: Turn order display in StatusInfoOverlay
   - **Missing**:
     - No `endturn` command
     - No `skipturn` command
     - No initiative manipulation commands
   - **Status**: ⚠️ **NEEDS TURN CONTROL COMMANDS**

5. **BattleManager**
   
   - Architecture.md: "Victory conditions, battle flow coordination"
   - Debug tools: Mentioned in integration timeline
   - **Missing**:
     - No battle state inspection
     - No victory condition testing commands
   - **Status**: ⚠️ **MINIMAL COVERAGE**

#### ❌ Missing GSComponent Coverage (2)

6. **CombatSystem**
   
   - Architecture.md: "Damage calculation, attack resolution, knockback"
   - Debug tools: CollisionDebugRenderer mentioned but not detailed
   - **Missing**:
     - No damage calculation testing
     - No combat formula inspection
     - No knockback visualization
   - **Status**: ❌ **NEEDS COMBAT DEBUG TOOLS**

7. **SpellSystem**
   
   - Architecture.md: "Spell casting, effects, slot management, upcasting"
   - Debug tools: Spell slots shown in StatusInfoOverlay
   - **Missing**:
     - No `castspell` debug command
     - No spell slot manipulation
     - No spell effect visualization tools
   - **Status**: ❌ **NEEDS SPELL DEBUG COMMANDS**

---

### 3. GameObject Components (7 Total)

#### ✅ Well-Covered GOComponents (3)

1. **GridPosition**
   
   - Debug tools: Visualized by GridDebugRenderer
   - Character positions shown on grid
   - **Status**: ✅ **COMPLETE**

2. **ActionPoints**
   
   - Debug tools: StatusInfoOverlay shows AP bars
   - God mode infinite AP mentioned
   - **Status**: ✅ **GOOD COVERAGE**

3. **SpellSlots**
   
   - Debug tools: StatusInfoOverlay shows slot counts
   - God mode infinite slots mentioned
   - **Status**: ✅ **GOOD COVERAGE**

#### ❌ Missing GOComponent Coverage (4)

4. **StatusEffects**
   
   - Architecture.md: "Individual buff/debuff collection"
   - Debug tools: Visual badges shown, but no manipulation commands
   - **Missing**:
     - No command to add/remove status effects for testing
     - No effect timer manipulation
   - **Status**: ⚠️ **NEEDS EFFECT MANIPULATION COMMANDS**

5. **AIMemory**
   
   - Architecture.md: "Tactical decision history"
   - Debug tools: **NOT MENTIONED**
   - **Missing**:
     - No AI memory inspection
     - No decision history viewer
   - **Status**: ❌ **COMPLETELY MISSING**

6. **DamageCalculator**
   
   - Architecture.md: "Dice-based combat resolution"
   - Debug tools: **NOT MENTIONED**
   - **Missing**:
     - No damage formula testing
     - No dice roll history for combat
   - **Status**: ❌ **COMPLETELY MISSING**

7. **TargetingSystem**
   
   - Architecture.md: "Attack/spell range validation"
   - Debug tools: Attack range overlay mentioned briefly
   - **Missing**:
     - No targeting validation testing
     - No range calculation inspection
   - **Status**: ⚠️ **MINIMAL COVERAGE**

---

### 4. Character Types (5 Total)

#### ✅ All Character Types Covered

1. **Dragon** - Referenced throughout debug tools ✅
2. **Fighter** - Mentioned in examples ✅
3. **Cleric** - Mentioned in examples ✅
4. **Wizard** - Mentioned in examples ✅
5. **Rogue** - Mentioned in examples ✅

**Status**: ✅ **COMPLETE** - All character types can be spawned and debugged

---

## Critical Missing Features

### Priority 1: Must Have for Development

1. **DiceManager Debug Commands** ❌
   
   ```
   roll 3d6                  # Test dice notation
   setseed 12345             # Reproducible testing
   showlastrolls             # Dice history
   ```
   
   **Impact**: Cannot test damage calculations or RNG behavior

2. **SpellSystem Debug Commands** ❌
   
   ```
   castspell dragon fireball 1 6 4    # Test spell casting
   spellslots dragon                   # Inspect slot counts
   addspellslot dragon 1               # Restore slots for testing
   ```
   
   **Impact**: Cannot test spells without playing full turns

3. **CombatSystem Debug Tools** ❌
   
   ```
   testdamage dragon fighter    # Calculate damage without execution
   showformula dragon fighter   # Display combat formula
   ```
   
   **Impact**: Cannot debug damage calculations

### Priority 2: Highly Recommended

4. **EventBus Inspection** ⚠️
   
   ```
   showevents                   # List all event types
   traceevent DamageTakenEvent  # Log specific event
   publishevent <event> <data>  # Manual event testing
   ```
   
   **Impact**: Difficult to debug event-driven systems

5. **TurnManager Commands** ⚠️
   
   ```
   endturn              # Force end current turn
   skipturn fighter     # Skip character's turn
   setinitiative        # Manually set turn order
   ```
   
   **Impact**: Slow iteration when testing turn-based mechanics

6. **StatusEffect Manipulation** ⚠️
   
   ```
   addeffect dragon burn 3      # Add burn for 3 turns
   removeeffect dragon burn     # Remove effect
   showeffects dragon           # List active effects
   ```
   
   **Impact**: Cannot test status effects without complex setups

### Priority 3: Nice to Have

7. **SaveManager Commands** ⚠️
   
   ```
   quicksave slot1       # Debug save
   quickload slot1       # Debug load
   listsaves             # Show all saves
   ```
   
   **Impact**: Manual save testing is time-consuming

8. **AIMemory Inspection** ❌
   
   ```
   showmemory fighter    # Display AI decision history
   clearmemory fighter   # Reset AI knowledge
   ```
   
   **Impact**: AI debugging is trial-and-error

---

## Recommended Additions

### 1. Add Missing Debug Commands (commands.md)

Add the following sections to [commands.md](commands.md):

#### Dice Testing Commands

```cpp
// Phase 1 - DiceManager Commands (Week 5)
DebugConsole::Instance().RegisterCommand("roll",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: roll <dice notation>", ConsoleColor::Red);
            DebugConsole::Instance().AddOutput("Example: roll 3d6, roll 2d8+5", ConsoleColor::Yellow);
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
        std::string breakdown = "Individual rolls: ";
        for (int roll : lastRolls) {
            breakdown += std::to_string(roll) + " ";
        }
        DebugConsole::Instance().AddOutput(breakdown, ConsoleColor::Yellow);
    },
    "roll <notation> - Test dice rolling (e.g., roll 3d6, roll 2d8+5)"
);

DebugConsole::Instance().RegisterCommand("setseed",
    [](std::vector<std::string> args) {
        if (args.empty()) {
            DebugConsole::Instance().AddOutput("Usage: setseed <number>", ConsoleColor::Red);
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

#### Spell Testing Commands

```cpp
// Phase 1 - SpellSystem Commands (Week 6-7)
DebugConsole::Instance().RegisterCommand("castspell",
    [](std::vector<std::string> args) {
        // castspell dragon fireball 1 6 4
        if (args.size() < 5) {
            DebugConsole::Instance().AddOutput(
                "Usage: castspell <caster> <spell> <level> <targetX> <targetY>",
                ConsoleColor::Red
            );
            return;
        }

        std::string casterName = args[0];
        std::string spellName = args[1];
        int level = std::stoi(args[2]);
        int targetX = std::stoi(args[3]);
        int targetY = std::stoi(args[4]);

        Character* caster = FindCharacterByName(casterName);
        if (!caster) {
            DebugConsole::Instance().AddOutput("Character not found: " + casterName, ConsoleColor::Red);
            return;
        }

        // Cast spell through SpellSystem
        SpellSystem* spellSys = GetGSComponent<SpellSystem>();
        spellSys->CastSpellDebug(caster, spellName, level, {targetX, targetY});

        DebugConsole::Instance().AddOutput(
            casterName + " cast " + spellName + " (Level " + std::to_string(level) + ")",
            ConsoleColor::Green
        );
    },
    "castspell <caster> <spell> <level> <x> <y> - Cast a spell for testing"
);
```

#### Turn Control Commands

```cpp
// Phase 2 - TurnManager Commands (Week 7-8)
DebugConsole::Instance().RegisterCommand("endturn",
    [](std::vector<std::string> args) {
        TurnManager* turnMgr = GetGSComponent<TurnManager>();
        Character* current = turnMgr->GetActiveCharacter();

        turnMgr->EndCurrentTurn();

        DebugConsole::Instance().AddOutput(
            "Ended " + current->TypeName() + "'s turn",
            ConsoleColor::Green
        );
    },
    "endturn - Force end the current character's turn"
);

DebugConsole::Instance().RegisterCommand("showturnorder",
    [](std::vector<std::string> args) {
        TurnManager* turnMgr = GetGSComponent<TurnManager>();
        auto turnOrder = turnMgr->GetTurnOrder();

        DebugConsole::Instance().AddOutput("=== Turn Order ===", ConsoleColor::Yellow);
        for (int i = 0; i < turnOrder.size(); ++i) {
            Character* character = turnOrder[i];
            std::string marker = (i == turnMgr->GetCurrentTurnIndex()) ? " <-- CURRENT" : "";
            DebugConsole::Instance().AddOutput(
                std::to_string(i + 1) + ". " + character->TypeName() + marker,
                ConsoleColor::White
            );
        }
    },
    "showturnorder - Display current turn order and active character"
);
```

### 2. Add Missing Debug Visualizers (tools.md)

Add these sections to [tools.md](tools.md):

#### DiceManager Debug Panel

```cpp
class DiceHistoryPanel : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix) override;

    void SetVisible(bool visible);
    void LogRoll(const std::string& notation, int result, std::vector<int> individualRolls);

private:
    bool isVisible = false;
    struct RollHistory {
        std::string notation;
        int result;
        std::vector<int> individualRolls;
        double timestamp;
    };
    std::vector<RollHistory> recentRolls;  // Last 10 rolls

    void DrawRollHistory();
};
```

#### Combat Formula Inspector

```cpp
class CombatFormulaInspector : public CS230::Component {
public:
    void Draw(Math::TransformationMatrix camera_matrix) override;

    void ShowDamageCalculation(Character* attacker, Character* target);
    void ShowDefenseCalculation(Character* defender);

private:
    void DrawFormulaBreakdown();
    void DrawDiceRollDetails();
    void DrawModifierList();
};
```

---

## Action Items

### Immediate (Week 1-2)

- [ ] Add DiceManager debug commands to commands.md (roll, setseed, showlastrolls)
- [ ] Add SpellSystem debug commands to commands.md (castspell, spellslots)
- [ ] Add TurnManager debug commands to commands.md (endturn, showturnorder, skipturn)

### Short-Term (Week 3-5)

- [ ] Add EventBus inspection tools to tools.md (event tracing, subscription viewer)
- [ ] Add CombatSystem formula inspector to tools.md
- [ ] Add StatusEffect manipulation commands to commands.md

### Medium-Term (Week 6-10)

- [ ] Add AIMemory inspection tools
- [ ] Add DamageCalculator debug panel
- [ ] Add SaveManager debug commands

### Long-Term (Week 11+)

- [ ] Add performance profiling tools
- [ ] Add network debug tools (if multiplayer added)
- [ ] Add mod loading debug commands

---

## Conclusion

The debug tools documentation has **strong foundational coverage** (grid visualization, AI paths, status overlays) but is **missing critical command-line tools** for testing core gameplay systems.

**Highest Priority Missing Features:**

1. DiceManager commands (roll, setseed) - **CRITICAL** for testing combat
2. SpellSystem commands (castspell) - **CRITICAL** for testing magic
3. TurnManager commands (endturn, skipturn) - **HIGH** for iteration speed
4. EventBus inspection - **HIGH** for debugging event-driven systems

**Recommendation**: Add these missing commands to [commands.md](commands.md) and visualizers to [tools.md](tools.md) before Week 5 (when combat systems go live).

---

**Next Steps**: Create pull request with proposed additions to debug documentation.
