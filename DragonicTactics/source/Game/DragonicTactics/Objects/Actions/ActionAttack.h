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

    std::string GetName() const override { return "Basic Attack"; }

    int GetActionPointCost() const override { return 1; }
    int GetSpellSlotCost() const override { return 0; }
    int GetSpellLevel() const override { return 0; }

    bool CanExecute(Character* performer, CS230::GameState* context) const override;
    void Execute(Character* performer, Character* target, Math::ivec2 tile_position) override;
};