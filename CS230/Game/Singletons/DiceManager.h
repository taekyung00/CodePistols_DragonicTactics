#pragma once
#include <random>
#include <string>
#include "..\..\Engine\Engine.h"
#include <vector>

class DiceManager {
public:
    static DiceManager& Instance();
    int  RollDice(int count, int sides);
    int  RollDiceFromString(const std::string& notation);  // TODO NdS +-M ������. RollDice���, ParseDiceString�� ����ؼ�
                                                           // ��Ȯ�� �������� Ȯ���ϱ�. if(!ParseDiceString)
                                                           // ���߿��� RollDiceFromString�� ������� ���� ���ɼ��� �ִ�. �׷��� �ؾ���.����
    //string �ʿ��� �κи� ������ --> 
    void SetSeed(int seed);
    const std::vector<int>& GetLastRolls() const;          // TODO

private:
    DiceManager();
    ~DiceManager() = default;
    DiceManager(const DiceManager&) = delete;              // ���� ���� (�̱��� ����)
    DiceManager& operator=(const DiceManager&) = delete;   // ���� ����
    DiceManager(DiceManager&&) = delete;
    DiceManager& operator=(DiceManager&&) = delete;

    //static bool ParseDiceString(const std::string& s,int& count, int& sides, int& mod); ��� ����������? ��� �ǰ�...
    void LogRoll(const std::string& notation, int total) const; // TODO �ֻ��� ����� �α׿� ����ϱ�. Engine::LOGGER���.

private:
    std::mt19937       rng;
    std::vector<int>   lastRolls;
};
