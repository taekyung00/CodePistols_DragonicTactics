/**
 * \file
 * \author Sangyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "./Engine/Component.h"
#include <functional>
#include <memory>
#include <string>

class DebugConsole;
class DebugVisualizer;
class GridSystem;

class DebugManager : public CS230::Component
{
  public:
  DebugManager();
  ~DebugManager();

  // Lifecycle methods
  void Init();							  // Call once at game start
  void Update(double dt);				  // Call every frame
  void Draw(const GridSystem* grid);	  // Draw in-game overlays
  void DrawImGui(const GridSystem* grid); // Draw ImGui panels

  // Process input (keyboard shortcuts)
  void ProcessInput();

  // Debug mode toggles
  void SetDebugMode(bool enabled);
  void ToggleDebugMode();
  bool IsDebugMode() const;

  // Main debug panel toggle
  void ToggleDebugTools();
  bool IsDebugToolsOpen() const;

  // Debug panel toggle
  void ToggleDebugPanel();
  bool IsDebugPanelOpen() const;

  // Visual debug toggles
  void ToggleGridOverlay();
  void ToggleCollisionBoxes();
  void ToggleAIPathVisualization();
  void ToggleStatusInfo();
  void ToggleDiceHistory();
  void ToggleCombatInspector();
  void ToggleEventTracer();

  bool IsGridOverlayEnabled() const;
  bool IsCollisionBoxesEnabled() const;
  bool IsAIPathVisualizationEnabled() const;
  bool IsStatusInfoEnabled() const;
  bool IsDiceHistoryEnabled() const;
  bool IsCombatInspectorEnabled() const;
  bool IsEventTracerEnabled() const;

  // God mode features
  void ToggleGodMode();
  bool IsGodModeEnabled() const;

  // Access to subsystems (if needed)
  DebugConsole*	   GetConsole();
  DebugVisualizer* GetVisualizer();

  private:
  void DrawDebugControlPanel();

  bool debug_mode{ false };
  bool show_debug_tools_{ true };
  bool grid_overlay{ false };
  bool collision_boxes{ false };
  bool status_info{ false };
  bool ai_path_visualization{ false };
  bool dice_history{ false };
  bool combat_inspector{ false };
  bool event_tracer{ false };
  bool god_mode{ false };

  // Owned subsystems
  std::unique_ptr<DebugConsole>	   console_;
  std::unique_ptr<DebugVisualizer> visualizer_;
  bool							   initialized_{ false };
};
