
// File: CS230/Game/System/SpellSystem.h
#pragma once
// #include "../Spells/SpellBase.h"
#include "../../../Engine/Vec2.h"
#include "./Engine/Component.h"
#include "./Game/DragonicTactics/Test/Week3TestMocks.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Character;
class EventBus;

// Step 1.1: SpellSystem singleton class
// Reason: Centralized spell management accessible from anywhere
class SpellSystem : public CS230::Component
{
  public:
  SpellSystem();
  ~SpellSystem();

  // Set EventBus for testing or runtime use
  void SetEventBus(EventBus* bus)
  {
	eventBus = bus;
  }

  // Step 1.2: Spell registration
  // Reason: Spells must be registered before they can be used
  void			 RegisterSpell(const std::string& spellName, MockSpellBase* spell);
  MockSpellBase* GetSpell(const std::string& spellName);

  // Step 1.3: Spell casting interface
  // Reason: Main entry point for casting spells
  MockSpellResult CastSpell(Character* caster, const std::string& spellName, Math::vec2 targetTile, int upcastLevel = 0);

  // Step 1.4: Spell validation
  // Reason: Check if spell can be cast before committing resources
  bool CanCastSpell(Character* caster, const std::string& spellName, Math::vec2 targetTile, int upcastLevel = 0) const;

  // Step 1.5: Spell slot management helpers
  // Reason: Query available spells for UI
  std::vector<std::string> GetAvailableSpells(Character* caster) const;
  int					   GetSpellSlotCount(Character* caster, int level) const;

  // Step 1.6: Spell preview (for UI)
  // Reason: Show which tiles will be affected before casting
  std::vector<Math::vec2> PreviewSpellArea(const std::string& spellName, Math::vec2 targetTile) const;

  private:


  // Step 1.7: Spell registry
  // Reason: Store all registered spells
  std::map<std::string, std::unique_ptr<MockSpellBase>> spells;

  // Step 1.8: Validation helpers
  bool ValidateSpellSlots(Character* caster, int requiredLevel) const;
  bool ValidateTarget(MockSpellBase* spell, Character* caster, Math::vec2 targetTile) const;

  EventBus* eventBus = nullptr;
};