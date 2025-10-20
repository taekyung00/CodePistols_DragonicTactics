#pragma once
#include "./Engine/Component.h"
#include <map>
class SpellSlots : public CS230::Component {
public:
    SpellSlots(const std::map<int, int>& max_slots_per_level);
    bool Consume(int level);
    bool HasSlot(int level) const;
    void Recover(int max_level_to_recover);
    void Update([[maybe_unused]] double dt) override {}
private:
    std::map<int, int> max_slots;
    std::map<int, int> current_slots;
};