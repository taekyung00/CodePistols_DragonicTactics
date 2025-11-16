#pragma once
#include <random>
#include <string>
#include "./Engine/Engine.hpp"
#include <vector>

class DiceManager {
public:
    DiceManager();
    ~DiceManager() = default;

    int  RollDice(int count, int sides);
    int  RollDiceFromString(const std::string& notation);  


    void SetSeed(int seed);
    const std::vector<int>& GetLastRolls() const;

private:
    DiceManager(const DiceManager&) = delete;              
    DiceManager& operator=(const DiceManager&) = delete;   
    DiceManager(DiceManager&&) = delete;
    DiceManager& operator=(DiceManager&&) = delete;


    void LogRoll(const std::string& notation, int total) const;

private:
    std::mt19937       rng;
    std::vector<int>   lastRolls;
};