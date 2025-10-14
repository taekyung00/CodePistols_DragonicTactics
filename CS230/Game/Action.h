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
#include "Vec2.h"

// --- 전방 선언 (Forward Declarations) ---
class Character;
namespace CS230 {
    class GameState;
}

class Action {
public:
    virtual ~Action() = default;

    // 이 액션의 이름 (예: "기본 공격", "화염탄") - UI나 로그에 사용됩니다.
    virtual std::string GetName() const = 0;

    // 이 액션을 수행하는 데 필요한 자원을 반환합니다.
    virtual int GetActionPointCost() const = 0;
    virtual int GetSpellSlotCost() const = 0;
    virtual int GetSpellLevel() const = 0;

    // 현재 이 액션을 사용할 수 있는지 확인합니다 (자원, 사거리, 조건 등).
    // AI의 의사 결정이나, 플레이어 UI에서 스킬을 비활성화할 때 사용됩니다.
    virtual bool CanExecute(Character* performer, CS230::GameState* context) const = 0;

    // 이 액션을 실제로 실행합니다.
    virtual void Execute(Character* performer, Character* target, Math::ivec2 tile_position) = 0;
};