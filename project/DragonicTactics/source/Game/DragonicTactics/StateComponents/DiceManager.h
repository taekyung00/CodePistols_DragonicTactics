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
<<<<<<< HEAD
	const std::string& notation); 
  void					  SetSeed(int seed);
  const std::vector<int>& GetLastRolls() const;
=======
	const std::string& notation);
  void					  SetSeed(int seed);
  const std::vector<int>& GetLastRolls() const;
  const std::string&      GetLastNotation() const;
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835

  private:
  void LogRoll(const std::string& notation, int total) const;

  private:
  std::mt19937	   rng;
  std::vector<int> lastRolls;
<<<<<<< HEAD
=======
  std::string      lastNotation;
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
};
