/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  PlayerInputHandler.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/

#include "PlayerInputHandler.h"
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
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/DiceManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/MainMenu.h"

#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"

Math::ivec2 ConvertScreenToGrid(Math::vec2 world_pos)
{
  int grid_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
  int grid_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
  return { grid_x, grid_y };
}

PlayerInputHandler::PlayerInputHandler() : m_state(ActionState::None)
{
}

void PlayerInputHandler::Update(double dt, Character* current_character, GridSystem* grid, CombatSystem* combat_system)
{
  if (current_character->GetCharacterType() != CharacterTypes::Dragon)
  {
	return;
  }

  Dragon* dragon = static_cast<Dragon*>(current_character);

  if (m_state == ActionState::Moving)
  {
	auto move_comp = dragon->GetGOComponent<MovementComponent>();
	if (move_comp && !move_comp->IsMoving())
	{
	  m_state = ActionState::None;

	  // Engine::GetLogger().LogEvent("Dragon finished moving.");
	}
  }

  HandleDragonInput(dt, dragon, grid, combat_system);
}

void PlayerInputHandler::HandleDragonInput([[maybe_unused]] double dt, Dragon* dragon, GridSystem* grid, CombatSystem* combat_system)
{
  auto& input		   = Engine::GetInput();
  bool	is_clicking_ui = ImGui::GetIO().WantCaptureMouse;

  // 1. 이동 모드일 때 마우스 호버 처리
  if (m_state == ActionState::SelectingMove)
  {
	Math::vec2	mouse_pos = input.GetMousePos();
	Math::ivec2 grid_pos  = ConvertScreenToGrid(mouse_pos);

	// 유효한 타일이고 이동 가능한 타일이면 경로 표시
	if (grid->IsValidTile(grid_pos) && grid->IsReachable(grid_pos))
	{
	  grid->SetHoveredTile(grid_pos);
	}
	else
	{
	  grid->ClearHoveredTile();
	}
  }

  // 우클릭 처리
  if (input.MouseJustPressed(2))
  {
	HandleRightClick(dragon);
	return;
  }

  // 좌클릭 처리
  if (input.MouseJustPressed(0) && !is_clicking_ui)
  {
	Math::vec2 mouse_pos = input.GetMousePos();
	HandleMouseClick(mouse_pos, dragon, grid, combat_system);
  }
}

void PlayerInputHandler::HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon, GridSystem* grid, CombatSystem* combat_system)
{
  Math::ivec2 grid_pos = ConvertScreenToGrid(mouse_pos);

  switch (m_state)
  {
	case ActionState::SelectingMove:
	  // 이동 가능한 타일인지 확인
	  if (grid->IsReachable(grid_pos))
	  {
		// A* 경로 찾기 (이미 계산된 경로 사용 가능)
		auto path = grid->FindPath(dragon->GetGridPosition()->Get(), grid_pos);

		if (!path.empty())
		{
		  dragon->SetPath(std::move(path));
		  m_state = ActionState::Moving;

		  Engine::GetLogger().LogEvent("Dragon moving to (" + std::to_string(grid_pos.x) + ", " + std::to_string(grid_pos.y) + ")");

		  // 이동 모드 비활성화
		  grid->DisableMovementMode();
		}
	  }
	  else
	  {
		Engine::GetLogger().LogDebug("Cannot move to (" + std::to_string(grid_pos.x) + ", " + std::to_string(grid_pos.y) + "): Not reachable");
	  }
	  break;

	case ActionState::TargetingForAttack:
	  {
		Character* target = grid->GetCharacterAt(grid_pos);
		if (target && target != dragon)
		{
		  if (combat_system)
		  {
			combat_system->ExecuteAttack(dragon, target);
		  }
		  m_state = ActionState::None;
		}
	  }
	  break;

	case ActionState::Moving:
	  {
		MovementComponent* move_comp = dragon->GetGOComponent<MovementComponent>();
		if (move_comp != nullptr && !move_comp->IsMoving())
		{
		  Engine::GetLogger().LogEvent("Movement finished. State returning to None.");
		  m_state = ActionState::None;
		}
	  }
	  break;

	case ActionState::SelectingAction:
	  // 행동 선택 중에는 클릭 무시
	  break;

	case ActionState::TargetingForSpell:
	  // 스펠 타겟 선택 (미구현)
	  break;

	case ActionState::None: break;
  }
  Engine::GetLogger().LogDebug("Mouse: " + std::to_string(mouse_pos.x) + ", " + std::to_string(mouse_pos.y));
  Engine::GetLogger().LogDebug("Grid Result: " + std::to_string(grid_pos.x) + ", " + std::to_string(grid_pos.y));
}

void PlayerInputHandler::HandleRightClick(Dragon* dragon)
{
  // 이동 모드 비활성화
  if (m_state == ActionState::SelectingMove)
  {
	auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	Engine::GetLogger().LogEvent("Movement mode cancelled by right-click");
	grid->DisableMovementMode();	
  }

  if (m_state == ActionState::Moving)
  {
	dragon->GetGOComponent<MovementComponent>()->ClearPath();
  }

  m_state = ActionState::None;
}

void PlayerInputHandler::CancelCurrentAction()
{
  // 이동 모드였다면 GridSystem에도 알려야 함
  if (m_state == ActionState::SelectingMove)
  {
	Engine::GetLogger().LogEvent("Movement mode cancelled");
	// GridSystem::DisableMovementMode() 호출 필요
	// GamePlay에서 처리하도록 수정
  }

  m_state = ActionState::None;
}
