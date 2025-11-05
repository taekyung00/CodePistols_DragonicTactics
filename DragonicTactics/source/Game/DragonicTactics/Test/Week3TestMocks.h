#pragma once
// File: DragonicTactics/source/Game/DragonicTactics/Test/Week3TestMocks.h
// Purpose: Mockups for Week 3 independent development
// Dev A: TreasureBox object and visualization
// Dev E: Spell system (Fireball, CreateWall, LavaPool)

#include "../../../Engine/Vec2.hpp"
#include "../../../Engine/GameObject.h"
#include "../Types/CharacterTypes.h"
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>

// ============================================================================
// DEV A: TREASURE BOX MOCKUPS
// ============================================================================

// Enum for treasure box states
enum class TreasureBoxState {
    Closed,      // Box is closed, not yet opened
    Opening,     // Animation playing
    Opened,      // Box is open, contents visible
    Empty        // Box has been looted
};

// Enum for treasure types
enum class TreasureType {
    Gold,        // Currency
    HealthPotion,// Restore HP
    SpellScroll, // Learn new spell or restore spell slot
    MagicItem,   // Equipment upgrade
    Nothing      // Empty box (trap or already looted)
};

// Mock TreasureBox GameObject
// Purpose: Interactive object on the grid that contains loot
// Developer A should implement full version with sprite animation
class MockTreasureBox : public CS230::GameObject {
public:
    MockTreasureBox(Math::ivec2 grid_position, TreasureType treasure_type = TreasureType::Gold);

    // GameObject interface
    GameObjectTypes Type() override { return GameObjectTypes::Count; } // Placeholder until added to enum
    std::string TypeName() override { return "TreasureBox"; }

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;

    // TreasureBox specific methods
    bool CanBeOpened() const;
    TreasureType Open();  // Returns what was inside, changes state to Opened
    bool IsOpened() const { return state == TreasureBoxState::Opened || state == TreasureBoxState::Empty; }

    // Grid interaction
    Math::ivec2 GetGridPosition() const { return grid_pos; }
    void SetGridPosition(Math::ivec2 pos) { grid_pos = pos; }

    // Visualization helpers
    std::string GetStateString() const;
    std::string GetTreasureString() const;

private:
    Math::ivec2 grid_pos;
    TreasureBoxState state;
    TreasureType treasure_type;
    double animation_timer;  // For opening animation
};

// ============================================================================
// DEV E: SPELL SYSTEM MOCKUPS
// ============================================================================

// Forward declarations
class Character;
class GridSystem;

// Spell result structure - returned after casting
struct MockSpellResult {
    bool success;                              // Did spell cast successfully?
    std::vector<Character*> affected_targets;  // Characters hit by spell
    std::vector<Math::ivec2> affected_tiles;   // Tiles affected (for terrain changes)
    int total_damage;                          // Total damage dealt (for damage spells)
    std::string failureReason;                // Why it failed (if success = false)
};

// Spell target types
enum class SpellTargetType {
    SingleTarget,   // Target one character (e.g., Magic Missile)
    AreaOfEffect,   // Area centered on tile (e.g., Fireball)
    Line,           // Tiles in a line (e.g., Lightning Bolt)
    Self,           // Caster only (e.g., Shield)
    Cone            // Cone from caster (e.g., Burning Hands)
};

// Base spell interface - all spells inherit from this
class MockSpellBase {
public:
    virtual ~MockSpellBase() = default;

    // Core spell info
    virtual std::string GetName() const = 0;
    virtual int GetLevel() const = 0;
    virtual int GetRange() const = 0;
    virtual SpellTargetType GetTargetType() const = 0;
    virtual std::string GetDescription() const = 0;

    // Validation
    virtual bool CanCast(Character* caster, Math::ivec2 target_tile) const = 0;

    // Execution
    virtual MockSpellResult Cast(Character* caster, Math::ivec2 target_tile) = 0;

    // Upcasting support (cast at higher level for more power)
    virtual MockSpellResult CastAtLevel(Character* caster, Math::ivec2 target_tile, int cast_level);
    virtual std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const = 0;

protected:
    // Helper for range checking
    bool IsInRange(Math::ivec2 caster_pos, Math::ivec2 target_pos, int range) const;
};

// ============================================================================
// FIREBALL SPELL
// Level 3 evocation spell
// Area of effect damage spell - classic D&D fireball
// ============================================================================
class MockFireball : public MockSpellBase {
public:
    MockFireball();

    // MockSpellBase interface
    std::string GetName() const override { return "Fireball"; }
    int GetLevel() const override { return 3; }
    int GetRange() const override { return 6; }  // 6 tiles range
    SpellTargetType GetTargetType() const override { return SpellTargetType::AreaOfEffect; }
    std::string GetDescription() const override;

    bool CanCast(Character* caster, Math::ivec2 target_tile) const override;
    MockSpellResult Cast(Character* caster, Math::ivec2 target_tile) override;
    MockSpellResult CastAtLevel(Character* caster, Math::ivec2 target_tile, int cast_level) override;
    std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const override;
private:
    int GetAreaRadius() const { return 2; }  // 2-tile radius explosion
    std::string GetDamageDice(int cast_level) const;  // Returns "8d6" for level 3, +1d6 per level
};

// ============================================================================
// CREATE WALL SPELL
// Level 2 conjuration spell
// Creates impassable wall tiles on the grid
// ============================================================================
class MockCreateWall : public MockSpellBase {
public:
    MockCreateWall();

    // MockSpellBase interface
    std::string GetName() const override { return "Create Wall"; }
    int GetLevel() const override { return 2; }
    int GetRange() const override { return 4; }  // 4 tiles range
    SpellTargetType GetTargetType() const override { return SpellTargetType::Line; }
    std::string GetDescription() const override;

    bool CanCast(Character* caster, Math::ivec2 target_tile) const override;
    MockSpellResult Cast(Character* caster, Math::ivec2 target_tile) override;
    MockSpellResult CastAtLevel(Character* caster, Math::ivec2 target_tile, int cast_level) override;
    std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const override;

private:
    int GetWallLength(int cast_level) const;  // Returns 3 for level 2, +1 per level
    std::vector<Math::ivec2> CalculateWallTiles(Math::ivec2 start, Math::ivec2 end, int length) const;
};

// ============================================================================
// LAVA POOL SPELL
// Level 4 conjuration spell
// Creates damaging terrain (lava tiles) that hurt anyone standing on them
// ============================================================================
class MockLavaPool : public MockSpellBase {
public:
    MockLavaPool();

    // MockSpellBase interface
    std::string GetName() const override { return "Lava Pool"; }
    int GetLevel() const override { return 4; }
    int GetRange() const override { return 5; }  // 5 tiles range
    SpellTargetType GetTargetType() const override { return SpellTargetType::AreaOfEffect; }
    std::string GetDescription() const override;

    bool CanCast(Character* caster, Math::ivec2 target_tile) const override;
    MockSpellResult Cast(Character* caster, Math::ivec2 target_tile) override;
    MockSpellResult CastAtLevel(Character* caster, Math::ivec2 target_tile, int cast_level) override;
    std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const override;

private:
    int GetPoolRadius(int cast_level) const;  // Returns 1 for level 4, +1 every 2 levels
    std::string GetDamagePerTurn() const { return "2d6"; }  // Damage per turn standing in lava
    int GetDuration(int cast_level) const;  // Returns 3 rounds for level 4, +1 per level
};

// ============================================================================
// SPELL SYSTEM MANAGER (Singleton)
// Manages all spells, handles casting, validates spell slots
// ============================================================================
// class MockSpellSystem {
// public:
//     // Singleton access
//     static MockSpellSystem& Instance() {
//         static MockSpellSystem instance;
//         return instance;
//     }

//     // Spell casting
//     MockSpellResult CastSpell(Character* caster, const std::string& spell_name,
//                               Math::ivec2 target_tile, int cast_level = -1);

//     // Spell registration (call during initialization)
//     void RegisterSpell(const std::string& name, MockSpellBase* spell);

//     // Spell queries
//     MockSpellBase* GetSpell(const std::string& name) const;
//     std::vector<std::string> GetAvailableSpells(Character* caster) const;
//     bool CanCharacterCastSpell(Character* caster, const std::string& spell_name, int level) const;

//     // Cleanup
//     void ClearSpells();

// private:
//     MockSpellSystem() = default;
//     ~MockSpellSystem();
//     MockSpellSystem(const MockSpellSystem&) = delete;
//     MockSpellSystem& operator=(const MockSpellSystem&) = delete;

//     std::map<std::string, MockSpellBase*> registered_spells;
// };

// ============================================================================
// TEST HELPER FUNCTIONS
// ============================================================================

// Test assertions for Week 3
namespace Week3Tests {
    // TreasureBox tests
    bool TestTreasureBoxCreation();
    bool TestTreasureBoxOpening();
    bool TestTreasureBoxVisualization();
    bool TestTreasureBoxGridInteraction();

    // Spell system tests
    bool TestFireballDamage();
    bool TestFireballAreaOfEffect();
    bool TestFireballUpcasting();

    bool TestCreateWallPlacement();
    bool TestCreateWallBlocking();
    bool TestCreateWallUpcasting();

    bool TestLavaPoolDamage();
    bool TestLavaPoolDuration();
    bool TestLavaPoolUpcasting();

    // Integration tests
    // bool TestSpellSystemRegistration();
    // bool TestSpellSlotConsumption();
    // bool TestMultipleSpellsCombat();

    // Run all Week 3 tests
    void RunAllWeek3Tests();
}

// ============================================================================
// INLINE IMPLEMENTATIONS
// ============================================================================

// MockSpellBase inline methods
inline MockSpellResult MockSpellBase::CastAtLevel(Character* caster, Math::ivec2 target_tile, [[maybe_unused]]int cast_level) {
    // Default implementation: just call Cast() and ignore cast_level
    // Subclasses should override for true upcasting support
    return Cast(caster, target_tile);
}

inline bool MockSpellBase::IsInRange(Math::ivec2 caster_pos, Math::ivec2 target_pos, int range) const {
    // Manhattan distance for grid-based range
    int dx = std::abs(target_pos.x - caster_pos.x);
    int dy = std::abs(target_pos.y - caster_pos.y);
    return (dx + dy) <= range;
}

// MockFireball inline methods
inline std::string MockFireball::GetDescription() const {
    return "A bright streak flashes to a point you choose within range and then blossoms with a roar into an explosion of flame. Deals 8d6 fire damage in a 2-tile radius.";
}

inline std::string MockFireball::GetDamageDice(int cast_level) const {
    // Base: 8d6 at level 3
    // +1d6 per level above 3
    int dice_count = 8 + (cast_level - 3);
    return std::to_string(dice_count) + "d6";
}

// MockCreateWall inline methods
inline std::string MockCreateWall::GetDescription() const {
    return "Creates a line of solid stone walls blocking movement. Creates 3 wall tiles at level 2, +1 per level.";
}

inline int MockCreateWall::GetWallLength(int cast_level) const {
    // Base: 3 tiles at level 2
    // +1 per level above 2
    return 3 + (cast_level - 2);
}

inline std::vector<Math::ivec2> MockCreateWall::CalculateWallTiles(Math::ivec2 start, Math::ivec2 end, int length) const {
    std::vector<Math::ivec2> tiles;

    // Calculate direction vector
    int dx = end.x - start.x;
    int dy = end.y - start.y;

    // Normalize to -1, 0, or 1
    int step_x = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int step_y = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    // Generate tiles along the line
    Math::ivec2 current = start;
    for (int i = 0; i < length; ++i) {
        tiles.push_back(current);
        current.x += step_x;
        current.y += step_y;
    }

    return tiles;
}

// MockLavaPool inline methods
inline std::string MockLavaPool::GetDescription() const {
    return "Summons a pool of molten lava that damages any creature standing on it. Deals 2d6 fire damage per turn for 3 rounds.";
}

inline int MockLavaPool::GetPoolRadius(int cast_level) const {
    // Base: radius 1 at level 4 (3x3 area)
    // +1 radius every 2 levels
    return 1 + ((cast_level - 4) / 2);
}

inline int MockLavaPool::GetDuration(int cast_level) const {
    // Base: 3 rounds at level 4
    // +1 round per level
    return 3 + (cast_level - 4);
}
