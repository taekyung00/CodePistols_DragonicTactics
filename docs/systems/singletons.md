## 2. Singleton Services - Detailed Descriptions

**REASONING: Singleton Pattern from CS230 Engine**

- All CS230 engine services use singleton pattern (Engine::GetLogger(), Engine::GetInput(), etc.)
- Singletons provide global access without passing pointers everywhere
- Thread-safe lazy initialization on first access
- Single instance ensures consistent state across all systems

### 2.1 DiceManager Singleton

**Purpose:** Centralized random number generation for all dice rolls
**Reasoning from dragonic_tactics.md:**

- Line 23-24: Attack uses dice notation (3d6, 2d8, etc.)
- Line 24-26: Defense uses dice rolls
- Centralized RNG enables seeding for replays, logging for debugging

```cpp
// DiceManager.h - Singleton for dice roll management
// REASONING: Centralized RNG ensures consistent randomness across all systems

#pragma once
#include <random>
#include <string>

class DiceManager {
public:
    // ===== Singleton Access =====
    static DiceManager& Instance() {
        static DiceManager instance;  // Thread-safe lazy initialization (C++11)
        return instance;
    }

    // Delete copy/move constructors (singleton must have single instance)
    DiceManager(const DiceManager&) = delete;
    DiceManager& operator=(const DiceManager&) = delete;

    // ===== Core Dice Rolling Methods =====

    int RollDice(int count, int sides);
    // Roll 'count' dice with 'sides' faces each, return sum
    // REASONING: Line 23 - "3D6 (6면체 주사위를 3번 던져 나온 값의 합)"
    // USAGE: RollDice(3, 6) for Dragon's 3d6 attack
    // IMPLEMENTATION:
    //   result = 0
    //   for i in 0..count: result += (rand() % sides) + 1
    //   return result

    int RollDiceFromString(const std::string& diceNotation);
    // Parse dice string (e.g., "3d6", "2d8") and roll
    // REASONING: Data-driven design - dice stored as strings in JSON
    // USAGE: RollDiceFromString("3d6") -> calls RollDice(3, 6)
    // IMPLEMENTATION:
    //   Parse "XdY" format: X = count, Y = sides
    //   Return RollDice(count, sides)

    void SetSeed(unsigned int seed);
    // Set RNG seed for deterministic results (testing/replays)
    // REASONING: Enables replay system and unit testing
    // USAGE: SetSeed(12345) before battle for reproducible results

    int GetLastRoll() const { return lastRoll; }
    // Return the last dice roll result for UI display
    // REASONING: UI can show "Rolled 14 (3d6)" for player feedback
    // USAGE: CombatSystem displays dice results in combat log

private:
    DiceManager();  // Private constructor (singleton)

    std::mt19937 rng;  // Mersenne Twister RNG
    int lastRoll;      // Cache last roll for UI
};
```

**USAGE EXAMPLE:**

```cpp
// In CombatSystem.cpp - Calculate dragon attack damage
int Dragon::GetAttackPower() const {
    // Dragon has base attack 0, variable 3d6 (line 123)
    int diceRoll = DiceManager::Instance().RollDiceFromString("3d6");
    return baseAttackPower + diceRoll;  // e.g., 0 + 14 = 14 damage
}

// In testing - Set deterministic seed for replay
void TestBattle::SetupBattle() {
    DiceManager::Instance().SetSeed(42);  // Reproducible results
    // Now all dice rolls are deterministic for this battle
}
```

### 2.2 EventBus Singleton

**Purpose:** Decoupled inter-system communication without direct dependencies
**Reasoning:** Modular architecture principle #2 - "Glue System" for loose coupling

```cpp
// EventBus.h - Pub/sub event system for decoupled communication
// REASONING: Prevents tight coupling between independent modules

#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

class EventBus {
public:
    // ===== Singleton Access =====
    static EventBus& Instance() {
        static EventBus instance;
        return instance;
    }

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    // ===== Event Subscription =====

    template<typename EventType>
    void Subscribe(std::function<void(const EventType&)> handler);
    // Register handler function to be called when EventType published
    // REASONING: Systems subscribe to events they care about without knowing publishers
    // USAGE: EventBus::Instance().Subscribe<CharacterDiedEvent>([](const auto& e) { ... });
    // EXAMPLE:
    //   BattleManager subscribes to CharacterDiedEvent to check victory conditions
    //   AISystem subscribes to CharacterDiedEvent to update threat assessments
    //   EffectManager subscribes to SpellCastEvent to play visual effects

    template<typename EventType>
    void Publish(const EventType& event);
    // Trigger event, calling all subscribed handlers
    // REASONING: Publishers don't need to know who's listening
    // USAGE: EventBus::Instance().Publish(CharacterDiedEvent{deadCharacter});
    // EXAMPLE:
    //   CombatSystem publishes DamageTakenEvent when character hit
    //   TurnManager publishes TurnStartedEvent when turn begins
    //   SpellSystem publishes SpellCastEvent when spell used

    // ===== Event Unsubscription =====

    template<typename EventType>
    void Unsubscribe(/* subscription handle */);
    // Remove handler subscription (for cleanup)
    // REASONING: Prevent calling handlers on deleted objects
    // USAGE: Unsubscribe when GameState unloads

private:
    EventBus() = default;

    // Map: event type -> list of handler functions
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<void>>> subscribers;
};
```

**COMMON EVENT TYPES:**

```cpp
// Events.h - Event data structures

// Combat Events
struct DamageTakenEvent {
    Character* target;
    Character* source;
    int damageAmount;
    bool wasCritical;
};

struct CharacterDiedEvent {
    Character* deadCharacter;
    Character* killer;
};

// Turn Events
struct TurnStartedEvent {
    Character* activeCharacter;
    int turnNumber;
};

struct TurnEndedEvent {
    Character* character;
    int actionsUsed;
};

// Spell Events
struct SpellCastEvent {
    Character* caster;
    SpellType spell;
    Math::vec2 target;
    int spellLevel;
};

// Battle Events
struct BattleStartedEvent {
    std::vector<Character*> allCombatants;
};

struct BattleEndedEvent {
    bool playerWon;
    int turnsElapsed;
};
```

**USAGE EXAMPLE:**

```cpp
// In BattleManager.cpp - Subscribe to death events for victory checking
void BattleManager::Load() {
    EventBus::Instance().Subscribe<CharacterDiedEvent>(
        [this](const CharacterDiedEvent& event) {
            this->OnCharacterDied(event);
        }
    );
}

void BattleManager::OnCharacterDied(const CharacterDiedEvent& event) {
    if (event.deadCharacter->Type() == GameObjectTypes::Dragon) {
        // Player dragon died - game over
        EndBattle(false);
    } else {
        // Check if all enemies dead
        if (AreAllEnemiesDefeated()) {
            EndBattle(true);
        }
    }
}

// In CombatSystem.cpp - Publish damage event when character hit
void CombatSystem::ApplyDamage(Character* target, int damage) {
    target->TakeDamage(damage);

    // Notify all subscribers
    EventBus::Instance().Publish(DamageTakenEvent{
        .target = target,
        .source = currentAttacker,
        .damageAmount = damage,
        .wasCritical = false
    });

    if (!target->IsAlive()) {
        EventBus::Instance().Publish(CharacterDiedEvent{
            .deadCharacter = target,
            .killer = currentAttacker
        });
    }
}
```

### 2.3 DataRegistry Singleton

**Purpose:** Load and manage JSON configuration data for data-driven design
**Reasoning:** Architecture principle #3 - Separate code from data for easy balancing

```cpp
// DataRegistry.h - JSON data loading and hot-reload management
// REASONING: Data-driven design allows balance changes without recompiling

#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>  // JSON library

class DataRegistry {
public:
    // ===== Singleton Access =====
    static DataRegistry& Instance() {
        static DataRegistry instance;
        return instance;
    }

    DataRegistry(const DataRegistry&) = delete;
    DataRegistry& operator=(const DataRegistry&) = delete;

    // ===== Data Loading =====

    void LoadFromFile(const std::string& filepath);
    // Load JSON file and merge into registry
    // REASONING: Load character stats, spell data, balance tables at startup
    // USAGE: DataRegistry::Instance().LoadFromFile("Data/CharacterStats.json");
    // EXAMPLE FILE STRUCTURE:
    //   {
    //     "Dragon": {
    //       "maxHP": 140,
    //       "speed": 5,
    //       "attackDice": "3d6",
    //       "spellSlots": {"1": 4, "2": 3, "3": 2, "4": 2, "5": 1}
    //     },
    //     "Fighter": { ... }
    //   }

    void ReloadFile(const std::string& filepath);
    // Hot-reload file at runtime for live balance tuning
    // REASONING: Designers can tweak values without restarting game
    // USAGE: Press F5 to reload balance data during playtesting

    // ===== Data Access =====

    template<typename T>
    T GetValue(const std::string& key, const T& defaultValue = T()) const;
    // Retrieve typed value from registry by key path
    // REASONING: Type-safe access with default fallback
    // USAGE: int hp = GetValue<int>("Dragon.maxHP", 100);
    // KEY PATH FORMAT: "CharacterType.StatName" (dot-separated)

    nlohmann::json GetJSON(const std::string& key) const;
    // Get raw JSON object for complex nested data
    // REASONING: Some data structures don't fit simple types
    // USAGE: auto slots = GetJSON("Dragon.spellSlots");

    bool HasKey(const std::string& key) const;
    // Check if key exists in registry
    // REASONING: Validate data before attempting access
    // USAGE: if (HasKey("Rogue.stealthDuration")) { ... }

private:
    DataRegistry() = default;

    nlohmann::json data;  // Complete JSON registry

    // Helper: Navigate nested JSON by dot-separated path
    nlohmann::json GetNestedValue(const std::string& keyPath) const;
};
```

**JSON FILE EXAMPLE:**

```json
// Data/CharacterStats.json
{
  "Dragon": {
    "maxHP": 140,
    "speed": 5,
    "maxActionPoints": 2,
    "baseAttackPower": 0,
    "attackDice": "3d6",
    "baseDefensePower": 0,
    "defenseDice": "2d8",
    "attackRange": 3,
    "spellSlots": {
      "1": 4,
      "2": 3,
      "3": 2,
      "4": 2,
      "5": 1
    }
  },
  "Fighter": {
    "maxHP": 90,
    "speed": 3,
    "maxActionPoints": 2,
    "baseAttackPower": 5,
    "attackDice": "2d6",
    "baseDefensePower": 0,
    "defenseDice": "1d10",
    "attackRange": 1,
    "spellSlots": {
      "1": 2,
      "2": 2
    }
  }
}
```

**USAGE EXAMPLE:**

```cpp
// In CharacterFactory.cpp - Load stats from JSON
std::unique_ptr<Dragon> CharacterFactory::CreateDragon() {
    auto dragon = std::make_unique<Dragon>(Math::vec2{0, 0});

    // Load all stats from DataRegistry
    dragon->maxHP = DataRegistry::Instance().GetValue<int>("Dragon.maxHP", 100);
    dragon->currentHP = dragon->maxHP;
    dragon->speed = DataRegistry::Instance().GetValue<int>("Dragon.speed", 5);
    dragon->maxActionPoints = DataRegistry::Instance().GetValue<int>("Dragon.maxActionPoints", 2);
    dragon->attackDice = DataRegistry::Instance().GetValue<std::string>("Dragon.attackDice", "3d6");
    dragon->defenseDice = DataRegistry::Instance().GetValue<std::string>("Dragon.defenseDice", "2d8");

    // Load spell slots
    auto slotsJSON = DataRegistry::Instance().GetJSON("Dragon.spellSlots");
    for (auto& [level, count] : slotsJSON.items()) {
        dragon->spellSlots[std::stoi(level)] = count.get<int>();
    }

    return dragon;
}

// During playtesting - Hot reload balance changes
if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::F5)) {
    DataRegistry::Instance().ReloadFile("Data/CharacterStats.json");
    Engine::GetLogger().LogDebug("Balance data reloaded!");
}
```

### 2.4 AIDirector Singleton

**Purpose:** Global AI coordination and difficulty scaling
**Reasoning from dragonic_tactics.md:**

- Line 75-86: Four AI characters with distinct roles must coordinate
- Line 287: "위험의 정의" - AI evaluates threat level for decision-making

```cpp
// AIDirector.h - Centralized AI coordination and difficulty management
// REASONING: Individual AI agents coordinate for team tactics

#pragma once
#include <vector>
#include "Character.h"

enum class DifficultyLevel {
    Easy,      // AI makes suboptimal moves, doesn't focus fire
    Normal,    // AI coordinates basic tactics, prioritizes threats
    Hard,      // AI uses advanced tactics, adapts to player strategy
    Nightmare  // AI perfect play, remembers player patterns
};

class AIDirector {
public:
    // ===== Singleton Access =====
    static AIDirector& Instance() {
        static AIDirector instance;
        return instance;
    }

    AIDirector(const AIDirector&) = delete;
    AIDirector& operator=(const AIDirector&) = delete;

    // ===== Difficulty Management =====

    void SetDifficultyLevel(DifficultyLevel level);
    // Adjust AI behavior based on difficulty setting
    // REASONING: Accessibility - let players choose challenge level
    // USAGE: AIDirector::Instance().SetDifficultyLevel(DifficultyLevel::Hard);
    // EFFECTS:
    //   Easy: AI attacks randomly, doesn't heal optimally
    //   Normal: AI prioritizes dragon, heals at 50% HP
    //   Hard: AI focus fires, protects Wizard, heals at 70% HP
    //   Nightmare: AI predicts player moves, uses advanced combos

    DifficultyLevel GetDifficultyLevel() const { return currentDifficulty; }

    // ===== Team Coordination =====

    struct TeamDecision {
        Character* actor;         // Who should act
        ActionType action;        // What action to take
        Math::vec2 target;        // Where to target
        int priority;             // Action urgency (1-10)
    };

    std::vector<TeamDecision> GetTeamDecisions(const std::vector<Character*>& aiTeam);
    // Coordinate all AI characters for optimal team play
    // REASONING: AI agents must work together (Fighter tanks, Cleric heals, etc.)
    // USAGE: AISystem calls this at turn start to plan all AI actions
    // IMPLEMENTATION:
    //   1. Evaluate battlefield state (positions, HP, threats)
    //   2. Assign roles: Fighter -> engage, Wizard -> damage, Cleric -> heal, Rogue -> flank
    //   3. Prioritize actions: Emergency heals > positioning > attacks
    //   4. Return sorted list of recommended actions

    // ===== Threat Assessment =====

    int EvaluateThreat(const Character* character) const;
    // Calculate how threatening a character is to AI team
    // REASONING: Line 287 - AI must evaluate danger level for targeting
    // USAGE: int threat = AIDirector::Instance().EvaluateThreat(dragon);
    // FORMULA:
    //   threat = (currentHP / maxHP) * 100 * attackPower / 10
    //   Examples:
    //     Dragon at 100% HP, 15 attack = 100 * 1.5 = 150 threat
    //     Dragon at 50% HP, 15 attack = 50 * 1.5 = 75 threat

    Character* GetHighestThreatTarget(const std::vector<Character*>& targets) const;
    // Find most dangerous target from list
    // REASONING: Focus fire on biggest threat
    // USAGE: auto target = GetHighestThreatTarget(allEnemies);

    // ===== Adaptive Learning =====

    void RecordPlayerAction(ActionType action, bool wasEffective);
    // Learn from player behavior to adapt strategy
    // REASONING: Higher difficulties adapt to player patterns
    // USAGE: AIDirector::Instance().RecordPlayerAction(ActionType::Fireball, true);
    // IMPLEMENTATION:
    //   Track player preferences (spell usage frequency, positioning)
    //   Adjust AI counters (spread out vs AOE, focus heal targets)

private:
    AIDirector() : currentDifficulty(DifficultyLevel::Normal) {}

    DifficultyLevel currentDifficulty;

    // AI Memory
    std::unordered_map<ActionType, int> playerActionFrequency;
    std::vector<Math::vec2> playerPreferredPositions;
};
```

**USAGE EXAMPLE:**

```cpp
// In AISystem.cpp - Coordinate team tactics
void AISystem::UpdateAI(double dt) {
    // Get all AI characters that need decisions
    std::vector<Character*> aiTeam;
    for (auto* character : allCharacters) {
        if (character->Type() != GameObjectTypes::Dragon && character->IsAlive()) {
            aiTeam.push_back(character);
        }
    }

    // Get coordinated team decisions
    auto decisions = AIDirector::Instance().GetTeamDecisions(aiTeam);

    // Execute highest priority actions
    for (const auto& decision : decisions) {
        if (decision.priority >= 7) {  // Urgent actions only
            decision.actor->PerformAction(decision.action);
            Engine::GetLogger().LogDebug(
                decision.actor->TypeName() + " performs " +
                ActionTypeToString(decision.action)
            );
        }
    }
}

// In Fighter AI - Query threat assessment
void Fighter::UpdateAI(double dt) {
    // Find dragon (main threat)
    Dragon* dragon = FindDragonOnBattlefield();

    // Evaluate if dragon is dangerous enough to prioritize
    int dragonThreat = AIDirector::Instance().EvaluateThreat(dragon);

    if (dragonThreat > 100) {
        // Dragon very threatening - must engage immediately
        change_state(&state_engaging);
    } else {
        // Dragon weakened - protect allies instead
        change_state(&state_tanking);
    }
}
```

### 2.5 EffectManager Singleton

**Purpose:** Visual effects and particle system coordination
**Reasoning:** Separate visual presentation from game logic

```cpp
// EffectManager.h - Visual effect and particle coordination
// REASONING: Decouple rendering from game logic for clean architecture

#pragma once
#include "SpellTypes.h"
#include "../Engine/Vec2.h"
#include "../Engine/Particle.h"

class EffectManager {
public:
    // ===== Singleton Access =====
    static EffectManager& Instance() {
        static EffectManager instance;
        return instance;
    }

    EffectManager(const EffectManager&) = delete;
    EffectManager& operator=(const EffectManager&) = delete;

    // ===== Spell Visual Effects =====

    void PlaySpellEffect(SpellType spell, Math::vec2 position, Math::vec2 target);
    // Trigger visual effect for spell casting
    // REASONING: Spells need visual feedback (fireballs, explosions, heal sparkles)
    // USAGE: EffectManager::Instance().PlaySpellEffect(SpellType::Fireball, casterPos, targetPos);
    // IMPLEMENTATION:
    //   Switch on spell type, spawn appropriate particles/animations
    //   Fireball: Orange projectile trail + explosion
    //   Heal: Green sparkles rising up
    //   Dragon Roar: Shockwave expanding from dragon

    void PlayAttackEffect(Character* attacker, Character* target);
    // Visual feedback for melee/ranged attacks
    // REASONING: Player needs to see attacks connecting
    // USAGE: EffectManager::Instance().PlayAttackEffect(fighter, dragon);
    // EFFECTS:
    //   Melee: Sword slash sprite, impact flash
    //   Ranged: Arrow/spell bolt projectile

    void PlayStatusEffect(Character* target, EffectType effect);
    // Continuous visual for status effects
    // REASONING: Show buffs/debuffs on characters
    // USAGE: PlayStatusEffect(dragon, EffectType::Burn);
    // VISUALS:
    //   Burn: Orange flames around character
    //   Fear: Shaking animation
    //   Blessed: Golden glow

    // ===== Particle System =====

    void EmitParticles(const std::string& particleType, Math::vec2 position, int count);
    // Spawn particle burst at location
    // REASONING: Generic particle spawning for various effects
    // USAGE: EmitParticles("smoke", explosionPos, 20);

    void UpdateAllEffects(double dt);
    // Update all active visual effects
    // REASONING: Animate particles, update effect lifetimes
    // USAGE: Called by GameState every frame

private:
    EffectManager() = default;

    std::vector<VisualEffect*> activeEffects;  // All playing effects
};
```

**USAGE EXAMPLE:**

```cpp
// In SpellSystem.cpp - Play fireball effect
void SpellSystem::CastFireball(Character* caster, Math::vec2 target) {
    // Visual effect
    EffectManager::Instance().PlaySpellEffect(
        SpellType::Fireball,
        caster->GetPosition(),
        target
    );

    // Wait for projectile to reach target (async)
    // Then apply damage

    // Explosion visual
    EffectManager::Instance().EmitParticles("fire", target, 30);
}

// In StatusEffectManager.cpp - Show burn status
void StatusEffectManager::ApplyBurnEffect(Character* target) {
    target->AddStatusEffect(StatusEffect::Burn, 5.0);  // 5 turn duration

    // Continuous visual
    EffectManager::Instance().PlayStatusEffect(target, EffectType::Burn);
}
```

### 2.6 SaveManager Singleton

**Purpose:** Campaign persistence and state serialization
**Reasoning:** Support multi-battle campaign progression

```cpp
// SaveManager.h - Save/load system for campaign progression
// REASONING: Players need to save progress between battles

#pragma once
#include <string>
#include "Character.h"

struct CampaignData {
    int currentLevel;
    int playerScore;

    // Dragon persistent state
    int dragonCurrentHP;
    std::map<int, int> dragonSpellSlots;  // Spell slots carry between battles

    // Unlocked abilities
    std::vector<SpellType> unlockedSpells;

    // Campaign progress
    std::vector<bool> levelsCompleted;
    double totalPlayTime;
};

class SaveManager {
public:
    // ===== Singleton Access =====
    static SaveManager& Instance() {
        static SaveManager instance;
        return instance;
    }

    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    // ===== Save/Load Operations =====

    void SaveCampaignState(const CampaignData& data, const std::string& saveName);
    // Serialize campaign state to disk
    // REASONING: Preserve progress between game sessions
    // USAGE: SaveManager::Instance().SaveCampaignState(campaignData, "slot1");
    // FORMAT: JSON file in Saves/ directory

    CampaignData LoadCampaignState(const std::string& saveName);
    // Deserialize campaign state from disk
    // REASONING: Restore progress on game start
    // USAGE: auto data = LoadCampaignState("slot1");

    bool SaveFileExists(const std::string& saveName) const;
    // Check if save file exists
    // REASONING: Show "Continue" option in main menu
    // USAGE: if (SaveFileExists("slot1")) { ShowContinueButton(); }

    std::vector<std::string> GetAllSaveFiles() const;
    // List all available save files
    // REASONING: Display save file selection menu
    // USAGE: auto saves = GetAllSaveFiles();

    void DeleteSaveFile(const std::string& saveName);
    // Remove save file from disk
    // REASONING: Let players delete old saves
    // USAGE: DeleteSaveFile("slot1");

private:
    SaveManager() = default;

    std::string GetSavePath(const std::string& saveName) const;
    // Returns: "Saves/slot1.json"
};
```

**USAGE EXAMPLE:**

```cpp
// In CampaignState.cpp - Save after battle victory
void CampaignState::OnBattleWon() {
    CampaignData data;
    data.currentLevel = currentLevel + 1;
    data.playerScore = totalScore;

    // Save dragon persistent state
    Dragon* dragon = GetPlayerDragon();
    data.dragonCurrentHP = dragon->GetCurrentHP();
    data.dragonSpellSlots = dragon->spellSlots;  // Spell slots DON'T regenerate!

    // Save progress
    data.levelsCompleted.push_back(true);
    data.totalPlayTime = GetTotalPlayTime();

    SaveManager::Instance().SaveCampaignState(data, "campaign_autosave");
    Engine::GetLogger().LogInfo("Progress saved!");
}

// In MainMenu.cpp - Show continue option
void MainMenu::Load() {
    if (SaveManager::Instance().SaveFileExists("campaign_autosave")) {
        // Show "Continue Campaign" button
        AddMenuButton("Continue Campaign", []() {
            auto data = SaveManager::Instance().LoadCampaignState("campaign_autosave");
            StartCampaignFromData(data);
        });
    }

    AddMenuButton("New Campaign", []() {
        // Confirm overwrite if save exists
        StartNewCampaign();
    });
}
```

---
