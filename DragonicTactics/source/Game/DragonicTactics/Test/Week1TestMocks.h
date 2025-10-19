#pragma once
// File: CS230/Game/Test/Week1TestMocks.h
#pragma once
#include "../../../Engine/Vec2.hpp"
#include <string>
#include <vector>
#include <iostream>

// Mock Character for EventBus and GridSystem testing
class MockCharacter {
public:
    MockCharacter(const std::string& _name = "MockChar")
        : name(_name), hp(100), maxHP(100), gridPos{ 0, 0 } {
    }

    // EventBus interface
    std::string TypeName() const { return name; }
    int GetCurrentHP() const { return hp; }
    int GetMaxHP() const { return maxHP; }
    void SetHP(int newHP) { hp = newHP; }

    // GridSystem interface
    Math::vec2 GetGridPosition() const { return gridPos; }
    void SetGridPosition(Math::vec2 pos) { gridPos = pos; }

private:
    std::string name;
    int hp, maxHP;
    Math::vec2 gridPos;
};

// Mock Logger for DebugConsole testing
class MockLogger {
public:
    void LogEvent(const std::string& msg) { events.push_back(msg); }
    void LogError(const std::string& msg) { errors.push_back(msg); }
    void LogDebug(const std::string& msg) { debug.push_back(msg); }

    std::vector<std::string> GetEvents() const { return events; }
    std::vector<std::string> GetErrors() const { return errors; }
    void Clear() { events.clear(); errors.clear(); debug.clear(); }

private:
    std::vector<std::string> events, errors, debug;
};

// Test assertion macros (lightweight)
inline bool ASSERT_TRUE(bool condition) {
    if (!(condition)) { 
        std::cout << " ASSERT_TRUE failed " << std::endl;
        return false; 
    }
    else {
        std::cout << " ASSERT_TRUE successed! " <<std::endl;
        return true;
    }
    
}


inline bool ASSERT_FALSE(bool condition) {
    if ((condition)) {
        std::cout << " ASSERT_FALSE failed"<< std::endl;
        return false;
    }
    else {
        std::cout << " ASSERT_FALSE successed! " <<std::endl;
        return true;
    }
}
template <typename T>
inline bool ASSERT_EQ(T actual, T expected) {
    if ((actual) != (expected)) {
        std::cout << " ASSERT_EQ failed"<< std::endl;
        return false;
    }
    else {
        std::cout << " ASSERT_EQ successed! "  << std::endl;
        return true;
    }
}
template <typename T>
inline bool ASSERT_NE(T actual, T expected) {
    if ((actual) == (expected)) {
        std::cout << " ASSERT_NE failed: " << std::endl;
        return false;
    }
    else {
        std::cout << " ASSERT_NE successed! " << std::endl;
        return true;
    }
}
template <typename T>
inline bool ASSERT_GE(T actual, T minimum) {
    if ((actual) < (minimum)) {
        std::cout << "ASSERT_GE failed: " << std::endl;
        return false;
    }
    else {
        std::cout << " ASSERT_GE successed! " << std::endl;
        return true;
    }
}
template <typename T>
inline bool ASSERT_LE(T actual, T maximum) {
    if ((actual) > (maximum)) {
        std::cout << "ASSERT_LE failed: " << std::endl;
		return false;
    }
	else {
        std::cout << " ASSERT_LE successed! "  << std::endl;
        return true;
    }
}