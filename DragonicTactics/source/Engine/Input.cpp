#include "pch.h"

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

#include "Input.h"
#include "Engine.h"
#include "Logger.h"
#include "Window.h"


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

    bool Input::MouseDown(int button) const
    { 
        if (button < 0 || button >= 3) return false;
        return current_mouse_state[button];
    }

    bool Input::MouseJustPressed(int button) const
    {
        if (button < 0 || button >= 3) return false;
        return current_mouse_state[button] && !last_mouse_state[button];
    }

    bool Input::MouseJustReleased(int button) const
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
        int window_height = Engine::GetWindow().GetSize().y;
        mouse_position.x = pos.x;
        mouse_position.y = window_height - pos.y;
    }

    void Input::SetMouseScroll(double offset)
    {
        mouse_scroll += offset;
    }

}
