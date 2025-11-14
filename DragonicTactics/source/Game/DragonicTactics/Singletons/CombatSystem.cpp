#include "CombatSystem.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/GameStateManager.hpp"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "./Game/DragonicTactics/StateComponents/GridSystem.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h" 
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "../Types/Events.h"
//#include "../SpellSlots.h"

int CombatSystem::CalculateDamage(Character* attacker, Character* defender,
    const std::string& damageDice, int baseDamage)
{
    if (attacker == nullptr || defender == nullptr) {
        Engine::GetLogger().LogError("CombatSystem: Null attacker or defender");
        return 0;
    }

    // Roll attack dice
    int diceRoll = Engine::GetDiceManager().RollDiceFromString(damageDice);
    int totalDamage = diceRoll + baseDamage;

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
        Engine::Instance().GetEventBus().Publish(CharacterDeathEvent{ defender, attacker });
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
    // Engine::Instance().GetEventBus().Publish(CharacterAttackedEvent{ attacker, defender, damage });

    return true;
}

int CombatSystem::RollAttackDamage(const std::string& damageDice, int baseDamage) {
    int diceRoll = Engine::GetDiceManager().RollDiceFromString(damageDice);
    return diceRoll + baseDamage;
}

bool CombatSystem::IsCriticalHit() {
    return false;
}

bool CombatSystem::IsInRange(Character* attacker, Character* target, int range) {
    if (attacker == nullptr || target == nullptr) return false;

    int distance = GetDistance(attacker, target);
    return distance <= range;
}

int CombatSystem::GetDistance(Character* char1, Character* char2) {
    if (char1 == nullptr || char2 == nullptr) return -1;

    auto sub = char1->GetGridPosition()->Get() - char2->GetGridPosition()->Get();
    if(sub.x < 0) sub.x = -sub.x;
    if(sub.y < 0) sub.y = -sub.y;

    return sub.x + sub.y;
}