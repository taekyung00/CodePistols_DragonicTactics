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
#include "./Engine/Component.h"

class Character;
class Dragon;
class GridSystem;
class CombatSystem;

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
	TargetingForSpell
  };

  PlayerInputHandler();
  ~PlayerInputHandler() = default;

  void Update(double dt, Character* current_character, GridSystem* grid, CombatSystem* combat_system);

  void CancelCurrentAction();

  ActionState GetCurrentState() const
  {
	  return m_state;
  }

  void SetState(ActionState new_state)
  {
	  m_state = new_state;
  }

  void SelectSpell(const std::string& spell_id)
  {
    m_selected_spell_id = spell_id;
    m_state = ActionState::TargetingForSpell;
  }

  private:
  ActionState m_state = ActionState::None;
  std::string m_selected_spell_id;

  //helper function
  Math::ivec2 ConvertScreenToGrid(Math::vec2 world_pos);

  void HandleDragonInput(double dt, Dragon* dragon, GridSystem* grid, CombatSystem* combat_system);
  void HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon, GridSystem* grid, CombatSystem* combat_system);
  void HandleRightClick(Dragon* dragon);
};
