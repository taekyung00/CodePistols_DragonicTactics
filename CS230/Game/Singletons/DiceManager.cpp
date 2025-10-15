#include "DiceManager.h"
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
    NdS.erase(std::remove_if(NdS.begin(), NdS.end(), ::isspace), NdS.end());    //��������
    
    size_t dD = NdS.find_first_of("dD");    //d�Ǵ� D�� ��ġ ã��.
    if (dD == std::string::npos) {
        lastRolls.clear();
        LogRoll(notation, 0);
        return 0;
    }

    int count = 0;
    int sides = 0;
    int mod = 0;
    
    try {
        count = std::stoi(NdS.substr(0, dD));   // stoi= string -> int�� ��ȯ��Ű��. �ֻ��� ����. NdS���� N�κа� d���� �ڿ��κ� �и�.
        if (count <= 0) {
            throw;
        }

        //NdS�ڿ� ���� ��ȣ�� ���� ã��.
        size_t sign = NdS.find('+', dD + 1);
        if (sign == std::string::npos)
            sign = NdS.find('-', dD + 1);

        if (sign == std::string::npos) {
            //���� ��ȣ ���� ���. ex) 3d6
            sides = std::stoi(NdS.substr(dD + 1));
        }
        else {
            //��ȣ �ִ� ��� 3d6 + 2
            sides = std::stoi(NdS.substr(dD + 1, sign - (dD + 1)));
            mod = std::stoi(NdS.substr(sign));
        }

        if (sides <= 0) {
            throw;
        }

    }

    catch (...) {
        //���� �Է¿� ������ ���� ���.
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