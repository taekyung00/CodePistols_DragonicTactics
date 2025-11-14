## Week 5 Integration Tests (Wednesday & Friday)

### Wednesday Mid-Week Integration Check

**Goal**: Verify all Week 5 systems integrate correctly before final push

**Participants**: All 5 developers

**Duration**: 60 minutes

**Test Scenarios**:

1. **Visual Effects Integration** (Developer A)
   - Verify damage numbers appear when characters take damage
   - Verify spell effects trigger on spell cast
   - Verify attack animations play correctly

2. **Game Flow Integration** (Developer B)
   - Verify character death triggers properly
   - Verify victory/defeat screens appear
   - Verify battle statistics track correctly

3. **UI Integration** (Developer C)
   - Verify health bars update on damage
   - Verify turn indicator follows active character
   - Verify spell selection UI appears and responds to input

4. **BattleState Integration** (Developer D)
   - Verify full game loop runs without crashes
   - Verify input handling works for all actions
   - Verify turn transitions work correctly

5. **Testing Integration** (Developer E)
   - Run automated test suite (should have 75+ tests)
   - Verify no critical bugs blocking playtest
   - Identify any integration issues

**Success Criteria**:

- ✅ All 5 developer systems integrated without crashes
- ✅ No critical bugs found
- ✅ 75+ automated tests passing
- ✅ Full combat loop completes successfully

---

### Friday Playtest 1 Integration Test

**Goal**: Final verification before external playtest

**Participants**: All 5 developers + external playtesters

**Duration**: 90 minutes (30 min dev test + 60 min external playtest)

#### Developer Pre-Playtest Check (30 minutes)

**Full Combat Scenario**:

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/Test/Week5IntegrationTests.cpp

bool Week5FinalIntegrationTest() {
    Engine::GetLogger().LogEvent("=== WEEK 5 FINAL INTEGRATION TEST ===");

    // Test 1: Battle Initialization
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Verify all systems initialized
    auto grid = battle.GetComponent<GridSystem>();
    if (!grid) {
        Engine::GetLogger().LogError("GridSystem not initialized");
        return false;
    }

    // Test 2: UI Systems
    UIManager& ui = UIManager::Instance();
    ui.UpdateTurnIndicator(dragon);

    // Test 3: Visual Effects
    VFXManager& vfx = VFXManager::Instance();
    vfx.SpawnDamageNumber({100, 100}, 25, DamageType::Fire, false);

    // Test 4: Full Combat Loop
    int turn_count = 0;
    while (dragon->IsAlive() && fighter->IsAlive() && turn_count < 20) {
        Character* current = TurnManager::Instance().GetCurrentCharacter();

        // Execute action
        if (current == dragon) {
            // Dragon uses spells
            if (turn_count == 0) {
                SpellSystem::Instance().CastSpell(dragon, "Fireball", fighter->GetGridPosition());
            } else if (turn_count == 2) {
                SpellSystem::Instance().CastSpell(dragon, "CreateWall", {4, 4});
            } else if (turn_count == 4) {
                SpellSystem::Instance().CastSpell(dragon, "LavaPool", {5, 5});
            } else {
                dragon->PerformAttack(fighter);
            }
        } else {
            // Fighter uses AI
            fighter->DecideAction();
        }

        battle.EndCurrentTurn();
        turn_count++;

        // Update all systems
        ui.Update(0.016);
        vfx.Update(0.016);
    }

    // Test 5: Victory/Defeat
    battle.CheckVictoryConditions();

    // Verify statistics tracked
    BattleStatistics& stats = BattleStatistics::Instance();
    if (stats.GetTurns() != turn_count) {
        Engine::GetLogger().LogError("Statistics tracking failed");
        return false;
    }

    battle.Unload();

    Engine::GetLogger().LogEvent("========================================");
    Engine::GetLogger().LogEvent("✅ ALL WEEK 5 INTEGRATION TESTS PASSED");
    Engine::GetLogger().LogEvent("========================================");

    return true;
}
```

**Verification Checklist**:

- [ ] Battle initializes without errors
- [ ] Dragon and Fighter spawn at correct positions
- [ ] Grid displays correctly
- [ ] Health bars appear and update
- [ ] Turn indicator shows active character
- [ ] All 3 Dragon spells cast successfully
  - [ ] Fireball deals damage and shows explosion effect
  - [ ] CreateWall creates wall tiles
  - [ ] LavaPool creates lava tiles with damage
- [ ] Fighter AI makes decisions
- [ ] Damage numbers appear on hits
- [ ] Attack animations play
- [ ] Character dies when HP reaches 0
- [ ] Victory/defeat screen appears
- [ ] Battle statistics display correctly
- [ ] No crashes during 10-turn combat
- [ ] 80+ automated tests pass

---

#### External Playtest (60 minutes)

**Playtest Instructions** (hand to playtesters):

```
Dragonic Tactics - Playtest 1 Instructions

Welcome to Playtest 1 of Dragonic Tactics!

GOAL:
Defeat the Fighter using your Dragon's spells and attacks.

CONTROLS:
- Arrow Keys: Move grid cursor
- 1: Cast Fireball (damages area)
- 2: Cast Create Wall (creates wall tiles)
- 3: Cast Lava Pool (creates damaging lava)
- A: Melee Attack
- M: Move character
- Enter: Confirm action
- ESC: Cancel action

HOW TO PLAY:
1. On your turn, select an action (spell or attack)
2. Use arrow keys to select target tile
3. Press Enter to confirm
4. Fighter will automatically take its turn
5. Defeat the Fighter to win!

WHAT TO LOOK FOR:
- Any crashes or freezes
- Confusing UI or controls
- Balance issues (too easy/hard?)
- Visual glitches
- Missing features you expected

Please report ALL issues, no matter how small!
```

**Feedback Collection Form**:

```
Playtest 1 Feedback Form

Name (optional): _______________
Date: _______________

Rate your experience (1-5):
[ ] Overall enjoyment
[ ] Controls/UI clarity
[ ] Visual polish
[ ] Combat balance
[ ] Game difficulty

Did you encounter any bugs? (describe):




What did you like most?




What needs improvement?




Additional comments:




```

---

## Week 5 Deliverables & Verification

### Final Deliverables (End of Week 5)

**Deadline**: Friday Week 5, 5:00 PM

#### **Developer A Deliverables**

- [ ] VFXManager singleton complete
- [ ] Damage numbers functional
- [ ] All 3 spell visual effects working (Fireball, CreateWall, LavaPool)
- [ ] Attack visual feedback complete
- [ ] Grid highlights and hover effects
- [ ] 5+ VFX tests passing

#### **Developer B Deliverables**

- [ ] Character death handling complete
- [ ] BattleStatistics singleton tracking all metrics
- [ ] Victory/defeat conditions detecting correctly
- [ ] VictoryScreen GameState implemented
- [ ] DefeatScreen GameState implemented
- [ ] 5+ game flow tests passing

#### **Developer C Deliverables**

- [ ] UIManager singleton complete
- [ ] Health bars displaying and updating
- [ ] Turn indicator functional
- [ ] Spell selection UI complete
- [ ] Grid tile highlights and tooltips
- [ ] 5+ UI tests passing

#### **Developer D Deliverables**

- [ ] BattleState full integration complete
- [ ] Input handling state machine functional
- [ ] Action execution pipeline working
- [ ] All Week 1-4 systems integrated
- [ ] Battle phase transitions working
- [ ] 5+ integration tests passing

#### **Developer E Deliverables**

- [ ] 80+ comprehensive tests written
- [ ] TestRunner singleton complete
- [ ] All integration tests passing
- [ ] Stable Release build prepared
- [ ] Playtest instructions written
- [ ] Feedback form created
- [ ] Bug tracking system set up

---

### Integration Verification

**Run Full Test Suite**:

```bash
# Build
cmake --preset windows-debug
cmake --build --preset windows-debug

# Run all tests
./build/windows-debug/dragonic_tactics.exe --test all

# Expected output:
========================================
    RUNNING ALL TESTS
========================================
Running: TestEventBusPublish
  ✅ PASSED
Running: TestDiceManager3d6
  ✅ PASSED
...
[80+ tests]
...
========================================
    TEST RESULTS
========================================
Total: 82
Passed: 82
Failed: 0
✅ ALL TESTS PASSED
========================================
```

**Manual Playtest Verification**:

```bash
# Run game in Release mode
cmake --preset windows-release
cmake --build --preset windows-release
./build/windows-release/dragonic_tactics.exe
```

**Verification Steps**:

1. Launch game
2. Navigate to BattleState (Main Menu → Battle)
3. Play full combat:
   - Cast Fireball on Fighter
   - Cast Create Wall to block path
   - Cast Lava Pool near Fighter
   - Attack Fighter until defeated
4. Verify victory screen appears
5. Check statistics displayed correctly
6. No crashes occurred

---

### Success Criteria

Week 5 is complete when:

1. ✅ **All Visual Effects Working**: Damage numbers, spell effects, attack animations
2. ✅ **Complete Game Flow**: Death handling, victory/defeat screens, statistics tracking
3. ✅ **Polished UI**: Health bars, turn indicator, spell selection, grid highlights
4. ✅ **Full BattleState Integration**: All Week 1-4 systems working together
5. ✅ **80+ Tests Passing**: Comprehensive test coverage with all tests green
6. ✅ **Stable Build**: No crashes during 10-minute play session
7. ✅ **Playtest Ready**: Instructions, feedback form, and build prepared
8. ✅ **All Spells Functional**: Fireball, CreateWall, LavaPool cast successfully
9. ✅ **Fighter AI Working**: Makes autonomous decisions
10. ✅ **Victory/Defeat Triggers**: Game ends correctly when battle concludes

---

## Playtest 1 Execution Plan

### Schedule (Friday Week 5, 2:00 PM - 3:30 PM)

**2:00 PM - 2:15 PM**: Setup & Introduction (15 min)
- Set up 3-5 playtesting stations
- Brief playtesters on goals and controls
- Distribute feedback forms

**2:15 PM - 3:00 PM**: Playtesting Session (45 min)
- Playtesters play Dragon vs Fighter battle
- Developers observe and take notes
- Developers available for critical bug fixes only (no help unless game-breaking)

**3:00 PM - 3:15 PM**: Feedback Collection (15 min)
- Collect completed feedback forms
- Brief verbal feedback session
- Thank playtesters

**3:15 PM - 3:30 PM**: Developer Debrief (15 min)
- Discuss major findings
- Prioritize Week 6 improvements
- Celebrate Playtest 1 completion!

---

### Playtester Profiles

**Target Playtesters** (3-5 people):

- **Playtester 1**: Experienced with tactical RPGs (Baldur's Gate 3, XCOM)
- **Playtester 2**: Casual gamer (some RPG experience)
- **Playtester 3**: New to tactical RPGs (fresh perspective)
- **Playtester 4 (optional)**: Game developer (technical feedback)
- **Playtester 5 (optional)**: Friend/family (honest feedback)

---

### Known Issues to Document

**Before Playtest, Document These Known Issues**:

```
Known Issues (Playtest 1)
=========================

CRITICAL (Game-Breaking):
- [None expected - fixed during Week 5]

MAJOR (Significant Impact):
- [TBD based on Wednesday integration test]

MINOR (Cosmetic/Polish):
- Limited spell animations (placeholders)
- No sound effects
- Basic UI layout
- Fighter AI uses simple logic (only attacks, no strategy)
- Only 1v1 combat supported
- No save/load functionality

FUTURE FEATURES (Not Implemented):
- Cleric, Wizard, Rogue characters (Week 11-15)
- Advanced AI (Week 16-20)
- More Dragon spells (Week 6-10)
- Multiplayer (not planned)
```

---

### Post-Playtest Actions

**Immediate (Friday Evening)**:

1. Compile all feedback forms
2. Categorize bugs by severity
3. Create Week 6 priority list

**Week 6 Preview Based on Playtest 1**:

- **Developer A**: Fix VFX bugs, add more spell effects
- **Developer B**: Improve game flow based on feedback
- **Developer C**: UI/UX improvements from playtest findings
- **Developer D**: Balance adjustments, AI improvements
- **Developer E**: Expand test coverage for found bugs

---

### Celebration & Retrospective

**Week 5 Achievements**:

- ✅ **First Playable Build**: Dragon vs Fighter fully playable
- ✅ **All 3 Dragon Spells**: Fireball, CreateWall, LavaPool working
- ✅ **Complete Game Flow**: Start to victory/defeat
- ✅ **80+ Tests**: Comprehensive test coverage
- ✅ **Foundation Complete**: All core systems integrated

**Team Retrospective Questions**:

1. What went well this week?
2. What could be improved?
3. Any blockers or dependencies issues?
4. What did we learn from the playtest?
5. Are we on track for Playtest 2 (Week 10)?

**Looking Ahead to Week 6**:

- Implement playtest feedback
- Begin Dragon spell expansion (Week 6-10 goal: 10 total spells)
- Start debug tools implementation
- Prepare for Playtest 2 (Week 10)

---

## Week 5 Summary

**Playtest 1 Milestone**: ✅ COMPLETE

This week transformed individual systems into a cohesive, playable game. The Dragon vs Fighter battle is fully functional with spells, AI, visual effects, and complete game flow from start to victory/defeat screen.

**Key Metrics**:

- **Development Time**: 5 weeks (Weeks 1-5)
- **Total Tests**: 80+ passing
- **Lines of Code**: ~5,000+ (estimated)
- **Playable Build**: Ready for external testing
- **Systems Complete**: 15+ (Character, Grid, Combat, Spells, Turn, Events, VFX, UI, Game Flow, etc.)

**Next Milestone**: Playtest 2 (Week 10) - Full Dragon spell arsenal + debug tools + balance improvements

---

**🎉 Congratulations on completing Playtest 1! 🎉**

The foundation is solid. Now it's time to expand, polish, and iterate based on player feedback!

