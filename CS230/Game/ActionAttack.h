/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ActionAttack.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 15, 2025
*/

#pragma once
#include "Action.h"

class ActionAttack : public Action {
public:
    // Action의 순수 가상 함수들을 모두 구현(override)합니다.
    std::string GetName() const override { return "Basic Attack"; }

    int GetActionPointCost() const override { return 1; }
    int GetSpellSlotCost() const override { return 0; } // 기본 공격은 주문 슬롯을 사용하지 않음
    int GetSpellLevel() const override { return 0; }

    bool CanExecute(Character* performer, CS230::GameState* context) const override;
    void Execute(Character* performer, Character* target, Math::ivec2 tile_position) override;
};