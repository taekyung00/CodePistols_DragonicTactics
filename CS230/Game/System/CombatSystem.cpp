#include "CombatSystem.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../System/GridSystem.h"
#include "../../Engine/Engine.h"
#include "../StatsComponent.h" 
#include "../GridPosition.h"
#include "../ActionPoints.h"
#include "../Types/Events.h"
//#include "../SpellSlots.h"

CombatSystem& CombatSystem::Instance() {
    static CombatSystem instance;
    return instance;
}

int CombatSystem::CalculateDamage(Character* attacker, Character* defender,
    const std::string& damageDice, int baseDamage)
{
    if (attacker == nullptr || defender == nullptr) {
        Engine::GetLogger().LogError("CombatSystem: Null attacker or defender");
        return 0;
    }

    // Roll attack dice
    int diceRoll = DiceManager::Instance().RollDiceFromString(damageDice);
    int totalDamage = diceRoll + baseDamage;

    // Week 2: Simple damage calculation
    // Week 4: Add defense calculation, resistances, etc.

    Engine::GetLogger().LogEvent("CombatSystem: " + attacker->TypeName() +
        " rolled " + damageDice + " = " + std::to_string(diceRoll) +
        " + " + std::to_string(baseDamage) + " = " + std::to_string(totalDamage) + " damage");

    return totalDamage;
}

void CombatSystem::ApplyDamage(Character* attacker, Character* defender, int damage) {
    if (defender == nullptr) {
        Engine::GetLogger().LogError("CombatSystem: Null defender");
        return;
    }

    if (damage < 0) {
        Engine::GetLogger().LogError("CombatSystem: Negative damage (" + std::to_string(damage) + ")");
        damage = 0;
    }

    // Apply damage to defender
    auto hpBefore = defender->GetStatsComponent()->GetCurrentHP();
    defender->TakeDamage(damage, attacker);
    int hpAfter = defender->GetStatsComponent()->GetCurrentHP();

    Engine::GetLogger().LogEvent("CombatSystem: " + defender->TypeName() +
        " took " + std::to_string(damage) + " damage (" +
        std::to_string(hpBefore) + " -> " + std::to_string(hpAfter) + " HP)");

    // Publish damage event
    Engine::Instance().GetEventBus().Publish(
        CharacterDamagedEvent{
        defender,
        damage,
        hpAfter,
        attacker,
        !defender->IsAlive()
        });

    // Check if defender died
    if (!defender->IsAlive()) {
        Engine::GetLogger().LogEvent("CombatSystem: " + defender->TypeName() + " died!");
        //Engine::Instance().GetEventBus().Publish(CharacterDiedEvent{ defender, attacker });
    }
}

bool CombatSystem::ExecuteAttack(Character* attacker, Character* defender) {
    if (attacker == nullptr || defender == nullptr) {
        Engine::GetLogger().LogError("CombatSystem: Null attacker or defender");
        return false;
    }

    if (!attacker->IsAlive()) {
        Engine::GetLogger().LogError("CombatSystem: Dead attacker cannot attack");
        return false;
    }

    if (!defender->IsAlive()) {
        Engine::GetLogger().LogError("CombatSystem: Cannot attack dead defender");
        return false;
    }

    // Check range
    if (!IsInRange(attacker, defender, attacker->GetStatsComponent()->GetAttackRange())) {
        Engine::GetLogger().LogError("CombatSystem: Target out of range");
        return false;
    }

    // Check AP (attack costs 2 AP)
    int attackCost = 2;
    if (attacker->GetActionPoints() < attackCost) {
        Engine::GetLogger().LogError("CombatSystem: Not enough AP");
        return false;
    }

    // Calculate and apply damage
    int damage = CalculateDamage(attacker, defender,
        attacker->GetStatsComponent()->GetAttackDice(),
        attacker->GetStatsComponent()->GetBaseAttack());
    ApplyDamage(attacker, defender, damage);

    // Consume AP
    attacker->GetActionPointsComponent()->Consume(attackCost);

    // Publish attack event
    //Engine::Instance().GetEventBus().Publish(CharacterAttackedEvent{ attacker, defender, damage });

    return true;
}

int CombatSystem::RollAttackDamage(const std::string& damageDice, int baseDamage) {
    int diceRoll = DiceManager::Instance().RollDiceFromString(damageDice);
    return diceRoll + baseDamage;
}

bool CombatSystem::IsCriticalHit() {
    // Week 4: Implement critical hit system (1d20 >= 18)
    return false;
}

bool CombatSystem::IsInRange(Character* attacker, Character* target, int range) {
    if (attacker == nullptr || target == nullptr) return false;

    int distance = GetDistance(attacker, target);
    return distance <= range;
}

int CombatSystem::GetDistance(Character* char1, Character* char2) {
    if (char1 == nullptr || char2 == nullptr) return -1;

    return GridSystem::Instance().ManhattanDistance(
        char1->GetGridPosition(),
        char2->GetGridPosition()
    );
}