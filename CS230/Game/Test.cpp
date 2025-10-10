#include "../Engine/Engine.h"
#include "../Game/Test/Week1TestMocks.h"
#include "../Game/Types/Events.h"
#include "Test.h"
#include "States.h"

class Claracter;
Test::Test()
{
}

void Test::Load() {

}

void Test::Update([[maybe_unused]] double dt) {
    

    test_subscribe_publish_singleSubscriber();
    test_multiple_subscribers_sameEvent();
    test_multiple_different_events();
    test_EventData_CompleteTransfer();
    test_EventData_MultiplePublishes();

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {

        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Test::Draw() {
    Engine::GetWindow().Clear(0x000000FF);
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

void Test::test_multiple_subscribers_sameEvent()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    // Setup: 3 subscribers to same event
    int callback1Count = 0;
    int callback2Count = 0;
    int callback3Count = 0;

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]]const CharacterDamagedEvent& e) {
        callback1Count++;
        });

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent& e) {
        callback2Count++;
        });

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent& e) {
        callback3Count++;
        });

    // Action: Publish event
    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>( & character), 10, 90, nullptr, false});

    // All 3 callbacks should be invoked
    ASSERT_EQ(callback1Count, 1);
    ASSERT_EQ(callback2Count, 1);
    ASSERT_EQ(callback3Count, 1);
}

void Test::test_multiple_different_events()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool damageCalled = false;
    bool deathCalled = false;
    bool spellCalled = false;

    eventbus.Subscribe<CharacterDamagedEvent>([&]([[maybe_unused]] const CharacterDamagedEvent& e) {
        damageCalled = true;
        });

    eventbus.Subscribe<CharacterDeathEvent>([&]([[maybe_unused]] const CharacterDeathEvent& e) {
        deathCalled = true;
        });

    eventbus.Subscribe<SpellCastEvent>([&]([[maybe_unused]] const SpellCastEvent& e) {
        spellCalled = true;
        });

    // Publish all 3 events
    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDeathEvent{ reinterpret_cast<Character*>(&character), nullptr });
    eventbus.Publish(SpellCastEvent{ reinterpret_cast<Character*>(&character), "Fireball", 1, {0,0}, 1 });

    // All 3 should be called
    ASSERT_TRUE(damageCalled);
    ASSERT_TRUE(deathCalled);
    ASSERT_TRUE(spellCalled);
}

void Test::test_EventData_CompleteTransfer()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    // Setup: Complex event with all fields
    MockCharacter victim("Victim"), attacker("Attacker");
    CharacterDamagedEvent receivedEvent;

    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
        receivedEvent = e;  // Copy event
        });

    // Publish
    CharacterDamagedEvent originalEvent{ reinterpret_cast<Character*>(&victim), 42, 58, reinterpret_cast<Character*>(&attacker), true };
    eventbus.Publish(originalEvent);

    // Verify all fields transferred correctly
    ASSERT_EQ(reinterpret_cast<Character*>(receivedEvent.target), reinterpret_cast<Character*>(&victim));
    ASSERT_EQ(receivedEvent.damageAmount, 42);
    ASSERT_EQ(receivedEvent.remainingHP, 58);
    ASSERT_EQ(reinterpret_cast<Character*>(receivedEvent.attacker), reinterpret_cast<Character*>(&attacker));
    ASSERT_TRUE(receivedEvent.wasCritical);
}

void Test::test_EventData_MultiplePublishes()
{
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    // Setup
    std::vector<int> damages;
    eventbus.Subscribe<CharacterDamagedEvent>([&](const CharacterDamagedEvent& e) {
        damages.push_back(e.damageAmount);
        });

    // Publish multiple events
    MockCharacter character("TestChar");
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 10, 90, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 20, 70, nullptr, false });
    eventbus.Publish(CharacterDamagedEvent{ reinterpret_cast<Character*>(&character), 30, 40, nullptr, true });

    // Verify all received in order
    ASSERT_EQ(static_cast<int>(damages.size()), 3);
    ASSERT_EQ(damages[0], 10);
    ASSERT_EQ(damages[1], 20);
    ASSERT_EQ(damages[2], 30);
}

void Test::Unload() {
}