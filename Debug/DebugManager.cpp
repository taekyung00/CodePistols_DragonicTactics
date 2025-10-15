#include "DebugManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"

DebugManager& DebugManager::Instance()
{
    static DebugManager instance;
    return instance;
}

void DebugManager::SetDebugMode(bool enabled)
{
    debug_mode = enabled;
}

bool DebugManager::IsDebugMode() const
{
    return debug_mode;
}

void DebugManager::ToggleGridOverlay()
{
    grid_overlay = !grid_overlay;
    Engine::GetLogger().LogEvent(grid_overlay ? "Grid overlay ON" : "Grid overlay OFF");
}

void DebugManager::ToggleCollisionBoxes()
{
    collision_boxes = !collision_boxes;
    Engine::GetLogger().LogEvent(collision_boxes ? "Collision boxes ON" : "Collision boxes OFF");
}

void DebugManager::ToggleAIPathVisualization(){
    ai_path_visualization = !ai_path_visualization;
    Engine::GetLogger().LogEvent(ai_path_visualization ? "AI Path Visualization ON" : "AI Path Visualization OFF");
}

void DebugManager::ToggleStatusInfo()
{
    status_info = !status_info;
    Engine::GetLogger().LogEvent(status_info ? "Status info ON" : "Status info OFF");
}

void DebugManager::ToggleGodMode(){
    god_mode = !god_mode;
    Engine::GetLogger().LogEvent(god_mode ? "GodMode ON" : "GodMode OFF");
}

bool DebugManager::IsGridOverlayEnabled() const { return debug_mode && grid_overlay; }
bool DebugManager::IsCollisionBoxesEnabled() const { return debug_mode && collision_boxes; }
bool DebugManager::IsStatusInfoEnabled() const { return debug_mode && status_info; }
bool DebugManager::IsGodModeEnabled() const { return debug_mode && god_mode; }
