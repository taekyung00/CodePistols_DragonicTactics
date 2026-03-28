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

void GamePlayUIManager::ShowDamageLog(std::string str, Math::vec2 position, Math::vec2 size) 
{
    DamageText text{ str, position, size, 1.0 };
    m_damage_log.push_back(text);
}

void GamePlayUIManager::ShowGameEnd(std::string&& text)
{
    game_end_text = std::make_unique<std::string>(text);
}

void GamePlayUIManager::Update(double dt)
{

	// 마우스 입력 가져오기
	Math::vec2 mouse_pos = Engine::GetInput().GetMousePos();
	bool mouse_clicked   = Engine::GetInput().MouseJustPressed(0); 

	button_manager_.Update(mouse_pos, mouse_clicked);

  for (auto& text : m_damage_texts)
  {
	text.lifetime -= dt;
  }
  for (auto& text : m_damage_log)
  {
	text.lifetime -= dt;
  }

  m_damage_texts.erase(std::remove_if(m_damage_texts.begin(), m_damage_texts.end(), [](const DamageText& text) { return text.lifetime <= 0; }), m_damage_texts.end());
  
  m_damage_log.erase(
		std::remove_if(m_damage_log.begin(), m_damage_log.end(),
			[](const DamageText& text) { return text.lifetime <= 0; }),
		m_damage_log.end()
	);
}

void GamePlayUIManager::Draw([[maybe_unused]] Math::TransformationMatrix camera_matrix)
{
	button_manager_.Draw(camera_matrix);
	auto& textMng = Engine::GetTextManager();
	for (const auto& text : m_damage_texts)
	{
		textMng.DrawText(text.text, text.position, Fonts::Outlined, text.size, CS200::VIOLET);
	}

  	int i = 0;
	for (const auto& text : m_damage_log) {
		textMng.DrawText(text.text, text.position + Math::vec2{ 0.0, 30.0 * i },Fonts::Outlined, text.size, CS200::RED);
		++i;
	}

	if(game_end_text) {
		auto size = Engine::GetWindow().GetSize();
		textMng.DrawText(*game_end_text, Math::ivec2{ 0, size.y / 2 }, Fonts::Outlined, Math::vec2{GAME_END_TEXT_SIZE, GAME_END_TEXT_SIZE}, CS200::WHITE);
	}

// // 특정 스펠 사용 가능 시 스펠 버튼 추가
// SpellSystem* ss = GetGSComponent<SpellSystem>();
// auto available = ss->GetAvailableSpells(current_character);

// // 기존 스펠 버튼 제거
// for (const auto& spell : cached_spell_buttons_)
//     button_manager_.RemoveButton("spell_" + spell.id);

// // 새 스펠 버튼 생성
// for (int i = 0; i < static_cast<int>(available.size()); ++i)
// {
//     button_manager_.AddButton({
//         "spell_" + available[i].id,
//         { 500.0 + i * 130.0, 700.0 },
//         { 120.0, 35.0 },
//         available[i].spell_name,
//         false,  // 초기 visible
//         current_character->GetSpellSlotCount(available[i].spell_level) == 0  // 슬롯 없으면 disabled
//     });
// }
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
  const double panel_x		  = static_cast<double>(window_size.x) - 330.0;
  const double panel_start_y = static_cast<double>(window_size.y) - 150.0;

  const double panel_height_per_char = 90.0;

  // [설정] 텍스트 레이아웃 상수
  const double	   text_left_margin = 20.0;					// 패널 왼쪽 끝에서 텍스트까지의 거리 (X축 정렬용)
  const double	   line_height		= 30.0;					// 줄 간격
  const double	   first_line_y		= 20.0;					// 패널 상단에서 첫 줄까지의 거리
  const Math::vec2 text_scale		= Math::vec2{ 0.5, 0.5 }; // 폰트 크기

  double current_y = panel_start_y;

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
	double text_x_pos = panel_x + text_left_margin;

	// 1. 이름 (Name)
	std::string name = character->TypeName();
	Engine::GetTextManager().DrawText(name, Math::vec2{ text_x_pos + 40.0, current_y + panel_height_per_char - first_line_y }, Fonts::Outlined, text_scale, CS200::WHITE);

	// 2. HP (Name 아래로 line_height만큼 이동)
	int			current_hp = character->GetHP();
	int			max_hp	   = character->GetMaxHP();
	std::string hp_text	   = "HP: " + std::to_string(current_hp) + " / " + std::to_string(max_hp);

	Engine::GetTextManager().DrawText(hp_text, Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 1.0) }, Fonts::Outlined, text_scale, CS200::RED);

	// 3. AP
	int			current_ap = character->GetActionPoints();
	std::string ap_text	   = "AP: " + std::to_string(current_ap);

	Engine::GetTextManager().DrawText(ap_text, Math::vec2{ text_x_pos + 50.0, current_y + panel_height_per_char - (first_line_y + line_height * 2.0) }, Fonts::Outlined, text_scale, CS200::YELLOW);

	// 4. Speed
	int			speed	   = character->GetMovementRange();
	std::string speed_text = "Speed: " + std::to_string(speed);

	Engine::GetTextManager().DrawText(speed_text, Math::vec2{ text_x_pos + 30.0, current_y + panel_height_per_char - (first_line_y + line_height * 3.0) }, Fonts::Outlined, text_scale, CS200::GREEN);

	// 다음 캐릭터 패널 위치로 이동
	current_y -= panel_height_per_char + 40.0;
  }
}

void GamePlayUIManager::InitButtons()
{
    // 화면 하단 중앙에 버튼 배치 (1200x800 기준)
    // 좌상단 원점 기준, y가 아래로 증가하는 화면 좌표

    constexpr double BTN_W = 170.0;  // 버튼 너비
    constexpr double BTN_H = 50.0;   // 버튼 높이
    constexpr double BTN_Y = 750.0;  // 하단에서 50px 위 (y=800기준)
    constexpr double GAP   = 20.0;   // 버튼 간격
    constexpr double START_X = 30.0; // 시작 x

    // Move 버튼
    button_manager_.AddButton({
        "btn_move",
        { START_X, BTN_Y },
        { BTN_W, BTN_H },
        "Move"
    });

    // Action 버튼
    button_manager_.AddButton({
        "btn_action",
        { START_X + BTN_W + GAP, BTN_Y },
        { BTN_W, BTN_H },
        "Action"
    });

    // End Turn 버튼
    button_manager_.AddButton({
        "btn_end_turn",
        { START_X + (BTN_W + GAP) * 2, BTN_Y },
        { BTN_W, BTN_H },
        "End Turn"
    });

    // Action 서브 버튼 (Action 선택 시에만 표시)
    button_manager_.AddButton({
        "btn_attack",
        { START_X + BTN_W + GAP, BTN_Y - BTN_H - GAP },
        { BTN_W, BTN_H },
        "Attack",
        false  // 초기에는 숨김
    });

    button_manager_.AddButton({
        "btn_spell",
        { START_X + BTN_W + GAP, BTN_Y - (BTN_H + GAP) * 2 },
        { BTN_W, BTN_H },
        "Spell",
        false  // 초기에는 숨김
    });
}

ButtonManager& GamePlayUIManager::GetButtons(){
  return button_manager_;
}