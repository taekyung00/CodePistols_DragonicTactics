// File: CS230/Game/System/CombatSystem.h
#pragma once
#include "../Characters/Character.h"
#include <string>

enum class DamageType {
    Physical,
    Fire,
    Cold,
    Lightning,
    Poison
};

class CombatSystem {
public:
    static CombatSystem& Instance();

    // Core combat methods
    int CalculateDamage(Character* attacker, Character* defender, const std::string& damageDice, int baseDamage);
    void ApplyDamage(Character* attacker, Character* defender, int damage);

    // Attack execution
    bool ExecuteAttack(Character* attacker, Character* defender);

    // Damage calculation helpers
    int RollAttackDamage(const std::string& damageDice, int baseDamage);
    bool IsCriticalHit();  // Week 4: critical hit system

    // Range checking
    bool IsInRange(Character* attacker, Character* target, int range);
    int GetDistance(Character* char1, Character* char2);

private:
    CombatSystem() = default;
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;
};