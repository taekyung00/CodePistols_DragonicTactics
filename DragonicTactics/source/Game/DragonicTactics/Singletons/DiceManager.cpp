#include "DiceManager.h"
#include "./Engine/Logger.hpp"
#include <cctype>
#include <sstream>

DiceManager::DiceManager() {
    std::random_device rand;
    rng.seed(rand());
}

DiceManager& DiceManager::Instance() {
    static DiceManager instance;
    return instance;
}

void DiceManager::SetSeed(int seed) {
    rng.seed(static_cast<unsigned int>(seed));
}

const std::vector<int>& DiceManager::GetLastRolls() const {
    return lastRolls;
}

int DiceManager::RollDice(int count, int sides) {
    lastRolls.clear();
    if (count <= 0 || sides <= 0) {
        lastRolls.clear();
        return 0;
    }
    
    std::uniform_int_distribution<int> dice(1, sides);
    lastRolls.clear();
    int sum = 0;

    for (int i = 0; i < count; i++) {
        int roll = dice(rng);
        lastRolls.push_back(roll);
        sum += roll;
    }
    return sum;
}

//bool DiceManager::ParseDiceString(const std::string& s, int& count, int& sides, int& mod) {
//}

int DiceManager::RollDiceFromString(const std::string& notation) {
    std::string NdS = notation;
    NdS.erase(std::remove_if(NdS.begin(), NdS.end(), ::isspace), NdS.end());    //공백제거
    
    size_t dD = NdS.find_first_of("dD");    //d또는 D의 위치 찾기.
    if (dD == std::string::npos) {
        lastRolls.clear();
        LogRoll(notation, 0);
        return 0;
    }

    int count = 0;
    int sides = 0;
    int mod = 0;
    
    try {
        count = std::stoi(NdS.substr(0, dD));   // stoi= string -> int로 변환시키기. 주사위 갯수. NdS에서 N부분과 d이후 뒤에부분 분리.
        if (count <= 0) {
            throw;
        }

        //NdS뒤에 오는 부호가 뭔지 찾기.
        size_t sign = NdS.find('+', dD + 1);
        if (sign == std::string::npos)
            sign = NdS.find('-', dD + 1);

        if (sign == std::string::npos) {
            //따로 부호 없는 경우. ex) 3d6
            sides = std::stoi(NdS.substr(dD + 1));
        }
        else {
            //부호 있는 경우 3d6 + 2
            sides = std::stoi(NdS.substr(dD + 1, sign - (dD + 1)));
            mod = std::stoi(NdS.substr(sign));
        }

        if (sides <= 0) {
            throw;
        }

    }

    catch (...) {
        //만약 입력에 오류가 났을 경우.
        lastRolls.clear();
        LogRoll(notation, 0);
        return 0;
    }

    int total = RollDice(count, sides) + mod;
    LogRoll(notation, total);
    return total;
}

void DiceManager::LogRoll(const std::string& notation, int total) const {
    std::string log = "[Dice] " + notation + " => " + std::to_string(total);

    if (!lastRolls.empty()) {
        log += " [";
        for (size_t i = 0; i < lastRolls.size(); i++) {
            log += std::to_string(lastRolls[i]);
            if (i + 1 < lastRolls.size())
                log += ", ";
        }
        log += "]";
    }

    Engine::GetLogger().LogDebug(log);
}