/**
 * @file Wizard.h
 * @brief 위저드 캐릭터 (AI 제어, 원거리 마법 공격 / Sweet Spot 포지셔닝)
 */
#pragma once
#include "Character.h"
#include "./Game/DragonicTactics/StateComponents/AISystem.h"

class Wizard : public Character
{
  public:
  Wizard(Math::ivec2 start_coordinates);

  std::string TypeName() override
  {
    return "Wizard";
  }

  void OnTurnStart() override;
  void Action();
  void OnTurnEnd() override;
  void Update(double dt) override;
  void TakeDamage(int damage, Character* attacker) override;
  bool IsAIControlled() const override;
};
