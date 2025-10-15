/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  SpellSlots.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 12, 2025
*/

#include "SpellSlots.h"

SpellSlots::SpellSlots(const std::map<int, int>& max_slots_per_level)
    : max_slots(max_slots_per_level), current_slots(max_slots_per_level)
{
    // 생성 시점에는 현재 슬롯이 최대치와 같도록 초기화합니다.
}

bool SpellSlots::Consume(int level) {
    if (HasSlot(level)) {
        current_slots[level]--;
        return true;
    }
    // TODO: GDD에 있는 '업캐스팅' 규칙(낮은 레벨 주문을 높은 레벨 슬롯으로 사용)을
    // 여기에 구현할 수 있습니다. 예를 들어, 1레벨 슬롯이 없으면 2레벨 슬롯을 찾아서 소모합니다.
    return false;
}

bool SpellSlots::HasSlot(int level) const {
    // 맵에서 해당 레벨을 찾습니다.
    auto it = current_slots.find(level);
    if (it != current_slots.end()) {
        // 해당 레벨의 슬롯이 1개 이상 남아있는지 확인합니다.
        return it->second > 0;
    }
    // 해당 레벨의 슬롯 자체가 존재하지 않으면 false를 반환합니다.
    return false;
}

void SpellSlots::Recover(int max_level_to_recover) {
    for (auto& pair : current_slots) {
        int level = pair.first;
        if (level <= max_level_to_recover) {
            // 해당 레벨의 현재 슬롯을 최대치로 복구합니다.
            current_slots[level] = max_slots[level];
        }
    }
}