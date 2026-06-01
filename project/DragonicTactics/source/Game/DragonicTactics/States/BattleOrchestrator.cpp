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

	// 스펠은 SpellDelayObject가 0.5s 후 효과 적용 → 0.6s 대기로 상태 반영 보장
	// 공격은 AttackDelayObject(0.3s) + 이펙트 확인 여유(0.3s) = 0.6s
	// 이동은 시각적 간격 0.3s
	if (decision.type == AIDecisionType::UseAbility)
	  m_wait_timer = 0.6;
	else if (decision.type == AIDecisionType::Attack)
	  m_wait_timer = 0.6;
	else
	  m_wait_timer = 0.3;
  }
}

bool BattleOrchestrator::CheckVictoryCondition()
{
  return false;
}
