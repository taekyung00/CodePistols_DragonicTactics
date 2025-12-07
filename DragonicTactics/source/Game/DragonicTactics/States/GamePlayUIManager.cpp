/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlayUIManager.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/
#include "./CS200/IRenderer2D.h"
#include "./CS200/NDC.h"
#include "./Engine/Engine.h"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Logger.h"
#include "./Engine/TextManager.h"
#include "./Engine/Window.h"
#include "GamePlay.h"

#include "../Debugger/DebugManager.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/DiceManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/MainMenu.h"

#include "Engine/Input.h"
#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"
#include "GamePlayUIManager.h"
#include <imgui.h>

#include "../Objects/Character.h"
#include "../Objects/Components/StatsComponent.h"
#include "../Objects/Components/ActionPoints.h"

void GamePlayUIManager::ShowDamageText(int damage, Math::vec2 position, Math::vec2 size)
{
  DamageText text{ std::to_string(damage), position, size, 0.5 };
  m_damage_texts.push_back(text);
}

void GamePlayUIManager::Update(double dt)
{
  for (auto& text : m_damage_texts)
  {
	text.lifetime -= dt;
  }

  m_damage_texts.erase(std::remove_if(m_damage_texts.begin(), m_damage_texts.end(), [](const DamageText& text) { return text.lifetime <= 0; }), m_damage_texts.end());
}

void GamePlayUIManager::Draw([[maybe_unused]] Math::TransformationMatrix camera_matrix)
{
  for (const auto& text : m_damage_texts)
  {
	  Engine::GetTextManager().DrawText(text.text, text.position, Fonts::Outlined, text.size, CS200::VIOLET);
  }
  DrawCharacterStatsPanel(camera_matrix);
}

// ========================================
// 캐릭터 설정
// ========================================
void GamePlayUIManager::SetCharacters(const std::vector<Character*>& characters)
{
  m_characters = characters;
  Engine::GetLogger().LogEvent("GamePlayUIManager: Tracking " + std::to_string(m_characters.size()) + " characters for stats display");
}

// ========================================
// 캐릭터 스탯 패널 그리기 (화면 오른쪽)
// ========================================
void GamePlayUIManager::DrawCharacterStatsPanel(Math::TransformationMatrix camera_matrix)
{
  if (m_characters.empty())
  {
    return; // 추적할 캐릭터 없음
  }

  // 화면 크기 가져오기
  Math::ivec2 window_size = Engine::GetWindow().GetSize();

  // 패널 위치: 화면 오른쪽 상단
  const float panel_x = static_cast<float>(window_size.x) - 250.0f; // 오른쪽에서 250px 떨어진 곳
  const float panel_start_y = static_cast<float>(window_size.y) - 100.0f; // 위에서 100px 아래

  // 각 캐릭터당 패널 높이
  const float panel_height_per_char = 120.0f;
  const float panel_width = 230.0f;

  float current_y = panel_start_y;

  for (Character* character : m_characters)
  {
    if (character == nullptr)
    {
      continue;
    }

    // ========================================
    // 1. 패널 배경 그리기 (반투명 검정)
    // ========================================
    auto* renderer = Engine::GetTextureManager().GetRenderer2D();

    Math::TransformationMatrix bg_transform = Math::TransformationMatrix::build_translation({ panel_x, current_y });

    renderer->DrawRectangle(
      bg_transform * Math::TransformationMatrix::build_scale({ panel_width, panel_height_per_char }),
      CS200::RGBA{ 30, 30, 30, 180 },  // fill_color: 반투명 어두운 회색
      CS200::RGBA{ 100, 100, 100, 255 }, // line_color: 밝은 회색 테두리
      2.0,  // line_width
      0.5f  // depth
    );

    // ========================================
    // 2. 캐릭터 이름 표시
    // ========================================
    std::string name = character->TypeName();
    Engine::GetTextManager().DrawText(
      name,
      Math::vec2{ panel_x + 10.0f, current_y + panel_height_per_char - 20.0f },
      Fonts::Outlined,
      Math::vec2{ 1.5f, 1.5f },  // 크기
      CS200::WHITE
    );

    // ========================================
    // 3. HP 표시 (빨간색)
    // ========================================
    int current_hp = character->GetHP();
    int max_hp = character->GetMaxHP();
    std::string hp_text = "HP: " + std::to_string(current_hp) + " / " + std::to_string(max_hp);

    Engine::GetTextManager().DrawText(
      hp_text,
      Math::vec2{ panel_x + 10.0f, current_y + panel_height_per_char - 50.0f },
      Fonts::Outlined,
      Math::vec2{ 1.2f, 1.2f },
      CS200::RED
    );

    // ========================================
    // 4. Action Points 표시 (노란색)
    // ========================================
    int current_ap = character->GetActionPoints();
    std::string ap_text = "AP: " + std::to_string(current_ap);

    Engine::GetTextManager().DrawText(
      ap_text,
      Math::vec2{ panel_x + 10.0f, current_y + panel_height_per_char - 75.0f },
      Fonts::Outlined,
      Math::vec2{ 1.2f, 1.2f },
      CS200::YELLOW
    );

    // ========================================
    // 5. Speed (Movement Range) 표시 (초록색)
    // ========================================
    int speed = character->GetMovementRange();
    std::string speed_text = "Speed: " + std::to_string(speed);

    Engine::GetTextManager().DrawText(
      speed_text,
      Math::vec2{ panel_x + 10.0f, current_y + panel_height_per_char - 100.0f },
      Fonts::Outlined,
      Math::vec2{ 1.2f, 1.2f },
      CS200::GREEN
    );

    // 다음 캐릭터 패널 위치로 이동
    current_y -= panel_height_per_char + 10.0f; // 10px 간격
  }
}