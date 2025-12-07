#include "pch.h"

#include "./Engine/Engine.h"
#include "./Engine/Input.h"
#include "./Engine/Logger.h"
#include "DebugConsole.h"
#include "DebugManager.h"
#include "DebugVisualizer.h"

DebugManager::DebugManager() : console_(std::make_unique<DebugConsole>()), visualizer_(std::make_unique<DebugVisualizer>())
{
}

DebugManager::~DebugManager() = default;

void DebugManager::Init()
{
  if (initialized_)
  {
	return;
  }

  Engine::GetLogger().LogEvent("DebugManager: Initializing debug systems");

  // Initialize visualizer (subscribes to events)
  if (visualizer_)
  {
	visualizer_->Init();
  }

  initialized_ = true;
  Engine::GetLogger().LogEvent("DebugManager: Initialization complete");
}

void DebugManager::Update(double dt)
{
  if (!initialized_)
  {
	return;
  }

  // Process keyboard input
  ProcessInput();

  // Update console
  if (console_)
  {
	console_->Update(dt);
  }

  // Update visualizer
  if (visualizer_)
  {
	visualizer_->Update(dt);
  }
}

void DebugManager::ProcessInput()
{
  CS230::Input& input = Engine::Instance().GetInput();

  if (input.KeyJustPressed(CS230::Input::Keys::Tab))
  {
	ToggleDebugTools();
  }
}

void DebugManager::Draw(const GridSystem* grid)
{
  if (!initialized_ || !debug_mode)
  {
	return;
  }

  // Draw in-game overlays (grid only)
  if (visualizer_)
  {
	visualizer_->DrawGridOverlay(grid);
  }
}

void DebugManager::DrawImGui(const GridSystem* grid)
{
  if (!initialized_)
  {
	return;
  }

  // Draw debug control panel
  if (show_debug_tools_)
  {
	DrawDebugControlPanel();
  }

  // Draw ImGui debug panel
  if (visualizer_)
  {
	visualizer_->DrawImGuiDebugPanel(grid);
  }

  // Draw console window
  if (console_)
  {
	console_->DrawImGui();
  }
}

void DebugManager::DrawDebugControlPanel()
{
  ImGui::SetNextWindowSize(ImVec2(280, 450), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);

  if (ImGui::Begin("Debug Tools", &show_debug_tools_, ImGuiWindowFlags_NoResize))
  {
	// === Main Toggle ===
	ImGui::PushStyleColor(ImGuiCol_Button, debug_mode ? ImVec4(0.2f, 0.6f, 0.2f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
	if (ImGui::Button("DEBUG MODE", ImVec2(-1, 30)))
	{
	  ToggleDebugMode();
	}
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// === Visualization (TODO : Not perfect) ===
	// ImGui::Text("Visualization");
	// ImGui::Spacing();
	//
	// ImGui::Checkbox("Grid Overlay", &grid_overlay);
	// ImGui::Checkbox("Collision Boxes", &collision_boxes);
	// ImGui::Checkbox("AI Paths", &ai_path_visualization);
	// ImGui::Checkbox("Status Info", &status_info);
	//
	// ImGui::Spacing();
	// ImGui::Separator();
	// ImGui::Spacing();

	// === Panels ===
	ImGui::Text("Panels");
	ImGui::Spacing();

	bool panel_open = IsDebugPanelOpen();
	if (ImGui::Checkbox("Events & Stats", &panel_open))
	{
	  ToggleDebugPanel();
	}

	bool console_open = console_ && console_->IsOpen();
	if (ImGui::Checkbox("Console (Tab)", &console_open))
	{
	  if (console_)
	  {
		console_->ToggleConsole();
	  }
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// === Cheats ===
	ImGui::Text("Cheats");
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, god_mode ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::Checkbox("God Mode", &god_mode);
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// === Info ===
	ImGui::TextWrapped("Tab: Toggle This Panel\nESC: Close Panel");
  }
  ImGui::End();
}

void DebugManager::ToggleDebugTools()
{
  show_debug_tools_ = !show_debug_tools_;
  Engine::GetLogger().LogEvent(show_debug_tools_ ? "Debug Tools OPENED" : "Debug Tools CLOSED");
}

bool DebugManager::IsDebugToolsOpen() const
{
  return show_debug_tools_;
}

void DebugManager::ToggleDebugPanel()
{
  if (visualizer_)
  {
	visualizer_->ToggleDebugPanel();
  }
}

bool DebugManager::IsDebugPanelOpen() const
{
  return visualizer_ ? visualizer_->IsDebugPanelOpen() : false;
}

DebugConsole* DebugManager::GetConsole()
{
  return console_.get();
}

DebugVisualizer* DebugManager::GetVisualizer()
{
  return visualizer_.get();
}

void DebugManager::SetDebugMode(bool enabled)
{
  debug_mode = enabled;
}

void DebugManager::ToggleDebugMode()
{
  debug_mode = !debug_mode;
  Engine::GetLogger().LogEvent(debug_mode ? "Debug Mode ON" : "Debug Mode OFF");
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

void DebugManager::ToggleAIPathVisualization()
{
  ai_path_visualization = !ai_path_visualization;
  Engine::GetLogger().LogEvent(ai_path_visualization ? "AI Path Visualization ON" : "AI Path Visualization OFF");
}

void DebugManager::ToggleDiceHistory()
{
  dice_history = !dice_history;
  Engine::GetLogger().LogEvent(dice_history ? "Dice History ON" : "Dice History OFF");
}

void DebugManager::ToggleCombatInspector()
{
  combat_inspector = !combat_inspector;
  Engine::GetLogger().LogEvent(combat_inspector ? "Combat Inspector ON" : "Combat Inspector OFF");
}

void DebugManager::ToggleEventTracer()
{
  event_tracer = !event_tracer;
  Engine::GetLogger().LogEvent(event_tracer ? "Event Tracer ON" : "Event Tracer OFF");
}

void DebugManager::ToggleStatusInfo()
{
  status_info = !status_info;
  Engine::GetLogger().LogEvent(status_info ? "Status info ON" : "Status info OFF");
}

void DebugManager::ToggleGodMode()
{
  god_mode = !god_mode;
  Engine::GetLogger().LogEvent(god_mode ? "God Mode ON" : "God Mode OFF");
}

bool DebugManager::IsGridOverlayEnabled() const
{
  return debug_mode && grid_overlay;
}

bool DebugManager::IsCollisionBoxesEnabled() const
{
  return debug_mode && collision_boxes;
}

bool DebugManager::IsAIPathVisualizationEnabled() const
{
  return debug_mode && ai_path_visualization;
}

bool DebugManager::IsStatusInfoEnabled() const
{
  return debug_mode && status_info;
}

bool DebugManager::IsDiceHistoryEnabled() const
{
  return debug_mode && dice_history;
}

bool DebugManager::IsCombatInspectorEnabled() const
{
  return debug_mode && combat_inspector;
}

bool DebugManager::IsEventTracerEnabled() const
{
  return debug_mode && event_tracer;
}

bool DebugManager::IsGodModeEnabled() const
{
  return debug_mode && god_mode;
}
