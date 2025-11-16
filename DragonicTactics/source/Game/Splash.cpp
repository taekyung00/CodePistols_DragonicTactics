/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/
#include "../Engine/Engine.hpp"
#include "../Engine/TextureManager.hpp"
#include "../Engine/Logger.hpp"
#include "../Engine/GameStateManager.hpp"
#include "../Engine/Window.hpp"
#include "../CS200/RenderingAPI.hpp"
#include "../CS200/IRenderer2D.hpp"
#include "../CS200/NDC.hpp"
#include "MainMenu.h"
#include "Splash.h"


void Splash::Load() {
    counter = 0.0;
    texture = Engine::GetTextureManager().Load("Assets/images/Splash/DigiPen.png");
}

void Splash::Update([[maybe_unused]] double dt) {
    Engine::GetLogger().LogDebug(std::to_string(counter));
    if (counter >= 0.3) {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }
    counter+=dt;
}

void Splash::Unload()
{
}


void Splash::Draw()  {

    CS200::RenderingAPI::Clear();
    auto& renderer_2d          = Engine::GetRenderer2D();
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
    texture->Draw(Math::TranslationMatrix({ (Engine::GetWindow().GetSize() - texture->GetSize()) / 2 }));

    renderer_2d.EndScene();
}

void Splash::DrawImGui()
{
}

gsl::czstring Splash::GetName() const
{
    return "Splash";
}
