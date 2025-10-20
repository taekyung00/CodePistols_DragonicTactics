#pragma once
#include"./Engine/Component.h"
#include "./Game/DragonicTactics/Types/CharacterTypes.h"
#include<string>

class StatsComponent : public CS230::Component {
public:
    StatsComponent(const CharacterStats& initial_stats);

    void TakeDamage(int damage);
    void Heal(int amount);
    bool IsAlive() const;
    int GetCurrentHP() const;
    int GetMaxHP() const;
    float GetHealthPercentage() const;


    int GetBaseAttack() const;
    const std::string& GetAttackDice() const;
    int GetBaseDefend() const;
    const std::string& GetDefendDice() const;
    int GetSpeed() const;
    int GetAttackRange() const;

    const CharacterStats& GetAllStats() const { return stats; }

private:
    CharacterStats stats;
};