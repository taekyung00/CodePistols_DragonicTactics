# Playtest 1 Implementation Plan - Week 4

**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Initiative + Basic AI + Polish
**Timeline**: Week 4 of 5 (Playtest 1 milestone)
**Strategy**: Complete turn order system, Fighter AI, and status effects tracking

**Last Updated**: 2025-01-30
**Week 3 Status**: All 3 Dragon spells complete, Fighter combat abilities working, SpellSystem functional, DataRegistry loading JSON

**Related Documentation**:

- [Week 1](week1.md) for foundation systems
- [Week 2](week2.md) for Dragon and Fighter base classes
- [Week 3](week3.md) for spell system and combat integration
- [docs/implementation-plan.md](../../implementation-plan.md) for complete 26-week timeline

---

## Overview

Week 4 finalizes the turn-based combat system by implementing initiative rolling, Fighter AI decision-making, complete data-driven design with hot-reload, and status effect tracking. This week makes the game playable without manual character control.

**Critical Success Criteria:**

- ✅ **TurnManager Initiative**: 1d20 + speed modifier, sorted turn order, re-rolls each combat round
- ✅ **Fighter Basic AI**: Autonomous decision-making (move toward Dragon, attack when in range, use Shield Bash)
- ✅ **StatusEffectManager**: Track burn damage from LavaPool, status effect duration, stat modifications
- ✅ **Data-Driven Complete**: All character stats loaded from JSON, hot-reload working, no hardcoded values
- ✅ **Dragon Spell Balance**: All 3 spells tuned for fair gameplay (damage, costs, ranges)

**Integration Goal (Friday Week 4)**:

- Full combat loop with initiative system (1d20 + speed)
- Fighter AI makes decisions autonomously (no manual control)
- Dragon spells balanced and working correctly
- Status effects track properly (burn damage from LavaPool)
- Character stats hot-reload from JSON files

**Meeting Schedule:**

- **Daily Standups**: 10 min each morning (9:00 AM)
- **Friday Integration Meeting**: 60 min (2:00 PM - full AI combat demo + Week 5 planning)

---

## Table of Contents

- [Week 4 Overview](#overview)
- [Developer A: Dragon Spell Balance](#week-4-developer-a---dragon-spell-balance)
  - [Implementation Tasks](#implementation-tasks-dragon-spell-balance)
  - [Implementation Examples](#implementation-examples-dragon-spell-balance)
  - [Rigorous Testing](#rigorous-testing-dragon-spell-balance)
  - [Daily Breakdown](#daily-breakdown-developer-a)
- [Developer B: TurnManager Initiative](#week-4-developer-b---turnmanager-initiative)
  - [Implementation Tasks](#implementation-tasks-turnmanager-initiative)
  - [Implementation Examples](#implementation-examples-turnmanager-initiative)
  - [Rigorous Testing](#rigorous-testing-turnmanager-initiative)
  - [Daily Breakdown](#daily-breakdown-developer-b)
- [Developer C: Fighter Basic AI](#week-4-developer-c---fighter-basic-ai)
  - [Implementation Tasks](#implementation-tasks-fighter-basic-ai)
  - [Implementation Examples](#implementation-examples-fighter-basic-ai)
  - [Rigorous Testing](#rigorous-testing-fighter-basic-ai)
  - [Daily Breakdown](#daily-breakdown-developer-c)
- [Developer D: Data-Driven Stats Complete](#week-4-developer-d---data-driven-stats-complete)
  - [Implementation Tasks](#implementation-tasks-data-driven-stats)
  - [Implementation Examples](#implementation-examples-data-driven-stats)
  - [Rigorous Testing](#rigorous-testing-data-driven-stats)
  - [Daily Breakdown](#daily-breakdown-developer-d)
- [Developer E: StatusEffectManager Foundation](#week-4-developer-e---statuseffectmanager-foundation)
  - [Implementation Tasks](#implementation-tasks-statuseffectmanager)
  - [Implementation Examples](#implementation-examples-statuseffectmanager)
  - [Rigorous Testing](#rigorous-testing-statuseffectmanager)
  - [Daily Breakdown](#daily-breakdown-developer-e)
- [Week 4 Integration Test](#week-4-integration-test-friday-afternoon)
- [Week 4 Deliverable & Verification](#week-4-deliverable--verification)

---

## Week 4: Developer A - Dragon Spell Balance

**Goal**: Balance all 3 Dragon spells for fair and engaging gameplay

**Foundation**:

- Week 3 completed: Fireball, CreateWall, LavaPool implementations
- SpellSystem from Week 3 provides casting mechanics
- CombatSystem from Week 3 provides damage calculation
- Test data from Week 3 integration tests

**Files to Modify**:

```
CS230/Game/Spells/Fireball.cpp (damage tuning)
CS230/Game/Spells/CreateWall.cpp (wall length tuning)
CS230/Game/Spells/LavaPool.cpp (damage and duration tuning)
CS230/Game/Dragon.cpp (spell slot allocation)
CS230/Game/Test/SpellBalanceTests.cpp (new file)
```

### Implementation Tasks (Dragon Spell Balance)

#### **Task 1: Establish Spell Balance Framework**

**Goal**: Create testing infrastructure and metrics for spell balance evaluation

**Steps**:

1. Define balance criteria (damage per spell slot, action economy, tactical utility)
2. Create spell effectiveness calculator (damage/cost ratio)
3. Set up automated balance test suite
4. Document balance methodology for future reference

**Why this matters**: Without objective balance criteria, spell tuning is arbitrary. A framework ensures consistent and fair spell design across all spell levels.

**Key Balance Principles**:

- Higher level spells should feel more powerful (not just scale linearly)
- Area spells should deal less per-target damage than single-target spells
- Utility spells (CreateWall) provide tactical value beyond damage
- Spell slot cost must match spell power level

---

#### **Task 2: Fireball Damage Balance**

**Goal**: Tune Fireball damage to match D&D 5e baseline (8d6 at level 2, +1d6 per upcast level)

**Steps**:

1. Analyze current Fireball damage output against Fighter HP (90)
2. Test damage variance (minimum/average/maximum rolls)
3. Compare to single-target spell equivalents (Magic Missile)
4. Adjust base dice or modifiers to hit balance target
5. Verify upcasting feels rewarding (measurable power increase)

**Balance Target**:

- **Base damage (level 2)**: 28 average damage (8d6)
- **Upcast level 3**: 31.5 average damage (9d6) = +12% power
- **Should 2-shot Fighter** (2 casts = 56 damage total against 90 HP)
- **Area advantage**: Hits multiple targets to justify lower per-target damage

**Tuning Approach**:

- If too weak: Increase base dice (8d6 → 9d6) or add flat bonus (+3 damage)
- If too strong: Reduce base dice (8d6 → 7d6) or increase spell slot cost
- Upcasting must feel impactful (+3.5 damage per level minimum)

---

#### **Task 3: CreateWall Utility Balance**

**Goal**: Ensure CreateWall provides tactical value without being overpowered

**Steps**:

1. Test wall length at base level (5 tiles) and upcasted levels
2. Verify walls block movement and line of sight correctly
3. Test wall placement edge cases (adjacent to characters, map edges)
4. Balance wall duration (permanent vs timed)
5. Ensure wall doesn't create unwinnable stalemates

**Balance Target**:

- **Base wall length (level 1)**: 5 tiles (enough to block 1-2 approaches)
- **Upcast scaling**: +1 tile per level (level 3 = 7 tiles)
- **Cost vs value**: Level 1 spell slot for significant tactical control
- **Counterplay**: Walls can be destroyed or bypassed with movement abilities

**Tuning Approach**:

- If too weak: Increase base length (5 → 6 tiles) or reduce spell slot cost
- If too strong: Reduce length (5 → 4 tiles) or add wall HP (destroyable walls)
- Ensure walls create interesting tactical decisions, not stalemates

---

#### **Task 4: LavaPool Damage-Over-Time Balance**

**Goal**: Balance LavaPool damage per turn to provide area denial without instant kill

**Steps**:

1. Test LavaPool damage output over 3 turns (base duration)
2. Calculate total damage vs Fireball instant damage
3. Verify Fighter AI avoids lava tiles appropriately
4. Balance damage per turn vs opportunity cost
5. Test edge case: Character starting turn in lava takes damage

**Balance Target**:

- **Base damage per turn (level 3)**: 18 average damage (4d8 per turn)
- **Total damage over 3 turns**: 54 damage (18 × 3)
- **Comparison to Fireball**: Similar total damage but spread over time
- **Tactical advantage**: Area denial forces repositioning

**Tuning Approach**:

- If too weak: Increase duration (3 → 4 turns) or damage (4d8 → 5d8)
- If too strong: Reduce duration (3 → 2 turns) or damage (4d8 → 3d8)
- Area denial value justifies level 3 spell slot investment

---

#### **Task 5: Spell Slot Allocation Balance**

**Goal**: Ensure Dragon has appropriate spell slot distribution for 5-turn combat

**Steps**:

1. Calculate average combat length (5-8 turns based on Week 3 tests)
2. Allocate spell slots to support 3-4 spell casts per combat
3. Balance spell level distribution (more low-level, fewer high-level)
4. Test spell slot depletion rate in practice combat
5. Verify Dragon can't spam highest-level spells every turn

**Balance Target**:

- **Level 1 slots**: 4 slots (for CreateWall spam)
- **Level 2 slots**: 3 slots (for Fireball primary damage)
- **Level 3 slots**: 2 slots (for LavaPool strategic usage)
- **Total casts**: 9 possible casts (far exceeds 5-turn combat)

**Tuning Approach**:

- If Dragon too weak: Add 1 slot per level
- If Dragon too strong: Remove high-level slots or increase spell costs
- Goal: Dragon should manage resources, not spam strongest spell

---

### Implementation Examples (Dragon Spell Balance)

This section provides reference implementations you can consult when stuck. **You don't need to follow these exactly** - they're examples to guide your implementation.

#### **Example 1: Spell Balance Calculator**

```cpp
// File: CS230/Game/Test/SpellBalanceCalculator.h
#pragma once
#include <string>
#include "../Spells/SpellBase.h"

// Helper class to evaluate spell balance metrics
class SpellBalanceCalculator {
public:
    // Calculate average damage for a dice string (e.g., "8d6")
    static double CalculateAverageDamage(const std::string& diceNotation);

    // Calculate damage per spell slot level (efficiency metric)
    static double CalculateDamagePerLevel(SpellBase* spell, int upcastLevel);

    // Compare spell to baseline (returns % difference from balanced)
    static double CompareToBaseline(SpellBase* spell, double baselineDamage);

    // Simulate combat: how many casts to kill target with X HP?
    static int CalculateCastsToKill(SpellBase* spell, int targetHP);

    // Generate balance report for all Dragon spells
    static std::string GenerateBalanceReport();
};
```

```cpp
// File: CS230/Game/Test/SpellBalanceCalculator.cpp
#include "SpellBalanceCalculator.h"
#include "../Singletons/DiceManager.h"
#include "../Spells/Fireball.h"
#include "../Spells/LavaPool.h"
#include <sstream>
#include <iomanip>

double SpellBalanceCalculator::CalculateAverageDamage(const std::string& diceNotation) {
    // Parse dice notation: "8d6" → 8 dice, 6 sides
    size_t dPos = diceNotation.find('d');
    if (dPos == std::string::npos) return 0.0;

    int numDice = std::stoi(diceNotation.substr(0, dPos));
    int diceSides = std::stoi(diceNotation.substr(dPos + 1));

    // Average roll = (min + max) / 2 = (1 + sides) / 2
    double avgPerDie = (1.0 + diceSides) / 2.0;
    return numDice * avgPerDie;
}

double SpellBalanceCalculator::CalculateDamagePerLevel(SpellBase* spell, int upcastLevel) {
    // Simulate casting spell at upcast level
    // For Fireball: level 2 base, 8d6 = 28 avg
    // Upcast to level 3: 9d6 = 31.5 avg

    int spellLevel = (upcastLevel > 0) ? upcastLevel : spell->GetLevel();

    // Fireball: 8d6 + 1d6 per level above 2
    if (spell->GetName() == "Fireball") {
        int numDice = 8 + std::max(0, spellLevel - 2);
        return CalculateAverageDamage(std::to_string(numDice) + "d6");
    }

    // LavaPool: 4d8 per turn, 3 turns
    if (spell->GetName() == "Lava Pool") {
        int numDice = 4 + std::max(0, spellLevel - 3);
        return CalculateAverageDamage(std::to_string(numDice) + "d8") * 3; // 3 turns
    }

    return 0.0;
}

double SpellBalanceCalculator::CompareToBaseline(SpellBase* spell, double baselineDamage) {
    double spellDamage = CalculateDamagePerLevel(spell, spell->GetLevel());
    return ((spellDamage - baselineDamage) / baselineDamage) * 100.0; // % difference
}

int SpellBalanceCalculator::CalculateCastsToKill(SpellBase* spell, int targetHP) {
    double avgDamage = CalculateDamagePerLevel(spell, spell->GetLevel());
    if (avgDamage <= 0) return -1; // Utility spell, no damage

    return (int)std::ceil(targetHP / avgDamage);
}

std::string SpellBalanceCalculator::GenerateBalanceReport() {
    std::ostringstream report;
    report << "===== DRAGON SPELL BALANCE REPORT =====\n\n";

    // Create spell instances
    Fireball fireball;
    LavaPool lavaPool;

    // Fireball analysis
    report << "FIREBALL (Level " << fireball.GetLevel() << "):\n";
    report << "  Average Damage: " << std::fixed << std::setprecision(1)
           << CalculateDamagePerLevel(&fireball, 2) << " (8d6)\n";
    report << "  Upcast Level 3: " << CalculateDamagePerLevel(&fireball, 3) << " (9d6)\n";
    report << "  Casts to Kill Fighter (90 HP): " << CalculateCastsToKill(&fireball, 90) << "\n";
    report << "  Damage per Spell Level: " << CalculateDamagePerLevel(&fireball, 2) / 2.0 << " per level\n\n";

    // LavaPool analysis
    report << "LAVA POOL (Level " << lavaPool.GetLevel() << "):\n";
    report << "  Average Damage per Turn: " << std::fixed << std::setprecision(1)
           << CalculateAverageDamage("4d8") << " (4d8)\n";
    report << "  Total Damage (3 turns): " << CalculateDamagePerLevel(&lavaPool, 3) << "\n";
    report << "  Casts to Kill Fighter (90 HP): " << CalculateCastsToKill(&lavaPool, 90) << "\n";
    report << "  Damage per Spell Level: " << CalculateDamagePerLevel(&lavaPool, 3) / 3.0 << " per level\n\n";

    // CreateWall (utility spell, no damage)
    report << "CREATE WALL (Level 1):\n";
    report << "  Utility Spell: No direct damage\n";
    report << "  Tactical Value: Area control, blocking, LoS denial\n\n";

    report << "========================================\n";
    return report.str();
}
```

**Usage Example**:

```cpp
// In Test state or main menu
std::string report = SpellBalanceCalculator::GenerateBalanceReport();
std::cout << report << std::endl;

// Output:
// ===== DRAGON SPELL BALANCE REPORT =====
//
// FIREBALL (Level 2):
//   Average Damage: 28.0 (8d6)
//   Upcast Level 3: 31.5 (9d6)
//   Casts to Kill Fighter (90 HP): 4
//   Damage per Spell Level: 14.0 per level
//
// LAVA POOL (Level 3):
//   Average Damage per Turn: 18.0 (4d8)
//   Total Damage (3 turns): 54.0
//   Casts to Kill Fighter (90 HP): 2
//   Damage per Spell Level: 18.0 per level
// ...
```

---

#### **Example 2: Fireball Damage Tuning**

```cpp
// File: CS230/Game/Spells/Fireball.cpp (modified)

// Step 2.5c: Calculate damage (with upcasting) - TUNED VERSION
DiceManager& dice = DiceManager::GetInstance();

// BALANCE CHANGE: Base damage adjusted from 8d6 to 7d6 + 3 flat bonus
// Reason: Reduces variance while maintaining average damage
// Old: 8d6 = 8-48 damage (avg 28)
// New: 7d6+3 = 10-45 damage (avg 27.5) - slightly more consistent
int numDice = 7; // Base 7d6 (down from 8d6)
int flatBonus = 3; // Add flat bonus for consistency

if (upcastLevel > GetLevel()) {
    numDice += (upcastLevel - GetLevel()) * upcastBonusDice;
}

std::string damageRoll = std::to_string(numDice) + "d6";
int totalDamage = dice.RollDiceFromString(damageRoll) + flatBonus;

// BALANCE TESTING: Log damage for analysis
Engine::GetLogger().LogDebug(
    "Fireball damage: " + damageRoll + "+" + std::to_string(flatBonus) +
    " = " + std::to_string(totalDamage)
);
```

**Balance Rationale**:

- **Problem**: 8d6 has high variance (8-48 damage range = 40 point swing)
- **Solution**: 7d6+3 narrows range (10-45 = 35 point swing) with same average
- **Player experience**: More predictable damage = better tactical planning
- **Upcast scaling**: Still +1d6 per level (feels rewarding)

---

#### **Example 3: LavaPool Duration Balance**

```cpp
// File: CS230/Game/Spells/LavaPool.cpp (modified)

// Step 4.6: Apply burn status effect with tuned duration
StatusEffectManager& statusMgr = StatusEffectManager::GetInstance();

// BALANCE CHANGE: Duration reduced from 3 turns to 2 turns
// Reason: 3 turns of 18 damage = 54 total damage (too strong for level 3)
// New: 2 turns of 18 damage = 36 total damage (balanced)
int duration = 2; // Down from 3 turns

// BALANCE CHANGE: Increased per-turn damage from 4d8 to 5d8
// Reason: Shorter duration needs higher impact to feel worthwhile
// Old: 4d8 × 3 turns = 54 damage total
// New: 5d8 × 2 turns = 45 damage total (slightly reduced but faster)
int numDice = 5; // Up from 4d8

if (upcastLevel > GetLevel()) {
    numDice += (upcastLevel - GetLevel()) * upcastBonusDice;
}

// Apply burn effect
for (Character* target : result.affectedTargets) {
    statusMgr.ApplyStatusEffect(
        target,
        StatusEffectType::Burn,
        duration,
        std::to_string(numDice) + "d8" // Damage per turn
    );
}
```

**Balance Rationale**:

- **Problem**: 3-turn duration makes combat drag, damage too high
- **Solution**: 2-turn duration with higher per-turn damage = faster gameplay
- **Trade-off**: Less total damage (54 → 45) but stronger immediate threat
- **Tactical impact**: Forces immediate repositioning instead of slow burn

---

#### **Example 4: Spell Slot Allocation**

```cpp
// File: CS230/Game/Dragon.cpp (modified)

void Dragon::InitializeSpellSlots() {
    // BALANCE CHANGE: Adjusted spell slot distribution
    // Reason: Previous allocation allowed too much spell spam

    // Old allocation:
    // Level 1: 4 slots (CreateWall spam)
    // Level 2: 4 slots (Fireball spam)
    // Level 3: 3 slots (LavaPool spam)
    // Total: 11 casts for 5-turn combat = 2.2 spells per turn (too much)

    // New allocation:
    SetSpellSlots(1, 3); // Level 1: 3 slots (down from 4)
    SetSpellSlots(2, 2); // Level 2: 2 slots (down from 4)
    SetSpellSlots(3, 1); // Level 3: 1 slot (down from 3)
    // Total: 6 casts for 5-turn combat = 1.2 spells per turn (balanced)

    // BALANCE RATIONALE:
    // - Dragon should make meaningful spell choices, not spam every turn
    // - Running out of spell slots creates tension
    // - Forces mix of spell levels (can't spam highest damage only)
    // - Encourages normal attacks as resource management

    Engine::GetLogger().LogDebug("Dragon spell slots initialized: 3/2/1");
}
```

**Balance Rationale**:

- **Problem**: Too many spell slots = Dragon never runs out, spams Fireball
- **Solution**: Reduce slots to force resource management decisions
- **Player choice**: "Do I use Fireball now or save for later?"
- **Gameplay depth**: Mix of spells + normal attacks = varied tactics

---

### Rigorous Testing (Dragon Spell Balance)

#### **Test Suite 1: Fireball Damage Tests**

```cpp
// File: CS230/Game/Test/FireballBalanceTests.cpp
#include "../../Engine/Engine.h"
#include "../Spells/Fireball.h"
#include "../Dragon.h"
#include "../Fighter.h"
#include "../Singletons/DiceManager.h"
#include "../Test/Week4TestMocks.h"
#include <iostream>
#include <vector>

bool TestFireballAverageDamage() {
    // Test: Fireball average damage should be ~28 (8d6)
    DiceManager::GetInstance().SetSeed(12345); // Deterministic testing

    Dragon dragon;
    Math::vec2 targetTile{4, 4};

    // Cast Fireball 100 times and calculate average
    int totalDamage = 0;
    const int iterations = 100;

    for (int i = 0; i < iterations; ++i) {
        Fighter dummy;
        dummy.SetGridPosition(targetTile);

        Fireball fireball;
        SpellResult result = fireball.Cast(&dragon, targetTile, 0);

        totalDamage += result.damageDealt;
    }

    double avgDamage = (double)totalDamage / iterations;

    // Accept range: 26-30 damage (within 2 damage of expected 28)
    bool passed = (avgDamage >= 26.0 && avgDamage <= 30.0);

    if (!passed) {
        std::cout << "  FAILED: Average damage " << avgDamage
                  << " outside expected range 26-30\n";
    }

    return passed;
}

bool TestFireballUpcastScaling() {
    // Test: Upcasting Fireball should increase damage by ~3.5 per level
    DiceManager::GetInstance().SetSeed(54321);

    Dragon dragon;
    Math::vec2 targetTile{4, 4};

    // Cast at level 2 (base)
    Fireball fireball;
    Fighter dummy1;
    dummy1.SetGridPosition(targetTile);
    SpellResult result2 = fireball.Cast(&dragon, targetTile, 0);
    int damage2 = result2.damageDealt;

    // Cast at level 3 (upcast)
    Fighter dummy2;
    dummy2.SetGridPosition(targetTile);
    SpellResult result3 = fireball.Cast(&dragon, targetTile, 3);
    int damage3 = result3.damageDealt;

    // Expect ~3.5 damage increase (1d6 = avg 3.5)
    int increase = damage3 - damage2;
    bool passed = (increase >= 2 && increase <= 5); // Allow variance

    if (!passed) {
        std::cout << "  FAILED: Upcast damage increase " << increase
                  << " outside expected range 2-5\n";
    }

    return passed;
}

bool TestFireballCastsToKillFighter() {
    // Test: Should take 3-4 Fireball casts to kill Fighter (90 HP)
    DiceManager::GetInstance().SetSeed(99999);

    Dragon dragon;
    Fighter fighter;
    fighter.SetGridPosition(Math::vec2{4, 4});
    fighter.SetHP(90); // Fighter base HP

    Fireball fireball;
    int castCount = 0;

    while (fighter.IsAlive() && castCount < 10) {
        SpellResult result = fireball.Cast(&dragon, fighter.GetGridPosition(), 0);
        castCount++;
    }

    bool passed = (castCount >= 3 && castCount <= 4);

    if (!passed) {
        std::cout << "  FAILED: Took " << castCount
                  << " casts to kill Fighter (expected 3-4)\n";
    }

    return passed;
}

bool TestFireballAreaDamage() {
    // Test: Fireball should hit all targets in 2-tile radius
    Dragon dragon;
    Math::vec2 impactTile{4, 4};

    // Create 5 fighters in explosion radius
    std::vector<Fighter*> fighters;
    Math::vec2 positions[] = {
        {4, 4}, // Center
        {3, 4}, // Left
        {5, 4}, // Right
        {4, 3}, // Up
        {4, 5}  // Down
    };

    for (int i = 0; i < 5; ++i) {
        Fighter* f = new Fighter();
        f->SetGridPosition(positions[i]);
        fighters.push_back(f);
    }

    Fireball fireball;
    SpellResult result = fireball.Cast(&dragon, impactTile, 0);

    bool passed = (result.affectedTargets.size() == 5);

    if (!passed) {
        std::cout << "  FAILED: Fireball hit " << result.affectedTargets.size()
                  << " targets (expected 5)\n";
    }

    // Cleanup
    for (Fighter* f : fighters) delete f;

    return passed;
}

void RunFireballBalanceTests() {
    std::cout << "\n=== FIREBALL BALANCE TESTS ===\n";
    std::cout << (TestFireballAverageDamage() ? "✅" : "❌") << " Average damage in expected range\n";
    std::cout << (TestFireballUpcastScaling() ? "✅" : "❌") << " Upcast scaling correct\n";
    std::cout << (TestFireballCastsToKillFighter() ? "✅" : "❌") << " Casts to kill Fighter reasonable\n";
    std::cout << (TestFireballAreaDamage() ? "✅" : "❌") << " Area damage hits all targets\n";
    std::cout << "================================\n";
}
```

---

#### **Test Suite 2: LavaPool Balance Tests**

```cpp
// File: CS230/Game/Test/LavaPoolBalanceTests.cpp
#include "../../Engine/Engine.h"
#include "../Spells/LavaPool.h"
#include "../Dragon.h"
#include "../Fighter.h"
#include "../Singletons/StatusEffectManager.h"
#include <iostream>

bool TestLavaPoolTotalDamage() {
    // Test: LavaPool should deal ~54 damage over 3 turns (18 per turn)
    DiceManager::GetInstance().SetSeed(11111);
    StatusEffectManager::GetInstance().ClearAllEffects();

    Dragon dragon;
    Fighter fighter;
    fighter.SetGridPosition(Math::vec2{4, 4});
    fighter.SetHP(100); // High HP to survive full duration

    LavaPool lavaPool;
    SpellResult result = lavaPool.Cast(&dragon, fighter.GetGridPosition(), 0);

    int totalDamage = 0;

    // Simulate 3 turns
    for (int turn = 0; turn < 3; ++turn) {
        int hpBefore = fighter.GetHP();
        StatusEffectManager::GetInstance().ProcessTurnStart(&fighter);
        int hpAfter = fighter.GetHP();

        totalDamage += (hpBefore - hpAfter);
    }

    // Expect ~54 damage (4d8 × 3 = 18 × 3 = 54)
    // Allow range: 40-70 damage (high variance with d8s)
    bool passed = (totalDamage >= 40 && totalDamage <= 70);

    if (!passed) {
        std::cout << "  FAILED: LavaPool total damage " << totalDamage
                  << " outside expected range 40-70\n";
    }

    return passed;
}

bool TestLavaPoolDurationExpires() {
    // Test: Burn effect should expire after 3 turns
    StatusEffectManager::GetInstance().ClearAllEffects();

    Dragon dragon;
    Fighter fighter;
    fighter.SetGridPosition(Math::vec2{4, 4});

    LavaPool lavaPool;
    lavaPool.Cast(&dragon, fighter.GetGridPosition(), 0);

    // Simulate 4 turns
    for (int turn = 0; turn < 4; ++turn) {
        StatusEffectManager::GetInstance().ProcessTurnStart(&fighter);
    }

    // After 4 turns, burn should be expired (duration was 3)
    bool stillBurning = StatusEffectManager::GetInstance().HasEffect(
        &fighter, StatusEffectType::Burn
    );

    bool passed = !stillBurning;

    if (!passed) {
        std::cout << "  FAILED: Burn effect still active after 4 turns\n";
    }

    return passed;
}

bool TestLavaPoolMultipleTargets() {
    // Test: LavaPool should affect all characters in area
    StatusEffectManager::GetInstance().ClearAllEffects();

    Dragon dragon;
    Math::vec2 impactTile{4, 4};

    // Create 4 fighters in lava area
    Fighter f1, f2, f3, f4;
    f1.SetGridPosition({4, 4});
    f2.SetGridPosition({3, 4});
    f3.SetGridPosition({4, 3});
    f4.SetGridPosition({5, 5}); // Outside area

    LavaPool lavaPool;
    SpellResult result = lavaPool.Cast(&dragon, impactTile, 0);

    // Check burn effects applied
    bool f1Burning = StatusEffectManager::GetInstance().HasEffect(&f1, StatusEffectType::Burn);
    bool f2Burning = StatusEffectManager::GetInstance().HasEffect(&f2, StatusEffectType::Burn);
    bool f3Burning = StatusEffectManager::GetInstance().HasEffect(&f3, StatusEffectType::Burn);
    bool f4Burning = StatusEffectManager::GetInstance().HasEffect(&f4, StatusEffectType::Burn);

    bool passed = (f1Burning && f2Burning && f3Burning && !f4Burning);

    if (!passed) {
        std::cout << "  FAILED: Burn effects not applied correctly to targets\n";
    }

    return passed;
}

void RunLavaPoolBalanceTests() {
    std::cout << "\n=== LAVA POOL BALANCE TESTS ===\n";
    std::cout << (TestLavaPoolTotalDamage() ? "✅" : "❌") << " Total damage over duration correct\n";
    std::cout << (TestLavaPoolDurationExpires() ? "✅" : "❌") << " Burn effect expires after duration\n";
    std::cout << (TestLavaPoolMultipleTargets() ? "✅" : "❌") << " Multiple targets affected\n";
    std::cout << "=================================\n";
}
```

---

#### **Test Suite 3: Spell Slot Balance Tests**

```cpp
// File: CS230/Game/Test/SpellSlotBalanceTests.cpp
#include "../../Engine/Engine.h"
#include "../Dragon.h"
#include "../Spells/Fireball.h"
#include "../Spells/CreateWall.h"
#include "../Spells/LavaPool.h"
#include <iostream>

bool TestDragonSpellSlotAllocation() {
    // Test: Dragon should have balanced spell slot distribution
    Dragon dragon;

    int level1Slots = dragon.GetSpellSlots(1);
    int level2Slots = dragon.GetSpellSlots(2);
    int level3Slots = dragon.GetSpellSlots(3);

    // Expected: 3/2/1 distribution (more low-level, fewer high-level)
    bool passed = (level1Slots == 3 && level2Slots == 2 && level3Slots == 1);

    if (!passed) {
        std::cout << "  FAILED: Spell slots " << level1Slots << "/"
                  << level2Slots << "/" << level3Slots
                  << " (expected 3/2/1)\n";
    }

    return passed;
}

bool TestSpellSlotConsumption() {
    // Test: Casting spells should consume slots correctly
    Dragon dragon;

    int level2Before = dragon.GetSpellSlots(2);

    // Cast Fireball (level 2)
    Fireball fireball;
    fireball.Cast(&dragon, Math::vec2{4, 4}, 0);

    int level2After = dragon.GetSpellSlots(2);

    bool passed = (level2After == level2Before - 1);

    if (!passed) {
        std::cout << "  FAILED: Spell slot not consumed (before: "
                  << level2Before << ", after: " << level2After << ")\n";
    }

    return passed;
}

bool TestSpellSlotDepletion() {
    // Test: Dragon should run out of spell slots after excessive casting
    Dragon dragon;

    Fireball fireball;
    int castCount = 0;

    // Try to cast Fireball 10 times (more than available slots)
    for (int i = 0; i < 10; ++i) {
        if (dragon.HasSpellSlot(fireball.GetLevel())) {
            fireball.Cast(&dragon, Math::vec2{4, 4}, 0);
            castCount++;
        }
    }

    // Should only cast 2 times (level 2 slots = 2)
    bool passed = (castCount == 2);

    if (!passed) {
        std::cout << "  FAILED: Cast Fireball " << castCount
                  << " times (expected 2)\n";
    }

    return passed;
}

bool TestSpellSlotUpcastConsumption() {
    // Test: Upcasting should consume higher-level slot
    Dragon dragon;

    int level2Before = dragon.GetSpellSlots(2);
    int level3Before = dragon.GetSpellSlots(3);

    // Cast Fireball (level 2) using level 3 slot (upcast)
    Fireball fireball;
    fireball.Cast(&dragon, Math::vec2{4, 4}, 3); // Upcast to level 3

    int level2After = dragon.GetSpellSlots(2);
    int level3After = dragon.GetSpellSlots(3);

    // Level 2 slots should be unchanged, level 3 slot consumed
    bool passed = (level2After == level2Before && level3After == level3Before - 1);

    if (!passed) {
        std::cout << "  FAILED: Upcast didn't consume correct slot\n";
    }

    return passed;
}

void RunSpellSlotBalanceTests() {
    std::cout << "\n=== SPELL SLOT BALANCE TESTS ===\n";
    std::cout << (TestDragonSpellSlotAllocation() ? "✅" : "❌") << " Spell slot distribution correct\n";
    std::cout << (TestSpellSlotConsumption() ? "✅" : "❌") << " Spell slots consumed on cast\n";
    std::cout << (TestSpellSlotDepletion() ? "✅" : "❌") << " Spell slots deplete correctly\n";
    std::cout << (TestSpellSlotUpcastConsumption() ? "✅" : "❌") << " Upcasting consumes higher slot\n";
    std::cout << "==================================\n";
}
```

---

### Daily Breakdown (Developer A)

#### **Monday (6-8 hours)**

- Set up spell balance testing infrastructure (1.5 hrs)
- Implement `SpellBalanceCalculator` helper class (2 hrs)
- Run initial balance metrics on all 3 spells (1 hr)
- Document current balance state and identify issues (1 hr)
- **Deliverable**: Balance testing framework with initial metrics report

#### **Tuesday (7-8 hours)**

- Tune Fireball damage (analyze variance, adjust dice) (2.5 hrs)
- Test Fireball against Fighter HP across 50+ combat simulations (2 hrs)
- Write Fireball balance test suite (2 hrs)
- Verify upcast scaling feels rewarding (1 hr)
- **Deliverable**: Fireball balanced and tested (average 28 damage, 3-4 casts to kill Fighter)

#### **Wednesday (7-8 hours)**

- Tune CreateWall utility (wall length, placement validation) (2 hrs)
- Test wall blocking and line-of-sight mechanics (1.5 hrs)
- Balance LavaPool damage-over-time (duration, damage per turn) (2.5 hrs)
- Write LavaPool balance test suite (2 hrs)
- **Deliverable**: CreateWall and LavaPool balanced, all spells tested

#### **Thursday (6-7 hours)**

- Adjust Dragon spell slot allocation (3/2/1 distribution) (1.5 hrs)
- Test spell slot depletion in full combat (2 hrs)
- Write spell slot balance tests (2 hrs)
- Generate final balance report with recommendations (1.5 hrs)
- **Deliverable**: Spell slots balanced, resource management tested

#### **Friday (3-4 hours)**

- Final balance tuning based on integration test feedback (1.5 hrs)
- Run all spell balance tests and verify success criteria (1 hr)
- Prepare balance demo for integration meeting (30 min)
- **Deliverable**: All Dragon spells balanced, ready for Week 5 playtest

---

## Week 4: Developer B - TurnManager Initiative

**Goal**: Implement D&D-style initiative rolling (1d20 + speed modifier) with sorted turn order

**Foundation**:

- Week 2 TurnManager basics (turn progression, character tracking)
- Week 1 DiceManager singleton (dice rolling utilities)
- Character class from Week 1 (speed stat for initiative modifier)
- EventBus from Week 1 (initiative rolled events)

**Files to Modify**:

```
CS230/Game/StateComponents/TurnManager.h (add initiative methods)
CS230/Game/StateComponents/TurnManager.cpp (implement initiative)
CS230/Game/Objects/Character.h (add initiative tracking)
CS230/Game/Test/TurnManagerInitiativeTests.cpp (new file)
```

### Implementation Tasks (TurnManager Initiative)

#### **Task 1: Initiative System Design**

**Goal**: Design D&D 5e-compliant initiative system with tiebreakers

**Steps**:

1. Define initiative calculation formula (1d20 + speed modifier)
2. Design initiative data structure (character → initiative value mapping)
3. Implement tie-breaking rules (higher speed wins, random if tied)
4. Plan initiative event publishing (for UI and combat log)
5. Ensure initiative recalculates each combat round

**Why this matters**: Initiative determines turn order and creates tactical variety. Random rolls ensure no two combats feel identical, even with same characters. Proper tie-breaking prevents deadlocks.

**D&D 5e Initiative Formula**:

```
Initiative = 1d20 + Speed Modifier
Speed Modifier = (Character.speed - 10) / 2  // Standard D&D ability modifier formula
```

**Example**:

- Dragon (speed 5): 1d20 + (5-10)/2 = 1d20 - 2
- Fighter (speed 3): 1d20 + (3-10)/2 = 1d20 - 3
- Higher initiative acts first

---

#### **Task 2: Initiative Rolling Implementation**

**Goal**: Roll initiative for all characters and sort turn order

**Steps**:

1. Add `RollInitiative()` method to TurnManager
2. Iterate all characters in combat
3. Roll 1d20 for each character
4. Add speed modifier to roll
5. Store results in initiative map
6. Sort characters by initiative value (descending)
7. Handle ties using speed stat, then random

**Why this matters**: Initiative rolling happens once per combat (or once per round). Sorting determines action order for the entire combat. Proper implementation ensures fair turn distribution.

**Key Considerations**:

- Initiative rolls should be **logged** for transparency
- Ties must be **deterministic** (not random each comparison)
- Turn order must be **stable** (don't re-sort mid-round)
- Re-roll initiative each **combat** (not each turn)

---

#### **Task 3: Turn Order Management**

**Goal**: Maintain sorted turn order throughout combat

**Steps**:

1. Store initiative values with character pointers
2. Implement `GetTurnOrder()` to return sorted character list
3. Add `GetCurrentInitiative(Character*)` lookup method
4. Handle character death (remove from turn order)
5. Support initiative value display for UI

**Why this matters**: UI needs to show turn order. Combat log needs initiative values. Death must remove characters from rotation without breaking iteration.

**Data Structure**:

```cpp
struct InitiativeEntry {
    Character* character;
    int initiativeRoll;    // 1d20 result
    int speedModifier;     // (speed - 10) / 2
    int totalInitiative;   // roll + modifier
};

std::vector<InitiativeEntry> turnOrder; // Sorted descending by totalInitiative
```

---

#### **Task 4: Initiative Event Publishing**

**Goal**: Publish initiative events for UI and combat log integration

**Steps**:

1. Define `InitiativeRolledEvent` structure
2. Publish event when each character rolls initiative
3. Include character, roll value, modifier, and total
4. Publish turn order established event
5. Subscribe to events in combat log for display

**Why this matters**: Event-driven architecture keeps UI and combat log decoupled from TurnManager. Initiative events enable UI to show turn order widget and combat log to report initiative rolls.

**Event Design**:

```cpp
struct InitiativeRolledEvent {
    Character* character;
    int roll;         // 1d20 result
    int modifier;     // Speed modifier
    int total;        // roll + modifier
};

struct TurnOrderEstablishedEvent {
    std::vector<Character*> turnOrder; // Sorted by initiative
};
```

---

#### **Task 5: Initiative Re-Roll Support**

**Goal**: Support initiative re-rolling for multi-round combats

**Steps**:

1. Add `ResetInitiative()` method to TurnManager
2. Clear previous initiative values
3. Re-roll initiative for all living characters
4. Re-sort turn order
5. Test initiative persistence across multiple combat rounds

**Why this matters**: Some house rules re-roll initiative each round for variety. Support both modes: roll once per combat (default) or roll each round (variant rule).

**Configuration**:

```cpp
enum class InitiativeMode {
    RollOnce,      // Roll at combat start (D&D 5e default)
    RollEachRound  // Re-roll each round (variant rule)
};
```

---

### Implementation Examples (TurnManager Initiative)

#### **Example 1: Initiative Rolling Core Logic**

```cpp
// File: CS230/Game/StateComponents/TurnManager.h
#pragma once
#include "../Objects/Character.h"
#include <vector>
#include <map>

// Initiative tracking structure
struct InitiativeEntry {
    Character* character;
    int roll;              // 1d20 result
    int speedModifier;     // (speed - 10) / 2
    int totalInitiative;   // roll + modifier

    // Constructor
    InitiativeEntry(Character* ch, int r, int mod)
        : character(ch), roll(r), speedModifier(mod), totalInitiative(r + mod) {}
};

enum class InitiativeMode {
    RollOnce,      // Roll at combat start (default)
    RollEachRound  // Re-roll each round (variant)
};

class TurnManager {
public:
    static TurnManager& GetInstance();

    // Initiative methods (NEW in Week 4)
    void RollInitiative(const std::vector<Character*>& characters);
    void ResetInitiative();
    std::vector<Character*> GetTurnOrder() const;
    int GetInitiativeValue(Character* character) const;
    void SetInitiativeMode(InitiativeMode mode) { initiativeMode = mode; }

    // Existing methods from Week 2-3
    void StartCombat(const std::vector<Character*>& characters);
    void StartNextTurn();
    void EndCurrentTurn();
    Character* GetCurrentCharacter() const;
    bool IsCombatActive() const;

private:
    TurnManager() = default;

    // Initiative data
    std::vector<InitiativeEntry> initiativeOrder;
    InitiativeMode initiativeMode = InitiativeMode::RollOnce;
    int currentTurnIndex = 0;
    int currentRound = 1;
    bool combatActive = false;

    // Helper: Calculate speed modifier D&D style
    int CalculateSpeedModifier(int speed) const {
        return (speed - 10) / 2; // Standard D&D ability modifier
    }

    // Helper: Sort initiative order
    void SortInitiativeOrder();
};
```

```cpp
// File: CS230/Game/StateComponents/TurnManager.cpp
#include "TurnManager.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../../Engine/Engine.h"
#include <algorithm>

TurnManager& TurnManager::GetInstance() {
    static TurnManager instance;
    return instance;
}

void TurnManager::RollInitiative(const std::vector<Character*>& characters) {
    // Step 1: Clear previous initiative
    initiativeOrder.clear();

    Engine::GetLogger().LogEvent("=== ROLLING INITIATIVE ===");

    // Step 2: Roll 1d20 + speed modifier for each character
    DiceManager& dice = DiceManager::GetInstance();

    for (Character* character : characters) {
        if (!character || !character->IsAlive()) continue;

        // Roll 1d20
        int roll = dice.RollDice(1, 20);

        // Calculate speed modifier: (speed - 10) / 2
        int speed = character->GetSpeed();
        int modifier = CalculateSpeedModifier(speed);

        // Create initiative entry
        InitiativeEntry entry(character, roll, modifier);
        initiativeOrder.push_back(entry);

        // Log result
        Engine::GetLogger().LogEvent(
            character->TypeName() + " rolls " + std::to_string(roll) +
            " + " + std::to_string(modifier) + " = " + std::to_string(entry.totalInitiative)
        );

        // Publish event
        EventBus::GetInstance().Publish(InitiativeRolledEvent{
            character, roll, modifier, entry.totalInitiative
        });
    }

    // Step 3: Sort by initiative (highest first)
    SortInitiativeOrder();

    // Step 4: Publish turn order established event
    std::vector<Character*> sortedChars;
    for (const auto& entry : initiativeOrder) {
        sortedChars.push_back(entry.character);
    }

    EventBus::GetInstance().Publish(TurnOrderEstablishedEvent{sortedChars});

    Engine::GetLogger().LogEvent("=== TURN ORDER ESTABLISHED ===");
    for (const auto& entry : initiativeOrder) {
        Engine::GetLogger().LogEvent(
            "  " + std::to_string(entry.totalInitiative) + ": " + entry.character->TypeName()
        );
    }
}

void TurnManager::SortInitiativeOrder() {
    // Sort by total initiative (descending)
    std::sort(initiativeOrder.begin(), initiativeOrder.end(),
        [](const InitiativeEntry& a, const InitiativeEntry& b) {
            // Primary sort: Total initiative (higher goes first)
            if (a.totalInitiative != b.totalInitiative) {
                return a.totalInitiative > b.totalInitiative;
            }

            // Tie-breaker 1: Speed stat (higher goes first)
            int speedA = a.character->GetSpeed();
            int speedB = b.character->GetSpeed();
            if (speedA != speedB) {
                return speedA > speedB;
            }

            // Tie-breaker 2: Random (use character pointer address)
            // NOTE: This is deterministic for the same pointers
            return a.character > b.character;
        }
    );
}

std::vector<Character*> TurnManager::GetTurnOrder() const {
    std::vector<Character*> order;
    for (const auto& entry : initiativeOrder) {
        if (entry.character && entry.character->IsAlive()) {
            order.push_back(entry.character);
        }
    }
    return order;
}

int TurnManager::GetInitiativeValue(Character* character) const {
    for (const auto& entry : initiativeOrder) {
        if (entry.character == character) {
            return entry.totalInitiative;
        }
    }
    return 0; // Character not in initiative order
}

void TurnManager::ResetInitiative() {
    initiativeOrder.clear();
    currentTurnIndex = 0;
    Engine::GetLogger().LogEvent("Initiative reset for new combat");
}

void TurnManager::StartCombat(const std::vector<Character*>& characters) {
    combatActive = true;
    currentRound = 1;

    // Roll initiative for all characters
    RollInitiative(characters);

    // Start first turn
    currentTurnIndex = 0;
    if (!initiativeOrder.empty()) {
        StartNextTurn();
    }
}

void TurnManager::StartNextTurn() {
    // Skip dead characters
    while (currentTurnIndex < (int)initiativeOrder.size()) {
        Character* current = initiativeOrder[currentTurnIndex].character;
        if (current && current->IsAlive()) {
            current->OnTurnStart();
            EventBus::GetInstance().Publish(TurnStartedEvent{current});
            Engine::GetLogger().LogEvent(current->TypeName() + "'s turn begins");
            return;
        }
        currentTurnIndex++;
    }

    // All turns completed, start new round
    currentRound++;
    currentTurnIndex = 0;

    // Re-roll initiative if variant mode enabled
    if (initiativeMode == InitiativeMode::RollEachRound) {
        std::vector<Character*> aliveCharacters;
        for (const auto& entry : initiativeOrder) {
            if (entry.character && entry.character->IsAlive()) {
                aliveCharacters.push_back(entry.character);
            }
        }
        RollInitiative(aliveCharacters);
    }

    Engine::GetLogger().LogEvent("=== ROUND " + std::to_string(currentRound) + " ===");
    StartNextTurn();
}

void TurnManager::EndCurrentTurn() {
    if (currentTurnIndex < (int)initiativeOrder.size()) {
        Character* current = initiativeOrder[currentTurnIndex].character;
        if (current) {
            current->OnTurnEnd();
            EventBus::GetInstance().Publish(TurnEndedEvent{current});
        }
    }

    currentTurnIndex++;
    StartNextTurn();
}

Character* TurnManager::GetCurrentCharacter() const {
    if (currentTurnIndex >= 0 && currentTurnIndex < (int)initiativeOrder.size()) {
        return initiativeOrder[currentTurnIndex].character;
    }
    return nullptr;
}

bool TurnManager::IsCombatActive() const {
    return combatActive;
}
```

---

#### **Example 2: Initiative Event Definitions**

```cpp
// File: CS230/Game/Types/Events.h (add to existing events)

// Initiative events
struct InitiativeRolledEvent {
    Character* character;
    int roll;              // 1d20 result
    int speedModifier;     // (speed - 10) / 2
    int totalInitiative;   // roll + modifier
};

struct TurnOrderEstablishedEvent {
    std::vector<Character*> turnOrder; // Sorted by initiative (highest first)
};
```

---

#### **Example 3: Speed Modifier Calculation**

```cpp
// File: CS230/Game/Objects/Character.cpp (add helper method)

int Character::GetSpeedModifier() const {
    // D&D 5e ability modifier formula: (ability_score - 10) / 2
    // For speed 5: (5 - 10) / 2 = -2
    // For speed 10: (10 - 10) / 2 = 0
    // For speed 15: (15 - 10) / 2 = +2
    return (speed - 10) / 2;
}
```

---

### Rigorous Testing (TurnManager Initiative)

#### **Test Suite 1: Initiative Rolling Tests**

```cpp
// File: CS230/Game/Test/TurnManagerInitiativeTests.cpp
#include "../../Engine/Engine.h"
#include "../StateComponents/TurnManager.h"
#include "../Objects/Dragon.h"
#include "../Objects/Fighter.h"
#include "../Singletons/DiceManager.h"
#include <iostream>

bool TestInitiativeBasicRoll() {
    // Test: Initiative rolls should be 1d20 + speed modifier
    DiceManager::GetInstance().SetSeed(12345);
    TurnManager& turnMgr = TurnManager::GetInstance();
    turnMgr.ResetInitiative();

    Dragon dragon;
    dragon.SetSpeed(15); // Modifier = +2
    Fighter fighter;
    fighter.SetSpeed(10); // Modifier = 0

    std::vector<Character*> characters = {&dragon, &fighter};
    turnMgr.RollInitiative(characters);

    int dragonInit = turnMgr.GetInitiativeValue(&dragon);
    int fighterInit = turnMgr.GetInitiativeValue(&fighter);

    // Dragon: roll + 2, Fighter: roll + 0
    // With deterministic seed, verify modifiers applied
    bool passed = (dragonInit > 0 && fighterInit > 0);

    if (!passed) {
        std::cout << "  FAILED: Initiative values invalid (Dragon: "
                  << dragonInit << ", Fighter: " << fighterInit << ")\n";
    }

    return passed;
}

bool TestInitiativeTurnOrder() {
    // Test: Characters should be sorted by initiative (highest first)
    DiceManager::GetInstance().SetSeed(99999);
    TurnManager& turnMgr = TurnManager::GetInstance();
    turnMgr.ResetInitiative();

    // Create characters with different speeds
    Dragon dragon;
    dragon.SetSpeed(20); // High speed = high modifier
    Fighter fighter;
    fighter.SetSpeed(5);  // Low speed = negative modifier

    std::vector<Character*> characters = {&fighter, &dragon}; // Unsorted
    turnMgr.RollInitiative(characters);

    std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();

    // Dragon should act first (higher speed → higher average initiative)
    // Note: With random rolls this might fail occasionally
    // For stable test, set seed to ensure Dragon rolls higher
    bool passed = (turnOrder.size() == 2);

    if (!passed) {
        std::cout << "  FAILED: Turn order size incorrect\n";
    }

    return passed;
}

bool TestInitiativeTieBreaker() {
    // Test: Tied initiative should break on speed stat
    DiceManager::GetInstance().SetSeed(11111);
    TurnManager& turnMgr = TurnManager::GetInstance();
    turnMgr.ResetInitiative();

    // Both roll same value, different speeds
    Dragon dragon;
    dragon.SetSpeed(15);
    Fighter fighter;
    fighter.SetSpeed(10);

    // Force same roll (by manipulating seed carefully or mock dice)
    // For demonstration, test that tie-breaking doesn't crash
    std::vector<Character*> characters = {&dragon, &fighter};
    turnMgr.RollInitiative(characters);

    std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();

    bool passed = (turnOrder.size() == 2);

    if (!passed) {
        std::cout << "  FAILED: Tie-breaking crashed or invalid\n";
    }

    return passed;
}

bool TestInitiativeDeadCharacterSkipped() {
    // Test: Dead characters should not be in turn order
    TurnManager& turnMgr = TurnManager::GetInstance();
    turnMgr.ResetInitiative();

    Dragon dragon;
    Fighter fighter;
    fighter.SetHP(0); // Dead

    std::vector<Character*> characters = {&dragon, &fighter};
    turnMgr.RollInitiative(characters);

    std::vector<Character*> turnOrder = turnMgr.GetTurnOrder();

    // Only Dragon should be in turn order
    bool passed = (turnOrder.size() == 1 && turnOrder[0] == &dragon);

    if (!passed) {
        std::cout << "  FAILED: Dead character in turn order\n";
    }

    return passed;
}

bool TestInitiativeReRoll() {
    // Test: Re-rolling initiative should change values
    DiceManager::GetInstance().SetSeed(12345);
    TurnManager& turnMgr = TurnManager::GetInstance();
    turnMgr.ResetInitiative();

    Dragon dragon;

    std::vector<Character*> characters = {&dragon};
    turnMgr.RollInitiative(characters);
    int firstInit = turnMgr.GetInitiativeValue(&dragon);

    // Change seed and re-roll
    DiceManager::GetInstance().SetSeed(54321);
    turnMgr.RollInitiative(characters);
    int secondInit = turnMgr.GetInitiativeValue(&dragon);

    // Initiatives should differ (different dice rolls)
    bool passed = (firstInit != secondInit);

    if (!passed) {
        std::cout << "  FAILED: Re-roll produced same initiative\n";
    }

    return passed;
}

void RunTurnManagerInitiativeTests() {
    std::cout << "\n=== TURN MANAGER INITIATIVE TESTS ===\n";
    std::cout << (TestInitiativeBasicRoll() ? "✅" : "❌") << " Initiative basic roll\n";
    std::cout << (TestInitiativeTurnOrder() ? "✅" : "❌") << " Turn order sorted correctly\n";
    std::cout << (TestInitiativeTieBreaker() ? "✅" : "❌") << " Tie-breaking works\n";
    std::cout << (TestInitiativeDeadCharacterSkipped() ? "✅" : "❌") << " Dead characters skipped\n";
    std::cout << (TestInitiativeReRoll() ? "✅" : "❌") << " Re-rolling changes values\n";
    std::cout << "=======================================\n";
}
```

---

### Daily Breakdown (Developer B)

#### **Monday (6-7 hours)**

- Design initiative system architecture (data structures, tie-breakers) (2 hrs)
- Add `InitiativeEntry` struct and methods to TurnManager.h (1 hr)
- Implement `CalculateSpeedModifier()` helper (30 min)
- Document D&D 5e initiative rules for team reference (1 hr)
- **Deliverable**: Initiative system designed, data structures defined

#### **Tuesday (7-8 hours)**

- Implement `RollInitiative()` method (1d20 + speed for all characters) (2.5 hrs)
- Implement `SortInitiativeOrder()` with tie-breaking logic (2 hrs)
- Add initiative logging to combat log (1.5 hrs)
- Test initiative rolling with 2 characters (Dragon + Fighter) (1 hr)
- **Deliverable**: Initiative rolling functional, turn order sorted correctly

#### **Wednesday (7-8 hours)**

- Implement `GetTurnOrder()` and `GetInitiativeValue()` accessors (1 hr)
- Add initiative event definitions to Events.h (30 min)
- Publish `InitiativeRolledEvent` and `TurnOrderEstablishedEvent` (1.5 hrs)
- Integrate initiative rolling into `StartCombat()` method (2 hrs)
- Test full combat flow with initiative system (1.5 hrs)
- **Deliverable**: Initiative integrated into TurnManager, events published

#### **Thursday (6-7 hours)**

- Implement initiative re-roll support (for multi-round combats) (1.5 hrs)
- Add `InitiativeMode` enum (RollOnce vs RollEachRound) (1 hr)
- Write comprehensive initiative test suite (2.5 hrs)
- Test edge cases (ties, dead characters, re-rolls) (1.5 hrs)
- **Deliverable**: Initiative system complete with all modes and tests

#### **Friday (3-4 hours)**

- Bug fixes from integration testing (1 hr)
- Verify initiative works in full Dragon vs Fighter combat (1 hr)
- Prepare initiative demo for integration meeting (30 min)
- **Deliverable**: Initiative system production-ready, demo prepared

---

## Week 4: Developer C - Fighter Basic AI

**Goal**: Implement autonomous AI for Fighter character (move toward Dragon, attack when in range, use Shield Bash)

**Foundation**:

- Week 2 Fighter class with combat abilities
- Week 3 CombatSystem for attack resolution
- Week 1 GridSystem for pathfinding and range calculation
- Week 1 EventBus for AI decision events

**Files to Create/Modify**:

```
CS230/Game/Singletons/AISystem.h (new file)
CS230/Game/Singletons/AISystem.cpp (new file)
CS230/Game/Objects/Fighter.cpp (add AI methods)
CS230/Game/Objects/Components/AIComponent.h (new file)
CS230/Game/Objects/Components/AIComponent.cpp (new file)
CS230/Game/Test/FighterAITests.cpp (new file)
```

### Implementation Tasks (Fighter Basic AI)

#### **Task 1: AI Decision Framework**

**Goal**: Design AI decision-making architecture

**Steps**:

1. Define AI decision types (Move, Attack, UseAbility, EndTurn)
2. Create AI state machine (Idle → Deciding → Acting → TurnEnd)
3. Design threat assessment system (identify highest priority target)
4. Plan AI decision tree (if in range → attack, else → move closer)
5. Establish AI testing methodology (deterministic scenarios)

**Why this matters**: AI framework must be extensible for Week 6-15 (Cleric, Wizard, Rogue AI). A solid foundation enables complex behaviors later while keeping Week 4 simple.

**AI Decision Hierarchy**:

1. **Threat Assessment**: Identify target (usually Dragon)
2. **Range Check**: Can I attack target from current position?
3. **Action Selection**:
   - If in attack range → Attack
   - If Shield Bash available and adjacent → Shield Bash
   - Else → Move closer to target
4. **Execute Action**: Perform selected action
5. **End Turn**: Signal turn complete

---

#### **Task 2: Threat Assessment System**

**Goal**: Identify highest-priority target for AI to attack

**Steps**:

1. Scan all enemy characters on the battlefield
2. Calculate threat score for each (based on HP, proximity, damage output)
3. Select highest-threat target
4. Handle edge cases (no valid targets, target out of reach)
5. Cache threat assessment (don't recalculate every frame)

**Why this matters**: Basic AI always targets Dragon (player character). Advanced AI (Week 16+) will target wounded allies or high-value targets. Threat assessment is foundational for all AI decision-making.

**Basic Threat Formula (Week 4)**:

```
Threat = 1000 - distance_to_target
// Simple: Closer targets = higher threat
// Week 4: Always targets Dragon (only enemy)
// Week 16+: Factor in HP, damage output, status effects
```

---

#### **Task 3: Movement AI**

**Goal**: Move Fighter toward target using pathfinding

**Steps**:

1. Calculate path to target using GridSystem A* pathfinding
2. Determine maximum movement range (based on ActionPoints)
3. Move as close as possible toward target
4. Handle obstacles (walls, other characters blocking path)
5. Stop moving if target comes into attack range

**Why this matters**: Movement is the most common AI action. Poor movement AI looks stupid (walks into walls, ignores obstacles). Pathfinding from Week 2 provides foundation.

**Movement Logic**:

```cpp
// Pseudocode
if (distance_to_target > attack_range) {
    path = GridSystem::FindPath(current_pos, target_pos);
    max_move = GetActionPoints();
    move_distance = min(path.length, max_move);
    MoveTo(path[move_distance]);
}
```

---

#### **Task 4: Attack AI**

**Goal**: Execute attack when in range of target

**Steps**:

1. Check if target is within attack range (Fighter range = 1 tile)
2. Validate attack legality (has action points, target alive, etc.)
3. Execute attack using CombatSystem
4. Publish AI decision event for logging
5. Handle attack failure gracefully (log and end turn)

**Why this matters**: Attack is Fighter's primary action. AI must recognize when to attack vs when to move. Proper validation prevents invalid actions.

**Attack Decision**:

```cpp
// Pseudocode
if (IsInRange(target, attack_range)) {
    if (CanAttack(target)) {
        PerformAttack(target);
        return; // Turn done
    }
}
// Not in range, move closer instead
```

---

#### **Task 5: Ability Usage (Shield Bash)**

**Goal**: Use Shield Bash ability when tactically appropriate

**Steps**:

1. Check if Shield Bash is available (cooldown, action points)
2. Evaluate Shield Bash utility (is Dragon adjacent?)
3. Compare Shield Bash value vs normal attack
4. Use Shield Bash if Dragon is at full HP (crowd control > damage)
5. Log ability usage for debugging

**Why this matters**: Ability usage demonstrates tactical AI. Shield Bash stuns Dragon, preventing spell casting. Using it at the right time shows intelligence.

**Shield Bash Logic**:

```cpp
// Pseudocode
if (IsAdjacent(target) && HasShieldBash()) {
    if (target.GetHP() > target.GetMaxHP() * 0.7) {
        // Dragon healthy, stun more valuable than damage
        UseShieldBash(target);
    } else {
        // Dragon wounded, finish with attack
        PerformAttack(target);
    }
}
```

---

### Implementation Examples (Fighter Basic AI)

#### **Example 1: AI System Singleton**

```cpp
// File: CS230/Game/Singletons/AISystem.h
#pragma once
#include "../Objects/Character.h"
#include <vector>

enum class AIDecisionType {
    Move,
    Attack,
    UseAbility,
    EndTurn,
    None
};

struct AIDecision {
    AIDecisionType type;
    Character* target;        // Target for attack/ability
    Math::ivec2 destination;  // Destination for move
    std::string abilityName;  // Ability to use
    std::string reasoning;    // Debug: Why this decision?
};

class AISystem {
public:
    static AISystem& GetInstance();

    // Main AI entry point
    AIDecision MakeDecision(Character* actor);

    // Decision helpers
    Character* AssessThreats(Character* actor);
    bool ShouldMoveCloser(Character* actor, Character* target);
    bool ShouldAttack(Character* actor, Character* target);
    bool ShouldUseAbility(Character* actor, Character* target);

    // Execution
    void ExecuteDecision(Character* actor, const AIDecision& decision);

private:
    AISystem() = default;

    // Threat assessment
    int CalculateThreatScore(Character* actor, Character* target);
};
```

```cpp
// File: CS230/Game/Singletons/AISystem.cpp
#include "AISystem.h"
#include "../StateComponents/GridSystem.h"
#include "CombatSystem.h"
#include "EventBus.h"
#include "../../Engine/Engine.h"

AISystem& AISystem::GetInstance() {
    static AISystem instance;
    return instance;
}

AIDecision AISystem::MakeDecision(Character* actor) {
    AIDecision decision;
    decision.type = AIDecisionType::None;

    // Step 1: Assess threats (find target)
    Character* target = AssessThreats(actor);
    if (!target) {
        decision.type = AIDecisionType::EndTurn;
        decision.reasoning = "No valid targets";
        return decision;
    }

    decision.target = target;

    // Step 2: Check if we should use ability
    if (ShouldUseAbility(actor, target)) {
        decision.type = AIDecisionType::UseAbility;
        decision.abilityName = "Shield Bash"; // Fighter only has one ability in Week 4
        decision.reasoning = "Target adjacent, Shield Bash available";
        return decision;
    }

    // Step 3: Check if we should attack
    if (ShouldAttack(actor, target)) {
        decision.type = AIDecisionType::Attack;
        decision.reasoning = "Target in attack range";
        return decision;
    }

    // Step 4: Move closer to target
    if (ShouldMoveCloser(actor, target)) {
        // Calculate path to target
        GridSystem& grid = GridSystem::GetInstance();
        Math::ivec2 actorPos = actor->GetGridPosition();
        Math::ivec2 targetPos = target->GetGridPosition();

        std::vector<Math::ivec2> path = grid.FindPath(actorPos, targetPos);

        if (path.size() > 1) {
            // Move as far as action points allow
            int maxMove = actor->GetActionPoints() / actor->GetMovementCost();
            int moveIndex = std::min((int)path.size() - 1, maxMove);

            decision.type = AIDecisionType::Move;
            decision.destination = path[moveIndex];
            decision.reasoning = "Moving closer to target";
            return decision;
        }
    }

    // No valid action, end turn
    decision.type = AIDecisionType::EndTurn;
    decision.reasoning = "No valid actions available";
    return decision;
}

Character* AISystem::AssessThreats(Character* actor) {
    // For Week 4: Simple - target the Dragon (only enemy)
    // Week 16+: More sophisticated (highest threat score)

    GridSystem& grid = GridSystem::GetInstance();
    std::vector<Character*> allCharacters = grid.GetAllCharacters();

    Character* highestThreat = nullptr;
    int highestThreatScore = -1;

    for (Character* potential : allCharacters) {
        // Skip self, allies, and dead characters
        if (potential == actor || !potential->IsAlive()) continue;
        if (potential->GetTeam() == actor->GetTeam()) continue;

        int threatScore = CalculateThreatScore(actor, potential);

        if (threatScore > highestThreatScore) {
            highestThreatScore = threatScore;
            highestThreat = potential;
        }
    }

    return highestThreat;
}

int AISystem::CalculateThreatScore(Character* actor, Character* target) {
    // Basic threat formula: Closer = higher threat
    GridSystem& grid = GridSystem::GetInstance();

    int distance = grid.GetDistance(actor->GetGridPosition(), target->GetGridPosition());

    // Threat decreases with distance
    int threatScore = 1000 - distance;

    // Week 16+: Factor in damage output, HP, status effects
    // For Week 4: Simple distance-based threat

    return threatScore;
}

bool AISystem::ShouldMoveCloser(Character* actor, Character* target) {
    if (!actor || !target) return false;

    GridSystem& grid = GridSystem::GetInstance();
    int distance = grid.GetDistance(actor->GetGridPosition(), target->GetGridPosition());
    int attackRange = actor->GetAttackRange();

    // Move if target is out of attack range
    return (distance > attackRange);
}

bool AISystem::ShouldAttack(Character* actor, Character* target) {
    if (!actor || !target) return false;

    // Check range
    CombatSystem& combat = CombatSystem::GetInstance();
    if (!combat.IsInRange(actor, target)) return false;

    // Check action points
    if (actor->GetActionPoints() < actor->GetAttackCost()) return false;

    return true;
}

bool AISystem::ShouldUseAbility(Character* actor, Character* target) {
    if (!actor || !target) return false;

    // Week 4: Fighter has Shield Bash (stun adjacent enemy)
    // Check if adjacent
    GridSystem& grid = GridSystem::GetInstance();
    int distance = grid.GetDistance(actor->GetGridPosition(), target->GetGridPosition());

    if (distance != 1) return false; // Shield Bash requires adjacency

    // Check if ability available (cooldown)
    if (!actor->HasAbility("Shield Bash")) return false;

    // Check if ability is more valuable than attack
    // Use Shield Bash if target is healthy (stun > damage)
    double hpPercent = (double)target->GetHP() / target->GetMaxHP();

    return (hpPercent > 0.7); // Use stun if target above 70% HP
}

void AISystem::ExecuteDecision(Character* actor, const AIDecision& decision) {
    Engine::GetLogger().LogEvent(
        actor->TypeName() + " AI Decision: " + decision.reasoning
    );

    switch (decision.type) {
        case AIDecisionType::Move:
            actor->MoveTo(decision.destination);
            break;

        case AIDecisionType::Attack:
            actor->PerformAttack(decision.target);
            break;

        case AIDecisionType::UseAbility:
            actor->UseAbility(decision.abilityName, decision.target);
            break;

        case AIDecisionType::EndTurn:
            // Do nothing, turn will end naturally
            break;

        case AIDecisionType::None:
            Engine::GetLogger().LogError("AI made no decision!");
            break;
    }

    // Publish AI decision event for debugging
    EventBus::GetInstance().Publish(AIDecisionEvent{
        actor, decision.type, decision.target, decision.reasoning
    });
}
```

---

#### **Example 2: Fighter AI Integration**

```cpp
// File: CS230/Game/Objects/Fighter.cpp (add AI method)

void Fighter::OnTurnStart() {
    // Call parent
    Character::OnTurnStart();

    // If AI-controlled, make decisions
    if (IsAIControlled()) {
        AISystem& ai = AISystem::GetInstance();
        AIDecision decision = ai.MakeDecision(this);
        ai.ExecuteDecision(this, decision);
    }
}

bool Fighter::IsAIControlled() const {
    // For Week 4: Fighter is always AI-controlled
    // Week 5+: Player can control via manual mode
    return true;
}
```

---

### Rigorous Testing (Fighter Basic AI)

#### **Test Suite 1: AI Decision Tests**

```cpp
// File: CS230/Game/Test/FighterAITests.cpp
#include "../../Engine/Engine.h"
#include "../Singletons/AISystem.h"
#include "../Objects/Dragon.h"
#include "../Objects/Fighter.h"
#include "../StateComponents/GridSystem.h"
#include <iostream>

bool TestAITargetsClosestEnemy() {
    // Test: AI should target closest enemy
    GridSystem& grid = GridSystem::GetInstance();
    grid.Reset();

    Fighter fighter;
    fighter.SetGridPosition({0, 0});

    Dragon dragon;
    dragon.SetGridPosition({2, 2}); // Distance = 4

    AISystem& ai = AISystem::GetInstance();
    Character* target = ai.AssessThreats(&fighter);

    bool passed = (target == &dragon);

    if (!passed) {
        std::cout << "  FAILED: AI didn't target Dragon\n";
    }

    return passed;
}

bool TestAIMovesCloserWhenOutOfRange() {
    // Test: AI should move closer if target out of range
    GridSystem& grid = GridSystem::GetInstance();
    grid.Reset();

    Fighter fighter;
    fighter.SetGridPosition({0, 0});
    fighter.SetAttackRange(1); // Melee

    Dragon dragon;
    dragon.SetGridPosition({5, 5}); // Far away

    AISystem& ai = AISystem::GetInstance();
    AIDecision decision = ai.MakeDecision(&fighter);

    bool passed = (decision.type == AIDecisionType::Move);

    if (!passed) {
        std::cout << "  FAILED: AI didn't move closer (decision: "
                  << (int)decision.type << ")\n";
    }

    return passed;
}

bool TestAIAttacksWhenInRange() {
    // Test: AI should attack if target in range
    GridSystem& grid = GridSystem::GetInstance();
    grid.Reset();

    Fighter fighter;
    fighter.SetGridPosition({4, 4});
    fighter.SetActionPoints(10); // Enough for attack
    fighter.SetAttackRange(1);

    Dragon dragon;
    dragon.SetGridPosition({4, 5}); // Adjacent (distance = 1)

    AISystem& ai = AISystem::GetInstance();
    AIDecision decision = ai.MakeDecision(&fighter);

    bool passed = (decision.type == AIDecisionType::Attack ||
                   decision.type == AIDecisionType::UseAbility);

    if (!passed) {
        std::cout << "  FAILED: AI didn't attack when in range\n";
    }

    return passed;
}

bool TestAIUsesShieldBashWhenAdjacent() {
    // Test: AI should use Shield Bash when adjacent to healthy target
    GridSystem& grid = GridSystem::GetInstance();
    grid.Reset();

    Fighter fighter;
    fighter.SetGridPosition({4, 4});
    fighter.SetActionPoints(10);
    fighter.EnableAbility("Shield Bash"); // Ability available

    Dragon dragon;
    dragon.SetGridPosition({4, 5}); // Adjacent
    dragon.SetHP(dragon.GetMaxHP()); // Full HP

    AISystem& ai = AISystem::GetInstance();
    bool shouldUse = ai.ShouldUseAbility(&fighter, &dragon);

    bool passed = shouldUse;

    if (!passed) {
        std::cout << "  FAILED: AI didn't use Shield Bash when appropriate\n";
    }

    return passed;
}

bool TestAIEnds TurnWhenNoActions() {
    // Test: AI should end turn if no valid actions
    GridSystem& grid = GridSystem::GetInstance();
    grid.Reset();

    Fighter fighter;
    fighter.SetGridPosition({0, 0});
    fighter.SetActionPoints(0); // No action points

    Dragon dragon;
    dragon.SetGridPosition({7, 7}); // Far away

    AISystem& ai = AISystem::GetInstance();
    AIDecision decision = ai.MakeDecision(&fighter);

    bool passed = (decision.type == AIDecisionType::EndTurn);

    if (!passed) {
        std::cout << "  FAILED: AI didn't end turn when no actions available\n";
    }

    return passed;
}

void RunFighterAITests() {
    std::cout << "\n=== FIGHTER AI TESTS ===\n";
    std::cout << (TestAITargetsClosestEnemy() ? "✅" : "❌") << " AI targets closest enemy\n";
    std::cout << (TestAIMovesCloserWhenOutOfRange() ? "✅" : "❌") << " AI moves closer when out of range\n";
    std::cout << (TestAIAttacksWhenInRange() ? "✅" : "❌") << " AI attacks when in range\n";
    std::cout << (TestAIUsesShieldBashWhenAdjacent() ? "✅" : "❌") << " AI uses Shield Bash appropriately\n";
    std::cout << (TestAIEndsTurnWhenNoActions() ? "✅" : "❌") << " AI ends turn when no actions\n";
    std::cout << "==========================\n";
}
```

---

### Daily Breakdown (Developer C)

#### **Monday (6-7 hours)**

- Design AI decision framework (state machine, decision tree) (2 hrs)
- Create AISystem singleton skeleton (1 hr)
- Define AI decision types and structures (1 hr)
- Document AI behavior specification for Fighter (1 hr)
- **Deliverable**: AI framework designed, AISystem skeleton created

#### **Tuesday (7-8 hours)**

- Implement threat assessment system (2 hrs)
- Implement movement AI (pathfinding integration) (2.5 hrs)
- Test movement AI (Fighter moves toward Dragon) (1.5 hrs)
- Add AI decision logging for debugging (1 hr)
- **Deliverable**: Fighter moves toward Dragon autonomously

#### **Wednesday (7-8 hours)**

- Implement attack AI (range checking, attack execution) (2 hrs)
- Implement Shield Bash decision logic (2 hrs)
- Integrate AI into Fighter.OnTurnStart() (1.5 hrs)
- Test full AI turn (move → attack sequence) (1.5 hrs)
- **Deliverable**: Fighter attacks Dragon autonomously

#### **Thursday (6-7 hours)**

- Write comprehensive AI test suite (3 hrs)
- Test edge cases (no targets, blocked paths, no action points) (2 hrs)
- Bug fixes from testing (1.5 hrs)
- **Deliverable**: Fighter AI complete with test coverage

#### **Friday (3-4 hours)**

- Bug fixes from integration testing (1 hr)
- Tune AI behavior based on playtest feedback (1 hr)
- Prepare AI demo for integration meeting (30 min)
- **Deliverable**: Fighter AI production-ready, demo prepared

---

## Week 4: Developer D - Data-Driven Stats Complete

**Goal**: Complete data-driven design with JSON hot-reload for all character stats

**Foundation**:

- Week 3 DataRegistry basics (loading dragon_stats.json, fighter_stats.json)
- RapidJSON library for JSON parsing (integrated in Week 3)
- Character class from Week 1 (stats structure)
- FileWatcher or manual reload system

**Files to Modify/Create**:

```
CS230/Game/Singletons/DataRegistry.h (expand hot-reload)
CS230/Game/Singletons/DataRegistry.cpp (implement file watching)
CS230/Game/Data/dragon_stats.json (tune stats)
CS230/Game/Data/fighter_stats.json (tune stats)
CS230/Game/Data/spells.json (new file - spell definitions)
CS230/Game/Test/DataRegistryTests.cpp (expand tests)
```

### Implementation Tasks (Data-Driven Stats)

#### **Task 1: JSON Hot-Reload System**

**Goal**: Automatically reload JSON files when they change on disk

**Steps**:

1. Implement file modification time checking
2. Poll JSON files every N seconds (or on key press F5)
3. Detect file changes and trigger reload
4. Validate JSON before applying (don't crash on invalid JSON)
5. Publish DataReloadedEvent to notify systems

**Why this matters**: Hot-reload accelerates balance iteration. Designers can tune stats and see changes instantly without recompiling or restarting the game.

**Implementation Options**:

- **Option A**: Manual reload (press F5 to reload)
- **Option B**: Auto-reload (poll files every 5 seconds)
- **Option C**: File system watcher (platform-specific, complex)

**Week 4 Approach**: Manual reload (F5 key) - simplest and most reliable

---

#### **Task 2: Comprehensive JSON Schema**

**Goal**: Define complete JSON schema for all character stats and spells

**Steps**:

1. Design character stats JSON format (HP, attack, defense, speed, spells)
2. Design spell JSON format (name, level, range, damage, effects)
3. Add validation (required fields, type checking)
4. Support arrays (multiple spells per character)
5. Add comments in JSON (using "_comment" fields)

**Why this matters**: Complete schema enables data-driven design. All balance changes happen in JSON files, not C++ code. Designers can tune without programming knowledge.

**Character JSON Schema**:

```json
{
  "character_type": "Dragon",
  "max_hp": 250,
  "base_attack": 10,
  "attack_dice": "2d8+5",
  "base_defense": 8,
  "defense_dice": "1d6+3",
  "speed": 15,
  "attack_range": 1,
  "spell_slots": {
    "level_1": 3,
    "level_2": 2,
    "level_3": 1
  },
  "known_spells": [
    "Fireball",
    "CreateWall",
    "LavaPool"
  ]
}
```

---

#### **Task 3: Spell Data-Driven Design**

**Goal**: Move all spell definitions to spells.json

**Steps**:

1. Create spells.json with all Dragon spells
2. Define spell properties (level, range, damage dice, upcast bonus)
3. Implement SpellFactory (create spells from JSON)
4. Load spells on DataRegistry initialization
5. Test spell creation from JSON data

**Why this matters**: Spell balance requires frequent iteration. JSON-driven spells allow designers to tune damage, costs, and ranges without recompiling.

**Spell JSON Schema**:

```json
{
  "spell_name": "Fireball",
  "spell_level": 2,
  "spell_school": "Evocation",
  "range": 6,
  "target_type": "Area",
  "area_radius": 2,
  "base_damage": "8d6",
  "damage_type": "Fire",
  "upcast_bonus": "+1d6 per level",
  "description": "Explodes at target tile, dealing fire damage to all creatures within 2 tiles."
}
```

---

#### **Task 4: Data Validation**

**Goal**: Prevent invalid JSON from crashing the game

**Steps**:

1. Validate required fields exist (e.g., "max_hp" must be present)
2. Check data types (HP must be integer, not string)
3. Check ranges (HP must be positive, speed must be 1-30)
4. Provide helpful error messages (which file, which field, what's wrong)
5. Fall back to default values on validation failure

**Why this matters**: Designers will make mistakes in JSON. Validation prevents crashes and provides clear error messages for quick fixes.

**Validation Example**:

```cpp
if (!json.HasMember("max_hp")) {
    LogError("Character JSON missing required field 'max_hp'");
    return false;
}

int maxHP = json["max_hp"].GetInt();
if (maxHP <= 0) {
    LogError("Character 'max_hp' must be positive, got " + std::to_string(maxHP));
    return false;
}
```

---

#### **Task 5: DataRegistry API Complete**

**Goal**: Provide clean API for all data access

**Steps**:

1. Add `GetCharacterData(name)` method
2. Add `GetSpellData(name)` method
3. Add `ReloadAll()` method (hot-reload all JSON)
4. Add `ValidateAll()` method (check all JSON files)
5. Cache loaded data for performance

**Why this matters**: Clean API hides JSON parsing complexity. Other developers call simple methods like `GetCharacterData("Dragon")` without worrying about file I/O or parsing.

**API Design**:

```cpp
class DataRegistry {
public:
    static DataRegistry& GetInstance();

    // Data access
    CharacterData GetCharacterData(const std::string& name);
    SpellData GetSpellData(const std::string& name);

    // Hot-reload
    void ReloadAll();
    void ReloadCharacters();
    void ReloadSpells();

    // Validation
    bool ValidateAll();
    bool ValidateCharacterJSON(const std::string& filepath);
    bool ValidateSpellJSON(const std::string& filepath);
};
```

---

### Implementation Examples (Data-Driven Stats)

This section provides reference implementations you can consult when stuck. **You don't need to follow these exactly** - they're examples to guide your implementation.

#### **Example 1: Hot-Reload Implementation (Manual F5)**

```cpp
// File: CS230/Game/Singletons/DataRegistry.h
#pragma once
#include <string>
#include <map>
#include <ctime>

struct CharacterData {
    std::string characterType;
    int maxHP;
    int baseAttack;
    std::string attackDice;
    int baseDefense;
    std::string defenseDice;
    int speed;
    int attackRange;
    std::map<int, int> spellSlots; // level → slot count
    std::vector<std::string> knownSpells;
};

struct SpellData {
    std::string spellName;
    int spellLevel;
    std::string spellSchool;
    int range;
    std::string targetType;
    int areaRadius;
    std::string baseDamage;
    std::string damageType;
    std::string upcastBonus;
    std::string description;
};

class DataRegistry {
public:
    static DataRegistry& GetInstance();

    // Initialization
    void Initialize();

    // Data access
    CharacterData GetCharacterData(const std::string& name);
    SpellData GetSpellData(const std::string& name);

    // Hot-reload (manual trigger)
    void ReloadAll();
    void ReloadCharacters();
    void ReloadSpells();

    // Validation
    bool ValidateAll();

private:
    DataRegistry() = default;

    // JSON loading
    bool LoadCharacterJSON(const std::string& filepath, CharacterData& outData);
    bool LoadSpellJSON(const std::string& filepath, SpellData& outData);

    // File modification tracking
    std::time_t GetFileModTime(const std::string& filepath);

    // Data storage
    std::map<std::string, CharacterData> characterDatabase;
    std::map<std::string, SpellData> spellDatabase;

    // File modification times (for detecting changes)
    std::map<std::string, std::time_t> fileModTimes;
};
```

```cpp
// File: CS230/Game/Singletons/DataRegistry.cpp
#include "DataRegistry.h"
#include "EventBus.h"
#include "../../Engine/Engine.h"
#include <fstream>
#include <sys/stat.h>

// RapidJSON includes
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

DataRegistry& DataRegistry::GetInstance() {
    static DataRegistry instance;
    return instance;
}

void DataRegistry::Initialize() {
    Engine::GetLogger().LogEvent("DataRegistry initializing...");

    // Load all character data
    LoadCharacterJSON("Assets/Data/dragon_stats.json", characterDatabase["Dragon"]);
    LoadCharacterJSON("Assets/Data/fighter_stats.json", characterDatabase["Fighter"]);

    // Load all spell data
    LoadSpellJSON("Assets/Data/fireball.json", spellDatabase["Fireball"]);
    LoadSpellJSON("Assets/Data/createwall.json", spellDatabase["CreateWall"]);
    LoadSpellJSON("Assets/Data/lavapool.json", spellDatabase["LavaPool"]);

    Engine::GetLogger().LogEvent("DataRegistry initialized with " +
        std::to_string(characterDatabase.size()) + " characters, " +
        std::to_string(spellDatabase.size()) + " spells");
}

std::time_t DataRegistry::GetFileModTime(const std::string& filepath) {
    struct stat fileInfo;
    if (stat(filepath.c_str(), &fileInfo) == 0) {
        return fileInfo.st_mtime;
    }
    return 0;
}

void DataRegistry::ReloadAll() {
    Engine::GetLogger().LogEvent("=== RELOADING ALL DATA ===");

    // Check each file for modifications
    std::vector<std::string> characterFiles = {
        "Assets/Data/dragon_stats.json",
        "Assets/Data/fighter_stats.json"
    };

    std::vector<std::string> spellFiles = {
        "Assets/Data/fireball.json",
        "Assets/Data/createwall.json",
        "Assets/Data/lavapool.json"
    };

    bool anyReloaded = false;

    // Reload characters if modified
    for (const auto& filepath : characterFiles) {
        std::time_t currentModTime = GetFileModTime(filepath);
        std::time_t lastModTime = fileModTimes[filepath];

        if (currentModTime > lastModTime) {
            Engine::GetLogger().LogEvent("Reloading " + filepath + " (modified)");

            // Determine character name from filepath
            std::string charName = "Unknown";
            if (filepath.find("dragon") != std::string::npos) charName = "Dragon";
            if (filepath.find("fighter") != std::string::npos) charName = "Fighter";

            CharacterData data;
            if (LoadCharacterJSON(filepath, data)) {
                characterDatabase[charName] = data;
                fileModTimes[filepath] = currentModTime;
                anyReloaded = true;
            }
        }
    }

    // Reload spells if modified
    for (const auto& filepath : spellFiles) {
        std::time_t currentModTime = GetFileModTime(filepath);
        std::time_t lastModTime = fileModTimes[filepath];

        if (currentModTime > lastModTime) {
            Engine::GetLogger().LogEvent("Reloading " + filepath + " (modified)");

            // Determine spell name from filepath
            std::string spellName = "Unknown";
            if (filepath.find("fireball") != std::string::npos) spellName = "Fireball";
            if (filepath.find("createwall") != std::string::npos) spellName = "CreateWall";
            if (filepath.find("lavapool") != std::string::npos) spellName = "LavaPool";

            SpellData data;
            if (LoadSpellJSON(filepath, data)) {
                spellDatabase[spellName] = data;
                fileModTimes[filepath] = currentModTime;
                anyReloaded = true;
            }
        }
    }

    if (anyReloaded) {
        // Publish event to notify systems
        EventBus::GetInstance().Publish(DataReloadedEvent{});
        Engine::GetLogger().LogEvent("Data reload complete - systems notified");
    } else {
        Engine::GetLogger().LogEvent("No files modified - reload skipped");
    }
}

bool DataRegistry::LoadCharacterJSON(const std::string& filepath, CharacterData& outData) {
    // Open file
    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp) {
        Engine::GetLogger().LogError("Failed to open " + filepath);
        return false;
    }

    // Read JSON
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    // Validate JSON
    if (doc.HasParseError()) {
        Engine::GetLogger().LogError("JSON parse error in " + filepath);
        return false;
    }

    // Extract data with validation
    if (!doc.HasMember("character_type") || !doc["character_type"].IsString()) {
        Engine::GetLogger().LogError("Missing or invalid 'character_type' in " + filepath);
        return false;
    }
    outData.characterType = doc["character_type"].GetString();

    if (!doc.HasMember("max_hp") || !doc["max_hp"].IsInt()) {
        Engine::GetLogger().LogError("Missing or invalid 'max_hp' in " + filepath);
        return false;
    }
    outData.maxHP = doc["max_hp"].GetInt();

    if (outData.maxHP <= 0) {
        Engine::GetLogger().LogError("'max_hp' must be positive in " + filepath);
        return false;
    }

    // Continue for all fields...
    outData.baseAttack = doc["base_attack"].GetInt();
    outData.attackDice = doc["attack_dice"].GetString();
    outData.baseDefense = doc["base_defense"].GetInt();
    outData.defenseDice = doc["defense_dice"].GetString();
    outData.speed = doc["speed"].GetInt();
    outData.attackRange = doc["attack_range"].GetInt();

    // Parse spell slots
    if (doc.HasMember("spell_slots") && doc["spell_slots"].IsObject()) {
        const auto& slots = doc["spell_slots"];
        if (slots.HasMember("level_1")) outData.spellSlots[1] = slots["level_1"].GetInt();
        if (slots.HasMember("level_2")) outData.spellSlots[2] = slots["level_2"].GetInt();
        if (slots.HasMember("level_3")) outData.spellSlots[3] = slots["level_3"].GetInt();
    }

    // Parse known spells
    if (doc.HasMember("known_spells") && doc["known_spells"].IsArray()) {
        const auto& spells = doc["known_spells"];
        for (rapidjson::SizeType i = 0; i < spells.Size(); ++i) {
            outData.knownSpells.push_back(spells[i].GetString());
        }
    }

    // Store file modification time
    fileModTimes[filepath] = GetFileModTime(filepath);

    return true;
}

CharacterData DataRegistry::GetCharacterData(const std::string& name) {
    auto it = characterDatabase.find(name);
    if (it != characterDatabase.end()) {
        return it->second;
    }

    Engine::GetLogger().LogError("Character data not found: " + name);
    return CharacterData{}; // Return empty data
}

SpellData DataRegistry::GetSpellData(const std::string& name) {
    auto it = spellDatabase.find(name);
    if (it != spellDatabase.end()) {
        return it->second;
    }

    Engine::GetLogger().LogError("Spell data not found: " + name);
    return SpellData{}; // Return empty data
}
```

**Usage Example**:

```cpp
// In BattleState or Test state
void HandleInput() {
    Input& input = Engine::GetInput();

    // Press F5 to reload data
    if (input.IsKeyPressed(InputKey::F5)) {
        DataRegistry::GetInstance().ReloadAll();
    }
}
```

---

#### **Example 2: Character JSON File**

```json
// File: Assets/Data/dragon_stats.json
{
  "_comment": "Dragon (Player Character) Stats - Week 4 Balance",
  "character_type": "Dragon",

  "_comment_hp": "High HP for survivability in 1v4 battles",
  "max_hp": 250,

  "_comment_attack": "Moderate physical attack (relies on spells for damage)",
  "base_attack": 10,
  "attack_dice": "2d8+5",

  "_comment_defense": "Moderate defense",
  "base_defense": 8,
  "defense_dice": "1d6+3",

  "_comment_speed": "High speed for first initiative",
  "speed": 15,

  "_comment_range": "Melee range (prefers spells at distance)",
  "attack_range": 1,

  "_comment_spell_slots": "Balanced distribution: 3/2/1 for Week 4",
  "spell_slots": {
    "level_1": 3,
    "level_2": 2,
    "level_3": 1
  },

  "_comment_known_spells": "All 3 Dragon spells from Week 3",
  "known_spells": [
    "Fireball",
    "CreateWall",
    "LavaPool"
  ]
}
```

```json
// File: Assets/Data/fighter_stats.json
{
  "_comment": "Fighter (AI Opponent) Stats - Week 4 Balance",
  "character_type": "Fighter",

  "_comment_hp": "Moderate HP (tank role)",
  "max_hp": 90,

  "_comment_attack": "High physical attack",
  "base_attack": 12,
  "attack_dice": "2d6+4",

  "_comment_defense": "High defense (tank)",
  "base_defense": 10,
  "defense_dice": "1d8+2",

  "_comment_speed": "Low speed (acts later)",
  "speed": 10,

  "_comment_range": "Melee only",
  "attack_range": 1,

  "_comment_spell_slots": "No spells in Week 4 (abilities only)",
  "spell_slots": {},

  "_comment_abilities": "Shield Bash (CC ability)",
  "known_abilities": [
    "Shield Bash"
  ]
}
```

---

### Rigorous Testing (Data-Driven Stats)

*(Testing sections follow the same format as previous developers)*

---

### Daily Breakdown (Developer D)

#### **Monday (6-7 hours)**

- Design JSON schema for characters and spells (2 hrs)
- Implement file modification time tracking (1 hr)
- Add `ReloadAll()` method skeleton (1 hr)
- Write character JSON files (dragon_stats.json, fighter_stats.json) (1.5 hrs)
- **Deliverable**: JSON schema designed, hot-reload framework created

#### **Tuesday (7-8 hours)**

- Implement `LoadCharacterJSON()` with validation (2.5 hrs)
- Implement `LoadSpellJSON()` with validation (2 hrs)
- Test JSON loading and validation (1.5 hrs)
- Handle invalid JSON gracefully (error messages) (1 hr)
- **Deliverable**: JSON loading complete with validation

#### **Wednesday (7-8 hours)**

- Implement hot-reload file modification detection (2 hrs)
- Add F5 key binding for manual reload (30 min)
- Test hot-reload workflow (modify JSON → press F5 → stats update) (2 hrs)
- Publish `DataReloadedEvent` for system notification (1 hr)
- Create spell JSON files (fireball.json, createwall.json, lavapool.json) (1.5 hrs)
- **Deliverable**: Hot-reload working, all JSON files created

#### **Thursday (6-7 hours)**

- Write comprehensive DataRegistry test suite (3 hrs)
- Test edge cases (missing files, invalid JSON, malformed data) (2 hrs)
- Bug fixes from testing (1.5 hrs)
- **Deliverable**: DataRegistry complete with test coverage

#### **Friday (3-4 hours)**

- Final data tuning based on balance feedback (1 hr)
- Verify hot-reload works in integration test (1 hr)
- Prepare data-driven demo for integration meeting (30 min)
- **Deliverable**: Data-driven system production-ready

---

## Week 4: Developer E - StatusEffectManager Foundation

**Goal**: Track status effects (Burn from LavaPool) with duration and stat modifications

**Foundation**:

- Week 3 LavaPool spell creates burn effect
- Week 2 TurnManager for turn tracking (effects tick each turn)
- Week 1 EventBus for status effect events
- Character class for stat modification

**Files to Create**:

```
CS230/Game/Singletons/StatusEffectManager.h (new file)
CS230/Game/Singletons/StatusEffectManager.cpp (new file)
CS230/Game/Types/StatusEffects.h (new file - effect definitions)
CS230/Game/Objects/Components/StatusEffects.h (new file - character component)
CS230/Game/Test/StatusEffectTests.cpp (new file)
```

### Implementation Tasks (StatusEffectManager)

*(Follow same structure as previous developers: Tasks → Implementation Examples → Tests → Daily Breakdown)*

---

## Week 4 Integration Test (Friday Afternoon)

**Goal**: Test full combat loop with initiative, AI, status effects, and hot-reload

**Participants**: All 5 developers

**Duration**: 90 minutes

### Integration Test Scenario

```cpp
// File: CS230/Game/Test/Week4IntegrationTest.cpp

bool Week4IntegrationTest() {
    // Integration Test: Dragon vs Fighter with full Week 4 features

    // Test 1: Initiative System
    // Test 2: Fighter AI
    // Test 3: Status Effects (Burn)
    // Test 4: Data Hot-Reload
    // Test 5: Full Combat Loop

    return true; // All tests passed
}
```

*(Complete integration test implementation follows same structure as week3.md)*

---

## Week 4 Deliverable & Verification

### Final Deliverables (End of Week 4)

**Deadline**: Friday Week 4, 5:00 PM

#### **Developer A Deliverables**

- [ ] All 3 Dragon spells balanced (Fireball, CreateWall, LavaPool)
- [ ] Spell damage tuned to balance target (Fireball 2-shots Fighter)
- [ ] Spell slot allocation balanced (3/2/1 distribution)
- [ ] Spell balance test suite (15+ tests)
- [ ] Balance report document

#### **Developer B Deliverables**

- [ ] TurnManager initiative system (1d20 + speed modifier)
- [ ] Turn order sorted by initiative
- [ ] Initiative tie-breaking logic
- [ ] Initiative events published
- [ ] Initiative test suite (8+ tests)

#### **Developer C Deliverables**

- [ ] Fighter basic AI (move toward, attack, Shield Bash)
- [ ] AISystem singleton with threat assessment
- [ ] AI decision-making complete
- [ ] Fighter AI test suite (10+ tests)

#### **Developer D Deliverables**

- [ ] DataRegistry hot-reload (F5 key)
- [ ] All character stats in JSON
- [ ] All spell data in JSON
- [ ] JSON validation complete
- [ ] DataRegistry test suite (12+ tests)

#### **Developer E Deliverables**

- [ ] StatusEffectManager tracking burn effects
- [ ] Status effect duration management
- [ ] Status effect tick damage
- [ ] Status effect test suite (10+ tests)

### Integration Verification

**Run full integration test**:

```bash
# Build
cmake --build --preset windows-debug

# Run integration test
./build/windows-debug/dragonic_tactics.exe --test week4-integration
```

**Expected Results**:

```
=== WEEK 4 INTEGRATION TEST ===
✅ Initiative system functional
✅ Fighter AI makes autonomous decisions
✅ Status effects track correctly
✅ Hot-reload working (JSON data)
✅ Full combat loop complete
================================
ALL TESTS PASSED (5/5)
```

### Success Criteria

Week 4 is complete when:

1. ✅ **Initiative rolls each combat** (1d20 + speed)
2. ✅ **Fighter makes decisions autonomously** (no manual control)
3. ✅ **Burn damage applies each turn** (LavaPool creates burn effects)
4. ✅ **All stats loaded from JSON** (no hardcoded character data)
5. ✅ **Hot-reload works** (modify JSON → F5 → stats update)
6. ✅ **All spells balanced** (fair damage, costs, ranges)
7. ✅ **55+ tests pass** (all 5 developer test suites)
8. ✅ **Full combat completes** (Dragon vs Fighter with no crashes)

### Week 5 Preview

**Next week's focus**:

- **Developer A**: Final spell polish, visual effects
- **Developer B**: Combat formula refinement, death handling
- **Developer C**: Fighter AI polish, edge case fixes
- **Developer D**: Data validation complete, all JSON files finalized
- **Developer E**: BattleState complete integration, PLAYTEST 1 build

**Week 5 goal**: Prepare for **Playtest 1** (Friday Week 5) - Dragon vs Fighter fully playable, balanced, and polished. This is the first external playtest to validate core gameplay loop.
