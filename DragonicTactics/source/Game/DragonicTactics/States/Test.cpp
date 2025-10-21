#include "Test.h"
#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Window.hpp"

#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include "./Game/DragonicTactics/Types/Events.h"

#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/MainMenu.h"

#include "../Singletons/DiceManager.h"
#include "../Singletons/EventBus.h"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/System/CombatSystem.h"

Test::Test() : fighter(nullptr), dragon(nullptr)
{
}

void Test::Load()
{
    AddGSComponent(new CS230::GameObjectManager());
    fighter = new Fighter({ 5, 5 });
    GetGSComponent<CS230::GameObjectManager>()->Add(fighter);
    dragon = new Dragon({ 6, 5 });
    GetGSComponent<CS230::GameObjectManager>()->Add(dragon);
    Engine::GetLogger().LogEvent("========== Combat Testbed Initialized ==========");
    Engine::GetLogger().LogEvent("'T' -> Fighter's Turn | 'Y' -> Dragon's Turn | 'D' -> Damage Dragon | 'H' -> Heal Fighter");
    LogFighterStatus();
    LogDragonStatus();
}

void Test::Update([[maybe_unused]] double dt)
{
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::F))
    {
        test_subscribe_publish_singleSubscriber();
        test_multiple_subscribers_sameEvent();
        test_multiple_different_events();
        test_EventData_CompleteTransfer();
        test_EventData_MultiplePublishes();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::E))
    {
        DiceManager& dice = DiceManager::Instance();
        dice.SetSeed(42);
        dice.RollDiceFromString("4d8+2");
    }


    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::T))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'T': Starting Fighter's Turn ---");
        fighter->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Y))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'Y': Starting Dragon's Turn ---");
        dragon->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::D))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'D': Testing PerformAttack ---");
        if (fighter->GetActionPoints() > 0)
        {
            fighter->PerformAttack(dragon);
        }
        else
        {
            Engine::GetLogger().LogDebug("Fighter has no Action Points to attack!");
        }
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::H))
    {
        Engine::GetLogger().LogEvent("--- Player presses 'H': Applying 10 Heal to Fighter ---");
        fighter->ReceiveHeal(10);
        LogFighterStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::S))
    { // junyoung
        GridSystem grid;

        // Should NOT crash, should log error instead
        grid.SetTileType(Math::vec2{ -1, 0 }, GridSystem::TileType::Wall);    // Invalid
        grid.SetTileType(Math::vec2{ 8, 8 }, GridSystem::TileType::Wall);     // Invalid
        grid.SetTileType(Math::vec2{ 100, 100 }, GridSystem::TileType::Wall); // Invalid

        // Verify valid tiles unaffected
        ASSERT_EQ(grid.GetTileType(Math::vec2{ 0, 0 }), GridSystem::TileType::Empty);
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}

void Test::Draw()
{
    Engine::GetWindow().Clear(0x1a1a1aff);
    auto& renderer_2d = Engine::GetRenderer2D();
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    renderer_2d.DrawCircle(Math::TranslationMatrix(Math::ivec2{ 100, 100 }) * Math::ScaleMatrix(30.0));

    renderer_2d.EndScene();
}

void Test::DrawImGui()
{
}

gsl::czstring Test::GetName() const
{
    return "Test";
}

void Test::LogFighterStatus()
{
    if (fighter == nullptr)
        return;

    GridPosition*   grid_pos = fighter->GetGridPosition();
    StatsComponent* stats    = fighter->GetStatsComponent();
    ActionPoints*   ap       = fighter->GetActionPointsComponent();

    Engine::GetLogger().LogDebug("========== Fighter Status Report ==========");

    if (grid_pos != nullptr)
    {
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    }
    if (stats != nullptr)
    {
        std::string hp_status = "HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP());
        hp_status += " (" + std::to_string(stats->GetHealthPercentage() * 100.0f) + "%)";
        Engine::GetLogger().LogDebug(hp_status);
        Engine::GetLogger().LogDebug("IsAlive: " + std::string(stats->IsAlive() ? "true" : "false"));
        Engine::GetLogger().LogDebug("Speed: " + std::to_string(stats->GetSpeed()));
    }
    if (ap != nullptr)
    {
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    }

    Engine::GetLogger().LogDebug("==========================================");
}

void Test::test_multiple_subscribers_sameEvent()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    int callback1Count = 0;
    int callback2Count = 0;
    int callback3Count = 0;

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { callback1Count++; });
    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { callback2Count++; });
    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { callback3Count++; });

    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });

    ASSERT_EQ(callback1Count, 1);
    ASSERT_EQ(callback2Count, 1);
    ASSERT_EQ(callback3Count, 1);
}

void Test::test_multiple_different_events()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool damageCalled = false;
    bool deathCalled  = false;
    bool spellCalled  = false;

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { damageCalled = true; });
    eventbus.Subscribe<CharacterDeathEvent>([&]([[maybe_unused]] const CharacterDeathEvent&) { deathCalled = true; });
    eventbus.Subscribe<SpellCastEvent>([&]([[maybe_unused]] const SpellCastEvent&) { spellCalled = true; });

    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDeathEvent{ reinterpret_cast<Character*>(&character), nullptr });
    eventbus.Publish(
        SpellCastEvent{
            reinterpret_cast<Character*>(&character), "Fireball", 1, { 0, 0 },
               1
    });

    ASSERT_TRUE(damageCalled);
    ASSERT_TRUE(deathCalled);
    ASSERT_TRUE(spellCalled);
}

void Test::test_EventData_CompleteTransfer()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    MockCharacter         victim("Victim"), attacker("Attacker");
    CharacterDamagedEvent receivedEvent;

    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) { receivedEvent = e; });

    CharacterDamagedEvent originalEvent{ reinterpret_cast<Character*>(&victim), 42, 58, reinterpret_cast<Character*>(&attacker), true };
    eventbus.Publish(originalEvent);

    ASSERT_EQ(reinterpret_cast<Character*>(receivedEvent.target), reinterpret_cast<Character*>(&victim));
    ASSERT_EQ(receivedEvent.damageAmount, 42);
    ASSERT_EQ(receivedEvent.remainingHP, 58);
    ASSERT_EQ(reinterpret_cast<Character*>(receivedEvent.attacker), reinterpret_cast<Character*>(&attacker));
    ASSERT_TRUE(receivedEvent.wasCritical);
}

void Test::test_subscribe_publish_singleSubscriber()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool           callbackInvoked = false;
    // int receivedDamage = 0;
    MockCharacter* receivedTarget  = nullptr;

    MockCharacter         character("TestDragon");
    const int             damage = 30;
    CharacterDamagedEvent event{ reinterpret_cast<Character*>(&character), damage, 70, nullptr, false };
    eventbus.Subscribe<CharacterDamagedEvent>(
        [&](const CharacterDamagedEvent& e)
        {
            callbackInvoked = true;
            character.SetHP(character.GetCurrentHP() - e.damageAmount);
            receivedTarget = reinterpret_cast<MockCharacter*>(e.target);
        });

    eventbus.Publish(event);

    ASSERT_TRUE(callbackInvoked);
    ASSERT_EQ(character.GetCurrentHP(), event.remainingHP);
    ASSERT_EQ(receivedTarget, &character);
}

void Test::test_EventData_MultiplePublishes()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    std::vector<int> damages;
    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) { damages.push_back(e.damageAmount); });

    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 20, 70, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 30, 40, nullptr, true });

    ASSERT_EQ(static_cast<int>(damages.size()), 3);
    ASSERT_EQ(damages[0], 10);
    ASSERT_EQ(damages[1], 20);
    ASSERT_EQ(damages[2], 30);
}

void Test::LogDragonStatus()
{
    if (dragon == nullptr)
        return;
    Engine::GetLogger().LogDebug("========== Dragon Status Report ==========");

    GridPosition*   grid_pos = dragon->GetGridPosition();
    StatsComponent* stats    = dragon->GetStatsComponent();
    ActionPoints*   ap       = dragon->GetActionPointsComponent();
    SpellSlots*     ss       = dragon->GetSpellSlots();

    if (grid_pos != nullptr)
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    if (stats != nullptr)
        Engine::GetLogger().LogDebug("HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP()));
    if (ap != nullptr)
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    if (ss != nullptr)
        Engine::GetLogger().LogDebug("Has Lvl 5 Slot: " + std::string(ss->HasSlot(5) ? "Yes" : "No"));

    Engine::GetLogger().LogDebug("==========================================");
}

void Test::Unload()
{
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    fighter = nullptr;
    dragon  = nullptr;
}

//-----------------------------------------------------------------------------------

Test2::Test2() : fighter(nullptr), dragon(nullptr)
{
}

void Test2::Load() {
    AddGSComponent(new CS230::GameObjectManager());

    AddGSComponent(new GridSystem());
    CS230::GameObjectManager* go_manager = GetGSComponent<CS230::GameObjectManager>();

    GridSystem* grid_system = GetGSComponent<GridSystem>();

    const std::vector<std::string> map_data = {
    "wwwwwwww",
    "weefeeew",
    "weeeeeew",
    "weeeeeew",
    "weeeeeew",
    "weeeeeew",
    "weedeeew",
    "wwwwwwww"
    };



    for (int y = 0; y < map_data.size(); ++y) {
        for (int x = 0; x < map_data[y].length(); ++x) {
            char tile_char = map_data[y][x];

            Math::ivec2 current_pos = { x, static_cast<int>(map_data.size()) - 1 - y };

            switch (tile_char) {
            case 'w':
                grid_system->SetTileType(current_pos, GridSystem::TileType::Wall);
                break;
            case 'e':
                grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
                break;
            case 'f':
                grid_system->SetTileType(current_pos, GridSystem::TileType::Empty); 
                fighter = new Fighter(current_pos);
                go_manager->Add(fighter);
                grid_system->AddCharacter(fighter, current_pos);
                break;
            case 'd':
                grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
                dragon = new Dragon(current_pos);
                go_manager->Add(dragon);
                grid_system->AddCharacter(dragon, current_pos);
                break;
            }
        }
    }

}

void Test2::Update([[maybe_unused]] double dt) {


    if (dragon != nullptr && dragon->IsAlive()) {

        Math::ivec2 current_pos = dragon->GetGridPosition()->Get();
        Math::ivec2 target_pos = current_pos;
        bool move_requested = false;

        if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
            target_pos.y++;
            move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Down)) {
            target_pos.y--;
            move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Left)) {
            target_pos.x--;
            move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Right)) {
            target_pos.x++;
            move_requested = true;
        }

        if (move_requested) {
            GridSystem* grid = GetGSComponent<GridSystem>();
            if (grid != nullptr && grid->IsWalkable(target_pos)) {

                grid->MoveCharacter(current_pos, target_pos);

                dragon->GetGridPosition()->Set(target_pos);

                dragon->SetPosition({
                    static_cast<double>(target_pos.x * GridSystem::TILE_SIZE),
                    static_cast<double>(target_pos.y * GridSystem::TILE_SIZE)
                    });

                Engine::GetLogger().LogEvent("Dragon moved to (" + std::to_string(target_pos.x) + ", " + std::to_string(target_pos.y) + ")");
                LogDragonStatus();
            }
            else {
                Engine::GetLogger().LogEvent("Dragon cannot move there! (Wall or Occupied)");
            }
        }
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}

void Test2::Draw() {
    Engine::GetWindow().Clear(0x1a1a1aff);
    auto& renderer_2d = Engine::GetRenderer2D();
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    if (grid_system != nullptr) {
        grid_system->Draw();
    }
    renderer_2d.EndScene();
}

void Test2::LogFighterStatus() {
    if (fighter == nullptr) return;

    GridPosition* grid_pos = fighter->GetGridPosition();
    StatsComponent* stats = fighter->GetStatsComponent();
    ActionPoints* ap = fighter->GetActionPointsComponent();

    Engine::GetLogger().LogDebug("========== Fighter Status Report ==========");

    if (grid_pos != nullptr) {
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    }
    if (stats != nullptr) {
        std::string hp_status = "HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP());
        hp_status += " (" + std::to_string(stats->GetHealthPercentage() * 100.0f) + "%)";
        Engine::GetLogger().LogDebug(hp_status);
        Engine::GetLogger().LogDebug("IsAlive: " + std::string(stats->IsAlive() ? "true" : "false"));
        Engine::GetLogger().LogDebug("Speed: " + std::to_string(stats->GetSpeed()));
    }
    if (ap != nullptr) {
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    }

    Engine::GetLogger().LogDebug("==========================================");
}

void Test2::Test2_multiple_subscribers_sameEvent() {
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    int callback1Count = 0;
    int callback2Count = 0;
    int callback3Count = 0;

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { callback1Count++; });
    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { callback2Count++; });
    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { callback3Count++; });

    MockCharacter character("Test2Char");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });

    ASSERT_EQ(callback1Count, 1);
    ASSERT_EQ(callback2Count, 1);
    ASSERT_EQ(callback3Count, 1);
}

void Test2::Test2_multiple_different_events() {
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool damageCalled = false;
    bool deathCalled = false;
    bool spellCalled = false;

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { damageCalled = true; });
    eventbus.Subscribe<CharacterDeathEvent>([&]([[maybe_unused]] const CharacterDeathEvent&) { deathCalled = true; });
    eventbus.Subscribe<SpellCastEvent>([&]([[maybe_unused]] const SpellCastEvent&) { spellCalled = true; });

    MockCharacter character("Test2Char");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDeathEvent{ reinterpret_cast<Character*>(&character), nullptr });
    eventbus.Publish(SpellCastEvent{ reinterpret_cast<Character*>(&character), "Fireball", 1, {0,0}, 1 });

    ASSERT_TRUE(damageCalled);
    ASSERT_TRUE(deathCalled);
    ASSERT_TRUE(spellCalled);
}

void Test2::Test2_EventData_CompleteTransfer() {
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    MockCharacter victim("Victim"), attacker("Attacker");
    CharacterDamagedEvent receivedEvent;

    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
        receivedEvent = e;
        });

    CharacterDamagedEvent originalEvent{ reinterpret_cast<Character*>(&victim), 42, 58, reinterpret_cast<Character*>(&attacker), true };
    eventbus.Publish(originalEvent);

    ASSERT_EQ(reinterpret_cast<Character*>(receivedEvent.target), reinterpret_cast<Character*>(&victim));
    ASSERT_EQ(receivedEvent.damageAmount, 42);
    ASSERT_EQ(receivedEvent.remainingHP, 58);
    ASSERT_EQ(reinterpret_cast<Character*>(receivedEvent.attacker), reinterpret_cast<Character*>(&attacker));
    ASSERT_TRUE(receivedEvent.wasCritical);
}
void Test2::Test2_subscribe_publish_singleSubscriber()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool callbackInvoked = false;
    MockCharacter* receivedTarget = nullptr;

    MockCharacter character("Test2Dragon");
    const int damage = 30;
    CharacterDamagedEvent event{ reinterpret_cast<Character*>(&character),damage,70,nullptr,false };
    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
        callbackInvoked = true;
        character.SetHP(character.GetCurrentHP() - e.damageAmount);
        receivedTarget = reinterpret_cast<MockCharacter*>(e.target);
        });

    eventbus.Publish(event);

    ASSERT_TRUE(callbackInvoked);
    ASSERT_EQ(character.GetCurrentHP(), event.remainingHP);
    ASSERT_EQ(receivedTarget, &character);

}

void Test2::Test2_EventData_MultiplePublishes() {
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    std::vector<int> damages;
    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
        damages.push_back(e.damageAmount);
        });

    MockCharacter character("Test2Char");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 20, 70, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 30, 40, nullptr, true });

    ASSERT_EQ(static_cast<int>(damages.size()), 3);
    ASSERT_EQ(damages[0], 10);
    ASSERT_EQ(damages[1], 20);
    ASSERT_EQ(damages[2], 30);
}

void Test2::LogDragonStatus() {
    if (dragon == nullptr) return;
    Engine::GetLogger().LogDebug("========== Dragon Status Report ==========");

    GridPosition* grid_pos = dragon->GetGridPosition();
    StatsComponent* stats = dragon->GetStatsComponent();
    ActionPoints* ap = dragon->GetActionPointsComponent();
    SpellSlots* ss = dragon->GetSpellSlots();

    if (grid_pos != nullptr)
        Engine::GetLogger().LogDebug("Position: (" + std::to_string(grid_pos->Get().x) + ", " + std::to_string(grid_pos->Get().y) + ")");
    if (stats != nullptr)
        Engine::GetLogger().LogDebug("HP: " + std::to_string(stats->GetCurrentHP()) + " / " + std::to_string(stats->GetMaxHP()));
    if (ap != nullptr)
        Engine::GetLogger().LogDebug("Action Points: " + std::to_string(ap->GetCurrentPoints()) + " / " + std::to_string(ap->GetMaxPoints()));
    if (ss != nullptr)
        Engine::GetLogger().LogDebug("Has Lvl 5 Slot: " + std::string(ss->HasSlot(5) ? "Yes" : "No"));

    Engine::GetLogger().LogDebug("==========================================");
}




bool Test2::Test_CombatSystem_CalculateDamage() {
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    int damage = CombatSystem::Instance().CalculateDamage(&dragon, &fighter, "3d8", 5);

    ASSERT_GE(damage, 8);
    ASSERT_LE(damage, 29);

    std::cout << "Test_CombatSystem_CalculateDamage passed (damage=" << damage << ")" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_CalculateDamage_MinRoll() {
    // Setup
    DiceManager::Instance().SetSeed(1000);
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    int minDamage = 999;
    for (int i = 0; i < 100; ++i) {
        int damage = CombatSystem::Instance().CalculateDamage(&dragon, &fighter, "3d8", 5);
        if (damage < minDamage) minDamage = damage;
    }

    ASSERT_GE(minDamage, 8);
    ASSERT_LE(minDamage, 10);

    std::cout << "Test_CombatSystem_CalculateDamage_MinRoll passed (min=" << minDamage << ")" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_CalculateDamage_MaxRoll() {
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    int maxDamage = 0;
    for (int i = 0; i < 100; ++i) {
        int damage = CombatSystem::Instance().CalculateDamage(&dragon, &fighter, "3d8", 5);
        if (damage > maxDamage) maxDamage = damage;
    }

    ASSERT_GE(maxDamage, 25);
    ASSERT_LE(maxDamage, 29);

    std::cout << "Test_CombatSystem_CalculateDamage_MaxRoll passed (max=" << maxDamage << ")" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_ApplyDamage() {
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});
    int fighterHPBefore = fighter.GetStatsComponent()->GetCurrentHP();

    CombatSystem::Instance().ApplyDamage(&dragon, &fighter, 20);

    ASSERT_EQ(fighter.GetStatsComponent()->GetCurrentHP(), fighterHPBefore - 20);
    ASSERT_TRUE(fighter.IsAlive());

    std::cout << "Test_CombatSystem_ApplyDamage passed" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_ApplyDamage_Negative() {
    Fighter fighter({0, 0});
    int hpBefore = fighter.GetStatsComponent()->GetCurrentHP();

    CombatSystem::Instance().ApplyDamage(nullptr, &fighter, -10);

    ASSERT_EQ(fighter.GetStatsComponent()->GetCurrentHP(), hpBefore);

    std::cout << "Test_CombatSystem_ApplyDamage_Negative passed" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_ExecuteAttack_Valid() {
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});
    Engine::GetGameStateManager().GetGSComponent<GridSystem>()->MoveCharacter(dragon.GetGridPosition()->Get(), {0, 0});
    Engine::GetGameStateManager().GetGSComponent<GridSystem>()->MoveCharacter(fighter.GetGridPosition()->Get(), {0, 1});

    int fighterHPBefore = fighter.GetStatsComponent()->GetCurrentHP();
    int dragonAPBefore = dragon.GetActionPoints();

    bool success = CombatSystem::Instance().ExecuteAttack(&dragon, &fighter);

    ASSERT_TRUE(success);
    ASSERT_TRUE(fighter.GetStatsComponent()->GetCurrentHP() < fighterHPBefore);
    ASSERT_EQ(dragon.GetActionPoints(), dragonAPBefore - 2);

    std::cout << "Test_CombatSystem_ExecuteAttack_Valid passed" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_ExecuteAttack_OutOfRange() {
    Dragon dragon({0, 0});
    Fighter fighter({5, 5});
    Engine::GetGameStateManager().GetGSComponent<GridSystem>()->MoveCharacter(dragon.GetGridPosition()->Get(), {0, 0});
    Engine::GetGameStateManager().GetGSComponent<GridSystem>()->MoveCharacter(fighter.GetGridPosition()->Get(), {5, 5});

    int fighterHPBefore = fighter.GetStatsComponent()->GetCurrentHP();

    bool success = CombatSystem::Instance().ExecuteAttack(&dragon, &fighter);

    ASSERT_FALSE(success);
    ASSERT_EQ(fighter.GetStatsComponent()->GetCurrentHP(), fighterHPBefore);

    std::cout << "Test_CombatSystem_ExecuteAttack_OutOfRange passed" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_ExecuteAttack_NotEnoughAP() {
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});
    Engine::GetGameStateManager().GetGSComponent<GridSystem>()->MoveCharacter(dragon.GetGridPosition()->Get(), {0, 0});
    Engine::GetGameStateManager().GetGSComponent<GridSystem>()->MoveCharacter(fighter.GetGridPosition()->Get(), {0, 1});

    dragon.GetActionPointsComponent()->Consume(4);

    int fighterHPBefore = fighter.GetStatsComponent()->GetCurrentHP();

    bool success = CombatSystem::Instance().ExecuteAttack(&dragon, &fighter);

    ASSERT_FALSE(success);
    ASSERT_EQ(fighter.GetStatsComponent()->GetCurrentHP(), fighterHPBefore);

    std::cout << "Test_CombatSystem_ExecuteAttack_NotEnoughAP passed" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_IsInRange_Adjacent() {
    Dragon dragon({0, 0});
    Fighter fighter({0, 1});

    bool inRange = CombatSystem::Instance().IsInRange(&dragon, &fighter, 1);

    ASSERT_TRUE(inRange);

    std::cout << "Test_CombatSystem_IsInRange_Adjacent passed" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_IsInRange_TooFar() {
    Dragon dragon({0, 0});
    Fighter fighter({5, 5});

    bool inRange = CombatSystem::Instance().IsInRange(&dragon, &fighter, 1);

    ASSERT_FALSE(inRange);

    std::cout << "Test_CombatSystem_IsInRange_TooFar passed" << std::endl;
    return true;
}

bool Test2::Test_CombatSystem_GetDistance() {
    Dragon dragon({0, 0});
    Fighter fighter({3, 4});

    int distance = CombatSystem::Instance().GetDistance(&dragon, &fighter);

    ASSERT_EQ(distance, 7);

    std::cout << "Test_CombatSystem_GetDistance passed" << std::endl;
    return true;
}



void Test2::Unload() {
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    fighter = nullptr;
    dragon = nullptr;
}

void Test2::DrawImGui()
{
}

gsl::czstring Test2::GetName() const
{
    return "Test2";
}