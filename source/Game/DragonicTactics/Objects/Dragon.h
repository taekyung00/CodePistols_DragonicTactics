#pragma once
#include "Character.h"

class Dragon : public Character {
public:
    Dragon(Math::ivec2 start_coordinates);

    std::string TypeName() override { return "Dragon"; }
    void OnTurnStart() override;
    void OnTurnEnd() override;
    void TakeDamage(int damage, Character* attacker) override;

protected:
    void DecideAction() override;
};
