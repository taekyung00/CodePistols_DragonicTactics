/**
 * \file
 * \author Rudy Castan
 * \date 2024 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "Demo/DemoShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Window.hpp"
#include "Game/Splash.h"

namespace
{
    [[maybe_unused]] int  gWindowWidth  = 400;
    [[maybe_unused]] int  gWindowHeight = 400;
    [[maybe_unused]] bool gNeedResize   = false;
}


#if defined(__EMSCRIPTEN__)
#    include <emscripten.h>
#    include <emscripten/bind.h>
#    include <emscripten/em_asm.h>

void main_loop()
{
    Engine& engine = Engine::Instance();
    if (gNeedResize)
    {
        Engine::GetWindow().ForceResize(gWindowWidth, gWindowHeight);
        gNeedResize = false;
    }

    engine.Update();

    if (engine.HasGameEnded())
    {
        emscripten_cancel_main_loop();
        engine.Stop();
    }
}

EMSCRIPTEN_BINDINGS(main_window)
{
    emscripten::function(
        "setWindowSize", emscripten::optional_override(
                             [](int sizeX, int sizeY)
                             {
                                 sizeX                  = (sizeX < 400) ? 400 : sizeX;
                                 sizeY                  = (sizeY < 400) ? 400 : sizeY;
                                 const auto window_size = Engine::GetWindow().GetSize();
                                 if (sizeX != window_size.x || sizeY != window_size.y)
                                 {
                                     gNeedResize   = true;
                                     gWindowWidth  = sizeX;
                                     gWindowHeight = sizeY;
                                 }
                             }));
}
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Engine& engine = Engine::Instance();
    engine.Start("Taekyung Ho OpenGL Engine");
    // engine.AddFont("Assets/fonts/Font_Simple.png");
    // engine.AddFont("Assets/fonts/Font_Outlined.png");
    engine.GetGameStateManager().PushState<Splash>();

    //std::ifstream in_file("Assets/sprites/CS230_Final/Tears.spt");

    //if (in_file.is_open() == false)
    //{
    //    throw std::runtime_error("Failed to load ");
    //}

#if !defined(__EMSCRIPTEN__)
    while (engine.HasGameEnded() == false)
    {
        engine.Update();
    }
    engine.Stop();
#else
    // https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop
    constexpr bool simulate_infinite_loop  = true;
    constexpr int  match_browser_framerate = -1;
    emscripten_set_main_loop(main_loop, match_browser_framerate, simulate_infinite_loop);
#endif
    return 0;
}
