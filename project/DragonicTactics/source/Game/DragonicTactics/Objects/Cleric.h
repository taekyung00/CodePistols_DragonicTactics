/**
 * @file Cleric.h
 * @brief 클레릭 캐릭터 (AI 제어, 힐/버프/디버프 지원)
 */
#pragma once
#include "Character.h"
#include "./Game/DragonicTactics/StateComponents/AISystem.h"

class Cleric : public Character
{
  public:
  Cleric(Math::ivec2 start_coordinates);

  std::string TypeName() override
  {
    return "Cleric";
  }

  void OnTurnStart() override;
  void Action();
  void OnTurnEnd() override;
  void Update(double dt) override;
  void TakeDamage(int damage, Character* attacker) override;
  bool IsAIControlled() const;
};
