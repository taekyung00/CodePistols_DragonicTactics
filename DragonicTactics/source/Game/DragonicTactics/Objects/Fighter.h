#pragma once
#include "Character.h"
// Ginam: Week3 Developer B - Fighter combat abilities
#include "../Abilities/MeleeAttack.h"
#include "../Abilities/ShieldBash.h"
#include "./Game/DragonicTactics/Singletons/AISystem.h"

class Fighter : public Character {
public:
    Fighter(Math::ivec2 start_coordinates);
    std::string TypeName() override { return "Fighter"; }
    void OnTurnStart() override;
    void Action();
    void OnTurnEnd() override;
    void Update(double dt) override;
    void TakeDamage(int damage, Character* attacker) override;
    bool IsAIControlled() const;
        // Ginam: Fighter ability methods (Week3 Developer B)
    AbilityResult PerformMeleeAttack(Character* target);  // Ginam: basic melee attack
    AbilityResult PerformShieldBash(Character* target);   // Ginam: special ability with knockback
protected:
    //void DecideAction() override;
    
private:
    // Ginam: Fighter abilities (Week3 Developer B)
    MeleeAttack meleeAttack;  // Ginam: basic attack ability
    ShieldBash shieldBash;    // Ginam: special knockback ability
};
