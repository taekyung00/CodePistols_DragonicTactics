<parameter name="todos">[{"content": "Complete GridSystem, CombatSystem, SpellSystem GSComponents documentation", "status": "completed", "activeForm": "Documenting core battle system GSComponents"}, {"content": "Complete AISystem and StatusEffectManager GSComponents documentation", "status": "in_progress", "activeForm": "Documenting AI and status effect GSComponents"}, {"content": "Document all 7 GameObject Components with full method descriptions", "status": "pending", "activeForm": "Documenting GameObject Components"}, {"content": "Complete Interface Design with full method documentation", "status": "pending", "activeForm": "Documenting all interface methods"}, {"content": "Create complete Factory Pattern usage examples for all character types", "status": "pending", "activeForm": "Creating factory pattern examples"}]

### 3.6 AISystem GSComponent

**Purpose:** AI behavior coordination and decision making
**Reasoning from dragonic_tactics.md:**

- Line 75-86: Four AI characters with distinct tactical roles
- Line 80-81: Fighter - engage dragon, manage aggro
- Line 83-85: Cleric - heal allies, extend battles
- Line 82-83: Wizard - long range, high damage, fragile
- Line 85-86: Rogue - flank, stealth, ambush

```cpp
// AISystem.h - GSComponent for AI coordination
// REASONING: Manage all AI character decisions and behaviors

#pragma once
#include "../Engine/Component.h"
#include "Character.h"
#include <vector>

struct AIDecision {
    ActionType action;      // What to do
    Math::vec2 target;      // Where to do it
    int priority;           // Urgency (1-10)
    std::string reasoning;  // Debug info
};

class AISystem : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Update all AI characters' decisions
    // REASONING: Process AI turns when TurnManager activates them
    // IMPLEMENTATION:
    //   1. Check if current character is AI
    //   2. If yes, call MakeDecision()
    //   3. Execute highest priority action
    //   4. Continue until no action points remain

    // ===== AI Decision Making =====

    AIDecision MakeDecision(Character* aiCharacter);
    // Generate best action for AI character
    // REASONING: Each AI type has different priorities (line 75-86)
    // IMPLEMENTATION:
    //   Switch on character type:
    //     Fighter: Prioritize engaging dragon
    //     Cleric: Prioritize healing wounded allies
    //     Wizard: Prioritize staying at safe distance
    //     Rogue: Prioritize flanking for ambush
    //   Return highest priority valid action
    // USAGE: auto decision = aiSys->MakeDecision(fighter);

    std::vector<AIDecision> EvaluateAllActions(Character* aiCharacter);
    // Get all possible actions with priority scores
    // REASONING: AI needs to compare all options
    // IMPLEMENTATION:
    //   1. Get available actions (move, attack, spell, heal)
    //   2. Score each based on situation
    //   3. Return sorted by priority
    // USAGE: auto options = EvaluateAllActions(cleric);

    // ===== Role-Specific AI =====

    AIDecision FighterAI(Fighter* fighter);
    // Tank role logic
    // REASONING: Line 80-81 - "드래곤에게 접근해서 어그로 담당"
    // PRIORITY LIST:
    //   1. Move toward dragon (if not in range)
    //   2. Attack dragon (if in melee range)
    //   3. Block dragon's path to allies
    //   4. Tank hits to protect backline
    // IMPLEMENTATION:
    //   dragon_dist = Distance(fighter, dragon)
    //   if dragon_dist > 1:
    //     return MoveTo(dragon)  // Close distance
    //   else:
    //     return Attack(dragon)  // Engage

    AIDecision ClericAI(Cleric* cleric);
    // Support role logic
    // REASONING: Line 83-85 - "동료 모험가 치유, 전투를 길게 끌수록 위협적"
    // PRIORITY LIST:
    //   1. Heal critically wounded ally (HP < 30%)
    //   2. Heal moderately wounded ally (HP < 60%)
    //   3. Position safely behind fighter
    //   4. Attack dragon if all allies healthy
    // IMPLEMENTATION:
    //   wounded = FindMostWoundedAlly()
    //   if wounded.HP < wounded.maxHP * 0.3:
    //     return Heal(wounded)  // Emergency heal
    //   else if wounded.HP < wounded.maxHP * 0.6:
    //     return Heal(wounded)  // Preventive heal
    //   else:
    //     return Attack(dragon) // No healing needed

    AIDecision WizardAI(Wizard* wizard);
    // Glass cannon logic
    // REASONING: Line 82-83 - "낮은 체력과 방어력, 넓은 공격 범위와 높은 데미지, 원거리에서 공격"
    // PRIORITY LIST:
    //   1. RETREAT if dragon within 3 tiles (too close!)
    //   2. Attack dragon if at 4-6 tile range (optimal)
    //   3. Advance toward dragon if beyond 6 tiles
    //   4. Hide behind fighter for protection
    // IMPLEMENTATION:
    //   dragon_dist = Distance(wizard, dragon)
    //   if dragon_dist < 4:
    //     return Retreat()      // Danger!
    //   else if dragon_dist <= 6:
    //     return Attack(dragon) // Safe range
    //   else:
    //     return Advance()      // Get in range

    AIDecision RogueAI(Rogue* rogue);
    // Assassin logic
    // REASONING: Line 85-86 - "높은 속력과 높은 공격력, 드래곤의 측면을 노려 기습 공격을 노림"
    // PRIORITY LIST:
    //   1. Ambush dragon from behind/side (8+2d6 damage!)
    //   2. Use stealth if low HP (< 30%)
    //   3. Sprint to flanking position
    //   4. Coordinate with fighter to sandwich dragon
    // IMPLEMENTATION:
    //   if IsBehindOrBeside(dragon):
    //     return Ambush(dragon)   // Bonus damage
    //   else if HP < maxHP * 0.3:
    //     return Stealth()        // Survive
    //   else:
    //     return MoveTo(dragonFlank) // Reposition

    // ===== Threat Assessment =====

    int CalculateThreatLevel(Character* target) const;
    // Determine how dangerous a character is
    // REASONING: Line 287 - "위험의 정의: 모험가의 체력이 모두 30프로 이하이면서 드래곤의 체력이 50프로 이상일 때"
    // FORMULA:
    //   threat = (currentHP / maxHP) * attackPower * speedModifier
    // EXAMPLES:
    //   Dragon at 100% HP, 15 avg attack: 100 * 15 * 1.0 = 1500
    //   Dragon at 50% HP, 15 avg attack: 50 * 15 * 1.0 = 750
    //   Fighter at 80% HP, 10 avg attack: 80 * 10 * 0.8 = 640
    // USAGE: int threat = CalculateThreatLevel(dragon);

    Character* FindHighestThreatEnemy() const;
    // Find most dangerous target
    // REASONING: AI should focus fire on biggest threat
    // IMPLEMENTATION: return max(CalculateThreatLevel(all enemies))
    // USAGE: auto* target = FindHighestThreatEnemy();  // Usually dragon

    // ===== Team Coordination =====

    void CoordinateTeamActions(const std::vector<Character*>& aiTeam);
    // Plan team strategy before executing actions
    // REASONING: AI characters should work together
    // IMPLEMENTATION:
    //   1. Fighter: Position to tank
    //   2. Cleric: Stay behind fighter
    //   3. Wizard: Position at max range
    //   4. Rogue: Flank opposite side from fighter
    // USAGE: CoordinateTeamActions({fighter, cleric, wizard, rogue});

    Math::vec2 FindOptimalPosition(Character* character) const;
    // Calculate best tile for character
    // REASONING: Positioning is key to tactical combat
    // IMPLEMENTATION: Based on character role and team formation
    // USAGE: auto pos = FindOptimalPosition(wizard);

private:
    // AI state tracking
    std::map<Character*, AIDecision> lastDecisions;
    std::map<Character*, int> decisionFailures;  // Track stuck AI
};
```

**USAGE EXAMPLE:**

```cpp
// In BattleState.cpp - Process AI turn
void BattleState::Update(double dt) {
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    Character* current = turnMgr->GetCurrentCharacter();

    // Check if AI character's turn
    if (current->Type() != GameObjectTypes::Dragon && current->IsAlive()) {
        AISystem* aiSys = GetGSComponent<AISystem>();

        // Get AI decision
        AIDecision decision = aiSys->MakeDecision(current);

        Engine::GetLogger().LogDebug(
            current->TypeName() + " decides: " + decision.reasoning
        );

        // Execute decision
        current->PerformAction(decision.action);

        // Check if turn complete
        if (current->GetActionPoints() == 0) {
            turnMgr->AdvanceToNextTurn();
        }
    }
}

// In AISystem.cpp - Cleric healing logic
AIDecision AISystem::ClericAI(Cleric* cleric) {
    // Find most wounded ally
    Character* mostWounded = nullptr;
    float lowestHPPercent = 1.0f;

    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    for (auto* ally : GetAllAllies()) {
        if (ally == cleric || !ally->IsAlive()) continue;

        float hpPercent = (float)ally->GetCurrentHP() / ally->GetMaxHP();
        if (hpPercent < lowestHPPercent) {
            lowestHPPercent = hpPercent;
            mostWounded = ally;
        }
    }

    // Emergency healing (< 30% HP)
    if (mostWounded && lowestHPPercent < 0.3f) {
        // Check if in heal range (2 tiles, line 201)
        int distance = grid->ManhattanDistance(
            cleric->GetGridPosition(),
            mostWounded->GetGridPosition()
        );

        if (distance <= 2) {
            return AIDecision{
                .action = ActionType::Heal,
                .target = mostWounded->GetGridPosition(),
                .priority = 10,  // CRITICAL
                .reasoning = "Emergency heal on " + mostWounded->TypeName()
            };
        } else {
            // Move closer to heal
            auto path = grid->FindPath(
                cleric->GetGridPosition(),
                mostWounded->GetGridPosition()
            );
            return AIDecision{
                .action = ActionType::Move,
                .target = path[std::min(cleric->GetSpeed(), (int)path.size() - 1)],
                .priority = 9,
                .reasoning = "Moving to heal " + mostWounded->TypeName()
            };
        }
    }

    // Preventive healing (< 60% HP)
    if (mostWounded && lowestHPPercent < 0.6f) {
        int distance = grid->ManhattanDistance(
            cleric->GetGridPosition(),
            mostWounded->GetGridPosition()
        );

        if (distance <= 2) {
            return AIDecision{
                .action = ActionType::Heal,
                .target = mostWounded->GetGridPosition(),
                .priority = 7,  // High but not critical
                .reasoning = "Preventive heal on " + mostWounded->TypeName()
            };
        }
    }

    // No healing needed - attack dragon
    Dragon* dragon = FindDragon();
    if (dragon) {
        return AIDecision{
            .action = ActionType::RangedAttack,
            .target = dragon->GetGridPosition(),
            .priority = 5,
            .reasoning = "All allies healthy, attacking dragon"
        };
    }

    // Default: skip turn
    return AIDecision{
        .action = ActionType::Wait,
        .target = cleric->GetGridPosition(),
        .priority = 1,
        .reasoning = "No valid targets"
    };
}
```

### 3.7 StatusEffectManager GSComponent

**Purpose:** Global status effect tracking and duration management
**Reasoning from dragonic_tactics.md:**

- Line 61-74: Six status effects (Curse, Fear, Bind, Blind, Blessing, Haste)
- Line 53: End phase applies status damage (burn, poison, etc.)

```cpp
// StatusEffectManager.h - GSComponent for status effect coordination
// REASONING: Track all active buffs/debuffs across battlefield

#pragma once
#include "../Engine/Component.h"
#include "Character.h"
#include <map>
#include <vector>

// Status effects from dragonic_tactics.md line 61-74
enum class StatusEffectType {
    Curse,      // Line 63: All incoming damage +, all outgoing damage -
    Fear,       // Line 65: Attack power -, Speed -
    Bind,       // Line 67: Cannot move
    Blind,      // Line 69: All ranges reduced
    Blessing,   // Line 71: All incoming damage -, all outgoing damage +
    Haste,      // Line 73: Speed +, Action points +
    Burn        // From lava spell (line 102): Damage over time
};

struct StatusEffect {
    StatusEffectType type;
    int turnsRemaining;     // Duration in turns
    int magnitude;          // Effect strength
    Character* source;      // Who applied it
};

class StatusEffectManager : public CS230::Component {
public:
    // ===== CS230::Component Interface =====

    void Update(double dt) override;
    // Process status effects each turn
    // REASONING: Line 53 - "종료 페이즈: 턴 종료시 발동하는 효과(독, 화상등) 처리"
    // IMPLEMENTATION:
    //   On end of round:
    //     1. Apply damage-over-time effects (burn)
    //     2. Decrement durations
    //     3. Remove expired effects
    //     4. Recalculate modified stats

    // ===== Effect Application =====

    void ApplyStatusEffect(Character* target, StatusEffectType type, int duration, int magnitude = 1);
    // Add status effect to character
    // REASONING: Spells and abilities apply buffs/debuffs
    // IMPLEMENTATION:
    //   1. Create StatusEffect struct
    //   2. Add to target's active effects
    //   3. Apply stat modifiers immediately
    //   4. Publish StatusEffectAppliedEvent
    // USAGE: statusMgr->ApplyStatusEffect(fighter, StatusEffectType::Fear, 3, 2);

    void RemoveStatusEffect(Character* target, StatusEffectType type);
    // Remove specific effect from character
    // REASONING: Some spells cure debuffs
    // IMPLEMENTATION:
    //   1. Find effect in target's list
    //   2. Remove stat modifiers
    //   3. Delete effect
    //   4. Publish StatusEffectRemovedEvent
    // USAGE: statusMgr->RemoveStatusEffect(dragon, StatusEffectType::Curse);

    void RemoveAllEffects(Character* target);
    // Clear all status effects
    // REASONING: Death, battle end, cleanse spells
    // IMPLEMENTATION: Call RemoveStatusEffect for each active effect
    // USAGE: statusMgr->RemoveAllEffects(dragon);

    // ===== Effect Queries =====

    bool HasStatusEffect(Character* target, StatusEffectType type) const;
    // Check if character has specific effect
    // REASONING: Check immunity, prevent duplicate effects
    // USAGE: if (!HasStatusEffect(dragon, StatusEffectType::Fear)) { ApplyFear(); }

    std::vector<StatusEffect> GetActiveEffects(Character* target) const;
    // Get all effects on character
    // REASONING: UI displays active buffs/debuffs
    // USAGE: auto effects = GetActiveEffects(dragon);  // Show icons

    int GetEffectMagnitude(Character* target, StatusEffectType type) const;
    // Get strength of effect
    // REASONING: Effects can stack or have varying power
    // USAGE: int fearLevel = GetEffectMagnitude(fighter, StatusEffectType::Fear);

    // ===== Stat Modification =====

    int GetModifiedStat(Character* target, const std::string& statName) const;
    // Calculate stat with all effect modifiers
    // REASONING: Status effects modify stats (line 61-74)
    // IMPLEMENTATION:
    //   baseStat = target->GetBaseStat(statName)
    //   for each effect:
    //     apply modifier based on effect type
    //   return modified value
    // EXAMPLES:
    //   Fear: attack -= 20%, speed -= 1
    //   Blessing: damage taken -= 25%, damage dealt += 25%
    //   Haste: speed += 1, action points += 1
    // USAGE: int effectiveSpeed = GetModifiedStat(rogue, "speed");

    // ===== Specific Status Effects =====

    void ApplyCurse(Character* target, int duration);
    // Line 63: "모든 받는 피해 증가, 모든 주는 피해 감소"
    // IMPLEMENTATION: Damage taken +25%, damage dealt -25%

    void ApplyFear(Character* target, int duration);
    // Line 65: "공격력 감소, 속력감소"
    // IMPLEMENTATION: Attack -20%, speed -1

    void ApplyBind(Character* target, int duration);
    // Line 67: "이동 불가"
    // IMPLEMENTATION: Set movement range = 0

    void ApplyBlind(Character* target, int duration);
    // Line 69: "모든 범위 감소"
    // IMPLEMENTATION: Attack range -2, spell range -2

    void ApplyBlessing(Character* target, int duration);
    // Line 71: "모든 받는 피해 감소, 모든 주는 피해 증가"
    // IMPLEMENTATION: Damage taken -25%, damage dealt +25%

    void ApplyHaste(Character* target, int duration);
    // Line 73: "속력 증가, 행동력 증가"
    // IMPLEMENTATION: Speed +1, action points +1

    void ApplyBurn(Character* target, int damagePerTurn, int duration);
    // From lava spell (line 102): 8 damage per turn for 5 turns
    // IMPLEMENTATION: Deal damage at end of each turn

private:
    // Map: Character -> list of active effects
    std::map<Character*, std::vector<StatusEffect>> activeEffects;

    // Duration tracking
    void DecrementDurations();
    void RemoveExpiredEffects();
    void ProcessEndOfTurnEffects();  // Apply burn damage, etc.
};
```

**USAGE EXAMPLE:**

```cpp
// In Dragon.cpp - Dragon Roar spell applies fear
void Dragon::Spell_DragonRoar(int upcastLevel) {
    // Line 103: "용의 포효: 모든 적에게 공포 상태 이상 부여, 시전자 기준 3칸 이내"
    int range = 3 + upcastLevel;  // Upcast: +1 range per level
    int duration = 2 + upcastLevel;  // Upcast: +1 turn per level

    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    StatusEffectManager* statusMgr = Engine::GetGameStateManager().GetGSComponent<StatusEffectManager>();

    // Find all enemies in range
    auto enemies = grid->GetCharactersInRange(GetGridPosition(), range);

    for (auto* enemy : enemies) {
        if (enemy->Type() != GameObjectTypes::Dragon) {
            // Apply fear effect (line 65: attack -, speed -)
            statusMgr->ApplyFear(enemy, duration);

            Engine::GetLogger().LogInfo(
                enemy->TypeName() + " is feared for " + std::to_string(duration) + " turns!"
            );

            // Visual effect
            EffectManager::Instance().PlayStatusEffect(enemy, StatusEffectType::Fear);
        }
    }

    // Consume resources
    ConsumeSpellSlot(2 + upcastLevel);
    currentActionPoints--;
}

// In StatusEffectManager.cpp - Apply fear debuff
void StatusEffectManager::ApplyFear(Character* target, int duration) {
    // Line 65: "공포 : 공격력 감소, 속력감소"

    // Check if already feared (don't stack)
    if (HasStatusEffect(target, StatusEffectType::Fear)) {
        Engine::GetLogger().LogWarning(target->TypeName() + " already feared!");
        return;
    }

    // Create fear effect
    StatusEffect fear;
    fear.type = StatusEffectType::Fear;
    fear.turnsRemaining = duration;
    fear.magnitude = 1;  // Standard fear
    fear.source = nullptr;  // Could track who applied it

    // Add to active effects
    activeEffects[target].push_back(fear);

    // Apply stat modifiers immediately
    target->attackPowerModifier -= 0.2f;  // -20% attack
    target->speedModifier -= 1;           // -1 speed

    EventBus::Instance().Publish(StatusEffectAppliedEvent{target, StatusEffectType::Fear, duration});
}

// In CombatSystem.cpp - Use modified stats for damage
int CombatSystem::CalculateDamage(Character* attacker, Character* target, AttackType type) {
    StatusEffectManager* statusMgr = Engine::GetGameStateManager().GetGSComponent<StatusEffectManager>();

    // Get modified attack (accounts for Fear, Curse, Blessing)
    int attackRoll = statusMgr->GetModifiedStat(attacker, "attack");

    // Get modified defense
    int defenseRoll = (type != AttackType::True) ?
        statusMgr->GetModifiedStat(target, "defense") : 0;

    // Calculate final damage
    int baseDamage = std::max(0, attackRoll - defenseRoll);

    // Apply damage modifiers (Curse, Blessing on target)
    float damageMultiplier = 1.0f;

    if (statusMgr->HasStatusEffect(target, StatusEffectType::Curse)) {
        damageMultiplier += 0.25f;  // +25% damage taken
    }
    if (statusMgr->HasStatusEffect(target, StatusEffectType::Blessing)) {
        damageMultiplier -= 0.25f;  // -25% damage taken
    }

    int finalDamage = (int)(baseDamage * damageMultiplier);

    return finalDamage;
}

// In StatusEffectManager.cpp - Process burn damage at end of turn
void StatusEffectManager::ProcessEndOfTurnEffects() {
    // Line 53: "종료 페이즈: 턴 종료시 발동하는 효과(독, 화상등) 처리"

    for (auto& [character, effects] : activeEffects) {
        for (auto& effect : effects) {
            if (effect.type == StatusEffectType::Burn) {
                // Apply burn damage
                int burnDamage = effect.magnitude;  // e.g., 8 damage per turn
                character->TakeDamage(burnDamage);

                Engine::GetLogger().LogInfo(
                    character->TypeName() + " takes " + std::to_string(burnDamage) + " burn damage!"
                );

                // Visual effect
                EffectManager::Instance().PlayStatusEffect(character, StatusEffectType::Burn);
            }
        }
    }

    // Decrement durations
    DecrementDurations();
    RemoveExpiredEffects();
}
```

---

## 4. GameObject Components - Character Behaviors

**REASONING: GOComponent Pattern from CS230 Engine**

- Character.h adds components via `AddGOComponent(new Component())`
- Components accessed via `GetGOComponent<Type>()`
- Cat.cpp line 22, 24, 25 shows component attachment pattern
- Each character is a GameObject with attached components

### 4.1 GridPosition GOComponent

**Purpose:** Store tile-based position with pathfinding cache
**Reasoning:** Grid-based positioning fundamental to tactical combat

```cpp
// GridPosition.h - GOComponent for grid positioning
// REASONING: Characters exist at discrete tile coordinates

#pragma once
#include "../Engine/Component.h"
#include "../Engine/Vec2.h"

class GridPosition : public CS230::Component {
public:
    GridPosition(int gridX, int gridY) : tilePosition(gridX, gridY) {}

    void Update(double dt) override;
    // Sync world position with grid position
    // REASONING: Both grid and world coords must stay synchronized
    // USAGE: Automatically updates sprite position when grid changes

    Math::vec2 GetTilePosition() const { return tilePosition; }
    void SetTilePosition(Math::vec2 pos);
    // Update grid position and recalc world position
    // REASONING: Movement changes tiles, must update rendering

    Math::vec2 GetWorldPosition() const;
    // Convert grid to pixel coordinates for rendering
    // IMPLEMENTATION: return tilePosition * TILE_SIZE + offset

private:
    Math::vec2 tilePosition;  // Discrete grid coordinates (0-7, 0-7)
    Math::vec2 cachedWorldPosition;
};
```

### 4.2 ActionPoints GOComponent

**Purpose:** Turn-based action economy management
**Reasoning from dragonic_tactics.md:**

- Line 27-28: Action points consumed by attacks/spells, refresh each turn

```cpp
// ActionPoints.h - GOComponent for action point tracking
// REASONING: Line 27 - "행동력은 턴마다 할 수 있는 행동의 횟수이다"

#pragma once
#include "../Engine/Component.h"

class ActionPoints : public CS230::Component {
public:
    ActionPoints(int maxAP) : maxPoints(maxAP), currentPoints(maxAP) {}

    void Update(double dt) override {}
    // No per-frame logic needed

    int GetCurrent() const { return currentPoints; }
    int GetMax() const { return maxPoints; }

    bool CanAfford(int cost) const;
    // Check if enough points for action
    // REASONING: Must validate before consuming
    // USAGE: if (actionPoints->CanAfford(1)) { Attack(); }

    void Spend(int cost);
    // Consume action points
    // REASONING: Line 27 - "공격, 주문 사용에는 행동이 소모 된다"
    // USAGE: actionPoints->Spend(1);

    void Refresh();
    // Reset to max at turn start
    // REASONING: Line 28 - "매 턴이 시작될 때마다 행동력은 회복되며, 남은 행동력은 이월되지 않는다"
    // USAGE: turnMgr->RefreshActionPoints() calls this

private:
    int maxPoints;
    int currentPoints;
};
```

### 4.3 SpellSlots GOComponent

**Purpose:** Magic resource tracking with level restrictions
**Reasoning from dragonic_tactics.md:**

- Line 29-32: Spell slots have levels, support upcasting, never regenerate

```cpp
// SpellSlots.h - GOComponent for spell slot management
// REASONING: Line 29 - "주문슬롯은 강력한 행동인 주문을 사용할 때 소모되는 자원이다"

#pragma once
#include "../Engine/Component.h"
#include <map>

class SpellSlots : public CS230::Component {
public:
    SpellSlots(const std::map<int, int>& slots) : maxSlots(slots), currentSlots(slots) {}

    void Update(double dt) override {}

    bool HasSlot(int level) const;
    // Check if slot available at level
    // REASONING: Validate before casting
    // USAGE: if (spellSlots->HasSlot(3)) { CastLevel3Spell(); }

    void ConsumeSlot(int level);
    // Use spell slot
    // REASONING: Line 29 - "주문슬롯은 게임이 끝날 때까지 재생이 되지 않는다"
    // USAGE: spellSlots->ConsumeSlot(2);

    void RestoreSlots(int minLevel, int maxLevel);
    // Restore slot range (for Heat Release spell)
    // REASONING: Line 108 - "3레벨 이하의 주문슬롯을 전부 회복"
    // USAGE: spellSlots->RestoreSlots(1, 3);

    std::map<int, int> GetCurrentSlots() const { return currentSlots; }
    std::map<int, int> GetMaxSlots() const { return maxSlots; }

private:
    std::map<int, int> maxSlots;      // Level -> max count
    std::map<int, int> currentSlots;  // Level -> remaining
};
```

**USAGE EXAMPLE:**

```cpp
// In Dragon.cpp - Cast Fireball with upcasting
void Dragon::Spell_Fireball(Math::vec2 target, int upcastLevel) {
    SpellSlots* slots = GetGOComponent<SpellSlots>();

    // Base spell is level 1, upcast uses higher slots
    int slotLevel = 1 + upcastLevel;

    // Check if slot available
    if (!slots->HasSlot(slotLevel)) {
        Engine::GetLogger().LogError("No level " + std::to_string(slotLevel) + " slot available!");
        return;
    }

    // Calculate damage: 2d8 base + 1d6 per upcast level
    DiceManager& dice = DiceManager::Instance();
    int baseDamage = dice.RollDice("2d8");
    int bonusDamage = upcastLevel > 0 ? dice.RollDice(std::to_string(upcastLevel) + "d6") : 0;

    // Apply damage
    CombatSystem* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
    combat->DealDamage(target, baseDamage + bonusDamage, DamageType::Fire);

    // Consume spell slot
    slots->ConsumeSlot(slotLevel);

    Engine::GetLogger().LogInfo(
        "Cast Fireball at level " + std::to_string(slotLevel) +
        " for " + std::to_string(baseDamage + bonusDamage) + " damage!"
    );
}

// In Dragon.cpp - Heat Release restores low-level slots
void Dragon::Spell_HeatRelease() {
    // Line 108: "자신의 현재 체력의 50%를 소모하고, 3레벨 이하의 주문슬롯을 전부 회복"
    SpellSlots* slots = GetGOComponent<SpellSlots>();

    // Pay HP cost
    int hpCost = GetCurrentHP() / 2;
    TakeDamage(hpCost);

    // Restore 1st, 2nd, 3rd level slots
    slots->RestoreSlots(1, 3);

    Engine::GetLogger().LogInfo(
        TypeName() + " sacrifices " + std::to_string(hpCost) +
        " HP to restore spell slots (levels 1-3)!"
    );
}
```

---

### 4.4 StatusEffects GOComponent

**Purpose:** Individual character's active buff/debuff collection
**Reasoning from dragonic_tactics.md:**

- Line 61-74: Six status effects with duration tracking
- Characters can have multiple effects simultaneously
- Effects modify stats and behavior

```cpp
// StatusEffects.h - GOComponent for individual character effects
// REASONING: Each character tracks their own active buffs/debuffs
// NOTE: This is DIFFERENT from StatusEffectManager GSComponent
//   - StatusEffects (GOComponent): Stores effects on ONE character
//   - StatusEffectManager (GSComponent): Coordinates ALL characters' effects globally

#pragma once
#include "../Engine/Component.h"
#include <vector>

// Forward declaration
enum class StatusEffectType;
struct StatusEffect;

class StatusEffects : public CS230::Component {
public:
    StatusEffects() {}

    void Update(double dt) override;
    // Visual updates for effect animations
    // REASONING: Status effects should show visual indicators (icons, particle effects)
    // IMPLEMENTATION:
    //   1. Update effect animation timers
    //   2. Pulse effect icons
    //   3. Display duration counters
    // USAGE: Called automatically by GameObject::Update()

    // ===== Effect Management =====

    void AddEffect(const StatusEffect& effect);
    // Add new status effect to this character
    // REASONING: StatusEffectManager calls this when applying effects
    // IMPLEMENTATION:
    //   1. Check if effect already exists
    //   2. If stackable (Burn, Poison): Add new instance
    //   3. If not stackable (Fear, Blessing): Refresh duration
    //   4. Update stat modifiers
    // USAGE: effects->AddEffect(fearEffect);

    void RemoveEffect(StatusEffectType type);
    // Remove specific effect type
    // REASONING: Cleansing spells, effect expiration
    // IMPLEMENTATION:
    //   1. Find effect in activeEffects vector
    //   2. Remove stat modifiers
    //   3. Erase from vector
    // USAGE: effects->RemoveEffect(StatusEffectType::Curse);

    void RemoveAllEffects();
    // Clear all status effects
    // REASONING: Character death, battle end
    // IMPLEMENTATION: Call RemoveEffect for each active effect
    // USAGE: effects->RemoveAllEffects();

    // ===== Effect Queries =====

    bool HasEffect(StatusEffectType type) const;
    // Check if character has specific effect
    // REASONING: Prevent duplicate non-stacking effects
    // USAGE: if (!effects->HasEffect(StatusEffectType::Fear)) { ApplyFear(); }

    std::vector<StatusEffect> GetAllEffects() const { return activeEffects; }
    // Get all active effects
    // REASONING: UI displays effect icons, stat calculations need all modifiers
    // USAGE: auto allEffects = effects->GetAllEffects();

    int GetEffectDuration(StatusEffectType type) const;
    // Get turns remaining for effect
    // REASONING: UI shows countdown timers
    // USAGE: int turnsLeft = effects->GetEffectDuration(StatusEffectType::Blessing);

    int GetEffectCount(StatusEffectType type) const;
    // Count stacks of effect
    // REASONING: Burn and Poison can stack
    // USAGE: int burnStacks = effects->GetEffectCount(StatusEffectType::Burn);

    // ===== Stat Modifiers =====

    float GetAttackModifier() const;
    // Calculate total attack modifier from all effects
    // REASONING: Fear reduces attack, Blessing increases it
    // IMPLEMENTATION:
    //   float modifier = 1.0f;
    //   if (HasEffect(Fear)) modifier *= 0.8f;  // -20%
    //   if (HasEffect(Blessing)) modifier *= 1.25f;  // +25%
    //   return modifier;
    // USAGE: float attackMod = effects->GetAttackModifier();

    float GetDefenseModifier() const;
    // Calculate total defense modifier
    // REASONING: Curse increases damage taken, Blessing reduces it
    // USAGE: float defenseMod = effects->GetDefenseModifier();

    int GetSpeedModifier() const;
    // Calculate speed modifier
    // REASONING: Fear -1 speed, Haste +1 speed
    // USAGE: int totalSpeed = baseSpeed + effects->GetSpeedModifier();

    int GetActionPointModifier() const;
    // Calculate action point bonus
    // REASONING: Haste grants +1 action point
    // USAGE: int maxAP = baseAP + effects->GetActionPointModifier();

    // ===== Movement/Targeting Restrictions =====

    bool CanMove() const;
    // Check if movement allowed
    // REASONING: Bind effect prevents movement
    // IMPLEMENTATION: return !HasEffect(StatusEffectType::Bind);
    // USAGE: if (effects->CanMove()) { ProcessMovement(); }

    int GetRangeModifier() const;
    // Calculate range reduction
    // REASONING: Blind reduces all ranges
    // IMPLEMENTATION: return HasEffect(StatusEffectType::Blind) ? -2 : 0;
    // USAGE: int effectiveRange = baseRange + effects->GetRangeModifier();

    // ===== Visual Feedback =====

    void RenderEffectIcons(Math::vec2 position) const;
    // Draw status effect icons above character
    // REASONING: Player needs to see active effects at a glance
    // IMPLEMENTATION:
    //   1. Get icon sprite for each effect type
    //   2. Position icons in row above character
    //   3. Draw duration counter below each icon
    //   4. Apply visual effects (Fear = shaking, Blessing = glow)
    // USAGE: Called by Character::Draw()

private:
    std::vector<StatusEffect> activeEffects;

    // Visual state
    float iconAnimationTimer = 0.0f;

    // Helper methods
    bool IsStackableEffect(StatusEffectType type) const;
    // Returns true for Burn, Poison; false for Fear, Blessing, Curse
};
```

**USAGE EXAMPLE:**

```cpp
// In Character.cpp - Check if can perform action
bool Character::CanPerformAction(ActionType action) const {
    StatusEffects* effects = GetGOComponent<StatusEffects>();

    // Check movement restriction
    if (action == ActionType::Move && !effects->CanMove()) {
        Engine::GetLogger().LogWarning(TypeName() + " is bound and cannot move!");
        return false;
    }

    // Check action points
    ActionPoints* ap = GetGOComponent<ActionPoints>();
    if (!ap->CanAfford(1)) {
        return false;
    }

    return true;
}

// In CombatSystem.cpp - Apply stat modifiers to damage
int CombatSystem::CalculateDamage(Character* attacker, Character* target, AttackType type) {
    // Get components
    StatusEffects* attackerEffects = attacker->GetGOComponent<StatusEffects>();
    StatusEffects* targetEffects = target->GetGOComponent<StatusEffects>();
    DamageCalculator* damageCalc = attacker->GetGOComponent<DamageCalculator>();

    // Calculate base damage with dice
    int baseDamage = damageCalc->RollAttack();

    // Apply attacker's stat modifiers (Fear, Blessing)
    float attackMod = attackerEffects->GetAttackModifier();
    baseDamage = (int)(baseDamage * attackMod);

    // Calculate defense
    int defense = damageCalc->RollDefense(target);

    // Apply target's defense modifiers (Curse, Blessing)
    float defenseMod = targetEffects->GetDefenseModifier();
    defense = (int)(defense * defenseMod);

    // Final damage
    int finalDamage = std::max(0, baseDamage - defense);

    Engine::GetLogger().LogDebug(
        attacker->TypeName() + " attack: " + std::to_string(baseDamage) +
        " (mod: " + std::to_string(attackMod) + ") vs " +
        target->TypeName() + " defense: " + std::to_string(defense) +
        " (mod: " + std::to_string(defenseMod) + ") = " +
        std::to_string(finalDamage) + " damage"
    );

    return finalDamage;
}
```

---

### 4.5 AIMemory GOComponent

**Purpose:** Tactical decision history for AI characters
**Reasoning:**

- AI should learn from previous turns
- Remember which positions worked/failed
- Track damage dealt/taken for threat assessment

```cpp
// AIMemory.h - GOComponent for AI decision tracking
// REASONING: AI needs memory to avoid repeating mistakes and make strategic decisions

#pragma once
#include "../Engine/Component.h"
#include "../Engine/Vec2.h"
#include <vector>
#include <map>

struct TurnMemory {
    int turnNumber;
    ActionType actionTaken;
    Math::vec2 positionUsed;
    Character* targetedCharacter;
    int damageDealt;
    int damageTaken;
    bool wasSuccessful;  // Did the action achieve its goal?
};

class AIMemory : public CS230::Component {
public:
    AIMemory() {}

    void Update(double dt) override {}
    // No per-frame logic needed

    // ===== Memory Recording =====

    void RecordTurn(const TurnMemory& memory);
    // Save decision and outcome
    // REASONING: AI learns from experience
    // IMPLEMENTATION:
    //   1. Add to turnHistory vector
    //   2. Update position success rate
    //   3. Update target priority scores
    //   4. Prune old memories (keep last 10 turns)
    // USAGE: aiMemory->RecordTurn({turnNum, action, pos, target, dmg, taken, success});

    void RecordDamageTaken(Character* source, int amount);
    // Track who is hurting this AI character
    // REASONING: AI should prioritize threats
    // USAGE: aiMemory->RecordDamageTaken(dragon, 25);

    void RecordDamageDealt(Character* target, int amount);
    // Track damage output
    // REASONING: Helps evaluate effectiveness
    // USAGE: aiMemory->RecordDamageDealt(dragon, 18);

    void RecordFailedAction(ActionType action, const std::string& reason);
    // Remember why action failed
    // REASONING: Avoid repeating same mistakes
    // USAGE: aiMemory->RecordFailedAction(ActionType::Move, "Path blocked");

    // ===== Memory Queries =====

    float GetPositionSuccessRate(Math::vec2 position) const;
    // Get success rate for specific tile
    // REASONING: Avoid positions that led to high damage taken
    // IMPLEMENTATION:
    //   successes = count(turns where pos used and wasSuccessful == true)
    //   failures = count(turns where pos used and wasSuccessful == false)
    //   return successes / (successes + failures)
    // USAGE: float rate = aiMemory->GetPositionSuccessRate({3, 4});

    Character* GetHighestThreatTarget() const;
    // Find character that dealt most damage to us
    // REASONING: AI should prioritize dangerous opponents
    // IMPLEMENTATION: return max(damageBySource)
    // USAGE: auto* threat = aiMemory->GetHighestThreatTarget();

    std::vector<Math::vec2> GetDangerousPositions() const;
    // Get tiles where AI took heavy damage
    // REASONING: Avoid walking into danger zones
    // IMPLEMENTATION: return positions where damageTaken > 20
    // USAGE: auto dangerZones = aiMemory->GetDangerousPositions();

    int GetTotalDamageDealt() const;
    // Sum of all damage this AI has inflicted
    // REASONING: Measure offensive effectiveness
    // USAGE: int totalDmg = aiMemory->GetTotalDamageDealt();

    int GetTotalDamageTaken() const;
    // Sum of all damage this AI has received
    // REASONING: Measure survival effectiveness
    // USAGE: int totalTaken = aiMemory->GetTotalDamageTaken();

    // ===== Strategic Insights =====

    bool ShouldRetreat() const;
    // Evaluate if tactical retreat needed
    // REASONING: AI should flee when overwhelmed
    // IMPLEMENTATION:
    //   recentDamageTaken = sum(last 3 turns damageTaken)
    //   if recentDamageTaken > currentHP * 0.5: return true
    //   return false
    // USAGE: if (aiMemory->ShouldRetreat()) { FindSafeTile(); }

    Math::vec2 GetMostSuccessfulPosition() const;
    // Find best historical position
    // REASONING: Return to positions that worked well
    // IMPLEMENTATION: return max(GetPositionSuccessRate for all visited positions)
    // USAGE: auto safeTile = aiMemory->GetMostSuccessfulPosition();

    ActionType GetMostEffectiveAction() const;
    // Find action that dealt most damage
    // REASONING: Prioritize high-value actions
    // USAGE: auto bestAction = aiMemory->GetMostEffectiveAction();

    // ===== Memory Management =====

    void ClearMemory();
    // Wipe all recorded data
    // REASONING: New battle should start fresh
    // USAGE: aiMemory->ClearMemory();

    int GetTurnCount() const { return turnHistory.size(); }
    // Get number of turns this AI has taken
    // USAGE: int turns = aiMemory->GetTurnCount();

private:
    std::vector<TurnMemory> turnHistory;  // Recent turn records (max 10)

    // Aggregated statistics
    std::map<Character*, int> damageBySource;   // Who hurt us
    std::map<Character*, int> damageToTarget;   // Who we hurt
    std::map<Math::vec2, int> positionSuccesses;
    std::map<Math::vec2, int> positionFailures;

    int totalDamageDealt = 0;
    int totalDamageTaken = 0;

    static constexpr int MAX_MEMORY_SIZE = 10;  // Keep last 10 turns
};
```

**USAGE EXAMPLE:**

```cpp
// In AISystem.cpp - Use memory for better decisions
ActionDecision AISystem::DecideAction(Character* aiCharacter) {
    AIMemory* memory = aiCharacter->GetGOComponent<AIMemory>();

    // Check if should retreat based on recent damage
    if (memory->ShouldRetreat()) {
        // Find safest position
        Math::vec2 safeTile = memory->GetMostSuccessfulPosition();
        return ActionDecision{
            .action = ActionType::Move,
            .target = nullptr,
            .position = safeTile,
            .priority = 10  // High priority survival move
        };
    }

    // Use most effective action from history
    ActionType bestAction = memory->GetMostEffectiveAction();

    // Target highest threat
    Character* threat = memory->GetHighestThreatTarget();
    if (!threat) {
        threat = FindDragon();  // Default to dragon
    }

    return ActionDecision{
        .action = bestAction,
        .target = threat,
        .position = threat->GetGridPosition(),
        .priority = 7
    };
}

// In Fighter.cpp - Record turn outcome
void Fighter::ExecuteDecision(const ActionDecision& decision) {
    AIMemory* memory = GetGOComponent<AIMemory>();
    TurnManager* turnMgr = Engine::GetGameStateManager().GetGSComponent<TurnManager>();

    // Store initial HP to calculate damage taken this turn
    int hpBefore = GetCurrentHP();

    // Execute action
    PerformAction(decision.action);

    // Record outcome
    int hpAfter = GetCurrentHP();
    int damageTaken = hpBefore - hpAfter;

    TurnMemory turnRecord;
    turnRecord.turnNumber = turnMgr->GetCurrentTurnNumber();
    turnRecord.actionTaken = decision.action;
    turnRecord.positionUsed = GetGridPosition();
    turnRecord.targetedCharacter = decision.target;
    turnRecord.damageDealt = lastDamageDealt;  // Tracked by DamageCalculator
    turnRecord.damageTaken = damageTaken;
    turnRecord.wasSuccessful = (damageTaken < 10);  // Low damage = good position

    memory->RecordTurn(turnRecord);
}
```

---

### 4.6 DamageCalculator GOComponent

**Purpose:** Dice-based combat resolution for individual character
**Reasoning from dragonic_tactics.md:**

- Line 21-25: D&D-style dice rolling (3d6, 2d8, etc.)
- Line 39-40: Attack/Defense both use dice + base values
- Each character has unique dice formulas

```cpp
// DamageCalculator.h - GOComponent for dice-based damage calculation
// REASONING: Line 21 - "D&D와 유사하게 피해를 주사위로 표현"

#pragma once
#include "../Engine/Component.h"
#include <string>

class DamageCalculator : public CS230::Component {
public:
    DamageCalculator(int baseAtk, const std::string& atkDice,
                     int baseDef, const std::string& defDice)
        : baseAttackPower(baseAtk), attackDiceFormula(atkDice),
          baseDefensePower(baseDef), defenseDiceFormula(defDice) {}

    void Update(double dt) override {}

    // ===== Attack Calculation =====

    int RollAttack();
    // Roll attack damage
    // REASONING: Line 39 - "공격력 = 공격력 상수 + 공격용 주사위"
    // IMPLEMENTATION:
    //   int diceResult = DiceManager::Instance().RollDice(attackDiceFormula);
    //   return baseAttackPower + diceResult;
    // EXAMPLES:
    //   Dragon: 0 + 3d6 = 0 + (3-18) = 3-18 damage
    //   Fighter: 5 + 2d6 = 5 + (2-12) = 7-17 damage
    //   Wizard: 12 + 3d6 = 12 + (3-18) = 15-30 damage
    // USAGE: int damage = damageCalc->RollAttack();

    int RollDefense(Character* attacker = nullptr);
    // Roll defense value
    // REASONING: Line 40 - "방어력 = 방어력 상수 + 방어용 주사위"
    // IMPLEMENTATION:
    //   int diceResult = DiceManager::Instance().RollDice(defenseDiceFormula);
    //   return baseDefensePower + diceResult;
    // EXAMPLES:
    //   Dragon: 0 + 2d8 = 0 + (2-16) = 2-16 defense
    //   Fighter: 0 + 1d10 = 0 + (1-10) = 1-10 defense
    // USAGE: int defense = damageCalc->RollDefense();

    int CalculateFinalDamage(int attackRoll, int defenseRoll);
    // Compute final damage after defense
    // REASONING: Line 41 - "최종 피해 = max(0, 공격력 - 방어력)"
    // IMPLEMENTATION: return std::max(0, attackRoll - defenseRoll);
    // USAGE: int dmg = damageCalc->CalculateFinalDamage(15, 8);  // = 7

    // ===== Stat Queries =====

    int GetBaseAttackPower() const { return baseAttackPower; }
    std::string GetAttackDice() const { return attackDiceFormula; }
    int GetAverageAttack() const;
    // Calculate expected attack value
    // REASONING: AI needs to estimate damage for decisions
    // IMPLEMENTATION:
    //   avgDice = DiceManager::Instance().GetAverageDiceValue(attackDiceFormula);
    //   return baseAttackPower + avgDice;
    // USAGE: int expectedDmg = damageCalc->GetAverageAttack();

    int GetBaseDefensePower() const { return baseDefensePower; }
    std::string GetDefenseDice() const { return defenseDiceFormula; }
    int GetAverageDefense() const;
    // Calculate expected defense value
    // USAGE: int expectedDef = damageCalc->GetAverageDefense();

    // ===== Stat Modification =====

    void ModifyBaseAttack(int amount);
    // Temporarily boost/reduce attack
    // REASONING: Buffs, debuffs, equipment can modify base values
    // USAGE: damageCalc->ModifyBaseAttack(5);  // +5 attack

    void ModifyBaseDefense(int amount);
    // Temporarily boost/reduce defense
    // USAGE: damageCalc->ModifyBaseDefense(-3);  // -3 defense

    void SetAttackDice(const std::string& newFormula);
    // Change attack dice (rare, for special abilities)
    // USAGE: damageCalc->SetAttackDice("5d6");  // Temporary power boost

    // ===== Combat Statistics =====

    void RecordDamageDealt(int amount);
    // Track damage for statistics
    // REASONING: Used by AIMemory and UI
    // USAGE: damageCalc->RecordDamageDealt(18);

    void RecordDamageTaken(int amount);
    // Track incoming damage
    // USAGE: damageCalc->RecordDamageTaken(12);

    int GetTotalDamageDealt() const { return totalDamageDealt; }
    int GetTotalDamageTaken() const { return totalDamageTaken; }
    int GetLastDamageDealt() const { return lastDamageDealt; }
    int GetLastDamageTaken() const { return lastDamageTaken; }

private:
    // Base stats (from JSON via DataRegistry)
    int baseAttackPower;
    std::string attackDiceFormula;  // e.g., "3d6", "2d8+1d6"
    int baseDefensePower;
    std::string defenseDiceFormula;

    // Combat tracking
    int totalDamageDealt = 0;
    int totalDamageTaken = 0;
    int lastDamageDealt = 0;
    int lastDamageTaken = 0;
};
```

**USAGE EXAMPLE:**

```cpp
// In CombatSystem.cpp - Execute attack with dice rolls
void CombatSystem::ExecuteAttack(Character* attacker, Character* target, AttackType type) {
    DamageCalculator* attackerCalc = attacker->GetGOComponent<DamageCalculator>();
    DamageCalculator* targetCalc = target->GetGOComponent<DamageCalculator>();

    // Roll attack dice
    int attackRoll = attackerCalc->RollAttack();

    Engine::GetLogger().LogDebug(
        attacker->TypeName() + " rolls attack: " +
        std::to_string(attackerCalc->GetBaseAttackPower()) + " + " +
        attackerCalc->GetAttackDice() + " = " + std::to_string(attackRoll)
    );

    // Roll defense dice (unless true damage)
    int defenseRoll = 0;
    if (type != AttackType::True) {
        defenseRoll = targetCalc->RollDefense();
        Engine::GetLogger().LogDebug(
            target->TypeName() + " rolls defense: " +
            std::to_string(targetCalc->GetBaseDefensePower()) + " + " +
            targetCalc->GetDefenseDice() + " = " + std::to_string(defenseRoll)
        );
    }

    // Calculate final damage
    int finalDamage = attackerCalc->CalculateFinalDamage(attackRoll, defenseRoll);

    // Apply damage
    target->TakeDamage(finalDamage);

    // Record statistics
    attackerCalc->RecordDamageDealt(finalDamage);
    targetCalc->RecordDamageTaken(finalDamage);

    // Update AIMemory
    if (AIMemory* memory = attacker->GetGOComponent<AIMemory>()) {
        memory->RecordDamageDealt(target, finalDamage);
    }
    if (AIMemory* memory = target->GetGOComponent<AIMemory>()) {
        memory->RecordDamageTaken(attacker, finalDamage);
    }

    Engine::GetLogger().LogInfo(
        attacker->TypeName() + " hits " + target->TypeName() +
        " for " + std::to_string(finalDamage) + " damage!"
    );

    // Publish event
    EventBus::Instance().Publish(DamageDealtEvent{attacker, target, finalDamage});
}
```

---

### 4.7 TargetingSystem GOComponent

**Purpose:** Attack/spell range validation and target filtering
**Reasoning from dragonic_tactics.md:**

- Line 141, 170, 201, 227, 254: Each character has different attack ranges
- Line 272-273: Dragon uses straight-line targeting (queen movement)
- Line 284-286: Different spells have different range/targeting rules

```cpp
// TargetingSystem.h - GOComponent for range validation and target selection
// REASONING: Each character has unique targeting constraints

#pragma once
#include "../Engine/Component.h"
#include "../Engine/Vec2.h"
#include <vector>

enum class TargetingMode {
    Melee,          // Adjacent tiles only (range 1)
    Ranged,         // Straight lines or free aim
    StraightLine,   // Line 272: Dragon's queen-like movement
    Area,           // AOE targeting
    Self,           // Self-targeted buffs/heals
    Ally            // Friendly unit targeting
};

class TargetingSystem : public CS230::Component {
public:
    TargetingSystem(int range, TargetingMode mode)
        : attackRange(range), targetingMode(mode) {}

    void Update(double dt) override {}

    // ===== Range Validation =====

    bool IsInRange(Math::vec2 targetPosition) const;
    // Check if target is within attack range
    // REASONING: Must validate targeting before attacks/spells
    // IMPLEMENTATION:
    //   GameObject* owner = GetOwner();
    //   Math::vec2 myPos = owner->GetPosition();
    //   int distance = ManhattanDistance(myPos, targetPosition);
    //
    //   switch (targetingMode):
    //     Melee: return distance == 1
    //     Ranged: return distance <= attackRange
    //     StraightLine: return IsOnStraightLine(myPos, targetPosition) && distance <= attackRange
    // USAGE: if (targeting->IsInRange(enemyPos)) { Attack(); }

    bool IsValidTarget(Character* target) const;
    // Check if character is valid target
    // REASONING: Cannot target dead, stealthed, or friendly units (usually)
    // IMPLEMENTATION:
    //   if (!target || !target->IsAlive()) return false;
    //   if (target->IsStealthed() && targetingMode != TargetingMode::Area) return false;
    //   if (targetingMode == TargetingMode::Ally && target->IsEnemy()) return false;
    //   return true;
    // USAGE: if (targeting->IsValidTarget(dragon)) { SelectTarget(dragon); }

    std::vector<Character*> GetTargetsInRange() const;
    // Get all valid targets within range
    // REASONING: AI needs list of possible targets for decision-making
    // IMPLEMENTATION:
    //   GridSystem* grid = GetGSComponent<GridSystem>();
    //   std::vector<Character*> targets;
    //   for (auto* character : grid->GetAllCharacters()):
    //     if (IsInRange(character->GetGridPosition()) && IsValidTarget(character)):
    //       targets.push_back(character);
    //   return targets;
    // USAGE: auto enemies = targeting->GetTargetsInRange();

    // ===== Line of Sight =====

    bool HasLineOfSight(Math::vec2 targetPosition) const;
    // Check if path to target is unobstructed
    // REASONING: Line 284 - Some spells require clear line of sight
    // IMPLEMENTATION:
    //   GridSystem* grid = GetGSComponent<GridSystem>();
    //   std::vector<Math::vec2> line = grid->GetLinePositions(myPos, targetPosition);
    //   for (auto pos : line):
    //     if (grid->IsBlocked(pos)) return false;  // Wall/obstacle blocks
    //   return true;
    // USAGE: if (targeting->HasLineOfSight(target)) { CastSpell(); }

    bool IsOnStraightLine(Math::vec2 from, Math::vec2 to) const;
    // Check if target on horizontal/vertical/diagonal line
    // REASONING: Line 272 - Dragon attacks like chess queen (straight lines)
    // IMPLEMENTATION:
    //   Math::vec2 delta = to - from;
    //   return (delta.x == 0) ||  // Vertical
    //          (delta.y == 0) ||  // Horizontal
    //          (abs(delta.x) == abs(delta.y));  // Diagonal
    // USAGE: if (IsOnStraightLine(myPos, enemyPos)) { DragonAttack(); }

    // ===== Area Targeting =====

    std::vector<Math::vec2> GetAreaTiles(Math::vec2 center, int radius, bool square = false) const;
    // Get tiles in area effect
    // REASONING: AOE spells like Fireball, Lava Pool need area calculation
    // IMPLEMENTATION:
    //   GridSystem* grid = GetGSComponent<GridSystem>();
    //   return grid->GetTilesInArea(center, radius, square);
    // EXAMPLES:
    //   Fireball: GetAreaTiles(target, 1, false)  // 1-tile radius circle
    //   Lava Pool: GetAreaTiles(target, 2, true)  // 2x2 square
    // USAGE: auto affectedTiles = targeting->GetAreaTiles(castPos, 2, true);

    std::vector<Character*> GetCharactersInArea(Math::vec2 center, int radius, bool square = false) const;
    // Get all characters hit by AOE
    // REASONING: Need to apply damage/effects to multiple targets
    // IMPLEMENTATION:
    //   auto tiles = GetAreaTiles(center, radius, square);
    //   GridSystem* grid = GetGSComponent<GridSystem>();
    //   std::vector<Character*> chars;
    //   for (auto tile : tiles):
    //     if (Character* c = grid->GetCharacterAt(tile)) chars.push_back(c);
    //   return chars;
    // USAGE: auto victims = targeting->GetCharactersInArea(explodePos, 1);

    // ===== Range Modification =====

    void SetRange(int newRange);
    // Modify attack range
    // REASONING: Blind effect reduces ranges, buffs can increase
    // USAGE: targeting->SetRange(originalRange - 2);  // Blind debuff

    int GetRange() const { return attackRange; }
    int GetEffectiveRange() const;
    // Get range after status effect modifiers
    // REASONING: Blind reduces all ranges by 2
    // IMPLEMENTATION:
    //   int range = attackRange;
    //   StatusEffects* effects = GetOwner()->GetGOComponent<StatusEffects>();
    //   range += effects->GetRangeModifier();  // Blind: -2
    //   return std::max(0, range);
    // USAGE: int realRange = targeting->GetEffectiveRange();

    // ===== Targeting Mode =====

    TargetingMode GetMode() const { return targetingMode; }
    void SetMode(TargetingMode mode) { targetingMode = mode; }
    // Change targeting mode (rare, for special abilities)
    // USAGE: targeting->SetMode(TargetingMode::Area);

private:
    int attackRange;
    TargetingMode targetingMode;

    // Cached for performance
    mutable std::vector<Character*> cachedTargets;
    mutable int lastCacheFrame = -1;
};
```

**USAGE EXAMPLE:**

```cpp
// In Dragon.cpp - Validate straight-line attack
bool Dragon::CanAttack(Character* target) const {
    TargetingSystem* targeting = GetGOComponent<TargetingSystem>();

    // Check basic validity
    if (!targeting->IsValidTarget(target)) {
        return false;
    }

    // Dragon attacks in straight lines (line 272-273)
    Math::vec2 targetPos = target->GetGridPosition();
    if (!targeting->IsOnStraightLine(GetGridPosition(), targetPos)) {
        Engine::GetLogger().LogWarning("Target not on straight line!");
        return false;
    }

    // Check range
    if (!targeting->IsInRange(targetPos)) {
        Engine::GetLogger().LogWarning("Target out of range!");
        return false;
    }

    // Check line of sight
    if (!targeting->HasLineOfSight(targetPos)) {
        Engine::GetLogger().LogWarning("Line of sight blocked!");
        return false;
    }

    return true;
}

// In Dragon.cpp - Fireball AOE targeting
void Dragon::Spell_Fireball(Math::vec2 targetCenter, int upcastLevel) {
    TargetingSystem* targeting = GetGOComponent<TargetingSystem>();

    // Get all characters in 1-tile radius
    auto victims = targeting->GetCharactersInArea(targetCenter, 1, false);

    // Calculate damage
    DiceManager& dice = DiceManager::Instance();
    int baseDamage = dice.RollDice("2d8");
    int bonusDamage = upcastLevel > 0 ? dice.RollDice(std::to_string(upcastLevel) + "d6") : 0;
    int totalDamage = baseDamage + bonusDamage;

    // Apply to all victims
    CombatSystem* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
    for (auto* victim : victims) {
        combat->DealDamage(victim, totalDamage, DamageType::Fire);
        Engine::GetLogger().LogInfo(
            victim->TypeName() + " caught in fireball for " + std::to_string(totalDamage) + " damage!"
        );
    }

    Engine::GetLogger().LogInfo(
        "Fireball hits " + std::to_string(victims.size()) + " targets!"
    );
}

// In AISystem.cpp - Find best target using TargetingSystem
Character* AISystem::SelectBestTarget(Character* attacker) {
    TargetingSystem* targeting = attacker->GetGOComponent<TargetingSystem>();

    // Get all valid targets
    auto potentialTargets = targeting->GetTargetsInRange();

    if (potentialTargets.empty()) {
        return nullptr;
    }

    // Score each target
    Character* bestTarget = nullptr;
    int highestScore = -1;

    for (auto* target : potentialTargets) {
        int score = CalculateThreatLevel(target);

        // Bonus: Target lowest HP enemy for kill potential
        float hpPercent = (float)target->GetCurrentHP() / target->GetMaxHP();
        if (hpPercent < 0.3f) {
            score += 50;  // High priority on low HP targets
        }

        if (score > highestScore) {
            highestScore = score;
            bestTarget = target;
        }
    }

    return bestTarget;
}
```

---

## Summary

All seven GameObject Components are now fully documented:

1. ✅ **GridPosition** - Tile-based positioning and world coord sync
2. ✅ **ActionPoints** - Turn-based action economy
3. ✅ **SpellSlots** - Magic resource tracking with upcasting
4. ✅ **StatusEffects** - Individual character's buff/debuff collection
5. ✅ **AIMemory** - Tactical decision history and learning
6. ✅ **DamageCalculator** - Dice-based combat resolution
7. ✅ **TargetingSystem** - Range validation and target filtering

These GOComponents attach to Character GameObjects to provide modular, composable functionality for the tactical RPG combat system.
