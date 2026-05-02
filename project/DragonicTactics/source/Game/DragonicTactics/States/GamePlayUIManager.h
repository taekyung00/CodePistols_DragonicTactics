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

  void InitButtons(PlayerInputHandler* inputHandler);
  void SetCamera(const TacticalCamera* camera);
  ButtonManager& GetButtons();

  void OnTurnStarted(const std::string& actor_name, int turn_number);
  void AddBattleLogEntry(const std::string& line);

  private:
  struct DamageText
  {
    std::string text;
    Math::vec2  position;
    Math::vec2  size;
    double      lifetime;
  };

  std::vector<DamageText> m_damage_texts;

  const double GAME_END_TEXT_SIZE = 2.0;

  std::unique_ptr<std::string> game_end_text = nullptr;

  std::vector<Character*> m_characters;
  ButtonManager button_manager_;

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

  // Hover tooltip
  Character* hovered_character_ = nullptr;

  // InputHandler pointer
  PlayerInputHandler*   m_input_handler_ptr_ = nullptr;
  const TacticalCamera* m_camera_            = nullptr;

  // Virtual-resolution mouse position (updated in Update(), used in Draw*)
  Math::vec2 m_virtual_mouse_ = { 0.0, 0.0 };

  // === Battle Log ===
  struct TurnEntry
  {
    int                      turn_number;
    std::string              actor_name;
    std::vector<std::string> lines;
  };

  std::deque<TurnEntry> turn_history_;
  bool                  show_battle_log_{ false };
  static constexpr int  MAX_LOG_TURNS = 5;

  void DrawCharacterStatsPanel(Math::TransformationMatrix camera_matrix);
  void DrawBattleLog();
  void DrawSlotBar();
  void DrawUicastPopup();
  void DrawTurnIndicator();
  void DrawHoverTooltip();
  void DrawActionLabel();
};
