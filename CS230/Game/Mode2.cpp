/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/
#include "../Engine/Engine.h"
#include "../Engine/ShowCollision.h"

#include "States.h"
#include "Mode2.h"
#include "Ship.h"
#include "Meteor.h"

void Mode2::update_score_text()
{
    delete score_texture;
    score_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Score: " + std::to_string(GetGSComponent<Score>()->Value()), 0xFFFFFFFF);
}

Mode2::Mode2()  :
    ship_ptr(nullptr),
    gameover_texture(nullptr),
    reload_texture(nullptr)
{}

void Mode2::Load() {
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new CS230::ParticleManager<Particles::Hit>());
    AddGSComponent(new CS230::ParticleManager<Particles::MeteorBit>());
    AddGSComponent(new CS230::Timer(meteor_timer));
    AddGSComponent(new Score(0));
    temp_score = GetGSComponent<Score>()->Value();
    update_score_text();
#ifdef _DEBUG
    AddGSComponent(new CS230::ShowCollision());
#endif
    ship_ptr = new Ship({ static_cast<double>(Engine::GetWindow().GetSize().x) / 2,
             static_cast<double>(Engine::GetWindow().GetSize().y) / 2 });
    GetGSComponent<CS230::GameObjectManager>()->Add(ship_ptr);
    //GetGSComponent<CS230::GameObjectManager>()->Add(new Meteor());
    //GetGSComponent<CS230::GameObjectManager>()->Add(new Meteor());
    //GetGSComponent<CS230::GameObjectManager>()->Add(new Meteor());
    //GetGSComponent<CS230::GameObjectManager>()->Add(new Meteor());
    //GetGSComponent<CS230::GameObjectManager>()->Add(new Meteor());
    gameover_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Game Over!");
    reload_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Press R Top Restart");
}

void Mode2::Update([[maybe_unused]] double dt) {
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);  
    //Engine::GetLogger().LogDebug(std::to_string(GetGSComponent<CS230::Timer>()->Remaining()));

    if (temp_score != GetGSComponent<Score>()->Value()) {
        update_score_text();
    }
    if (ship_ptr->GetExploded() == true) {
        if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R)) {
            Engine::GetGameStateManager().ReloadState();
        }
    }
    else {
        if (GetGSComponent<CS230::Timer>()->Remaining() == 0.0) {
            GetGSComponent<CS230::GameObjectManager>()->Add(new Meteor(ship_ptr->GetPosition()));
            GetGSComponent<CS230::Timer>()->Set(meteor_timer);
        }
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Mode2::Unload() {
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    ClearGSComponents();
    ship_ptr = nullptr;
}

void Mode2::Draw() {
    Engine::GetWindow().Clear(0x000000FF);
    GetGSComponent<CS230::GameObjectManager>()->DrawAll(Math::TransformationMatrix());
    if (ship_ptr->GetExploded() == true) {
        gameover_texture->Draw(Math::TranslationMatrix(Math::ivec2{
        Engine::GetWindow().GetSize().x / 2 - gameover_texture->GetSize().x / 2,
        Engine::GetWindow().GetSize().y - gameover_texture->GetSize().y - 450 }));
        reload_texture->Draw(Math::TranslationMatrix(Math::ivec2{
        Engine::GetWindow().GetSize().x / 2 - reload_texture->GetSize().x / 2,
        Engine::GetWindow().GetSize().y - reload_texture->GetSize().y - 100 }));
    }
    score_texture->Draw(Math::TranslationMatrix(Math::ivec2{ Engine::GetWindow().GetSize().x - 10 - score_texture->GetSize().x, Engine::GetWindow().GetSize().y - score_texture->GetSize().y - 75 }));
}

