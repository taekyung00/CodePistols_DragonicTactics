#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MovementComponent.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Nov 16, 2025
*/

#include "Engine/GameObject.h"								 // GetGOComponent, SetPosition
#include "Engine/Logger.h"									 // Engine::GetLogger
#include "Engine/SoundManager.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h" // TILE_SIZE, IsWalkable, MoveCharacter
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/Objects/Character.h"
#include "GridPosition.h"									 // Get, Set
#include "MovementComponent.h"
#include "StatsComponent.h" // GetSpeed, ReduceSpeed, IsAlive
#include "../../StateComponents/EventBus.h"
#include "../../Types/Events.h"

MovementComponent::MovementComponent(CS230::GameObject* owner) : CS230::Component(), m_owner(owner)
{
  if (m_owner != nullptr)
  {
	m_gridPosition = m_owner->GetGOComponent<GridPosition>();
	m_stats		   = m_owner->GetGOComponent<StatsComponent>();
  }
  else
  {
	Engine::GetLogger().LogError("MovementComponent created without a valid owner!");
	m_gridPosition = nullptr;
	m_stats		   = nullptr;
  }
}

void MovementComponent::SetGridSystem(GridSystem* grid)
{
  m_gridSystem = grid;
}

void MovementComponent::SetPath(std::vector<Math::ivec2> path)
{
  if (m_current_path != path)
  {
	m_current_path = std::move(path);
	// m_stats->ReduceSpeed(static_cast<int>(m_current_path.size()));
	m_moveTimer	   = 0.0;
  }
}

bool MovementComponent::IsMoving() const
{
  return !m_current_path.empty();
}

void MovementComponent::ClearPath()
{
  m_current_path.clear();
  m_moveTimer = 0.0;
}

void MovementComponent::Update(double dt)
{
  if (m_current_path.empty() || m_owner == nullptr)
  {
	return;
  }

  if (m_stats == nullptr || m_gridPosition == nullptr)
  {
	Engine::GetLogger().LogError(m_owner->TypeName() + " MovementComponent dependencies not found!");
	m_current_path.clear();
	return;
  }

  if (!m_stats->IsAlive())
  {
	m_current_path.clear();
	return;
  }

  if (m_gridSystem == nullptr)
  {
	Engine::GetLogger().LogError(m_owner->TypeName() + " MovementComponent: GridSystem is null!");
	m_current_path.clear();
	return;
  }

  m_moveTimer += dt;

  if (m_moveTimer >= MOVE_TIME_PER_TILE)
  {
	m_moveTimer = 0.0;

	if (m_stats->GetSpeed() <= 0)
	{
	  Engine::GetLogger().LogEvent(m_owner->TypeName() + " is out of MOV.");
	  m_current_path.clear();
	  return;
	}

	Math::ivec2 next_pos = m_current_path.front();
	m_current_path.erase(m_current_path.begin());

	Math::ivec2 current_pos = m_gridPosition->Get();

	GridSystem::TileType tile_type  = m_gridSystem->GetTileType(next_pos);
	bool                 can_enter  = (tile_type == GridSystem::TileType::Empty || tile_type == GridSystem::TileType::Lava)
	                                  && !m_gridSystem->IsOccupied(next_pos);

	if (can_enter)
	{
	  m_gridSystem->MoveCharacter(current_pos, next_pos);

	  m_gridPosition->Set(next_pos);

	  m_owner->SetPosition({ static_cast<double>(next_pos.x * GridSystem::TILE_SIZE), static_cast<double>(next_pos.y * GridSystem::TILE_SIZE) });

	  m_stats->ReduceSpeed();

	  auto* character = dynamic_cast<Character*>(m_owner);
	  if (character)
	  {
	    const char* walk_sfx = (character->GetCharacterType() == CharacterTypes::Dragon)
	                           ? SoundManager::SFX_DRAGON_WALK
	                           : SoundManager::SFX_HUMAN_WALK;
	    Engine::GetSoundManager().PlaySFX(walk_sfx);
	  }

	  Engine::GetLogger().LogEvent(m_owner->TypeName() + " moved. MOV remaining: " + std::to_string(m_stats->GetSpeed()));

	  // 배틀 로그 UI 이벤트 전송 — 은신 중에는 이동을 노출하지 않음
	  // 경로의 마지막 타일(최종 목적지)에 도달했을 때만 1회 출력
	  if (m_current_path.empty() && (!character || !character->Has("Stealth")))
	  {
	    if (auto* eventBus = Engine::GetGameStateManager().GetGSComponent<EventBus>())
	      eventBus->Publish(BattleLogMessageEvent{ m_owner->TypeName() + " moved to (" + std::to_string(next_pos.x) + ", " + std::to_string(next_pos.y) + ")" });
	  }

	  // 용암 이동 피해
	  if (tile_type == GridSystem::TileType::Lava)
	  {
	    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
	    auto* combat    = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
	    if (spell_sys && combat)
	    {
	      int dmg = spell_sys->GetLavaDamageAt(next_pos);
	      if (dmg > 0)
	      {
	        Engine::GetLogger().LogEvent(m_owner->TypeName() + " takes " + std::to_string(dmg) + " lava damage while moving");
	        combat->ApplyDamage(nullptr, static_cast<Character*>(m_owner), dmg);
	      }
	    }
	  }
	}
	else
	{
	  Engine::GetLogger().LogError(m_owner->TypeName() + " path blocked!");
	  m_current_path.clear();
	}
  }
}