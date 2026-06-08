/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  BattleOrchestrator.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/

#include "BattleOrchestrator.h"
#include "Game/DragonicTactics/Types/GameTimings.h"
#include "./CS200/IRenderer2D.h"
#include "./CS200/NDC.h"
#include "GamePlay.h"
#include "pch.h"


#include "../Debugger/DebugManager.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/StateComponents/AISystem.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/DiceManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/MainMenu.h"

#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"

void BattleOrchestrator::Update(double dt, TurnManager* turn_manager, AISystem* ai_system)
{
  if (!turn_manager->IsCombatActive())
	return;

  Character* current	   = turn_manager->GetCurrentCharacter();
  int		 current_round = turn_manager->GetRoundNumber();
  if (current_round != m_previous_round)
  {
	m_previous_round = current_round;
  }

  if (current->GetCharacterType() != CharacterTypes::Dragon)
  {
	HandleAITurn(current, turn_manager, ai_system, dt);
  }
}

// bool BattleOrchestrator::ShouldContinueTurn(Character* current_character, AISystem* ai_system, CS230::GameObjectManager* go_manager) {
//     Character* target = nullptr;

//     for (const auto& obj_ptr : go_manager->GetAll()) {
//         CS230::GameObject* obj = obj_ptr.get();

//         if (obj->Type() == GameObjectTypes::Character) {
//             Character* character = static_cast<Character*>(obj);

//             if (character->GetCharacterType() == CharacterTypes::Dragon) {
//                 target = character;
//                 break;
//             }
//         }
//     }

//     if (!target) return false;

//     bool shouldAttack = ai_system->ShouldAttack(current_character, target);
//     bool shouldMove   = ai_system->ShouldMoveCloser(current_character, target);

//     return shouldAttack || shouldMove;
// }


void BattleOrchestrator::HandleAITurn(Character* ai_character, TurnManager* turn_manager, AISystem* ai_system, double dt)
{
  // 0. 새 AI 캐릭터 턴 시작 감지 → "생각 중" 딜레이
  if (ai_character != m_last_ai_character_)
  {
	m_last_ai_character_ = ai_character;
	m_think_timer_       = GameTimings::AI_THINK;
  }

  if (m_think_timer_ > 0.0)
  {
	m_think_timer_ -= dt;
	return;
  }

  // 1. 이동 애니메이션 완료 대기
  MovementComponent* move_comp = ai_character->GetGOComponent<MovementComponent>();
  if (move_comp && move_comp->IsMoving())
	return;

  // 2. 행동 후 비차단 대기 (busy-wait 대체 — 게임 루프를 멈추지 않음)
  if (m_wait_timer > 0.0)
  {
	m_wait_timer -= dt;
	return;
  }

  // 3. AI 결정
  AIDecision decision = ai_system->MakeDecision(ai_character);

  // 4. 결정에 따른 분기 처리
  if (decision.type == AIDecisionType::EndTurn)
  {
	Engine::GetLogger().LogEvent(ai_character->TypeName() + " ends turn. Reason: " + decision.reasoning);
	turn_manager->EndCurrentTurn();
  }
  else
  {
	// 실행 후 다음 프레임에 다시 HandleAITurn 진입 → MakeDecision 반복
	ai_system->ExecuteDecision(ai_character, decision);

	if (decision.type == AIDecisionType::UseAbility)
	{
	  // Fire/MagicMissile/Meteor SFX 스펠은 0.5초 추가 대기 (애니메이션 길이 연장과 동기화)
	  const std::string& id = decision.abilityName;
	  bool slow_spell = (id == "S_ATK_010" || id == "S_ATK_030" ||
	                     id == "S_ATK_060" || id == "S_ATK_040");
	  m_wait_timer = GameTimings::AI_WAIT_SPELL + (slow_spell ? GameTimings::SLOW_SPELL_EXTRA : 0.0);
	}
	else if (decision.type == AIDecisionType::Attack)
	  m_wait_timer = GameTimings::AI_WAIT_ATTACK;
	else
	  m_wait_timer = GameTimings::AI_WAIT_MOVE;
  }
}

bool BattleOrchestrator::CheckVictoryCondition()
{
  return false;
}
