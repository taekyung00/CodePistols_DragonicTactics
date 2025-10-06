## 5. Factory Pattern - Character Creation Documentation

**REASONING: Pure GameObject Inheritance Pattern**

Following CS230 engine patterns, characters use **pure GameObject inheritance** WITHOUT interfaces:

- `class Character : public CS230::GameObject` (base character class)
- `class Dragon : public Character` (player character)
- `class Fighter : public Character` (AI tank)
- `class Cleric : public Character` (AI support)
- `class Wizard : public Character` (AI glass cannon)
- `class Rogue : public Character` (AI assassin)

**NO interface inheritance** (no ICharacter, no IAIBehavior, etc.) - this matches existing CS230 code patterns like Cat.h:

```cpp
// CS230/Game/Cat.h line 22 - CORRECT CS230 pattern
class Cat : public CS230::GameObject { ... }

// Dragonic Tactics follows same pattern
class Character : public CS230::GameObject { ... }
class Dragon : public Character { ... }
```

---

## 6. Factory Pattern - Complete Usage Examples

**REASONING: Factory Pattern Benefits**

- Centralizes character creation logic
- Ensures consistent component attachment
- Loads data from DataRegistry (data-driven)
- Returns raw pointers for CS230 GameObjectManager (NO unique_ptr)

### 6.1 CharacterFactory - Complete Implementation

```cpp
// CharacterFactory.h - Character creation with pure GameObject inheritance
// REASONING: Centralized creation ensures consistency

#pragma once
#include "Character.h"
#include "Dragon.h"
#include "Fighter.h"
#include "Cleric.h"
#include "Wizard.h"
#include "Rogue.h"

enum class CharacterType {
    Dragon,
    Fighter,
    Cleric,
    Wizard,
    Rogue
};

class CharacterFactory {
public:
    // Create character by type
    static Character* CreateCharacter(CharacterType type, Math::vec2 gridPos);

private:
    // Type-specific factory methods
    static Dragon* CreateDragon(Math::vec2 gridPos);
    static Fighter* CreateFighter(Math::vec2 gridPos);
    static Cleric* CreateCleric(Math::vec2 gridPos);
    static Wizard* CreateWizard(Math::vec2 gridPos);
    static Rogue* CreateRogue(Math::vec2 gridPos);

    // Helper to load character data from JSON
    struct CharacterData {
        int maxHP;
        int speed;
        int maxActionPoints;
        int baseAttackPower;
        std::string attackDice;
        int baseDefensePower;
        std::string defenseDice;
        int attackRange;
        std::map<int, int> spellSlots;  // level -> count
    };

    static CharacterData LoadCharacterData(const std::string& characterName);
};
```

```cpp
// CharacterFactory.cpp - Character creation with all components
// REASONING: Centralized creation ensures consistency

#include "CharacterFactory.h"
#include "../Engine/DataRegistry.h"
#include "../Engine/Engine.h"

Character* CharacterFactory::CreateCharacter(CharacterType type, Math::vec2 gridPos) {
    switch(type) {
        case CharacterType::Dragon:  return CreateDragon(gridPos);
        case CharacterType::Fighter: return CreateFighter(gridPos);
        case CharacterType::Cleric:  return CreateCleric(gridPos);
        case CharacterType::Wizard:  return CreateWizard(gridPos);
        case CharacterType::Rogue:   return CreateRogue(gridPos);
        default: return nullptr;
    }
}

// ===== DRAGON FACTORY =====
Dragon* CharacterFactory::CreateDragon(Math::vec2 gridPos) {
    // Load stats from JSON (line 115-137 from dragonic_tactics.md)
    CharacterData data = LoadCharacterData("Dragon");

    // Create dragon instance
    Dragon* dragon = new Dragon(gridPos);

    // Set base stats
    dragon->maxHP = data.maxHP;
    dragon->currentHP = dragon->maxHP;
    dragon->speed = data.speed;
    dragon->maxActionPoints = data.maxActionPoints;
    dragon->baseAttackPower = data.baseAttackPower;
    dragon->attackDice = data.attackDice;
    dragon->baseDefensePower = data.baseDefensePower;
    dragon->defenseDice = data.defenseDice;
    dragon->attackRange = data.attackRange;
    dragon->spellSlots = data.spellSlots;

    // Add CS230 Engine components
    dragon->AddGOComponent(new CS230::Sprite("Assets/Dragon.spt", dragon));
    dragon->AddGOComponent(new CS230::RectCollision({-32, -32, 32, 32}, dragon));

    // Add Tactical RPG components
    dragon->AddGOComponent(new GridPosition(gridPos.x, gridPos.y));
    dragon->AddGOComponent(new ActionPoints(dragon->maxActionPoints));
    dragon->AddGOComponent(new SpellSlots(dragon->spellSlots));
    dragon->AddGOComponent(new StatusEffects());
    dragon->AddGOComponent(new DamageCalculator(dragon->baseDefensePower, dragon->defenseDice));
    dragon->AddGOComponent(new TargetingSystem(dragon->attackRange));

    // Initialize state machine
    dragon->current_state = &dragon->state_waiting_turn;

    Engine::GetLogger().LogInfo("Dragon created at grid position " + std::to_string((int)gridPos.x) + ", " + std::to_string((int)gridPos.y));

    return dragon;
}

// ===== FIGHTER FACTORY =====
Fighter* CharacterFactory::CreateFighter(Math::vec2 gridPos) {
    // Load stats from JSON (line 147-170)
    CharacterData data = LoadCharacterData("Fighter");

    Fighter* fighter = new Fighter(gridPos);

    // Set base stats
    fighter->maxHP = data.maxHP;
    fighter->currentHP = fighter->maxHP;
    fighter->speed = data.speed;
    fighter->maxActionPoints = data.maxActionPoints;
    fighter->baseAttackPower = data.baseAttackPower;
    fighter->attackDice = data.attackDice;
    fighter->baseDefensePower = data.baseDefensePower;
    fighter->defenseDice = data.defenseDice;
    fighter->attackRange = data.attackRange;
    fighter->spellSlots = data.spellSlots;

    // Add components
    fighter->AddGOComponent(new CS230::Sprite("Assets/Fighter.spt", fighter));
    fighter->AddGOComponent(new CS230::RectCollision({-16, -16, 16, 16}, fighter));
    fighter->AddGOComponent(new GridPosition(gridPos.x, gridPos.y));
    fighter->AddGOComponent(new ActionPoints(fighter->maxActionPoints));
    fighter->AddGOComponent(new SpellSlots(fighter->spellSlots));
    fighter->AddGOComponent(new StatusEffects());
    fighter->AddGOComponent(new DamageCalculator(fighter->baseDefensePower, fighter->defenseDice));
    fighter->AddGOComponent(new TargetingSystem(fighter->attackRange));
    fighter->AddGOComponent(new AIMemory());  // AI-specific component

    // Initialize AI state
    fighter->current_state = &fighter->state_waiting_turn;

    Engine::GetLogger().LogInfo("Fighter created at grid position " + std::to_string((int)gridPos.x) + ", " + std::to_string((int)gridPos.y));

    return fighter;
}

// ===== CLERIC FACTORY =====
Cleric* CharacterFactory::CreateCleric(Math::vec2 gridPos) {
    CharacterData data = LoadCharacterData("Cleric");

    Cleric* cleric = new Cleric(gridPos);

    // Set base stats
    cleric->maxHP = data.maxHP;
    cleric->currentHP = cleric->maxHP;
    cleric->speed = data.speed;
    cleric->maxActionPoints = data.maxActionPoints;
    cleric->baseAttackPower = data.baseAttackPower;
    cleric->attackDice = data.attackDice;
    cleric->baseDefensePower = data.baseDefensePower;
    cleric->defenseDice = data.defenseDice;
    cleric->attackRange = data.attackRange;
    cleric->spellSlots = data.spellSlots;

    // Add components
    cleric->AddGOComponent(new CS230::Sprite("Assets/Cleric.spt", cleric));
    cleric->AddGOComponent(new CS230::RectCollision({-16, -16, 16, 16}, cleric));
    cleric->AddGOComponent(new GridPosition(gridPos.x, gridPos.y));
    cleric->AddGOComponent(new ActionPoints(cleric->maxActionPoints));
    cleric->AddGOComponent(new SpellSlots(cleric->spellSlots));
    cleric->AddGOComponent(new StatusEffects());
    cleric->AddGOComponent(new DamageCalculator(cleric->baseDefensePower, cleric->defenseDice));
    cleric->AddGOComponent(new TargetingSystem(cleric->attackRange));
    cleric->AddGOComponent(new AIMemory());  // AI-specific component

    // Initialize AI state
    cleric->current_state = &cleric->state_waiting_turn;

    Engine::GetLogger().LogInfo("Cleric created at grid position " + std::to_string((int)gridPos.x) + ", " + std::to_string((int)gridPos.y));

    return cleric;
}

// ===== WIZARD FACTORY =====
Wizard* CharacterFactory::CreateWizard(Math::vec2 gridPos) {
    CharacterData data = LoadCharacterData("Wizard");

    Wizard* wizard = new Wizard(gridPos);

    // Set base stats
    wizard->maxHP = data.maxHP;
    wizard->currentHP = wizard->maxHP;
    wizard->speed = data.speed;
    wizard->maxActionPoints = data.maxActionPoints;
    wizard->baseAttackPower = data.baseAttackPower;
    wizard->attackDice = data.attackDice;
    wizard->baseDefensePower = data.baseDefensePower;
    wizard->defenseDice = data.defenseDice;
    wizard->attackRange = data.attackRange;
    wizard->spellSlots = data.spellSlots;

    // Add components
    wizard->AddGOComponent(new CS230::Sprite("Assets/Wizard.spt", wizard));
    wizard->AddGOComponent(new CS230::RectCollision({-16, -16, 16, 16}, wizard));
    wizard->AddGOComponent(new GridPosition(gridPos.x, gridPos.y));
    wizard->AddGOComponent(new ActionPoints(wizard->maxActionPoints));
    wizard->AddGOComponent(new SpellSlots(wizard->spellSlots));
    wizard->AddGOComponent(new StatusEffects());
    wizard->AddGOComponent(new DamageCalculator(wizard->baseDefensePower, wizard->defenseDice));
    wizard->AddGOComponent(new TargetingSystem(wizard->attackRange));
    wizard->AddGOComponent(new AIMemory());  // AI-specific component

    // Initialize AI state
    wizard->current_state = &wizard->state_waiting_turn;

    Engine::GetLogger().LogInfo("Wizard created at grid position " + std::to_string((int)gridPos.x) + ", " + std::to_string((int)gridPos.y));

    return wizard;
}

// ===== ROGUE FACTORY =====
Rogue* CharacterFactory::CreateRogue(Math::vec2 gridPos) {
    CharacterData data = LoadCharacterData("Rogue");

    Rogue* rogue = new Rogue(gridPos);

    // Set base stats
    rogue->maxHP = data.maxHP;
    rogue->currentHP = rogue->maxHP;
    rogue->speed = data.speed;
    rogue->maxActionPoints = data.maxActionPoints;
    rogue->baseAttackPower = data.baseAttackPower;
    rogue->attackDice = data.attackDice;
    rogue->baseDefensePower = data.baseDefensePower;
    rogue->defenseDice = data.defenseDice;
    rogue->attackRange = data.attackRange;
    rogue->spellSlots = data.spellSlots;

    // Add components
    rogue->AddGOComponent(new CS230::Sprite("Assets/Rogue.spt", rogue));
    rogue->AddGOComponent(new CS230::RectCollision({-16, -16, 16, 16}, rogue));
    rogue->AddGOComponent(new GridPosition(gridPos.x, gridPos.y));
    rogue->AddGOComponent(new ActionPoints(rogue->maxActionPoints));
    rogue->AddGOComponent(new SpellSlots(rogue->spellSlots));
    rogue->AddGOComponent(new StatusEffects());
    rogue->AddGOComponent(new DamageCalculator(rogue->baseDefensePower, rogue->defenseDice));
    rogue->AddGOComponent(new TargetingSystem(rogue->attackRange));
    rogue->AddGOComponent(new AIMemory());  // AI-specific component

    // Initialize AI state
    rogue->current_state = &rogue->state_waiting_turn;

    Engine::GetLogger().LogInfo("Rogue created at grid position " + std::to_string((int)gridPos.x) + ", " + std::to_string((int)gridPos.y));

    return rogue;
}

// ===== DATA LOADING HELPER =====
CharacterFactory::CharacterData CharacterFactory::LoadCharacterData(const std::string& characterName) {
    CharacterData data;
    auto& registry = DataRegistry::Instance();

    data.maxHP = registry.GetValue<int>(characterName + ".maxHP", 100);
    data.speed = registry.GetValue<int>(characterName + ".speed", 3);
    data.maxActionPoints = registry.GetValue<int>(characterName + ".maxActionPoints", 2);
    data.baseAttackPower = registry.GetValue<int>(characterName + ".baseAttackPower", 0);
    data.attackDice = registry.GetValue<std::string>(characterName + ".attackDice", "1d6");
    data.baseDefensePower = registry.GetValue<int>(characterName + ".baseDefensePower", 0);
    data.defenseDice = registry.GetValue<std::string>(characterName + ".defenseDice", "1d6");
    data.attackRange = registry.GetValue<int>(characterName + ".attackRange", 1);

    // Load spell slots (if any)
    if (registry.HasKey(characterName + ".spellSlots")) {
        auto slotsJSON = registry.GetJSON(characterName + ".spellSlots");
        for (auto& [level, count] : slotsJSON.items()) {
            data.spellSlots[std::stoi(level)] = count.get<int>();
        }
    }

    return data;
}
```

### ===== USAGE EXAMPLE IN BATTLESTATE =====

```cpp
void BattleState::Load() {
    AddGSComponent(new BattleManager());
    AddGSComponent(new TurnManager());
    AddGSComponent(new GridSystem());
    AddGSComponent(new CombatSystem());
    AddGSComponent(new SpellSystem());
    AddGSComponent(new AISystem());
    AddGSComponent(new StatusEffectManager());
    AddGSComponent(new GameObjectManager());

    // Create all characters using factory
    Character* dragon = CharacterFactory::CreateCharacter(CharacterType::Dragon, {4, 4});
    Character* fighter = CharacterFactory::CreateCharacter(CharacterType::Fighter, {0, 0});
    Character* cleric = CharacterFactory::CreateCharacter(CharacterType::Cleric, {0, 7});
    Character* wizard = CharacterFactory::CreateCharacter(CharacterType::Wizard, {7, 7});
    Character* rogue = CharacterFactory::CreateCharacter(CharacterType::Rogue, {7, 0});

    // Add to game object manager (transfers ownership)
    auto* objMgr = GetGSComponent<GameObjectManager>();
    objMgr->Add(dragon);
    objMgr->Add(fighter);
    objMgr->Add(cleric);
    objMgr->Add(wizard);
    objMgr->Add(rogue);

    // Initialize battle
    std::vector<Character*> allCombatants = {dragon, fighter, cleric, wizard, rogue};
    GetGSComponent<BattleManager>()->StartBattle(allCombatants);
}
```

---

## Summary

This document provides **complete technical specifications** for Dragonic Tactics Factory Pattern:

✅ **Pure GameObject Inheritance** - NO interfaces, follows CS230 Cat.h pattern
✅ **CharacterFactory** - Centralized creation with data-driven stats from DataRegistry
✅ **Component Attachment** - All characters get required components (GridPosition, ActionPoints, etc.)
✅ **Memory Management** - Raw pointers for CS230 GameObjectManager compatibility

**Every implementation follows CS230 engine patterns established in existing codebase!**
