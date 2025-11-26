#include "pch.h"

/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Engine.hpp"

#include "CS200/ImGuiHelper.hpp"
#include "CS200/ImmediateRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "FPS.hpp"
#include "Font.hpp"
#include "GameState.hpp"
#include "GameStateManager.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "TextManager.hpp"
#include "TextureManager.hpp"
#include "Timer.hpp"
#include "Window.hpp"

#include <chrono>

namespace {
    CS230::Input::Keys convert_sdl_to_cs230(SDL_Scancode scancode)
    {
        switch (scancode)
        {
            case SDL_SCANCODE_TAB: return CS230::Input::Keys::Tab;
            case SDL_SCANCODE_LEFT: return CS230::Input::Keys::Left;
            case SDL_SCANCODE_RIGHT: return CS230::Input::Keys::Right;
            case SDL_SCANCODE_UP: return CS230::Input::Keys::Up;
            case SDL_SCANCODE_DOWN: return CS230::Input::Keys::Down;
  
            case SDL_SCANCODE_SPACE: return CS230::Input::Keys::Space;
            case SDL_SCANCODE_RETURN: return CS230::Input::Keys::Enter;
            case SDL_SCANCODE_ESCAPE: return CS230::Input::Keys::Escape;

            case SDL_SCANCODE_A: return CS230::Input::Keys::A;
            case SDL_SCANCODE_B: return CS230::Input::Keys::B;
            case SDL_SCANCODE_C: return CS230::Input::Keys::C;
            case SDL_SCANCODE_D: return CS230::Input::Keys::D;
            case SDL_SCANCODE_E: return CS230::Input::Keys::E;
            case SDL_SCANCODE_F: return CS230::Input::Keys::F;
            case SDL_SCANCODE_G: return CS230::Input::Keys::G;
            case SDL_SCANCODE_H: return CS230::Input::Keys::H;
            case SDL_SCANCODE_I: return CS230::Input::Keys::I;
            case SDL_SCANCODE_J: return CS230::Input::Keys::J;
            case SDL_SCANCODE_K: return CS230::Input::Keys::K;
            case SDL_SCANCODE_L: return CS230::Input::Keys::L;
            case SDL_SCANCODE_M: return CS230::Input::Keys::M;
            case SDL_SCANCODE_N: return CS230::Input::Keys::N;
            case SDL_SCANCODE_O: return CS230::Input::Keys::O;
            case SDL_SCANCODE_P: return CS230::Input::Keys::P;
            case SDL_SCANCODE_Q: return CS230::Input::Keys::Q;
            case SDL_SCANCODE_R: return CS230::Input::Keys::R;
            case SDL_SCANCODE_S: return CS230::Input::Keys::S;
            case SDL_SCANCODE_T: return CS230::Input::Keys::T;
            case SDL_SCANCODE_U: return CS230::Input::Keys::U;
            case SDL_SCANCODE_V: return CS230::Input::Keys::V;
            case SDL_SCANCODE_W: return CS230::Input::Keys::W;
            case SDL_SCANCODE_X: return CS230::Input::Keys::X;
            case SDL_SCANCODE_Y: return CS230::Input::Keys::Y;
            case SDL_SCANCODE_Z: return CS230::Input::Keys::Z;
            default: return CS230::Input::Keys::Count;
        }
    }
}

// Pimpl implementation class
class Engine::Impl
{
public:
    Impl()
        :
#ifdef DEVELOPER_VERSION
          logger(CS230::Logger::Severity::Debug, true, std::chrono::system_clock::now())
#else
          logger(CS230::Logger::Severity::Debug, true, std::chrono::system_clock::now())
#endif
          ,
          window{}, input{}
    {
    }

    CS230::Logger              logger;
    CS230::Window              window{};
    CS230::Input               input{};
    ImGuiHelper::Viewport      viewport{};
    util::FPS                  fps{};
    util::Timer                timer{};
    WindowEnvironment          environment{};
    CS230::GameStateManager    gameStateManager{};
    CS200::ImmediateRenderer2D renderer2D{};
    CS230::TextureManager      textureManager{};
    TextManager                textManager{};
};


Engine& Engine::Instance()
{
    static Engine instance;
    return instance;
}

CS230::Logger& Engine::GetLogger()
{
    return Instance().impl->logger;
}

CS230::Window& Engine::GetWindow()
{
    return Instance().impl->window;
}

CS230::Input& Engine::GetInput()
{
    return Instance().impl->input;
}

const WindowEnvironment& Engine::GetWindowEnvironment()
{
    return Instance().impl->environment;
}

CS230::GameStateManager& Engine::GetGameStateManager()
{
    return Instance().impl->gameStateManager;
}

CS200::IRenderer2D& Engine::GetRenderer2D()
{
    return Instance().impl->renderer2D;
}

CS230::TextureManager& Engine::GetTextureManager()
{
    return Instance().impl->textureManager;
}

TextManager& Engine::GetTextManager()
{
    return Instance().impl->textManager;
}

void Engine::OnEvent(const SDL_Event& event)
{
    ImGuiHelper::FeedEvent(event);
    
    switch (event.type)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            CS230::Input::Keys key = convert_sdl_to_cs230(event.key.keysym.scancode);
            if (key != CS230::Input::Keys::Count)
            {
                if (event.type == SDL_KEYDOWN)
                    impl->input.SetKeyPressed(key);
                else
                    impl->input.SetKeyReleased(key);
            }
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button >= 1 && event.button.button <= 3)
                impl->input.SetMousePressed(event.button.button - 1);
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button >= 1 && event.button.button <= 3)
                impl->input.SetMouseReleased(event.button.button - 1);
            break;

        case SDL_MOUSEMOTION:
            impl->input.SetMousePos({ static_cast<float>(event.motion.x), static_cast<float>(event.motion.y) });
            break;

        case SDL_MOUSEWHEEL:
            impl->input.SetMouseScroll(static_cast<double>(event.wheel.y));
            break;

        default:
            break;
    }
}

void Engine::Start(std::string_view window_title)
{
    impl->logger.LogEvent("Engine Started");
#if defined(DEVELOPER_VERSION)
    impl->logger.LogEvent("Developer Build");
#endif
    impl->window.Start(window_title);
    auto& window = impl->window;

    const auto window_size = window.GetSize();
    impl->viewport         = { 0, 0, window_size.x, window_size.y };
    CS200::RenderingAPI::SetViewport(window_size);
    impl->environment.DisplaySize = { static_cast<double>(window_size.x), static_cast<double>(window_size.y) };
    ImGuiHelper::Initialize(window.GetSDLWindow(), window.GetGLContext());
    window.SetEventCallback([this](const SDL_Event& event){this->OnEvent(event);});
    impl->renderer2D.Init();
    impl->timer.ResetTimeStamp();
    impl->textManager.Init();
}

void Engine::Stop()
{
    impl->renderer2D.Shutdown();
    impl->gameStateManager.Clear();
    ImGuiHelper::Shutdown();
    impl->logger.LogEvent("Engine Stopped");
}

void Engine::Update()
{
    updateEnvironment();

    // service update
    auto& environment = impl->environment;
    impl->input.Update();
    impl->window.Update();

    auto& state_manager = impl->gameStateManager;
    state_manager.Update(environment.DeltaTime);
    const auto        viewport      = impl->viewport;
    const Math::ivec2 viewport_size = { viewport.width, viewport.height };
    CS200::RenderingAPI::SetViewport(viewport_size, { viewport.x, viewport.y });
    state_manager.Draw();
    impl->viewport = ImGuiHelper::Begin();
    state_manager.DrawImGui();
    ImGuiHelper::End();
}

bool Engine::HasGameEnded()
{
    return impl->window.IsClosed() || impl->gameStateManager.HasGameEnded();
}

Engine::Engine() : impl(new Impl())
{
}

Engine::~Engine()
{
    delete impl;
}

void Engine::updateEnvironment()
{
    auto& environment     = impl->environment;
    environment.DeltaTime = impl->timer.GetElapsedSeconds();
    impl->timer.ResetTimeStamp();
    environment.ElapsedTime += environment.DeltaTime;
    ++environment.FrameCount;
    impl->fps.Update(environment.DeltaTime);
    environment.FPS               = impl->fps;
    const auto viewport           = impl->viewport;
    impl->environment.DisplaySize = { static_cast<double>(viewport.width), static_cast<double>(viewport.height) };
}