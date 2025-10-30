/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \author Seungju Song
 * \date 2025 Fall
 * \update 10-30-2025
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Input.hpp"
#include "Engine.hpp"
#include "Logger.hpp"
#include <SDL.h>

namespace CS230
{
    Input::Input() : mouse_position(0.0f, 0.0f), mouse_scroll(0.0)
    {
        Init();
    }

    void Input::Init()
    {
        previousKeys.fill(false);
        currentKeys.fill(false);
        current_mouse_state.fill(false);
        last_mouse_state.fill(false);
    }

    void Input::Update()
    {
        previousKeys = currentKeys;
        // via SDL get keyboard state
        // mark each keyboard that is down
        for (Keys key = Keys::A; key < Keys::Count; ++key)
        {
            const auto   sdl_key        = convert_cs230_to_sdl(static_cast<Keys>(key));
            const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
            SetKeyDown(key, keyboard_state[sdl_key]);
            if (KeyJustPressed(key))
            {
                Engine::GetLogger().LogDebug("Key Pressed");
            }
            else if (KeyJustReleased(key))
            {
                Engine::GetLogger().LogDebug("Key Released");
            }
        }

        last_mouse_state = current_mouse_state;
        mouse_scroll = 0.0;
    }

    bool Input::KeyDown(Input::Keys key) const
    {
        return currentKeys[static_cast<std::size_t>(key)];
    }

    bool Input::KeyJustPressed(Input::Keys key) const
    {
        const std::size_t index = static_cast<std::size_t>(key);
        return currentKeys[index] && !previousKeys[index];
    }

    bool Input::KeyJustReleased(Input::Keys key) const
    {
        const std::size_t index = static_cast<std::size_t>(key);
        return !currentKeys[index] && previousKeys[index];
    }

    bool Input::MouseDown(int button)
    {
        if (button < 0 || button >= 3) return false;
        return current_mouse_state[button];
    }

    bool Input::MouseJustPressed(int button)
    {
        if (button < 0 || button >= 3) return false;
        return current_mouse_state[button] && !last_mouse_state[button];
    }

    bool Input::MouseJustReleased(int button)
    {
        if (button < 0 || button >= 3) return false;
        return !current_mouse_state[button] && last_mouse_state[button];
    }

    Math::vec2 Input::GetMousePos() const
    {
        return mouse_position;
    }

    double Input::GetMouseScroll() const
    {
        return mouse_scroll;
    }

    void Input::SetKeyDown(Input::Keys key, bool is_pressed)
    {
        currentKeys[static_cast<std::size_t>(key)] = is_pressed;
    }

    void Input::SetKeyPressed(Keys key)
    {
        currentKeys[static_cast<std::size_t>(key)] = true;
    }

    void Input::SetKeyReleased(Keys key)
    {
        currentKeys[static_cast<std::size_t>(key)] = false;
    }

    void Input::SetMousePressed(int button)
    {
        if (button < 0 || button >= 3) return;
        current_mouse_state[button] = true;
    }

    void Input::SetMouseReleased(int button)
    {
        if (button < 0 || button >= 3) return;
        current_mouse_state[button] = false;
    }

    void Input::SetMousePos(Math::vec2 pos)
    {
        mouse_position = pos;
    }

    void Input::SetMouseScroll(double offset)
    {
        mouse_scroll += offset;
    }

    SDL_Scancode convert_cs230_to_sdl(Input::Keys cs230_key)
    {
        switch (cs230_key)
        {
            case CS230::Input::Keys::A: return SDL_SCANCODE_A;
            case CS230::Input::Keys::B: return SDL_SCANCODE_B;
            case CS230::Input::Keys::C: return SDL_SCANCODE_C;
            case CS230::Input::Keys::D: return SDL_SCANCODE_D;
            case CS230::Input::Keys::E: return SDL_SCANCODE_E;
            case CS230::Input::Keys::F: return SDL_SCANCODE_F;
            case CS230::Input::Keys::G: return SDL_SCANCODE_G;
            case CS230::Input::Keys::H: return SDL_SCANCODE_H;
            case CS230::Input::Keys::I: return SDL_SCANCODE_I;
            case CS230::Input::Keys::J: return SDL_SCANCODE_J;
            case CS230::Input::Keys::K: return SDL_SCANCODE_K;
            case CS230::Input::Keys::L: return SDL_SCANCODE_L;
            case CS230::Input::Keys::M: return SDL_SCANCODE_M;
            case CS230::Input::Keys::N: return SDL_SCANCODE_N;
            case CS230::Input::Keys::O: return SDL_SCANCODE_O;
            case CS230::Input::Keys::P: return SDL_SCANCODE_P;
            case CS230::Input::Keys::Q: return SDL_SCANCODE_Q;
            case CS230::Input::Keys::R: return SDL_SCANCODE_R;
            case CS230::Input::Keys::S: return SDL_SCANCODE_S;
            case CS230::Input::Keys::T: return SDL_SCANCODE_T;
            case CS230::Input::Keys::U: return SDL_SCANCODE_U;
            case CS230::Input::Keys::V: return SDL_SCANCODE_V;
            case CS230::Input::Keys::W: return SDL_SCANCODE_W;
            case CS230::Input::Keys::X: return SDL_SCANCODE_X;
            case CS230::Input::Keys::Y: return SDL_SCANCODE_Y;
            case CS230::Input::Keys::Z: return SDL_SCANCODE_Z;
            case CS230::Input::Keys::Space: return SDL_SCANCODE_SPACE;
            case CS230::Input::Keys::Enter: return SDL_SCANCODE_RETURN;
            case CS230::Input::Keys::Left: return SDL_SCANCODE_LEFT;
            case CS230::Input::Keys::Up: return SDL_SCANCODE_UP;
            case CS230::Input::Keys::Right: return SDL_SCANCODE_RIGHT;
            case CS230::Input::Keys::Down: return SDL_SCANCODE_DOWN;
            case CS230::Input::Keys::Escape: return SDL_SCANCODE_ESCAPE;
            case CS230::Input::Keys::Tab: return SDL_SCANCODE_TAB;
            default: return SDL_SCANCODE_UNKNOWN; // SDL's equivalent of an unknown key
        }
    }

}
