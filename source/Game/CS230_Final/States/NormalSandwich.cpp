#include "NormalSandwich.h"
#include "Project.h"
#include "../../MainMenu.h"
#include "../../../Engine/TextureManager.hpp"
#include "../../../Engine/GameStateManager.hpp"
#include "../../../Engine/Input.hpp"
#include "../../../Engine/Window.hpp"
void NormalSandwich::Load() {
    texture = Engine::GetTextureManager().Load("Assets/images/CS230_Final/NormalSandwich.png");
}

void NormalSandwich::Update([[maybe_unused]] double dt) {
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R)) {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<Project>();
    }
}

void NormalSandwich::Unload() {

}

void NormalSandwich::Draw() {
    Engine::GetWindow().Clear(UINT_MAX);
    texture->Draw(Math::TransformationMatrix());
}