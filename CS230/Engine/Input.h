/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Input.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 17, 2025
*/

#pragma once
#include <vector>

namespace CS230 {
    class Input {
    public:
        enum class Keys {
            A, B, C, D, E, F, G, H, I, J, K, L, M,
            N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            Space, Enter, Left, Up, Right, Down, One, Two,Escape,Tab,
            Count //count has to be end!!!
            
        };
        Input();
        void Update();
        void SetKeyDown(Keys key, bool value);
        bool KeyDown(Keys key);
        bool KeyJustPressed(Keys key);
        bool KeyJustReleased(Keys key);

    private:
        std::vector<bool> keys_down;
		std::vector<bool> previous_keys_down;

    };
}

