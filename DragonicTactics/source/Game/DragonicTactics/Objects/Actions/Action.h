/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Action.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 14, 2025
*/

#pragma once
#include <string>
#include "./Engine/Vec2.hpp"

class Character;
namespace CS230 {
    class GameState;
}

class Action {
public:
    virtual ~Action() = default;

    virtual std::string GetName() const = 0;

    virtual int GetActionPointCost() const = 0;
    virtual int GetSpellSlotCost() const = 0;
    virtual int GetSpellLevel() const = 0;

    virtual bool CanExecute(Character* performer, CS230::GameState* context) const = 0;

    virtual void Execute(Character* performer, Character* target, Math::ivec2 tile_position) = 0;
};