#include "DebugConsole.hpp"
#include "../Engine/Engine.h"
#include "../Engine/Input.h"

void DebugConsole::Update(double dt)
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
