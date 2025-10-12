/*
Copyright (C) 2025 Taekyung Ho
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Test.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    Oct 8, 2025
*/

#pragma once
#include "../Engine/GameState.h"

class Fighter;

class Test : public CS230::GameState {
public:
    Test();
    void Load() override;
    void Update(double) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "Test";
    }
private:
    Fighter* fighter;
    void LogFighterStatus();
    //void test_subscribe_publish_singleSubscriber();
};