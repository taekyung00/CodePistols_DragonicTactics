/**
 * \file
 * \author Seungju Song
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Character.h"
#include "./Game/DragonicTactics/StateComponents/AISystem.h"

class Fighter : public Character
{
  public:
  Fighter(Math::ivec2 start_coordinates);

  std::string TypeName() override
  {
	return "Fighter";
  }

  void OnTurnStart() override;
  void Action();
  void OnTurnEnd() override;
  void Update(double dt) override;
  void TakeDamage(int damage, Character* attacker) override;
  bool IsAIControlled() const override;

  protected:
  // void DecideAction() override;
};
