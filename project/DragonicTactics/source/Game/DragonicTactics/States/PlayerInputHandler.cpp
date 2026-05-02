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

Math::ivec2 PlayerInputHandler::ConvertScreenToGrid(Math::vec2 screen_pos)
{
  Math::vec2 world_pos = screen_pos;
  if (m_camera)
    world_pos = m_camera->ScreenToWorld(screen_pos, Engine::GetWindow().GetSize());
  int grid_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
  int grid_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
  return { grid_x, grid_y };
}

PlayerInputHandler::PlayerInputHandler() : m_state(ActionState::None)
{
}

void PlayerInputHandler::OnAttackPressed()
{
    CancelCurrentAction();
    SetState(ActionState::TargetingForAttack);
}

void PlayerInputHandler::OnEndTurnPressed()
{
    TurnManager* tm = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
    if (tm) tm->EndCurrentTurn();
}

void PlayerInputHandler::Update(double dt, Character* current_character, GridSystem* grid, CombatSystem* combat_system, ButtonManager& btns, const TacticalCamera* camera)
{
    m_camera = camera;

    auto debugMgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
    if (debugMgr != nullptr) {
        if (debugMgr->IsGodModeEnabled()) {
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::F)) {
                debugMgr->timeScale = debugMgr->FAST_FORWARD_SPEED;
            } else {
                debugMgr->timeScale = 1.0f;
            }
        } else {
            debugMgr->timeScale = 1.0f;
        }
    }

  if (current_character->GetCharacterType() != CharacterTypes::Dragon)
  {
	return;
  }

  Dragon* dragon = static_cast<Dragon*>(current_character);

  // ── Wall Creation 확인 버튼 ──────────────────────────────────────
  if (btns.IsPressed("slot_wall_confirm") && m_state == ActionState::WallPlacementMulti)
  {
    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    if (spell_sys && !m_wall_placement_tiles.empty())
      spell_sys->CastWalls(dragon, m_selected_spell_id,
                           m_wall_placement_tiles, m_selected_upcast_level);
    m_wall_placement_tiles.clear();
    m_state = ActionState::None;
    btns.SetVisible("slot_wall_confirm", false);
    grid->DisableSpellTargetingMode();
    grid->ClearWallPreviewTiles();
  }

  // ── Magma Blast 확인 버튼 ──────────────────────────────────────
  if (btns.IsPressed("slot_wall_confirm") && m_state == ActionState::LavaPlacementMulti)
  {
    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    if (spell_sys && !m_wall_placement_tiles.empty())
      spell_sys->CastLavaZones(dragon, m_selected_spell_id,
                               m_wall_placement_tiles, m_selected_upcast_level);
    m_wall_placement_tiles.clear();
    m_state = ActionState::None;
    btns.SetVisible("slot_wall_confirm", false);
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
        m_wall_placement_tiles.erase(it);
        grid->SetWallPreviewTiles(m_wall_placement_tiles);
      }
      else
      {
        m_wall_placement_tiles.clear();
        m_state = ActionState::None;
        btns.SetVisible("slot_wall_confirm", false);
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
	}
  }

  HandleDragonInput(dt, dragon, grid, combat_system);
}

void PlayerInputHandler::HandleDragonInput([[maybe_unused]] double dt, Dragon* dragon, GridSystem* grid, CombatSystem* combat_system)
{
  auto& input		   = Engine::GetInput();
  bool	is_clicking_ui = ImGui::GetIO().WantCaptureMouse;

  // 이동 모드일 때 마우스 호버 처리
  if (m_state == ActionState::SelectingMove)
  {
	Math::vec2	mouse_pos = input.GetMousePos();
	Math::ivec2 grid_pos  = ConvertScreenToGrid(mouse_pos);

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
	case ActionState::None:
	{
	  // Dragon 타일 클릭 → 이동 선택 모드 진입
	  if (grid_pos == dragon->GetGridPosition()->Get() && dragon->GetMovementRange() > 0)
	  {
		SetState(ActionState::SelectingMove);
		grid->EnableMovementMode(dragon->GetGridPosition()->Get(), dragon->GetMovementRange());
	  }
	  break;
	}

	case ActionState::SelectingMove:
	  if (grid->IsReachable(grid_pos))
	  {
		auto path = grid->FindPath(dragon->GetGridPosition()->Get(), grid_pos);

		if (!path.empty())
		{
		  dragon->SetPath(std::move(path));
		  m_state = ActionState::Moving;

		  Engine::GetLogger().LogEvent("Dragon moving to (" + std::to_string(grid_pos.x) + ", " + std::to_string(grid_pos.y) + ")");

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
  }
  Engine::GetLogger().LogDebug("Mouse: " + std::to_string(mouse_pos.x) + ", " + std::to_string(mouse_pos.y));
  Engine::GetLogger().LogDebug("Grid Result: " + std::to_string(grid_pos.x) + ", " + std::to_string(grid_pos.y));
}

void PlayerInputHandler::HandleRightClick(Dragon* dragon)
{
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
  auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

  if (m_state == ActionState::SelectingMove)
  {
    Engine::GetLogger().LogEvent("Movement mode cancelled");
    if (grid) grid->DisableMovementMode();
  }
  else if (m_state == ActionState::TargetingForSpell ||
           m_state == ActionState::WallPlacementMulti ||
           m_state == ActionState::LavaPlacementMulti)
  {
    if (grid) grid->DisableSpellTargetingMode();
    if (m_state == ActionState::WallPlacementMulti || m_state == ActionState::LavaPlacementMulti)
    {
      m_wall_placement_tiles.clear();
      if (grid) grid->ClearWallPreviewTiles();
    }
  }

  m_state = ActionState::None;
}

void PlayerInputHandler::SelectSpell(const std::string& spell_id, Character* caster, int upcast_level, ButtonManager& btns)
{
    // Cancel any previous targeting before switching spells
    {
        auto* g = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
        if (g && (m_state == ActionState::TargetingForSpell ||
                  m_state == ActionState::WallPlacementMulti ||
                  m_state == ActionState::LavaPlacementMulti))
        {
            g->DisableSpellTargetingMode();
            if (m_state == ActionState::WallPlacementMulti || m_state == ActionState::LavaPlacementMulti)
            {
                m_wall_placement_tiles.clear();
                g->ClearWallPreviewTiles();
            }
        }
    }

    m_selected_spell_id     = spell_id;
    m_selected_upcast_level = upcast_level;

    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto* grid      = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    if (spell_id == "S_GEO_010")
    {
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
        btns.SetVisible("slot_wall_confirm", true);

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
        btns.SetVisible("slot_wall_confirm", true);

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
