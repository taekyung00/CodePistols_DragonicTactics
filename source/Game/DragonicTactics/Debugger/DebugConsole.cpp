#include "DebugConsole.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Input.hpp"

void DebugConsole::Update([[maybe_unused]]double dt)
{
#ifdef _DEBUG
    if(Engine::Instance().GetInput().KeyJustPressed(CS230::Input::Keys::Enter)){
        ToggleConsole();
    }
#endif
}

//void DebugConsole::Draw()
//{
//
//}

void DebugConsole::ToggleConsole()
{
    open = !open;
}

bool DebugConsole::IsOpen() const
{
    return open;
}

//void DebugConsole::RegisterCommand(const std::string& name, std::function<void(std::vector<std::string>)> handler, const std::string& helpText)
//{
//}
//
//void DebugConsole::ExecuteCommand(const std::string& commandLine)
//{
//}
