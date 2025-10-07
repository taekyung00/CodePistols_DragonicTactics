#include "../Engine/Engine.h"

#include "Test.h"
#include "States.h"
Test::Test()
{
}
void Test::Load() {

}
void Test::Update([[maybe_unused]] double dt) {
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {

        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Test::Draw() {
    Engine::GetWindow().Clear(0x000000FF);
}

void Test::Unload() {
}