## Week 5: Developer E - Testing & Build Preparation

**Goal**: Create comprehensive test suite (80+ tests), prepare stable build for Playtest 1

**Foundation**:

- Week 1-4 test infrastructure (TestAssert, Week1TestMocks, Week3TestMocks)
- All implemented systems need test coverage
- CMake build system for build automation

**Files to Create**:

```
DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.h (new file)
DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.h (new file)
DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/PlaytestBuild.h (new file)
DragonicTactics/source/Game/DragonicTactics/Test/PlaytestBuild.cpp (new file)
```

**Files to Modify**:

```
DragonicTactics/CMakeLists.txt (add test configuration)
All existing test files (organize and expand coverage)
```

### Implementation Tasks (Testing & Build)

#### **Task 1: Comprehensive Test Suite**

**Goal**: Achieve 80+ tests covering all Week 1-5 systems

**Steps**:

1. Audit existing tests (Week 1-4 test files)
2. Identify gaps in test coverage
3. Write missing unit tests for each system
4. Write integration tests for system interactions
5. Create automated test runner

**Test Coverage Target**:

- **Character System**: 15 tests
- **Combat System**: 12 tests
- **Spell System**: 10 tests
- **Grid System**: 8 tests
- **Turn Manager**: 8 tests
- **Event System**: 6 tests
- **UI System**: 8 tests
- **VFX System**: 5 tests
- **Game Flow**: 8 tests
- **Total**: 80+ tests

**Why this matters**: Comprehensive testing catches bugs early, ensures system stability, and gives confidence that the playtest build won't crash.

---

#### **Task 2: Test Runner & Automation**

**Goal**: Create automated test runner to execute all tests with single command

**Steps**:

1. Create TestRunner singleton to manage all tests
2. Implement test registration system
3. Add test result reporting (passed/failed/skipped)
4. Create test suites for different categories
5. Add command-line test execution

**Test Runner Features**:

- Run all tests with single command
- Filter tests by category (unit, integration, system)
- Generate test report (console + log file)
- Return non-zero exit code on failure (for CI)

---

#### **Task 3: Integration Test Scenarios**

**Goal**: Create realistic end-to-end integration tests

**Steps**:

1. Write "Full Combat" integration test (Dragon vs Fighter to victory)
2. Write "All Spells" integration test (cast all 3 spells in battle)
3. Write "Death Handling" integration test (character death + game over)
4. Write "Turn System" integration test (10 turns with proper order)
5. Write "UI Integration" integration test (all UI elements update correctly)

**Integration Test Requirements**:

- Test must run without manual input
- Test must verify expected outcomes
- Test must clean up resources properly
- Test must complete in < 5 seconds

---

#### **Task 4: Playtest Build Preparation**

**Goal**: Prepare stable, playable build for external playtesting

**Steps**:

1. Create Release build configuration (optimizations enabled)
2. Verify all assets bundle correctly
3. Test build on clean machine (no dev environment)
4. Create playtest instructions document
5. Prepare feedback collection form

**Build Requirements**:

- No crashes during 10-minute play session
- All 3 Dragon spells functional
- Fighter AI makes reasonable decisions
- Victory/defeat screens display correctly
- Performance: 60+ FPS on target hardware

---

#### **Task 5: Bug Tracking & Issue Management**

**Goal**: Track and prioritize bugs found during testing

**Steps**:

1. Set up bug tracking system (could be simple text file or GitHub Issues)
2. Categorize bugs by severity (Critical, Major, Minor)
3. Triage bugs for Week 5 fixes vs post-playtest
4. Fix critical bugs blocking playtest
5. Document known issues for playtesters

**Bug Severity Levels**:

- **Critical**: Crashes, soft-locks, game-breaking bugs (MUST FIX)
- **Major**: Significant gameplay impact (fix if time permits)
- **Minor**: Visual glitches, minor issues (defer to Week 6)

---

### Implementation Examples (Testing & Build)

#### **Example 1: TestRunner Singleton**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.h
#pragma once
#include <string>
#include <vector>
#include <functional>

// Test function signature: returns true if passed
using TestFunction = std::function<bool()>;

struct TestCase {
    std::string name;
    std::string category; // "Unit", "Integration", "System"
    TestFunction function;
};

// Manages and executes all tests
class TestRunner {
public:
    // Singleton access
    static TestRunner& Instance();

    // Test registration (public interface - SnakeCase)
    void RegisterTest(const std::string& name, const std::string& category, TestFunction test_func);

    // Test execution
    int RunAllTests();
    int RunCategory(const std::string& category);
    int RunSingleTest(const std::string& name);

    // Reporting
    void PrintResults() const;
    std::string GenerateReport() const;

    // Statistics
    int GetPassedCount() const { return passed_count_; }
    int GetFailedCount() const { return failed_count_; }
    int GetTotalCount() const { return static_cast<int>(tests_.size()); }

private:
    TestRunner() = default;
    ~TestRunner() = default;
    TestRunner(const TestRunner&) = delete;
    TestRunner& operator=(const TestRunner&) = delete;

    // Test storage (private - snake_case)
    std::vector<TestCase> tests_;
    int passed_count_ = 0;
    int failed_count_ = 0;

    // Test execution helpers
    bool run_test(const TestCase& test);
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/TestRunner.cpp
#include "TestRunner.h"
#include "../../../Engine/Engine.hpp"
#include <iostream>

TestRunner& TestRunner::Instance() {
    static TestRunner instance;
    return instance;
}

void TestRunner::RegisterTest(const std::string& name, const std::string& category, TestFunction test_func) {
    TestCase test{name, category, test_func};
    tests_.push_back(test);
    Engine::GetLogger().LogVerbose("TestRunner: Registered test '" + name + "' in category '" + category + "'");
}

int TestRunner::RunAllTests() {
    passed_count_ = 0;
    failed_count_ = 0;

    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("    RUNNING ALL TESTS");
    Engine::GetLogger().LogEvent("========================================");

    for (const auto& test : tests_) {
        if (run_test(test)) {
            passed_count_++;
        } else {
            failed_count_++;
        }
    }

    PrintResults();

    return failed_count_; // Return 0 if all passed
}

int TestRunner::RunCategory(const std::string& category) {
    passed_count_ = 0;
    failed_count_ = 0;

    Engine::GetLogger().LogEvent("Running tests in category: " + category);

    for (const auto& test : tests_) {
        if (test.category == category) {
            if (run_test(test)) {
                passed_count_++;
            } else {
                failed_count_++;
            }
        }
    }

    PrintResults();

    return failed_count_;
}

bool TestRunner::run_test(const TestCase& test) {
    Engine::GetLogger().LogEvent("Running: " + test.name);

    try {
        bool result = test.function();
        if (result) {
            Engine::GetLogger().LogEvent("  ✅ PASSED");
        } else {
            Engine::GetLogger().LogError("  ❌ FAILED");
        }
        return result;
    }
    catch (const std::exception& e) {
        Engine::GetLogger().LogError("  ❌ EXCEPTION: " + std::string(e.what()));
        return false;
    }
}

void TestRunner::PrintResults() const {
    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("    TEST RESULTS");
    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("Total: " + std::to_string(GetTotalCount()));
    Engine::GetLogger().LogEvent("Passed: " + std::to_string(passed_count_));
    Engine::GetLogger().LogEvent("Failed: " + std::to_string(failed_count_));

    if (failed_count_ == 0) {
        Engine::GetLogger().LogEvent("✅ ALL TESTS PASSED");
    } else {
        Engine::GetLogger().LogError("❌ SOME TESTS FAILED");
    }
    Engine::GetLogger().LogEvent("========================================");
}

std::string TestRunner::GenerateReport() const {
    std::stringstream report;
    report << "Test Report\n";
    report << "===========\n";
    report << "Total: " << GetTotalCount() << "\n";
    report << "Passed: " << passed_count_ << "\n";
    report << "Failed: " << failed_count_ << "\n";
    return report.str();
}
```

---

#### **Example 2: Integration Test - Full Combat**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.cpp

bool TestFullCombatIntegration() {
    Engine::GetLogger().LogEvent("=== FULL COMBAT INTEGRATION TEST ===");

    // Initialize battle
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Verify initial state
    if (!dragon->IsAlive() || !fighter->IsAlive()) {
        Engine::GetLogger().LogError("Characters not alive at start");
        return false;
    }

    // Simulate combat (Dragon always attacks)
    int turn_limit = 20;
    for (int turn = 0; turn < turn_limit; ++turn) {
        Character* current = TurnManager::Instance().GetCurrentCharacter();

        if (!dragon->IsAlive() || !fighter->IsAlive()) {
            break; // Battle ended
        }

        // Execute action
        if (current == dragon) {
            dragon->PerformAttack(fighter);
        } else {
            fighter->PerformAttack(dragon);
        }

        battle.EndCurrentTurn();
    }

    // Verify battle ended (one character died)
    bool battle_ended = !dragon->IsAlive() || !fighter->IsAlive();
    if (!battle_ended) {
        Engine::GetLogger().LogError("Battle didn't end after " + std::to_string(turn_limit) + " turns");
        battle.Unload();
        return false;
    }

    // Verify victory conditions triggered
    battle.CheckVictoryConditions();

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Full combat integration test passed");
    return true;
}
```

---

### Rigorous Testing (Testing & Build)

#### **Test 1: TestRunner Registration**

```cpp
bool TestTestRunnerRegistration() {
    TestRunner& runner = TestRunner::Instance();

    int initial_count = runner.GetTotalCount();

    runner.RegisterTest("Dummy Test", "Unit", []() { return true; });

    if (runner.GetTotalCount() != initial_count + 1) {
        Engine::GetLogger().LogError("TestRunner registration failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ TestRunner registration test passed");
    return true;
}
```

#### **Test 2: Test Execution and Results**

```cpp
bool TestTestExecution() {
    TestRunner& runner = TestRunner::Instance();

    // Register passing test
    runner.RegisterTest("Passing Test", "Unit", []() { return true; });

    // Register failing test
    runner.RegisterTest("Failing Test", "Unit", []() { return false; });

    int failures = runner.RunCategory("Unit");

    if (failures == 0) {
        Engine::GetLogger().LogError("TestRunner should have detected failures");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Test execution test passed");
    return true;
}
```

#### **Test 3: Category Filtering**

```cpp
bool TestCategoryFiltering() {
    TestRunner& runner = TestRunner::Instance();

    runner.RegisterTest("Unit Test 1", "Unit", []() { return true; });
    runner.RegisterTest("Integration Test 1", "Integration", []() { return true; });

    int unit_failures = runner.RunCategory("Unit");
    int integration_failures = runner.RunCategory("Integration");

    if (unit_failures != 0 || integration_failures != 0) {
        Engine::GetLogger().LogError("Category filtering failed");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Category filtering test passed");
    return true;
}
```

#### **Test 4: Build Verification**

```cpp
bool TestBuildVerification() {
    // Verify all required systems are present
    bool grid_ok = (GetComponent<GridSystem>() != nullptr);
    bool turn_manager_ok = true; // TurnManager::Instance() works

    if (!grid_ok || !turn_manager_ok) {
        Engine::GetLogger().LogError("Build verification failed - missing systems");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Build verification test passed");
    return true;
}
```

#### **Test 5: Performance Benchmark**

```cpp
bool TestPerformanceBenchmark() {
    // Measure frame time for 100 updates
    auto start = std::chrono::high_resolution_clock::now();

    BattleState battle;
    battle.Load();

    for (int i = 0; i < 100; ++i) {
        battle.Update(0.016); // 60 FPS frame time
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    battle.Unload();

    // Should complete in < 2 seconds
    if (duration > 2000) {
        Engine::GetLogger().LogError("Performance too slow: " + std::to_string(duration) + "ms");
        return false;
    }

    Engine::GetLogger().LogEvent("✅ Performance benchmark test passed (" +
                                  std::to_string(duration) + "ms)");
    return true;
}
```

---

### Daily Breakdown (Developer E)

#### **Monday (7-8 hours)**

- Audit existing tests from Weeks 1-4 (1.5 hrs)
- Identify test coverage gaps (1 hr)
- Write 15+ missing unit tests (3 hrs)
- Organize tests into categories (1 hr)
- **Deliverable**: 60+ tests with improved coverage

#### **Tuesday (7-8 hours)**

- Create TestRunner singleton (2 hrs)
- Implement test registration and execution (2 hrs)
- Add test reporting and statistics (1 hr)
- Write 10+ integration tests (2.5 hrs)
- **Deliverable**: TestRunner complete, 70+ tests

#### **Wednesday (6-7 hours)**

- Write full combat integration test (1.5 hrs)
- Write all-spells integration test (1.5 hrs)
- Write death handling integration test (1 hr)
- Write turn system integration test (1 hr)
- Write UI integration test (1.5 hrs)
- **Deliverable**: 5 major integration tests, 75+ tests total

#### **Thursday (6-7 hours)**

- Create Release build configuration (1 hr)
- Test build on clean machine (2 hrs)
- Fix critical bugs found in testing (2.5 hrs)
- Write playtest instructions (1 hr)
- **Deliverable**: Stable playtest build ready

#### **Friday (4-5 hours)**

- Run full test suite (80+ tests) (1 hr)
- Create playtest feedback form (30 min)
- Prepare playtest demo environment (1 hr)
- Final bug triage and documentation (1.5 hrs)
- **Deliverable**: Playtest 1 ready for execution

---
