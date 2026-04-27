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
    btns.SetVisible("btn_wall_confirm", false);

    btns.SetVisible("S_ATK_020", false);
    btns.SetVisible("S_ENH_050", false);
    btns.SetVisible("S_DEB_020", false);

    btns.SetVisible("lbl_S_ATK_010", false);
    for (int lv = 1; lv <= 5; ++lv)
        btns.SetVisible("S_ATK_010_lv" + std::to_string(lv), false);

    btns.SetVisible("lbl_S_ENH_040", false);
    for (int lv = 0; lv <= 5; ++lv)
        btns.SetVisible("S_ENH_040_lv" + std::to_string(lv), false);

    btns.SetVisible("lbl_S_ATK_030", false);
    for (int lv = 3; lv <= 5; ++lv)
        btns.SetVisible("S_ATK_030_lv" + std::to_string(lv), false);

    btns.SetVisible("lbl_S_ATK_040", false);
    for (int lv = 3; lv <= 5; ++lv)
        btns.SetVisible("S_ATK_040_lv" + std::to_string(lv), false);

    btns.SetVisible("lbl_S_GEO_010", false);
    for (int lv = 2; lv <= 5; ++lv)
        btns.SetVisible("S_GEO_010_lv" + std::to_string(lv), false);

    btns.SetVisible("lbl_S_GEO_020", false);
    for (int lv = 1; lv <= 5; ++lv)
        btns.SetVisible("S_GEO_020_lv" + std::to_string(lv), false);

    btns.SetVisible("lbl_S_GEO_030", false);
    for (int lv = 0; lv <= 5; ++lv)
        btns.SetVisible("S_GEO_030_lv" + std::to_string(lv), false);
}

void PlayerInputHandler::Update(double dt, Character* current_character, GridSystem* grid, CombatSystem* combat_system, ButtonManager& btns)
{
 
    // 1. GameStateManager를 통해 DebugManager 컴포넌트 포인터를 가져옵니다.
    auto debugMgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();

    // 2. 컴포넌트가 정상적으로 등록되어 있는지 확인 (안전장치)
    if (debugMgr != nullptr) {
        // 3. 갓모드 체크 및 빨리감기 로직
        if (debugMgr->IsGodModeEnabled()) { 
            // 'F' 키를 누르고 있는 동안 빨리감기
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::F)) {
                debugMgr->timeScale = debugMgr->FAST_FORWARD_SPEED;
                
            } else {
                debugMgr->timeScale = 1.0f; // 키를 떼면 원상복구
            }
        } else {
            // 갓모드가 꺼지면 무조건 기본 속도 유지
            debugMgr->timeScale = 1.0f; 
        }
    }
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
        btns.SetVisible("lbl_S_ATK_010", true);
        for (int lv = 1; lv <= 5; ++lv)
            btns.SetVisible("S_ATK_010_lv" + std::to_string(lv), true);

        // 업캐스트: Mana Conversion Lv0~5
        btns.SetVisible("lbl_S_ENH_040", true);
        for (int lv = 0; lv <= 5; ++lv)
            btns.SetVisible("S_ENH_040_lv" + std::to_string(lv), true);

        // 업캐스트: Dragon's Fury Lv3~5
        btns.SetVisible("lbl_S_ATK_030", true);
        for (int lv = 3; lv <= 5; ++lv)
            btns.SetVisible("S_ATK_030_lv" + std::to_string(lv), true);

        // 업캐스트: Meteor Lv3~5
        btns.SetVisible("lbl_S_ATK_040", true);
        for (int lv = 3; lv <= 5; ++lv)
            btns.SetVisible("S_ATK_040_lv" + std::to_string(lv), true);

        // 업캐스트: Magma Blast Lv2~5
        btns.SetVisible("lbl_S_GEO_010", true);
        for (int lv = 2; lv <= 5; ++lv)
            btns.SetVisible("S_GEO_010_lv" + std::to_string(lv), true);

        // 업캐스트: Wall Creation Lv1~5
        btns.SetVisible("lbl_S_GEO_020", true);
        for (int lv = 1; lv <= 5; ++lv)
            btns.SetVisible("S_GEO_020_lv" + std::to_string(lv), true);

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

  // ── Wall Creation 확인 버튼 ──────────────────────────────────────
  if (btns.IsPressed("btn_wall_confirm") && m_state == ActionState::WallPlacementMulti)
  {
    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    if (spell_sys && !m_wall_placement_tiles.empty())
      spell_sys->CastWalls(dragon, m_selected_spell_id,
                           m_wall_placement_tiles, m_selected_upcast_level);
    m_wall_placement_tiles.clear();
    m_state = ActionState::None;
    btns.SetVisible("btn_wall_confirm", false);
    grid->DisableSpellTargetingMode();
    grid->ClearWallPreviewTiles();
  }

  // ── Magma Blast 확인 버튼 ──────────────────────────────────────
  if (btns.IsPressed("btn_wall_confirm") && m_state == ActionState::LavaPlacementMulti)
  {
    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    if (spell_sys && !m_wall_placement_tiles.empty())
      spell_sys->CastLavaZones(dragon, m_selected_spell_id,
                               m_wall_placement_tiles, m_selected_upcast_level);
    m_wall_placement_tiles.clear();
    m_state = ActionState::None;
    btns.SetVisible("btn_wall_confirm", false);
    grid->DisableSpellTargetingMode();
    grid->ClearWallPreviewTiles();
  }

  // ── WallPlacementMulti / LavaPlacementMulti 우클릭: 선택 타일이면 해제, 아니면 전체 취소 ──
  if (m_state == ActionState::WallPlacementMulti || m_state == ActionState::LavaPlacementMulti)
  {
    auto& input_ref = Engine::GetInput();
    if (input_ref.MouseJustPressed(2))
    {
      Math::ivec2 rclick_pos = ConvertScreenToGrid(input_ref.GetMousePos());
      auto it = std::find(m_wall_placement_tiles.begin(), m_wall_placement_tiles.end(), rclick_pos);
      if (it != m_wall_placement_tiles.end())
      {
        // 해당 타일만 선택 해제
        m_wall_placement_tiles.erase(it);
        grid->SetWallPreviewTiles(m_wall_placement_tiles);
      }
      else
      {
        // 전체 취소
        m_wall_placement_tiles.clear();
        m_state = ActionState::None;
        btns.SetVisible("btn_wall_confirm", false);
        grid->DisableSpellTargetingMode();
        grid->ClearWallPreviewTiles();
      }
      return;
    }
  }

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

	case ActionState::WallPlacementMulti:
	{
	  if (grid->IsValidTile(grid_pos)
	      && grid->GetTileType(grid_pos) == GridSystem::TileType::Empty
	      && !grid->IsOccupied(grid_pos))
	  {
	    bool already_selected = std::find(m_wall_placement_tiles.begin(),
	                                      m_wall_placement_tiles.end(),
	                                      grid_pos) != m_wall_placement_tiles.end();
	    if (!already_selected && static_cast<int>(m_wall_placement_tiles.size()) < m_wall_placement_count)
	    {
	      m_wall_placement_tiles.push_back(grid_pos);
	      grid->SetWallPreviewTiles(m_wall_placement_tiles);
	      Engine::GetLogger().LogEvent("Wall tile selected: ("
	        + std::to_string(grid_pos.x) + "," + std::to_string(grid_pos.y) + ") "
	        + std::to_string(m_wall_placement_tiles.size()) + "/" + std::to_string(m_wall_placement_count));
	    }
	  }
	  break;
	}

	case ActionState::LavaPlacementMulti:
	{
	  if (grid->IsValidTile(grid_pos)
	      && grid->GetTileType(grid_pos) == GridSystem::TileType::Empty
	      && !grid->IsOccupied(grid_pos))
	  {
	    bool already_selected = std::find(m_wall_placement_tiles.begin(),
	                                      m_wall_placement_tiles.end(),
	                                      grid_pos) != m_wall_placement_tiles.end();
	    if (!already_selected && static_cast<int>(m_wall_placement_tiles.size()) < m_wall_placement_count)
	    {
	      m_wall_placement_tiles.push_back(grid_pos);
	      grid->SetWallPreviewTiles(m_wall_placement_tiles);
	      Engine::GetLogger().LogEvent("Lava tile selected: ("
	        + std::to_string(grid_pos.x) + "," + std::to_string(grid_pos.y) + ") "
	        + std::to_string(m_wall_placement_tiles.size()) + "/" + std::to_string(m_wall_placement_count));
	    }
	  }
	  break;
	}

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
    m_selected_spell_id     = spell_id;
    m_selected_upcast_level = upcast_level;

    HideAllSpellButtons(btns);  // ★ 스펠 선택 완료 → 목록 닫기

    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto* grid      = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    if (spell_id == "S_GEO_010")
    {
        // Magma Blast — 멀티클릭 용암 배치 모드
        m_wall_placement_tiles.clear();
        if (spell_sys)
        {
            const SpellData* spell = spell_sys->GetSpellData(spell_id);
            m_wall_placement_count = spell ? (upcast_level - spell->spell_level + 1) : 1;
        }
        else
        {
            m_wall_placement_count = 1;
        }
        m_state = ActionState::LavaPlacementMulti;
        btns.SetVisible("btn_wall_confirm", true);

        if (spell_sys && grid && caster)
        {
            const SpellData* spell = spell_sys->GetSpellData(spell_id);
            if (spell)
                grid->EnableSpellTargetingMode(
                    caster->GetGridPosition()->Get(),
                    spell->targeting.geometry,
                    spell->targeting.range
                );
        }
    }
    else if (spell_id == "S_GEO_020")
    {
        // Wall Creation — 멀티클릭 배치 모드
        m_wall_placement_tiles.clear();
        if (spell_sys)
        {
            const SpellData* spell = spell_sys->GetSpellData(spell_id);
            m_wall_placement_count = spell ? (upcast_level - spell->spell_level + 1) : 1;
        }
        else
        {
            m_wall_placement_count = 1;
        }
        m_state = ActionState::WallPlacementMulti;
        btns.SetVisible("btn_wall_confirm", true);

        if (spell_sys && grid && caster)
        {
            const SpellData* spell = spell_sys->GetSpellData(spell_id);
            if (spell)
                grid->EnableSpellTargetingMode(
                    caster->GetGridPosition()->Get(),
                    spell->targeting.geometry,
                    spell->targeting.range
                );
        }
    }
    else
    {
        m_state = ActionState::TargetingForSpell;

        if (spell_sys && grid && caster)
        {
            const SpellData* spell = spell_sys->GetSpellData(spell_id);
            if (spell)
                grid->EnableSpellTargetingMode(
                    caster->GetGridPosition()->Get(),
                    spell->targeting.geometry,
                    spell->targeting.range
                );
        }
    }
}
