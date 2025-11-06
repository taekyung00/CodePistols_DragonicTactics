#pragma once
#include <random>
#include <string>
#include "./Engine/Engine.hpp"
#include <vector>

class DiceManager {
public:
    static DiceManager& Instance();
    int  RollDice(int count, int sides);
    int  RollDiceFromString(const std::string& notation);  // TODO NdS +-M 굴리기. RollDice사용, ParseDiceString를 사용해서
                                                           // 정확한 형태인지 확인하기. if(!ParseDiceString)
                                                           // 나중에는 RollDiceFromString을 사용하지 않을 가능성도 있다. 그래도 해야함.ㅇㅇ
    //string 필요한 부분만 빼오기 --> 
    void SetSeed(int seed);
    const std::vector<int>& GetLastRolls() const;          // TODO

private:
    DiceManager();
    ~DiceManager() = default;
    DiceManager(const DiceManager&) = delete;              // 복사 금지 (싱글톤 보장)
    DiceManager& operator=(const DiceManager&) = delete;   // 대입 금지
    DiceManager(DiceManager&&) = delete;
    DiceManager& operator=(DiceManager&&) = delete;

    //static bool ParseDiceString(const std::string& s,int& count, int& sides, int& mod); 없어도 되지않을까? 라는 의견...
    void LogRoll(const std::string& notation, int total) const; // TODO 주사위 결과값 로그에 출력하기. Engine::LOGGER사용.

private:
    std::mt19937       rng;
    std::vector<int>   lastRolls;
};
