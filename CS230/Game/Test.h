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
#include "../Game/System/GridSystem.h"

class Dragon;
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
    void test_subscribe_publish_singleSubscriber();
    void test_multiple_subscribers_sameEvent();
    void test_multiple_different_events();
    void test_EventData_CompleteTransfer();
    void test_EventData_MultiplePublishes();
    Fighter* fighter;
    Dragon* dragon;
    void LogFighterStatus();
    void LogDragonStatus();
    //void test_subscribe_publish_singleSubscriber();
};