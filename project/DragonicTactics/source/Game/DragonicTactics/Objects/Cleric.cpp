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
      CharacterTypes::Cleric, start_coordinates, 2,
      {
        { 1, 3 },
        { 2, 2 }
      })
{
  CharacterStats cleric_stats;
  cleric_stats.max_hp       = 75;
  cleric_stats.current_hp   = 75;
  cleric_stats.speed        = 3;
  cleric_stats.base_attack  = 4;
  cleric_stats.attack_dice  = "1d6";
  cleric_stats.base_defend  = 2;
  cleric_stats.defend_dice  = "1d8";
  cleric_stats.attack_range = 1;

  *GetStatsComponent() = StatsComponent(cleric_stats);
  m_action_list.push_back(new ActionAttack());
  // cleric.spt 미존재 — 임시로 fighter.spt 사용
  AddGOComponent(new CS230::Sprite("Assets/sprites/fighter.spt", this));
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
