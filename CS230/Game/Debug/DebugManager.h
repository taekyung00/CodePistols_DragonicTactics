#pragma once
#include <functional>
#include <string>

class Character;

class DebugManager {
public:
    static DebugManager& Instance();

    // Debug mode toggles
    void SetDebugMode(bool enabled);
    bool IsDebugMode() const;

    // Visual debug toggles
    void ToggleGridOverlay();
    void ToggleCollisionBoxes();
    void ToggleAIPathVisualization();
    void ToggleStatusInfo();
    bool IsGridOverlayEnabled() const;
    bool IsCollisionBoxesEnabled() const;
    bool IsStatusInfoEnabled() const;

    // God mode features
    void ToggleGodMode();
    bool IsGodModeEnabled() const;
    // void SetInvincibility(Character* character, bool enabled);
    // void SetInfiniteActionPoints(Character* character, bool enabled);
    // void SetInfiniteSpellSlots(Character* character, bool enabled);

    // Console commands
    // void ExecuteCommand(const std::string& command);
    // void RegisterCommand(const std::string& name, std::function<void(std::vector<std::string>)> handler);

    // Hot reload
    // void ReloadAllData();
    // void ReloadCharacterData();
    // void ReloadSpellData();
    // void WatchFileChanges();  // Monitor JSON files for changes

private:
    DebugManager() = default;
    ~DebugManager() = default;
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;
    DebugManager(DebugManager&&) = delete;
    DebugManager& operator=(DebugManager&&) = delete;

    bool debug_mode{false};
    bool grid_overlay{false};
    bool collision_boxes{false};
    bool status_info{false};
    bool ai_path_visualization{false};
    bool god_mode{false};
};
