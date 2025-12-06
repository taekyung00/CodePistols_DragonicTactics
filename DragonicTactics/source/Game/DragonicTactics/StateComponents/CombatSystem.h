<<<<<<< HEAD
﻿#pragma once
#include "./Engine/Component.h"
=======
#pragma once
>>>>>>> main
#include "../Objects/Character.h"
#include "./Engine/Component.h"
#include <string>

class DiceManager;

enum class DamageType
{
  Physical,
  Fire,
  Cold,
  Lightning,
  Poison
};

class CombatSystem : public CS230::Component
{
  public:
  CombatSystem()  = default;
  ~CombatSystem() = default;

  // Set DiceManager for testing or runtime use
  void SetDiceManager(DiceManager* dice)
  {
	diceManager = dice;
  }

  // Core combat methods
  int  CalculateDamage(Character* attacker, Character* defender, const std::string& damageDice, int baseDamage);
  void ApplyDamage(Character* attacker, Character* defender, int damage);

  // Attack execution
  bool ExecuteAttack(Character* attacker, Character* defender);

  // Damage calculation helpers
  int  RollAttackDamage(const std::string& damageDice, int baseDamage);
  bool IsCriticalHit(); // Week 4: critical hit system

  // Range checking
  bool IsInRange(Character* attacker, Character* target, int range);
  int  GetDistance(Character* char1, Character* char2);

<<<<<<< HEAD
private:
    DiceManager* diceManager = nullptr;
};
=======
  private:
  DiceManager* diceManager = nullptr;
};
>>>>>>> main
