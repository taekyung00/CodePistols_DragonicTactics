/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
// File: CS230/Game/Test/Week1TestMocks.h
#pragma once
#include "../../../Engine/Vec2.h"
#include <iostream>
#include <string>
#include <vector>

// Mock StatsComponent for Initiative testing (defined BEFORE MockCharacter)
class MockStatsComponent
{
  public:
  MockStatsComponent(int initialSpeed = 10) : speed(initialSpeed)
  {
  }

  int GetSpeed() const
  {
	return speed;
  }

  void SetSpeed(int newSpeed)
  {
	speed = newSpeed;
  }

  private:
  int speed;
};

// Mock Character for EventBus and GridSystem testing
class MockCharacter
{
  public:
  MockCharacter(const std::string& _name = "MockChar");
  ~MockCharacter();

  // EventBus interface
  std::string TypeName() const;
  int		  GetCurrentHP() const;

  int  GetMaxHP() const;
  void SetHP(int newHP);

  // GridSystem interface
  Math::vec2 GetGridPosition() const;
  void		 SetGridPosition(Math::vec2 pos);

  // Initiative system interface (Week 4)
  bool IsAlive() const
  {
	return hp > 0;
  }

  int GetSpeed() const
  {
	return speed;
  }

  void SetSpeed(int newSpeed)
  {
	speed = newSpeed;
	if (statsComp)
	{
	  statsComp->SetSpeed(newSpeed);
	}
  }

  MockStatsComponent* GetStatsComponent()
  {
	return statsComp;
  }

  private:
  std::string		  name;
  int				  hp, maxHP;
  Math::vec2		  gridPos;
  int				  speed = 10; // Default speed
  MockStatsComponent* statsComp;
};

// Mock Logger for DebugConsole testing
class MockLogger
{
  public:
  void LogEvent(const std::string& msg);
  void LogError(const std::string& msg);
  void LogDebug(const std::string& msg);

  std::vector<std::string> GetEvents() const;
  std::vector<std::string> GetErrors() const;
  void					   Clear();

  private:
  std::vector<std::string> events, errors, debug;
};