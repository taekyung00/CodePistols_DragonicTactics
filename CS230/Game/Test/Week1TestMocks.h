#pragma once
// File: CS230/Game/Test/Week1TestMocks.h
#pragma once
#include "../../Engine/Vec2.h"
#include <string>
#include <vector>
#include <iostream>

// Mock Character for EventBus and GridSystem testing
class MockCharacter {
public:
    MockCharacter(const std::string& name = "MockChar")
        : name(name), hp(100), maxHP(100), gridPos{ 0, 0 } {
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
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cout << " ASSERT_TRUE failed: " << #condition << std::endl; \
        return false; \
    }

#define ASSERT_FALSE(condition) \
    if ((condition)) { \
        std::cout << " ASSERT_FALSE failed: " << #condition << std::endl; \
        return false; \
    }

#define ASSERT_EQ(actual, expected) \
    if ((actual) != (expected)) { \
        std::cout << " ASSERT_EQ failed: " << #actual << " = " << (actual) \
                  << ", expected " << (expected) << std::endl; \
        return false; \
    }

#define ASSERT_NE(actual, expected) \
    if ((actual) == (expected)) { \
        std::cout << " ASSERT_NE failed: " << #actual << " = " << (actual) \
                  << ", expected NOT " << (expected) << std::endl; \
        return false; \
    }

#define ASSERT_GE(actual, minimum) \
    if ((actual) < (minimum)) { \
        std::cout << "ASSERT_GE failed: " << #actual << " = " << (actual) \
                  << ", expected >= " << (minimum) << std::endl; \
        return false; \
    }

#define ASSERT_LE(actual, maximum) \
    if ((actual) > (maximum)) { \
        std::cout << "ASSERT_LE failed: " << #actual << " = " << (actual) \
                  << ", expected <= " << (maximum) << std::endl; \
        return false; \
    }