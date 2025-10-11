/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ActionPoints.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 12, 2025
*/

#include "ActionPoints.h"

ActionPoints::ActionPoints(int max_points)
    : max_points(max_points), current_points(max_points)
{
    // 생성 시점에는 현재 행동력이 최대치와 같도록 초기화합니다.
}

void ActionPoints::Refresh() {
    current_points = max_points;
}

bool ActionPoints::Consume(int amount) {
    if (HasEnough(amount)) {
        current_points -= amount;
        return true;
    }
    return false;
}

int ActionPoints::GetCurrentPoints() const {
    return current_points;
}

int ActionPoints::GetMaxPoints() const {
    return max_points;
}

bool ActionPoints::HasEnough(int amount) const {
    return current_points >= amount;
}