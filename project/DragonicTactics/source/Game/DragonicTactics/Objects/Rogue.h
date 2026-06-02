#pragma once
#include "Character.h"
#include "./Game/DragonicTactics/StateComponents/AISystem.h"

class Rogue : public Character
{
public:
  //void Draw(Math::TransformationMatrix camera_matrix, unsigned int color = 0xFFFFFFFF, float depth = DrawDepth::CHARACTER) override;
  Rogue(Math::ivec2 start_coordinates);

  std::string TypeName() override
  {
    return "Rogue";
  }

  void OnTurnStart() override;
  void Action();
  void OnTurnEnd() override;
  void Update(double dt) override;
  void TakeDamage(int damage, Character* attacker) override;
  bool IsAIControlled() const override;
};
