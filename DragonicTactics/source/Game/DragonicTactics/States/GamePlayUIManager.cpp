/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:   GamePlayUIManager.cpp
Project:     CS230 Engine
Author:      Seungju Song
Created:     November 24, 2025
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
#include "../Objects/Components/ActionPoints.h"
#include "../Objects/Components/StatsComponent.h"

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

void GamePlayUIManager::SetCharacters(const std::vector<Character*>& characters)
{
  m_characters = characters;
  Engine::GetLogger().LogEvent("GamePlayUIManager: Tracking " + std::to_string(m_characters.size()) + " characters for stats display");
}

void GamePlayUIManager::DrawCharacterStatsPanel([[maybe_unused]] Math::TransformationMatrix camera_matrix)
{

	
  if (m_characters.empty())
  {
	return;
  }

  Math::ivec2 window_size = Engine::GetWindow().GetSize();

  // 패널 위치 설정
  const float panel_x		= static_cast<float>(window_size.x) - 330.0f;
  const float panel_start_y = static_cast<float>(window_size.y) - 150.0f;

  const float panel_height_per_char = 90.0f;

  // [설정] 텍스트 레이아웃 상수
  const float	   text_left_margin = 20.0f;					// 패널 왼쪽 끝에서 텍스트까지의 거리 (X축 정렬용)
  const float	   line_height		= 30.0f;					// 줄 간격
  const float	   first_line_y		= 20.0f;					// 패널 상단에서 첫 줄까지의 거리
  const Math::vec2 text_scale		= Math::vec2{ 0.5f, 0.5f }; // 폰트 크기

  float current_y = panel_start_y;

  for (Character* character : m_characters)
  {
	if (character == nullptr)
	{
	  continue;
	}

	// 배경 그리기 (필요하면 주석 해제)
	/*
	Math::TransformationMatrix bg_transform = Math::TranslationMatrix(Math::vec2{ panel_x, current_y });
	// ... 배경 그리기 코드 ...
	*/

	// 텍스트 X 위치 통일 (왼쪽 정렬)
	float text_x_pos = panel_x + text_left_margin;

	// 1. 이름 (Name)
	std::string name = character->TypeName();
	Engine::GetTextManager().DrawText(name, Math::vec2{ text_x_pos + 40, current_y + panel_height_per_char - first_line_y }, Fonts::Outlined, text_scale, CS200::WHITE);

	// 2. HP (Name 아래로 line_height만큼 이동)
	int			current_hp = character->GetHP();
	int			max_hp	   = character->GetMaxHP();
	std::string hp_text	   = "HP: " + std::to_string(current_hp) + " / " + std::to_string(max_hp);

	Engine::GetTextManager().DrawText(hp_text, Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 1) }, Fonts::Outlined, text_scale, CS200::RED);

	// 3. AP
	int			current_ap = character->GetActionPoints();
	std::string ap_text	   = "AP: " + std::to_string(current_ap);

	Engine::GetTextManager().DrawText(ap_text, Math::vec2{ text_x_pos + 50, current_y + panel_height_per_char - (first_line_y + line_height * 2) }, Fonts::Outlined, text_scale, CS200::YELLOW);

	// 4. Speed
	int			speed	   = character->GetMovementRange();
	std::string speed_text = "Speed: " + std::to_string(speed);

	Engine::GetTextManager().DrawText(speed_text, Math::vec2{ text_x_pos + 30, current_y + panel_height_per_char - (first_line_y + line_height * 3) }, Fonts::Outlined, text_scale, CS200::GREEN);

	// 다음 캐릭터 패널 위치로 이동
	current_y -= panel_height_per_char + 40.0f;
  }
}