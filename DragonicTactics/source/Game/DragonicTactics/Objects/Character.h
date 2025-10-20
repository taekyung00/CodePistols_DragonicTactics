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
#include "./Engine/GameObject.h"
#include "./Engine/GameObjectManager.h"
#include "./Engine/Matrix.hpp"
#include "./Engine/Input.hpp"
#include "./Game/DragonicTactics/Types/GameTypes.h"
#include "./Game/DragonicTactics/Types/CharacterTypes.h"
#include "./Game/DragonicTactics/Objects/Actions/Action.h"


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

    virtual void PerformAttack(Character* target);
    virtual void PerformAction(Action* action, Character* target, Math::ivec2 tile_position);
    virtual void TakeDamage(int damage, Character* attacker);
    virtual void ReceiveHeal(int amount);

    void SetPathTo(Math::ivec2 destination);

    CharacterTypes GetCharacterType() const { return m_character_type; }
    bool IsAlive();
    const CharacterStats& GetStats();
    int GetMovementRange();
    int GetActionPoints();
    bool HasSpellSlot(int level);

    GridPosition* GetGridPosition() ;
    StatsComponent* GetStatsComponent() ;
    ActionPoints* GetActionPointsComponent() ;
    SpellSlots* GetSpellSlots() ;
    //StatusEffects* GetStatusEffects() const;
    //GridFootprint* GetGridFootprint() const;
protected:
    Character(CharacterTypes charType, Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);


    void InitializeComponents(Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);
    virtual void DecideAction() = 0;
    void UpdateMovement(double dt);

    CharacterTypes m_character_type;

    GameObject* m_turn_target = nullptr;
    TurnGoal m_turn_goal = TurnGoal::None;

    std::vector<Math::ivec2> m_current_path;
    bool m_is_moving = false;

    std::vector<Action*> m_action_list;

private:
};