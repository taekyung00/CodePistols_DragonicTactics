/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Character.cpp
Project:    GAM200
Author:     Seungju Song & Gemini
Created:    Oct 08, 2025
Updated:    Oct 10, 2025
*/

#include "Character.h"
#include"../Engine/GameObject.h"
#include "StatsComponent.h" 
#include "GridPosition.h"
#include "ActionPoints.h"
#include "SpellSlots.h"
Character::Character(CharacterTypes charType, Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level)
    : CS230::GameObject(Math::vec2{ 0, 0 }),
    m_character_type(charType),
    m_facing_direction(Direction::South)
{
    InitializeComponents(start_coordinates, max_action_points, max_slots_per_level);
}

void Character::InitializeComponents(Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level) {
    CharacterStats initial_stats;

    AddGOComponent(new StatsComponent(initial_stats));
    AddGOComponent(new GridPosition(start_coordinates));
    AddGOComponent(new ActionPoints(max_action_points));
    AddGOComponent(new SpellSlots(max_slots_per_level));
}

void Character::Update(double dt) {
    if (m_is_moving) {
        UpdateMovement(dt);
    }

    CS230::GameObject::Update(dt);
}

void Character::Draw(Math::TransformationMatrix camera_matrix) {
    CS230::GameObject::Draw(camera_matrix);
}



void Character::PerformAttack(Character* target) {
    if (target == nullptr || !target->IsAlive()) {
        Engine::GetLogger().LogDebug(TypeName() + " has no valid target to attack.");
        return;
    }

    UpdateFacingDirection(target->GetGridPosition()->Get());
    ActionPoints* ap = GetActionPointsComponent();
    if (ap == nullptr || ap->Consume(1) == false) {
        Engine::GetLogger().LogDebug(TypeName() + " tried to attack, but has no Action Points.");
        return;
    }

    int total_damage = 10;

    Engine::GetLogger().LogEvent(
        TypeName() + " attacks " + target->TypeName() +
        " for " + std::to_string(total_damage) + " damage."
    );
    target->TakeDamage(total_damage, this);
}

void Character::PerformAction([[maybe_unused]] Action* action, [[maybe_unused]] Character* target, [[maybe_unused]] Math::ivec2 tile_position) {
}

void Character::ReceiveHeal(int amount) {
    if (GetStatsComponent() != nullptr) {
        GetStatsComponent()->Heal(amount);
    }
}

void Character::TakeDamage(int damage, [[maybe_unused]] Character* attacker) {
    if (GetStatsComponent() != nullptr) {
        GetStatsComponent()->TakeDamage(damage);
    }

    if (IsAlive() == false) {
        // Die();
    }
}


void Character::SetPathTo([[maybe_unused]] Math::ivec2 destination) {
}

void Character::UpdateMovement([[maybe_unused]]double dt) {
    //if (is_arrived_at_tile) {
    //    // ...
    //    if (!m_current_path.empty()) {
    //        // [수정] 다음 경로를 향해 방향을 바꿉니다.
    //        UpdateFacingDirection(m_current_path.front());
    //    }
    //}
}


bool Character::IsAlive() const {
    const StatsComponent* stats = GetStatsComponent();
    return (stats != nullptr) ? stats->IsAlive() : false;
}

const CharacterStats& Character::GetStats() const {
    return GetStatsComponent()->GetAllStats();
}

int Character::GetMovementRange() const {
    const StatsComponent* stats = GetStatsComponent();
    return (stats != nullptr) ? stats->GetSpeed() : 0;
}

int Character::GetActionPoints() const {
    const ActionPoints* ap = GetActionPointsComponent();
    return (ap != nullptr) ? ap->GetCurrentPoints() : 0;
}

bool Character::HasSpellSlot(int level) const {
    const SpellSlots* ss = GetSpellSlots();
    return (ss != nullptr) ? ss->HasSlot(level) : false;
}


StatsComponent* Character::GetStatsComponent() const {
    return GetGOComponent<StatsComponent>();
}

GridPosition* Character::GetGridPosition() const {
    return GetGOComponent<GridPosition>();
}

ActionPoints* Character::GetActionPointsComponent() const {
    return GetGOComponent<ActionPoints>();
}

SpellSlots* Character::GetSpellSlots() const {
    return GetGOComponent<SpellSlots>();
}

Direction Character::GetFacingDirection() const {
    return m_facing_direction;
}

void Character::SetFacingDirection(Direction new_direction) {
    m_facing_direction = new_direction;
}

void Character::UpdateFacingDirection(Math::ivec2 target_pos) {
    GridPosition* my_pos = GetGridPosition();
    if (my_pos == nullptr) return;

    Math::ivec2 dir_vec = target_pos - my_pos->Get();

    // x축 이동이 더 큰 경우 동/서 방향
    if (abs(dir_vec.x) > abs(dir_vec.y)) {
        if (dir_vec.x > 0) {
            SetFacingDirection(Direction::East);
        }
        else {
            SetFacingDirection(Direction::West);
        }
    }
    // y축 이동이 더 크거나 같은 경우 남/북 방향
    else {
        if (dir_vec.y > 0) {
            SetFacingDirection(Direction::North);
        }
        else {
            SetFacingDirection(Direction::South);
        }
    }
}