/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  StatsComponent.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 10, 2025
*/

#include "StatsComponent.h"
#include "./Game/DragonicTactics/Types/CharacterTypes.h"
#include <algorithm>

StatsComponent::StatsComponent(const CharacterStats& initial_stats)
    : stats(initial_stats)
{
  
}

void StatsComponent::TakeDamage(int damage) {
    stats.current_hp -= damage;
    stats.current_hp = std::max(0, stats.current_hp);
}

void StatsComponent::Heal(int amount) {
    stats.current_hp += amount;
    stats.current_hp = std::min(stats.max_hp, stats.current_hp);
}

bool StatsComponent::IsAlive() const {
    return stats.current_hp > 0;
}

int StatsComponent::GetCurrentHP() const {
    return stats.current_hp;
}

int StatsComponent::GetMaxHP() const {
    return stats.max_hp;
}

float StatsComponent::GetHealthPercentage() const {
    if (stats.max_hp <= 0) {
        return 0.0f;
    }
    return static_cast<float>(stats.current_hp) / static_cast<float>(stats.max_hp);
}

int StatsComponent::GetBaseAttack() const {
    return stats.base_attack;
}

const std::string& StatsComponent::GetAttackDice() const {
    return stats.attack_dice;
}

int StatsComponent::GetBaseDefend() const {
    return stats.base_defend;
}

const std::string& StatsComponent::GetDefendDice() const {
    return stats.defend_dice;
}

int StatsComponent::GetSpeed() const {
    return stats.speed;
}

int StatsComponent::GetAttackRange() const {
    return stats.attack_range;
}