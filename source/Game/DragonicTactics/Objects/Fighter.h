#pragma once
#include "Character.h"
class Fighter : public Character {
public:
    Fighter(Math::ivec2 start_coordinates);
    std::string TypeName() override { return "Fighter"; }
    void OnTurnStart() override;
    void OnTurnEnd() override;
    void TakeDamage(int damage, Character* attacker) override;
protected:
    void DecideAction() override;
};
