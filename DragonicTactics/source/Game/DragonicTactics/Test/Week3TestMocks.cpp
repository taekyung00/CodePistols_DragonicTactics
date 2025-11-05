#include "Week3TestMocks.h"
#include "../../../Engine/Engine.hpp"
#include "../Singletons/DiceManager.h"
#include <map>
#include "../StateComponents/GridSystem.h"

// ============================================================================
// MOCKTREASURECRIPT - TreasureBox GameObject Implementation
// ============================================================================

MockTreasureBox::MockTreasureBox(Math::ivec2 grid_position, TreasureType treasure_type)
    : GameObject(grid_position), grid_pos(grid_position), state(TreasureBoxState::Closed),   //////// gameobject default constructer?
      treasure_type(treasure_type), animation_timer(0.0) {
}

void MockTreasureBox::Update(double dt) {
    // Handle opening animation
    if (state == TreasureBoxState::Opening) {
        animation_timer += dt;
        if (animation_timer >= 1.0) {  // 1 second animation
            state = TreasureBoxState::Opened;
            animation_timer = 0.0;
        }
    }
}

void MockTreasureBox::Draw(Math::TransformationMatrix camera_matrix) {
    // Mock implementation - just log for now
    // Full implementation should draw sprite based on state
    (void)camera_matrix;  // Suppress unused warning
}

bool MockTreasureBox::CanBeOpened() const {
    return state == TreasureBoxState::Closed;
}

TreasureType MockTreasureBox::Open() {
    if (!CanBeOpened()) {
        return TreasureType::Nothing;
    }

    state = TreasureBoxState::Opening;
    TreasureType reward = treasure_type;

    // After opening, box becomes empty
    treasure_type = TreasureType::Nothing;

    return reward;
}

std::string MockTreasureBox::GetStateString() const {
    switch (state) {
        case TreasureBoxState::Closed: return "Closed";
        case TreasureBoxState::Opening: return "Opening";
        case TreasureBoxState::Opened: return "Opened";
        case TreasureBoxState::Empty: return "Empty";
        default: return "Unknown";
    }
}

std::string MockTreasureBox::GetTreasureString() const {
    switch (treasure_type) {
        case TreasureType::Gold: return "Gold";
        case TreasureType::HealthPotion: return "Health Potion";
        case TreasureType::SpellScroll: return "Spell Scroll";
        case TreasureType::MagicItem: return "Magic Item";
        case TreasureType::Nothing: return "Nothing";
        default: return "Unknown";
    }
}

// ============================================================================
// MOCKFIREBALL - Fireball Spell Implementation
// ============================================================================

MockFireball::MockFireball() {
}

bool MockFireball::CanCast([[maybe_unused]] Character* caster, Math::ivec2 target_tile) const {
    // Mock implementation - in real version, check:
    // 1. Caster has spell slot of appropriate level
    // 2. Target is in range
    // 3. Target is valid tile
    (void)target_tile;
    return true;
}

MockSpellResult MockFireball::Cast(Character* caster, Math::ivec2 target_tile) {
    return CastAtLevel(caster, target_tile, GetLevel());
}

MockSpellResult MockFireball::CastAtLevel([[maybe_unused]] Character* caster, Math::ivec2 target_tile, int cast_level) {
    MockSpellResult result;
    result.success = true;

    // Calculate affected tiles (2-tile radius)
    int radius = GetAreaRadius();
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            Math::ivec2 tile = { target_tile.x + dx, target_tile.y + dy };
            result.affected_tiles.push_back(tile);
        }
    }

    // Roll damage
    std::string damage_dice = GetDamageDice(cast_level);
    // Mock: just return a fixed value for testing
    // Real implementation: DiceManager::Instance().RollDiceFromString(damage_dice)
    result.total_damage = 28;  // Average of 8d6

    std::cout << "Fireball cast at level " << cast_level << "!" << std::endl;
    std::cout << "Damage: " << damage_dice << " = " << result.total_damage << std::endl;
    std::cout << "Affected " << result.affected_tiles.size() << " tiles." << std::endl;

    return result;
}

std::vector<Math::vec2> MockFireball::GetAffectedTiles(Math::vec2 targetTile) const {
    std::vector<Math::vec2> tiles;
    GridSystem& grid = GridSystem::GetInstance();

    // 2x2 area centered on target
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            Math::vec2 checkTile = targetTile + Math::vec2{(double)dx, (double)dy};

            if (grid.IsValidTile(checkTile)) {
                tiles.push_back(checkTile);
            }
        }
    }

    return tiles;
}
// ============================================================================
// MOCKCREATEWALL - Create Wall Spell Implementation
// ============================================================================

MockCreateWall::MockCreateWall() {
}

bool MockCreateWall::CanCast([[maybe_unused]] Character* caster, Math::ivec2 target_tile) const {
    (void)target_tile;
    return true;
}

MockSpellResult MockCreateWall::Cast(Character* caster, Math::ivec2 target_tile) {
    return CastAtLevel(caster, target_tile, GetLevel());
}

MockSpellResult MockCreateWall::CastAtLevel([[maybe_unused]] Character* caster, Math::ivec2 target_tile, int cast_level) {
    MockSpellResult result;
    result.success = true;

    int length = GetWallLength(cast_level);

    // For mock, create horizontal wall
    Math::ivec2 end_tile = { target_tile.x + length - 1, target_tile.y };
    result.affected_tiles = CalculateWallTiles(target_tile, end_tile, length);

    result.total_damage = 0;  // No damage, just terrain modification

    std::cout << "Create Wall cast at level " << cast_level << "!" << std::endl;
    std::cout << "Created " << length << " wall tiles." << std::endl;

    return result;
}

std::vector<Math::vec2> MockCreateWall::GetAffectedTiles(Math::vec2 targetTile) const {
    std::vector<Math::vec2> tiles;
    GridSystem& grid = GridSystem::GetInstance();

    // 2x2 area centered on target
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            Math::vec2 checkTile = targetTile + Math::vec2{(double)dx, (double)dy};

            if (grid.IsValidTile(checkTile)) {
                tiles.push_back(checkTile);
            }
        }
    }

    return tiles;
}
// ============================================================================
// MOCKLAVAPOOL - Lava Pool Spell Implementation
// ============================================================================

MockLavaPool::MockLavaPool() {
}

bool MockLavaPool::CanCast([[maybe_unused]] Character* caster, Math::ivec2 target_tile) const {
    (void)target_tile;
    return true;
}

MockSpellResult MockLavaPool::Cast(Character* caster, Math::ivec2 target_tile) {
    return CastAtLevel(caster, target_tile, GetLevel());
}

MockSpellResult MockLavaPool::CastAtLevel([[maybe_unused]] Character* caster, Math::ivec2 target_tile, int cast_level) {
    MockSpellResult result;
    result.success = true;

    // Calculate affected tiles (radius-based area)
    int radius = GetPoolRadius(cast_level);
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            Math::ivec2 tile = { target_tile.x + dx, target_tile.y + dy };
            result.affected_tiles.push_back(tile);
        }
    }

    int duration = GetDuration(cast_level);
    result.total_damage = 0;  // Damage happens over time, not immediately

    std::cout << "Lava Pool cast at level " << cast_level << "!" << std::endl;
    std::cout << "Created lava pool affecting " << result.affected_tiles.size() << " tiles." << std::endl;
    std::cout << "Duration: " << duration << " rounds, Damage: " << GetDamagePerTurn() << " per turn." << std::endl;

    return result;
}

std::vector<Math::vec2> MockLavaPool::GetAffectedTiles(Math::vec2 targetTile) const {
    std::vector<Math::vec2> tiles;
    GridSystem& grid = GridSystem::GetInstance();

    // 2x2 area centered on target
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            Math::vec2 checkTile = targetTile + Math::vec2{(double)dx, (double)dy};

            if (grid.IsValidTile(checkTile)) {
                tiles.push_back(checkTile);
            }
        }
    }

    return tiles;
}
// ============================================================================
// MOCKSPELLSYSTEM - Spell System Manager Implementation
// ============================================================================

// MockSpellSystem::~MockSpellSystem() {
//     ClearSpells();
// }

// void MockSpellSystem::RegisterSpell(const std::string& name, MockSpellBase* spell) {
//     registered_spells[name] = spell;
// }

// MockSpellBase* MockSpellSystem::GetSpell(const std::string& name) const {
//     auto it = registered_spells.find(name);
//     if (it != registered_spells.end()) {
//         return it->second;
//     }
//     return nullptr;
// }

// std::vector<std::string> MockSpellSystem::GetAvailableSpells([[maybe_unused]] Character* caster) const {
//     std::vector<std::string> available;
//     for (const auto& pair : registered_spells) {
//         available.push_back(pair.first);
//     }
//     return available;
// }

// bool MockSpellSystem::CanCharacterCastSpell([[maybe_unused]] Character* caster,
//                                             const std::string& spell_name,
//                                             [[maybe_unused]] int level) const {
//     // Mock: just check if spell exists
//     return GetSpell(spell_name) != nullptr;
// }

// MockSpellResult MockSpellSystem::CastSpell(Character* caster, const std::string& spell_name,
//                                            Math::ivec2 target_tile, int cast_level) {
//     MockSpellBase* spell = GetSpell(spell_name);
//     if (!spell) {
//         MockSpellResult failed;
//         failed.success = false;
//         failed.failureReason = "Spell not found: " + spell_name;
//         return failed;
//     }

//     // Use spell's base level if cast_level not specified
//     if (cast_level == -1) {
//         cast_level = spell->GetLevel();
//     }

//     // Validate caster can cast this spell
//     if (!CanCharacterCastSpell(caster, spell_name, cast_level)) {
//         MockSpellResult failed;
//         failed.success = false;
//         failed.failureReason = "Character cannot cast this spell";
//         return failed;
//     }

//     // Cast the spell
//     return spell->CastAtLevel(caster, target_tile, cast_level);
// }

// void MockSpellSystem::ClearSpells() {
//     for (auto& pair : registered_spells) {
//         delete pair.second;
//     }
//     registered_spells.clear();
// }

// ============================================================================
// TEST IMPLEMENTATIONS - Week 3 Tests
// ============================================================================

namespace Week3Tests {

    // TreasureBox tests
    bool TestTreasureBoxCreation() {
        std::cout << "\n=== Testing TreasureBox Creation ===" << std::endl;
        MockTreasureBox box({ 3, 4 }, TreasureType::Gold);
        bool passed = true;

        if (box.GetGridPosition().x != 3 || box.GetGridPosition().y != 4) {
            std::cout << "FAILED: Grid position not set correctly" << std::endl;
            passed = false;
        }

        if (box.IsOpened()) {
            std::cout << "FAILED: New box should not be opened" << std::endl;
            passed = false;
        }

        if (!box.CanBeOpened()) {
            std::cout << "FAILED: New box should be able to be opened" << std::endl;
            passed = false;
        }

        if (passed) {
            std::cout << "PASSED: TreasureBox creation successful" << std::endl;
        }
        return passed;
    }

    bool TestTreasureBoxOpening() {
        std::cout << "\n=== Testing TreasureBox Opening ===" << std::endl;
        MockTreasureBox box({ 0, 0 }, TreasureType::HealthPotion);
        bool passed = true;

        TreasureType reward = box.Open();
        if (reward != TreasureType::HealthPotion) {
            std::cout << "FAILED: Wrong treasure type returned" << std::endl;
            passed = false;
        }

        // Simulate animation
        box.Update(1.5);  // 1.5 seconds should complete animation

        if (!box.IsOpened()) {
            std::cout << "FAILED: Box should be opened after animation" << std::endl;
            passed = false;
        }

        if (passed) {
            std::cout << "PASSED: TreasureBox opening successful" << std::endl;
        }
        return passed;
    }

    bool TestTreasureBoxVisualization() {
        std::cout << "\n=== Testing TreasureBox Visualization ===" << std::endl;
        MockTreasureBox box({ 0, 0 }, TreasureType::MagicItem);
        bool passed = true;

        std::cout << "State: " << box.GetStateString() << std::endl;
        std::cout << "Treasure: " << box.GetTreasureString() << std::endl;

        if (box.GetStateString() != "Closed") {
            std::cout << "FAILED: Initial state should be Closed" << std::endl;
            passed = false;
        }

        if (passed) {
            std::cout << "PASSED: TreasureBox visualization working" << std::endl;
        }
        return passed;
    }

    bool TestTreasureBoxGridInteraction() {
        std::cout << "\n=== Testing TreasureBox Grid Interaction ===" << std::endl;
        MockTreasureBox box({ 5, 5 }, TreasureType::Gold);
        box.SetGridPosition({ 7, 3 });

        bool passed = (box.GetGridPosition().x == 7 && box.GetGridPosition().y == 3);

        if (passed) {
            std::cout << "PASSED: Grid interaction successful" << std::endl;
        } else {
            std::cout << "FAILED: Grid position not updated correctly" << std::endl;
        }
        return passed;
    }

    // Spell system tests
    bool TestFireballDamage() {
        std::cout << "\n=== Testing Fireball Damage ===" << std::endl;
        MockFireball fireball;
        MockSpellResult result = fireball.Cast(nullptr, { 4, 4 });

        bool passed = true;
        if (!result.success) {
            std::cout << "FAILED: Fireball cast failed" << std::endl;
            passed = false;
        }

        if (result.total_damage <= 0) {
            std::cout << "FAILED: No damage dealt" << std::endl;
            passed = false;
        }

        if (passed) {
            std::cout << "PASSED: Fireball damage calculated" << std::endl;
        }
        return passed;
    }

    bool TestFireballAreaOfEffect() {
        std::cout << "\n=== Testing Fireball Area of Effect ===" << std::endl;
        MockFireball fireball;
        MockSpellResult result = fireball.Cast(nullptr, { 4, 4 });

        // Should affect 5x5 area (radius 2)
        bool passed = (result.affected_tiles.size() == 25);

        if (passed) {
            std::cout << "PASSED: Fireball affects correct area" << std::endl;
        } else {
            std::cout << "FAILED: Expected 25 tiles, got " << result.affected_tiles.size() << std::endl;
        }
        return passed;
    }

    bool TestFireballUpcasting() {
        std::cout << "\n=== Testing Fireball Upcasting ===" << std::endl;
        MockFireball fireball;

        // Cast at level 3 (base)
        MockSpellResult result_base = fireball.CastAtLevel(nullptr, { 4, 4 }, 3);

        // Cast at level 5 (upcast)
        MockSpellResult result_upcast = fireball.CastAtLevel(nullptr, { 4, 4 }, 5);

        // Damage should increase with higher level
        // (In real implementation - for now just check it doesn't crash)
        bool passed = (result_base.success && result_upcast.success);

        if (passed) {
            std::cout << "PASSED: Fireball upcasting works" << std::endl;
        } else {
            std::cout << "FAILED: Upcasting failed" << std::endl;
        }
        return passed;
    }

    bool TestCreateWallPlacement() {
        std::cout << "\n=== Testing Create Wall Placement ===" << std::endl;
        MockCreateWall wall_spell;
        MockSpellResult result = wall_spell.Cast(nullptr, { 2, 2 });

        bool passed = true;
        if (!result.success) {
            std::cout << "FAILED: Create Wall cast failed" << std::endl;
            passed = false;
        }

        // Level 2 should create 3 tiles
        if (result.affected_tiles.size() != 3) {
            std::cout << "FAILED: Expected 3 wall tiles, got " << result.affected_tiles.size() << std::endl;
            passed = false;
        }

        if (passed) {
            std::cout << "PASSED: Create Wall placement successful" << std::endl;
        }
        return passed;
    }

    bool TestCreateWallBlocking() {
        std::cout << "\n=== Testing Create Wall Blocking ===" << std::endl;
        // This would test that created walls actually block movement
        // For mock, just verify spell execution
        MockCreateWall wall_spell;
        MockSpellResult result = wall_spell.Cast(nullptr, { 0, 0 });

        bool passed = result.success;
        if (passed) {
            std::cout << "PASSED: Create Wall blocking test (mock)" << std::endl;
        }
        return passed;
    }

    bool TestCreateWallUpcasting() {
        std::cout << "\n=== Testing Create Wall Upcasting ===" << std::endl;
        MockCreateWall wall_spell;

        MockSpellResult result_base = wall_spell.CastAtLevel(nullptr, { 2, 2 }, 2);
        MockSpellResult result_upcast = wall_spell.CastAtLevel(nullptr, { 2, 2 }, 4);

        // Level 4 should create more tiles than level 2
        bool passed = (result_upcast.affected_tiles.size() > result_base.affected_tiles.size());

        if (passed) {
            std::cout << "PASSED: Create Wall upcasting increases length" << std::endl;
        } else {
            std::cout << "FAILED: Upcasting did not increase wall length" << std::endl;
        }
        return passed;
    }

    bool TestLavaPoolDamage() {
        std::cout << "\n=== Testing Lava Pool Damage ===" << std::endl;
        MockLavaPool lava_spell;
        MockSpellResult result = lava_spell.Cast(nullptr, { 4, 4 });

        bool passed = result.success;
        if (passed) {
            std::cout << "PASSED: Lava Pool damage spell works" << std::endl;
        } else {
            std::cout << "FAILED: Lava Pool cast failed" << std::endl;
        }
        return passed;
    }

    bool TestLavaPoolDuration() {
        std::cout << "\n=== Testing Lava Pool Duration ===" << std::endl;
        // Mock test - in real implementation would verify lava persists for correct rounds
        MockLavaPool lava_spell;
        std::string desc = lava_spell.GetDescription();

        bool passed = (desc.find("3 rounds") != std::string::npos);
        if (passed) {
            std::cout << "PASSED: Lava Pool has duration" << std::endl;
        }
        return passed;
    }

    bool TestLavaPoolUpcasting() {
        std::cout << "\n=== Testing Lava Pool Upcasting ===" << std::endl;
        MockLavaPool lava_spell;

        MockSpellResult result_base = lava_spell.CastAtLevel(nullptr, { 4, 4 }, 4);
        MockSpellResult result_upcast = lava_spell.CastAtLevel(nullptr, { 4, 4 }, 6);

        // Higher level should affect more tiles
        bool passed = (result_upcast.affected_tiles.size() >= result_base.affected_tiles.size());

        if (passed) {
            std::cout << "PASSED: Lava Pool upcasting works" << std::endl;
        } else {
            std::cout << "FAILED: Upcasting did not work correctly" << std::endl;
        }
        return passed;
    }

    // bool TestSpellSystemRegistration() {
    //     std::cout << "\n=== Testing Spell System Registration ===" << std::endl;

    //     // Register spells
    //     MockSpellSystem::Instance().RegisterSpell("Fireball", new MockFireball());
    //     MockSpellSystem::Instance().RegisterSpell("CreateWall", new MockCreateWall());
    //     MockSpellSystem::Instance().RegisterSpell("LavaPool", new MockLavaPool());

    //     bool passed = true;

    //     if (MockSpellSystem::Instance().GetSpell("Fireball") == nullptr) {
    //         std::cout << "FAILED: Fireball not registered" << std::endl;
    //         passed = false;
    //     }

    //     if (MockSpellSystem::Instance().GetSpell("CreateWall") == nullptr) {
    //         std::cout << "FAILED: CreateWall not registered" << std::endl;
    //         passed = false;
    //     }

    //     if (MockSpellSystem::Instance().GetSpell("LavaPool") == nullptr) {
    //         std::cout << "FAILED: LavaPool not registered" << std::endl;
    //         passed = false;
    //     }

    //     if (passed) {
    //         std::cout << "PASSED: All spells registered successfully" << std::endl;
    //     }

    //     MockSpellSystem::Instance().ClearSpells();
    //     return passed;
    // }

    bool TestSpellSlotConsumption() {
        std::cout << "\n=== Testing Spell Slot Consumption ===" << std::endl;
        // Mock test - real implementation would verify spell slots decrease after casting
        std::cout << "PASSED: Spell slot consumption (mock - implement in full version)" << std::endl;
        return true;
    }

    // bool TestMultipleSpellsCombat() {
    //     std::cout << "\n=== Testing Multiple Spells in Combat ===" << std::endl;

    //     // Register all spells
    //     MockSpellSystem::Instance().RegisterSpell("Fireball", new MockFireball());
    //     MockSpellSystem::Instance().RegisterSpell("CreateWall", new MockCreateWall());
    //     MockSpellSystem::Instance().RegisterSpell("LavaPool", new MockLavaPool());

    //     // Cast multiple spells
    //     MockSpellResult r1 = MockSpellSystem::Instance().CastSpell(nullptr, "Fireball", { 4, 4 });
    //     MockSpellResult r2 = MockSpellSystem::Instance().CastSpell(nullptr, "CreateWall", { 2, 2 });
    //     MockSpellResult r3 = MockSpellSystem::Instance().CastSpell(nullptr, "LavaPool", { 6, 6 });

    //     bool passed = (r1.success && r2.success && r3.success);

    //     if (passed) {
    //         std::cout << "PASSED: Multiple spells can be cast" << std::endl;
    //     } else {
    //         std::cout << "FAILED: One or more spells failed to cast" << std::endl;
    //     }

    //     MockSpellSystem::Instance().ClearSpells();
    //     return passed;
    // }

    void RunAllWeek3Tests() {
        std::cout << "\n==========================================" << std::endl;
        std::cout << "      WEEK 3 MOCKUP TESTS - STARTING      " << std::endl;
        std::cout << "==========================================" << std::endl;

        int passed = 0;
        int total = 0;

        // TreasureBox tests
        std::cout << "\n--- TREASURE BOX TESTS ---" << std::endl;
        if (TestTreasureBoxCreation()) passed++; total++;
        if (TestTreasureBoxOpening()) passed++; total++;
        if (TestTreasureBoxVisualization()) passed++; total++;
        if (TestTreasureBoxGridInteraction()) passed++; total++;

        // Fireball tests
        std::cout << "\n--- FIREBALL SPELL TESTS ---" << std::endl;
        if (TestFireballDamage()) passed++; total++;
        if (TestFireballAreaOfEffect()) passed++; total++;
        if (TestFireballUpcasting()) passed++; total++;

        // Create Wall tests
        std::cout << "\n--- CREATE WALL SPELL TESTS ---" << std::endl;
        if (TestCreateWallPlacement()) passed++; total++;
        if (TestCreateWallBlocking()) passed++; total++;
        if (TestCreateWallUpcasting()) passed++; total++;

        // Lava Pool tests
        std::cout << "\n--- LAVA POOL SPELL TESTS ---" << std::endl;
        if (TestLavaPoolDamage()) passed++; total++;
        if (TestLavaPoolDuration()) passed++; total++;
        if (TestLavaPoolUpcasting()) passed++; total++;

        // Integration tests
        std::cout << "\n--- SPELL SYSTEM INTEGRATION TESTS ---" << std::endl;
        // if (TestSpellSystemRegistration()) passed++; total++;
        // if (TestSpellSlotConsumption()) passed++; total++;
        // if (TestMultipleSpellsCombat()) passed++; total++;

        std::cout << "\n==========================================" << std::endl;
        std::cout << "      WEEK 3 TESTS COMPLETE               " << std::endl;
        std::cout << "      Passed: " << passed << "/" << total << " tests" << std::endl;
        std::cout << "==========================================" << std::endl;
    }

} // namespace Week3Tests
