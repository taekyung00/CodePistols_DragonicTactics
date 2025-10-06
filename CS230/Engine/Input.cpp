/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Input.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 17, 2025
*/

#include "Input.h"
#include "Engine.h"
#include <raylib.h>


constexpr int cs230_to_rl(CS230::Input::Keys cs230_key) noexcept {
    switch (cs230_key) {
    case CS230::Input::Keys::A:
        return KEY_A;
    case CS230::Input::Keys::B:
        return KEY_B;
    case CS230::Input::Keys::C:
        return KEY_C;
    case CS230::Input::Keys::D:
        return KEY_D;
    case CS230::Input::Keys::E:
        return KEY_E;
    case CS230::Input::Keys::F:
        return KEY_F;
    case CS230::Input::Keys::G:
        return KEY_G;
    case CS230::Input::Keys::H:
        return KEY_H;
    case CS230::Input::Keys::I:
        return KEY_I;
    case CS230::Input::Keys::J:
        return KEY_J;
    case CS230::Input::Keys::K:
        return KEY_K;
    case CS230::Input::Keys::L:
        return KEY_L;
    case CS230::Input::Keys::M:
        return KEY_M;
    case CS230::Input::Keys::N:
        return KEY_N;
    case CS230::Input::Keys::O:
        return KEY_O;
    case CS230::Input::Keys::P:
        return KEY_P;
    case CS230::Input::Keys::Q:
        return KEY_Q;
    case CS230::Input::Keys::R:
        return KEY_R;
    case CS230::Input::Keys::S:
        return KEY_S;
    case CS230::Input::Keys::T:
        return KEY_T;
    case CS230::Input::Keys::U:
        return KEY_U;
    case CS230::Input::Keys::V:
        return KEY_V;
    case CS230::Input::Keys::W:
        return KEY_W;
    case CS230::Input::Keys::X:
        return KEY_X;
    case CS230::Input::Keys::Y:
        return KEY_Y;
    case CS230::Input::Keys::Z:
        return KEY_Z;
    case CS230::Input::Keys::Space:
        return KEY_SPACE;
    case CS230::Input::Keys::Enter:
        return KEY_ENTER;
    case CS230::Input::Keys::Left:
        return KEY_LEFT;
    case CS230::Input::Keys::Right:
        return KEY_RIGHT;
    case CS230::Input::Keys::Up:
        return KEY_UP;
    case CS230::Input::Keys::Down:
        return KEY_DOWN;
    case CS230::Input::Keys::One:
        return KEY_ONE;
    case CS230::Input::Keys::Two:
        return KEY_TWO;
    case CS230::Input::Keys::Escape:
        return KEY_ESCAPE;
    case CS230::Input::Keys::Tab:
        return KEY_TAB;
    default:
        return -1;
    }

}

constexpr CS230::Input::Keys& operator++(CS230::Input::Keys& key) noexcept {
    key = static_cast<CS230::Input::Keys>(static_cast<unsigned>(key) + 1);
    return key;
}

CS230::Input::Input() {
    keys_down.resize(static_cast<int>(Keys::Count));
    previous_keys_down.resize(static_cast<int>(Keys::Count));
}

void CS230::Input::Update() {
    previous_keys_down = keys_down;
    for (Keys key = Keys::A; key < Keys::Count; ++key) {
        const auto rl_key = cs230_to_rl(static_cast<Keys>(key));
        SetKeyDown(key, IsKeyDown(rl_key));
        if (KeyJustPressed(key)) {
            Engine::GetLogger().LogDebug("Key Pressed");
        }
        else if (KeyJustReleased(key)) {
            Engine::GetLogger().LogDebug("Key Released");
        }
    }

}

void CS230::Input::SetKeyDown(Keys key, bool value){
    keys_down[static_cast<int>(key)] = value;
}

bool CS230::Input::KeyDown(Keys key) {
    return keys_down[static_cast<int>(key)];
}

bool CS230::Input::KeyJustPressed(Keys key) {
    return keys_down[static_cast<int>(key)] == true && previous_keys_down[static_cast<int>(key)] == false;
}

bool CS230::Input::KeyJustReleased(Keys key) {
    return keys_down[static_cast<int>(key)] == false && previous_keys_down[static_cast<int>(key)] == true;
}



