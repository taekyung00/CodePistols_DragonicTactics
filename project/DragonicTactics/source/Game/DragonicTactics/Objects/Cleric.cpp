#include "pch.h"

#include "Cleric.h"
#include "Engine/Engine.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"

Cleric::Cleric(Math::ivec2 start_coordinates)
  : Character(
      CharacterTypes::Cleric, start_coordinates, 1,
      {
        { 1, 3 },
        { 2, 2 }
      })
{
  // 스탯은 CharacterFactory::CreateCleric이 DataRegistry로 초기화
  m_action_list.push_back(new ActionAttack());
  AddGOComponent(new CS230::Sprite("Assets/sprites/cleric.spt", this));
}

void Cleric::OnTurnStart()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - BEGIN");
  Character::OnTurnStart();
}

void Cleric::Action()
{
  if (IsAIControlled())
  {
    AISystem*  ai       = Engine::GetGameStateManager().GetGSComponent<AISystem>();
    AIDecision decision = ai->MakeDecision(this);
    ai->ExecuteDecision(this, decision);
  }
}

bool Cleric::IsAIControlled() const
{
  return true;
}

void Cleric::OnTurnEnd()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " called");
  Character::OnTurnEnd();
}

void Cleric::Update(double dt)
{
  Character::Update(dt);
}

void Cleric::TakeDamage(int damage, Character* attacker)
{
  Character::TakeDamage(damage, attacker);
}
