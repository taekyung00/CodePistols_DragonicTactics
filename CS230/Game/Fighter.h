/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Fighter.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 12, 2025
*/


#pragma once
#include "Character.h"

class Fighter : public Character {
public:
    Fighter(Math::ivec2 start_coordinates);

    // Character로부터 상속받은 순수 가상 함수들을 구현합니다.
    std::string TypeName() override { return "Fighter"; }
    void OnTurnStart() override;
    void OnTurnEnd() override;

    // Character의 TakeDamage를 오버라이드하여 파이터만의 로직을 추가할 수 있습니다.
    void TakeDamage(int damage, Character* attacker) override;

protected:
    // 파이터의 AI 로직("두뇌")을 구현합니다.
    void DecideAction() override;
};