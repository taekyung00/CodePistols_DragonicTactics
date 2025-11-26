#pragma once
#include <random>
#include <string>
#include "./Engine/Component.h"
#include "./Engine/Engine.hpp"
#include <vector>

class DiceManager : public CS230::Component{
public:
    DiceManager();
    ~DiceManager() = default;

    int  RollDice(int count, int sides);
    int  RollDiceFromString(const std::string& notation);  // TODO NdS +-M ������. RollDice���, ParseDiceString�� ����ؼ�
                                                           // ��Ȯ�� �������� Ȯ���ϱ�. if(!ParseDiceString)
                                                           // ���߿��� RollDiceFromString�� ������� ���� ���ɼ��� �ִ�. �׷��� �ؾ���.����
    //string �ʿ��� �κи� ������ --> 
    void SetSeed(int seed);
    const std::vector<int>& GetLastRolls() const;

private:
    void LogRoll(const std::string& notation, int total) const;

private:
    std::mt19937       rng;
    std::vector<int>   lastRolls;
};
