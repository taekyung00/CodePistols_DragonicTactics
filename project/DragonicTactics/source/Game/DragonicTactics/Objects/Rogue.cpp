#include "pch.h"

#include "Rogue.h"
#include "Engine/Engine.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"

Rogue::Rogue(Math::ivec2 start_coordinates)
  : Character(
      CharacterTypes::Rogue, start_coordinates, 1,
      {
        { 1, 2 },
        { 2, 2 }
      })
{
  // 스탯은 CharacterFactory::CreateRogue가 DataRegistry로 초기화
  m_action_list.push_back(new ActionAttack());
  AddGOComponent(new CS230::Sprite("Assets/sprites/rogue.spt", this));
}

void Rogue::OnTurnStart()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - BEGIN");
  Character::OnTurnStart();
}

void Rogue::Action()
{
  if (IsAIControlled())
  {
    AISystem*  ai       = Engine::GetGameStateManager().GetGSComponent<AISystem>();
    AIDecision decision = ai->MakeDecision(this);
    ai->ExecuteDecision(this, decision);
  }
}

bool Rogue::IsAIControlled() const
{
  return true;
}

void Rogue::OnTurnEnd()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " called");
  Character::OnTurnEnd();
}

void Rogue::Update(double dt)
{
  Character::Update(dt);
}

void Rogue::TakeDamage(int damage, Character* attacker)
{
  Character::TakeDamage(damage, attacker);
}
