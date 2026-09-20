/**
 * \file
 * \author Seungju Song
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "ActionPoints.h"

ActionPoints::ActionPoints(int _max_points) : max_points(_max_points), current_points(max_points)
{
}

void ActionPoints::Refresh()
{
  current_points = max_points;
}

bool ActionPoints::Consume(int amount)
{
  if (HasEnough(amount))
  {
	current_points -= amount;
	return true;
  }
  return false;
}

int ActionPoints::GetCurrentPoints() const
{
  return current_points;
}

int ActionPoints::GetMaxPoints() const
{
  return max_points;
}

void ActionPoints::SetPoints(int new_points)
{
  // Haste의 OnTurnStart 보너스(AP+1)는 max_points를 넘어설 수 있어야 한다.
  // max_points로 클램프하면 RefreshActionPoints() 직후(=max_points) 호출되는
  // Haste 보너스가 항상 무시되어 시전 첫 턴에만 효과가 발생하는 버그가 생긴다.
  current_points = std::max(0, new_points);
}

bool ActionPoints::HasEnough(int amount) const
{
  return current_points >= amount;
}