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

#include "PlayerInputHandler.h"

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

  m_damage_log.erase(std::remove_if(m_damage_log.begin(), m_damage_log.end(), [](const DamageText& text) { return text.lifetime <= 0; }), m_damage_log.end());
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
  for (const auto& text : m_damage_log)
  {
	textMng.DrawText(text.text, text.position + Math::vec2{ 0.0, 30.0 * i }, Fonts::Outlined, text.size, CS200::RED);
	++i;
  }

  if (game_end_text)
  {
	auto size = Engine::GetWindow().GetSize();
	textMng.DrawText(*game_end_text, Math::ivec2{ 0, size.y / 2 }, Fonts::Outlined, Math::vec2{ GAME_END_TEXT_SIZE, GAME_END_TEXT_SIZE }, CS200::WHITE);
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
  const double panel_x		 = static_cast<double>(window_size.x) - 650.0;
  const double panel_start_y = static_cast<double>(window_size.y) - 200.0;

  const double panel_height_per_char = 180.0;

  // [설정] 텍스트 레이아웃 상수
  const double	   text_left_margin = 20.0;					  // 패널 왼쪽 끝에서 텍스트까지의 거리 (X축 정렬용)
  const double	   line_height		= 30.0;					  // 줄 간격
  const double	   first_line_y		= 20.0;					  // 패널 상단에서 첫 줄까지의 거리
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

	// ── 신규 블록 1: 스펠 슬롯 ──
	SpellSlots* slots = character->GetSpellSlots();
	if (slots)
	{
	  std::string slot_text = "Slots:";
	  for (int lv = 1; lv <= 5; ++lv)
	  {
		int max_count = slots->GetMaxSlotCount(lv);
		if (max_count == 0)
		  continue; // 이 레벨 슬롯 없으면 스킵

		int cur_count = slots->GetSpellSlotCount(lv);
		slot_text += " Lv" + std::to_string(lv) + ":" + std::to_string(cur_count) + "/" + std::to_string(max_count);
	  }
	  Engine::GetTextManager().DrawText(slot_text, Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 4.0) }, Fonts::Outlined, text_scale, CS200::ORANGE);
	}

	// ── 신규 블록 2: 활성 상태 효과 ──
	{
	  const auto& effects = character->GetActiveEffects();
	  std::string fx_text = "FX:";
	  for (const auto& e : effects)
		fx_text += " " + e.name + "(" + std::to_string(e.duration) + ")";

	  Engine::GetTextManager().DrawText(fx_text, Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 5.0) }, Fonts::Outlined, text_scale, CS200::YELLOW);
	}

	// 다음 캐릭터 패널 위치로 이동
	current_y -= panel_height_per_char + 40.0;
  }
}

void GamePlayUIManager::InitButtons(PlayerInputHandler* inputHandler)
{
    constexpr double BTN_W   = 170.0;
    constexpr double BTN_H   = 50.0;
    constexpr double BTN_Y   = 750.0;
    constexpr double GAP     = 20.0;
    constexpr double START_X = 30.0;

    // ── 헬퍼 함수 1: 버튼 생성 및 등록을 깔끔하게 처리 ──
    auto add_btn = [&](const std::string& id, Math::vec2 pos, Math::vec2 size, const std::string& label, bool visible, std::function<void()> onClick = nullptr) {
        Button b;
        b.id = id;
        b.position = pos;
        b.size = size;
        b.label = label;
        b.visible = visible;
        b.on_click = onClick; // 콜백 묶기
        button_manager_.AddButton(b);
    };

    // ── 헬퍼 함수 2: 스펠 버튼 전용 클릭 이벤트(콜백) 생성기 ──
    auto create_spell_callback = [this, inputHandler](const std::string& spell_id, int level) {
        return [this, inputHandler, spell_id, level]() {
            // 버튼이 클릭되는 순간 현재 턴인 캐릭터를 가져옵니다.
            auto* turnMgr = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
            if (!turnMgr) return;
            
            Character* current_char = turnMgr->GetCurrentCharacter();
            if (current_char) {
                Engine::GetLogger().LogEvent("Spell Button Clicked: " + spell_id + " Lv." + std::to_string(level));
                // PlayerInputHandler의 SelectSpell 실행 -> 타일이 빨갛게 변함!
                inputHandler->SelectSpell(spell_id, current_char, level, this->GetButtons());
            }
        };
    };

    // ── 메인 액션 버튼 3종 ────────────────────────────────────────
    add_btn("btn_move",     { START_X, BTN_Y },                    { BTN_W, BTN_H }, "Move",     true);
    add_btn("btn_action",   { START_X + BTN_W + GAP, BTN_Y },      { BTN_W, BTN_H }, "Action",   true);
    add_btn("btn_end_turn", { START_X + (BTN_W+GAP)*2, BTN_Y },    { BTN_W, BTN_H }, "End Turn", true);

    // ── Action 서브 버튼 ──────────────────────────────────────────
    add_btn("btn_attack", { START_X + BTN_W + GAP, BTN_Y - BTN_H - GAP },          { BTN_W, BTN_H }, "Attack", false);
    add_btn("btn_spell",  { START_X + BTN_W + GAP, BTN_Y - (BTN_H+GAP)*2 },        { BTN_W, BTN_H }, "Spell",  false);

    // ── Spell List Cancel ─────────────────────────────────────────
    add_btn("btn_spell_cancel", { START_X + BTN_W + GAP, BTN_Y - (BTN_H+GAP)*3 }, { BTN_W, BTN_H }, "Cancel", false);

    // ── 스펠 버튼 레이아웃 상수 ───────────────────────────────────
    constexpr double SX          = START_X + BTN_W + GAP; 
    constexpr double SW          = BTN_W;                 
    constexpr double ULW         = 130.0;                 
    constexpr double UBW         = 42.0;                  
    constexpr double UGAP        = 4.0;                   

    // ── 비업캐스트 스펠 (단일 버튼) ──────────────────────────────
    add_btn("S_ATK_020", { SX, BTN_Y - (BTN_H+GAP)*4 }, { SW, BTN_H }, "Tail Swipe (Lv.2)", false, create_spell_callback("S_ATK_020", 2));
    add_btn("S_ENH_050", { SX, BTN_Y - (BTN_H+GAP)*5 }, { SW, BTN_H }, "Purify (Lv.1)",     false, create_spell_callback("S_ENH_050", 1));
    add_btn("S_DEB_020", { SX, BTN_Y - (BTN_H+GAP)*6 }, { SW, BTN_H }, "Fearful Cry (Lv.1)",false, create_spell_callback("S_DEB_020", 1));

    // ── 업캐스트 가능 스펠 (레벨별 버튼) ─────────────────────────
    for (int lv = 1; lv <= 5; ++lv) {
        add_btn("S_ATK_010_lv" + std::to_string(lv),
                { SX + ULW + (UBW+UGAP)*(lv-1), BTN_Y - (BTN_H+GAP)*7 },
                { UBW, BTN_H }, "Lv" + std::to_string(lv), false, 
                create_spell_callback("S_ATK_010", lv)); // <- 핵심 로직 연결!
    }

    for (int lv = 0; lv <= 5; ++lv) {
        add_btn("S_ENH_040_lv" + std::to_string(lv),
                { SX + ULW + (UBW+UGAP)*lv, BTN_Y - (BTN_H+GAP)*8 },
                { UBW, BTN_H }, "Lv" + std::to_string(lv), false,
                create_spell_callback("S_ENH_040", lv));
    }

    for (int lv = 3; lv <= 5; ++lv) {
        add_btn("S_ATK_030_lv" + std::to_string(lv),
                { SX + ULW + (UBW+UGAP)*(lv-3), BTN_Y - (BTN_H+GAP)*9 },
                { UBW, BTN_H }, "Lv" + std::to_string(lv), false,
                create_spell_callback("S_ATK_030", lv));
    }

    for (int lv = 3; lv <= 5; ++lv) {
        add_btn("S_ATK_040_lv" + std::to_string(lv),
                { SX + ULW + (UBW+UGAP)*(lv-3), BTN_Y - (BTN_H+GAP)*10 },
                { UBW, BTN_H }, "Lv" + std::to_string(lv), false,
                create_spell_callback("S_ATK_040", lv));
    }

    for (int lv = 2; lv <= 5; ++lv) {
        add_btn("S_GEO_010_lv" + std::to_string(lv),
                { SX + ULW + (UBW+UGAP)*(lv-2), BTN_Y - (BTN_H+GAP)*11 },
                { UBW, BTN_H }, "Lv" + std::to_string(lv), false,
                create_spell_callback("S_GEO_010", lv));
    }

    for (int lv = 1; lv <= 5; ++lv) {
        add_btn("S_GEO_020_lv" + std::to_string(lv),
                { SX + ULW + (UBW+UGAP)*(lv-1), BTN_Y - (BTN_H+GAP)*12 },
                { UBW, BTN_H }, "Lv" + std::to_string(lv), false,
                create_spell_callback("S_GEO_020", lv));
    }

    for (int lv = 0; lv <= 5; ++lv) {
        add_btn("S_GEO_030_lv" + std::to_string(lv),
                { SX + ULW + (UBW+UGAP)*lv, BTN_Y - (BTN_H+GAP)*13 },
                { UBW, BTN_H }, "Lv" + std::to_string(lv), false,
                create_spell_callback("S_GEO_030", lv));
    }
}

ButtonManager& GamePlayUIManager::GetButtons(){
  return button_manager_;
}