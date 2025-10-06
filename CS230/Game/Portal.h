/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Portal.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 28, 2025
*/
#pragma once
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"
class Portal : public CS230::GameObject {
public:
    Portal(int to_state, Math::irect boundary);
    std::string TypeName() override { return "Portal"; }
    virtual GameObjectTypes Type() override { return GameObjectTypes::Portal; }

    void GoToState();
private:
    int to_state;
};

