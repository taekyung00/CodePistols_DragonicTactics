# Week 3 Implementation Guide

**Date**: 2025-10-28
**Project**: Dragonic Tactics - Turn-based Tactical RPG
**Phase**: Week 3 - Combat + Spells
**Status**: Ready for independent development

---

## Executive Summary

This document provides a comprehensive guide for Week 3 independent development. All developers can now work in parallel using the provided mockups.

### Week 2 Implementation Status ✅

**VERIFIED IMPLEMENTATIONS**:

1. **Character System** ✅ COMPLETE
   - `Character.h/cpp` - Base character class with components
   - `Dragon.h/cpp` - Player character (HP: 140, 3d6 attack, spell slots)
   - `Fighter.h/cpp` - AI opponent (HP: 90, 2d6 attack)
   - Location: `DragonicTactics/source/Game/DragonicTactics/Objects/`

2. **Character Components** ✅ COMPLETE
   - `GridPosition.h/cpp` - Grid tile coordinates (Math::ivec2)
   - `StatsComponent.h/cpp` - HP, attack, defense, speed
   - `ActionPoints.h/cpp` - Movement/action budget per turn
   - `SpellSlots.h/cpp` - Magic system with spell slot levels
   - Location: `DragonicTactics/source/Game/DragonicTactics/Objects/Components/`

3. **Core Singletons** ✅ COMPLETE
   - `EventBus.h/cpp` - Event pub/sub system
   - `DiceManager.h/cpp` - D&D dice notation (e.g., "3d6+2")
   - `CombatSystem.h/cpp` - Combat damage calculation
   - `DataRegistry.h/cpp` - JSON data loading
   - Location: `DragonicTactics/source/Game/DragonicTactics/Singletons/`

4. **Grid & Turn Systems** ✅ COMPLETE
   - `GridSystem.h/cpp` - 8x8 tactical grid (in StateComponents)
   - `TurnManager.h/cpp` - Turn order management (in StateComponents)
   - Location: `DragonicTactics/source/Game/DragonicTactics/StateComponents/`

5. **Data Files** ✅ COMPLETE
   - `characters.json` - Dragon and Fighter stats
   - Location: `DragonicTactics/Assets/Data/`

### Week 2 Missing Features (To Address in Week 3)

**NOT YET IMPLEMENTED**:

1. **Spell System** ❌ NOT IMPLEMENTED
   - No `Spell` base class
   - No concrete spell implementations (Fireball, CreateWall, LavaPool)
   - No `SpellSystem` singleton to manage spell casting
   - **Dev E responsibility** - Use mockups provided below

2. **TreasureBox GameObject** ❌ NOT IMPLEMENTED
   - No `TreasureBox` class
   - No treasure loot system
   - No interaction with grid objects
   - **Dev A responsibility** - Use mockups provided below

3. **GameObjectTypes** ⚠️ NEEDS UPDATE
   - Current: Only `Character` type defined for Dragonic Tactics
   - Needs: `TreasureBox`, `Spell`, `WallTile`, `LavaTile` types
   - Location: `DragonicTactics/source/Game/DragonicTactics/Types/GameObjectTypes.h`

---

## Week 3 Mockups - READY FOR USE

### Location

All mockups are in: `DragonicTactics/source/Game/DragonicTactics/Test/`

**Files Created**:
- `Week3TestMocks.h` - Header with class declarations, interfaces, and inline implementations
- `Week3TestMocks.cpp` - Full implementations and test suite

### How to Use the Mockups

#### For Developer A (TreasureBox)

**Mockup Classes Provided**:
```cpp
// Main class
class MockTreasureBox : public CS230::GameObject

// Supporting enums
enum class TreasureBoxState { Closed, Opening, Opened, Empty }
enum class TreasureType { Gold, HealthPotion, SpellScroll, MagicItem, Nothing }
```

**How to Use**:

1. **Include the mockup**:
   ```cpp
   #include "Test/Week3TestMocks.h"
   ```

2. **Create TreasureBox instances**:
   ```cpp
   // Create a treasure box at grid position (3, 4) with Gold
   MockTreasureBox* box = new MockTreasureBox(Math::ivec2{3, 4}, TreasureType::Gold);
   ```

3. **Test opening mechanic**:
   ```cpp
   if (box->CanBeOpened()) {
       TreasureType reward = box->Open();  // Returns treasure type
       // Box enters "Opening" state (animation)
       box->Update(1.0);  // Simulate 1 second animation
       // Now box is in "Opened" state
   }
   ```

4. **Visual state debugging**:
   ```cpp
   std::cout << "Box state: " << box->GetStateString() << std::endl;
   std::cout << "Contains: " << box->GetTreasureString() << std::endl;
   ```

5. **Run Dev A tests**:
   ```cpp
   Week3Tests::TestTreasureBoxCreation();
   Week3Tests::TestTreasureBoxOpening();
   Week3Tests::TestTreasureBoxVisualization();
   Week3Tests::TestTreasureBoxGridInteraction();
   ```

**Dev A Tasks**:
1. ✅ Use `MockTreasureBox` to prototype treasure mechanics
2. ✅ Add sprite rendering in `Draw()` method
3. ✅ Implement opening animation (replace mock timer with real animation system)
4. ✅ Add to `GameObjectTypes` enum (suggest: `GameObjectTypes::TreasureBox`)
5. ✅ Integrate with GridSystem for placement validation
6. ✅ Create treasure sprites (Assets/images/)

**Dev A Visualization Requirements**:
- Closed state: Locked chest sprite
- Opening state: Animated opening sequence
- Opened state: Open chest sprite with glow effect
- Empty state: Open chest sprite (no glow)

---

#### For Developer E (Spell System)

**Mockup Classes Provided**:
```cpp
// Base spell interface
class MockSpellBase

// Concrete spells (all 3 Dragon spells)
class MockFireball : public MockSpellBase      // Level 3, AOE damage
class MockCreateWall : public MockSpellBase    // Level 2, terrain modification
class MockLavaPool : public MockSpellBase      // Level 4, AOE + duration

// Spell manager
class MockSpellSystem  // Singleton

// Supporting types
struct MockSpellResult
enum class SpellTargetType
```

**How to Use**:

1. **Include the mockup**:
   ```cpp
   #include "Test/Week3TestMocks.h"
   ```

2. **Register spells** (in BattleState::Load() or similar):
   ```cpp
   MockSpellSystem::Instance().RegisterSpell("Fireball", new MockFireball());
   MockSpellSystem::Instance().RegisterSpell("CreateWall", new MockCreateWall());
   MockSpellSystem::Instance().RegisterSpell("LavaPool", new MockLavaPool());
   ```

3. **Cast a spell**:
   ```cpp
   // Cast Fireball at grid tile (4, 4)
   MockSpellResult result = MockSpellSystem::Instance().CastSpell(
       dragon_character,     // Caster
       "Fireball",          // Spell name
       Math::ivec2{4, 4},   // Target tile
       3                    // Cast at level 3 (or -1 for base level)
   );

   if (result.success) {
       std::cout << "Damage: " << result.total_damage << std::endl;
       std::cout << "Affected " << result.affected_tiles.size() << " tiles" << std::endl;

       // Apply damage to characters on affected tiles
       for (Character* target : result.affected_targets) {
           target->TakeDamage(result.total_damage, nullptr);
       }
   } else {
       std::cout << "Failed: " << result.failure_reason << std::endl;
   }
   ```

4. **Direct spell usage** (without SpellSystem):
   ```cpp
   MockFireball fireball;

   // Check if can cast
   if (fireball.CanCast(caster, target_tile)) {
       // Cast at higher level (upcasting)
       MockSpellResult result = fireball.CastAtLevel(caster, target_tile, 5);

       std::cout << "Fireball Level 5: " << result.total_damage << " damage" << std::endl;
   }
   ```

5. **Run Dev E tests**:
   ```cpp
   Week3Tests::TestFireballDamage();
   Week3Tests::TestFireballAreaOfEffect();
   Week3Tests::TestFireballUpcasting();

   Week3Tests::TestCreateWallPlacement();
   Week3Tests::TestCreateWallBlocking();
   Week3Tests::TestCreateWallUpcasting();

   Week3Tests::TestLavaPoolDamage();
   Week3Tests::TestLavaPoolDuration();
   Week3Tests::TestLavaPoolUpcasting();

   Week3Tests::TestSpellSystemRegistration();
   Week3Tests::TestMultipleSpellsCombat();

   // Or run all at once:
   Week3Tests::RunAllWeek3Tests();
   ```

**Dev E Tasks**:
1. ✅ Use spell mockups to implement full spell system
2. ✅ Integrate with `Character::GetSpellSlots()` for spell slot consumption
3. ✅ Integrate with `EventBus` to publish spell events (SpellCastEvent, SpellEffectAppliedEvent)
4. ✅ Integrate with `DiceManager` for real damage rolls (replace mock damage values)
5. ✅ Integrate with `GridSystem` to modify terrain (CreateWall, LavaPool)
6. ✅ Add spell visual effects (particle systems, animations)
7. ✅ Implement spell data loading from JSON (spell_data.json)

**Spell Specifications**:

**Fireball** (Level 3 Evocation):
- Range: 6 tiles
- Area: 2-tile radius (5x5 grid)
- Damage: 8d6 fire damage (base level 3)
- Upcasting: +1d6 per level above 3
- Effect: Instant damage to all characters in area
- Visual: Fire explosion animation

**CreateWall** (Level 2 Conjuration):
- Range: 4 tiles
- Area: Line of tiles
- Length: 3 tiles (base level 2)
- Upcasting: +1 tile per level above 2
- Effect: Creates impassable wall tiles
- Duration: Permanent (until destroyed)
- Visual: Stone wall sprites

**LavaPool** (Level 4 Conjuration):
- Range: 5 tiles
- Area: Radius-based (3x3 at level 4, 5x5 at level 6, etc.)
- Damage: 2d6 fire damage per turn to anyone standing on lava
- Duration: 3 rounds (base level 4), +1 round per level
- Upcasting: Larger radius and longer duration
- Effect: Changes tile type to "Lava" (damaging terrain)
- Visual: Lava tile animation, bubbling effect

---

## Integration with Existing Systems

### How Spells Use Existing Systems

**EventBus Integration**:
```cpp
// After casting a spell
SpellCastEvent event{
    .caster = caster_character,
    .spell_name = "Fireball",
    .target_tile = target_tile,
    .spell_level = 3
};
EventBus::Instance().Publish(event);

// After applying damage
for (Character* target : affected_targets) {
    SpellEffectAppliedEvent effect_event{
        .target = target,
        .effect_type = "Damage",
        .magnitude = damage_amount,
        .spell_name = "Fireball"
    };
    EventBus::Instance().Publish(effect_event);
}
```

**DiceManager Integration**:
```cpp
// Roll spell damage
std::string damage_dice = "8d6";  // Fireball damage
int damage = DiceManager::Instance().RollDiceFromString(damage_dice);

// Get individual dice results for UI display
std::vector<int> rolls = DiceManager::Instance().GetLastRolls();
std::cout << "Rolled: ";
for (int roll : rolls) {
    std::cout << roll << " ";
}
std::cout << " = " << damage << " total" << std::endl;
```

**SpellSlots Integration**:
```cpp
// Check if character has spell slot
if (caster->HasSpellSlot(spell_level)) {
    // Cast spell
    MockSpellResult result = spell->Cast(caster, target_tile);

    if (result.success) {
        // Consume spell slot
        caster->GetSpellSlots()->Consume(spell_level);

        // Publish spell slot consumed event
        SpellSlotConsumedEvent event{
            .character = caster,
            .spell_level = spell_level,
            .remaining_slots = caster->GetSpellSlots()->GetRemainingSlots(spell_level)
        };
        EventBus::Instance().Publish(event);
    }
}
```

**GridSystem Integration**:
```cpp
// CreateWall spell - modify grid tiles
MockSpellResult result = create_wall_spell.Cast(caster, target_tile);

for (Math::ivec2 tile : result.affected_tiles) {
    // Change tile type to Wall
    grid_system->SetTileType(tile, TileType::Wall);

    // Publish terrain changed event
    TerrainChangedEvent event{
        .tile_position = tile,
        .old_type = TileType::Empty,
        .new_type = TileType::Wall,
        .source_spell = "Create Wall"
    };
    EventBus::Instance().Publish(event);
}

// LavaPool spell - create damaging terrain
for (Math::ivec2 tile : result.affected_tiles) {
    grid_system->SetTileType(tile, TileType::Lava);

    // Register hazard for duration tracking
    grid_system->RegisterHazard(tile, TileType::Lava, duration_rounds, "2d6");
}
```

### How TreasureBox Uses Existing Systems

**GridSystem Integration**:
```cpp
// Place treasure box on grid
MockTreasureBox* box = new MockTreasureBox(Math::ivec2{3, 4}, TreasureType::Gold);
grid_system->PlaceObject(box);

// Check if tile is occupied before movement
if (grid_system->GetObjectAt(target_tile) != nullptr) {
    // Check if it's a treasure box
    if (auto* box = dynamic_cast<MockTreasureBox*>(grid_system->GetObjectAt(target_tile))) {
        if (box->CanBeOpened()) {
            TreasureType reward = box->Open();
            // Handle reward...
        }
    }
}
```

**EventBus Integration**:
```cpp
// When treasure box is opened
TreasureType reward = box->Open();

TreasureOpenedEvent event{
    .treasure_box = box,
    .opener = character,
    .reward_type = reward,
    .grid_position = box->GetGridPosition()
};
EventBus::Instance().Publish(event);

// When reward is collected
switch (reward) {
    case TreasureType::HealthPotion:
        character->ReceiveHeal(30);
        ItemCollectedEvent heal_event{.character = character, .item_type = "Health Potion"};
        EventBus::Instance().Publish(heal_event);
        break;

    case TreasureType::SpellScroll:
        character->GetSpellSlots()->Recover(3);  // Restore level 3 spell slot
        ItemCollectedEvent scroll_event{.character = character, .item_type = "Spell Scroll"};
        EventBus::Instance().Publish(scroll_event);
        break;

    // ... other treasure types
}
```

---

## Running the Tests

### Week 3 Test Suite

**Location**: `Week3TestMocks.cpp` - `Week3Tests` namespace

**Individual Tests**:

```cpp
#include "Test/Week3TestMocks.h"

// TreasureBox tests (Dev A)
Week3Tests::TestTreasureBoxCreation();
Week3Tests::TestTreasureBoxOpening();
Week3Tests::TestTreasureBoxVisualization();
Week3Tests::TestTreasureBoxGridInteraction();

// Fireball tests (Dev E)
Week3Tests::TestFireballDamage();
Week3Tests::TestFireballAreaOfEffect();
Week3Tests::TestFireballUpcasting();

// Create Wall tests (Dev E)
Week3Tests::TestCreateWallPlacement();
Week3Tests::TestCreateWallBlocking();
Week3Tests::TestCreateWallUpcasting();

// Lava Pool tests (Dev E)
Week3Tests::TestLavaPoolDamage();
Week3Tests::TestLavaPoolDuration();
Week3Tests::TestLavaPoolUpcasting();

// Integration tests (Dev E)
Week3Tests::TestSpellSystemRegistration();
Week3Tests::TestSpellSlotConsumption();
Week3Tests::TestMultipleSpellsCombat();
```

**Run All Tests**:
```cpp
Week3Tests::RunAllWeek3Tests();
```

**Expected Output**:
```
==========================================
      WEEK 3 MOCKUP TESTS - STARTING
==========================================

--- TREASURE BOX TESTS ---
=== Testing TreasureBox Creation ===
PASSED: TreasureBox creation successful
... (more tests)

--- FIREBALL SPELL TESTS ---
=== Testing Fireball Damage ===
Fireball cast at level 3!
Damage: 8d6 = 28
Affected 25 tiles.
PASSED: Fireball damage calculated
... (more tests)

==========================================
      WEEK 3 TESTS COMPLETE
      Passed: 16/16 tests
==========================================
```

---

## Data Files for Week 3

### Suggested JSON Files to Create

**1. spell_data.json** (Dev E)

```json
{
  "Fireball": {
    "name": "Fireball",
    "level": 3,
    "school": "Evocation",
    "range": 6,
    "area_radius": 2,
    "damage_dice": "8d6",
    "damage_type": "Fire",
    "upcasting": {
      "damage_per_level": "1d6"
    },
    "description": "A bright streak flashes to a point within range and blossoms into an explosion of flame.",
    "visual_effect": "FireExplosion"
  },
  "CreateWall": {
    "name": "Create Wall",
    "level": 2,
    "school": "Conjuration",
    "range": 4,
    "wall_length": 3,
    "upcasting": {
      "length_per_level": 1
    },
    "duration": "Permanent",
    "description": "Creates a line of solid stone walls blocking movement.",
    "visual_effect": "StoneWall"
  },
  "LavaPool": {
    "name": "Lava Pool",
    "level": 4,
    "school": "Conjuration",
    "range": 5,
    "area_radius": 1,
    "damage_per_turn": "2d6",
    "damage_type": "Fire",
    "duration_rounds": 3,
    "upcasting": {
      "radius_per_2_levels": 1,
      "duration_per_level": 1
    },
    "description": "Summons a pool of molten lava that damages creatures standing on it.",
    "visual_effect": "LavaBubbling"
  }
}
```

**2. treasure_data.json** (Dev A)

```json
{
  "treasures": [
    {
      "type": "Gold",
      "value": 100,
      "sprite": "GoldPile",
      "rarity": "Common"
    },
    {
      "type": "HealthPotion",
      "healing_amount": 30,
      "sprite": "RedPotion",
      "rarity": "Common"
    },
    {
      "type": "SpellScroll",
      "spell_level_restored": 3,
      "sprite": "ScrollGlowing",
      "rarity": "Uncommon"
    },
    {
      "type": "MagicItem",
      "stat_bonus": "attack+2",
      "sprite": "MagicSword",
      "rarity": "Rare"
    }
  ],
  "spawn_chances": {
    "Gold": 0.4,
    "HealthPotion": 0.3,
    "SpellScroll": 0.2,
    "MagicItem": 0.1
  }
}
```

### Loading Data Example

**Using DataRegistry** (already implemented):

```cpp
// Load spell data
DataRegistry::Instance().LoadFromFile("Assets/Data/spell_data.json");

// Access spell info
int fireball_level = DataRegistry::Instance().GetValue<int>("Fireball.level", 3);
std::string fireball_damage = DataRegistry::Instance().GetValue<std::string>("Fireball.damage_dice", "8d6");
int fireball_range = DataRegistry::Instance().GetValue<int>("Fireball.range", 6);

// Load treasure data
DataRegistry::Instance().LoadFromFile("Assets/Data/treasure_data.json");

// Access treasure info
int gold_value = DataRegistry::Instance().GetValue<int>("treasures.0.value", 100);
```

---

## Next Steps for Each Developer

### Developer A (TreasureBox + Visualization)

**This Week** (Week 3):
1. ✅ Use `MockTreasureBox` from `Week3TestMocks.h`
2. ✅ Create treasure box sprites (closed, opening animation, opened, empty)
3. ✅ Implement `Draw()` method with sprite rendering
4. ✅ Add opening animation using CS230 Animation system
5. ✅ Test with `Week3Tests::TestTreasureBox*()` functions
6. ✅ Integrate with GridSystem for placement
7. ✅ Add `TreasureBox` to `GameObjectTypes` enum
8. ✅ Create `treasure_data.json` for loot configuration

**Testing**:
- Run treasure box tests individually
- Test in BattleState with real Dragon character
- Verify sprite animations play correctly
- Verify grid placement works

**Deliverable**: Fully functional TreasureBox GameObject with visual feedback

---

### Developer E (Spell System)

**This Week** (Week 3):
1. ✅ Use spell mockups from `Week3TestMocks.h` (Fireball, CreateWall, LavaPool)
2. ✅ Integrate with `SpellSlots` component (consume slots on cast)
3. ✅ Integrate with `DiceManager` for damage rolls
4. ✅ Integrate with `EventBus` for spell events
5. ✅ Integrate with `GridSystem` for terrain modification
6. ✅ Test with `Week3Tests::TestFireball*()`, `TestCreateWall*()`, `TestLavaPool*()` functions
7. ✅ Add spell visual effects (particle systems)
8. ✅ Create `spell_data.json` for spell configuration
9. ✅ Implement upcasting logic

**Testing**:
- Run spell tests individually
- Test spell casting with real Dragon character
- Verify spell slots decrease after casting
- Verify damage is calculated with dice rolls
- Verify terrain changes for CreateWall and LavaPool

**Deliverable**: Fully functional spell system with all 3 Dragon spells working

---

## Files You Need to Modify

### GameObjectTypes.h (Everyone)

**Current**:
```cpp
enum class GameObjectTypes {
    Cat, Robot, Asteroid, Crates, Meteor, Ship,
    Floor, Portal, Score, Laser, Particle,
    Character,  // Added in Week 2
    Count,
};
```

**Suggested Update** (Week 3):
```cpp
enum class GameObjectTypes {
    Cat, Robot, Asteroid, Crates, Meteor, Ship,
    Floor, Portal, Score, Laser, Particle,
    Character,      // Week 2
    TreasureBox,    // Week 3 - Dev A
    Spell,          // Week 3 - Dev E (for spell projectile visualization)
    WallTile,       // Week 3 - Dev E (CreateWall spell)
    LavaTile,       // Week 3 - Dev E (LavaPool spell)
    Count,
};
```

---

## Common Issues & Solutions

### Issue 1: Mockup includes not found

**Problem**: `#include "Test/Week3TestMocks.h"` fails to compile

**Solution**:
```cpp
// Use correct relative path from your source file
#include "../Test/Week3TestMocks.h"  // From States/
#include "Test/Week3TestMocks.h"     // From Game/DragonicTactics/
#include "../../Test/Week3TestMocks.h"  // From Objects/Spells/
```

### Issue 2: Character* is incomplete type

**Problem**: Mockups use `Character*` but it's not defined

**Solution**: Include Character.h before Week3TestMocks.h
```cpp
#include "Objects/Character.h"
#include "Test/Week3TestMocks.h"
```

### Issue 3: Math::ivec2 not recognized

**Problem**: Grid coordinates don't compile

**Solution**: Include Vec2.hpp
```cpp
#include "Engine/Vec2.hpp"
```

### Issue 4: EventBus events not defined

**Problem**: `SpellCastEvent` or other events don't exist yet

**Solution**: Add to `Types/Events.h` (example below)

```cpp
// Add to Events.h

struct SpellCastEvent {
    Character* caster;
    std::string spell_name;
    Math::ivec2 target_tile;
    int spell_level;
};

struct SpellEffectAppliedEvent {
    Character* target;
    std::string effect_type;  // "Damage", "Heal", "Terrain", etc.
    int magnitude;
    std::string spell_name;
};

struct TreasureOpenedEvent {
    GameObject* treasure_box;
    Character* opener;
    TreasureType reward_type;
    Math::ivec2 grid_position;
};

struct ItemCollectedEvent {
    Character* character;
    std::string item_type;
};

struct TerrainChangedEvent {
    Math::ivec2 tile_position;
    TileType old_type;
    TileType new_type;
    std::string source_spell;
};
```

---

## Contact & Coordination

**Daily Standup**: 9:00 AM (10 minutes)
- What did you complete yesterday?
- What are you working on today?
- Any blockers?

**Friday Integration Meeting**: 2:00 PM (60 minutes)
- Demo all Week 3 features
- Integration testing
- Plan Week 4 tasks

**Merge Strategy**:
1. Dev A commits TreasureBox implementation to feature branch `week3/treasure-box`
2. Dev E commits Spell system to feature branch `week3/spell-system`
3. Friday: Merge both branches to `week3/integration`
4. Test integration branch together
5. Merge to `main` after all tests pass

---

## Summary

### Week 2 Status: ✅ COMPLETE
- Character, Dragon, Fighter classes implemented
- All components (GridPosition, Stats, ActionPoints, SpellSlots) working
- EventBus, DiceManager, CombatSystem, DataRegistry functional
- GridSystem and TurnManager operational
- JSON data loading working

### Week 3 Status: 🚧 READY FOR DEVELOPMENT
- **Dev A**: Use `MockTreasureBox` to implement treasure system + visualization
- **Dev E**: Use spell mockups (`MockFireball`, `MockCreateWall`, `MockLavaPool`) to implement full spell system
- All mockups tested and ready
- Test suite available (`Week3Tests::RunAllWeek3Tests()`)
- Data file templates provided

### Success Criteria (End of Week 3)
- ✅ Dragon can cast Fireball, CreateWall, and LavaPool spells
- ✅ Spell slots are consumed when casting
- ✅ Spells interact with GridSystem (terrain changes)
- ✅ TreasureBox can be placed on grid and opened
- ✅ Treasure rewards are applied to characters
- ✅ All visual effects working (sprites, animations, particles)
- ✅ All tests passing (Week3Tests + integration tests)

**Let's build Week 3! 🔥🧱🌋💰**
