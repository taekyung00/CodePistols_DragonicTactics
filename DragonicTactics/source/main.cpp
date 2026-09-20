/**
 * \file
 * \author Rudy Castan
 * \date 2024 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "Engine/Engine.h"
#include "Engine/GameStateManager.h"
#include "Engine/Window.h"
#include "Game/Splash.h"

#if defined(_WIN32)
#  include <windows.h>
#  include <dbghelp.h>
#  include <ctime>
#  pragma comment(lib, "dbghelp.lib")

static LONG WINAPI CrashHandler(EXCEPTION_POINTERS* ep)
{
    // 파일명: crash_YYYYMMDD_HHMMSS.dmp
    std::time_t now = std::time(nullptr);
    std::tm     lt{};
    localtime_s(&lt, &now);
    char fname[64];
    std::strftime(fname, sizeof(fname), "crash_%Y%m%d_%H%M%S.dmp", &lt);

    HANDLE hFile = CreateFileA(fname, GENERIC_WRITE, 0, nullptr,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION mei{};
        mei.ThreadId          = GetCurrentThreadId();
        mei.ExceptionPointers = ep;
        mei.ClientPointers    = FALSE;

        // MiniDumpWithFullMemory: 전체 힙 + 스택 + 모듈 정보 포함
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                          hFile, MiniDumpWithFullMemory, &mei, nullptr, nullptr);
        CloseHandle(hFile);

        // 파일명을 콘솔에도 출력
        OutputDebugStringA("[CrashHandler] Minidump saved: ");
        OutputDebugStringA(fname);
        OutputDebugStringA("\n");
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

namespace
{
  [[maybe_unused]] int	gWindowWidth  = 400;
  [[maybe_unused]] int	gWindowHeight = 400;
  [[maybe_unused]] bool gNeedResize	  = false;
}


#if defined(__EMSCRIPTEN__)
#  include <emscripten.h>
#  include <emscripten/bind.h>
#  include <emscripten/em_asm.h>

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
						 sizeX					= (sizeX < 400) ? 400 : sizeX;
						 sizeY					= (sizeY < 400) ? 400 : sizeY;
						 const auto window_size = Engine::GetWindow().GetSize();
						 if (sizeX != window_size.x || sizeY != window_size.y)
						 {
						   gNeedResize	 = true;
						   gWindowWidth	 = sizeX;
						   gWindowHeight = sizeY;
						 }
					   }));
}
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
#if defined(_WIN32)
  SetUnhandledExceptionFilter(CrashHandler);
#endif

  Engine& engine = Engine::Instance();
  engine.Start("Dragonic Tactics");

  engine.GetGameStateManager().PushState<Splash>();


#if !defined(__EMSCRIPTEN__)
  while (engine.HasGameEnded() == false)
  {
	engine.Update();
  }
  engine.Stop();
#else
  // https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop
  constexpr bool simulate_infinite_loop	 = true;
  constexpr int	 match_browser_framerate = -1;
  emscripten_set_main_loop(main_loop, match_browser_framerate, simulate_infinite_loop);
#endif
  return 0;
}
