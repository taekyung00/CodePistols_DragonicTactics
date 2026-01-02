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
#include "Game/DragonicTactics/StateComponents/GridSystem.h" // TILE_SIZE, IsWalkable, MoveCharacter
#include "GridPosition.h"									 // Get, Set
#include "MovementComponent.h"
#include "StatsComponent.h" // GetSpeed, ReduceSpeed, IsAlive

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
	  Engine::GetLogger().LogEvent(m_owner->TypeName() + " is out of speed.");
	  m_current_path.clear();
	  return;
	}

	Math::ivec2 next_pos = m_current_path.front();
	m_current_path.erase(m_current_path.begin());

	Math::ivec2 current_pos = m_gridPosition->Get();

	if (m_gridSystem->IsWalkable(next_pos))
	{
	  m_gridSystem->MoveCharacter(current_pos, next_pos);

	  m_gridPosition->Set(next_pos);

	  m_owner->SetPosition({ static_cast<double>(next_pos.x * GridSystem::TILE_SIZE), static_cast<double>(next_pos.y * GridSystem::TILE_SIZE) });

	  m_stats->ReduceSpeed();

	  Engine::GetLogger().LogEvent(m_owner->TypeName() + " moved. Speed remaining: " + std::to_string(m_stats->GetSpeed()));
	}
	else
	{
	  Engine::GetLogger().LogError(m_owner->TypeName() + " path blocked!");
	  m_current_path.clear();
	}
  }
}