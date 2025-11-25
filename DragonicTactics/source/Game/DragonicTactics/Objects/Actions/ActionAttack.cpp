/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ActionAttack.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 15, 2025
*/

#include "ActionAttack.h"
#include "../Character.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
// #include "DamageCalculator.h"

bool ActionAttack::CanExecute(Character* performer, [[maybe_unused]]CS230::GameState* context) const {
    if (performer == nullptr) return false;

    ActionPoints* ap = performer->GetActionPointsComponent();
    if (ap == nullptr || ap->HasEnough(GetActionPointCost()) == false) {
        return false;
    }

    // TODO

    return true;
}

void ActionAttack::Execute(Character* performer, Character* target, [[maybe_unused]]Math::ivec2 tile_position) {
    if (performer == nullptr || target == nullptr) return;

    performer->GetActionPointsComponent()->Consume(GetActionPointCost());

    int damage = performer->GetStatsComponent()->GetBaseAttack();

    target->TakeDamage(damage, performer);
}