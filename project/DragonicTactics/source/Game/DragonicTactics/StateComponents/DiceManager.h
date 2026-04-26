/**
 * \file
 * \author Ginam Park
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "./Engine/Component.h"
#include "./Engine/Engine.h"
#include <random>
#include <string>
#include <vector>

class DiceManager : public CS230::Component
{
  public:
  DiceManager();
  ~DiceManager() = default;

  int RollDice(int count, int sides);
  int RollDiceFromString(
	const std::string& notation); 
  void					  SetSeed(int seed);
  const std::vector<int>& GetLastRolls() const;

  private:
  void LogRoll(const std::string& notation, int total) const;

  private:
  std::mt19937	   rng;
  std::vector<int> lastRolls;
};
