## Week 5: Developer D - BattleState Integration

**Goal**: Integrate all Week 1-4 systems into complete BattleState game loop

**Foundation**:

- Week 4 TurnManager for turn-based gameplay
- Week 3 CombatSystem and SpellSystem
- Week 2 GridSystem and pathfinding
- Week 1 EventBus and all core systems

**Files to Create**:

```
DragonicTactics/source/Game/DragonicTactics/States/BattleController.h (new file)
DragonicTactics/source/Game/DragonicTactics/States/BattleController.cpp (new file)
DragonicTactics/source/Game/DragonicTactics/Test/BattleIntegrationTests.cpp (new file)
```

**Files to Modify**:

```
DragonicTactics/source/Game/DragonicTactics/States/BattleState.h (complete integration)
DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp (full game loop)
```

### Implementation Tasks (BattleState Integration)

#### **Task 1: Complete BattleState Game Loop**

**Goal**: Implement full game loop integrating all systems from Weeks 1-4

**Steps**:

1. Implement BattleState::Load() to initialize all systems
2. Create game loop in Update() (input → logic → rendering)
3. Integrate TurnManager for turn-based flow
4. Connect input handling to character actions
5. Implement proper state transitions (Setup → Combat → End)

**Why this matters**: BattleState is the central orchestrator that brings all systems together. Proper integration ensures smooth gameplay and correct system interactions.

---

#### **Task 2: Input Handling System**

**Goal**: Map player input to game actions (movement, attacks, spells)

**Steps**:

1. Implement input state machine (selecting action → selecting target → confirming)
2. Add keyboard controls (WASD for grid navigation, numbers for spell selection)
3. Implement mouse support (click tiles, hover tooltips)
4. Add input validation (prevent invalid actions)
5. Provide visual feedback for all inputs

**Input Mapping**:

- **Arrow Keys**: Navigate grid cursor
- **Space/Enter**: Confirm selection
- **ESC**: Cancel action
- **1-9**: Select spell by number
- **A**: Attack action
- **M**: Move action

---

#### **Task 3: Character Action Pipeline**

**Goal**: Complete pipeline from input to action execution

**Steps**:

1. Implement action selection UI integration
2. Connect spell selection to SpellSystem
3. Handle target selection for spells and attacks
4. Execute actions through CombatSystem
5. Update game state after action completion

**Action Flow**:

1. Player selects action type (Move/Attack/Spell)
2. System validates action is available
3. Player selects target (tile or character)
4. System validates target is valid
5. Action executes through appropriate system
6. Events published, UI updated
7. Turn ends automatically

---

#### **Task 4: System Integration & Coordination**

**Goal**: Ensure all Week 1-4 systems work together seamlessly

**Steps**:

1. Integrate GridSystem with character movement
2. Connect TurnManager to character action flow
3. Integrate EventBus for system communication
4. Connect VFXManager to combat events
5. Integrate UIManager with game state

**System Coordination**:

- **TurnManager** → determines active character
- **GridSystem** → provides valid movement tiles
- **CombatSystem** → resolves attacks
- **SpellSystem** → executes spells
- **VFXManager** → displays effects
- **UIManager** → shows UI elements

---

#### **Task 5: State Management & Transitions**

**Goal**: Manage BattleState phases and transitions to other states

**Steps**:

1. Implement battle phase system (Setup, PlayerTurn, EnemyTurn, BattleEnd)
2. Add phase transition logic
3. Handle victory/defeat state transitions
4. Implement battle restart functionality
5. Add proper cleanup in Unload()

**Battle Phases**:

- **Setup**: Initialize characters, grid, systems
- **PlayerTurn**: Wait for player input and action
- **EnemyTurn**: AI executes fighter actions
- **BattleEnd**: Transition to victory/defeat screen

---

### Implementation Examples (BattleState Integration)

#### **Example 1: Complete BattleState::Load()**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp

void BattleState::Load() {
    Engine::GetLogger().LogEvent("BattleState: Loading...");

    // Initialize grid system (StateComponent)
    AddComponent(new GridSystem());
    auto grid = GetComponent<GridSystem>();

    // Create characters
    dragon = new Dragon({2, 2});
    fighter = new Fighter({6, 6});

    // Add characters to grid
    grid->AddCharacter(dragon, {2, 2});
    grid->AddCharacter(fighter, {6, 6});

    // Add characters to game object manager
    game_object_manager_.Add(dragon);
    game_object_manager_.Add(fighter);

    // Initialize turn manager
    std::vector<Character*> participants = {dragon, fighter};
    TurnManager::Instance().InitializeTurnOrder(participants);

    // Initialize UI
    UIManager::Instance().RegisterHealthBar(new HealthBar(dragon));
    UIManager::Instance().RegisterHealthBar(new HealthBar(fighter));

    // Initialize statistics
    BattleStatistics::Instance().StartNewBattle();

    // Set initial phase
    current_phase_ = BattlePhase::Setup;

    // Start first turn
    TurnManager::Instance().StartNextTurn();
    current_phase_ = BattlePhase::PlayerTurn;

    Engine::GetLogger().LogEvent("BattleState: Loaded successfully");
}
```

---

#### **Example 2: Input Handling State Machine**

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/States/BattleState.h

class BattleState : public CS230::GameState {
public:
    enum class InputState {
        SelectingAction,    // Choosing Move/Attack/Spell
        SelectingTarget,    // Choosing target tile/character
        ConfirmingAction,   // Confirming action
        Executing           // Action in progress (no input)
    };

private:
    InputState input_state_ = InputState::SelectingAction;
    std::string selected_action_;
    Math::ivec2 selected_target_;

    void handle_input_selecting_action();
    void handle_input_selecting_target();
    void handle_input_confirming_action();
};
```

```cpp
// File: DragonicTactics/source/Game/DragonicTactics/States/BattleState.cpp

void BattleState::HandlePlayerTurnInput() {
    switch (input_state_) {
        case InputState::SelectingAction:
            handle_input_selecting_action();
            break;

        case InputState::SelectingTarget:
            handle_input_selecting_target();
            break;

        case InputState::ConfirmingAction:
            handle_input_confirming_action();
            break;

        case InputState::Executing:
            // No input during execution
            break;
    }
}

void BattleState::handle_input_selecting_action() {
    Input& input = Engine::GetInput();

    if (input.IsKeyPressed(InputKey::A)) {
        selected_action_ = "Attack";
        input_state_ = InputState::SelectingTarget;
        Engine::GetLogger().LogEvent("Action selected: Attack");
    }
    else if (input.IsKeyPressed(InputKey::Num1)) {
        selected_action_ = "Fireball";
        input_state_ = InputState::SelectingTarget;
        UIManager::Instance().ShowSpellSelectionUI(false);
        Engine::GetLogger().LogEvent("Action selected: Fireball");
    }
    else if (input.IsKeyPressed(InputKey::M)) {
        selected_action_ = "Move";
        input_state_ = InputState::SelectingTarget;
        Engine::GetLogger().LogEvent("Action selected: Move");
    }
}

void BattleState::handle_input_selecting_target() {
    Input& input = Engine::GetInput();

    // Move cursor
    if (input.IsKeyPressed(InputKey::Up)) {
        cursor_position_.y -= 1;
    }
    if (input.IsKeyPressed(InputKey::Down)) {
        cursor_position_.y += 1;
    }
    if (input.IsKeyPressed(InputKey::Left)) {
        cursor_position_.x -= 1;
    }
    if (input.IsKeyPressed(InputKey::Right)) {
        cursor_position_.x += 1;
    }

    // Confirm target
    if (input.IsKeyPressed(InputKey::Enter)) {
        selected_target_ = cursor_position_;
        execute_action();
        input_state_ = InputState::Executing;
    }

    // Cancel
    if (input.IsKeyPressed(InputKey::Escape)) {
        input_state_ = InputState::SelectingAction;
        selected_action_ = "";
    }
}

void BattleState::execute_action() {
    if (selected_action_ == "Attack") {
        auto grid = GetComponent<GridSystem>();
        Character* target = grid->GetCharacterAt(selected_target_);
        if (target) {
            dragon->PerformAttack(target);
        }
    }
    else if (selected_action_ == "Fireball") {
        // Cast Fireball spell
        SpellSystem::Instance().CastSpell(dragon, "Fireball", selected_target_);
    }
    else if (selected_action_ == "Move") {
        auto grid = GetComponent<GridSystem>();
        grid->MoveCharacter(dragon, selected_target_);
    }

    // End turn after action
    EndCurrentTurn();
    input_state_ = InputState::SelectingAction;
}
```

---

### Rigorous Testing (BattleState Integration)

#### **Test 1: BattleState Load and Initialization**

```cpp
bool TestBattleStateInitialization() {
    BattleState battle;
    battle.Load();

    // Verify systems initialized
    auto grid = battle.GetComponent<GridSystem>();
    if (!grid) {
        Engine::GetLogger().LogError("GridSystem not initialized");
        return false;
    }

    // Verify characters created
    if (!battle.GetDragon() || !battle.GetFighter()) {
        Engine::GetLogger().LogError("Characters not created");
        return false;
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ BattleState initialization test passed");
    return true;
}
```

#### **Test 2: Input State Machine**

```cpp
bool TestInputStateMachine() {
    BattleState battle;
    battle.Load();

    // Start in SelectingAction state
    // Simulate pressing 'A' for attack
    // Should transition to SelectingTarget

    // Simulate pressing Enter
    // Should transition to Executing

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Input state machine test passed");
    return true;
}
```

#### **Test 3: Action Execution Pipeline**

```cpp
bool TestActionExecutionPipeline() {
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Execute attack action
    dragon->PerformAttack(fighter);

    // Verify damage applied
    auto stats = fighter->GetGOComponent<StatsComponent>();
    if (stats->GetCurrentHP() == stats->GetMaxHP()) {
        Engine::GetLogger().LogError("Action execution failed - no damage");
        return false;
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Action execution pipeline test passed");
    return true;
}
```

#### **Test 4: Turn Flow Integration**

```cpp
bool TestTurnFlowIntegration() {
    BattleState battle;
    battle.Load();

    // Turn 1: Dragon's turn
    Character* current = TurnManager::Instance().GetCurrentCharacter();
    if (current->TypeName() != "Dragon" && current->TypeName() != "Fighter") {
        Engine::GetLogger().LogError("Turn flow failed - invalid first character");
        return false;
    }

    // End turn
    battle.EndCurrentTurn();

    // Turn 2: Should switch to other character
    Character* next = TurnManager::Instance().GetCurrentCharacter();
    if (next == current) {
        Engine::GetLogger().LogError("Turn flow failed - same character");
        return false;
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Turn flow integration test passed");
    return true;
}
```

#### **Test 5: Full Combat Integration**

```cpp
bool TestFullCombatIntegration() {
    BattleState battle;
    battle.Load();

    Dragon* dragon = battle.GetDragon();
    Fighter* fighter = battle.GetFighter();

    // Simulate full combat loop
    for (int turn = 0; turn < 10; ++turn) {
        Character* current = TurnManager::Instance().GetCurrentCharacter();

        if (current == dragon) {
            // Dragon attacks
            dragon->PerformAttack(fighter);
        } else {
            // Fighter attacks
            fighter->PerformAttack(dragon);
        }

        battle.EndCurrentTurn();

        // Check victory conditions
        if (fighter->IsDead()) {
            Engine::GetLogger().LogEvent("Dragon victory!");
            break;
        }
        if (dragon->IsDead()) {
            Engine::GetLogger().LogEvent("Fighter victory!");
            break;
        }
    }

    battle.Unload();
    Engine::GetLogger().LogEvent("✅ Full combat integration test passed");
    return true;
}
```

---

### Daily Breakdown (Developer D)

#### **Monday (7-8 hours)**

- Implement complete BattleState::Load() (2 hrs)
- Initialize all systems (Grid, Turn, UI, Stats) (2 hrs)
- Create character instances and placement (1 hr)
- Test initialization sequence (2 hrs)
- **Deliverable**: BattleState loads correctly

#### **Tuesday (7-8 hours)**

- Implement input handling state machine (2.5 hrs)
- Add keyboard controls for all actions (2 hrs)
- Implement cursor movement and selection (1.5 hrs)
- Test input flow (1.5 hrs)
- **Deliverable**: Input system complete

#### **Wednesday (6-7 hours)**

- Implement action execution pipeline (2 hrs)
- Connect spell selection to SpellSystem (1.5 hrs)
- Integrate combat actions with CombatSystem (1.5 hrs)
- Test action execution (2 hrs)
- **Deliverable**: Actions execute correctly

#### **Thursday (6-7 hours)**

- Integrate all Week 1-4 systems (3 hrs)
- Implement battle phase transitions (2 hrs)
- Add victory/defeat detection (1.5 hrs)
- Test system coordination (1.5 hrs)
- **Deliverable**: Full integration complete

#### **Friday (4-5 hours)**

- Write comprehensive integration test suite (2 hrs)
- Final bug fixes and polish (1.5 hrs)
- Test full combat scenarios (1 hr)
- Prepare for playtest (30 min)
- **Deliverable**: BattleState production-ready

---
