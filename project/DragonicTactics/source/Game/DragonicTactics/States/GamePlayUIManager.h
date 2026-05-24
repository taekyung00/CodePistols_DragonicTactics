/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlayUIManager.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/

// GamePlayUIManager.h
#pragma once
#include "Engine/Matrix.h"
#include "Engine/Vec2.h"
#include <array>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ButtonManager.h"

namespace CS230 { class Texture; }
class Character;
class PlayerInputHandler;
struct TacticalCamera;

class GamePlayUIManager
{
  public:
  void ShowDamageText(int damage, Math::vec2 position, Math::vec2 size);
  void ShowGameEnd(std::string&& text);
  void Update(double dt);
  void Draw(Math::TransformationMatrix camera_matrix);

  void SetCharacters(const std::vector<Character*>& characters);
  void SetPlayer(Character* player);

  void InitButtons(PlayerInputHandler* inputHandler);
  void InitSpellTooltips();
  void InitStatusEffectIcons();
  void SetCamera(const TacticalCamera* camera);
  ButtonManager& GetButtons();
  void DrawWorld();

  void OnTurnStarted(const std::string& actor_name, int turn_number, bool is_player, int round_number);
  void AddBattleLogEntry(const std::string& line);
  bool IsMouseOverLogPanel() const;
  void ScrollLog(double delta);

  void ShowNotice(const std::string& text);

  void DrawHoveredTileOutline();
  void DrawTileOutlineAtPosition(Math::vec2 world_pos, uint32_t border_color);
  
  private:
  struct DamageText
  {
    std::string text;
    Math::vec2  position;
    Math::vec2  size;
    double      lifetime;
  };

  std::string popup_hover_reason_;
  Math::vec2  popup_hover_pos_;
  
  std::vector<DamageText> m_damage_texts;

  const double GAME_END_TEXT_SIZE = 2.0;

  std::unique_ptr<std::string> game_end_text = nullptr;

  std::vector<Character*> m_characters;
  Character*              m_player_ = nullptr;
  ButtonManager button_manager_;

  // Cancel hint pulse timer (Feature 3)
  double m_cancel_hint_time_ = 0.0;

  // Slot icons (index 0~9: spells, 10: End Turn)
  std::vector<std::shared_ptr<CS230::Texture>> slot_icons_;

  // Slot coordinate cache ([0..9]=slots, [10]=End Turn)
  std::array<double, 11> slot_bar_x_{};
  double slot_bar_center_y_ = 0.0;

  // Upcast popup state
  bool        popup_open_           = false;
  std::string popup_spell_id_;
  int         popup_slot_index_     = -1;
  bool        popup_hit_this_frame_ = false;

  // Character hover tooltip
  Character* hovered_character_ = nullptr;

  // Spell hover tooltip
  std::string  hovered_spell_id_;
  double       hovered_slot_cx_ = 0.0;
  std::unordered_map<std::string, std::vector<std::string>> spell_tooltip_cache_;
  std::unordered_map<std::string, double>                  spell_tooltip_widths_;

  // Status effect icons
  std::unordered_map<std::string, std::shared_ptr<CS230::Texture>> status_icon_textures_;
  std::unordered_map<std::string, std::string>                     effect_descriptions_;
  std::unordered_map<std::string, double>                          effect_tooltip_widths_;
  std::string hovered_effect_name_;
  int         hovered_effect_duration_ = 0;

  // Character portraits for left-side status panel (key = CharacterTypes int value)
  std::unordered_map<int, std::shared_ptr<CS230::Texture>> portrait_textures_;

  // InputHandler pointer
  PlayerInputHandler*   m_input_handler_ptr_ = nullptr;
  const TacticalCamera* m_camera_            = nullptr;

  // Virtual-resolution mouse position (updated in Update(), used in Draw*)
  Math::vec2 m_virtual_mouse_ = { 0.0, 0.0 };

  // === Battle Log ===
  struct TurnEntry
  {
    int                      round_number;
    int                      turn_number;
    std::string              actor_name;
    bool                     is_player;
    std::vector<std::string> lines;
  };

  static constexpr int    MAX_LOG_ROUNDS = 5;
  static constexpr double LOG_PANEL_W    = 320.0;
  static constexpr double LOG_PANEL_H    = 512.0;
  static constexpr double LOG_PANEL_X    = 1600.0 - 64.0 - 10.0 - LOG_PANEL_W - 2.0;
  static constexpr double LOG_PANEL_Y    = 900.0 * 0.5 + LOG_PANEL_H * 0.5;
  static constexpr double LOG_TITLE_H    = 45.0;
  static constexpr double LOG_LINE_H     = 22.0;
  static constexpr double LOG_INDENT     = 12.0;
  static constexpr double LOG_SB_W       = 8.0;
  static constexpr double LOG_SB_X       = LOG_PANEL_X + LOG_PANEL_W - LOG_SB_W - 2.0;

  std::deque<TurnEntry> turn_history_;
  bool                  show_battle_log_{ false };

  double log_scroll_offset_{ 0.0 };
  bool   log_scrollbar_dragging_{ false };
  double log_drag_start_mouse_y_{ 0.0 };
  double log_drag_start_offset_{ 0.0 };

  double end_turn_click_timer_ = 0.0;

  // Notice toast
  std::string m_notice_text_;
  double      m_notice_timer_                = 0.0;
  static constexpr double NOTICE_DURATION   = 1.5;
  static constexpr double NOTICE_W          = 460.0;
  static constexpr double NOTICE_H          = 44.0;

  void DrawCharacterStatsPanel(Math::TransformationMatrix camera_matrix);
  void DrawNotice();
  void   DrawBattleLog();
  double ComputeLogContentHeight() const;
  void DrawSlotBar();
  void DrawUicastPopup();
  void DrawTurnIndicator();
  void DrawHoverTooltip();
  void DrawSpellTooltip();
  void DrawStatusEffectTooltip();
  void DrawStatusEffectPanel();
  void DrawActionLabel();
  void DrawDisableReasonTooltip();
  void DrawDragonWorldHoverTooltip();
  void DrawDragonHUD();
  void DrawCancelHint();
};
