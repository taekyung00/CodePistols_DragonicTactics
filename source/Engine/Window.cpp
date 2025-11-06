/**
 * \file Window.cpp
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Window.hpp"
#include "../OpenGL/GL.hpp"
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

    /*Basic setup*/
    void Window::setupSDLWindow(std::string_view title)
    {
        // Part 1 - Initialize SDL for visual use
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw_error_message("Failed to init SDK error: ", SDL_GetError());
        }

// Part 2 - Configure OpenGL context attributes (before window creation)
#if defined(IS_WEBGL2)
        hint_gl(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        hint_gl(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        hint_gl(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
        hint_gl(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
        hint_gl(SDL_GL_DOUBLEBUFFER, true);
        hint_gl(SDL_GL_STENCIL_SIZE, 8);
        hint_gl(SDL_GL_DEPTH_SIZE, 24);
        hint_gl(SDL_GL_RED_SIZE, 8);
        hint_gl(SDL_GL_GREEN_SIZE, 8);
        hint_gl(SDL_GL_BLUE_SIZE, 8);
        hint_gl(SDL_GL_ALPHA_SIZE, 8);
        hint_gl(SDL_GL_MULTISAMPLEBUFFERS, 1);
        hint_gl(SDL_GL_MULTISAMPLESAMPLES, 4);

        // Part 3 - Create the SDL window
        sdl_window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, default_width, default_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (sdl_window == nullptr)
        {
            throw_error_message("Failed to create window: ", SDL_GetError());
        }
    }

    /*Setup GL*/
    void Window::setupOpenGL()
    {
        // Create OpenGL context
        if (gl_context = SDL_GL_CreateContext(sdl_window); gl_context == nullptr)
        {
            throw_error_message("Failed to create opengl context: ", SDL_GetError());
        }

        // Make context current
        SDL_GL_MakeCurrent(sdl_window, gl_context);

        // Initialize GLEW for extension loading
        if (const auto result = glewInit(); GLEW_OK != result)
        {
            throw_error_message("Unable to initialize GLEW - error: ", glewGetErrorString(result));
        }

        // Configure VSync
        constexpr int ADAPTIVE_VSYNC = -1;
        constexpr int VSYNC          = 1;
        if (const auto result = SDL_GL_SetSwapInterval(ADAPTIVE_VSYNC); result != 0)
        {
            SDL_GL_SetSwapInterval(VSYNC);
        }

        // Initialize our rendering abstraction layer
        CS200::RenderingAPI::Init();
    }

    void Window::Start(std::string_view title)
    {
        setupSDLWindow(title);
        setupOpenGL();
        window_size.x = default_width;
        window_size.y = default_height;

        // Get actual drawable size for high-DPI displays
        SDL_GL_GetDrawableSize(sdl_window, &window_size.x, &window_size.y);


        GL::Viewport(0, 0, window_size.x, window_size.y);

        // Set initial clear color through our rendering abstraction
        CS200::RenderingAPI::SetClearColor(default_background);
    }

    void Window::Update()
    {
        SDL_GL_SwapWindow(sdl_window);


        SDL_Event event{ 0 };
        while (SDL_PollEvent(&event) != 0)
        {
            eventCallback(event);

            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_CLOSE: closed = true; break;
                case SDL_WINDOWEVENT_RESIZED: window_size = { event.window.data1, event.window.data2 }; break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    SDL_GL_GetDrawableSize(sdl_window, &window_size.x, &window_size.y);
                    GL::Viewport(0, 0, window_size.x, window_size.y);
                    break;
                    break;
                default: break;
            }
        }
    }

    bool Window::IsClosed() const
    {
        return closed;
    }

    void Window::Clear(CS200::RGBA color)
    {
        auto c = CS200::unpack_color(color);
        GL::ClearColor(c[0], c[1], c[2], c[3]);
        GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::ForceResize(int desired_width, int desired_height)
    {
        SDL_SetWindowSize(sdl_window, desired_width, desired_height);
        window_size.x = desired_width;
        window_size.y = desired_height;
    }

    void Window::SetWindowPosition(int x, int y)
    {
        SDL_SetWindowPosition(sdl_window,x,y);
    }

    SDL_Window* Window::GetSDLWindow() const
    {
        return sdl_window;
    }

    SDL_GLContext Window::GetGLContext() const
    {
        return gl_context;
    }

    void Window::SetEventCallback(WindowEventCallback callback)
    {
        eventCallback = std::move(callback);
    }

    float Window::background_r = 0.0f;
    float Window::background_g = 0.0f;
    float Window::background_b = 0.0f;

    void Window::SetBackgroundColor(float r, float g, float b) noexcept
    {
        background_r = r;
        background_g = g;
        background_b = b;
    }

    //Math::ivec2 Window::GetWindowSize() const
    //{
    //    return window_size;
    //}

    Window::~Window()
    {
        if (gl_context)
        {
            SDL_GL_DeleteContext(gl_context);
        }
        if (sdl_window)
        {
            SDL_DestroyWindow(sdl_window);
        }

        SDL_Quit();
    }
}
