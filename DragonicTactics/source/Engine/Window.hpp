/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "CS200/RGBA.hpp"
#include "Vec2.hpp"
#include <functional>
#include <gsl/gsl>
#include <string_view>

struct SDL_Window;
typedef void*           SDL_GLContext;
typedef union SDL_Event SDL_Event;

namespace CS230
{
    class Window
    {
    public:
        Window() noexcept = default;
        ~Window();

        Window(const Window&)                = delete;
        Window& operator=(const Window&)     = delete;
        Window(Window&&) noexcept            = delete;
        Window& operator=(Window&&) noexcept = delete;

    public:
        void Start(std::string_view title);
        void Update();
        bool IsClosed() const;

        [[nodiscard]] Math::ivec2 GetSize() const noexcept
        {
            return { window_size.x, window_size.y };
        }

        void          Clear(CS200::RGBA color);
        void          ForceResize(int w, int h);
        void          SetWindowPosition(int x, int y);
        SDL_Window*   GetSDLWindow() const;
        SDL_GLContext GetGLContext() const;

        using WindowEventCallback = std::function<void(const SDL_Event&)>;
        void SetEventCallback(WindowEventCallback callback);

        static void SetBackgroundColor(float r, float g, float b) noexcept;

        // Math::ivec2 GetWindowSize() const;

    private:

        void setupSDLWindow(std::string_view title);
        void setupOpenGL();


        const int                     default_width      = 800;
        const int                     default_height     = 600;
        Math::ivec2                   window_size        = { default_width, default_height };
        static constexpr unsigned int default_background = UINT_MAX;
        static constexpr CS200::RGBA  default_background1{ CS200::WHITE };

        gsl::owner<SDL_Window*>   sdl_window = nullptr;
        gsl::owner<SDL_GLContext> gl_context = nullptr;
        bool                      closed     = false;
        // Math::ivec2               size       = { 800, 600 };

        WindowEventCallback eventCallback;

        std::function<void(SDL_Event)> func_event_callback;
        static float                   background_r;
        static float                   background_g;
        static float                   background_b;
    };
}
