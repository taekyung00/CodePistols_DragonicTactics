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
