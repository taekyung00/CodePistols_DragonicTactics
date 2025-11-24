// File: CS230/Game/System/SpellSystem.cpp
#include "SpellSystem.h"
#include "../Objects/Character.h"
#include "../StateComponents/EventBus.h"
#include "./Engine/Engine.hpp"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "../Types/Events.h"

// class Character;

// ///////////////////////////./////////////////////////////////////////////////////////////////////////////////////////////////////
// // Step 1.1: Define spell result structure
// // Reason: Spells need to communicate back what happened (damage dealt, tiles affected, etc.)
// // This enables proper event publishing and UI feedback
// struct SpellResult {
//     bool success;                          // Did the spell cast successfully?
//     std::vector<Character*> affectedTargets; // Who was hit?
//     std::vector<Math::vec2> affectedTiles;   // Which tiles were affected?
//     int damageDealt;                         // Total damage (for damage spells)
//     std::string failureReason;               // Why it failed (out of range, no slots, etc.)
// };

// // Step 1.2: Define spell targeting types
// // Reason: Different spells target differently (single enemy, area, self, etc.)
// // This allows UI to show appropriate targeting cursors
// enum class SpellTargetType {
//     Single,      // Target one character (e.g., Magic Missile)
//     Area,        // Target area centered on a tile (e.g., Fireball)
//     Line,        // Target tiles in a line (e.g., Lightning Bolt)
//     Self,        // Target caster only (e.g., Shield spell)
//     AllEnemies   // Target all enemies (rare, powerful spells)
// };

// // Step 1.3: Base spell interface
// // Reason: All spells share common properties (name, cost, range) and behavior (validation, casting)
// // Using pure virtual functions ensures every spell implements required methods
// class SpellBase {
// public:
//     virtual ~SpellBase() = default;

//     // Core spell information
//     virtual std::string GetName() const = 0;
//     virtual int GetLevel() const = 0;
//     virtual int GetRange() const = 0;
//     virtual SpellTargetType GetTargetType() const = 0;
//     virtual std::string GetDescription() const = 0;

//     // Spell validation (before casting)
//     // Reason: Prevent invalid casts (out of range, invalid target, etc.)
//     virtual bool CanCast(Character* caster, Math::vec2 targetTile) const = 0;

//     // Spell execution
//     // Reason: This is where the spell's unique effect happens
//     virtual SpellResult Cast(Character* caster, Math::vec2 targetTile, int upcastLevel = 0) = 0;

//     // Area of effect calculation
//     // Reason: Some spells affect multiple tiles (Fireball, LavaPool)
//     // Returns all tiles affected by the spell
//     virtual std::vector<Math::vec2> GetAffectedTiles(Math::vec2 targetTile) const = 0;

// protected:
//     // Helper: Check if target is in range
//     // Reason: Common logic shared by all spells
//     bool IsInRange(Math::vec2 casterPos, Math::vec2 targetPos, int range) const {
//         int dx = abs((int)targetPos.x - (int)casterPos.x);
//         int dy = abs((int)targetPos.y - (int)casterPos.y);
//         return (dx + dy) <= range; // Manhattan distance
//     }
// };
// ////////////////////////////////////////////////////////////////////////////////////////////////


// Step 1.9: Singleton implementation


SpellSystem::SpellSystem() {
    //Engine::GetLogger().LogEvent("SpellSystem initialized");
}

SpellSystem::~SpellSystem() {
    spells.clear();
}

// Step 2.1: Register a spell
// Reason: Add spell to the system's registry
void SpellSystem::RegisterSpell(const std::string& spellName, MockSpellBase* spell) {
    if (!spell) {
        Engine::GetLogger().LogError("SpellSystem: Attempted to register null spell");
        return;
    }

    // Check if spell already registered
    if (spells.find(spellName) != spells.end()) {
        Engine::GetLogger().LogError("SpellSystem: Spell '" + spellName + "' already registered");
        return;
    }

    // Step 2.1a: Store spell in registry
    // Reason: unique_ptr ensures proper cleanup
    spells[spellName] = std::unique_ptr<MockSpellBase>(spell);

    Engine::GetLogger().LogEvent("SpellSystem: Registered spell '" + spellName + "'");
}

// Step 2.2: Retrieve a spell by name
// Reason: Allow other systems to look up spells
MockSpellBase* SpellSystem::GetSpell(const std::string& spellName) {
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        Engine::GetLogger().LogError("SpellSystem: Spell '" + spellName + "' not found");
        return nullptr;
    }

    return it->second.get();
}

// Step 3.1: Validate spell slots
// Reason: Caster must have available spell slots of the required level
bool SpellSystem::ValidateSpellSlots(Character* caster, int requiredLevel) const {
    if (!caster) return false;

    // Check if caster has any slots of the required level or higher
    for (int level = requiredLevel; level <= 9; ++level) {
        if (caster->GetSpellSlotCount(level) > 0) {
            return true;
        }
    }

    return false;
}

// Step 3.2: Validate target
// Reason: Use spell's own validation logic
bool SpellSystem::ValidateTarget(MockSpellBase* spell, Character* caster, Math::vec2 targetTile) const {
    if (!spell || !caster) return false;

    return spell->CanCast(caster, targetTile);
}

// Step 3.3: Main validation method
// Reason: Check all prerequisites before allowing cast
bool SpellSystem::CanCastSpell(
    Character* caster,
    const std::string& spellName,
    Math::vec2 targetTile,
    int upcastLevel
) const {
    // Check 1: Spell exists
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        return false;
    }

    MockSpellBase* spell = it->second.get();

    // Check 2: Determine required spell slot level
    int requiredLevel = (upcastLevel > 0) ? upcastLevel : spell->GetLevel();

    // Check 3: Validate spell slots
    if (!ValidateSpellSlots(caster, requiredLevel)) {
        return false;
    }

    // Check 4: Validate target
    if (!ValidateTarget(spell, caster, targetTile)) {
        return false;
    }

    return true;
}

// Step 4.1: Cast spell
// Reason: Main entry point for spell casting
MockSpellResult SpellSystem::CastSpell(
    Character* caster,
    const std::string& spellName,
    Math::vec2 targetTile,
    int upcastLevel
) {
    MockSpellResult result;
    result.success = false;

    // Step 4.1a: Validate spell exists
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        result.failureReason = "Spell '" + spellName + "' not found";
        Engine::GetLogger().LogError("SpellSystem: " + result.failureReason);
        return result;
    }

    MockSpellBase* spell = it->second.get();

    // Step 4.1b: Validate can cast
    if (!CanCastSpell(caster, spellName, targetTile, upcastLevel)) {
        result.failureReason = "Cannot cast " + spellName;
        Engine::GetLogger().LogError("SpellSystem: " + result.failureReason);
        return result;
    }

    // Step 4.1c: Log spell cast attempt
    Engine::GetLogger().LogEvent(
        "SpellSystem: " + caster->TypeName() + " casting " + spellName +
        " at (" + std::to_string((int)targetTile.x) + "," +
        std::to_string((int)targetTile.y) + ")" +
        (upcastLevel > 0 ? " (upcast to level " + std::to_string(upcastLevel) + ")" : "")
    );
	caster->ConsumeSpell(spell->GetLevel());
    // Step 4.1d: Execute the spell
    // Reason: Delegate to the spell's own Cast() method
    result = spell->Cast(caster, targetTile/*, upcastLevel*/);

    // Step 4.1e: Publish system-level event
    // Reason: Other systems may need to react to spell casts
    if (result.success) {
        if (eventBus) {
            eventBus->Publish(SpellCastEventForSpell{
                caster,
                spellName,
                targetTile,
                result.affected_targets,
                result.total_damage
            });
        } else {
            // Fallback to GameStateManager if no EventBus was set
            auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
            if (bus) {
                bus->Publish(SpellCastEventForSpell{
                    caster,
                    spellName,
                    targetTile,
                    result.affected_targets,
                    result.total_damage
                });
            }
        }
    }

    return result;
}

// Step 5.1: Get available spells for a caster
// Reason: UI needs to show which spells the character can cast
std::vector<std::string> SpellSystem::GetAvailableSpells(Character* caster) const {
    std::vector<std::string> available;

    if (!caster) return available;

    // Check each registered spell
    for (const auto& pair : spells) {
        const std::string& spellName = pair.first;
        MockSpellBase* spell = pair.second.get();

        // Check if caster has spell slots for this spell level
        if (ValidateSpellSlots(caster, spell->GetLevel())) {
            available.push_back(spellName);
        }
    }

    return available;
}

// Step 5.2: Get spell slot count
// Reason: UI displays remaining spell slots
int SpellSystem::GetSpellSlotCount(Character* caster, int level) const {
    if (!caster || level < 1 || level > 9) {
        return 0;
    }
    return caster->GetSpellSlotCount(level);
}

// Step 5.3: Preview spell area
// Reason: Show player which tiles will be affected before casting
std::vector<Math::vec2> SpellSystem::PreviewSpellArea(
    const std::string& spellName,
    Math::vec2 targetTile
) const {
    auto it = spells.find(spellName);
    if (it == spells.end()) {
        return {};
    }

    MockSpellBase* spell = it->second.get();
    return spell->GetAffectedTiles(targetTile);
}