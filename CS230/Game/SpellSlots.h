/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  SpellSlots.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 12, 2025
*/

#pragma once
#include "../Engine/Component.h"
#include <map>

class SpellSlots : public CS230::Component {
public:
    // 생성자: 레벨별 최대 슬롯 개수를 map 형태로 받아서 초기화합니다.
    // 예: {{1, 4}, {2, 3}} -> 1레벨 4개, 2레벨 3개
    SpellSlots(const std::map<int, int>& max_slots_per_level);

    // 주문 슬롯을 소모합니다. 성공하면 true, 슬롯이 부족하면 false를 반환합니다.
    bool Consume(int level);

    // 해당 레벨의 주문 슬롯이 하나 이상 남아있는지 확인합니다.
    bool HasSlot(int level) const;

    // 특정 레벨 이하의 모든 주문 슬롯을 최대치로 회복합니다. (드래곤의 '열 방출' 스킬용)
    void Recover(int max_level_to_recover);

    // 이 컴포넌트는 스스로 업데이트할 로직이 없으므로 Update 함수는 비워둡니다.
    void Update(double dt) override {}

private:
    // Key: 주문 레벨, Value: 최대 슬롯 개수
    std::map<int, int> max_slots;
    // Key: 주문 레벨, Value: 현재 남은 슬롯 개수
    std::map<int, int> current_slots;
};