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

}

const std::vector<int>& DiceManager::GetLastRolls() const {
    // TODO: 직전 굴림 개별 값 반환
    return lastRolls;
}

int DiceManager::RollDice(int count, int sides) {
    lastRolls.clear();
    if (count <= 0);
}

bool DiceManager::ParseDiceString(const std::string& s, int& count, int& sides, int& mod) {
}

int DiceManager::RollDiceFromString(const std::string& notation) {
}

void DiceManager::LogRoll(const std::string& notation, int total) const {
}
