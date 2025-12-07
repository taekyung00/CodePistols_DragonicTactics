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

class Character; // 전방 선언 추가

class GamePlayUIManager
{
public:
  void ShowDamageText(int damage, Math::vec2 position, Math::vec2 size);
  void Update(double dt);
  void Draw(Math::TransformationMatrix camera_matrix);

  // ========================================
  // 신규 추가: 지속적 스탯 표시
  // ========================================

  /// @brief 추적할 캐릭터들 설정 (GamePlay::Load에서 호출)
  void SetCharacters(const std::vector<Character*>& characters);

  /// @brief 화면 오른쪽에 모든 캐릭터의 스탯 패널 그리기
  void DrawCharacterStatsPanel(Math::TransformationMatrix camera_matrix);

private:
  struct DamageText
  {
    std::string text;
    Math::vec2  position;
    Math::vec2  size;
    double      lifetime;
  };

  std::vector<DamageText> m_damage_texts;

  // ========================================
  // 신규 추가: 캐릭터 추적
  // ========================================
  std::vector<Character*> m_characters; // 추적할 캐릭터 목록
};