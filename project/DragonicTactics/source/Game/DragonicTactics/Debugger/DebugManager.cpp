/**
 * \file
 * \author Sangyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "./Engine/Engine.h"
<<<<<<< HEAD
=======
#include "./Engine/GameStateManager.h"
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
#include "./Engine/Input.h"
#include "./Engine/Logger.h"
#include "./Engine/SoundManager.h"
#include "DebugConsole.h"
#include "DebugManager.h"
#include "DebugVisualizer.h"
<<<<<<< HEAD
=======
#include "Game/DragonicTactics/Objects/Character.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/StateComponents/TurnManager.h"

#include <algorithm>
#include <cctype>

namespace {
Character* FindCharacterByName(const std::string& name)
{
  auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  if (!grid)
	return nullptr;
  for (auto* ch : grid->GetAllCharacters())
  {
	if (!ch)
	  continue;
	std::string type = ch->TypeName();
	std::transform(type.begin(), type.end(), type.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	if (type == name)
	  return ch;
  }
  return nullptr;
}
} // namespace
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835

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
<<<<<<< HEAD
=======

  RegisterGameCommands();
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
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

<<<<<<< HEAD

=======
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
void DebugManager::ProcessInput()
{
  CS230::Input& input = Engine::Instance().GetInput();

<<<<<<< HEAD
  if (input.KeyJustPressed(CS230::Input::Keys::Tab))
  {
	ToggleDebugTools();
=======
  // F1: debug_mode 자체 토글 (항상 작동 — Tab 없이도 진입 가능)
  if (input.KeyJustPressed(CS230::Input::Keys::F1))
  {
    ToggleDebugMode();
  }

  // Tab: debug_mode가 true일 때만 Debug Tools 패널 토글
  if (debug_mode && input.KeyJustPressed(CS230::Input::Keys::Tab))
  {
    ToggleDebugTools();
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
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

void DebugManager::DrawImGui([[maybe_unused]]const GridSystem* grid)
{
#if defined(DEVELOPER_VERSION)
<<<<<<< HEAD
  if (!initialized_)
=======
  if (!initialized_ || !debug_mode)
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
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
#endif // DEVELOPER_VERSION
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

<<<<<<< HEAD
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

=======
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
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

	// === Sound ===
	ImGui::Text("Sound");
	ImGui::Spacing();

	SoundManager& sm = Engine::GetSoundManager();

	// BGM Vol
	float bgm_vol = sm.GetBGMVolume();
	if (ImGui::SliderFloat("BGM Volume", &bgm_vol, 0.0f, 1.0f))
	  sm.SetBGMVolume(bgm_vol);

	// BGM Play control
	if (sm.IsBGMPlaying())
	{
	  if (ImGui::Button("Pause BGM"))
		sm.PauseBGM();
	  ImGui::SameLine();
	  if (ImGui::Button("Stop BGM"))
		sm.StopBGM();
	}
	else if (sm.IsBGMPaused())
	{
	  if (ImGui::Button("Resume BGM"))
		sm.ResumeBGM();
	  ImGui::SameLine();
	  if (ImGui::Button("Stop BGM"))
		sm.StopBGM();
	}
	else
	{
	  ImGui::TextDisabled("No BGM playing");
	}

	ImGui::Spacing();

	// SFX VOl
	float sfx_vol = sm.GetSFXVolume();
	if (ImGui::SliderFloat("SFX Volume", &sfx_vol, 0.0f, 1.0f))
	  sm.SetSFXVolume(sfx_vol);

	if (ImGui::Button("Stop All SFX"))
	  sm.StopAllSFX();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	//===========================================

	// === Info ===
<<<<<<< HEAD
	ImGui::TextWrapped("Tab: Toggle This Panel\nESC: Close Panel");
=======
	ImGui::TextWrapped("F1: Toggle Debug Mode\nTab: Toggle This Panel");
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
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
<<<<<<< HEAD
=======
  if (!enabled)
  {
    show_debug_tools_ = false;
    if (visualizer_) visualizer_->ClosePanel();
    if (console_ && console_->IsOpen()) console_->ToggleConsole();
  }
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
}

void DebugManager::ToggleDebugMode()
{
<<<<<<< HEAD
  debug_mode = !debug_mode;
=======
  SetDebugMode(!debug_mode);
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
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
<<<<<<< HEAD
=======

void DebugManager::RegisterGameCommands()
{
  if (!console_)
	return;

  // set_hp dragon 30
  console_->RegisterCommand(
	"set_hp",
	[](std::vector<std::string> args)
	{
	  if (args.size() < 2)
		return;
	  if (auto* ch = FindCharacterByName(args[0]))
		ch->SetHP(std::stoi(args[1]));
	},
	"set_hp <target> <value> — HP 직접 설정");

  // set_ap dragon 3
  console_->RegisterCommand(
	"set_ap",
	[](std::vector<std::string> args)
	{
	  if (args.size() < 2)
		return;
	  if (auto* ch = FindCharacterByName(args[0]))
		ch->SetActionPoints(std::stoi(args[1]));
	},
	"set_ap <target> <value> — AP 설정");

  // end_turn
  console_->RegisterCommand(
	"end_turn",
	[](std::vector<std::string>)
	{
	  auto* tm = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
	  if (tm)
		tm->EndCurrentTurn();
	},
	"end_turn — 현재 턴 강제 종료");

  // kill fighter
  console_->RegisterCommand(
	"kill",
	[](std::vector<std::string> args)
	{
	  if (args.empty())
		return;
	  auto* ch = FindCharacterByName(args[0]);
	  if (ch && ch->IsAlive())
		ch->TakeDamage(ch->GetHP() + 1, nullptr);
	},
	"kill <target> — 즉사 (DeathEvent 발행됨)");

  // add_effect dragon Fear 3
  console_->RegisterCommand(
	"add_effect",
	[](std::vector<std::string> args)
	{
	  if (args.size() < 3)
		return;
	  if (auto* ch = FindCharacterByName(args[0]))
		ch->AddEffect(args[1], std::stoi(args[2]));
	},
	"add_effect <target> <effect_name> <duration> — 상태이상 부여");

  // spell_restore dragon 1
  console_->RegisterCommand(
	"spell_restore",
	[](std::vector<std::string> args)
	{
	  if (args.size() < 2)
		return;
	  auto* ch = FindCharacterByName(args[0]);
	  if (!ch)
		return;
	  if (auto* slots = ch->GetSpellSlots())
		slots->RestoreOne(std::stoi(args[1]));
	},
	"spell_restore <target> <level> — 해당 레벨 슬롯 1개 복구");

  // spell_restore_all dragon
  console_->RegisterCommand(
	"spell_restore_all",
	[](std::vector<std::string> args)
	{
	  if (args.empty())
		return;
	  auto* ch = FindCharacterByName(args[0]);
	  if (!ch)
		return;
	  if (auto* slots = ch->GetSpellSlots())
	  {
		const auto& m = slots->GetMaxSlots();
		if (!m.empty())
		  slots->Recover(m.rbegin()->first);
	  }
	},
	"spell_restore_all <target> — 모든 슬롯 최대치 복구");

  // spell_consume dragon 2
  console_->RegisterCommand(
	"spell_consume",
	[](std::vector<std::string> args)
	{
	  if (args.size() < 2)
		return;
	  auto* ch = FindCharacterByName(args[0]);
	  if (!ch)
		return;
	  if (auto* slots = ch->GetSpellSlots())
		slots->Consume(std::stoi(args[1]));
	},
	"spell_consume <target> <level> — 해당 레벨 슬롯 1개 소모");

  // spell_slots dragon
  console_->RegisterCommand(
	"spell_slots",
	[this](std::vector<std::string> args)
	{
	  if (args.empty())
		return;
	  auto* ch = FindCharacterByName(args[0]);
	  if (!ch)
	  {
		console_->Print("Character not found: " + args[0]);
		return;
	  }
	  auto* slots = ch->GetSpellSlots();
	  if (!slots)
	  {
		console_->Print(args[0] + " has no spell slots");
		return;
	  }
	  console_->Print("=== " + ch->TypeName() + " Spell Slots ===");
	  for (const auto& [lv, max_cnt] : slots->GetMaxSlots())
	  {
		int cur = ch->GetSpellSlotCount(lv);
		console_->Print("  Lv" + std::to_string(lv) + ": " + std::to_string(cur) + " / " + std::to_string(max_cnt));
	  }
	},
	"spell_slots <target> — 슬롯 잔량 전체 출력");
}
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
