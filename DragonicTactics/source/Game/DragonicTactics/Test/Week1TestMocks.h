#pragma once
// File: CS230/Game/Test/Week1TestMocks.h
#pragma once
#include "../../../Engine/Vec2.hpp"
#include <iostream>
#include <string>
#include <vector>

// Mock Character for EventBus and GridSystem testing
class MockCharacter
{
public:
    MockCharacter(const std::string& _name = "MockChar");


    // EventBus interface
    std::string TypeName() const;
    int         GetCurrentHP() const;

    int  GetMaxHP() const;
    void SetHP(int newHP);

    // GridSystem interface
    Math::vec2 GetGridPosition() const;
    void       SetGridPosition(Math::vec2 pos);

private:
    std::string name;
    int         hp, maxHP;
    Math::vec2  gridPos;
};

// Mock Logger for DebugConsole testing
class MockLogger
{
public:
    void LogEvent(const std::string& msg);
    void LogError(const std::string& msg);
    void LogDebug(const std::string& msg);

    std::vector<std::string> GetEvents() const;
    std::vector<std::string> GetErrors() const;
    void                     Clear();

private:
    std::vector<std::string> events, errors, debug;
};