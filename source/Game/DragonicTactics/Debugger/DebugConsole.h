#pragma once
#include "./Engine/Matrix.hpp"
#include "./Engine/Vec2.hpp"
#include "./Engine/Component.h"

#include <string>
#include <vector>
#include <functional>
#include <map>

class DebugConsole : public CS230::Component {
public:
    void Update(double dt) override;
    //void Draw();

    void ToggleConsole();
    bool IsOpen() const;
        
    //void RegisterCommand(const std::string& name, std::function<void(std::vector<std::string>)> handler, const std::string& helpText);
    //void ExecuteCommand(const std::string& commandLine);
private:
    bool open{false};
};