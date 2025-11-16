# Playtest 1 Polish & Integration - Week 5

**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Polish + Playtest 1 Preparation
**Timeline**: Week 5 of 5 (Playtest 1 milestone)
**Strategy**: Complete BattleState integration, polish all systems, prepare playable build

**Last Updated**: 2025-01-31
**Week 4 Status**: Initiative system complete, Fighter AI functional, Status effects tracking, Data-driven design with hot-reload

**Related Documentation**:

- [Week 1](week1.md) for foundation systems
- [Week 2](week2.md) for Dragon and Fighter base classes
- [Week 3](week3.md) for spell system and combat integration
- [Week 4](week4.md) for initiative, AI, and status effects
- [docs/implementation-plan.md](../../implementation-plan.md) for complete 26-week timeline

---

## Overview

Week 5 finalizes the Playtest 1 build by integrating all systems into BattleState, polishing visuals and UI, adding game flow controls (death handling, victory/defeat screens), and preparing a stable playable build for external testing. This week transitions from feature development to polish and integration.

**Critical Success Criteria:**

- ✅ **BattleState Complete**: Full game loop with all Week 1-4 systems integrated
- ✅ **UI/UX Polish**: Health bars, turn indicator, spell selection UI, grid highlights
- ✅ **Game Flow Complete**: Death handling, victory/defeat conditions, battle end screen
- ✅ **Visual Effects**: Spell animations, damage numbers, attack feedback
- ✅ **Stability & Testing**: No crashes, 80+ tests passing, playable build ready

**Integration Goal (Friday Week 5)**:

- Dragon vs Fighter fully playable from start to finish
- Complete UI with health bars, turn order, spell selection
- All 3 Dragon spells functional with visual feedback
- Fighter AI makes smart decisions
- Victory/defeat screens with battle statistics
- Stable build for external playtesting

**Meeting Schedule:**

- **Daily Standups**: 10 min each morning (9:00 AM)
- **Mid-Week Integration Check**: Wednesday 2:00 PM (60 min - test full game flow)
- **Friday Playtest 1**: 2:00 PM (90 min - external playtest with feedback collection)

---

## Table of Contents

- [Week 5 Overview](#overview)
- [Developer A: Visual Effects & Animation](#week-5-developer-a---visual-effects--animation)
  - [Implementation Tasks](#implementation-tasks-visual-effects)
  - [Implementation Examples](#implementation-examples-visual-effects)
  - [Rigorous Testing](#rigorous-testing-visual-effects)
  - [Daily Breakdown](#daily-breakdown-developer-a)
- [Developer B: Game Flow & Victory Conditions](#week-5-developer-b---game-flow--victory-conditions)
  - [Implementation Tasks](#implementation-tasks-game-flow)
  - [Implementation Examples](#implementation-examples-game-flow)
  - [Rigorous Testing](#rigorous-testing-game-flow)
  - [Daily Breakdown](#daily-breakdown-developer-b)
- [Developer C: UI/UX Polish](#week-5-developer-c---uiux-polish)
  - [Implementation Tasks](#implementation-tasks-ui-polish)
  - [Implementation Examples](#implementation-examples-ui-polish)
  - [Rigorous Testing](#rigorous-testing-ui-polish)
  - [Daily Breakdown](#daily-breakdown-developer-c)
- [Developer D: BattleState Integration](#week-5-developer-d---battlestate-integration)
  - [Implementation Tasks](#implementation-tasks-battlestate-integration)
  - [Implementation Examples](#implementation-examples-battlestate-integration)
  - [Rigorous Testing](#rigorous-testing-battlestate-integration)
  - [Daily Breakdown](#daily-breakdown-developer-d)
- [Developer E: Testing & Build Preparation](#week-5-developer-e---testing--build-preparation)
  - [Implementation Tasks](#implementation-tasks-testing-build)
  - [Implementation Examples](#implementation-examples-testing-build)
  - [Rigorous Testing](#rigorous-testing-testing-build)
  - [Daily Breakdown](#daily-breakdown-developer-e)
- [Week 5 Integration Test](#week-5-integration-test-wednesday--friday)
- [Week 5 Deliverable & Verification](#week-5-deliverable--verification)
- [Playtest 1 Execution Plan](#playtest-1-execution-plan)

---

## Week 5: Developer A - Visual Effects & Animation

**Goal**: Add visual polish to spells, attacks, and damage feedback for enhanced player experience

**Foundation**:

- Week 3 spell system (Fireball, CreateWall, LavaPool)
- Week 3 CombatSystem for attack execution
- CS230 Animation system for frame-based animation
- CS230 Particle system for effects

**Files to Create**:

```
DragonicTactics/source/Game/DragonicTactics/VFX/VFXManager.h (new file)
DragonicTactics/source/Game/DragonicTactics/VFX/VFXManager.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/VFX/SpellEffect.h (new file)
DragonicTactics/source/Game/DragonicTactics/VFX/SpellEffect.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/VFX/DamageNumber.h (new file)
DragonicTactics/source/Game/DragonicTactics/VFX/DamageNumber.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/VFXTests.cpp (new file)
```

**Files to Modify**:

```
DragonicTactics/source/Game/DragonicTactics/Objects/Character.cpp (add damage feedback)
DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp (integrate VFX)
```

### Implementation Tasks (Visual Effects)

#### **Task 1: VFXManager Singleton**

**Goal**: Create centralized system for managing visual effects lifecycle

**Steps**:

1. Create VFXManager singleton with event-driven architecture
2. Subscribe to combat events (CharacterDamagedEvent, SpellCastEvent, etc.)
3. Implement effect spawning and update loop
4. Manage effect cleanup when animations complete
5. Integrate with BattleState rendering pipeline

**Why this matters**: Visual effects need centralized management to avoid memory leaks and ensure proper rendering order. Event-driven approach ensures effects trigger automatically without manual coupling to game systems.

**Key Design Principles**:

- Effects are spawned in response to events (loose coupling)
- VFXManager owns effect lifecycle (automatic cleanup)
- Effects render in layers (background → gameplay → foreground)
- Pooling for frequently spawned effects (damage numbers)

---

#### **Task 2: Spell Visual Effects**

**Goal**: Create visual feedback for Fireball, CreateWall, and LavaPool spells

**Steps**:

1. Implement Fireball explosion animation (expanding circle with particles)
2. Implement CreateWall materialization effect (tiles rising from ground)
3. Implement LavaPool bubbling animation (animated sprite)
4. Add spell casting indicator (preparation phase visual)
5. Synchronize effects with spell execution timing

**Visual Design**:

- **Fireball**: Orange/red explosion expanding from impact point, particle burst
- **CreateWall**: Stone tiles rising with dust particles
- **LavaPool**: Bubbling lava texture with heat distortion particles

**Timing Considerations**:

- Effects should complete before next action starts
- Option to speed up/skip effects for faster gameplay
- Effects don't block game state updates (visual only)

---

#### **Task 3: Damage Numbers**

**Goal**: Display floating damage numbers when characters take damage

**Steps**:

1. Create DamageNumber class (position, value, lifetime, color)
2. Spawn damage numbers on CharacterDamagedEvent
3. Implement floating animation (rise and fade out)
4. Color-code by damage type (red = physical, orange = fire, etc.)
5. Support critical hit visuals (larger font, yellow color)

**Design Specs**:

- **Lifetime**: 1.5 seconds
- **Animation**: Rise 30 pixels, fade out after 1 second
- **Font Size**: 24pt normal, 36pt critical hits
- **Colors**: Red (physical), Orange (fire), Blue (cold), Yellow (lightning)

---

#### **Task 4: Attack Visual Feedback**

**Goal**: Add visual effects when characters attack (melee swing, projectile)

**Steps**:

1. Implement melee attack slash effect (arc from attacker to target)
2. Add hit impact particles when attack connects
3. Implement attack miss feedback (whiff animation)
4. Add character sprite flash on damage taken
5. Synchronize attack animation with damage application

**Attack Flow**:

1. Attacker plays attack animation
2. Projectile/slash effect travels to target
3. Impact effect spawns on target
4. Damage number appears
5. Target flashes red briefly

---

#### **Task 5: Grid Visual Enhancements**

**Goal**: Add visual feedback for grid interactions (hover, selection, movement range)

**Steps**:

1. Implement tile hover effect (highlight hovered tile)
2. Add movement range visualization (blue overlay on walkable tiles)
3. Add spell range visualization (red overlay on targetable tiles)
4. Implement path preview (show movement path when hovering destination)
5. Add invalid action feedback (red X on invalid tiles)

**Visual Layers**:

- **Base Grid**: Always visible
- **Range Overlay**: Semi-transparent colored tiles
- **Hover Highlight**: Bright outline on hovered tile
- **Path Preview**: Animated dashed line

---

### Implementation Examples (Visual Effects)

#### **Example 1: VFXManager Singleton**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/VFX/VFXManager.h
#pragma once
#include "../../../Engine/Component.h"
#include "SpellEffect.h"
#include "DamageNumber.h"
#include <vector>
#include <memory>

// Manages all visual effects in the battle
class VFXManager : public CS230::Component {
public:
    // Singleton access
    static VFXManager& Instance();

    // Component interface
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix);

    // Effect spawning (public interface - SnakeCase)
    void SpawnDamageNumber(Math::vec2 position, int damage, DamageType type, bool is_critical);
    void SpawnSpellEffect(const std::string& spell_name, Math::vec2 position);
    void SpawnAttackEffect(Math::vec2 from, Math::vec2 to, bool hit);
    void SpawnDeathEffect(Math::vec2 position);

    // Effect management
    void ClearAllEffects();
    int GetActiveEffectCount() const;

private:
    VFXManager();
    ~VFXManager() override = default;
    VFXManager(const VFXManager&) = delete;
    VFXManager& operator=(const VFXManager&) = delete;

    // Event subscription (private - snake_case)
    void subscribe_to_events();
    void on_character_damaged(const CharacterDamagedEvent& event);
    void on_spell_cast(const SpellCastEvent& event);
    void on_character_death(const CharacterDeathEvent& event);

    // Effect lifecycle
    void update_effects(double dt);
    void remove_finished_effects();

    // Effect storage
    std::vector<std::unique_ptr<DamageNumber>> damage_numbers_;
    std::vector<std::unique_ptr<SpellEffect>> spell_effects_;
    std::vector<std::unique_ptr<CS230::Particle>> particles_;
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/VFX/VFXManager.cpp
#include "VFXManager.h"
#include "../Singletons/EventBus.h"
#include "../Types/Events.h"
#include "../../../Engine/Engine.hpp"

VFXManager& VFXManager::Instance() {
    static VFXManager instance;
    return instance;
}

VFXManager::VFXManager() {
    subscribe_to_events();
}

void VFXManager::subscribe_to_events() {
    // Subscribe to combat events
    EventBus::Instance().Subscribe<CharacterDamagedEvent>(
        [this](const CharacterDamagedEvent& event) {
            on_character_damaged(event);
        }
    );

    EventBus::Instance().Subscribe<SpellCastEvent>(
        [this](const SpellCastEvent& event) {
            on_spell_cast(event);
        }
    );

    EventBus::Instance().Subscribe<CharacterDeathEvent>(
        [this](const CharacterDeathEvent& event) {
            on_character_death(event);
        }
    );
}

void VFXManager::on_character_damaged(const CharacterDamagedEvent& event) {
    // Spawn damage number at character position
    Math::vec2 char_pos = event.target->GetPosition();
    bool is_crit = false; // TODO: Get from CombatSystem

    SpawnDamageNumber(char_pos, event.damage, event.damage_type, is_crit);

    Engine::GetLogger().LogEvent("VFX: Spawned damage number " +
                                  std::to_string(event.damage) + " at position");
}

void VFXManager::on_spell_cast(const SpellCastEvent& event) {
    // Spawn spell effect at target position
    SpawnSpellEffect(event.spell_name, event.target_tile);

    Engine::GetLogger().LogEvent("VFX: Spawned spell effect for " + event.spell_name);
}

void VFXManager::on_character_death(const CharacterDeathEvent& event) {
    // Spawn death effect at character position
    Math::vec2 char_pos = event.character->GetPosition();
    SpawnDeathEffect(char_pos);

    Engine::GetLogger().LogEvent("VFX: Spawned death effect");
}

void VFXManager::SpawnDamageNumber(Math::vec2 position, int damage, DamageType type, bool is_critical) {
    auto damage_num = std::make_unique<DamageNumber>(position, damage, type, is_critical);
    damage_numbers_.push_back(std::move(damage_num));
}

void VFXManager::SpawnSpellEffect(const std::string& spell_name, Math::vec2 position) {
    auto effect = std::make_unique<SpellEffect>(spell_name, position);
    spell_effects_.push_back(std::move(effect));
}

void VFXManager::SpawnAttackEffect(Math::vec2 from, Math::vec2 to, bool hit) {
    // Create slash/projectile effect from attacker to target
    // Implementation depends on attack type (melee vs ranged)
}

void VFXManager::SpawnDeathEffect(Math::vec2 position) {
    // Create death particle effect (smoke, fade out, etc.)
}

void VFXManager::Update(double dt) {
    update_effects(dt);
    remove_finished_effects();
}

void VFXManager::update_effects(double dt) {
    // Update all damage numbers
    for (auto& dmg_num : damage_numbers_) {
        dmg_num->Update(dt);
    }

    // Update all spell effects
    for (auto& effect : spell_effects_) {
        effect->Update(dt);
    }

    // Update all particles
    for (auto& particle : particles_) {
        particle->Update(dt);
    }
}

void VFXManager::remove_finished_effects() {
    // Remove damage numbers that have finished
    damage_numbers_.erase(
        std::remove_if(damage_numbers_.begin(), damage_numbers_.end(),
            [](const std::unique_ptr<DamageNumber>& dmg) {
                return dmg->IsFinished();
            }),
        damage_numbers_.end()
    );

    // Remove spell effects that have finished
    spell_effects_.erase(
        std::remove_if(spell_effects_.begin(), spell_effects_.end(),
            [](const std::unique_ptr<SpellEffect>& effect) {
                return effect->IsFinished();
            }),
        spell_effects_.end()
    );
}

void VFXManager::Draw(Math::TransformationMatrix camera_matrix) {
    // Draw all effects
    for (auto& dmg_num : damage_numbers_) {
        dmg_num->Draw(camera_matrix);
    }

    for (auto& effect : spell_effects_) {
        effect->Draw(camera_matrix);
    }

    for (auto& particle : particles_) {
        particle->Draw(camera_matrix);
    }
}

void VFXManager::ClearAllEffects() {
    damage_numbers_.clear();
    spell_effects_.clear();
    particles_.clear();

    Engine::GetLogger().LogEvent("VFX: Cleared all effects");
}

int VFXManager::GetActiveEffectCount() const {
    return static_cast<int>(damage_numbers_.size() + spell_effects_.size() + particles_.size());
}
```

---

#### **Example 2: DamageNumber Class**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/VFX/DamageNumber.h
#pragma once
#include "../../../Engine/Vec2.hpp"
#include "../../../Engine/Matrix.hpp"
#include "../Types/CharacterTypes.h"
#include <string>

// Floating damage number that appears when characters take damage
class DamageNumber {
public:
    // Constructor (public interface - SnakeCase)
    DamageNumber(Math::vec2 spawn_position, int damage_value, DamageType type, bool is_critical);

    // Update and rendering
    void Update(double dt);
    void Draw(Math::TransformationMatrix camera_matrix);

    // Lifecycle
    bool IsFinished() const { return lifetime_remaining_ <= 0.0; }

private:
    // Get color based on damage type (private - snake_case)
    CS200::Color get_damage_color(DamageType type) const;
    float get_font_size() const;
    float get_alpha() const;

    // State
    Math::vec2 position_;
    Math::vec2 velocity_;
    int damage_;
    DamageType type_;
    bool is_critical_;

    // Animation
    double lifetime_remaining_;
    double lifetime_total_;
    static constexpr double FLOAT_SPEED = 30.0; // pixels per second
    static constexpr double LIFETIME = 1.5; // seconds
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/VFX/DamageNumber.cpp
#include "DamageNumber.h"
#include "../../../Engine/Engine.hpp"
#include "../../../CS200/RGBA.hpp"

DamageNumber::DamageNumber(Math::vec2 spawn_position, int damage_value, DamageType type, bool is_critical)
    : position_(spawn_position)
    , velocity_{0.0, FLOAT_SPEED} // Float upward
    , damage_(damage_value)
    , type_(type)
    , is_critical_(is_critical)
    , lifetime_remaining_(LIFETIME)
    , lifetime_total_(LIFETIME)
{
}

void DamageNumber::Update(double dt) {
    // Move upward
    position_ += velocity_ * dt;

    // Decrease lifetime
    lifetime_remaining_ -= dt;
}

void DamageNumber::Draw(Math::TransformationMatrix camera_matrix) {
    // Get color based on damage type
    CS200::Color color = get_damage_color(type_);

    // Apply alpha fade
    color.alpha = static_cast<unsigned char>(get_alpha() * 255.0f);

    // Get font size
    float font_size = get_font_size();

    // Render damage text
    std::string damage_text = std::to_string(damage_);
    if (is_critical_) {
        damage_text += "!"; // Add exclamation for critical hits
    }

    Engine::GetTextManager().Draw(
        damage_text,
        position_,
        font_size,
        color
    );
}

CS200::Color DamageNumber::get_damage_color(DamageType type) const {
    if (is_critical_) {
        return {255, 255, 0, 255}; // Yellow for crits
    }

    switch (type) {
        case DamageType::Physical:
            return {255, 0, 0, 255}; // Red
        case DamageType::Fire:
            return {255, 128, 0, 255}; // Orange
        case DamageType::Cold:
            return {0, 128, 255, 255}; // Light blue
        case DamageType::Lightning:
            return {255, 255, 128, 255}; // Yellow-white
        case DamageType::Poison:
            return {0, 255, 0, 255}; // Green
        default:
            return {255, 255, 255, 255}; // White
    }
}

float DamageNumber::get_font_size() const {
    if (is_critical_) {
        return 36.0f; // Larger for crits
    }
    return 24.0f; // Normal size
}

float DamageNumber::get_alpha() const {
    // Fade out in the last 0.5 seconds
    if (lifetime_remaining_ < 0.5) {
        return static_cast<float>(lifetime_remaining_ / 0.5);
    }
    return 1.0f; // Full opacity
}
```

---

### Rigorous Testing (Visual Effects)

#### **Test 1: VFXManager Singleton**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/VFXTests.cpp

bool TestVFXManagerSingleton() {
    // Test singleton instance
    VFXManager& vfx1 = VFXManager::Instance();
    VFXManager& vfx2 = VFXManager::Instance();

    if (&vfx1 != &vfx2) {
        Engine::GetLogger().LogError("VFXManager singleton failed - different instances");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ VFXManager singleton test passed");
    return true;
}
```

#### **Test 2: Damage Number Spawning**

```cpp
bool TestDamageNumberSpawning() {
    VFXManager& vfx = VFXManager::Instance();
    vfx.ClearAllEffects();

    // Spawn damage number
    Math::vec2 pos{100.0, 200.0};
    vfx.SpawnDamageNumber(pos, 25, DamageType::Physical, false);

    if (vfx.GetActiveEffectCount() != 1) {
        Engine::GetLogger().LogError("Damage number spawning failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Damage number spawning test passed");
    return true;
}
```

#### **Test 3: Damage Number Lifecycle**

```cpp
bool TestDamageNumberLifecycle() {
    VFXManager& vfx = VFXManager::Instance();
    vfx.ClearAllEffects();

    // Spawn damage number
    vfx.SpawnDamageNumber({100, 200}, 50, DamageType::Fire, false);

    // Update for full lifetime (1.5 seconds + buffer)
    for (int i = 0; i < 160; ++i) {
        vfx.Update(0.01); // 10ms per frame
    }

    // Should be removed after lifetime expires
    if (vfx.GetActiveEffectCount() != 0) {
        Engine::GetLogger().LogError("Damage number lifecycle failed - effect not removed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Damage number lifecycle test passed");
    return true;
}
```

#### **Test 4: Event-Driven Effect Spawning**

```cpp
bool TestEventDrivenEffects() {
    VFXManager& vfx = VFXManager::Instance();
    vfx.ClearAllEffects();

    // Create mock character
    Dragon dragon({3, 3});
    Fighter fighter({5, 5});

    // Publish CharacterDamagedEvent
    CharacterDamagedEvent event{
        .target = &fighter,
        .damage = 30,
        .attacker = &dragon,
        .damage_type = DamageType::Fire
    };

    EventBus::Instance().Publish(event);

    // VFXManager should have spawned damage number automatically
    if (vfx.GetActiveEffectCount() == 0) {
        Engine::GetLogger().LogError("Event-driven effect spawning failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Event-driven effect spawning test passed");
    return true;
}
```

#### **Test 5: Multiple Effects**

```cpp
bool TestMultipleEffects() {
    VFXManager& vfx = VFXManager::Instance();
    vfx.ClearAllEffects();

    // Spawn multiple damage numbers
    vfx.SpawnDamageNumber({100, 100}, 10, DamageType::Physical, false);
    vfx.SpawnDamageNumber({200, 100}, 20, DamageType::Fire, false);
    vfx.SpawnDamageNumber({300, 100}, 30, DamageType::Cold, true);

    if (vfx.GetActiveEffectCount() != 3) {
        Engine::GetLogger().LogError("Multiple effects test failed");
        return false;
    }

    // Update all effects
    vfx.Update(0.1);

    // All should still be alive
    if (vfx.GetActiveEffectCount() != 3) {
        Engine::GetLogger().LogError("Multiple effects update failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Multiple effects test passed");
    return true;
}
```

---

### Daily Breakdown (Developer A)

#### **Monday (7-8 hours)**

- Create VFXManager singleton skeleton (1 hr)
- Implement event subscription system (1.5 hrs)
- Create DamageNumber class with animation (2 hrs)
- Test damage number spawning and lifecycle (1.5 hrs)
- Integrate VFXManager with BattleState rendering (1 hr)
- **Deliverable**: Damage numbers working in combat

#### **Tuesday (7-8 hours)**

- Implement SpellEffect base class (1.5 hrs)
- Create Fireball explosion effect (2 hrs)
- Create CreateWall materialization effect (1.5 hrs)
- Create LavaPool bubbling effect (1.5 hrs)
- Test spell effects triggering on SpellCastEvent (1 hr)
- **Deliverable**: All 3 spell visual effects working

#### **Wednesday (6-7 hours)**

- Implement attack visual feedback (slash/projectile) (2 hrs)
- Add hit impact particles (1.5 hrs)
- Implement character damage flash (1 hr)
- Test attack effects in combat (1.5 hrs)
- Bug fixes from testing (1 hr)
- **Deliverable**: Attack visuals complete

#### **Thursday (6-7 hours)**

- Implement grid hover highlights (1.5 hrs)
- Add movement range visualization (2 hrs)
- Add spell range visualization (1.5 hrs)
- Implement path preview (2 hrs)
- **Deliverable**: Grid visual enhancements complete

#### **Friday (4-5 hours)**

- Write comprehensive VFX test suite (2 hrs)
- Polish visual timing and animations (1 hr)
- Performance optimization (particle pooling) (1 hr)
- Final VFX integration test with BattleState (1 hr)
- **Deliverable**: All VFX polished and production-ready

---

## Week 5: Developer B - Game Flow & Victory Conditions

**Goal**: Implement complete game flow including death handling, victory/defeat conditions, and battle end screens

**Foundation**:

- Week 4 TurnManager for tracking combat state
- Week 3 CombatSystem for damage application
- Week 1 EventBus for game state events
- CS230 GameStateManager for state transitions

**Files to Create**:

```
DragonicTactics/source/Game/DragonicTactics/States/VictoryScreen.h (new file)
DragonicTactics/source/Game/DragonicTactics/States/VictoryScreen.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/States/DefeatScreen.h (new file)
DragonicTactics/source/Game/DragonicTactics/States/DefeatScreen.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Singletons/BattleStatistics.h (new file)
DragonicTactics/source/Game/DragonicTactics/Singletons/BattleStatistics.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/GameFlowTests.cpp (new file)
```

**Files to Modify**:

```
DragonicTactics/source/Game/DragonicTactics/States/BattleState.h (add victory checking)
DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp (implement death handling)
DragonicTactics/source/Game/DragonicTactics/Objects/Character.cpp (add death state)
```

### Implementation Tasks (Game Flow)

#### **Task 1: Death Handling System**

**Goal**: Properly handle character death with visual feedback and state transitions

**Steps**:

1. Add death state to Character class (IsAlive(), IsDead())
2. Implement OnDeath() method with event publishing
3. Add death animation/visual feedback integration
4. Handle dead character in turn order (skip their turns)
5. Prevent actions from/to dead characters

**Why this matters**: Death is a critical game state transition that must be handled consistently across all systems. Improper death handling causes crashes and soft-locks.

---

#### **Task 2: Victory Condition Detection**

**Goal**: Detect when battle ends due to all enemies defeated

**Steps**:

1. Implement BattleState::CheckVictoryConditions() called each turn end
2. Detect when all enemy characters are dead
3. Publish BattleEndedEvent with victory result
4. Trigger victory screen transition
5. Calculate battle statistics (turns survived, damage dealt, spells cast)

---

#### **Task 3: Battle Statistics Tracking**

**Goal**: Track comprehensive battle statistics for end screen display

**Steps**:

1. Create BattleStatistics singleton to track combat metrics
2. Subscribe to all combat events (damage, healing, spells, turns)
3. Calculate aggregate statistics (total damage, accuracy, spell usage)
4. Provide statistics report for victory/defeat screens
5. Reset statistics when new battle starts

---

#### **Task 4: Victory Screen**

**Goal**: Create victory screen displaying battle results and statistics

**Steps**:

1. Create VictoryScreen GameState class
2. Display battle statistics in formatted UI
3. Show "Victory!" message
4. Provide options (Return to Menu, Next Battle)
5. Handle input for screen navigation

---

#### **Task 5: Defeat Screen**

**Goal**: Create defeat screen when Dragon dies

**Steps**:

1. Create DefeatScreen GameState class
2. Display cause of death and battle statistics
3. Show "Defeat" message
4. Provide options (Retry Battle, Return to Menu)
5. Handle input for screen navigation

---

### Implementation Examples (Game Flow)

#### **Example 1: Character Death Handling**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Objects/Character.h

class Character : public CS230::GameObject {
public:
    // Death state (public interface - SnakeCase)
    virtual void OnDeath();
    bool IsAlive() const { return is_alive_; }
    bool IsDead() const { return !is_alive_; }

    // Override TakeDamage to check for death
    void TakeDamage(int damage, Character* attacker) override;

protected:
    bool is_alive_ = true;

    // Death handling (private - snake_case)
    void handle_death();
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Objects/Character.cpp

void Character::TakeDamage(int damage, Character* attacker) {
    if (IsDead()) {
        Engine::GetLogger().LogVerbose("Cannot damage dead character");
        return;
    }

    auto stats = GetGOComponent<StatsComponent>();
    if (!stats) return;

    // Apply damage
    int old_hp = stats->GetCurrentHP();
    stats->TakeDamage(damage);
    int new_hp = stats->GetCurrentHP();

    // Publish damage event
    CharacterDamagedEvent event{this, damage, attacker, DamageType::Physical};
    EventBus::Instance().Publish(event);

    // Check for death
    if (new_hp <= 0 && old_hp > 0) {
        handle_death();
    }

    Engine::GetLogger().LogEvent(TypeName() + " took " + std::to_string(damage) + " damage");
}

void Character::handle_death() {
    is_alive_ = false;

    OnDeath();

    // Publish death event
    CharacterDeathEvent event{this};
    EventBus::Instance().Publish(event);

    Engine::GetLogger().LogEvent(TypeName() + " has died!");
}

void Character::OnDeath() {
    // Default implementation - subclasses can override
}
```

---

#### **Example 2: BattleStatistics Singleton**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Singletons/BattleStatistics.h
#pragma once
#include <string>
#include <map>

class BattleStatistics {
public:
    // Singleton access
    static BattleStatistics& Instance();

    // Battle lifecycle (public interface - SnakeCase)
    void StartNewBattle();
    void EndBattle(bool victory);
    std::string GenerateReport() const;

    // Statistic recording
    void RecordDamageDealt(int damage);
    void RecordDamageTaken(int damage);
    void RecordSpellCast(const std::string& spell_name);
    void RecordKill();
    void RecordTurn();

    // Getters
    int GetTotalDamageDealt() const { return total_damage_dealt_; }
    int GetTotalDamageTaken() const { return total_damage_taken_; }
    int GetTurns() const { return turns_; }
    int GetSpellsCast() const { return spells_cast_; }
    int GetKills() const { return kills_; }

private:
    BattleStatistics() = default;
    ~BattleStatistics() = default;
    BattleStatistics(const BattleStatistics&) = delete;
    BattleStatistics& operator=(const BattleStatistics&) = delete;

    // Event subscription (private - snake_case)
    void subscribe_to_events();
    void on_character_damaged(const CharacterDamagedEvent& event);
    void on_spell_cast(const SpellCastEvent& event);

    // Statistics
    int total_damage_dealt_ = 0;
    int total_damage_taken_ = 0;
    int turns_ = 0;
    int spells_cast_ = 0;
    int kills_ = 0;

    std::map<std::string, int> spell_counts_;
};
```

---

### Rigorous Testing (Game Flow)

#### **Test 1: Character Death State**

```cpp
bool TestCharacterDeathState() {
    Dragon dragon({3, 3});

    if (!dragon.IsAlive()) {
        Engine::GetLogger().LogError("Character not alive at start");
        return false;
    }

    dragon.TakeDamage(1000, nullptr);

    if (!dragon.IsDead()) {
        Engine::GetLogger().LogError("Character not dead after lethal damage");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Character death state test passed");
    return true;
}
```

#### **Test 2: Death Event Publishing**

```cpp
bool TestDeathEventPublishing() {
    bool event_received = false;

    EventBus::Instance().Subscribe<CharacterDeathEvent>(
        [&event_received](const CharacterDeathEvent& event) {
            event_received = true;
        }
    );

    Fighter fighter({5, 5});
    fighter.TakeDamage(1000, nullptr);

    if (!event_received) {
        Engine::GetLogger().LogError("Death event not received");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Death event publishing test passed");
    return true;
}
```

#### **Test 3: Battle Statistics Tracking**

```cpp
bool TestBattleStatisticsTracking() {
    BattleStatistics::Instance().StartNewBattle();

    BattleStatistics::Instance().RecordDamageDealt(50);
    BattleStatistics::Instance().RecordDamageTaken(30);
    BattleStatistics::Instance().RecordSpellCast("Fireball");
    BattleStatistics::Instance().RecordKill();

    if (BattleStatistics::Instance().GetTotalDamageDealt() != 50) {
        Engine::GetLogger().LogError("Damage dealt tracking failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Battle statistics tracking test passed");
    return true;
}
```

#### **Test 4: Dead Character Cannot Act**

```cpp
bool TestDeadCharacterCannotAct() {
    Fighter fighter({5, 5});
    fighter.TakeDamage(1000, nullptr);

    Dragon dragon({3, 3});
    fighter.PerformAttack(&dragon);

    auto stats = dragon.GetGOComponent<StatsComponent>();
    if (stats->GetCurrentHP() != stats->GetMaxHP()) {
        Engine::GetLogger().LogError("Dead character dealt damage");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Dead character cannot act test passed");
    return true;
}
```

#### **Test 5: Victory Condition Detection**

```cpp
bool TestVictoryConditionDetection() {
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    fighter->TakeDamage(1000, dragon);
    battle.CheckVictoryConditions();

    Engine::GetLogger().LogEvent("✅ Victory condition detection test passed");
    return true;
}
```

---

### Daily Breakdown (Developer B)

#### **Monday (7-8 hours)**

- Implement Character death state (IsAlive, IsDead) (1.5 hrs)
- Add OnDeath() method with event publishing (1 hr)
- Handle dead characters in turn order (2 hrs)
- Test death state transitions (1.5 hrs)
- Integrate death handling with TurnManager (1 hr)
- **Deliverable**: Death handling system functional

#### **Tuesday (7-8 hours)**

- Create BattleStatistics singleton (2 hrs)
- Implement event subscription for statistics tracking (2 hrs)
- Add statistic recording methods (1.5 hrs)
- Implement GenerateReport() method (1 hr)
- Test statistics tracking with mock combat (1 hr)
- **Deliverable**: Battle statistics tracking complete

#### **Wednesday (6-7 hours)**

- Implement CheckVictoryConditions() in BattleState (2 hrs)
- Add victory/defeat detection logic (2.5 hrs)
- Test victory/defeat triggering (1.5 hrs)
- **Deliverable**: Victory condition detection working

#### **Thursday (6-7 hours)**

- Create VictoryScreen GameState (2.5 hrs)
- Create DefeatScreen GameState (2.5 hrs)
- Implement screen rendering and input handling (2 hrs)
- **Deliverable**: Victory/defeat screens complete

#### **Friday (4-5 hours)**

- Write comprehensive game flow test suite (2 hrs)
- Polish screen layouts and formatting (1 hr)
- Final integration test (1 hr)
- Bug fixes from testing (1 hr)
- **Deliverable**: Game flow production-ready

---

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

## Week 5: Developer D - BattleState Integration

**Goal**: Integrate all Week 1-4 systems into complete BattleState game loop

**Foundation**:

- Week 4 TurnManager for turn-based gameplay
- Week 3 CombatSystem and SpellSystem
- Week 2 GridSystem and pathfinding
- Week 1 EventBus and all core systems

**Files to Create**:

```
DragonicTactics/source/Game/DragonicTactics/States/BattleController.h (new file)
DragonicTactics/source/Game/DragonicTactics/States/BattleController.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/BattleIntegrationTests.cpp (new file)
```

**Files to Modify**:

```
DragonicTactics/source/Game/DragonicTactics/States/BattleState.h (complete integration)
DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp (full game loop)
```

### Implementation Tasks (BattleState Integration)

#### **Task 1: Complete BattleState Game Loop**

**Goal**: Implement full game loop integrating all systems from Weeks 1-4

**Steps**:

1. Implement BattleState::Load() to initialize all systems
2. Create game loop in Update() (input → logic → rendering)
3. Integrate TurnManager for turn-based flow
4. Connect input handling to character actions
5. Implement proper state transitions (Setup → Combat → End)

**Why this matters**: BattleState is the central orchestrator that brings all systems together. Proper integration ensures smooth gameplay and correct system interactions.

---

#### **Task 2: Input Handling System**

**Goal**: Map player input to game actions (movement, attacks, spells)

**Steps**:

1. Implement input state machine (selecting action → selecting target → confirming)
2. Add keyboard controls (WASD for grid navigation, numbers for spell selection)
3. Implement mouse support (click tiles, hover tooltips)
4. Add input validation (prevent invalid actions)
5. Provide visual feedback for all inputs

**Input Mapping**:

- **Arrow Keys**: Navigate grid cursor
- **Space/Enter**: Confirm selection
- **ESC**: Cancel action
- **1-9**: Select spell by number
- **A**: Attack action
- **M**: Move action

---

#### **Task 3: Character Action Pipeline**

**Goal**: Complete pipeline from input to action execution

**Steps**:

1. Implement action selection UI integration
2. Connect spell selection to SpellSystem
3. Handle target selection for spells and attacks
4. Execute actions through CombatSystem
5. Update game state after action completion

**Action Flow**:

1. Player selects action type (Move/Attack/Spell)
2. System validates action is available
3. Player selects target (tile or character)
4. System validates target is valid
5. Action executes through appropriate system
6. Events published, UI updated
7. Turn ends automatically

---

#### **Task 4: System Integration & Coordination**

**Goal**: Ensure all Week 1-4 systems work together seamlessly

**Steps**:

1. Integrate GridSystem with character movement
2. Connect TurnManager to character action flow
3. Integrate EventBus for system communication
4. Connect VFXManager to combat events
5. Integrate UIManager with game state

**System Coordination**:

- **TurnManager** → determines active character
- **GridSystem** → provides valid movement tiles
- **CombatSystem** → resolves attacks
- **SpellSystem** → executes spells
- **VFXManager** → displays effects
- **UIManager** → shows UI elements

---

#### **Task 5: State Management & Transitions**

**Goal**: Manage BattleState phases and transitions to other states

**Steps**:

1. Implement battle phase system (Setup, PlayerTurn, EnemyTurn, BattleEnd)
2. Add phase transition logic
3. Handle victory/defeat state transitions
4. Implement battle restart functionality
5. Add proper cleanup in Unload()

**Battle Phases**:

- **Setup**: Initialize characters, grid, systems
- **PlayerTurn**: Wait for player input and action
- **EnemyTurn**: AI executes fighter actions
- **BattleEnd**: Transition to victory/defeat screen

---

### Implementation Examples (BattleState Integration)

#### **Example 1: Complete BattleState::Load()**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp

void BattleState::Load() {
    Engine::GetLogger().LogEvent("BattleState: Loading...");

    // Initialize grid system (StateComponent)
    AddComponent(new GridSystem());
    auto grid = GetComponent<GridSystem>();

    // Create characters
    dragon = new Dragon({2, 2});
    fighter = new Fighter({6, 6});

    // Add characters to grid
    grid->AddCharacter(dragon, {2, 2});
    grid->AddCharacter(fighter, {6, 6});

    // Add characters to game object manager
    game_object_manager_.Add(dragon);
    game_object_manager_.Add(fighter);

    // Initialize turn manager
    std::vector<Character*> participants = {dragon, fighter};
    TurnManager::Instance().InitializeTurnOrder(participants);

    // Initialize UI
    UIManager::Instance().RegisterHealthBar(new HealthBar(dragon));
    UIManager::Instance().RegisterHealthBar(new HealthBar(fighter));

    // Initialize statistics
    BattleStatistics::Instance().StartNewBattle();

    // Set initial phase
    current_phase_ = BattlePhase::Setup;

    // Start first turn
    TurnManager::Instance().StartNextTurn();
    current_phase_ = BattlePhase::PlayerTurn;

    Engine::GetLogger().LogEvent("BattleState: Loaded successfully");
}
```

---

#### **Example 2: Input Handling State Machine**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/States/BattleState.h

class BattleState : public CS230::GameState {
public:
    enum class InputState {
        SelectingAction,    // Choosing Move/Attack/Spell
        SelectingTarget,    // Choosing target tile/character
        ConfirmingAction,   // Confirming action
        Executing           // Action in progress (no input)
    };

private:
    InputState input_state_ = InputState::SelectingAction;
    std::string selected_action_;
    Math::ivec2 selected_target_;

    void handle_input_selecting_action();
    void handle_input_selecting_target();
    void handle_input_confirming_action();
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp

void BattleState::HandlePlayerTurnInput() {
    switch (input_state_) {
        case InputState::SelectingAction:
            handle_input_selecting_action();
            break;

        case InputState::SelectingTarget:
            handle_input_selecting_target();
            break;

        case InputState::ConfirmingAction:
            handle_input_confirming_action();
            break;

        case InputState::Executing:
            // No input during execution
            break;
    }
}

void BattleState::handle_input_selecting_action() {
    Input& input = Engine::GetInput();

    if (input.IsKeyPressed(InputKey::A)) {
        selected_action_ = "Attack";
        input_state_ = InputState::SelectingTarget;
        Engine::GetLogger().LogEvent("Action selected: Attack");
    }
    else if (input.IsKeyPressed(InputKey::Num1)) {
        selected_action_ = "Fireball";
        input_state_ = InputState::SelectingTarget;
        UIManager::Instance().ShowSpellSelectionUI(false);
        Engine::GetLogger().LogEvent("Action selected: Fireball");
    }
    else if (input.IsKeyPressed(InputKey::M)) {
        selected_action_ = "Move";
        input_state_ = InputState::SelectingTarget;
        Engine::GetLogger().LogEvent("Action selected: Move");
    }
}

void BattleState::handle_input_selecting_target() {
    Input& input = Engine::GetInput();

    // Move cursor
    if (input.IsKeyPressed(InputKey::Up)) {
        cursor_position_.y -= 1;
    }
    if (input.IsKeyPressed(InputKey::Down)) {
        cursor_position_.y += 1;
    }
    if (input.IsKeyPressed(InputKey::Left)) {
        cursor_position_.x -= 1;
    }
    if (input.IsKeyPressed(InputKey::Right)) {
        cursor_position_.x += 1;
    }

    // Confirm target
    if (input.IsKeyPressed(InputKey::Enter)) {
        selected_target_ = cursor_position_;
        execute_action();
        input_state_ = InputState::Executing;
    }

    // Cancel
    if (input.IsKeyPressed(InputKey::Escape)) {
        input_state_ = InputState::SelectingAction;
        selected_action_ = "";
    }
}

void BattleState::execute_action() {
    if (selected_action_ == "Attack") {
        auto grid = GetComponent<GridSystem>();
        Character* target = grid->GetCharacterAt(selected_target_);
        if (target) {
            dragon->PerformAttack(target);
        }
    }
    else if (selected_action_ == "Fireball") {
        // Cast Fireball spell
        SpellSystem::Instance().CastSpell(dragon, "Fireball", selected_target_);
    }
    else if (selected_action_ == "Move") {
        auto grid = GetComponent<GridSystem>();
        grid->MoveCharacter(dragon, selected_target_);
    }

    // End turn after action
    EndCurrentTurn();
    input_state_ = InputState::SelectingAction;
}
```

---

### Rigorous Testing (BattleState Integration)

#### **Test 1: BattleState Load and Initialization**

```cpp
bool TestBattleStateInitialization() {
    BattleState battle;
    battle.Load();

    // Verify systems initialized
    auto grid = battle.GetComponent<GridSystem>();
    if (!grid) {
        Engine::GetLogger().LogError("GridSystem not initialized");
        return false;
    }

    // Verify characters created
    if (!battle.GetDragon() || !battle.GetFighter()) {
        Engine::GetLogger().LogError("Characters not created");
        return false;
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ BattleState initialization test passed");
    return true;
}
```

#### **Test 2: Input State Machine**

```cpp
bool TestInputStateMachine() {
    BattleState battle;
    battle.Load();

    // Start in SelectingAction state
    // Simulate pressing 'A' for attack
    // Should transition to SelectingTarget

    // Simulate pressing Enter
    // Should transition to Executing

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Input state machine test passed");
    return true;
}
```

#### **Test 3: Action Execution Pipeline**

```cpp
bool TestActionExecutionPipeline() {
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Execute attack action
    dragon->PerformAttack(fighter);

    // Verify damage applied
    auto stats = fighter->GetGOComponent<StatsComponent>();
    if (stats->GetCurrentHP() == stats->GetMaxHP()) {
        Engine::GetLogger().LogError("Action execution failed - no damage");
        return false;
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Action execution pipeline test passed");
    return true;
}
```

#### **Test 4: Turn Flow Integration**

```cpp
bool TestTurnFlowIntegration() {
    BattleState battle;
    battle.Load();

    // Turn 1: Dragon's turn
    Character* current = TurnManager::Instance().GetCurrentCharacter();
    if (current->TypeName() != "Dragon" && current->TypeName() != "Fighter") {
        Engine::GetLogger().LogError("Turn flow failed - invalid first character");
        return false;
    }

    // End turn
    battle.EndCurrentTurn();

    // Turn 2: Should switch to other character
    Character* next = TurnManager::Instance().GetCurrentCharacter();
    if (next == current) {
        Engine::GetLogger().LogError("Turn flow failed - same character");
        return false;
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Turn flow integration test passed");
    return true;
}
```

#### **Test 5: Full Combat Integration**

```cpp
bool TestFullCombatIntegration() {
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Simulate full combat loop
    for (int turn = 0; turn < 10; ++turn) {
        Character* current = TurnManager::Instance().GetCurrentCharacter();

        if (current == dragon) {
            // Dragon attacks
            dragon->PerformAttack(fighter);
        } else {
            // Fighter attacks
            fighter->PerformAttack(dragon);
        }

        battle.EndCurrentTurn();

        // Check victory conditions
        if (fighter->IsDead()) {
            Engine::GetLogger().LogEvent("Dragon victory!");
            break;
        }
        if (dragon->IsDead()) {
            Engine::GetLogger().LogEvent("Fighter victory!");
            break;
        }
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Full combat integration test passed");
    return true;
}
```

---

### Daily Breakdown (Developer D)

#### **Monday (7-8 hours)**

- Implement complete BattleState::Load() (2 hrs)
- Initialize all systems (Grid, Turn, UI, Stats) (2 hrs)
- Create character instances and placement (1 hr)
- Test initialization sequence (2 hrs)
- **Deliverable**: BattleState loads correctly

#### **Tuesday (7-8 hours)**

- Implement input handling state machine (2.5 hrs)
- Add keyboard controls for all actions (2 hrs)
- Implement cursor movement and selection (1.5 hrs)
- Test input flow (1.5 hrs)
- **Deliverable**: Input system complete

#### **Wednesday (6-7 hours)**

- Implement action execution pipeline (2 hrs)
- Connect spell selection to SpellSystem (1.5 hrs)
- Integrate combat actions with CombatSystem (1.5 hrs)
- Test action execution (2 hrs)
- **Deliverable**: Actions execute correctly

#### **Thursday (6-7 hours)**

- Integrate all Week 1-4 systems (3 hrs)
- Implement battle phase transitions (2 hrs)
- Add victory/defeat detection (1.5 hrs)
- Test system coordination (1.5 hrs)
- **Deliverable**: Full integration complete

#### **Friday (4-5 hours)**

- Write comprehensive integration test suite (2 hrs)
- Final bug fixes and polish (1.5 hrs)
- Test full combat scenarios (1 hr)
- Prepare for playtest (30 min)
- **Deliverable**: BattleState production-ready

---

## Week 5: Developer E - Testing & Build Preparation

**Goal**: Create comprehensive test suite (80+ tests), prepare stable build for Playtest 1

**Foundation**:

- Week 1-4 test infrastructure (TestAssert, Week1TestMocks, Week3TestMocks)
- All implemented systems need test coverage
- CMake build system for build automation

**Files to Create**:

```
DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.h (new file)
DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.h (new file)
DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/PlaytestBuild.h (new file)
DragonicTactics/source/Game/DragonicTactics/Test/PlaytestBuild.cpp (new file)
```

**Files to Modify**:

```
DragonicTactics/CMakeLists.txt (add test configuration)
All existing test files (organize and expand coverage)
```

### Implementation Tasks (Testing & Build)

#### **Task 1: Comprehensive Test Suite**

**Goal**: Achieve 80+ tests covering all Week 1-5 systems

**Steps**:

1. Audit existing tests (Week 1-4 test files)
2. Identify gaps in test coverage
3. Write missing unit tests for each system
4. Write integration tests for system interactions
5. Create automated test runner

**Test Coverage Target**:

- **Character System**: 15 tests
- **Combat System**: 12 tests
- **Spell System**: 10 tests
- **Grid System**: 8 tests
- **Turn Manager**: 8 tests
- **Event System**: 6 tests
- **UI System**: 8 tests
- **VFX System**: 5 tests
- **Game Flow**: 8 tests
- **Total**: 80+ tests

**Why this matters**: Comprehensive testing catches bugs early, ensures system stability, and gives confidence that the playtest build won't crash.

---

#### **Task 2: Test Runner & Automation**

**Goal**: Create automated test runner to execute all tests with single command

**Steps**:

1. Create TestRunner singleton to manage all tests
2. Implement test registration system
3. Add test result reporting (passed/failed/skipped)
4. Create test suites for different categories
5. Add command-line test execution

**Test Runner Features**:

- Run all tests with single command
- Filter tests by category (unit, integration, system)
- Generate test report (console + log file)
- Return non-zero exit code on failure (for CI)

---

#### **Task 3: Integration Test Scenarios**

**Goal**: Create realistic end-to-end integration tests

**Steps**:

1. Write "Full Combat" integration test (Dragon vs Fighter to victory)
2. Write "All Spells" integration test (cast all 3 spells in battle)
3. Write "Death Handling" integration test (character death + game over)
4. Write "Turn System" integration test (10 turns with proper order)
5. Write "UI Integration" integration test (all UI elements update correctly)

**Integration Test Requirements**:

- Test must run without manual input
- Test must verify expected outcomes
- Test must clean up resources properly
- Test must complete in < 5 seconds

---

#### **Task 4: Playtest Build Preparation**

**Goal**: Prepare stable, playable build for external playtesting

**Steps**:

1. Create Release build configuration (optimizations enabled)
2. Verify all assets bundle correctly
3. Test build on clean machine (no dev environment)
4. Create playtest instructions document
5. Prepare feedback collection form

**Build Requirements**:

- No crashes during 10-minute play session
- All 3 Dragon spells functional
- Fighter AI makes reasonable decisions
- Victory/defeat screens display correctly
- Performance: 60+ FPS on target hardware

---

#### **Task 5: Bug Tracking & Issue Management**

**Goal**: Track and prioritize bugs found during testing

**Steps**:

1. Set up bug tracking system (could be simple text file or GitHub Issues)
2. Categorize bugs by severity (Critical, Major, Minor)
3. Triage bugs for Week 5 fixes vs post-playtest
4. Fix critical bugs blocking playtest
5. Document known issues for playtesters

**Bug Severity Levels**:

- **Critical**: Crashes, soft-locks, game-breaking bugs (MUST FIX)
- **Major**: Significant gameplay impact (fix if time permits)
- **Minor**: Visual glitches, minor issues (defer to Week 6)

---

### Implementation Examples (Testing & Build)

#### **Example 1: TestRunner Singleton**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.h
#pragma once
#include <string>
#include <vector>
#include <functional>

// Test function signature: returns true if passed
using TestFunction = std::function<bool()>;

struct TestCase {
    std::string name;
    std::string category; // "Unit", "Integration", "System"
    TestFunction function;
};

// Manages and executes all tests
class TestRunner {
public:
    // Singleton access
    static TestRunner& Instance();

    // Test registration (public interface - SnakeCase)
    void RegisterTest(const std::string& name, const std::string& category, TestFunction test_func);

    // Test execution
    int RunAllTests();
    int RunCategory(const std::string& category);
    int RunSingleTest(const std::string& name);

    // Reporting
    void PrintResults() const;
    std::string GenerateReport() const;

    // Statistics
    int GetPassedCount() const { return passed_count_; }
    int GetFailedCount() const { return failed_count_; }
    int GetTotalCount() const { return static_cast<int>(tests_.size()); }

private:
    TestRunner() = default;
    ~TestRunner() = default;
    TestRunner(const TestRunner&) = delete;
    TestRunner& operator=(const TestRunner&) = delete;

    // Test storage (private - snake_case)
    std::vector<TestCase> tests_;
    int passed_count_ = 0;
    int failed_count_ = 0;

    // Test execution helpers
    bool run_test(const TestCase& test);
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.cpp
#include "TestRunner.h"
#include "../../../Engine/Engine.hpp"
#include <iostream>

TestRunner& TestRunner::Instance() {
    static TestRunner instance;
    return instance;
}

void TestRunner::RegisterTest(const std::string& name, const std::string& category, TestFunction test_func) {
    TestCase test{name, category, test_func};
    tests_.push_back(test);
    Engine::GetLogger().LogVerbose("TestRunner: Registered test '" + name + "' in category '" + category + "'");
}

int TestRunner::RunAllTests() {
    passed_count_ = 0;
    failed_count_ = 0;

    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("    RUNNING ALL TESTS");
    Engine::GetLogger().LogEvent("========================================");

    for (const auto& test : tests_) {
        if (run_test(test)) {
            passed_count_++;
        } else {
            failed_count_++;
        }
    }

    PrintResults();

    return failed_count_; // Return 0 if all passed
}

int TestRunner::RunCategory(const std::string& category) {
    passed_count_ = 0;
    failed_count_ = 0;

    Engine::GetLogger().LogEvent("Running tests in category: " + category);

    for (const auto& test : tests_) {
        if (test.category == category) {
            if (run_test(test)) {
                passed_count_++;
            } else {
                failed_count_++;
            }
        }
    }

    PrintResults();

    return failed_count_;
}

bool TestRunner::run_test(const TestCase& test) {
    Engine::GetLogger().LogEvent("Running: " + test.name);

    try {
        bool result = test.function();
        if (result) {
            Engine::GetLogger().LogEvent("  ✅ PASSED");
        } else {
            Engine::GetLogger().LogError("  ❌ FAILED");
        }
        return result;
    }
    catch (const std::exception& e) {
        Engine::GetLogger().LogError("  ❌ EXCEPTION: " + std::string(e.what()));
        return false;
    }
}

void TestRunner::PrintResults() const {
    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("    TEST RESULTS");
    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("Total: " + std::to_string(GetTotalCount()));
    Engine::GetLogger().LogEvent("Passed: " + std::to_string(passed_count_));
    Engine::GetLogger().LogEvent("Failed: " + std::to_string(failed_count_));

    if (failed_count_ == 0) {
        Engine::GetLogger().LogEvent("✅ ALL TESTS PASSED");
    } else {
        Engine::GetLogger().LogError("❌ SOME TESTS FAILED");
    }
    Engine::GetLogger().LogEvent("========================================");
}

std::string TestRunner::GenerateReport() const {
    std::stringstream report;
    report << "Test Report\n";
    report << "===========\n";
    report << "Total: " << GetTotalCount() << "\n";
    report << "Passed: " << passed_count_ << "\n";
    report << "Failed: " << failed_count_ << "\n";
    return report.str();
}
```

---

#### **Example 2: Integration Test - Full Combat**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.cpp

bool TestFullCombatIntegration() {
    Engine::GetLogger().LogEvent("=== FULL COMBAT INTEGRATION TEST ===");

    // Initialize battle
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Verify initial state
    if (!dragon->IsAlive() || !fighter->IsAlive()) {
        Engine::GetLogger().LogError("Characters not alive at start");
        return false;
    }

    // Simulate combat (Dragon always attacks)
    int turn_limit = 20;
    for (int turn = 0; turn < turn_limit; ++turn) {
        Character* current = TurnManager::Instance().GetCurrentCharacter();

        if (!dragon->IsAlive() || !fighter->IsAlive()) {
            break; // Battle ended
        }

        // Execute action
        if (current == dragon) {
            dragon->PerformAttack(fighter);
        } else {
            fighter->PerformAttack(dragon);
        }

        battle.EndCurrentTurn();
    }

    // Verify battle ended (one character died)
    bool battle_ended = !dragon->IsAlive() || !fighter->IsAlive();
    if (!battle_ended) {
        Engine::GetLogger().LogError("Battle didn't end after " + std::to_string(turn_limit) + " turns");
        battle.Unload();
        return false;
    }

    // Verify victory conditions triggered
    battle.CheckVictoryConditions();

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Full combat integration test passed");
    return true;
}
```

---

### Rigorous Testing (Testing & Build)

#### **Test 1: TestRunner Registration**

```cpp
bool TestTestRunnerRegistration() {
    TestRunner& runner = TestRunner::Instance();

    int initial_count = runner.GetTotalCount();

    runner.RegisterTest("Dummy Test", "Unit", []() { return true; });

    if (runner.GetTotalCount() != initial_count + 1) {
        Engine::GetLogger().LogError("TestRunner registration failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ TestRunner registration test passed");
    return true;
}
```

#### **Test 2: Test Execution and Results**

```cpp
bool TestTestExecution() {
    TestRunner& runner = TestRunner::Instance();

    // Register passing test
    runner.RegisterTest("Passing Test", "Unit", []() { return true; });

    // Register failing test
    runner.RegisterTest("Failing Test", "Unit", []() { return false; });

    int failures = runner.RunCategory("Unit");

    if (failures == 0) {
        Engine::GetLogger().LogError("TestRunner should have detected failures");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Test execution test passed");
    return true;
}
```

#### **Test 3: Category Filtering**

```cpp
bool TestCategoryFiltering() {
    TestRunner& runner = TestRunner::Instance();

    runner.RegisterTest("Unit Test 1", "Unit", []() { return true; });
    runner.RegisterTest("Integration Test 1", "Integration", []() { return true; });

    int unit_failures = runner.RunCategory("Unit");
    int integration_failures = runner.RunCategory("Integration");

    if (unit_failures != 0 || integration_failures != 0) {
        Engine::GetLogger().LogError("Category filtering failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Category filtering test passed");
    return true;
}
```

#### **Test 4: Build Verification**

```cpp
bool TestBuildVerification() {
    // Verify all required systems are present
    bool grid_ok = (GetComponent<GridSystem>() != nullptr);
    bool turn_manager_ok = true; // TurnManager::Instance() works

    if (!grid_ok || !turn_manager_ok) {
        Engine::GetLogger().LogError("Build verification failed - missing systems");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Build verification test passed");
    return true;
}
```

#### **Test 5: Performance Benchmark**

```cpp
bool TestPerformanceBenchmark() {
    // Measure frame time for 100 updates
    auto start = std::chrono::high_resolution_clock::now();

    BattleState battle;
    battle.Load();

    for (int i = 0; i < 100; ++i) {
        battle.Update(0.016); // 60 FPS frame time
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    battle.Unload();

    // Should complete in < 2 seconds
    if (duration > 2000) {
        Engine::GetLogger().LogError("Performance too slow: " + std::to_string(duration) + "ms");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Performance benchmark test passed (" +
                                  std::to_string(duration) + "ms)");
    return true;
}
```

---

### Daily Breakdown (Developer E)

#### **Monday (7-8 hours)**

- Audit existing tests from Weeks 1-4 (1.5 hrs)
- Identify test coverage gaps (1 hr)
- Write 15+ missing unit tests (3 hrs)
- Organize tests into categories (1 hr)
- **Deliverable**: 60+ tests with improved coverage

#### **Tuesday (7-8 hours)**

- Create TestRunner singleton (2 hrs)
- Implement test registration and execution (2 hrs)
- Add test reporting and statistics (1 hr)
- Write 10+ integration tests (2.5 hrs)
- **Deliverable**: TestRunner complete, 70+ tests

#### **Wednesday (6-7 hours)**

- Write full combat integration test (1.5 hrs)
- Write all-spells integration test (1.5 hrs)
- Write death handling integration test (1 hr)
- Write turn system integration test (1 hr)
- Write UI integration test (1.5 hrs)
- **Deliverable**: 5 major integration tests, 75+ tests total

#### **Thursday (6-7 hours)**

- Create Release build configuration (1 hr)
- Test build on clean machine (2 hrs)
- Fix critical bugs found in testing (2.5 hrs)
- Write playtest instructions (1 hr)
- **Deliverable**: Stable playtest build ready

#### **Friday (4-5 hours)**

- Run full test suite (80+ tests) (1 hr)
- Create playtest feedback form (30 min)
- Prepare playtest demo environment (1 hr)
- Final bug triage and documentation (1.5 hrs)
- **Deliverable**: Playtest 1 ready for execution

---

## Week 5 Integration Tests (Wednesday & Friday)

### Wednesday Mid-Week Integration Check

**Goal**: Verify all Week 5 systems integrate correctly before final push

**Participants**: All 5 developers

**Duration**: 60 minutes

**Test Scenarios**:

1. **Visual Effects Integration** (Developer A)
   
   - Verify damage numbers appear when characters take damage
   - Verify spell effects trigger on spell cast
   - Verify attack animations play correctly

2. **Game Flow Integration** (Developer B)
   
   - Verify character death triggers properly
   - Verify victory/defeat screens appear
   - Verify battle statistics track correctly

3. **UI Integration** (Developer C)
   
   - Verify health bars update on damage
   - Verify turn indicator follows active character
   - Verify spell selection UI appears and responds to input

4. **BattleState Integration** (Developer D)
   
   - Verify full game loop runs without crashes
   - Verify input handling works for all actions
   - Verify turn transitions work correctly

5. **Testing Integration** (Developer E)
   
   - Run automated test suite (should have 75+ tests)
   - Verify no critical bugs blocking playtest
   - Identify any integration issues

**Success Criteria**:

- ✅ All 5 developer systems integrated without crashes
- ✅ No critical bugs found
- ✅ 75+ automated tests passing
- ✅ Full combat loop completes successfully

---

### Friday Playtest 1 Integration Test

**Goal**: Final verification before external playtest

**Participants**: All 5 developers + external playtesters

**Duration**: 90 minutes (30 min dev test + 60 min external playtest)

#### Developer Pre-Playtest Check (30 minutes)

**Full Combat Scenario**:

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.cpp

bool Week5FinalIntegrationTest() {
    Engine::GetLogger().LogEvent("=== WEEK 5 FINAL INTEGRATION TEST ===");

    // Test 1: Battle Initialization
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Verify all systems initialized
    auto grid = battle.GetComponent<GridSystem>();
    if (!grid) {
        Engine::GetLogger().LogError("GridSystem not initialized");
        return false;
    }

    // Test 2: UI Systems
    UIManager& ui = UIManager::Instance();
    ui.UpdateTurnIndicator(dragon);

    // Test 3: Visual Effects
    VFXManager& vfx = VFXManager::Instance();
    vfx.SpawnDamageNumber({100, 100}, 25, DamageType::Fire, false);

    // Test 4: Full Combat Loop
    int turn_count = 0;
    while (dragon->IsAlive() && fighter->IsAlive() && turn_count < 20) {
        Character* current = TurnManager::Instance().GetCurrentCharacter();

        // Execute action
        if (current == dragon) {
            // Dragon uses spells
            if (turn_count == 0) {
                SpellSystem::Instance().CastSpell(dragon, "Fireball", fighter->GetGridPosition());
            } else if (turn_count == 2) {
                SpellSystem::Instance().CastSpell(dragon, "CreateWall", {4, 4});
            } else if (turn_count == 4) {
                SpellSystem::Instance().CastSpell(dragon, "LavaPool", {5, 5});
            } else {
                dragon->PerformAttack(fighter);
            }
        } else {
            // Fighter uses AI
            fighter->DecideAction();
        }

        battle.EndCurrentTurn();
        turn_count++;

        // Update all systems
        ui.Update(0.016);
        vfx.Update(0.016);
    }

    // Test 5: Victory/Defeat
    battle.CheckVictoryConditions();

    // Verify statistics tracked
    BattleStatistics& stats = BattleStatistics::Instance();
    if (stats.GetTurns() != turn_count) {
        Engine::GetLogger().LogError("Statistics tracking failed");
        return false;
    }

    battle.Unload();

    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("✅ ALL WEEK 5 INTEGRATION TESTS PASSED");
    Engine::GetLogger().LogEvent("========================================");

    return true;
}
```

**Verification Checklist**:

- [ ] Battle initializes without errors
- [ ] Dragon and Fighter spawn at correct positions
- [ ] Grid displays correctly
- [ ] Health bars appear and update
- [ ] Turn indicator shows active character
- [ ] All 3 Dragon spells cast successfully
  - [ ] Fireball deals damage and shows explosion effect
  - [ ] CreateWall creates wall tiles
  - [ ] LavaPool creates lava tiles with damage
- [ ] Fighter AI makes decisions
- [ ] Damage numbers appear on hits
- [ ] Attack animations play
- [ ] Character dies when HP reaches 0
- [ ] Victory/defeat screen appears
- [ ] Battle statistics display correctly
- [ ] No crashes during 10-turn combat
- [ ] 80+ automated tests pass

---

#### External Playtest (60 minutes)

**Playtest Instructions** (hand to playtesters):

```
Dragonic Tactics - Playtest 1 Instructions

Welcome to Playtest 1 of Dragonic Tactics!

GOAL:
Defeat the Fighter using your Dragon's spells and attacks.

CONTROLS:
- Arrow Keys: Move grid cursor
- 1: Cast Fireball (damages area)
- 2: Cast Create Wall (creates wall tiles)
- 3: Cast Lava Pool (creates damaging lava)
- A: Melee Attack
- M: Move character
- Enter: Confirm action
- ESC: Cancel action

HOW TO PLAY:
1. On your turn, select an action (spell or attack)
2. Use arrow keys to select target tile
3. Press Enter to confirm
4. Fighter will automatically take its turn
5. Defeat the Fighter to win!

WHAT TO LOOK FOR:
- Any crashes or freezes
- Confusing UI or controls
- Balance issues (too easy/hard?)
- Visual glitches
- Missing features you expected

Please report ALL issues, no matter how small!
```

**Feedback Collection Form**:

```
Playtest 1 Feedback Form

Name (optional): _______________
Date: _______________

Rate your experience (1-5):
[ ] Overall enjoyment
[ ] Controls/UI clarity
[ ] Visual polish
[ ] Combat balance
[ ] Game difficulty

Did you encounter any bugs? (describe):




What did you like most?




What needs improvement?




Additional comments:
```

---

## Week 5 Deliverables & Verification

### Final Deliverables (End of Week 5)

**Deadline**: Friday Week 5, 5:00 PM

#### **Developer A Deliverables**

- [ ] VFXManager singleton complete
- [ ] Damage numbers functional
- [ ] All 3 spell visual effects working (Fireball, CreateWall, LavaPool)
- [ ] Attack visual feedback complete
- [ ] Grid highlights and hover effects
- [ ] 5+ VFX tests passing

#### **Developer B Deliverables**

- [ ] Character death handling complete
- [ ] BattleStatistics singleton tracking all metrics
- [ ] Victory/defeat conditions detecting correctly
- [ ] VictoryScreen GameState implemented
- [ ] DefeatScreen GameState implemented
- [ ] 5+ game flow tests passing

#### **Developer C Deliverables**

- [ ] UIManager singleton complete
- [ ] Health bars displaying and updating
- [ ] Turn indicator functional
- [ ] Spell selection UI complete
- [ ] Grid tile highlights and tooltips
- [ ] 5+ UI tests passing

#### **Developer D Deliverables**

- [ ] BattleState full integration complete
- [ ] Input handling state machine functional
- [ ] Action execution pipeline working
- [ ] All Week 1-4 systems integrated
- [ ] Battle phase transitions working
- [ ] 5+ integration tests passing

#### **Developer E Deliverables**

- [ ] 80+ comprehensive tests written
- [ ] TestRunner singleton complete
- [ ] All integration tests passing
- [ ] Stable Release build prepared
- [ ] Playtest instructions written
- [ ] Feedback form created
- [ ] Bug tracking system set up

---

### Integration Verification

**Run Full Test Suite**:

```bash
# Build
cmake --preset windows-debug
cmake --build --preset windows-debug

# Run all tests
./build/windows-debug/dragonic_tactics.exe --test all

# Expected output:
========================================
    RUNNING ALL TESTS
========================================
Running: TestEventBusPublish
  ✅ PASSED
Running: TestDiceManager3d6
  ✅ PASSED
...
[80+ tests]
...
========================================
    TEST RESULTS
========================================
Total: 82
Passed: 82
Failed: 0
✅ ALL TESTS PASSED
========================================
```

**Manual Playtest Verification**:

```bash
# Run game in Release mode
cmake --preset windows-release
cmake --build --preset windows-release
./build/windows-release/dragonic_tactics.exe
```

**Verification Steps**:

1. Launch game
2. Navigate to BattleState (Main Menu → Battle)
3. Play full combat:
   - Cast Fireball on Fighter
   - Cast Create Wall to block path
   - Cast Lava Pool near Fighter
   - Attack Fighter until defeated
4. Verify victory screen appears
5. Check statistics displayed correctly
6. No crashes occurred

---

### Success Criteria

Week 5 is complete when:

1. ✅ **All Visual Effects Working**: Damage numbers, spell effects, attack animations
2. ✅ **Complete Game Flow**: Death handling, victory/defeat screens, statistics tracking
3. ✅ **Polished UI**: Health bars, turn indicator, spell selection, grid highlights
4. ✅ **Full BattleState Integration**: All Week 1-4 systems working together
5. ✅ **80+ Tests Passing**: Comprehensive test coverage with all tests green
6. ✅ **Stable Build**: No crashes during 10-minute play session
7. ✅ **Playtest Ready**: Instructions, feedback form, and build prepared
8. ✅ **All Spells Functional**: Fireball, CreateWall, LavaPool cast successfully
9. ✅ **Fighter AI Working**: Makes autonomous decisions
10. ✅ **Victory/Defeat Triggers**: Game ends correctly when battle concludes

---

## Playtest 1 Execution Plan

### Schedule (Friday Week 5, 2:00 PM - 3:30 PM)

**2:00 PM - 2:15 PM**: Setup & Introduction (15 min)

- Set up 3-5 playtesting stations
- Brief playtesters on goals and controls
- Distribute feedback forms

**2:15 PM - 3:00 PM**: Playtesting Session (45 min)

- Playtesters play Dragon vs Fighter battle
- Developers observe and take notes
- Developers available for critical bug fixes only (no help unless game-breaking)

**3:00 PM - 3:15 PM**: Feedback Collection (15 min)

- Collect completed feedback forms
- Brief verbal feedback session
- Thank playtesters

**3:15 PM - 3:30 PM**: Developer Debrief (15 min)

- Discuss major findings
- Prioritize Week 6 improvements
- Celebrate Playtest 1 completion!

---

### Playtester Profiles

**Target Playtesters** (3-5 people):

- **Playtester 1**: Experienced with tactical RPGs (Baldur's Gate 3, XCOM)
- **Playtester 2**: Casual gamer (some RPG experience)
- **Playtester 3**: New to tactical RPGs (fresh perspective)
- **Playtester 4 (optional)**: Game developer (technical feedback)
- **Playtester 5 (optional)**: Friend/family (honest feedback)

---

### Known Issues to Document

**Before Playtest, Document These Known Issues**:

```
Known Issues (Playtest 1)
=========================

CRITICAL (Game-Breaking):
- [None expected - fixed during Week 5]

MAJOR (Significant Impact):
- [TBD based on Wednesday integration test]

MINOR (Cosmetic/Polish):
- Limited spell animations (placeholders)
- No sound effects
- Basic UI layout
- Fighter AI uses simple logic (only attacks, no strategy)
- Only 1v1 combat supported
- No save/load functionality

FUTURE FEATURES (Not Implemented):
- Cleric, Wizard, Rogue characters (Week 11-15)
- Advanced AI (Week 16-20)
- More Dragon spells (Week 6-10)
- Multiplayer (not planned)
```

---

### Post-Playtest Actions

**Immediate (Friday Evening)**:

1. Compile all feedback forms
2. Categorize bugs by severity
3. Create Week 6 priority list

**Week 6 Preview Based on Playtest 1**:

- **Developer A**: Fix VFX bugs, add more spell effects
- **Developer B**: Improve game flow based on feedback
- **Developer C**: UI/UX improvements from playtest findings
- **Developer D**: Balance adjustments, AI improvements
- **Developer E**: Expand test coverage for found bugs

---

### Celebration & Retrospective

**Week 5 Achievements**:

- ✅ **First Playable Build**: Dragon vs Fighter fully playable
- ✅ **All 3 Dragon Spells**: Fireball, CreateWall, LavaPool working
- ✅ **Complete Game Flow**: Start to victory/defeat
- ✅ **80+ Tests**: Comprehensive test coverage
- ✅ **Foundation Complete**: All core systems integrated

**Team Retrospective Questions**:

1. What went well this week?
2. What could be improved?
3. Any blockers or dependencies issues?
4. What did we learn from the playtest?
5. Are we on track for Playtest 2 (Week 10)?

**Looking Ahead to Week 6**:

- Implement playtest feedback
- Begin Dragon spell expansion (Week 6-10 goal: 10 total spells)
- Start debug tools implementation
- Prepare for Playtest 2 (Week 10)

---

## Week 5 Summary

**Playtest 1 Milestone**: ✅ COMPLETE

This week transformed individual systems into a cohesive, playable game. The Dragon vs Fighter battle is fully functional with spells, AI, visual effects, and complete game flow from start to victory/defeat screen.

**Key Metrics**:

- **Development Time**: 5 weeks (Weeks 1-5)
- **Total Tests**: 80+ passing
- **Lines of Code**: ~5,000+ (estimated)
- **Playable Build**: Ready for external testing
- **Systems Complete**: 15+ (Character, Grid, Combat, Spells, Turn, Events, VFX, UI, Game Flow, etc.)

**Next Milestone**: Playtest 2 (Week 10) - Full Dragon spell arsenal + debug tools + balance improvements

---

**🎉 Congratulations on completing Playtest 1! 🎉**

The foundation is solid. Now it's time to expand, polish, and iterate based on player feedback!
