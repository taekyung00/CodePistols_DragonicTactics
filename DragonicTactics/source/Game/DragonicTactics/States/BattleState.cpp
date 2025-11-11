// #include "BattleState.h"
// #include "./Engine/Engine.hpp"
// #include "./Engine/Logger.hpp"
// #include "./Engine/Input.hpp"
// #include "./Engine/GameStateManager.hpp"
// #include "./Engine/GameObjectManager.h"
// #include "./Game/DragonicTactics/StateComponents/GridSystem.h"
// #include "./Game/DragonicTactics/Types/Events.h"
// #include "../Singletons/DiceManager.h"
// #include "../Singletons/EventBus.h"
// // #include "../Spells/Fireball.h"
// // #include "../Spells/CreateWall.h"
// // #include "../Spells/LavaPool.h"

// BattleState::BattleState()
//     : dragon(nullptr)
//     , fighter(nullptr)
//     , currentPhase(BattlePhase::Setup)
//     , turnCount(0)
//     , cursorPosition{ 0, 0 }
//     , selectingTarget(false)
// {
// }

// BattleState::~BattleState() {
// }

// void BattleState::Load() {
//     Engine::GetLogger().LogEvent("BattleState: Loading");

//     //auto grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

//     // DataRegistry& registry = DataRegistry::GetInstance();
//     // registry.LoadAllCharacterData();

//     // SpellSystem& spellSys = SpellSystem::GetInstance();
//     // spellSys.RegisterSpell("Fireball", new Fireball());
//     // spellSys.RegisterSpell("CreateWall", new CreateWall());
//     // spellSys.RegisterSpell("LavaPool", new LavaPool());

//     // dragon = static_cast<Dragon*>(Character::CreateFromData("dragon", Math::vec2{ 1, 1 }));
//     // if (dragon) {
//     //     grid.PlaceCharacter(dragon, Math::vec2{ 1, 1 });
//     //     AddGameObject(dragon);
//     // }

//     // fighter = static_cast<Fighter*>(Character::CreateFromData("fighter", Math::vec2{ 6, 6 }));
//     // if (fighter) {
//     //     grid.PlaceCharacter(fighter, Math::vec2{ 6, 6 });
//     //     AddGameObject(fighter);
//     // }

//     currentPhase = BattlePhase::PlayerTurn;
//     turnCount = 1;

//     Engine::Instance().GetEventBus().Subscribe<CharacterDeathEvent>([this](const CharacterDeathEvent& event) {
//         Engine::GetLogger().LogEvent("BattleState: " + event.character->TypeName() + " has died");
//         CheckBattleEnd();
//         });

//     Engine::GetLogger().LogEvent("BattleState: Battle started - Dragon vs Fighter");
// }

// void BattleState::HandleInput() {
//     if (currentPhase == BattlePhase::PlayerTurn) {
//         HandlePlayerTurnInput();
//     }
//     else if (currentPhase == BattlePhase::EnemyTurn) {
//         HandleEnemyTurnInput();
//     }
// }

// void BattleState::HandlePlayerTurnInput() {
//     CS230::Input& input = Engine::GetInput();

//     if (input.KeyJustPressed(CS230::Input::Keys::Up)) {
//         cursorPosition.y = std::max(0.0, cursorPosition.y - 1);
//     }
//     if (input.KeyJustPressed(CS230::Input::Keys::Down)) {
//         cursorPosition.y = std::min(7.0, cursorPosition.y + 1);
//     }
//     if (input.KeyJustPressed(CS230::Input::Keys::Left)) {
//         cursorPosition.x = std::max(0.0, cursorPosition.x - 1);
//     }
//     if (input.KeyJustPressed(CS230::Input::Keys::Right)) {
//         cursorPosition.x = std::min(7.0, cursorPosition.x + 1);
//     }

//     if (input.KeyJustPressed(CS230::Input::Keys::Q)) {
//         // SpellSystem& spellSys = SpellSystem::GetInstance();
//         // SpellResult result = spellSys.CastSpell(dragon, "Fireball", cursorPosition);

//         //if (result.success) {
//             Engine::GetLogger().LogEvent("BattleState: Dragon cast Fireball");
//         //}
//     }
//     if (input.KeyJustPressed(CS230::Input::Keys::W)) {
//         // SpellSystem& spellSys = SpellSystem::GetInstance();
//         // SpellResult result = spellSys.CastSpell(dragon, "CreateWall", cursorPosition);

//         //if (result.success) {
//             Engine::GetLogger().LogEvent("BattleState: Dragon cast CreateWall");
//         //}
//     }
//     if (input.KeyJustPressed(CS230::Input::Keys::E)) {
//         // SpellSystem& spellSys = SpellSystem::GetInstance();
//         // SpellResult result = spellSys.CastSpell(dragon, "LavaPool", cursorPosition);

//         //if (result.success) {
//             Engine::GetLogger().LogEvent("BattleState: Dragon cast LavaPool");
//         //}
//     }

//     if (input.KeyJustPressed(CS230::Input::Keys::Space)) {
//         EndCurrentTurn();
//     }
// }

// void BattleState::HandleEnemyTurnInput() {
//     CS230::Input& input = Engine::GetInput();

//     if (input.KeyJustPressed(CS230::Input::Keys::Up)) {
//         cursorPosition.y = std::max(0.0, cursorPosition.y - 1);
//     }

//     if (input.KeyJustPressed(CS230::Input::Keys::A)) {
//         if (dragon) {
//             // MeleeAttack attack;
//             // AbilityResult result = attack.Use(fighter, dragon);

//             //if (result.success) {
//                 Engine::GetLogger().LogEvent("BattleState: Fighter used Melee Attack");
//             //}
//         }
//     }
//     if (input.KeyJustPressed(CS230::Input::Keys::S)) {
//         if (dragon) {
//             // ShieldBash bash;
//             // AbilityResult result = bash.Use(fighter, dragon);

//             //if (result.success) {
//                 Engine::GetLogger().LogEvent("BattleState: Fighter used Shield Bash");
//             //}
//         }
//     }

//     if (input.KeyJustPressed(CS230::Input::Keys::Space)) {
//         EndCurrentTurn();
//     }
// }


// void BattleState::EndCurrentTurn() {
//     if (currentPhase == BattlePhase::PlayerTurn) {
//         // dragon->RefreshActionPoints();

//         currentPhase = BattlePhase::EnemyTurn;
//         Engine::GetLogger().LogEvent("BattleState: Fighter's turn");
//     }
//     else if (currentPhase == BattlePhase::EnemyTurn) {
//         // fighter->RefreshActionPoints();

//         currentPhase = BattlePhase::PlayerTurn;
//         turnCount++;
//         Engine::GetLogger().LogEvent("BattleState: Dragon's turn (Turn " + std::to_string(turnCount) + ")");
//     }

//     CheckBattleEnd();
// }

// void BattleState::CheckBattleEnd() {
//     // if (dragon && dragon->GetCurrentHP() <= 0) {
//     //     currentPhase = BattlePhase::BattleEnd;
//     //     Engine::GetLogger().LogEvent("BattleState: BATTLE END - Fighter wins!");
//     // }
//     // else if (fighter && fighter->GetCurrentHP() <= 0) {
//     //     currentPhase = BattlePhase::BattleEnd;
//     //     Engine::GetLogger().LogEvent("BattleState: BATTLE END - Dragon wins!");
//     // }
// }

// Character* BattleState::GetCurrentTurnCharacter() {
//     if (currentPhase == BattlePhase::PlayerTurn) {
//         return dragon;
//     }
//     else if (currentPhase == BattlePhase::EnemyTurn) {
//         return fighter;
//     }
//     return nullptr;
// }

// void BattleState::Update(double dt) {
//     HandleInput();

//     GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);

//     CheckBattleEnd();
// }

// void BattleState::Draw() {
//     RenderGrid();

//     GetGSComponent<CS230::GameObjectManager>()->DrawAll(Math::TransformationMatrix());

//     RenderUI();
// }

// void BattleState::RenderGrid() {
//     // GridSystem& grid = GridSystem::GetInstance();

//     // TODO Use GridDebugRenderer for better visualization

//     for (int x = 0; x < 8; ++x) {
//         for (int y = 0; y < 8; ++y) {
//             Math::vec2 tilePos{ (double)x, (double)y };

//             // TileType type = grid.GetTileType(tilePos);
//         }
//     }

//     // Draw cursor
//     // ... (simple highlight at cursorPosition)
// }

// void BattleState::RenderUI() {
//     // CS230::Font& font = Engine::GetFont(Fonts::Default);

//     // if (dragon) {
//     //     std::string dragonInfo = "Dragon HP: " + std::to_string(dragon->GetCurrentHP()) + "/" + std::to_string(dragon->GetMaxHP());
//     //     dragonInfo += " | AP: " + std::to_string(dragon->GetCurrentActionPoints()) + "/" + std::to_string(dragon->GetMaxActionPoints());
//     //     font.Draw(dragonInfo, Math::vec2{ 10, 550 });
//     // }

//     // if (fighter) {
//     //     std::string fighterInfo = "Fighter HP: " + std::to_string(fighter->GetCurrentHP()) + "/" + std::to_string(fighter->GetMaxHP());
//     //     fighterInfo += " | AP: " + std::to_string(fighter->GetCurrentActionPoints()) + "/" + std::to_string(fighter->GetMaxActionPoints());
//     //     font.Draw(fighterInfo, Math::vec2{ 10, 530 });
//     // }

//     // std::string turnInfo = "Turn " + std::to_string(turnCount) + " - " +
//     //     (currentPhase == BattlePhase::PlayerTurn ? "Dragon's Turn" : "Fighter's Turn");
//     // font.Draw(turnInfo, Math::vec2{ 10, 10 });

//     // if (currentPhase == BattlePhase::BattleEnd) {
//     //     std::string winMessage = (dragon && dragon->GetCurrentHP() > 0) ? "DRAGON WINS!" : "FIGHTER WINS!";
//     //     font.Draw(winMessage, Math::vec2{ 300, 300 });
//     // }
// }

// void BattleState::Unload() {
//     Engine::GetLogger().LogEvent("BattleState: Unloading");

//     GetGSComponent<CS230::GameObjectManager>()->Unload();

//     dragon = nullptr;
//     fighter = nullptr;
// }

// void BattleState::DrawImGui()
// {
// }

// gsl::czstring BattleState::GetName() const
// {
//     return "BattleState";
// }
