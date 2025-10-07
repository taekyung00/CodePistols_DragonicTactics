/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  CharacterStates.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 08, 2025
*/
#pragma once
#include "../Engine/GameObject.h" 
#include <string>


class State_Idle : public CS230::GameObject::State {
public:
    void Enter(CS230::GameObject* object) override;
    void Update(CS230::GameObject* object, double dt) override;
    void CheckExit(CS230::GameObject* object) override;
    std::string GetName() override { return "Idle"; }
};

class State_Moving : public CS230::GameObject::State {
public:
    void Enter(CS230::GameObject* object) override;
    void Update(CS230::GameObject* object, double dt) override;
    void CheckExit(CS230::GameObject* object) override;
    std::string GetName() override { return "Moving"; }
}; 


class State_Attacking : public CS230::GameObject::State {
public:
    void Enter(CS230::GameObject* object) override;
    void Update(CS230::GameObject* object, double dt) override;
    void CheckExit(CS230::GameObject* object) override;
    std::string GetName() override { return "Attacking"; }
}; 

class State_TurnEnd : public CS230::GameObject::State {
public:
    void Enter(CS230::GameObject* object) override;
    void Update(CS230::GameObject* object, double dt) override;
    void CheckExit(CS230::GameObject* object) override;
    std::string GetName() override { return "TurnEnd"; }
}; 