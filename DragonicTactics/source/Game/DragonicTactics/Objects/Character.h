/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Character.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 08, 2025
Updated:    Oct 09, 2025
*/

#pragma once
#include "Engine/GameObject.h"
#include "Engine/GameObjectManager.h"
#include "Engine/Input.h"
#include "Engine/Matrix.h"
#include "Game/DragonicTactics/Objects/Actions/Action.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"
#include "Game/DragonicTactics/Types/GameTypes.h"
#include <map>
#include <vector>

class GridSystem;
class GridPosition;
class ActionPoints;
class SpellSlots;
class StatusEffects;
class DamageCalculator;
class TargetingSystem;
class AIMemory;
class GridFootprint;
class Action;
class StatsComponent;
class MovementComponent;

class Character : public CS230::GameObject
{
public:
  virtual ~Character() = default;

  void Update(double dt) override;
  void Draw(Math::TransformationMatrix camera_matrix) override;

  GameObjectTypes Type() override
  {
	return GameObjectTypes::Character;
  }

  std::string TypeName() override = 0;

  virtual void OnTurnStart();
  virtual void OnTurnEnd();

  // virtual void PerformAttack(Character* target);
  virtual void PerformAction(Action* action, Character* target, Math::ivec2 tile_position);
  virtual void TakeDamage(int damage, Character* attacker);
  virtual void ReceiveHeal(int amount);

  void SetPath(std::vector<Math::ivec2> path);
  void SetGridSystem(GridSystem* grid);

  CharacterTypes GetCharacterType() const
  {
	return m_character_type;
  }

  bool					IsAlive();
  const CharacterStats& GetStats();
  int					GetMovementRange();
  int					GetActionPoints();
  bool					HasSpellSlot(int level);
  void					RefreshActionPoints();

  GridPosition*	  GetGridPosition();
  StatsComponent* GetStatsComponent();
  ActionPoints*	  GetActionPointsComponent();
  SpellSlots*	  GetSpellSlots();
  void			  SetActionPoints(int new_points);
  // StatusEffects* GetStatusEffects() const;
  // GridFootprint* GetGridFootprint() const;
  int			  GetSpellSlotCount(int level);
  void			  SetSpellSlots(std::map<int, int> spellSlot);
  void			  ConsumeSpell(int level);

  void SetGridPosition(Math::ivec2 new_coordinates);
  int  GetHP();
  int  GetMaxHP();
  int  GetAttackRange();
  void SetAttackRange(int new_range);
  void SetHP(int HP);
  bool HasSpell(std::string spell_name);

  // ========================================
    // 상태 쿼리 메서드 (Fact Queries)
    // AI 전략 및 다른 시스템에서 사용
    // ========================================

    /// @brief HP 백분율 조회 (0.0 ~ 1.0)
    /// @return 현재 HP / 최대 HP, StatsComponent 없으면 0.0
    float GetHPPercentage() ;

    /// @brief 보물 소유 여부 조회
    /// @return true if has treasure
    bool HasTreasure() const { return has_treasure_; }

    /// @brief 보물 소유 상태 설정 (보물 시스템에서 호출)
    void SetTreasure(bool value) { has_treasure_ = value; }

    /// @brief 특정 레벨의 주문 슬롯 잔여량 조회
    /// @param level 주문 레벨 (1-9)
    /// @return 잔여 슬롯 개수, SpellSlots 없으면 0
    int GetAvailableSpellSlots(int level) ;

    /// @brief 모든 레벨의 주문 슬롯 중 1개라도 있는지
    /// @return true if has any spell slots
    bool HasAnySpellSlot();

    // TODO: Week 6+ StatusEffect 시스템 구현 후 추가
    // bool HasBuff(const std::string& buff_name) const;
    // bool HasDebuff(const std::string& debuff_name) const;

  protected:
  Character(CharacterTypes charType, Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);


  void InitializeComponents(Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);


  // virtual void                DecideAction()              = 0;
  CharacterTypes m_character_type;
  GameObject*	 m_turn_target = nullptr;
  TurnGoal		 m_turn_goal   = TurnGoal::None;


  GridSystem* m_gridSystem = nullptr;
  // void                        UpdateMovement(double dt);
  // std::vector<Math::ivec2>    m_current_path;
  // double                      m_moveTimer                 = 0.0;
  // static constexpr double     MOVE_TIME_PER_TILE          = 0.2;

  MovementComponent*   m_movement_component = nullptr;
  std::vector<Action*> m_action_list;


  private:
  bool has_treasure_ = false; // 보물 소유 여부
};