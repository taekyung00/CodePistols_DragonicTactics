/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  main.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 4, 2025
*/

#include <iostream>

#include "Engine/Engine.h"
#include "Game/Mode1.h"
#include "Game/Mode2.h"
#include "Game/Splash.h"
#include "Game/MainMenu.h"

int main() {
    try {
        Engine& engine = Engine::Instance();
        engine.Start("Assignment 12 - taekyung.ho@digipen.edu");

        engine.AddFont("Assets/Font_Simple.png");
        engine.AddFont("Assets/Font_Outlined.png");

        Splash splash;
        engine.GetGameStateManager().AddGameState(splash);
        MainMenu mainmenu;
        engine.GetGameStateManager().AddGameState(mainmenu);
        Mode1 mode1;
        engine.GetGameStateManager().AddGameState(mode1);
        Mode2 mode2;
        engine.GetGameStateManager().AddGameState(mode2);
        


        while (engine.HasGameEnded() == false) {
            engine.Update();
        }

        engine.Stop();

        return 0;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

}


