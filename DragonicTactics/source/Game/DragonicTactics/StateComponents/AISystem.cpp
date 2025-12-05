/**
 * @file AISystem.cpp
 * @author Sangyun Lee
 * @brief
 * @date 2025-12-04
 */

#include "AISystem.h"
#include "pch.h"

/* have to include characters IA */

#include "AI/FighterStrategy.h"
// #include "AI/WizardStrategy.h" (TODO)

#include "../StateComponents/CombatSystem.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/SpellSystem.h"
#include "../StateComponents/TurnManager.h"
#include "./Engine/GameStateManager.h"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "EventBus.h"

AISystem::AISystem()
{
  Init();
}

AISystem::~AISystem()
{
  for (auto& pair : m_strategies)
  {
	delete pair.second;
  }
  m_strategies.clear();
}

void AISystem::Init()
{
  // [핵심] 캐릭터 타입에 맞는 두뇌를 갈아끼우는 곳
  m_strategies[CharacterTypes::Fighter] = new FighterStrategy();

  // 나중에 이렇게 추가하면 됩니다:
  // m_strategies[CharacterTypes::Wizard] = new WizardStrategy();
  // m_strategies[CharacterTypes::Cleric] = new ClericStrategy();
}

AIDecision AISystem::MakeDecision(Character* actor)
{
  if (!actor)
	return { AIDecisionType::EndTurn, nullptr, {}, "", "Actor is null" };

  CharacterTypes type = actor->GetCharacterType();

  if (m_strategies.find(type) != m_strategies.end())
  {
	return m_strategies[type]->MakeDecision(actor);
  }

  return { AIDecisionType::EndTurn, nullptr, {}, "", "No strategy found" };
}

void AISystem::ExecuteDecision(Character* actor, const AIDecision& decision)
{
  Engine::GetLogger().LogEvent(actor->TypeName() + " AI Decision: " + decision.reasoning);

  auto&			gs			 = Engine::GetGameStateManager();
  GridSystem*	grid		 = gs.GetGSComponent<GridSystem>();
  CombatSystem* combat		 = gs.GetGSComponent<CombatSystem>();
  SpellSystem*	spell_system = gs.GetGSComponent<SpellSystem>();

  bool actionExecuted = false;

  switch (decision.type)
  {
	case AIDecisionType::Move:
	  if (grid && grid->IsWalkable(decision.destination))
	  {
		std::vector<Math::ivec2> path	   = grid->FindPath(actor->GetGridPosition()->Get(), decision.destination);
		MovementComponent*		 move_comp = actor->GetGOComponent<MovementComponent>();
		if (move_comp)
		{
		  move_comp->SetPath(std::move(path));
		  actionExecuted = true;
		}
	  }
	  break;

	case AIDecisionType::Attack:
	  if (combat)
	  {
		combat->ExecuteAttack(actor, decision.target);
		actionExecuted = true;
	  }
	  break;

	case AIDecisionType::UseAbility:
	  if (spell_system)
	  {
		spell_system->CastSpell(actor, decision.abilityName, decision.target->GetGridPosition()->Get());
		actionExecuted = true;
	  }
	  break;

	case AIDecisionType::EndTurn:
	default: actionExecuted = true; break;
  }

  if (auto* eventbus = gs.GetGSComponent<EventBus>())
  {
	eventbus->Publish(AIDecisionEvent{ actor, decision.type, decision.target, decision.reasoning });
  }
}