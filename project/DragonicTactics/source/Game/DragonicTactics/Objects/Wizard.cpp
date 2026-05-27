#include "pch.h"

#include "Wizard.h"
#include "Engine/Engine.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"

Wizard::Wizard(Math::ivec2 start_coordinates)
  : Character(
      CharacterTypes::Wizard, start_coordinates, 1,
      {
        { 1, 4 },
        { 2, 3 }
      })
{
  // 스탯은 CharacterFactory::CreateWizard가 DataRegistry로 초기화
  m_action_list.push_back(new ActionAttack());
  AddGOComponent(new CS230::Sprite("Assets/sprites/wizard.spt", this));
}

void Wizard::OnTurnStart()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - BEGIN");
  Character::OnTurnStart();
}

void Wizard::Action()
{
  if (IsAIControlled())
  {
    AISystem*  ai       = Engine::GetGameStateManager().GetGSComponent<AISystem>();
    AIDecision decision = ai->MakeDecision(this);
    ai->ExecuteDecision(this, decision);
  }
}

bool Wizard::IsAIControlled() const
{
  return true;
}

void Wizard::OnTurnEnd()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " called");
  Character::OnTurnEnd();
}

void Wizard::Update(double dt)
{
  Character::Update(dt);
}

void Wizard::TakeDamage(int damage, Character* attacker)
{
  Character::TakeDamage(damage, attacker);
}
