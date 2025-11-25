#include "TestEventBus.h"

#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"

#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/Test/Week1TestMocks.h"
#include "Game/DragonicTactics/Test/TestAssert.h"
#include "Game/DragonicTactics/Types/Events.h"
void test_multiple_different_events()
{
    EventBus eventbus;
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

void test_EventData_CompleteTransfer()
{
    EventBus eventbus;
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

void test_subscribe_publish_singleSubscriber()
{
    EventBus eventbus;
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

void test_multiple_subscribers_sameEvent()
{
    EventBus eventbus;
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

void test_EventData_MultiplePublishes()
{
    EventBus eventbus;
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
