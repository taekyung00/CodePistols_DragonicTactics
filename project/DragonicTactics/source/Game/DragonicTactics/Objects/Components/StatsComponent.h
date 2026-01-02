/**
 * \file
 * \author Seungju Song
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "./Engine/Component.h"
#include "./Game/DragonicTactics/Types/CharacterTypes.h"
#include <string>

class StatsComponent : public CS230::Component
{
  public:
  StatsComponent(const CharacterStats& initial_stats);

  void	TakeDamage(int damage);
  void	Heal(int amount);
  bool	IsAlive() const;
  int	GetCurrentHP() const;
  int	GetMaxHP() const;
  float GetHealthPercentage() const;
  void	SetHP(int HP);


  void RefreshSpeed();
  void ReduceSpeed(int i = 1);
  int  GetSpeed() const;

  int				 GetBaseAttack() const;
  const std::string& GetAttackDice() const;
  int				 GetBaseDefend() const;
  const std::string& GetDefendDice() const;
  int				 GetAttackRange() const;
  void				 SetAttackRange(int new_range);

  const CharacterStats& GetAllStats() const
  {
	return stats;
  }

  private:
  CharacterStats stats;
  int			 m_current_speed;
};