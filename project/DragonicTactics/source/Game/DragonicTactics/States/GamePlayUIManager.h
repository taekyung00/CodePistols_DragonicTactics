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
#include <string>
#include <vector>

class Character;

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

  private:
  struct DamageText
  {
	std::string text;
	Math::vec2	position;
	Math::vec2	size;
	double		lifetime;
  };

  std::vector<DamageText> m_damage_texts;
  
  std::vector<DamageText> m_damage_log;

  const double GAME_END_TEXT_SIZE = 2.0;

  std::unique_ptr<std::string> game_end_text = nullptr;

  std::vector<Character*> m_characters; 
};