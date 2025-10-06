/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/
#include "../Engine/Engine.h"
#include "../Engine/ShowCollision.h"

#include "States.h"
#include "Mode1.h"
#include "Cat.h"
#include "Asteroid.h"
#include "Crates.h"
#include "Robot.h"
#include "Floor.h"
#include "Portal.h"
#include "Score.h"

Mode1::Mode1() : 
cat_ptr(nullptr),
starting_floor_ptr(nullptr),
timer_texture(nullptr),
score_texture(nullptr),
last_timer(0)
{}
void Mode1::update_timer_text(int value) {
    delete timer_texture;
    timer_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Timer: " + std::to_string(value), 0xFFFFFFFF);
}

void Mode1::update_score_text()
{
    delete score_texture;
    score_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Score: " + std::to_string(GetGSComponent<Score>()->Value()), 0xFFFFFFFF);
}

void Mode1::Load() {
    AddGSComponent(new CS230::Camera({ { 0.15 * Engine::GetWindow().GetSize().x, 0 }, { 0.35 * Engine::GetWindow().GetSize().x, 0 } }));
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new Background());
    AddGSComponent(new CS230::Timer(timer_max));
    AddGSComponent(new Gravity(gravity));
    AddGSComponent(new Score(0));
    score = GetGSComponent<Score>()->Value();
    AddGSComponent(new CS230::ParticleManager<Particles::Smoke>());
#ifdef _DEBUG
    AddGSComponent(new CS230::ShowCollision());
#endif

    last_timer = static_cast<int>(timer_max);
    update_timer_text(last_timer);
    update_score_text();

    starting_floor_ptr = new Floor(Math::irect{ { 0, 0 }, { 930, static_cast<int>(floor) } });
    GetGSComponent<CS230::GameObjectManager>()->Add(starting_floor_ptr);
    GetGSComponent<CS230::GameObjectManager>()->Add(new Floor(Math::irect{ { 1014, 0 }, { 2700, static_cast<int>(floor) } }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Floor(Math::irect{ { 2884, 0 }, { 4126, static_cast<int>(floor) } }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Floor(Math::irect{ { 4208, 0 }, { 5760, static_cast<int>(floor) } }));
    
    cat_ptr = new Cat({ 300, floor },starting_floor_ptr);
    
    GetGSComponent<CS230::GameObjectManager>()->Add(cat_ptr);
    GetGSComponent<CS230::GameObjectManager>()->Add(new Asteroid({ 600, floor }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Asteroid({ 1800, floor }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Asteroid({ 2400, floor }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 900, floor }, 2));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 1400, floor }, 1));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 2000, floor }, 5));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 4000, floor }, 3));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 5400, floor }, 1));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 5500, floor }, 3));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 5600, floor }, 5));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 1025, floor }, cat_ptr, 1025, 1350));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 2050, floor }, cat_ptr, 2050, 2325));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 3400, floor }, cat_ptr, 3400, 3800));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 4225, floor }, cat_ptr, 4225, 4800));
    
    GetGSComponent<Background>()->Add("Assets/Planets.png", 0.25);
    GetGSComponent<Background>()->Add("Assets/Ships.png", 0.5);
    GetGSComponent<Background>()->Add("Assets/Foreground.png", 1);
    GetGSComponent<CS230::GameObjectManager>()->Add(new Portal(static_cast<int>(States::MainMenu), Math::irect{ {GetGSComponent<Background>()->GetSize() - Math::ivec2{10,GetGSComponent<Background>()->GetSize().y}}, {GetGSComponent<Background>()->GetSize()} }));

    //why? we have to use reference??
    //https://stackoverflow.com/questions/25827924/function-cannot-be-referenced-as-it-is-a-deleted-function
    GetGSComponent<CS230::Camera>()->SetPosition({ 0.0, 0.0 });
    GetGSComponent<CS230::Camera>()->SetLimit({ { 0,0 }, { GetGSComponent<Background>()->GetSize() - Engine::GetWindow().GetSize() } });
    //why?
}                                                                                                                                                                                                                           

void Mode1::Update([[maybe_unused]] double dt) {
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
    ////why we use instance !!!
    ////https://stackoverflow.com/questions/9818515/what-does-error-a-nonstatic-member-reference-must-be-relative-to-a-specific-o

    GetGSComponent<CS230::Camera>()->Update(cat_ptr->GetPosition());

    if (static_cast<int>(GetGSComponent<CS230::Timer>()->Remaining()) < last_timer) {
        last_timer = static_cast<int>(GetGSComponent<CS230::Timer>()->Remaining());
        update_timer_text(last_timer);
    }
    if (last_timer == 0) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }

    if (score != GetGSComponent<Score>()->Value()) {
        update_score_text();
    }
    

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {

        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Mode1::Unload() {
    GetGSComponent<Background>()->Unload();
    GetGSComponent<CS230::GameObjectManager>()->Unload();
    ClearGSComponents();
    cat_ptr = nullptr;
    starting_floor_ptr = nullptr;
    delete timer_texture;
    timer_texture = nullptr;

    delete score_texture;
    score_texture = nullptr;
}

void Mode1::Draw() {
    Engine::GetWindow().Clear(0x000000FF);
    GetGSComponent<Background>()->Draw(*(GetGSComponent<CS230::Camera>()));

    Math::TransformationMatrix camera_matrix = GetGSComponent<CS230::Camera>()->GetMatrix();

    GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);
    timer_texture->Draw(Math::TranslationMatrix(Math::ivec2{ Engine::GetWindow().GetSize().x - 10 - timer_texture->GetSize().x, Engine::GetWindow().GetSize().y - timer_texture->GetSize().y - 5}));
    score_texture->Draw(Math::TranslationMatrix(Math::ivec2{ Engine::GetWindow().GetSize().x - 10 - score_texture->GetSize().x, Engine::GetWindow().GetSize().y - score_texture->GetSize().y - 75}));


}

