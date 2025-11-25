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

//int SpellSlots::GetMaxLevel() const
//{
//	return static_cast<int>(max_slots.size());
//}

////////ADD////////
int SpellSlots::GetSpellSlotCount(int level) {
    return current_slots[level];
}

void SpellSlots::SetSpellSlots(std::map<int, int> spellSlots) {
	for (const auto& pair : spellSlots)
	{
		current_slots[pair.first] = pair.second;
	}
}
///////////////////

