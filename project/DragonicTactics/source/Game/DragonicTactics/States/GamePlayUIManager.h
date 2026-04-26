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
<<<<<<< HEAD
=======
#include <deque>
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
#include <string>
#include <vector>
#include "ButtonManager.h"

class Character;
class PlayerInputHandler; // 전방 선언

class GamePlayUIManager
{
  public:
  void ShowDamageText(int damage, Math::vec2 position, Math::vec2 size);
  void ShowDamageLog(std::string str, Math::vec2 position, Math::vec2 size);
  void ShowGameEnd(std::string&& text);
  void Update(double dt);
  void Draw(Math::TransformationMatrix camera_matrix);



  void SetCharacters(const std::vector<Character*>& characters);

  void DrawCharacterStatsPanel(Math::TransformationMatrix camera_matrix);

  void InitButtons(PlayerInputHandler* inputHandler);          // 버튼 초기 배치
  ButtonManager& GetButtons(); // PlayerInputHandler에서 접근용

  void AddSpellLog(const std::string& caster_name, const std::string& spell_name, int level);

<<<<<<< HEAD
=======
  // Battle log
  void OnTurnStarted(const std::string& actor_name, int turn_number);
  void AddBattleLogEntry(const std::string& line);

>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
  private:
  struct DamageText
  {
	std::string text;
	Math::vec2	position;
	Math::vec2	size;
	double		lifetime;
  };

  struct SpellLog
  {
	std::string text;
	double		lifetime;
  };

  std::vector<DamageText> m_damage_texts;
  std::vector<DamageText> m_damage_log;
  std::vector<SpellLog>   m_spell_logs;

  static constexpr double SPELL_LOG_LIFETIME  = 3.0;
  static constexpr int    SPELL_LOG_MAX_COUNT = 5;

  const double GAME_END_TEXT_SIZE = 2.0;

  std::unique_ptr<std::string> game_end_text = nullptr;

<<<<<<< HEAD
  std::vector<Character*> m_characters; 
  ButtonManager button_manager_;
=======
  std::vector<Character*> m_characters;
  ButtonManager button_manager_;

  // === Battle Log ===
  struct TurnEntry
  {
    int                      turn_number;
    std::string              actor_name;
    std::vector<std::string> lines;
  };

  void DrawBattleLog();

  std::deque<TurnEntry> turn_history_;
  bool                  show_battle_log_{ false };
  static constexpr int  MAX_LOG_TURNS = 5;
>>>>>>> a9fcc3c17804591a293c7d78ce2c79ee42247835
};