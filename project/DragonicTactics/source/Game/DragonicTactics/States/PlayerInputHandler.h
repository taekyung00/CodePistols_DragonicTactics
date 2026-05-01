/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  PlayerInputHandler.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/
#pragma once
#include "Engine/Vec2.h"
#include <memory>
#include <vector>
#include "./Engine/Component.h"

class Character;
class Dragon;
class GridSystem;
class CombatSystem;
class ButtonManager;
struct TacticalCamera;

class PlayerInputHandler
{
  public:
  enum class ActionState
  {
	None,
	SelectingMove,
	Moving,
	SelectingAction,
    SelectingSpell,
	TargetingForAttack,
	TargetingForSpell,
	WallPlacementMulti,
	LavaPlacementMulti
  };

  PlayerInputHandler();
  ~PlayerInputHandler() = default;

  void Update(double dt, Character* current_character, GridSystem* grid, CombatSystem* combat_system, ButtonManager& btns, const TacticalCamera* camera = nullptr);

  void CancelCurrentAction();
  void OnAttackPressed();
  void OnEndTurnPressed();

  ActionState GetCurrentState() const
  {
	  return m_state;
  }

  void SetState(ActionState new_state)
  {
	  m_state = new_state;
  }

  std::string GetSelectedSpellId() const { return m_selected_spell_id; }

  void SelectSpell(const std::string& spell_id, Character* caster, int upcast_level, ButtonManager& btns);

  private:
  ActionState              m_state = ActionState::None;
  std::string              m_selected_spell_id;
  int                      m_selected_upcast_level = 0;
  std::vector<Math::ivec2> m_wall_placement_tiles;
  int                      m_wall_placement_count = 0;
  const TacticalCamera*    m_camera               = nullptr;

  //helper function
  Math::ivec2 ConvertScreenToGrid(Math::vec2 world_pos);

  void HandleDragonInput(double dt, Dragon* dragon, GridSystem* grid, CombatSystem* combat_system);
  void HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon, GridSystem* grid, CombatSystem* combat_system);
  void HandleRightClick(Dragon* dragon);
};
