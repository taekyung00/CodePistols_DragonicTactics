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
  struct RollEntry
  {
	std::string      notation; // e.g. "2d8", "1d6"
	std::vector<int> rolls;    // individual die results
	int              total;    // sum of rolls
  };

  DiceManager();
  ~DiceManager() = default;

  int RollDice(int count, int sides);
  int RollDiceFromString(
	const std::string& notation);
  void					  SetSeed(int seed);
  const std::vector<int>& GetLastRolls() const;
  const std::string&      GetLastNotation() const;
  const std::vector<RollEntry>& GetRollLog() const;

  private:
  void LogRoll(const std::string& notation, int total) const;

  static constexpr size_t MAX_ROLL_LOG = 200;

  private:
  std::mt19937	        rng;
  std::vector<int>      lastRolls;
  std::string           lastNotation;
  std::vector<RollEntry> roll_log_;
};
