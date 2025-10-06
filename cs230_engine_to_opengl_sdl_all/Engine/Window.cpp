/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author TODO Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Window.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine.hpp"
#include "Error.hpp"
#include "Logger.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <functional>
#include <sstream>

namespace
{
    void hint_gl(SDL_GLattr attr, int value)
    {
        // // https://wiki.libsdl.org/SDL2/SDL_GL_SetAttribute
        if (const auto success = SDL_GL_SetAttribute(attr, value); success != 0)
        {
            Engine::GetLogger().LogError(std::string{ "Failed to Set GL Attribute: " } + SDL_GetError());
        }
    }
}

namespace CS230
{
    // TODO implement class here
}
