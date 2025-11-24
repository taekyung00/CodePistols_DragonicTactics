#include "TestDiceManager.h"

#include "./Engine/Engine.hpp"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"

#include "./Game/DragonicTactics/StateComponents/DiceManager.h"
#include "./Game/DragonicTactics/Test/TestAssert.h"

#include <iostream>

// ===== Basic Dice Rolling Tests =====

bool TestDiceManager_RollDice()
{
    Engine::GetLogger().LogEvent("=== Test: RollDice ===");
    
    DiceManager dice;
    
    // Roll 1d6
    int result = dice.RollDice(1, 6);
    
    // Result should be between 1 and 6
    ASSERT_GE(result, 1);
    ASSERT_LE(result, 6);
    
    std::cout << "TestDiceManager_RollDice passed (rolled: " << result << ")" << std::endl;
    return true;
}

bool TestDiceManager_RollMultipleDice()
{
    Engine::GetLogger().LogEvent("=== Test: RollMultipleDice ===");
    
    DiceManager dice;
    
    // Roll 3d6
    int result = dice.RollDice(3, 6);
    
    // Result should be between 3 (min) and 18 (max)
    ASSERT_GE(result, 3);
    ASSERT_LE(result, 18);
    
    std::cout << "TestDiceManager_RollMultipleDice passed (rolled: " << result << ")" << std::endl;
    return true;
}

bool TestDiceManager_RollFromString()
{
    Engine::GetLogger().LogEvent("=== Test: RollFromString ===");
    
    DiceManager dice;
    
    // Test "2d8" format
    int result = dice.RollDiceFromString("2d8");
    
    // Result should be between 2 and 16
    ASSERT_GE(result, 2);
    ASSERT_LE(result, 16);
    
    std::cout << "TestDiceManager_RollFromString passed (rolled: " << result << ")" << std::endl;
    return true;
}

// ===== Dice String Parsing Tests =====

bool TestDiceManager_ParseSimpleDice()
{
    Engine::GetLogger().LogEvent("=== Test: Parse Simple Dice ===");
    
    DiceManager dice;
    
    // Test "1d20"
    int result = dice.RollDiceFromString("1d20");
    
    ASSERT_GE(result, 1);
    ASSERT_LE(result, 20);
    
    std::cout << "TestDiceManager_ParseSimpleDice passed" << std::endl;
    return true;
}

bool TestDiceManager_ParseDiceWithModifier()
{
    Engine::GetLogger().LogEvent("=== Test: Parse Dice With Modifier ===");
    
    DiceManager dice;
    
    // Test "2d6+3" format
    int result = dice.RollDiceFromString("2d6+3");
    
    // Result should be between 5 (2+3) and 15 (12+3)
    ASSERT_GE(result, 5);
    ASSERT_LE(result, 15);
    
    std::cout << "TestDiceManager_ParseDiceWithModifier passed (rolled: " << result << ")" << std::endl;
    return true;
}

bool TestDiceManager_ParseInvalidString()
{
    Engine::GetLogger().LogEvent("=== Test: Parse Invalid String ===");
    
    DiceManager dice;
    
    // Test invalid format (should return 0 or handle gracefully)
    int result = dice.RollDiceFromString("invalid");
    
    // Should not crash and return a safe value
    ASSERT_GE(result, 0);
    
    std::cout << "TestDiceManager_ParseInvalidString passed" << std::endl;
    return true;
}

// ===== Seed and Determinism Tests =====

bool TestDiceManager_SetSeed()
{
    Engine::GetLogger().LogEvent("=== Test: SetSeed ===");
    
    DiceManager dice;
    
    // Set seed
    dice.SetSeed(12345);
    
    // Roll twice with same seed
    int roll1 = dice.RollDice(1, 20);
    
    dice.SetSeed(12345); // Reset to same seed
    int roll2 = dice.RollDice(1, 20);
    
    // Should get same result
    ASSERT_EQ(roll1, roll2);
    
    std::cout << "TestDiceManager_SetSeed passed (both rolled: " << roll1 << ")" << std::endl;
    return true;
}

bool TestDiceManager_DeterministicRolls()
{
    Engine::GetLogger().LogEvent("=== Test: Deterministic Rolls ===");
    
    DiceManager dice;
    
    // Set seed and roll sequence
    dice.SetSeed(42);
    int roll1 = dice.RollDice(1, 6);
    int roll2 = dice.RollDice(1, 6);
    int roll3 = dice.RollDice(1, 6);
    
    // Reset seed and roll again
    dice.SetSeed(42);
    int roll4 = dice.RollDice(1, 6);
    int roll5 = dice.RollDice(1, 6);
    int roll6 = dice.RollDice(1, 6);
    
    // Sequences should be identical
    ASSERT_EQ(roll1, roll4);
    ASSERT_EQ(roll2, roll5);
    ASSERT_EQ(roll3, roll6);
    
    std::cout << "TestDiceManager_DeterministicRolls passed" << std::endl;
    return true;
}

bool TestDiceManager_DifferentSeeds()
{
    Engine::GetLogger().LogEvent("=== Test: Different Seeds ===");
    
    DiceManager dice;
    
    // Roll with seed 1
    dice.SetSeed(1);
    std::vector<int> sequence1;
    for (int i = 0; i < 10; ++i) {
        sequence1.push_back(dice.RollDice(1, 20));
    }
    
    // Roll with seed 2
    dice.SetSeed(2);
    std::vector<int> sequence2;
    for (int i = 0; i < 10; ++i) {
        sequence2.push_back(dice.RollDice(1, 20));
    }
    
    // Sequences should be different
    bool allSame = true;
    for (size_t i = 0; i < sequence1.size(); ++i) {
        if (sequence1[i] != sequence2[i]) {
            allSame = false;
            break;
        }
    }
    
    ASSERT_FALSE(allSame);
    
    std::cout << "TestDiceManager_DifferentSeeds passed" << std::endl;
    return true;
}

// ===== Range Tests =====

bool TestDiceManager_RollRange()
{
    Engine::GetLogger().LogEvent("=== Test: Roll Range ===");
    
    DiceManager dice;
    
    // Roll 100 times and check all are in range
    for (int i = 0; i < 100; ++i) {
        int result = dice.RollDice(1, 6);
        ASSERT_GE(result, 1);
        ASSERT_LE(result, 6);
    }
    
    std::cout << "TestDiceManager_RollRange passed (100 rolls all in range)" << std::endl;
    return true;
}

bool TestDiceManager_MaxRoll()
{
    Engine::GetLogger().LogEvent("=== Test: Max Roll ===");
    
    DiceManager dice;
    
    // Roll many times, should eventually hit max
    bool hitMax = false;
    for (int i = 0; i < 1000; ++i) {
        int result = dice.RollDice(1, 6);
        if (result == 6) {
            hitMax = true;
            break;
        }
    }
    
    ASSERT_TRUE(hitMax);
    
    std::cout << "TestDiceManager_MaxRoll passed" << std::endl;
    return true;
}

bool TestDiceManager_MinRoll()
{
    Engine::GetLogger().LogEvent("=== Test: Min Roll ===");
    
    DiceManager dice;
    
    // Roll many times, should eventually hit min
    bool hitMin = false;
    for (int i = 0; i < 1000; ++i) {
        int result = dice.RollDice(1, 6);
        if (result == 1) {
            hitMin = true;
            break;
        }
    }
    
    ASSERT_TRUE(hitMin);
    
    std::cout << "TestDiceManager_MinRoll passed" << std::endl;
    return true;
}
