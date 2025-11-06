#include "SpellSlots.h"

SpellSlots::SpellSlots(const std::map<int, int>& max_slots_per_level)
    : max_slots(max_slots_per_level), current_slots(max_slots_per_level)
{
}

bool SpellSlots::Consume(int level) {
    if (HasSlot(level)) {
        current_slots[level]--;
        return true;
    }
    return false;
}

bool SpellSlots::HasSlot(int level) const {
    auto it = current_slots.find(level);
    if (it != current_slots.end()) {
        return it->second > 0;
    }
    return false;
}

void SpellSlots::Recover(int max_level_to_recover) {
    for (auto& pair : current_slots) {
        int level = pair.first;
        if (level <= max_level_to_recover) {
            current_slots[level] = max_slots[level];
        }
    }
}
