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
#include "Engine/Timer.h"
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

void BattleOrchestrator::Update([[maybe_unused]] double dt, TurnManager* turn_manager, AISystem* ai_system)
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
	HandleAITurn(current, turn_manager, ai_system);
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


void BattleOrchestrator::HandleAITurn(Character* ai_character, TurnManager* turn_manager, AISystem* ai_system)
{
  // 1. 캐릭터가 이동 중이거나 애니메이션 중이라면 대기 (기존 유지)
  MovementComponent* move_comp = ai_character->GetGOComponent<MovementComponent>();
  if (move_comp && move_comp->IsMoving())
  {
	return;
  }

  auto timer = Engine::GetGameStateManager().GetGSComponent<util::Timer>();
  timer->ResetTimeStamp();
  while (timer->GetElapsedSeconds() < 0.4)
  {
  }

  // 2. AISystem에게 "지금 뭐 할래?"라고 물어봅니다. (전략 패턴 활용)
  // 기존의 fighter->Action() 대신 시스템을 직접 이용합니다.
  AIDecision decision = ai_system->MakeDecision(ai_character);

  // 3. 결정에 따른 분기 처리
  if (decision.type == AIDecisionType::EndTurn)
  {
	// AI가 "턴 종료"를 선언했으면 턴을 넘깁니다.
	Engine::GetLogger().LogEvent(ai_character->TypeName() + " ends turn. Reason: " + decision.reasoning);
	turn_manager->EndCurrentTurn();
  }
  else
  {
	// 이동, 공격, 스킬 등의 행동을 실행합니다.
	// 실행 후에는 함수를 빠져나가고, 다음 Update 프레임에 다시 들어와서
	// AI가 또 다른 행동(예: 이동 후 공격)을 할지 다시 MakeDecision을 통해 확인하게 됩니다.
	ai_system->ExecuteDecision(ai_character, decision);
  }
}

bool BattleOrchestrator::CheckVictoryCondition()
{
  return false;
}
