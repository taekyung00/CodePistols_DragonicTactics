#include "../Engine/Engine.h"

#include "Test.h"
#include "States.h"

class Claracter;
Test::Test()
{
}
void Test::Load() {

}
void Test::Update([[maybe_unused]] double dt) {
    auto& eventbus = Engine::GetEventBus();
    eventbus.Clear();

    bool callbackInvoked = false;
    int receivedDamage = 0;


    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {

        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Test::Draw() {
    Engine::GetWindow().Clear(0x000000FF);
}

void Test::Unload() {
}