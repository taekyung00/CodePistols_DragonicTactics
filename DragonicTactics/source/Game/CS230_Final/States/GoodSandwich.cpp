#include "GoodSandwich.h"
#include "Project.h"
#include "../../MainMenu.h"
#include "../../../Engine/TextureManager.hpp"
#include "../../../Engine/GameStateManager.hpp"
#include "../../../Engine/Input.hpp"
#include "../../../Engine/Window.hpp"
void GoodSandwich::Load() {
    texture = Engine::GetTextureManager().Load("Assets/images/CS230_Final/GoodSandwich.png");
}

void GoodSandwich::Update([[maybe_unused]] double dt) {
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<Project>();
    }
}

void GoodSandwich::Unload() {
    
}

void GoodSandwich::Draw() {
    Engine::GetWindow().Clear(UINT_MAX);
    texture->Draw(Math::TransformationMatrix());
}