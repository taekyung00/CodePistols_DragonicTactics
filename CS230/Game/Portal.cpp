/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Portal.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 28, 2025
*/
#include "Portal.h"

Portal::Portal(int to_state, Math::irect boundary) :
    GameObject(static_cast<Math::vec2>(boundary.point_1)),
    to_state(to_state)
{
    AddGOComponent(new CS230::RectCollision({ Math::ivec2{ 0, 0 }, boundary.Size() }, this));
}

void Portal::GoToState() {
    Engine::GetGameStateManager().SetNextGameState(to_state);
}
