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
#include "Character.h"
#include "StatsComponent.h"
#include "ActionPoints.h"
#include "GridPosition.h"
// #include "DamageCalculator.h" // 나중에 데미지 계산기가 만들어지면 포함

bool ActionAttack::CanExecute(Character* performer, [[maybe_unused]]CS230::GameState* context) const {
    if (performer == nullptr) return false;

    // 1. 행동력이 충분한가?
    ActionPoints* ap = performer->GetActionPointsComponent();
    if (ap == nullptr || ap->HasEnough(GetActionPointCost()) == false) {
        return false;
    }

    // TODO: 2. 목표가 사거리 안에 있는가?
    // 이 부분은 TargetingSystem이나 Grid 시스템의 도움이 필요합니다.

    return true;
}

void ActionAttack::Execute(Character* performer, Character* target, [[maybe_unused]]Math::ivec2 tile_position) {
    if (performer == nullptr || target == nullptr) return;

    // 1. 자원을 소모합니다.
    performer->GetActionPointsComponent()->Consume(GetActionPointCost());

    // 2. 피해량을 계산합니다.
    // TODO: 지금은 임시로 기본 공격력만 사용합니다.
    //       나중에 DamageCalculator를 만들어 "2d6" 같은 주사위 규칙을 처리해야 합니다.
    int damage = performer->GetStatsComponent()->GetBaseAttack();

    // 3. 목표에게 피해를 적용합니다.
    target->TakeDamage(damage, performer);
}