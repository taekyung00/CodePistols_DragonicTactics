#include "BattleState.h"
#include "../System/GridSystem.h"
#include "../System/SpellSystem.h"
#include "../Singletons/DataRegistry.h"
#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "../Spells/Fireball.h"
#include "../Spells/CreateWall.h"
#include "../Spells/LavaPool.h"
#include "../../Engine/Engine.h"

BattleState::BattleState()
    : dragon(nullptr)
    , fighter(nullptr)
    , currentPhase(BattlePhase::Setup)
    , turnCount(0)
    , cursorPosition{ 0, 0 }
    , selectingTarget(false)
{
}

BattleState::~BattleState() {
}

void BattleState::Load() {
    Engine::GetLogger().LogEvent("BattleState: Loading");

    GridSystem& grid = Engine::GetGridSystem();
    grid.Initialize(8, 8);

    // Step 2.1b: Load character data
    // Reason: Dragon and Fighter stats from JSON files
    DataRegistry& registry = DataRegistry::GetInstance();
    registry.LoadAllCharacterData();

    // Step 2.1c: Register spells with SpellSystem
    // Reason: Spells must be registered before use
    SpellSystem& spellSys = SpellSystem::GetInstance();
    spellSys.RegisterSpell("Fireball", new Fireball());
    spellSys.RegisterSpell("CreateWall", new CreateWall());
    spellSys.RegisterSpell("LavaPool", new LavaPool());

    // Step 2.1d: Create Dragon (player controlled)
    // Reason: Load from JSON data
    dragon = static_cast<Dragon*>(Character::CreateFromData("dragon", Math::vec2{ 1, 1 }));
    if (dragon) {
        grid.PlaceCharacter(dragon, Math::vec2{ 1, 1 });
        AddGameObject(dragon);  // Add to GameObjectManager
    }

    // Step 2.1e: Create Fighter (enemy)
    fighter = static_cast<Fighter*>(Character::CreateFromData("fighter", Math::vec2{ 6, 6 }));
    if (fighter) {
        grid.PlaceCharacter(fighter, Math::vec2{ 6, 6 });
        AddGameObject(fighter);
    }

    // Step 2.1f: Initialize turn order
    // Reason: Dragon goes first for Week 3 (initiative system in Week 4)
    currentPhase = BattlePhase::PlayerTurn;
    turnCount = 1;

    // Step 2.1g: Subscribe to battle events
    // Reason: Listen for character death events
    EventBus::GetInstance().Subscribe<CharacterDeathEvent>([this](const CharacterDeathEvent& event) {
        Engine::GetLogger().LogEvent("BattleState: " + event.character->TypeName() + " has died");
        CheckBattleEnd();
        });

    Engine::GetLogger().LogEvent("BattleState: Battle started - Dragon vs Fighter");
}

// Step 3.1: Main input handler
// Reason: Route input based on current battle phase
void BattleState::HandleInput() {
    if (currentPhase == BattlePhase::PlayerTurn) {
        HandlePlayerTurnInput();
    }
    else if (currentPhase == BattlePhase::EnemyTurn) {
        HandleEnemyTurnInput();
    }
}

// Step 3.2: Player turn input
// Reason: Dragon movement, spell casting, ability use
void BattleState::HandlePlayerTurnInput() {
    CS230::Input& input = Engine::GetInput();

    // Step 3.2a: Move grid cursor with arrow keys
    // Reason: Player selects tiles for movement/spells
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Up)) {
        cursorPosition.y = std::max(0.0, cursorPosition.y - 1);
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Down)) {
        cursorPosition.y = std::min(7.0, cursorPosition.y + 1);
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Left)) {
        cursorPosition.x = std::max(0.0, cursorPosition.x - 1);
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Right)) {
        cursorPosition.x = std::min(7.0, cursorPosition.x + 1);
    }

    // Step 3.2b: Spell keys (1, 2, 3)
    // Reason: Cast Dragon spells
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Num1)) {
        // Cast Fireball at cursor position
        SpellSystem& spellSys = SpellSystem::GetInstance();
        SpellResult result = spellSys.CastSpell(dragon, "Fireball", cursorPosition);

        if (result.success) {
            Engine::GetLogger().LogEvent("BattleState: Dragon cast Fireball");
        }
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Num2)) {
        // Cast CreateWall at cursor position
        SpellSystem& spellSys = SpellSystem::GetInstance();
        SpellResult result = spellSys.CastSpell(dragon, "CreateWall", cursorPosition);

        if (result.success) {
            Engine::GetLogger().LogEvent("BattleState: Dragon cast CreateWall");
        }
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Num3)) {
        // Cast LavaPool at cursor position
        SpellSystem& spellSys = SpellSystem::GetInstance();
        SpellResult result = spellSys.CastSpell(dragon, "LavaPool", cursorPosition);

        if (result.success) {
            Engine::GetLogger().LogEvent("BattleState: Dragon cast LavaPool");
        }
    }

    // Step 3.2c: End turn (Space)
    // Reason: Player finishes actions
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Space)) {
        EndCurrentTurn();
    }
}

// Step 3.3: Enemy turn input (manual control for Week 3)
// Reason: Manually control Fighter for testing
void BattleState::HandleEnemyTurnInput() {
    CS230::Input& input = Engine::GetInput();

    // Step 3.3a: Move cursor (same as player turn)
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Up)) {
        cursorPosition.y = std::max(0.0, cursorPosition.y - 1);
    }
    // ... (same cursor movement as player turn)

    // Step 3.3b: Fighter abilities (A for melee, S for shield bash)
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::A)) {
        // Melee attack (need adjacent target)
        if (dragon) {
            MeleeAttack attack;
            AbilityResult result = attack.Use(fighter, dragon);

            if (result.success) {
                Engine::GetLogger().LogEvent("BattleState: Fighter used Melee Attack");
            }
        }
    }
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::S)) {
        // Shield Bash (need adjacent target)
        if (dragon) {
            ShieldBash bash;
            AbilityResult result = bash.Use(fighter, dragon);

            if (result.success) {
                Engine::GetLogger().LogEvent("BattleState: Fighter used Shield Bash");
            }
        }
    }

    // Step 3.3c: End turn (Space)
    if (input.IsKeyPressed(CS230::InputKey::Keyboard::Space)) {
        EndCurrentTurn();
    }
}

// Step 4.1: End current turn
// Reason: Switch between player and enemy turns
void BattleState::EndCurrentTurn() {
    if (currentPhase == BattlePhase::PlayerTurn) {
        // Step 4.1a: Refresh Dragon's AP
        // Reason: Full AP at start of turn
        dragon->RefreshActionPoints();

        // Step 4.1b: Switch to enemy turn
        currentPhase = BattlePhase::EnemyTurn;
        Engine::GetLogger().LogEvent("BattleState: Fighter's turn");
    }
    else if (currentPhase == BattlePhase::EnemyTurn) {
        // Step 4.1c: Refresh Fighter's AP
        fighter->RefreshActionPoints();

        // Step 4.1d: Switch to player turn
        currentPhase = BattlePhase::PlayerTurn;
        turnCount++;
        Engine::GetLogger().LogEvent("BattleState: Dragon's turn (Turn " + std::to_string(turnCount) + ")");
    }

    // Step 4.1e: Check if battle ended
    CheckBattleEnd();
}

// Step 4.2: Check battle end conditions
// Reason: Detect when one character dies
void BattleState::CheckBattleEnd() {
    if (dragon && dragon->GetCurrentHP() <= 0) {
        currentPhase = BattlePhase::BattleEnd;
        Engine::GetLogger().LogEvent("BattleState: BATTLE END - Fighter wins!");
    }
    else if (fighter && fighter->GetCurrentHP() <= 0) {
        currentPhase = BattlePhase::BattleEnd;
        Engine::GetLogger().LogEvent("BattleState: BATTLE END - Dragon wins!");
    }
}

// Step 4.3: Get current turn character
// Reason: Helper for UI display
Character* BattleState::GetCurrentTurnCharacter() {
    if (currentPhase == BattlePhase::PlayerTurn) {
        return dragon;
    }
    else if (currentPhase == BattlePhase::EnemyTurn) {
        return fighter;
    }
    return nullptr;
}

// Step 5.1: Update battle state
// Reason: Process input and update game objects
void BattleState::Update(double dt) {
    // Step 5.1a: Handle input
    HandleInput();

    // Step 5.1b: Update all game objects
    // Reason: Character animations, particle effects, etc.
    UpdateGameObjects(dt);  // CS230::GameState method

    // Step 5.1c: Check battle end (in case of events)
    CheckBattleEnd();
}

// Step 5.2: Draw battle scene
// Reason: Render grid, characters, UI
void BattleState::Draw() {
    // Step 5.2a: Render grid
    RenderGrid();

    // Step 5.2b: Render characters
    DrawGameObjects(Engine::GetWindow().GetCameraMatrix());  // CS230::GameState method

    // Step 5.2c: Render UI
    RenderUI();
}

// Step 5.3: Render grid (simple visualization for Week 3)
// Reason: Show 8x8 grid with tile types
void BattleState::RenderGrid() {
    GridSystem& grid = GridSystem::GetInstance();

    // Simple grid rendering (Week 3 basic version)
    // TODO Week 4: Use GridDebugRenderer for better visualization

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Math::vec2 tilePos{ (double)x, (double)y };

            // Get tile type
            TileType type = grid.GetTileType(tilePos);

            // Draw tile (placeholder - use debug renderer in Week 4)
            // ... (simple rectangle drawing based on tile type)
        }
    }

    // Draw cursor
    // ... (simple highlight at cursorPosition)
}

// Step 5.4: Render UI (simple text display for Week 3)
// Reason: Show HP, AP, turn info
void BattleState::RenderUI() {
    CS230::Font& font = Engine::GetFont(Fonts::Default);

    // Draw Dragon HP/AP
    if (dragon) {
        std::string dragonInfo = "Dragon HP: " + std::to_string(dragon->GetCurrentHP()) + "/" + std::to_string(dragon->GetMaxHP());
        dragonInfo += " | AP: " + std::to_string(dragon->GetCurrentActionPoints()) + "/" + std::to_string(dragon->GetMaxActionPoints());
        font.Draw(dragonInfo, Math::vec2{ 10, 550 });
    }

    // Draw Fighter HP/AP
    if (fighter) {
        std::string fighterInfo = "Fighter HP: " + std::to_string(fighter->GetCurrentHP()) + "/" + std::to_string(fighter->GetMaxHP());
        fighterInfo += " | AP: " + std::to_string(fighter->GetCurrentActionPoints()) + "/" + std::to_string(fighter->GetMaxActionPoints());
        font.Draw(fighterInfo, Math::vec2{ 10, 530 });
    }

    // Draw turn info
    std::string turnInfo = "Turn " + std::to_string(turnCount) + " - " +
        (currentPhase == BattlePhase::PlayerTurn ? "Dragon's Turn" : "Fighter's Turn");
    font.Draw(turnInfo, Math::vec2{ 10, 10 });

    // Draw battle end message
    if (currentPhase == BattlePhase::BattleEnd) {
        std::string winMessage = (dragon && dragon->GetCurrentHP() > 0) ? "DRAGON WINS!" : "FIGHTER WINS!";
        font.Draw(winMessage, Math::vec2{ 300, 300 });
    }
}

// Step 5.5: Unload battle state
// Reason: Clean up resources
void BattleState::Unload() {
    Engine::GetLogger().LogEvent("BattleState: Unloading");

    // Unload game objects
    ClearGameObjects();  // CS230::GameState method (deletes dragon, fighter)

    // Clear grid
    GridSystem::GetInstance().Clear();

    dragon = nullptr;
    fighter = nullptr;
}