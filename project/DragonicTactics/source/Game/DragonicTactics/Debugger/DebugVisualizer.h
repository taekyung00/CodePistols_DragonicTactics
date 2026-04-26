/**
 * \file
 * \author Sangyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "./CS200/RGBA.h"
#include "./Engine/Vec2.h"
#include <deque>
#include <string>
#include <vector>

class GridSystem;
class Character;

// Event-driven debug visualization system
// Subscribes to EventBus events and collects debug data passively
// Owned and managed by DebugManager
class DebugVisualizer
{
  public:
  DebugVisualizer()									 = default;
  ~DebugVisualizer()								 = default;
  DebugVisualizer(const DebugVisualizer&)			 = delete;
  DebugVisualizer& operator=(const DebugVisualizer&) = delete;

  // Subscribe to all relevant events (call once at game start)
  void Init();

  // Rendering methods
  void DrawGridOverlay(const GridSystem* grid);		// In-game overlay (minimal)
  void DrawImGuiDebugPanel(const GridSystem* grid); // ImGui panel (detailed info)
  void Update(double dt);

  void SetEnabled(bool enabled)
  {
	enabled_ = enabled;
  }

  bool IsEnabled() const
  {
	return enabled_;
  }

  void ToggleDebugPanel()
  {
	show_debug_panel_ = !show_debug_panel_;
  }

  bool IsDebugPanelOpen() const
  {
	return show_debug_panel_;
  }

  private:
  bool enabled_{ true };
  bool show_debug_panel_{ false }; // Toggle with Tab key

  // === Event Data Storage ===

  // Combat data from CharacterDamagedEvent
  struct DamageInfo
  {
	Character*	target;
	Character*	attacker;
	int			damage;
	Math::ivec2 position;
	double		lifetime;
  };

  std::deque<DamageInfo> recent_damage_;

  // Movement data from CharacterMovedEvent
  struct MovementInfo
  {
	Character*	character;
	Math::ivec2 from;
	Math::ivec2 to;
	double		timestamp;
  };

  std::deque<MovementInfo> recent_moves_;

  // Dice rolls (track via DiceManager query)
  struct DiceRollInfo
  {
	std::string		 notation;
	std::vector<int> rolls;
	int				 total;
	double			 timestamp;
  };

  std::deque<DiceRollInfo> dice_history_;

  // Event log
  struct EventLogEntry
  {
	std::string event_type;
	std::string details;
	double		timestamp;
  };

  std::deque<EventLogEntry> event_log_;

  struct AIDecisionLog
  {
	std::string actorName;
	std::string actionType;
	std::string targetName;
	Math::ivec2 destination;
	std::string reasoning;
	double		timestamp;
  };

  std::deque<AIDecisionLog> ai_decision_history_;

  double game_time_{ 0.0 };

  // === Event Handlers ===
  void OnAIDecision(const struct AIDecisionEvent& event);
  void OnCharacterDamaged(const struct CharacterDamagedEvent& event);
  void OnCharacterDeath(const struct CharacterDeathEvent& event);
  void OnTurnStarted(const struct TurnStartedEvent& event);

  // === Rendering Helpers ===
  // ImGui panels only - no in-game overlays for damage/HP
  void DrawImGuiEventLog();
  void DrawImGuiDiceHistory();
  void DrawImGuiCharacterStats(const GridSystem* grid);
  void DrawImGuiCombatLog();
  void DrawImGuiAIDecisions();

  //=== Utility ===
  std::string GetDecisionTypeString(AIDecisionType type);
};
