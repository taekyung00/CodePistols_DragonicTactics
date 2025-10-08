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

void Test::Unload() {
}