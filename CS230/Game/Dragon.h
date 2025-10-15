#pragma once
/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Dragon.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 15, 2025
*/

#pragma once
#include "Character.h"

class Dragon : public Character {
public:
    Dragon(Math::ivec2 start_coordinates);

    // Character로부터 상속받은 순수 가상 함수들을 구현합니다.
    std::string TypeName() override { return "Dragon"; }
    void OnTurnStart() override;
    void OnTurnEnd() override;

    // 드래곤만의 고유한 피격 로직을 위해 TakeDamage를 오버라이드합니다.
    void TakeDamage(int damage, Character* attacker) override;

protected:
    // 드래곤의 AI 로직을 구현합니다.
    void DecideAction() override;
};