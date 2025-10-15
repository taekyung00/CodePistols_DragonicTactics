#include "../Engine/Engine.h"
#include "../Game/Test/Week1TestMocks.h"
#include "../Game/Types/Events.h"
#include "Test.h"
#include "States.h"
#include "Dragon.h"
#include "Fighter.h"
#include "../Engine/GameObjectManager.h"
#include "GridPosition.h"
#include "StatsComponent.h"
#include "ActionPoints.h"
#include "SpellSlots.h"

Test::Test() : fighter(nullptr), dragon(nullptr)
{
}

void Test::Load() {
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

void Test::Update([[maybe_unused]] double dt) {
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::E)) {
        test_subscribe_publish_singleSubscriber();
        test_multiple_subscribers_sameEvent();
        test_multiple_different_events();
        test_EventData_CompleteTransfer();
        test_EventData_MultiplePublishes();
    }
    

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::T)) {
        Engine::GetLogger().LogEvent("--- Player presses 'T': Starting Fighter's Turn ---");
        fighter->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Y)) {
        Engine::GetLogger().LogEvent("--- Player presses 'Y': Starting Dragon's Turn ---");
        dragon->OnTurnStart();
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::D)) {
        Engine::GetLogger().LogEvent("--- Player presses 'D': Testing PerformAttack ---");
        if (fighter->GetActionPoints() > 0) {
            fighter->PerformAttack(dragon);
        }
        else {
            Engine::GetLogger().LogDebug("Fighter has no Action Points to attack!");
        }
        LogFighterStatus();
        LogDragonStatus();
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::H)) {
        Engine::GetLogger().LogEvent("--- Player presses 'H': Applying 10 Heal to Fighter ---");
        fighter->ReceiveHeal(10);
        LogFighterStatus();
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}

void Test::Draw() {
    Engine::GetWindow().Clear(0x1a1a1aff);
}

void Test::LogFighterStatus() {
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

void Test::test_multiple_subscribers_sameEvent() {
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

void Test::test_multiple_different_events() {
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool damageCalled = false;
    bool deathCalled = false;
    bool spellCalled = false;

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent&) { damageCalled = true; });
    eventbus.Subscribe<CharacterDeathEvent>([&]([[maybe_unused]] const CharacterDeathEvent&) { deathCalled = true; });
    eventbus.Subscribe<SpellCastEvent>([&]([[maybe_unused]] const SpellCastEvent&) { spellCalled = true; });

    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDeathEvent{ reinterpret_cast<Character*>(&character), nullptr });
    eventbus.Publish(SpellCastEvent{ reinterpret_cast<Character*>(&character), "Fireball", 1, {0,0}, 1 });

    ASSERT_TRUE(damageCalled);
    ASSERT_TRUE(deathCalled);
    ASSERT_TRUE(spellCalled);
}

void Test::test_EventData_CompleteTransfer() {
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
void Test::test_subscribe_publish_singleSubscriber()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool callbackInvoked = false;
    //int receivedDamage = 0;
    MockCharacter* receivedTarget = nullptr;

    MockCharacter character("TestDragon");
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

void Test::test_EventData_MultiplePublishes() {
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    std::vector<int> damages;
    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
        damages.push_back(e.damageAmount);
        });

    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 20, 70, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 30, 40, nullptr, true });

    ASSERT_EQ(static_cast<int>(damages.size()), 3);
    ASSERT_EQ(damages[0], 10);
    ASSERT_EQ(damages[1], 20);
    ASSERT_EQ(damages[2], 30);
}

void Test::LogDragonStatus() {
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

void Test::Unload() {
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    fighter = nullptr;
    dragon = nullptr;
}
