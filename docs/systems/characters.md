# Dragonic Tactics - Complete Technical Specification

## Document Purpose

This document provides the COMPLETE technical implementation specification for Dragonic Tactics, with:

1. **Line-by-line reasoning** based on dragonic_tactics.md game design document
2. **Proper CS230::GameObject inheritance** following Cat.h/Cat.cpp patterns
3. **Detailed descriptions** for all engine services, components, and systems
4. **Interface method explanations** with purpose and usage
5. **Factory pattern usage examples** with concrete code
6. **🆕 Debug system integration** for rapid development and testing

---

## 🔧 Debug Tools Integration

**IMPORTANT**: This project includes comprehensive debug tools for efficient development. See dedicated documentation:

- **[DEBUG_TOOLS_SPEC.md](DEBUG_TOOLS_SPEC.md)** - Complete debug system specification (visual tools, console commands, god mode, hot reloading)
- **[DEBUG_COMMAND_EXAMPLES.md](DEBUG_COMMAND_EXAMPLES.md)** - Full implementation examples for all console commands
- **[DEBUG_UI_LAYOUT.md](DEBUG_UI_LAYOUT.md)** - Visual layout and styling for debug overlays

**Debug System Features**:

- ✅ Console command system (50+ commands: spawn, kill, teleport, damage, heal, etc.)
- ✅ Hot reloading for JSON data files
- ✅ God mode (invincibility, infinite resources, instant kill)
- ✅ Visual debug overlays (grid, collision boxes, AI paths, HP bars, status effects)
- ✅ Debug UI panel (character info, turn order, performance metrics)

**Implementation Timeline**: Debug tools are implemented in **Week 0.5** (before Phase 1) and expanded throughout development alongside gameplay features.

---

## Table of Contents

- [1. Character System - Proper GameObject Implementation](#1-character-system---proper-gameobject-implementation)
- [2. Singleton Services - Detailed Descriptions](#2-singleton-services---detailed-descriptions)
- [3. GameState Components - Battle Systems](#3-gamestate-components---battle-systems)
- [4. GameObject Components - Character Behaviors](#4-gameobject-components---character-behaviors)
- [5. Interface Design - Method Documentation](#5-interface-design---method-documentation)
- [6. Factory Pattern - Complete Usage Examples](#6-factory-pattern---complete-usage-examples)
- [7. Debug System Integration](#7-debug-system-integration)

---

## 1. Character System - Proper GameObject Implementation

### 1.1 Base Character Class

**Design Reasoning from dragonic_tactics.md:**

- **Line 17-40**: All characters share HP, Speed, Attack, Defense, Action Points, Spell Slots
- **Line 18-19**: HP has max/current, death at 0 HP makes character immobile
- **Line 21-22**: Speed determines turn order AND movement range
- **Line 23-24**: Attack = Base Attack + Dice Roll (e.g., 3d6)
- **Line 24-26**: Defense reduces incoming damage, minimum 0 damage
- **Line 27-28**: Action Points consumed by actions (not movement), refresh each turn
- **Line 29-32**: Spell Slots have levels, support upcasting, never regenerate in battle
- **Line 34-37**: Facing direction stored as enum (North/South/East/West)
- **Line 38-40**: Attack range determines targetable tiles

```cpp
// Character.h - Base class for all battlefield characters
// REASONING: Extends CS230::GameObject following Cat.h pattern (lines 22-29)
#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/Sprite.h"
#include "../Engine/Collision.h"
#include "GameObjectTypes.h"

// Forward declarations
enum class ActionType;
enum class Direction;

class Character : public CS230::GameObject {
public:
    // ===== REQUIRED: CS230::GameObject Pure Virtual Function Overrides =====
    // REASONING: GameObject.h line 30-31 defines these as pure virtual, MUST implement

    virtual GameObjectTypes Type() override = 0;
    // Returns specific character type enum (Dragon, Fighter, Cleric, Wizard, Rogue)
    // REASONING: GameObject requires this for collision detection and type checking
    // USAGE: Used by collision system, factory pattern, AI targeting

    virtual std::string TypeName() override = 0;
    // Returns human-readable name for logging and debugging
    // REASONING: GameObject requires this for debug output and error messages
    // USAGE: Logger outputs, debug UI, crash reports

    // ===== REQUIRED: GameObject Virtual Function Overrides =====
    // REASONING: Following Cat.h pattern (lines 26-32)

    virtual void Update(double dt) override;
    // Updates character state, action points, status effects each frame
    // REASONING: Cat.cpp line 28-47 shows pattern: call base Update, then custom logic
    // USAGE: Called by GameObjectManager every frame with delta time

    virtual void Draw(Math::TransformationMatrix camera_matrix) override;
    // Renders character sprite with camera transformation
    // REASONING: Cat.cpp line 50-53 shows conditional drawing based on hurt_timer
    // USAGE: Called by GameObjectManager during render phase

    virtual bool CanCollideWith(GameObjectTypes other_object_type) override;
    // Determines if this character can collide with another object type
    // REASONING: Cat.cpp line 56-62 filters out Particle collisions
    // USAGE: Collision system calls before expensive collision tests

    virtual void ResolveCollision(GameObject* other_object) override;
    // Handles collision response with another object
    // REASONING: Cat.cpp line 64-131 shows complex collision resolution per type
    // USAGE: Called when collision detected, must handle position correction

    // ===== Character-Specific Combat Interface =====
    // REASONING: dragonic_tactics.md line 88-94 - Characters perform actions

    virtual void TakeDamage(int damage);
    // Apply damage after armor/resistance calculations, update HP
    // REASONING: Line 25-26 - Damage calculation: FinalDamage = max(0, Attack - Defense)
    // USAGE: CombatSystem calls this when character hit by attack/spell

    virtual void PerformAction(ActionType action) = 0;
    // Execute combat action (attack, spell, move, etc.)
    // REASONING: Line 90-94 - Actions consume action points and have effects
    // USAGE: TurnManager calls when player/AI selects action

    virtual bool CanPerformAction(ActionType action) const = 0;
    // Validate if action is possible (range, resources, line-of-sight)
    // REASONING: Line 27-28 - Must check action point availability before action
    // USAGE: UI system highlights valid actions, AI filters decision tree

    // ===== Turn-Based System Accessors =====
    // REASONING: dragonic_tactics.md line 27-28, 21-22 - Action economy and movement

    virtual int GetActionPoints() const;
    // Returns current action points remaining this turn
    // REASONING: Line 27 - Action points refresh each turn, not carried over
    // USAGE: UI displays remaining actions, AI evaluates possible moves

    virtual int GetMovementRange() const;
    // Returns maximum tiles this character can move per turn (equals speed)
    // REASONING: Line 21-22 - Speed = turn order priority AND movement range
    // USAGE: GridSystem calculates reachable tiles, pathfinding range

    virtual int GetSpeed() const;
    // Returns speed stat for initiative order calculation
    // REASONING: Line 21, 49 - Speed determines turn order at battle start
    // USAGE: TurnManager sorts characters by speed descending

    // ===== Health System =====
    // REASONING: dragonic_tactics.md line 18-19 - HP with max/current, death mechanics

    virtual int GetCurrentHP() const { return currentHP; }
    // Returns current health points
    // REASONING: Line 18 - Current HP tracked separately from max
    // USAGE: UI health bars, death checks, healing calculations

    virtual int GetMaxHP() const { return maxHP; }
    // Returns maximum health points
    // REASONING: Line 18 - Max HP defines healing cap
    // USAGE: Heal spell calculations, percentage-based effects

    virtual bool IsAlive() const { return currentHP > 0; }
    // Quick check if character is alive (HP > 0)
    // REASONING: Line 18-19 - HP=0 means immobile/inactive
    // USAGE: Turn manager skips dead characters, victory condition checks

    // ===== Grid Positioning =====
    // REASONING: dragonic_tactics.md 8x8 grid battlefield

    virtual Math::vec2 GetGridPosition() const;
    // Returns discrete grid coordinates (0-7, 0-7) on battlefield
    // REASONING: All tactical combat uses tile-based positioning
    // USAGE: Movement validation, attack range calculation, pathfinding

    virtual void SetGridPosition(Math::vec2 pos);
    // Sets grid position and updates world position
    // REASONING: Movement system needs to update both grid and world coords
    // USAGE: MovementSystem calls after validating path

    // ===== Combat Stats =====
    // REASONING: dragonic_tactics.md line 23-26 - Attack and defense mechanics

    virtual int GetAttackPower() const;
    // Returns attack power (Base + Dice Roll)
    // REASONING: Line 23 - Attack = Base Attack + Variable Dice (e.g., 5 + 2d6)
    // USAGE: CombatSystem rolls dice and adds to base attack

    virtual int GetDefensePower() const;
    // Returns defense power (Base + Dice Roll)
    // REASONING: Line 24 - Defense works same as attack but lower values
    // USAGE: CombatSystem subtracts from incoming damage

    // ===== Direction System =====
    // REASONING: dragonic_tactics.md line 34-37 - Facing affects combat

    Direction GetFacingDirection() const { return facing; }
    // Returns which cardinal direction character is facing
    // REASONING: Line 34 - Stored as North/South/East/West enum
    // USAGE: Rogue flanking mechanics, AI positioning

    void SetFacingDirection(Direction dir) { facing = dir; }
    // Updates facing direction based on last action
    // REASONING: Line 34-36 - Updates on movement or attacking
    // USAGE: Combat system calls after attacks, movement system after moves

protected:
    // ===== Character State Machine =====
    // REASONING: Following Cat.h pattern (lines 44-114) - State machine for behavior

    // IMPORTANT: Unlike Cat which has movement states (Idle, Running, Jumping),
    // tactical combat characters have turn-based states (WaitingTurn, Acting, Dead)

    class State_WaitingTurn : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        // Enter when character's turn ends, disable input/AI

        virtual void Update(GameObject* object, double dt) override;
        // Just update animations while waiting

        virtual void CheckExit(GameObject* object) override;
        // Exit when TurnManager activates this character

        std::string GetName() override { return "WaitingTurn"; }
    };
    State_WaitingTurn state_waiting_turn;

    class State_Acting : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        // Refresh action points, enable input/AI decision making

        virtual void Update(GameObject* object, double dt) override;
        // Process player input or AI decision tree

        virtual void CheckExit(GameObject* object) override;
        // Exit when all action points spent or player chooses "End Turn"

        std::string GetName() override { return "Acting"; }
    };
    State_Acting state_acting;

    class State_Dead : public CS230::GameObject::State {
    public:
        virtual void Enter(GameObject* object) override;
        // Play death animation, disable collisions, become corpse

        virtual void Update(GameObject* object, double dt) override;
        // Remain inactive, just render corpse sprite

        virtual void CheckExit(GameObject* object) override;
        // Never exit (unless resurrection mechanic added)

        std::string GetName() override { return "Dead"; }
    };
    State_Dead state_dead;

    // ===== Character Stats (Data-Driven from JSON via DataRegistry) =====
    // REASONING: dragonic_tactics.md line 115-266 defines all character stats

    int maxHP;              // Line 117, 152, 179, 207, 236 - Max health per character type
    int currentHP;          // Tracked separately, starts at maxHP
    int speed;              // Line 119, 154, 180, 209, 238 - Initiative and movement
    int maxActionPoints;    // Line 121, 156, 183, 211, 240 - Actions per turn
    int currentActionPoints;// Refreshed each turn
    int baseAttackPower;    // Line 123, 158, 185, 213, 242 - Constant attack component
    std::string attackDice; // "3d6", "2d6", etc. - Variable attack component
    int baseDefensePower;   // Line 125, 160, 187, 215, 244 - Constant defense component
    std::string defenseDice;// "2d8", "1d10", etc. - Variable defense component
    int attackRange;        // Line 141, 170, 201, 227, 254 - Attack reach in tiles
    Direction facing;       // Current facing direction (N/S/E/W)

    // Spell Slots (Map: level -> remaining uses)
    // REASONING: Line 127-137, 162-166, 189-194, 217-222, 246-250 - Spell slot levels
    std::map<int, int> spellSlots; // e.g., {1: 4, 2: 3, 3: 2} for Dragon level slots
};
```

### 1.2 Dragon (Player Character) Implementation

**Design Reasoning from dragonic_tactics.md:**

- **Line 115-137**: Dragon stats (140 HP, speed 5, 2 actions, 3d6 attack, 2d8 defense)
- **Line 127-137**: Spell slots: 1st(4), 2nd(3), 3rd(2), 4th(2), 5th(1)
- **Line 141**: Attack range 3 tiles
- **Line 96-109**: Dragon spell list with 9 unique spells
- **Line 272-273**: Dragon attacks in straight lines (queen movement from chess)

```cpp
// Dragon.h - Player-controlled dragon character
// REASONING: Powerful spellcaster with area control (dragonic_tactics.md line 77-78)

#pragma once
#include "Character.h"
#include "SpellTypes.h"

class Dragon : public Character {
public:
    // Constructor: Initialize dragon at starting position
    // REASONING: Dragon placed by player at level start
    Dragon(Math::vec2 startPosition);

    // ===== REQUIRED: Pure Virtual Function Implementations =====

    GameObjectTypes Type() override { return GameObjectTypes::Dragon; }
    // REASONING: Following Cat.h line 28 pattern for type identification

    std::string TypeName() override { return "Dragon"; }
    // REASONING: Following Cat.h line 29 pattern for debug strings

    // ===== Character Interface Implementation =====

    void PerformAction(ActionType action) override;
    // Execute dragon-specific actions (melee attack, spell casting, movement)
    // REASONING: Line 139-143 - Dragon can attack (3 tiles) or cast spells
    // IMPLEMENTATION: Switch on action type, call appropriate method

    bool CanPerformAction(ActionType action) const override;
    // Check if dragon can perform action (action points, spell slots, range)
    // REASONING: Line 27-28 - Must verify action point cost before acting
    // IMPLEMENTATION: Check currentActionPoints >= action cost

    // ===== Dragon-Specific Spell Casting =====
    // REASONING: Line 96-109 - Dragon has 9 unique spells at different levels

    void CastSpell(SpellType spell, Math::vec2 target);
    // Cast dragon spell at target location, consume spell slot and action point
    // REASONING: Line 94 - Casting costs 1 action + spell slot of appropriate level
    // IMPLEMENTATION:
    //   1. Verify spell slot available (CanCastSpell)
    //   2. Consume spell slot at spell's level
    //   3. Apply spell effect via SpellSystem
    //   4. Consume 1 action point

    bool CanCastSpell(SpellType spell) const;
    // Check if dragon has spell slot for this spell
    // REASONING: Line 29-32 - Spell slots don't regenerate, must check availability
    // IMPLEMENTATION: Check spellSlots[spell.level] > 0

    bool CanUpcast(SpellType spell, int targetLevel) const;
    // Check if dragon can upcast spell to higher slot level
    // REASONING: Line 30-31 - Can use higher level slot for lower level spell
    // IMPLEMENTATION: Check spellSlots[targetLevel] > 0 && targetLevel > spell.level

private:
    // ===== Dragon Spell List =====
    // REASONING: Line 96-109 - Complete dragon spell list with effects

    // Level 1 Spells (4 slots available)
    void Spell_CreateWall(Math::vec2 target);          // Line 100 - 2x1 wall, range 5
    void Spell_Fireball(Math::vec2 target, int level); // Line 101 - 2d8 damage, upcast +1d6/level

    // Level 2 Spells (3 slots available)
    void Spell_LavaPool(Math::vec2 target, int level);    // Line 102 - 2x2 lava, 8 burn damage/turn, 5 turns
    void Spell_DragonRoar(int level);                     // Line 103 - AOE fear, range 3
    void Spell_TailSwipe();                               // Line 104 - 1d8 + knockback 2 tiles, behind only

    // Level 3 Spells (2 slots available)
    void Spell_DragonWrath(Math::vec2 target, int level); // Line 105 - 4d6, hits first in line, upcast +2d6
    void Spell_MeteorStrike(bool evenTiles, int level);   // Line 106 - 3d20 AOE, skip next turn

    // Level 4 Spells (2 slots available)
    void Spell_HeatAbsorb();                              // Line 107 - Remove all burns/lava, gain stats

    // Level 5 Spells (1 slot available)
    void Spell_HeatRelease();                             // Line 108 - Self-damage 50% HP, restore 1st-3rd slots

    // ===== Dragon Stats Initialization =====
    // REASONING: Line 115-137 - Dragon stat block
    void InitializeDragonStats();
    // Sets: maxHP=140, speed=5, maxActionPoints=2, attack=3d6, defense=2d8,
    //       spellSlots={1:4, 2:3, 3:2, 4:2, 5:1}, attackRange=3
};
```

### 1.3 Fighter (AI Tank) Implementation

**Design Reasoning from dragonic_tactics.md:**

- **Line 147-170**: Fighter stats (90 HP, speed 3, 2 actions, 5+2d6 attack, 1d10 defense)
- **Line 80-81**: Fighter role - balanced stats, engage dragon, aggro management
- **Line 162-166**: Spell slots: 1st(2), 2nd(2)

```cpp
// Fighter.h - AI tank character
// REASONING: Front-line aggro management (dragonic_tactics.md line 80-81, 147-170)

#pragma once
#include "Character.h"
#include "AIBehavior.h"

class Fighter : public Character {
public:
    Fighter(Math::vec2 startPosition);

    // ===== REQUIRED: Pure Virtual Implementations =====
    GameObjectTypes Type() override { return GameObjectTypes::Fighter; }
    std::string TypeName() override { return "Fighter"; }

    // ===== Character Interface =====
    void PerformAction(ActionType action) override;
    bool CanPerformAction(ActionType action) const override;

    // ===== Fighter AI Behavior =====
    // REASONING: Line 80-81 - Fighter role is "engage dragon, manage aggro"

    void UpdateAI(double dt);
    // Fighter AI logic: Delegates decision-making to AISystem GSComponent
    // REASONING: Fighter is tank class, must protect backline (Wizard/Cleric)
    //
    // IMPORTANT INTERACTION WITH AISystem:
    // - UpdateAI() is called by State_Acting::Update() when it's Fighter's turn
    // - Fighter does NOT make decisions directly - it asks AISystem for guidance
    // - AISystem::DecideAction(this) evaluates all options and returns best action
    // - Fighter executes the action returned by AISystem
    //
    // IMPLEMENTATION:
    //   void Fighter::UpdateAI(double dt) {
    //       AISystem* aiSys = Engine::GetGameStateManager().GetGSComponent<AISystem>();
    //       ActionDecision decision = aiSys->DecideAction(this);  // Ask AISystem
    //       ExecuteDecision(decision);  // Do what AISystem says
    //   }
    //
    // WHY THIS DESIGN?
    // - Separates AI strategy (AISystem) from character behavior (Fighter)
    // - AISystem can coordinate multiple characters for team tactics
    // - Easy to swap AI personalities without changing Fighter class
    // - AI difficulty scaling happens in AISystem, not in each character

private:
    // ===== Fighter-Specific State Machine =====
    // REASONING: Following Cat.h state pattern but for tactical combat

    class State_Engaging : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        // Move toward dragon to close distance

        void Update(GameObject* object, double dt) override;
        // Calculate path to dragon, execute movement

        void CheckExit(GameObject* object) override;
        // Exit when in melee range of dragon

        std::string GetName() override { return "Engaging"; }
    };
    State_Engaging state_engaging;

    class State_Tanking : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        // Position in front of dragon, ready to attack

        void Update(GameObject* object, double dt) override;
        // Attack dragon if action points available

        void CheckExit(GameObject* object) override;
        // Exit if dragon moves out of range

        std::string GetName() override { return "Tanking"; }
    };
    State_Tanking state_tanking;

    // AI Memory
    Math::vec2 lastKnownDragonPosition;  // Track dragon location
    int turnsSinceDragonSeen;            // Timeout for search behavior
};
```

### 1.4 Cleric (AI Healer) Implementation

**Design Reasoning from dragonic_tactics.md:**

- **Line 174-201**: Cleric stats (90 HP, speed 2, 1 action, 5+1d6 attack, 2d6 defense)
- **Line 83-85**: Cleric role - healing, buffs, makes battles longer/harder
- **Line 201**: Heal ability - 2 tile range, heals for attack power amount

```cpp
// Cleric.h - AI support/healer character
// REASONING: Healing and buff management (dragonic_tactics.md line 83-85, 174-201)

#pragma once
#include "Character.h"

class Cleric : public Character {
public:
    Cleric(Math::vec2 startPosition);

    // ===== REQUIRED: Pure Virtual Implementations =====
    GameObjectTypes Type() override { return GameObjectTypes::Cleric; }
    std::string TypeName() override { return "Cleric"; }

    // ===== Character Interface =====
    void PerformAction(ActionType action) override;
    bool CanPerformAction(ActionType action) const override;

    // ===== Cleric-Specific Abilities =====
    // REASONING: Line 201 - Cleric has unique heal action

    void HealAlly(Character* target);
    // Heal ally for attack power amount (5+1d6)
    // REASONING: Line 201 - "전방 2칸 내의 아군에게 공격력만큼 회복"
    // IMPLEMENTATION:
    //   1. Check target in range (2 tiles forward)
    //   2. Check target is ally (Fighter/Wizard/Rogue)
    //   3. Roll healing dice (attack power)
    //   4. Add healing to target HP (capped at maxHP)
    //   5. Consume 1 action point

    // ===== Cleric AI Behavior =====
    // REASONING: Line 84 - "전투를 길게 끌수록 위협적" - prioritize healing

    void UpdateAI(double dt);
    // Cleric AI logic: Delegates to AISystem, which prioritizes healing
    // REASONING: Cleric makes battles longer by sustaining team
    //
    // IMPORTANT INTERACTION WITH AISystem:
    // - UpdateAI() called when it's Cleric's turn
    // - AISystem::DecideAction(this) evaluates heal vs attack options
    // - AISystem checks all allies' HP and chooses lowest HP target
    // - If ally below 50% HP: Return ActionDecision{Heal, lowestHPAlly}
    // - If all allies healthy: Return ActionDecision{Attack, dragon}
    //
    // IMPLEMENTATION:
    //   void Cleric::UpdateAI(double dt) {
    //       AISystem* aiSys = Engine::GetGameStateManager().GetGSComponent<AISystem>();
    //       ActionDecision decision = aiSys->DecideAction(this);
    //
    //       if (decision.action == ActionType::Heal) {
    //           HealAlly(decision.target);
    //       } else {
    //           AttackTarget(decision.target);
    //       }
    //   }
    //   2. If wounded ally found: Move to heal range, cast heal
    //   3. If all allies healthy: Support fighter by attacking dragon

private:
    // ===== Cleric-Specific State Machine =====

    class State_Healing : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Healing"; }
    };
    State_Healing state_healing;

    class State_Supporting : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Supporting"; }
    };
    State_Supporting state_supporting;

    // AI Decision Data
    Character* healTarget;  // Who to heal this turn
    int healPriority;       // Urgency score (based on HP%)
};
```

### 1.5 Wizard (AI Glass Cannon) Implementation

**Design Reasoning from dragonic_tactics.md:**

- **Line 203-228**: Wizard stats (55 HP, speed 1, 1 action, 2d10 attack, 1d6 defense)
- **Line 82-83**: Wizard role - low HP/defense, long range (6 tiles), high damage
- **Line 227**: Attack range 6 tiles (longest in game)

```cpp
// Wizard.h - AI ranged damage dealer
// REASONING: Glass cannon - high damage, low survivability (line 82-83, 203-228)

#pragma once
#include "Character.h"

class Wizard : public Character {
public:
    Wizard(Math::vec2 startPosition);

    // ===== REQUIRED: Pure Virtual Implementations =====
    GameObjectTypes Type() override { return GameObjectTypes::Wizard; }
    std::string TypeName() override { return "Wizard"; }

    // ===== Character Interface =====
    void PerformAction(ActionType action) override;
    bool CanPerformAction(ActionType action) const override;

    // ===== Wizard AI Behavior =====
    // REASONING: Line 82-83 - "원거리에서 공격" - maintain distance, maximize damage

    void UpdateAI(double dt);
    // Wizard AI logic: Stay at max range, attack dragon, retreat if approached
    // REASONING: 55 HP + 1d6 defense = extremely fragile, MUST avoid melee
    // IMPLEMENTATION:
    //   1. Check distance to dragon
    //   2. If range < 4: RETREAT (too close for safety)
    //   3. If range 4-6: ATTACK (optimal range)
    //   4. If range > 6: ADVANCE (get in attack range)
    //   5. Priority: Survival > Damage

private:
    // ===== Wizard-Specific State Machine =====

    class State_Positioning : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Positioning"; }
    };
    State_Positioning state_positioning;

    class State_Attacking : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Attacking"; }
    };
    State_Attacking state_attacking;

    class State_Retreating : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Retreating"; }
    };
    State_Retreating state_retreating;

    // AI Safety Parameters
    static constexpr int SAFE_DISTANCE = 4;   // Minimum tiles from dragon
    static constexpr int OPTIMAL_RANGE = 6;   // Preferred attack range
};
```

### 1.6 Rogue (AI Assassin) Implementation

**Design Reasoning from dragonic_tactics.md:**

- **Line 231-266**: Rogue stats (65 HP, speed 4, 1 action, 5+1d8 attack, 1d6 defense)
- **Line 85-86**: Rogue role - high speed/mobility, flank dragon's sides/back
- **Line 254, 258-264**: Special abilities - Ambush (8+2d6), Sprint (2x speed), Stealth, Smoke Bomb

```cpp
// Rogue.h - AI stealth assassin
// REASONING: High mobility flanker (dragonic_tactics.md line 85-86, 231-266)

#pragma once
#include "Character.h"

class Rogue : public Character {
public:
    Rogue(Math::vec2 startPosition);

    // ===== REQUIRED: Pure Virtual Implementations =====
    GameObjectTypes Type() override { return GameObjectTypes::Rogue; }
    std::string TypeName() override { return "Rogue"; }

    // ===== Character Interface =====
    void PerformAction(ActionType action) override;
    bool CanPerformAction(ActionType action) const override;

    // ===== Rogue-Specific Abilities =====
    // REASONING: Line 254-266 - Rogue has unique stealth mechanics

    void Ambush(Character* target);
    // Flanking attack from side/rear for extra damage (8+2d6 instead of 8+1d6)
    // REASONING: Line 258-259 - "기습: (은신상태이거나 적의 옆이나 뒤에서) 8+2d6만큼의 피해를 줌"
    // IMPLEMENTATION:
    //   1. Check if stealthed OR behind/beside target
    //   2. Deal 8 + 2d6 damage (vs normal 5 + 1d8)
    //   3. Break stealth if active
    //   4. Consume 1 action point

    void Sprint();
    // Double movement speed this turn
    // REASONING: Line 261 - "달리기: 속력을 2배로 함"
    // IMPLEMENTATION: speed *= 2 for this turn only

    void Stealth();
    // Become untargetable until attacking (costs 1st level spell slot)
    // REASONING: Line 263 - "은신: (1레벨 주문 소모) 공격하기 전까지 대상 지정이 안됨"
    // IMPLEMENTATION: Set stealthed flag, consume spell slot, disable targeting

    void SmokeBomb(Math::vec2 target);
    // 2x2 AOE that grants stealth to all inside (costs 2nd level spell slot)
    // REASONING: Line 265-266 - "연막: (2레벨 주문 소모) 2칸 내의 2×2 크기의 연막탄"
    // IMPLEMENTATION: Create smoke zone, grant stealth to allies in area

    // ===== Rogue AI Behavior =====
    // REASONING: Line 85-86 - "드래곤의 측면을 노려 기습 공격을 노림"

    void UpdateAI(double dt);
    // Rogue AI logic: Delegates to AISystem for tactical flanking decisions
    // REASONING: Speed 4 allows rapid repositioning for optimal flanking
    //
    // IMPORTANT INTERACTION WITH AISystem:
    // - UpdateAI() called when it's Rogue's turn
    // - AISystem::FindOptimalPosition(this, dragon) calculates best flank spot
    // - AISystem checks dragon's facing direction and Fighter's position
    // - Returns position behind/beside dragon for ambush damage
    // - If Rogue HP < 30%: AISystem prioritizes Stealth action
    //
    // IMPLEMENTATION:
    //   void Rogue::UpdateAI(double dt) {
    //       AISystem* aiSys = Engine::GetGameStateManager().GetGSComponent<AISystem>();
    //
    //       // Low HP? Use stealth to escape
    //       if (GetCurrentHP() < GetMaxHP() * 0.3f) {
    //           Stealth();
    //           return;
    //       }
    //
    //       // Ask AISystem for optimal flanking position
    //       Dragon* dragon = FindDragon();
    //       Math::vec2 flankPos = aiSys->FindOptimalPosition(this, dragon);
    //
    //       // If not in position, move there
    //       if (GetGridPosition() != flankPos) {
    //           MoveToPosition(flankPos);
    //       } else {
    //           // In position - ambush!
    //           Ambush(dragon);
    //       }
    //   }

private:
    // ===== Rogue-Specific State Machine =====

    class State_Flanking : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Flanking"; }
    };
    State_Flanking state_flanking;

    class State_Ambushing : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Ambushing"; }
    };
    State_Ambushing state_ambushing;

    class State_Stealthed : public CS230::GameObject::State {
    public:
        void Enter(GameObject* object) override;
        void Update(GameObject* object, double dt) override;
        void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Stealthed"; }
    };
    State_Stealthed state_stealthed;

    // Stealth State
    bool isStealthed;       // Currently invisible to targeting
    int turnsSinceVisible;  // Time in stealth for AI evaluation
};
```

---

## 2. Character-AISystem Interaction Flow

**CRITICAL ARCHITECTURE CONCEPT**: Character classes (`Fighter`, `Cleric`, `Wizard`, `Rogue`) do NOT contain complex AI logic themselves. Instead, they **delegate** decision-making to the `AISystem` GSComponent.

### 2.1 Why This Separation of Concerns?

**Problem without AISystem**:

- Each character would need duplicate AI code (pathfinding, target selection, threat assessment)
- No team coordination - each character acts independently
- Difficulty scaling requires editing 4 character classes
- AI personality changes mean recompiling character code

**Solution with AISystem**:

- ✅ **Single Source of Truth**: All AI logic centralized in one GSComponent
- ✅ **Team Coordination**: AISystem sees all characters and coordinates tactics
- ✅ **Easy Difficulty Scaling**: Change one variable in AISystem
- ✅ **Data-Driven AI**: Swap AI personalities via JSON without code changes

### 2.2 Complete Turn Execution Flow

Here's the full sequence when an AI character takes a turn:

```cpp
// ===== STEP 1: TurnManager activates character's turn =====
// In TurnManager.cpp

void TurnManager::Update(double dt) {
    Character* currentCharacter = turnOrder[currentTurnIndex];

    // Activate character by transitioning to State_Acting
    currentCharacter->change_state(&currentCharacter->state_acting);

    // State_Acting::Update() will call character's UpdateAI()
}

// ===== STEP 2: Character's State_Acting calls UpdateAI() =====
// In Fighter.cpp (same pattern for Cleric, Wizard, Rogue)

void Fighter::State_Acting::Update(GameObject* object, double dt) {
    Fighter* fighter = static_cast<Fighter*>(object);

    // Check if AI-controlled (not player)
    if (fighter->Type() != GameObjectTypes::Dragon) {
        fighter->UpdateAI(dt);  // Delegate to AI
    } else {
        // Player character - wait for input
        fighter->ProcessPlayerInput();
    }
}

// ===== STEP 3: Character's UpdateAI() asks AISystem for guidance =====
// In Fighter.cpp

void Fighter::UpdateAI(double dt) {
    // Get AISystem GSComponent
    AISystem* aiSys = Engine::GetGameStateManager().GetGSComponent<AISystem>();

    // Ask AISystem: "What should I do?"
    ActionDecision decision = aiSys->DecideAction(this);

    // Execute the decision AISystem returned
    ExecuteDecision(decision);
}

// ===== STEP 4: AISystem evaluates options and returns decision =====
// In AISystem.cpp

ActionDecision AISystem::DecideAction(Character* aiCharacter) {
    // AISystem knows about ALL characters and game state
    Dragon* dragon = FindDragon();
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    // Different logic per character type
    switch (aiCharacter->Type()) {
        case GameObjectTypes::Fighter:
            return DecideFighterAction(aiCharacter, dragon, grid);

        case GameObjectTypes::Cleric:
            return DecideClericAction(aiCharacter, dragon, grid);

        case GameObjectTypes::Wizard:
            return DecideWizardAction(aiCharacter, dragon, grid);

        case GameObjectTypes::Rogue:
            return DecideRogueAction(aiCharacter, dragon, grid);
    }
}

ActionDecision AISystem::DecideFighterAction(Character* fighter, Dragon* dragon, GridSystem* grid) {
    // Calculate distance to dragon
    int distance = grid->ManhattanDistance(fighter->GetGridPosition(), dragon->GetGridPosition());

    // Fighter AI Strategy: Close distance and attack
    if (distance <= 1 && fighter->GetActionPoints() > 0) {
        // In melee range - attack!
        return ActionDecision{
            .action = ActionType::MeleeAttack,
            .target = dragon,
            .position = fighter->GetGridPosition()
        };
    } else {
        // Too far - move closer
        Math::vec2 optimalPos = FindOptimalPosition(fighter, dragon);
        return ActionDecision{
            .action = ActionType::Move,
            .target = nullptr,
            .position = optimalPos
        };
    }
}

ActionDecision AISystem::DecideClericAction(Character* cleric, Dragon* dragon, GridSystem* grid) {
    // Find lowest HP ally within heal range
    Character* lowestHPAlly = nullptr;
    int lowestHP = 999;

    std::vector<Character*> allies = GetAlliesInRange(cleric, 2);
    for (auto* ally : allies) {
        if (ally->GetCurrentHP() < lowestHP && ally->GetCurrentHP() < ally->GetMaxHP()) {
            lowestHP = ally->GetCurrentHP();
            lowestHPAlly = ally;
        }
    }

    // Cleric AI Strategy: Heal if needed, attack otherwise
    if (lowestHPAlly && lowestHP < lowestHPAlly->GetMaxHP() * 0.5f) {
        // Ally below 50% HP - heal them!
        return ActionDecision{
            .action = ActionType::Heal,
            .target = lowestHPAlly,
            .position = cleric->GetGridPosition()
        };
    } else {
        // Everyone healthy - attack dragon
        return ActionDecision{
            .action = ActionType::RangedAttack,
            .target = dragon,
            .position = cleric->GetGridPosition()
        };
    }
}

// ===== STEP 5: Character executes the decision =====
// In Fighter.cpp

void Fighter::ExecuteDecision(const ActionDecision& decision) {
    switch (decision.action) {
        case ActionType::Move:
            MoveToPosition(decision.position);
            ConsumeActionPoint();
            break;

        case ActionType::MeleeAttack:
            AttackTarget(decision.target);
            ConsumeActionPoint();
            break;

        case ActionType::RangedAttack:
            RangedAttackTarget(decision.target);
            ConsumeActionPoint();
            break;

        case ActionType::Heal:
            HealAlly(decision.target);
            ConsumeActionPoint();
            break;

        case ActionType::CastSpell:
            CastSpell(decision.spellType, decision.target);
            ConsumeActionPoint();
            break;
    }

    // If no action points left, end turn
    if (currentActionPoints == 0) {
        change_state(&state_waiting_turn);
        Engine::GetGameStateManager().GetGSComponent<TurnManager>()->AdvanceToNextTurn();
    }
}
```

### 2.3 ActionDecision Struct

The communication protocol between Character and AISystem:

```cpp
// In AISystem.h

enum class ActionType {
    Move,
    MeleeAttack,
    RangedAttack,
    Heal,
    CastSpell,
    Hide,
    Sprint,
    EndTurn
};

struct ActionDecision {
    ActionType action;        // What to do
    Character* target;        // Who to target (nullptr for self-targeted actions)
    Math::vec2 position;      // Where to move/cast
    SpellType spellType;      // Which spell to cast (if CastSpell)
    int priority;             // Used for action scoring (0-100)
};
```

### 2.4 Team Coordination Example

AISystem can coordinate multiple characters for combined tactics:

```cpp
// In AISystem.cpp

void AISystem::CoordinateTeamStrategy() {
    Dragon* dragon = FindDragon();

    // Check if dragon is low HP
    if (dragon->GetCurrentHP() < dragon->GetMaxHP() * 0.3f) {
        // Dragon below 30% HP - ALL AI go aggressive!
        Engine::GetLogger().LogInfo("AISystem: Dragon is weak! All units focus fire!");

        // Update all AI personalities
        for (auto* character : allAICharacters) {
            characterPersonalities[character] = AIPersonality::Aggressive;
        }

        // Even Cleric stops healing and attacks
        // Fighter uses damage abilities instead of tanking
        // Wizard uses highest damage spells
        // Rogue goes for risky ambush
    }
}

bool AISystem::ShouldFocusFire(Character* target) {
    // Concentrated attacks more effective than spreading damage
    if (target->GetCurrentHP() < 30) {
        return true;  // Finish off low HP targets
    }

    if (target->IsPlayer()) {
        return true;  // Always focus dragon (player)
    }

    return false;
}
```

### 2.5 Difficulty Scaling Implementation

Change AI intelligence without touching character classes:

```cpp
// In AISystem.cpp

void AISystem::SetDifficultyLevel(int level) {
    difficultyLevel = level;

    switch (level) {
        case 1:  // Easy
            reactionDelay = 1.5f;  // Slow AI turns
            mistakeChance = 0.3f;   // 30% chance of suboptimal decision
            Engine::GetLogger().LogInfo("AI Difficulty: Easy");
            break;

        case 2:  // Normal
            reactionDelay = 0.8f;
            mistakeChance = 0.1f;   // 10% mistake rate
            Engine::GetLogger().LogInfo("AI Difficulty: Normal");
            break;

        case 3:  // Hard
            reactionDelay = 0.3f;
            mistakeChance = 0.0f;   // Perfect play
            enableTeamCoordination = true;
            Engine::GetLogger().LogInfo("AI Difficulty: Hard (optimal play)");
            break;

        case 4:  // Expert
            reactionDelay = 0.3f;
            mistakeChance = 0.0f;
            enableTeamCoordination = true;
            enableAdvancedTactics = true;  // Baiting, traps, combos
            Engine::GetLogger().LogInfo("AI Difficulty: Expert");
            break;
    }
}

ActionDecision AISystem::DecideAction(Character* aiCharacter) {
    // Get all possible actions
    std::vector<ActionDecision> possibleActions = EvaluateAllActions(aiCharacter);

    // Sort by priority score
    std::sort(possibleActions.begin(), possibleActions.end(),
        [](const ActionDecision& a, const ActionDecision& b) {
            return a.priority > b.priority;
        });

    // Easy mode: Sometimes pick random action instead of best
    if (difficultyLevel == 1 && Random::Chance(mistakeChance)) {
        int randomIndex = Random::Range(0, possibleActions.size() - 1);
        return possibleActions[randomIndex];
    }

    // Normal/Hard/Expert: Pick highest priority action
    return possibleActions[0];
}
```

### 2.6 Key Takeaways

1. **Character classes are "dumb"** - They know HOW to perform actions, but not WHEN
2. **AISystem is "smart"** - It knows WHEN to do actions based on game state
3. **Clear separation**: Character = execution, AISystem = strategy
4. **Scalability**: Add new character types without touching AISystem core
5. **Testability**: Can test AI logic independently from character implementation

---
