#include "pch.h"

#include "Week1TestMocks.h"

MockCharacter::MockCharacter(const std::string& _name) : name(_name), hp(100), maxHP(100), gridPos{ 0, 0 }, speed(10)
{
  statsComp = new MockStatsComponent(speed);
}

MockCharacter::~MockCharacter()
{
  delete statsComp;
}

std::string MockCharacter::TypeName() const
{
  return name;
}

int MockCharacter::GetCurrentHP() const
{
  return hp;
}

int MockCharacter::GetMaxHP() const
{
  return maxHP;
}

void MockCharacter::SetHP(int newHP)
{
  hp = newHP;
}

Math::vec2 MockCharacter::GetGridPosition() const
{
  return gridPos;
}

void MockCharacter::SetGridPosition(Math::vec2 pos)
{
  gridPos = pos;
}

void MockLogger::LogEvent(const std::string& msg)
{
  events.push_back(msg);
}

void MockLogger::LogError(const std::string& msg)
{
  errors.push_back(msg);
}

void MockLogger::LogDebug(const std::string& msg)
{
  debug.push_back(msg);
}

std::vector<std::string> MockLogger::GetEvents() const
{
  return events;
}

std::vector<std::string> MockLogger::GetErrors() const
{
  return errors;
}

void MockLogger::Clear()
{
  events.clear();
  errors.clear();
  debug.clear();
}
