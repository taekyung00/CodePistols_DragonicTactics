#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Character.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 08, 2025
Updated:    Oct 10, 2025
*/

#include "./Engine/Engine.h"
#include "./Engine/GameObject.h"
#include "./Engine/Logger.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/StateComponents/DiceManager.h"
#include "./Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Character.h"
#include "Components/GridPosition.h"

Character::Character(CharacterTypes charType, Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level)
	: CS230::GameObject({static_cast<double>(start_coordinates.x * GridSystem::TILE_SIZE), static_cast<double>(start_coordinates.y * GridSystem::TILE_SIZE)}), m_character_type(charType)
{
  InitializeComponents(start_coordinates, max_action_points, max_slots_per_level);
}

void Character::InitializeComponents(Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level)
{
  CharacterStats initial_stats;

  AddGOComponent(new StatsComponent(initial_stats));
  AddGOComponent(new GridPosition(start_coordinates));
  AddGOComponent(new ActionPoints(max_action_points));
  AddGOComponent(new SpellSlots(max_slots_per_level));
  AddGOComponent(new MovementComponent(this));
  AddGOComponent(new StatusEffectComponent());
  AddGOComponent(new ShakeComponent());
}



float Character::GetHPPercentage()
{
  const StatsComponent* stats = GetGOComponent<StatsComponent>();
  if (stats == nullptr)
	return 0.0f;

  return stats->GetHealthPercentage();
}

int Character::GetAvailableSpellSlots(int level)
{
  SpellSlots* slots = GetGOComponent<SpellSlots>();
  if (slots == nullptr)
	return 0;

  return slots->GetSpellSlotCount(level);
}

bool Character::HasAnySpellSlot()
{
  SpellSlots* slots = GetGOComponent<SpellSlots>();
  if (slots == nullptr)
	return false;

  // 레벨 1-5 중 하나라도 슬롯이 있는지 체크
  for (int level = 1; level <= 5; ++level)
  {
	if (slots->HasSlot(level))
	  return true;
  }
  return false;
}

void Character::RefreshActionPoints()
{
  GetGOComponent<ActionPoints>()->Refresh();

   has_attacked_this_turn_ = false; // 턴 시작 시 리셋
}

void Character::Update(double dt)
{
  CS230::GameObject::Update(dt);
  //GetShakeComponent()->Update(dt);
}

void Character::Draw(Math::TransformationMatrix camera_matrix , unsigned int color, float depth)
{
    // 1. ShakeComponent에서 현재 프레임의 흔들림 오프셋 가져오기
    Math::vec2 shakeOffset = GetShakeComponent()->GetOffset();

    // 2. 흔들림 오프셋이 0이 아니라면 (진동 중이라면)
    if (shakeOffset.x != 0.0f || shakeOffset.y != 0.0f)
    {
        // 현재 캐릭터의 진짜 위치 임시 저장
        Math::vec2 originalPos = GetPosition(); 

        // 렌더링을 위해 위치에 오프셋 더하기 (타입 오류 방지를 위해 double로 캐스팅)
        SetPosition({ 
            originalPos.x + static_cast<double>(shakeOffset.x), 
            originalPos.y + static_cast<double>(shakeOffset.y) 
        });

        // 흔들린 위치로 부모의 Draw 호출 (실제 화면에 그리기)
        CS230::GameObject::Draw(camera_matrix, color, depth);

        // 매우 중요. 화면에 그리고 난 후, 캐릭터의 위치를 다시 원래대로 원상복구!
        SetPosition(originalPos);
    }
    else
    {
        // 흔들림이 없을 때는 그냥 평소대로 그리기
        CS230::GameObject::Draw(camera_matrix, color, depth);
    }
}

void Character::OnTurnStart()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - BEGIN");

  RefreshActionPoints();

  Engine::GetLogger().LogEvent(TypeName() + " ActionPoints refreshed to " + std::to_string(GetActionPoints()));

  StatsComponent* stats = GetStatsComponent();
  if (stats)
  {
	stats->RefreshSpeed();
	Engine::GetLogger().LogEvent(TypeName() + " Speed refreshed to " + std::to_string(stats->GetSpeed()));
  }

  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - END");
}

void Character::OnTurnEnd()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " called");
  Engine::GetLogger().LogEvent(TypeName() + " turn ended");
}

// void Character::PerformAttack(Character* target)
// {
//     if (target == nullptr || !target->IsAlive())
//     {
//         Engine::GetLogger().LogDebug(TypeName() + " has no valid target to attack.");
//         return;
//     }

//     ActionPoints* ap = GetActionPointsComponent();
//     if (ap == nullptr || ap->Consume(1) == false)
//     {
//         Engine::GetLogger().LogDebug(TypeName() + " tried to attack, but has no Action Points.");
//         return;
//     }


//     int total_damage = 10 + Engine::GetDiceManager().RollDiceFromString("4d6");

//     Engine::GetLogger().LogEvent(TypeName() + " attacks " + target->TypeName() + " for " + std::to_string(total_damage) + " damage.");
//     target->TakeDamage(total_damage, this);
// }

void Character::PerformAction([[maybe_unused]] Action* action, [[maybe_unused]] Character* target, [[maybe_unused]] Math::ivec2 tile_position)
{
}

void Character::SetGridSystem(GridSystem* grid)
{
  m_gridSystem = grid;

  MovementComponent* move_comp = GetGOComponent<MovementComponent>();
  if (move_comp != nullptr)
  {
	move_comp->SetGridSystem(grid);
  }
  else
  {
	Engine::GetLogger().LogError(TypeName() + " is missing MovementComponent! Cannot set GridSystem for it.");
  }
}

void Character::SetPath(std::vector<Math::ivec2> path)
{
  if (m_movement_component == nullptr)
  {
	m_movement_component = GetGOComponent<MovementComponent>();
  }
  if (m_movement_component)
  {
	m_movement_component->SetPath(std::move(path));
  }
}

void Character::ReceiveHeal(int amount)
{
  if (GetStatsComponent() != nullptr)
  {
	GetStatsComponent()->Heal(amount);
  }
}

void Character::TakeDamage(int damage, [[maybe_unused]] Character* attacker)
{
  if (GetStatsComponent() != nullptr)
  {
	  GetStatsComponent()->TakeDamage(damage);
    GetShakeComponent()->StartShake(10.0f, 0.3f);
  }

  if (IsAlive() == false)
  {
	// Die();
  // 1. 그리드 맵 상에서 해당 캐릭터의 데이터를 비워줍니다.
        if (m_gridSystem != nullptr && GetGridPosition() != nullptr)
        {
            m_gridSystem->RemoveCharacter(GetGridPosition()->Get());
        }
        
        // 2. 엔진의 GameObjectManager가 이 오브젝트를 메모리에서 파괴하도록 예약합니다.
        Destroy();
  }
}

bool Character::IsAlive()
{
  const StatsComponent* stats = GetStatsComponent();
  return (stats != nullptr) ? stats->IsAlive() : false;
}

const CharacterStats& Character::GetStats()
{
  return GetStatsComponent()->GetAllStats();
}

int Character::GetMovementRange()
{
  const StatsComponent* stats = GetStatsComponent();
  return (stats != nullptr) ? stats->GetSpeed() : 0;
}

int Character::GetActionPoints()
{
  const ActionPoints* ap = GetActionPointsComponent();
  return (ap != nullptr) ? ap->GetCurrentPoints() : 0;
}

bool Character::HasSpellSlot(int level)
{
  const SpellSlots* ss = GetSpellSlots();
  return (ss != nullptr) ? ss->HasSlot(level) : false;
}

StatsComponent* Character::GetStatsComponent()
{
  return GetGOComponent<StatsComponent>();
}

int Character::GetHP()
{
  return GetGOComponent<StatsComponent>()->GetCurrentHP();
}

int Character::GetMaxHP()
{
  return GetGOComponent<StatsComponent>()->GetMaxHP();
}

int Character::GetAttackRange()
{
  return GetGOComponent<StatsComponent>()->GetAttackRange();
}

GridPosition* Character::GetGridPosition()
{
  return GetGOComponent<GridPosition>();
}

ActionPoints* Character::GetActionPointsComponent()
{
  return GetGOComponent<ActionPoints>();
}

SpellSlots* Character::GetSpellSlots()
{
  return GetGOComponent<SpellSlots>();
}

ShakeComponent* Character::GetShakeComponent()
{
  return GetGOComponent<ShakeComponent>();
}

int Character::GetSpellSlotCount(int level)
{
  return GetGOComponent<SpellSlots>()->GetSpellSlotCount(level);
}

void Character::SetSpellSlots(std::map<int, int> spellSlot)
{
  GetGOComponent<SpellSlots>()->SetSpellSlots(spellSlot);
}

void Character::ConsumeSpell(int level)
{ 
  GetGOComponent<SpellSlots>()->Consume(level);
}
const std::vector<ActiveEffect>& Character::GetActiveEffects()
{
    static const std::vector<ActiveEffect> empty;
    const auto* se = GetGOComponent<StatusEffectComponent>();
    return se ? se->GetAllEffects() : empty;
}
// void Character::RefreshActionPoints()
// {
//     GetGOComponent<ActionPoints>()->Refresh();
// }
// void Character::SetCurrentHP(int hp)
// {

// }

void Character::SetGridPosition(Math::ivec2 new_coordinates)
{
  GetGOComponent<GridPosition>()->Set(new_coordinates);
  SetPosition({
        static_cast<double>(new_coordinates.x * GridSystem::TILE_SIZE),
        static_cast<double>(new_coordinates.y * GridSystem::TILE_SIZE)
    });
}

void Character::SetHP(int HP)
{
  GetGOComponent<StatsComponent>()->SetHP(HP);
}

void Character::SetAttackRange(int new_range)
{
  GetGOComponent<StatsComponent>()->SetAttackRange(new_range);
}

bool Character::HasSpell([[maybe_unused]] std::string spell_name)
{
  return false; // TODO modify it to return actual spell name(type)
}

void Character::SetActionPoints(int new_points)
{
  return GetGOComponent<ActionPoints>()->SetPoints(new_points);
}

bool Character::Has(const std::string& effect_name) 
{
  StatusEffectComponent* se = GetGOComponent<StatusEffectComponent>();
    return se && se->Has(effect_name);
}

void Character::AddEffect(const std::string& name, int duration, int magnitude)
{
    StatusEffectComponent* se = GetGOComponent<StatusEffectComponent>();
    if (!se) return;
    se->AddEffect(name, duration, magnitude);

    EventBus* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
    if (bus)
        bus->Publish(StatusEffectAddedEvent{ this, name, duration, magnitude });
}

void Character::RemoveEffect(const std::string& name)
{
    if (StatusEffectComponent* se = GetGOComponent<StatusEffectComponent>())
        se->RemoveEffect(name);
}

void Character::RemoveAllEffects()
{
    if (StatusEffectComponent* se = GetGOComponent<StatusEffectComponent>())
        se->RemoveAllEffects();
}

