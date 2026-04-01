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
#include "ButtonManager.h"
#include "GamePlayUIManager.h"

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

// helper function to convert screen coordinates to grid coordinates
Math::ivec2 PlayerInputHandler::ConvertScreenToGrid(Math::vec2 world_pos)
{
  int grid_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
  int grid_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
  return { grid_x, grid_y };
}

PlayerInputHandler::PlayerInputHandler() : m_state(ActionState::None)
{
}

// PlayerInputHandler.cpp — 파일 내 static 헬퍼
static void HideAllSpellButtons(ButtonManager& btns)
{
    btns.SetVisible("btn_spell_cancel", false);

    btns.SetVisible("S_ATK_020", false);
    btns.SetVisible("S_ENH_050", false);
    btns.SetVisible("S_DEB_020", false);

    for (int lv = 1; lv <= 5; ++lv)
        btns.SetVisible("S_ATK_010_lv" + std::to_string(lv), false);

    for (int lv = 0; lv <= 5; ++lv)
        btns.SetVisible("S_ENH_040_lv" + std::to_string(lv), false);

    for (int lv = 3; lv <= 5; ++lv)
        btns.SetVisible("S_ATK_030_lv" + std::to_string(lv), false);

    for (int lv = 3; lv <= 5; ++lv)
        btns.SetVisible("S_ATK_040_lv" + std::to_string(lv), false);

    for (int lv = 2; lv <= 5; ++lv)
        btns.SetVisible("S_GEO_010_lv" + std::to_string(lv), false);

    for (int lv = 1; lv <= 5; ++lv)
        btns.SetVisible("S_GEO_020_lv" + std::to_string(lv), false);

    for (int lv = 0; lv <= 5; ++lv)
        btns.SetVisible("S_GEO_030_lv" + std::to_string(lv), false);
}

void PlayerInputHandler::Update(double dt, Character* current_character, GridSystem* grid, CombatSystem* combat_system, ButtonManager& btns)
{
 
    // Move 버튼 클릭
    if (btns.IsPressed("btn_move"))
    {
        if (m_state == ActionState::SelectingMove)
        {
            CancelCurrentAction();
            grid->DisableMovementMode();
        }
        else
        {
            SetState(ActionState::SelectingMove);
            // 이동 가능 타일 계산
            grid->EnableMovementMode(current_character->GetGridPosition()->Get(),
                                      current_character->GetMovementRange());
        }
    }

    // Action 버튼 클릭
    if (btns.IsPressed("btn_action"))
    {
        if (m_state == ActionState::SelectingAction)
        {
            CancelCurrentAction();
            btns.SetVisible("btn_attack", false);
            btns.SetVisible("btn_spell", false);
        }
        else
        {
            SetState(ActionState::SelectingAction);
            btns.SetVisible("btn_attack", true);
            btns.SetVisible("btn_spell", true);
        }
    }

    // Attack 서브 버튼
    if (btns.IsPressed("btn_attack"))
    {
        SetState(ActionState::TargetingForAttack);
        btns.SetVisible("btn_attack", false);
        btns.SetVisible("btn_spell", false);
    }

      // ── Spell 서브 버튼 ───────────────────────────────────────────
    if (btns.IsPressed("btn_spell"))
    {
        SetState(ActionState::SelectingSpell);
        btns.SetVisible("btn_attack", false);
        btns.SetVisible("btn_spell",  false);

        // 스펠 목록 & Cancel 표시
        btns.SetVisible("btn_spell_cancel", true);

        // 비업캐스트
        btns.SetVisible("S_ATK_020", true);  // Tail Swipe
        btns.SetVisible("S_ENH_050", true);  // Purify
        btns.SetVisible("S_DEB_020", true);  // Fearful Cry

        // 업캐스트: Fire Bolt Lv1~5
        for (int lv = 1; lv <= 5; ++lv)
            btns.SetVisible("S_ATK_010_lv" + std::to_string(lv), true);

        // 업캐스트: Mana Conversion Lv0~5
        for (int lv = 0; lv <= 5; ++lv)
            btns.SetVisible("S_ENH_040_lv" + std::to_string(lv), true);

        // 업캐스트: Dragon's Fury Lv3~5
        for (int lv = 3; lv <= 5; ++lv)
            btns.SetVisible("S_ATK_030_lv" + std::to_string(lv), true);

        // 업캐스트: Meteor Lv3~5
        for (int lv = 3; lv <= 5; ++lv)
            btns.SetVisible("S_ATK_040_lv" + std::to_string(lv), true);

        // 업캐스트: Magma Blast Lv2~5
        for (int lv = 2; lv <= 5; ++lv)
            btns.SetVisible("S_GEO_010_lv" + std::to_string(lv), true);

        // 업캐스트: Wall Creation Lv1~5
        for (int lv = 1; lv <= 5; ++lv)
            btns.SetVisible("S_GEO_020_lv" + std::to_string(lv), true);

        // 업캐스트: Teleport Lv0~5
        for (int lv = 0; lv <= 5; ++lv)
            btns.SetVisible("S_GEO_030_lv" + std::to_string(lv), true);
    }

    // ── Spell Cancel 버튼 ─────────────────────────────────────────
    if (btns.IsPressed("btn_spell_cancel"))
    {
        CancelCurrentAction();
        HideAllSpellButtons(btns);  // 아래 헬퍼 함수 참고
    }

    // End Turn 버튼
    if (btns.IsPressed("btn_end_turn"))
    {
        TurnManager* tm = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
        if (tm) tm->EndCurrentTurn();
    }

    // 버튼 비활성화 상태 갱신
    bool in_action = (m_state != ActionState::None);
    btns.SetDisabled("btn_move", in_action && m_state != ActionState::SelectingMove);
    btns.SetDisabled("btn_action", in_action && m_state != ActionState::SelectingAction);
    btns.SetDisabled("btn_end_turn", in_action);

    // Move/Action 버튼 레이블 갱신 (Cancel 표시)
    btns.SetLabel("btn_move",   (m_state == ActionState::SelectingMove)   ? "Cancel Move"   : "Move");
    btns.SetLabel("btn_action", (m_state == ActionState::SelectingAction) ? "Cancel Action" : "Action");

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
	case ActionState::SelectingSpell:
	  // 스펠 목록 선택 중에는 타일 클릭 무시
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
	  {
		Math::ivec2	 clicked_tile = ConvertScreenToGrid(mouse_pos);
		SpellSystem* spell_sys	  = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();

		if (spell_sys && spell_sys->CanCast(dragon, m_selected_spell_id,
                                         clicked_tile, m_selected_upcast_level))
		{
		  spell_sys->CastSpell(dragon, m_selected_spell_id, clicked_tile, m_selected_upcast_level);
		  m_state = ActionState::None;
		  if (grid) grid->DisableSpellTargetingMode();
		}
		else
		{
		  Engine::GetLogger().LogDebug("Cannot cast " + m_selected_spell_id + " at (" + std::to_string(clicked_tile.x) + ", " + std::to_string(clicked_tile.y) + ")");
		}
	  }
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

  if (m_state == ActionState::TargetingForSpell)
{
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (grid) grid->DisableSpellTargetingMode();
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

void PlayerInputHandler::SelectSpell(const std::string& spell_id, Character* caster, int upcast_level, ButtonManager& btns)
{
    m_selected_spell_id = spell_id;
	m_selected_upcast_level = upcast_level;
    m_state = ActionState::TargetingForSpell;

    HideAllSpellButtons(btns);  // ★ 스펠 선택 완료 → 목록 닫기

    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto* grid      = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (spell_sys && grid && caster)
    {
        const SpellData* spell = spell_sys->GetSpellData(spell_id);
        if (spell)
            grid->EnableSpellTargetingMode(
                caster->GetGridPosition()->Get(),
                spell->targeting.geometry,   // ← geometry 전달
                spell->targeting.range
            );
    }
}
