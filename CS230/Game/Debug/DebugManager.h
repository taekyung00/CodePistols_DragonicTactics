#pragma once

class DebugManager {
public:
    static DebugManager& Instance();

    void SetDebugMode(bool enabled);
    bool IsDebugMode() const;

    void ToggleGridOverlay();
    void ToggleCollisionBoxes();
    void ToggleStatusInfo();
};

void RegisterCommand(
    const std::string& name,
    std::function<void(std::vector<std::string>)> handler,
    const std::string& helpText
);

void ExecuteCommand(const std::string& commandLine);