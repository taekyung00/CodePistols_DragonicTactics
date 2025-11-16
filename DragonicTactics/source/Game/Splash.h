/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/

#pragma once
#include "../Engine/GameState.hpp"
#include "../Engine/Texture.hpp"

class Splash : public CS230::GameState
{
public:
    // Splash();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;


private:
    double                          counter = 0;
    std::shared_ptr<CS230::Texture> texture;
};
