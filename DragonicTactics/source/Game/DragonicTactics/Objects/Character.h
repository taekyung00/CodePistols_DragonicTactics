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
#include <map>
#include <vector>
#include "./Engine/GameObject.h"
#include "./Engine/GameObjectManager.h"
#include "./Engine/Matrix.hpp"
#include "./Engine/Input.hpp"
#include "./Game/DragonicTactics/Types/GameTypes.h"
#include "./Game/DragonicTactics/Types/CharacterTypes.h"
#include "./Game/DragonicTactics/Objects/Actions/Action.h"

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

class Character : public CS230::GameObject {
public:
    virtual ~Character() = default;

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
    GameObjectTypes Type() override { return GameObjectTypes::Character; }
    std::string TypeName() override = 0;

    virtual void OnTurnStart() = 0;
    virtual void OnTurnEnd() = 0;

    // virtual void PerformAttack(Character* target);
    virtual void PerformAction(Action* action, Character* target, Math::ivec2 tile_position);
    virtual void TakeDamage(int damage, Character* attacker);
    virtual void ReceiveHeal(int amount);

    void SetPath(std::vector<Math::ivec2> path);
    void SetGridSystem(GridSystem* grid);

    CharacterTypes GetCharacterType() const { return m_character_type; }
    bool IsAlive();
    const CharacterStats& GetStats();
    int GetMovementRange();
    int GetActionPoints();
    bool HasSpellSlot(int level);
    void RefreshActionPoints();

    GridPosition* GetGridPosition() ;
    StatsComponent* GetStatsComponent() ;
    ActionPoints* GetActionPointsComponent() ;
    SpellSlots* GetSpellSlots() ;
    void SetActionPoints(int new_points);
    //StatusEffects* GetStatusEffects() const;
    //GridFootprint* GetGridFootprint() const;
    int GetSpellSlotCount(int level);
    void SetSpellSlots(std::map<int, int> spellSlot);
    void ConsumeSpell(int level);

    void SetGridPosition(Math::ivec2 new_coordinates);
    int GetHP();
    int GetMaxHP();
    int GetAttackRange();
    void SetAttackRange(int new_range);
    void SetHP(int HP);
    bool HasSpell(std::string spell_name);

protected:
    Character(CharacterTypes charType, Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);


    void InitializeComponents(Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);
    
    
    virtual void                DecideAction()              = 0;
    CharacterTypes              m_character_type;
    GameObject*                 m_turn_target               = nullptr;
    TurnGoal                    m_turn_goal                 = TurnGoal::None;
    

    void                        UpdateMovement(double dt);
    GridSystem*                 m_gridSystem                = nullptr;
    std::vector<Math::ivec2>    m_current_path;
    double                      m_moveTimer                 = 0.0;
    static constexpr double     MOVE_TIME_PER_TILE          = 0.2;
   

    std::vector<Action*> m_action_list;

private:
};