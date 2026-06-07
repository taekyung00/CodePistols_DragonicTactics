/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:   GamePlayUIManager.cpp
Project:     CS230 Engine
Author:      Seungju Song
Created:     November 24, 2025
*/
#include <algorithm>
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
#include "Game/DragonicTactics/Types/Events.h"
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
#include "Engine/SoundManager.h"
#include "Game/Settings.h"
#include <sstream>

// Virtual resolution helpers — mirrors the demo's letterbox approach
static constexpr int VW = TacticalCamera::VIRTUAL_W;
static constexpr int VH = TacticalCamera::VIRTUAL_H;

// ── Pause Menu 레이아웃 상수 ────────────────────────────────────────────────
// PW/PH 두 줄만 바꾸면 모든 좌표가 자동 연동된다.
struct PauseLayout
{
	static constexpr double PW         = VW * 0.5;                     // 팝업 폭 (800)
	static constexpr double PH         = VH * 0.5;                     // 팝업 높이 (450)
	static constexpr double CX         = VW * 0.5;                     // 팝업 중심 X (800)
	static constexpr double CY         = VH * 0.5;                     // 팝업 중심 Y (450)
	static constexpr double LEFT       = CX - PW * 0.5;
	static constexpr double RIGHT      = CX + PW * 0.5;
	static constexpr double BOTTOM     = CY - PH * 0.5;
	static constexpr double TOP        = CY + PH * 0.5;

	static constexpr double LABEL_X    = LEFT + 40;
	static constexpr double SLIDER_X   = LEFT + 220;
	static constexpr double SLIDER_W   = PW - 350;
	static constexpr double SLIDER_H   = 15.0;
	static constexpr double BGM_Y      = CY + 130;
	static constexpr double SFX_Y      = CY + 40;
	static constexpr double PCT_X      = SLIDER_X + SLIDER_W + 30;

	static constexpr double BTN_W      = 200.0;
	static constexpr double BTN_H      = 50.0;
	static constexpr double BTN_GAP    = 20.0;
	static constexpr double BTN_Y      = BOTTOM + 90;
	static constexpr double BTN_START_X = CX - (3 * BTN_W + 2 * BTN_GAP) * 0.5;
};

static Math::vec2 to_virtual(Math::vec2 actual, Math::ivec2 actual_win) noexcept
{
    double scale = std::min(static_cast<double>(actual_win.x) / VW, static_cast<double>(actual_win.y) / VH);
    double ox    = (actual_win.x - VW * scale) * 0.5;
    double oy    = (actual_win.y - VH * scale) * 0.5;
    return { (actual.x - ox) / scale, (actual.y - oy) / scale };
}

void GamePlayUIManager::SetCamera(const TacticalCamera* camera)
{
    m_camera_ = camera;
}

void GamePlayUIManager::ShowDamageText(int damage, Math::vec2 position, Math::vec2 size, double delay)
{
  m_damage_texts.push_back({ std::to_string(damage), position, size, 0.5, delay });
}

void GamePlayUIManager::ShowGameEnd(std::string&& text)
{
  game_end_text = std::make_unique<std::string>(text);
}

void GamePlayUIManager::TogglePauseMenu()
{
	m_pause_open_     = !m_pause_open_;
	m_quit_requested_ = false;
	m_pause_drag_     = PauseDrag::None;
}

void GamePlayUIManager::Update(double dt)
{
    Math::vec2 mouse_pos   = Engine::GetInput().GetMousePos();
    bool       mouse_click = Engine::GetInput().MouseJustPressed(0);

    // 실제 화면 마우스를 가상 1600x900 좌표로 변환
    auto actual_win       = Engine::GetWindow().GetSize();
    Math::vec2 virt_mouse = to_virtual(mouse_pos, actual_win);
    m_virtual_mouse_      = virt_mouse;

    // 팝업이 열려있으면 팝업 입력만 처리하고 나머지 UI 입력 차단
    if (m_pause_open_)
    {
        UpdatePauseMenu(virt_mouse, mouse_click, Engine::GetInput().MouseDown(0));
        return;
    }

    // Feature 3: Cancel hint alpha pulse timer
    m_cancel_hint_time_ += dt;


    // 매 프레임 팝업창 슬롯 부족 호버 텍스트 초기화
    popup_hover_reason_ = "";

    // ── 1. 슬롯 비활성화 갱신 ─────────────────────────────────────
    auto* turnMgr = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
    if (turnMgr)
    {
        Character* current = turnMgr->GetCurrentCharacter();
        if (current)
        {
            SpellSlots* slots  = current->GetSpellSlots();
            bool        no_ap  = (current->GetActionPoints() == 0);
            bool        is_ai  = current->IsAIControlled();

            // 기본 비활성화/사유 세팅 헬퍼 함수
            auto set_disabled = [&](const std::string& id, bool disabled, const std::string& reason) {
                button_manager_.SetDisabled(id, disabled);
                button_manager_.SetDisableReason(id, disabled ? reason : "");
            };

            set_disabled("slot_attack", no_ap || is_ai, is_ai ? "Enemy's turn." : "No AP.");
            bool effects_pending = std::any_of(m_damage_texts.begin(), m_damage_texts.end(),
                [](const DamageText& t) { return t.delay > 0.0; });
            set_disabled("slot_end_turn",
                is_ai || game_end_text != nullptr || effects_pending,
                is_ai ? "Enemy's turn." : "");

            // 요구 AP를 파라미터로 추가 (기본값 1)
            auto spell_disabled = [&](const std::string& id, int min_lv, bool is_fixed, int req_ap = 1) {
                bool has_any_slot = false;
                if (slots) {
                    for (int lv = min_lv; lv <= 5; ++lv) {
                        if (slots->HasSlot(lv)) {
                            has_any_slot = true;
                            break;
                        }
                    }
                }
                
                // 현재 AP가 요구 AP보다 적은지 확인
                bool not_enough_ap = (current->GetActionPoints() < req_ap);
                bool disabled = is_ai || not_enough_ap || (is_fixed && !has_any_slot);

                std::string reason = "";
                if (disabled) {
                    if (is_ai) reason = "Enemy's turn.";
                    else if (not_enough_ap) reason = "Not enough AP.";
                    else if (is_fixed && !has_any_slot) reason = "No Spell slot.";
                }
                set_disabled(id, disabled, reason);
            };

            spell_disabled("slot_S_ATK_010", 1, false);
            spell_disabled("slot_S_ATK_020", 2, true);  // 고정
            spell_disabled("slot_S_ATK_030", 3, false); 
            spell_disabled("slot_S_ATK_040", 3, false, 3);
            spell_disabled("slot_S_ENH_040", 1, false);
            spell_disabled("slot_S_ENH_050", 1, true);  // 고정
            spell_disabled("slot_S_DEB_020", 1, true);  // 고정
            spell_disabled("slot_S_GEO_010", 2, false);
            spell_disabled("slot_S_GEO_020", 1, false);
        }
    }

    // ── 2. 업캐스트 팝업 클릭 및 호버 처리 (button_manager_.Update 보다 먼저) ──
    if (popup_open_)
    {
        int lv_min = 1, lv_max = 5;
        if      (popup_spell_id_ == "S_ATK_010") { lv_min = 1; lv_max = 5; }
        else if (popup_spell_id_ == "S_ATK_030") { lv_min = 3; lv_max = 5; }
        else if (popup_spell_id_ == "S_ATK_040") { lv_min = 3; lv_max = 5; }
        else if (popup_spell_id_ == "S_ENH_040") { lv_min = 1; lv_max = 4; }
        else if (popup_spell_id_ == "S_GEO_010") { lv_min = 2; lv_max = 5; }
        else if (popup_spell_id_ == "S_GEO_020") { lv_min = 1; lv_max = 5; }

        int    num_levels = lv_max - lv_min + 1;
        constexpr double BTN_W = 48.0;
        constexpr double BTN_H = 36.0;
        constexpr double GAP   = 4.0;

        double bar_top      = slot_bar_center_y_ + 32.0;
        double popup_bottom = bar_top + BTN_H + 8.0;
        double start_x      = slot_bar_x_[static_cast<size_t>(popup_slot_index_)] + 32.0
                              - (num_levels * BTN_W + (num_levels - 1) * GAP) * 0.5;

        bool hit = false;
        for (int i = 0; i < num_levels; ++i)
        {
            int    lv = lv_min + i;
            double bx = start_x + i * (BTN_W + GAP);
            double by = popup_bottom; 

            // 호버링 판정
            if (virt_mouse.x >= bx && virt_mouse.x <= bx + BTN_W &&
                virt_mouse.y >= by - BTN_H && virt_mouse.y <= by)
            {
                SpellSlots* slots = nullptr;
                if (turnMgr)
                {
                    Character* c = turnMgr->GetCurrentCharacter();
                    if (c) slots = c->GetSpellSlots();
                }
                
                // 팝업 슬롯 부족 툴팁용 사유 저장
                if (slots && !slots->HasSlot(lv)) {
                    popup_hover_reason_ = "No Spell slot.";
                    popup_hover_pos_    = Math::vec2{ bx + BTN_W * 0.5, by + 10.0 };
                }

                if (mouse_click) {
                    hit = true;
                    if (!slots || slots->HasSlot(lv))
                    {
                        if (m_input_handler_ptr_ && turnMgr)
                        {
                            Character* caster = turnMgr->GetCurrentCharacter();
                            if (caster)
                                m_input_handler_ptr_->SelectSpell(
                                    popup_spell_id_, caster, lv, button_manager_);
                        }
                    }
                    popup_open_ = false;
                }
            }
        }

        // 팝업 바깥을 누르면 닫힘
        if (mouse_click && !hit)
            popup_open_ = false;
    }

    // ── 3. 버튼 업데이트 ─────────────────────────────────────────
    button_manager_.Update(virt_mouse, mouse_click);

    // ── 3b. 스펠 슬롯 호버 감지 ──────────────────────────────────
    {
        static constexpr std::array<const char*, 9> SPELL_IDS = {
            "S_ATK_010","S_ATK_020","S_ATK_030","S_ATK_040",
            "S_ENH_040","S_ENH_050","S_DEB_020","S_GEO_010","S_GEO_020"
        };
        hovered_spell_id_.clear();
        for (size_t i = 0; i < 9; ++i)
        {
            if (button_manager_.IsHovered(std::string("slot_") + SPELL_IDS[i]))
            {
                hovered_spell_id_ = SPELL_IDS[i];
                hovered_slot_cx_  = slot_bar_x_[i + 1] + 32.0;
                break;
            }
        }

        hovered_attack_slot_ = false;
        if (hovered_spell_id_.empty() && button_manager_.IsHovered("slot_attack"))
        {
            hovered_attack_slot_ = true;
            hovered_slot_cx_     = slot_bar_x_[0] + 32.0;
        }
    }

    // ── 3c. 상태이상 아이콘 호버 감지 ─────────────────────────────
    hovered_effect_name_.clear();
    hovered_effect_duration_ = 0;
    {
        constexpr double ICON_S    = 32.0;
        constexpr double PANEL_TOP = 532.0;
        constexpr double PANEL_BOT = 168.0;

        // Dragon is now displayed in DrawDragonHUD — exclude from this panel
        int n_rows = 0;
        for (Character* ch : m_characters)
            if (ch && ch->GetCharacterType() != CharacterTypes::Dragon)
                ++n_rows;

        double avail_h  = PANEL_TOP - PANEL_BOT;
        int    reserve  = std::max(n_rows, 4);
        double PORT_H   = std::min((avail_h - (reserve - 1) * 4.0 - 8.0) / reserve, 128.0);
        double ICON_X0  = 8.0 + PORT_H + 4.0;
        double ROW_STEP = PORT_H + 4.0;

        double pan_top = PANEL_TOP;
        double row_bot = pan_top - 4.0 - PORT_H;

        for (Character* ch : m_characters)
        {
            if (!ch || ch->GetCharacterType() == CharacterTypes::Dragon) continue;

            double icon_y = row_bot + (PORT_H - ICON_S) * 0.5;
            const auto& effects = ch->GetActiveEffects();
            for (size_t ei = 0; ei < effects.size(); ++ei)
            {
                double icon_x = ICON_X0 + static_cast<double>(ei) * ICON_S;
                if (virt_mouse.x >= icon_x && virt_mouse.x < icon_x + ICON_S &&
                    virt_mouse.y >= icon_y && virt_mouse.y < icon_y + ICON_S)
                {
                    hovered_effect_name_     = effects[ei].name;
                    hovered_effect_duration_ = effects[ei].duration;
                    break;
                }
            }
            if (!hovered_effect_name_.empty()) break;
            row_bot -= ROW_STEP;
        }

        // Dragon HUD 상태이상 아이콘 호버 감지
        if (hovered_effect_name_.empty() && m_player_)
        {
            constexpr double PAN_BOT_D = static_cast<double>(VH) - 8.0 - 352.0; // 540
            constexpr double ICON_Y_D  = PAN_BOT_D + 10.0;                       // 550

            const auto& dragon_effects = m_player_->GetActiveEffects();
            for (size_t ei = 0; ei < dragon_effects.size(); ++ei)
            {
                double icon_x = 8.0 + 16.0 + static_cast<double>(ei) * (ICON_S + 4.0);
                if (virt_mouse.x >= icon_x && virt_mouse.x < icon_x + ICON_S &&
                    virt_mouse.y >= ICON_Y_D  && virt_mouse.y < ICON_Y_D + ICON_S)
                {
                    hovered_effect_name_     = dragon_effects[ei].name;
                    hovered_effect_duration_ = dragon_effects[ei].duration;
                    break;
                }
            }
        }
    }

    // ── 4. 호버 캐릭터 감지 ──────────────────────────────────────
    hovered_character_ = nullptr;
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (grid)
    {
        Math::vec2 world_pos = mouse_pos;
        if (m_camera_)
        {
            world_pos = m_camera_->ScreenToWorld(mouse_pos, actual_win);
        }

        int grid_x = static_cast<int>(std::floor(world_pos.x / GridSystem::TILE_SIZE));
        int grid_y = static_cast<int>(std::floor(world_pos.y / GridSystem::TILE_SIZE));

        for (Character* ch : m_characters)
        {
            if (ch && ch->IsAlive())
            {
                auto* pos_comp = ch->GetGridPosition();
                if (pos_comp && pos_comp->Get() == Math::ivec2{ grid_x, grid_y })
                {
                    hovered_character_ = ch;
                    break;
                }
            }
        }
    }

    // ── 4b. 배틀 로그 패널 드래그 스크롤 ─────────────────────────
    if (show_battle_log_)
    {
        bool mouse_down = Engine::GetInput().MouseDown(0);

        if (mouse_click && IsMouseOverLogPanel())
        {
            log_drag_active_ = true;
            log_drag_prev_y_ = virt_mouse.y;
        }

        if (log_drag_active_)
        {
            if (mouse_down)
            {
                double delta_y    = virt_mouse.y - log_drag_prev_y_;
                double visible_h  = LOG_PANEL_H - LOG_TITLE_H;
                double max_scroll = std::max(0.0, ComputeLogContentHeight() - visible_h);
                log_scroll_offset_ = std::clamp(log_scroll_offset_ - delta_y, 0.0, max_scroll);
                log_drag_prev_y_   = virt_mouse.y;
            }
            else
            {
                log_drag_active_ = false;
            }
        }
    }
    else
    {
        log_drag_active_ = false;
    }

    // ── 5. 데미지 텍스트 수명 감소 및 만료 제거 ───────────────────
    if (m_notice_timer_ > 0.0)
        m_notice_timer_ -= dt;

    for (auto& text : m_damage_texts)
    {
        if (text.delay > 0.0)
            text.delay -= dt;
        else
            text.lifetime -= dt;
    }
    m_damage_texts.erase(
        std::remove_if(m_damage_texts.begin(), m_damage_texts.end(),
                       [](const DamageText& t) { return t.delay <= 0.0 && t.lifetime <= 0.0; }),
        m_damage_texts.end());
}

void GamePlayUIManager::Draw([[maybe_unused]] Math::TransformationMatrix camera_matrix)
{
    button_manager_.Draw(camera_matrix);
    DrawSlotBar();
    DrawUicastPopup();
    DrawActionLabel();
    DrawTurnIndicator();
    DrawCancelHint();
    DrawHoverTooltip();
    DrawSpellTooltip();
    DrawAttackTooltip();
    DrawDragonHUD();
    DrawStatusEffectPanel();
    DrawStatusEffectTooltip();
    DrawBattleLog();
    DrawNotice();
    DrawDisableReasonTooltip();
    DrawDragonWorldHoverTooltip();

    auto& textMng = Engine::GetTextManager();

    for (const auto& text : m_damage_texts)
    {
        if (text.delay > 0.0) continue;
        Math::vec2 screen_pos = text.position;
        if (m_camera_)
            screen_pos = m_camera_->WorldToScreen(text.position, Engine::GetWindow().GetSize());
        textMng.DrawText(text.text, screen_pos, Fonts::Kings, text.size, CS200::VIOLET);
    }

    if (game_end_text)
    {
        auto size = Engine::GetWindow().GetSize();
        textMng.DrawText(*game_end_text, Math::ivec2{ 0, size.y / 2 },
                         Fonts::Kings,
                         Math::vec2{ GAME_END_TEXT_SIZE, GAME_END_TEXT_SIZE },
                         CS200::WHITE);
    }

    DrawPauseMenu();
}

void GamePlayUIManager::SetCharacters(const std::vector<Character*>& characters)
{
  m_characters = characters;
  Engine::GetLogger().LogEvent("GamePlayUIManager: Tracking " + std::to_string(m_characters.size()) + " characters for stats display");

  auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
  if (bus)
  {
    bus->Subscribe<CharacterDeathEvent>([this](const CharacterDeathEvent& e) {
      for (auto& ptr : m_characters)
      {
        if (ptr == e.character)
        {
          ptr = nullptr;
          break;
        }
      }
      if (m_player_ == e.character)
      {
        m_player_ = nullptr;
      }
    });

    // [여기에 추가!] 텍스트 메시지 이벤트를 받으면 배틀 로그에 띄웁니다.
        bus->Subscribe<BattleLogMessageEvent>([this](const BattleLogMessageEvent& e) {
            this->AddBattleLogEntry(e.message);
        });
  }
}

void GamePlayUIManager::SetPlayer(Character* player)
{
  m_player_ = player;
}

void GamePlayUIManager::InitButtons(PlayerInputHandler* inputHandler)
{
    m_input_handler_ptr_ = inputHandler;
    button_manager_.ClearAll();

    const Math::ivec2 win = { VW, VH };
    constexpr int    TILE = 64;
    const double box_x    = TILE / 2;
    const double bar_bot  = TILE;
    const double bar_h    = TILE * 1.5;
    const double bar_w    = static_cast<double>(win.x) - TILE;
    constexpr int N       = 10;
    const double end_btn_w = static_cast<double>(TILE);
    const double remaining = bar_w - N * TILE - end_btn_w;
    const double offset    = remaining / (N + 2);

    slot_bar_center_y_ = bar_bot + bar_h * 0.5; // = 64 + 48 = 112

    for (size_t i = 0; i < static_cast<size_t>(N); ++i)
        slot_bar_x_[i] = box_x + offset + static_cast<double>(i) * (TILE + offset);
    slot_bar_x_[static_cast<size_t>(N)] = slot_bar_x_[static_cast<size_t>(N) - 1] + TILE + offset;

    // 아이콘 텍스처 로드
    const std::array<std::string, 10> ICON_PATHS = {
        "Assets/images/dragon_attack.png",
        "Assets/images/dragon_firebolt.png",
        "Assets/images/dragon_tail_swipe.png",
        "Assets/images/dragon_fury.png",
        "Assets/images/dragon_meteor.png",
        "Assets/images/dragon_mana_conversion.png",
        "Assets/images/dragon_purify.png",
        "Assets/images/dragon_fearful_cry.png",
        "Assets/images/dragon_magma_blast.png",
        "Assets/images/dragon_wall_creation.png",
    };
    slot_icons_.resize(11, nullptr);
    for (size_t i = 0; i < 10; ++i)
        slot_icons_[i] = Engine::GetTextureManager().Load(ICON_PATHS[i]);
    slot_icons_[10] = Engine::GetTextureManager().Load("Assets/images/turn_end.png");

    // 슬롯 ID 및 스펠 ID 매핑
    const std::array<std::string, 10> SLOT_IDS = {
        "slot_attack",
        "slot_S_ATK_010",
        "slot_S_ATK_020",
        "slot_S_ATK_030",
        "slot_S_ATK_040",
        "slot_S_ENH_040",
        "slot_S_ENH_050",
        "slot_S_DEB_020",
        "slot_S_GEO_010",
        "slot_S_GEO_020",
    };
    // 단일 레벨 스펠: (slot_index, spell_id, level)
    // 업캐스트 스펠: (slot_index, spell_id) — popup 처리

    auto get_current_char = [&]() -> Character* {
        auto* tm = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
        return tm ? tm->GetCurrentCharacter() : nullptr;
    };

    // slot 0: Attack
    {
        Button b;
        b.id       = "slot_attack";
        b.position = { slot_bar_x_[0], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [inputHandler]() { inputHandler->OnAttackPressed(); };
        button_manager_.AddButton(b);
    }

    // slot 1: Fire Bolt (upcast Lv1~5)
    {
        int idx = 1;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [this, idx, inputHandler]() {
            if (popup_open_ && popup_spell_id_ == "S_ATK_010") {
                popup_open_ = false;
            } else {
                inputHandler->CancelCurrentAction();
                popup_open_       = true;
                popup_spell_id_   = "S_ATK_010";
                popup_slot_index_ = idx;
            }
        };
        button_manager_.AddButton(b);
    }

    // slot 2: Tail Swipe (single Lv2)
    {
        int idx = 2;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [inputHandler, get_current_char, this]() {
            Character* c = get_current_char();
            if (c) inputHandler->SelectSpell("S_ATK_020", c, 2, button_manager_);
        };
        button_manager_.AddButton(b);
    }

    // slot 3: Dragon's Fury (upcast Lv3~5)
    {
        int idx = 3;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [this, idx, inputHandler]() {
            if (popup_open_ && popup_spell_id_ == "S_ATK_030") {
                popup_open_ = false;
            } else {
                inputHandler->CancelCurrentAction();
                popup_open_       = true;
                popup_spell_id_   = "S_ATK_030";
                popup_slot_index_ = idx;
            }
        };
        button_manager_.AddButton(b);
    }

    // slot 4: Meteor (upcast Lv3~5)
    {
        int idx = 4;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [this, idx, inputHandler]() {
            if (popup_open_ && popup_spell_id_ == "S_ATK_040") {
                popup_open_ = false;
            } else {
                inputHandler->CancelCurrentAction();
                popup_open_       = true;
                popup_spell_id_   = "S_ATK_040";
                popup_slot_index_ = idx;
            }
        };
        button_manager_.AddButton(b);
    }

    // slot 5: Mana Conversion (upcast Lv1~4)
    {
        int idx = 5;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [this, idx, inputHandler]() {
            if (popup_open_ && popup_spell_id_ == "S_ENH_040") {
                popup_open_ = false;
            } else {
                inputHandler->CancelCurrentAction();
                popup_open_       = true;
                popup_spell_id_   = "S_ENH_040";
                popup_slot_index_ = idx;
            }
        };
        button_manager_.AddButton(b);
    }

    // slot 6: Purify (single Lv1)
    {
        int idx = 6;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [inputHandler, get_current_char, this]() {
            Character* c = get_current_char();
            if (c) inputHandler->SelectSpell("S_ENH_050", c, 1, button_manager_);
        };
        button_manager_.AddButton(b);
    }

    // slot 7: Fearful Cry (single Lv1)
    {
        int idx = 7;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [inputHandler, get_current_char, this]() {
            Character* c = get_current_char();
            if (c) inputHandler->SelectSpell("S_DEB_020", c, 1, button_manager_);
        };
        button_manager_.AddButton(b);
    }

    // slot 8: Magma Blast (upcast Lv2~5)
    {
        int idx = 8;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [this, idx, inputHandler]() {
            if (popup_open_ && popup_spell_id_ == "S_GEO_010") {
                popup_open_ = false;
            } else {
                inputHandler->CancelCurrentAction();
                popup_open_       = true;
                popup_spell_id_   = "S_GEO_010";
                popup_slot_index_ = idx;
            }
        };
        button_manager_.AddButton(b);
    }

    // slot 9: Wall Creation (upcast Lv1~5)
    {
        int idx = 9;
        Button b;
        b.id       = SLOT_IDS[static_cast<size_t>(idx)];
        b.position = { slot_bar_x_[static_cast<size_t>(idx)], slot_bar_center_y_ + TILE * 0.5 };
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = [this, idx, inputHandler]() {
            if (popup_open_ && popup_spell_id_ == "S_GEO_020") {
                popup_open_ = false;
            } else {
                inputHandler->CancelCurrentAction();
                popup_open_       = true;
                popup_spell_id_   = "S_GEO_020";
                popup_slot_index_ = idx;
            }
        };
        button_manager_.AddButton(b);
    }

    // End Turn 버튼
    {
        Button end;
        end.id         = "slot_end_turn";
        // 레이아웃 위치는 전혀 건드리지 않고 그대로 유지합니다.
        end.position   = { slot_bar_x_[static_cast<size_t>(N)], slot_bar_center_y_ + TILE * 0.5 };
        
        // [수정된 부분] 히트박스의 크기(size)를 하드코딩하지 않고, 실제 에셋 이미지의 해상도로 딱 맞게 동기화합니다.
        if (slot_icons_[10] != nullptr)
        {
            end.size = { static_cast<double>(slot_icons_[10]->GetSize().x),
                         static_cast<double>(slot_icons_[10]->GetSize().y) };
        }
        else
        {
            end.size = { end_btn_w, static_cast<double>(TILE) };
        }
        
        end.label      = "";
        end.on_click   = [inputHandler]() { inputHandler->OnEndTurnPressed(); };
        button_manager_.AddButton(end);
    }

    // Wall/Lava 확인 버튼 (처음엔 숨김)
    {
        Button wall_btn;
        wall_btn.id       = "slot_wall_confirm";
        wall_btn.position = { static_cast<double>(win.x) * 0.5 - TILE, slot_bar_center_y_ + TILE * 0.5 + TILE + 12.0 };
        wall_btn.size     = { static_cast<double>(TILE * 2), 40.0 };
        wall_btn.label    = "Confirm";
        wall_btn.visible  = false;
        button_manager_.AddButton(wall_btn);
    }

    // Battle Log 토글 버튼
    {
        Button log_btn;
        log_btn.id       = "btn_battle_log";
        log_btn.position = { static_cast<double>(win.x) - 64.0,
                             static_cast<double>(win.y) * 0.5 + 32.0 };
        log_btn.size     = { 64.0, 64.0 };
        log_btn.label    = "<";
        log_btn.on_click = [this]() {
            show_battle_log_ = !show_battle_log_;
            button_manager_.SetLabel("btn_battle_log", show_battle_log_ ? ">" : "<");
        };
        button_manager_.AddButton(log_btn);
    }
}

ButtonManager& GamePlayUIManager::GetButtons()
{
  return button_manager_;
}

void GamePlayUIManager::InitSpellTooltips()
{
    static constexpr std::array<const char*, 9> SPELL_IDS = {
        "S_ATK_010", "S_ATK_020", "S_ATK_030", "S_ATK_040",
        "S_ENH_040", "S_ENH_050", "S_DEB_020", "S_GEO_010", "S_GEO_020"
    };

    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    if (!spell_sys) return;

    for (const char* id : SPELL_IDS)
    {
        const SpellData* data = spell_sys->GetSpellData(id);
        if (!data) continue;

        std::vector<std::string> lines;
        lines.push_back(data->spell_name + " [Lv " + std::to_string(data->spell_level) + "]");

        std::istringstream ss(data->effect_raw);
        std::string line;
        while (std::getline(ss, line))
        {
            if (line.empty()) continue;
            if (line.find("\"Basic\"") != std::string::npos) continue;
            if (line == "Move to current location.") continue;
            if (line.find("Summons NULL") != std::string::npos) continue;
            if (line == "Deals 0 damage.") continue;
            lines.push_back(line);
        }

        spell_tooltip_cache_[id] = std::move(lines);

        // 각 줄의 실제 화면 폭(scale 적용) 중 최댓값 → 박스 폭 사전 계산
        constexpr double PAD = 10.0;
        auto& textMgr2 = Engine::GetTextManager();
        double max_text_w = 0.0;
        const auto& cached = spell_tooltip_cache_[id];
        if (!cached.empty())
            max_text_w = textMgr2.CalculateTextSize(cached[0], Fonts::Kings).x * 0.5;
        for (size_t li = 1; li < cached.size(); ++li)
            max_text_w = std::max(max_text_w,
                textMgr2.CalculateTextSize(cached[li], Fonts::Kings).x * 0.4);
        spell_tooltip_widths_[id] = std::min(max_text_w + PAD * 2.0,
                                             static_cast<double>(VW) - 20.0);
    }
}

void GamePlayUIManager::OnTurnStarted(const std::string& actor_name, int turn_number, bool is_player, int round_number)
{
  // 새 항목 추가 전에 "최신 로그를 보고 있었는가" 판정
  double visible_h      = LOG_PANEL_H - LOG_TITLE_H;
  double old_max_scroll = std::max(0.0, ComputeLogContentHeight() - visible_h);
  bool   was_at_bottom  = (log_scroll_offset_ >= old_max_scroll - LOG_LINE_H);

  turn_history_.push_back({ round_number, turn_number, actor_name, is_player, {} });

  // 5라운드 초과분 제거
  int oldest_allowed = round_number - MAX_LOG_ROUNDS + 1;
  while (!turn_history_.empty() && turn_history_.front().round_number < oldest_allowed)
    turn_history_.pop_front();

  // 마우스가 패널 위에 있고 이전 로그를 탐색 중이면 자동 스크롤 안 함
  if (IsMouseOverLogPanel() && !was_at_bottom)
    return;

  double new_max_scroll = std::max(0.0, ComputeLogContentHeight() - visible_h);
  log_scroll_offset_    = new_max_scroll;
}

double GamePlayUIManager::ComputeLogContentHeight() const
{
  double h              = 0.0;
  int    prev_round     = -1;
  bool   prev_player    = true;
  bool   first_in_round = true;

  for (const auto& entry : turn_history_)
  {
    if (entry.round_number != prev_round)
    {
      h += LOG_LINE_H;  // ─── Round N ─── 헤더
      prev_round     = entry.round_number;
      first_in_round = true;
    }
    if (first_in_round || entry.is_player != prev_player)
    {
      h += LOG_LINE_H;  // ▷ Player Turn / ▷ Enemy Turn 헤더
      prev_player    = entry.is_player;
      first_in_round = false;
    }
    h += LOG_LINE_H;   // 캐릭터 이름
    h += static_cast<double>(entry.lines.size()) * LOG_LINE_H;
    h += 4.0;           // 캐릭터 간 여백
  }
  return h;
}

bool GamePlayUIManager::IsMouseOverLogPanel() const
{
  if (!show_battle_log_) return false;
  return m_virtual_mouse_.x >= LOG_PANEL_X && m_virtual_mouse_.x <= LOG_PANEL_X + LOG_PANEL_W
      && m_virtual_mouse_.y <= LOG_PANEL_Y && m_virtual_mouse_.y >= LOG_PANEL_Y - LOG_PANEL_H;
}

void GamePlayUIManager::ScrollLog(double delta)
{
  double visible_h  = LOG_PANEL_H - LOG_TITLE_H;
  double max_scroll = std::max(0.0, ComputeLogContentHeight() - visible_h);
  log_scroll_offset_ = std::clamp(log_scroll_offset_ - delta * 30.0, 0.0, max_scroll);
}

void GamePlayUIManager::AddBattleLogEntry(const std::string& line)
{
  if (!turn_history_.empty())
    turn_history_.back().lines.push_back(line);
}

// ─── 슬롯 바 배경 + 아이콘 오버레이 ─────────────────────────────────
void GamePlayUIManager::DrawSlotBar()
{
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    constexpr Math::ivec2 win = { VW, VH };
    constexpr double TILE = 64.0;

    double bar_w = static_cast<double>(win.x) - TILE;
    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ win.x * 0.5, slot_bar_center_y_ }) *
        Math::ScaleMatrix(Math::vec2{ bar_w, TILE * 1.5 });
    renderer->DrawRectangle(bg, 0x1a1a2e99, 0x5555aaff, 1.5, DrawDepth::UI + 0.001f);

    // 아이콘은 버튼(UI=0.01f)보다 앞에 와야 보임 → UI - 0.005f = 0.005f
    for (size_t i = 0; i < slot_icons_.size(); ++i)
    {
        if (!slot_icons_[i]) continue;
        slot_icons_[i]->Draw(
            Math::TranslationMatrix(Math::vec2{ slot_bar_x_[i], slot_bar_center_y_ - 32.0 }),
            0xFFFFFFFF,
            DrawDepth::UI - 0.005f);
    }
}

// ─── 업캐스트 레벨 선택 팝업 ─────────────────────────────────────────
void GamePlayUIManager::DrawUicastPopup()
{
    if (!popup_open_) return;

    int lv_min = 1, lv_max = 5;
    if      (popup_spell_id_ == "S_ATK_010") { lv_min = 1; lv_max = 5; }
    else if (popup_spell_id_ == "S_ATK_030") { lv_min = 3; lv_max = 5; }
    else if (popup_spell_id_ == "S_ATK_040") { lv_min = 3; lv_max = 5; }
    else if (popup_spell_id_ == "S_ENH_040") { lv_min = 1; lv_max = 4; }
    else if (popup_spell_id_ == "S_GEO_010") { lv_min = 2; lv_max = 5; }
    else if (popup_spell_id_ == "S_GEO_020") { lv_min = 1; lv_max = 5; }

    int    num_levels = lv_max - lv_min + 1;
    constexpr double BTN_W = 48.0;
    constexpr double BTN_H = 36.0;
    constexpr double GAP   = 4.0;

    double bar_top      = slot_bar_center_y_ + 32.0;
    double popup_bottom = bar_top + BTN_H + 8.0;  // button top is fully above slot bar

    double start_x = slot_bar_x_[static_cast<size_t>(popup_slot_index_)] + 32.0
                     - (num_levels * BTN_W + (num_levels - 1) * GAP) * 0.5;

    // 배경 패널
    double panel_w  = num_levels * BTN_W + (num_levels - 1) * GAP + 8.0;
    double panel_cx = slot_bar_x_[static_cast<size_t>(popup_slot_index_)] + 32.0;
    double panel_cy = popup_bottom - BTN_H * 0.5;  // centered on buttons

    auto* renderer = CS230::TextureManager::GetRenderer2D();
    auto& textMgr  = Engine::GetTextManager();
    Math::vec2 mouse = m_virtual_mouse_;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ panel_cx, panel_cy }) *
        Math::ScaleMatrix(Math::vec2{ panel_w, BTN_H + 8.0 });
    renderer->DrawRectangle(bg, 0x1a1a2ecc, 0x5555aaff, 1.0, DrawDepth::UI + 0.001f);

    SpellSlots* slots = nullptr;
    auto* turnMgr = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
    if (turnMgr)
    {
        Character* c = turnMgr->GetCurrentCharacter();
        if (c) slots = c->GetSpellSlots();
    }

    for (int i = 0; i < num_levels; ++i)
    {
        int    lv = lv_min + i;
        double bx = start_x + i * (BTN_W + GAP);
        double by = popup_bottom;

        bool hover    = (mouse.x >= bx && mouse.x <= bx + BTN_W &&
                         mouse.y >= by - BTN_H && mouse.y <= by);
        bool disabled = (slots && !slots->HasSlot(lv));

        uint32_t fill   = disabled ? 0x444444cc : (hover ? 0x6666aaff : 0x3333aacc);
        uint32_t border = 0x8888ddff;

        Math::TransformationMatrix btn_t =
            Math::TranslationMatrix(Math::vec2{ bx + BTN_W * 0.5, by - BTN_H * 0.5 }) *
            Math::ScaleMatrix(Math::vec2{ BTN_W, BTN_H });
        renderer->DrawRectangle(btn_t, fill, border, 1.0, DrawDepth::UI - 0.002f);

        textMgr.DrawText("Lv" + std::to_string(lv),
            Math::vec2{ bx + 6.0, by - 6.0 },
            Fonts::Kings, { 0.35, 0.35 },
            disabled ? CS200::WHITE : CS200::GOLD,
            DrawDepth::UI - 0.003f);
    }
}

// ─── 상단 중앙 턴 인디케이터 ─────────────────────────────────────────
void GamePlayUIManager::DrawTurnIndicator()
{
    auto* turnMgr = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
    if (!turnMgr) return;

    Character* current = turnMgr->GetCurrentCharacter();
    if (!current) return;

    auto* renderer = CS230::TextureManager::GetRenderer2D();
    auto& textMgr  = Engine::GetTextManager();
    constexpr Math::ivec2 win = { VW, VH };

    constexpr double PANEL_W = 320.0;
    constexpr double PANEL_H = 48.0;

    double panel_cx  = win.x * 0.5;
    double panel_top = static_cast<double>(win.y) - 8.0;
    double panel_cy  = panel_top - PANEL_H * 0.5;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ panel_cx, panel_cy }) *
        Math::ScaleMatrix(Math::vec2{ PANEL_W, PANEL_H });
    renderer->DrawRectangle(bg, 0x1a1a2ecc, 0x5555aaff, 1.5, DrawDepth::UI + 0.001f);

    std::string main_text = current->TypeName() + "'s Turn";
    textMgr.DrawText(main_text,
        Math::vec2{ panel_cx - PANEL_W * 0.5 + 8.0, panel_cy - 8.0 },
        Fonts::Kings, { 0.45, 0.45 }, CS200::GOLD, DrawDepth::UI);
}

// ─── 마우스 오버 스탯 툴팁 ──────────────────────────────────────────
void GamePlayUIManager::DrawHoverTooltip()
{
    if (!hovered_character_) return;

    auto& textMgr  = Engine::GetTextManager();
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    Math::vec2 mouse = m_virtual_mouse_;
    constexpr Math::ivec2 win = { VW, VH };

    constexpr double TT_W = 256.0;
    constexpr double TT_H = 200.0;
    constexpr double LH   = 26.0;

    double tip_x = mouse.x + 72.0;
    if (tip_x + TT_W > win.x) tip_x = mouse.x - TT_W - 8.0;
    double tip_top = mouse.y + TT_H * 0.5;
    double tip_cx  = tip_x + TT_W * 0.5;
    double tip_cy  = tip_top - TT_H * 0.5;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ tip_cx, tip_cy }) *
        Math::ScaleMatrix(Math::vec2{ TT_W, TT_H });
    renderer->DrawRectangle(bg, 0x1a1a2edd, 0x8888aaff, 1.0, DrawDepth::UI + 0.001f);

    double ty = tip_top - 8.0;

    textMgr.DrawText(hovered_character_->TypeName(),
        Math::vec2{ tip_x + 8.0, ty }, Fonts::Kings,
        { 0.5, 0.5 }, CS200::GOLD, DrawDepth::UI);
    ty -= LH;

    std::string hp_str = "HP: " + std::to_string(hovered_character_->GetHP())
                       + "/" + std::to_string(hovered_character_->GetMaxHP());
    textMgr.DrawText(hp_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Kings,
        { 0.4, 0.4 }, CS200::RED, DrawDepth::UI);
    ty -= LH;

    std::string ap_str = "AP: " + std::to_string(hovered_character_->GetActionPoints());
    textMgr.DrawText(ap_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Kings,
        { 0.4, 0.4 }, CS200::YELLOW, DrawDepth::UI);
    ty -= LH;

    std::string spd_str = "MOV: " + std::to_string(hovered_character_->GetMovementRange());
    textMgr.DrawText(spd_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Kings,
        { 0.4, 0.4 }, CS200::GREEN, DrawDepth::UI);
    ty -= LH;

    SpellSlots* slots = hovered_character_->GetSpellSlots();
    if (slots)
    {
        std::string slot_str = "Slots:";
        for (int lv = 1; lv <= 5; ++lv)
        {
            int max_c = slots->GetMaxSlotCount(lv);
            if (max_c == 0) continue;
            int cur_c = slots->GetSpellSlotCount(lv);
            slot_str += " L" + std::to_string(lv) + ":"
                      + std::to_string(cur_c) + "/" + std::to_string(max_c);
        }
        textMgr.DrawText(slot_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Kings,
            { 0.4, 0.4 }, CS200::ORANGE, DrawDepth::UI);
        ty -= LH;
    }

    const auto& effects = hovered_character_->GetActiveEffects();
    if (!effects.empty())
    {
        std::string fx_str = "FX:";
        for (const auto& e : effects)
            fx_str += " " + e.name + "(" + std::to_string(e.duration) + ")";
        textMgr.DrawText(fx_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Kings,
            { 0.4, 0.4 }, CS200::YELLOW, DrawDepth::UI);
    }
}

void GamePlayUIManager::DrawCharacterStatsPanel([[maybe_unused]] Math::TransformationMatrix camera_matrix)
{
  if (m_characters.empty()) return;

  Math::ivec2 window_size = Engine::GetWindow().GetSize();

  const double panel_x		 = static_cast<double>(window_size.x) - 650.0;
  const double panel_start_y = static_cast<double>(window_size.y) - 200.0;

  const double panel_height_per_char = 180.0;
  const double text_left_margin      = 20.0;
  const double line_height           = 30.0;
  const double first_line_y          = 20.0;
  const Math::vec2 text_scale        = Math::vec2{ 0.5, 0.5 };

  double current_y = panel_start_y;

  for (Character* character : m_characters)
  {
	if (character == nullptr) continue;
    
	double text_x_pos = panel_x + text_left_margin;

	Engine::GetTextManager().DrawText(character->TypeName(),
	    Math::vec2{ text_x_pos + 40.0, current_y + panel_height_per_char - first_line_y },
	    Fonts::Kings, text_scale, CS200::WHITE);

	std::string hp_text = "HP: " + std::to_string(character->GetHP())
	                    + " / " + std::to_string(character->GetMaxHP());
	Engine::GetTextManager().DrawText(hp_text,
	    Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 1.0) },
	    Fonts::Kings, text_scale, CS200::RED);

	std::string ap_text = "AP: " + std::to_string(character->GetActionPoints());
	Engine::GetTextManager().DrawText(ap_text,
	    Math::vec2{ text_x_pos + 50.0, current_y + panel_height_per_char - (first_line_y + line_height * 2.0) },
	    Fonts::Kings, text_scale, CS200::YELLOW);

	std::string speed_text = "MOV: " + std::to_string(character->GetMovementRange());
	Engine::GetTextManager().DrawText(speed_text,
	    Math::vec2{ text_x_pos + 30.0, current_y + panel_height_per_char - (first_line_y + line_height * 3.0) },
	    Fonts::Kings, text_scale, CS200::GREEN);

	SpellSlots* slots = character->GetSpellSlots();
	if (slots)
	{
	  std::string slot_text = "Slots:";
	  for (int lv = 1; lv <= 5; ++lv)
	  {
		int max_count = slots->GetMaxSlotCount(lv);
		if (max_count == 0) continue;
		int cur_count = slots->GetSpellSlotCount(lv);
		slot_text += " Lv" + std::to_string(lv) + ":" + std::to_string(cur_count) + "/" + std::to_string(max_count);
	  }
	  Engine::GetTextManager().DrawText(slot_text,
	      Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 4.0) },
	      Fonts::Kings, text_scale, CS200::ORANGE);
	}

	const auto& effects = character->GetActiveEffects();
	std::string fx_text = "FX:";
	for (const auto& e : effects)
	  fx_text += " " + e.name + "(" + std::to_string(e.duration) + ")";
	Engine::GetTextManager().DrawText(fx_text,
	    Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 5.0) },
	    Fonts::Kings, text_scale, CS200::YELLOW);

	current_y -= panel_height_per_char + 40.0;
  }
}

void GamePlayUIManager::DrawSpellTooltip()
{
    if (hovered_spell_id_.empty()) return;
    if (popup_open_) return;

    auto it = spell_tooltip_cache_.find(hovered_spell_id_);
    if (it == spell_tooltip_cache_.end() || it->second.empty()) return;

    const auto& lines = it->second;
    auto& textMgr  = Engine::GetTextManager();
    auto* renderer = CS230::TextureManager::GetRenderer2D();

    auto wit = spell_tooltip_widths_.find(hovered_spell_id_);
    double TT_W = (wit != spell_tooltip_widths_.end()) ? wit->second : 380.0;
    constexpr double LH   = 24.0;
    constexpr double PAD  = 10.0;
    double TT_H = PAD * 2.0 + static_cast<double>(lines.size()) * LH;

    double tip_x = hovered_slot_cx_ - TT_W * 0.5;
    if (tip_x < 4.0) tip_x = 4.0;
    if (tip_x + TT_W > static_cast<double>(VW) - 4.0)
        tip_x = static_cast<double>(VW) - TT_W - 4.0;

    // 슬롯 바 아이콘 상단(center + 32 = 144) 기준으로 위쪽에 배치
    double tip_top = slot_bar_center_y_ + 32.0 + 8.0 + TT_H;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ tip_x + TT_W * 0.5, tip_top - TT_H * 0.5 }) *
        Math::ScaleMatrix(Math::vec2{ TT_W, TT_H });
    renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5, DrawDepth::UI + 0.001f);

    double ty = tip_top - PAD - 22;

    // 첫 줄: 스펠 이름 + 레벨 (금색)
    textMgr.DrawText(lines[0], Math::vec2{ tip_x + PAD, ty },
        Fonts::Kings, { 0.5, 0.5 }, CS200::GOLD, DrawDepth::UI);
    ty -= LH;

    // 이후 줄: 효과 설명 (흰색)
    for (size_t i = 1; i < lines.size(); ++i)
    {
        textMgr.DrawText(lines[i], Math::vec2{ tip_x + PAD, ty },
            Fonts::Kings, { 0.4, 0.4 }, CS200::WHITE, DrawDepth::UI);
        ty -= LH;
    }
}

void GamePlayUIManager::DrawAttackTooltip()
{
    if (!hovered_attack_slot_ || !m_player_) return;

    std::string dice = "?";
    if (auto* st = m_player_->GetStatsComponent())
        dice = st->GetAttackDice();

    const std::vector<std::string> lines = {
        "Basic Attack",
        "Deals " + dice + " damage",
        "Costs 1 AP"
    };

    auto& textMgr  = Engine::GetTextManager();
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    constexpr double PAD = 10.0;
    constexpr double LH  = 24.0;

    double max_w = textMgr.CalculateTextSize(lines[0], Fonts::Kings).x * 0.5;
    for (size_t i = 1; i < lines.size(); ++i)
        max_w = std::max(max_w, textMgr.CalculateTextSize(lines[i], Fonts::Kings).x * 0.4);
    double TT_W = std::min(max_w + PAD * 2.0, static_cast<double>(VW) - 20.0);
    double TT_H = PAD * 2.0 + static_cast<double>(lines.size()) * LH;

    double tip_x = hovered_slot_cx_ - TT_W * 0.5;
    if (tip_x < 4.0) tip_x = 4.0;
    if (tip_x + TT_W > static_cast<double>(VW) - 4.0)
        tip_x = static_cast<double>(VW) - TT_W - 4.0;
    double tip_top = slot_bar_center_y_ + 32.0 + 8.0 + TT_H;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ tip_x + TT_W * 0.5, tip_top - TT_H * 0.5 }) *
        Math::ScaleMatrix(Math::vec2{ TT_W, TT_H });
    renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5, DrawDepth::UI + 0.001f);

    double ty = tip_top - PAD - 22;
    textMgr.DrawText(lines[0], Math::vec2{ tip_x + PAD, ty },
        Fonts::Kings, { 0.5, 0.5 }, CS200::GOLD, DrawDepth::UI);
    ty -= LH;
    for (size_t i = 1; i < lines.size(); ++i)
    {
        textMgr.DrawText(lines[i], Math::vec2{ tip_x + PAD, ty },
            Fonts::Kings, { 0.4, 0.4 }, CS200::WHITE, DrawDepth::UI);
        ty -= LH;
    }
}

void GamePlayUIManager::InitStatusEffectIcons()
{
    static const std::pair<const char*, const char*> ICONS[] = {
        {"Blessing",   "Assets/images/blessing.png"},
        {"Lifesteal",  "Assets/images/lifesteal.png"},
        {"Frenzy",     "Assets/images/frenzy.png"},
        {"Exhaustion", "Assets/images/exhaustion.png"},
        {"Purify",     "Assets/images/purify.png"},
        {"Curse",      "Assets/images/curse.png"},
        {"Haste",      "Assets/images/haste.png"},
        {"Stealth",    "Assets/images/stealth.png"},
        {"Fear",       "Assets/images/fear.png"},
    };
    for (const auto& [name, path] : ICONS)
        status_icon_textures_[name] = Engine::GetTextureManager().Load(path);

    effect_descriptions_ = {
        {"Lifesteal",  "Recover 50% of damage dealt this turn (round down)"},
        {"Frenzy",     "If next attack deals 10+ damage, target receives random debuff; otherwise, the Fighter receives it."},
        {"Exhaustion", "MOV and Action points become 0 next turn"},
        {"Purify",     "Removes all status effects from self"},
        {"Blessing",   "All damage taken -3, all damage dealt +3"},
        {"Curse",      "All damage taken +3, all damage dealt -3"},
        {"Haste",      "MOV +1, Actions +1"},
        {"Stealth",    "Untargetable. First damage next turn doubled. Removed on attack."},
        {"Fear",       "All damage dealt -3, MOV -1"},
    };

    // 효과별 툴팁 폭 사전 계산
    {
        constexpr double PAD = 10.0;
        auto& tm = Engine::GetTextManager();
        for (const auto& [name, desc] : effect_descriptions_)
        {
            double w0 = tm.CalculateTextSize(name,                   Fonts::Kings).x * 0.5;
            double w1 = tm.CalculateTextSize("Duration: 99 turn(s)", Fonts::Kings).x * 0.4;
            double w2 = tm.CalculateTextSize(desc,                   Fonts::Kings).x * 0.35;
            double max_w = std::max({ w0, w1, w2 });
            effect_tooltip_widths_[name] = std::min(max_w + PAD * 2.0,
                                                    static_cast<double>(VW) - 20.0);
        }
    }

    // Portrait textures for the left-side status panel
    portrait_textures_[static_cast<int>(CharacterTypes::Dragon)]  = Engine::GetTextureManager().Load("Assets/images/dragon.png");
    portrait_textures_[static_cast<int>(CharacterTypes::Fighter)] = Engine::GetTextureManager().Load("Assets/images/fighter.png");
    portrait_textures_[static_cast<int>(CharacterTypes::Cleric)]  = Engine::GetTextureManager().Load("Assets/images/cleric_p.png");
    portrait_textures_[static_cast<int>(CharacterTypes::Rogue)]   = Engine::GetTextureManager().Load("Assets/images/rogue_p.png");
    portrait_textures_[static_cast<int>(CharacterTypes::Wizard)]  = Engine::GetTextureManager().Load("Assets/images/wizard_p.png");
}

void GamePlayUIManager::DrawWorld()
{
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    constexpr double ts = GridSystem::TILE_SIZE;

    // ── 호버 타일 흰색 테두리 ──
    if (m_camera_)
    {
        Math::vec2 mouse_pos = Engine::GetInput().GetMousePos();
        auto actual_win = Engine::GetWindow().GetSize();
        Math::vec2 world_pos = m_camera_->ScreenToWorld(mouse_pos, actual_win);

        int gx = static_cast<int>(std::floor(world_pos.x / ts));
        int gy = static_cast<int>(std::floor(world_pos.y / ts));

        auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
        if (grid && gx >= 1 && gx < grid->GetWidth() - 1 && gy >= 1 && gy < grid->GetHeight() - 1)
        {
            double cx = gx * ts + ts * 0.5;
            double cy = gy * ts + ts * 0.5;
            uint32_t fill = Engine::GetInput().MouseDown(0) ? 0x00000066u : 0x00000000u;
            renderer->DrawRectangle(
                Math::TranslationMatrix(Math::vec2{ cx, cy }) * Math::ScaleMatrix(Math::vec2{ ts, ts }),
                fill, 0xFFFFFFFF, 3.0, DrawDepth::OVERLAY);
        }
    }

    // ── 현재 턴 AI 캐릭터 빨간색 테두리 ──
    {
        auto* turnMgr = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
        if (turnMgr)
        {
            Character* cur = turnMgr->GetCurrentCharacter();
            if (cur && cur->GetCharacterType() != CharacterTypes::Dragon)
            {
                // [수정] 은신 상태일 때 릴리즈 모드면 UI 테두리 렌더링 스킵!
                bool skip_render = false;
                if (cur->Has("Stealth"))
                {
                #ifndef _DEBUG
                    skip_render = true; 
                #endif
                }

                if (!skip_render)
                {
                    Math::vec2 pos = cur->GetPosition();
                    int gx = static_cast<int>(std::floor(pos.x / ts));
                    int gy = static_cast<int>(std::floor(pos.y / ts));
                    double cx = gx * ts + ts * 0.5;
                    double cy = gy * ts + ts * 0.5;
                    renderer->DrawRectangle(
                        Math::TranslationMatrix(Math::vec2{ cx, cy }) * Math::ScaleMatrix(Math::vec2{ ts, ts }),
                        0x00000000u, 0xFF0000FFu, 3.0, DrawDepth::OVERLAY);
                }
            }
        }
    }

    constexpr double ICON_SIZE = 24.0; // 캐릭터 위에 띄울 아이콘의 작은 사이즈

    for (Character* ch : m_characters)
    {
        // 살아있는 캐릭터만 확인
        if (!ch || !ch->IsAlive()) continue;

        // [추가] 은신 상태인 캐릭터는 상태이상 아이콘도 띄우지 않고 건너뜁니다!
        if (ch->Has("Stealth"))
        {
        #ifndef _DEBUG
            continue; 
        #endif
        }

        const auto& effects = ch->GetActiveEffects();
        if (effects.empty()) continue;

        // 캐릭터의 월드 포지션을 기반으로 렌더링 위치 계산
        Math::vec2 pos = ch->GetPosition();
        
        // 캐릭터 타일 왼쪽 하단에서 시작
        double start_x = pos.x;
        double start_y = pos.y;

        for (size_t i = 0; i < effects.size(); ++i)
        {
            auto iit = status_icon_textures_.find(effects[i].name);
            if (iit != status_icon_textures_.end() && iit->second)
            {
                // 원본 텍스처 크기를 우리가 원하는 ICON_SIZE에 맞게 스케일링
                double scale_x = ICON_SIZE / static_cast<double>(iit->second->GetSize().x);
                double scale_y = ICON_SIZE / static_cast<double>(iit->second->GetSize().y);

                Math::TransformationMatrix mat =
                    Math::TranslationMatrix(Math::vec2{ start_x + (i * ICON_SIZE), start_y }) *
                    Math::ScaleMatrix(Math::vec2{ scale_x, scale_y });

                // UI 뎁스에 그려서 항상 가장 위에 보이도록 설정
                iit->second->Draw(mat, 0xFFFFFFFF, DrawDepth::UI); 
            }
        }
    }
}

void GamePlayUIManager::DrawStatusEffectPanel()
{
    if (m_characters.empty()) return;

    auto* renderer = CS230::TextureManager::GetRenderer2D();
    constexpr double ICON_S    = 32.0;
    constexpr double PAN_X     = 8.0;
    constexpr double PANEL_TOP = 532.0;   // 8px below Dragon HUD (PAN_BOT=540)
    constexpr double PANEL_BOT = 168.0;   // 8px above slot bar top (160)

    // Dragon is shown in DrawDragonHUD — exclude from this panel
    int n_rows = 0;
    for (Character* ch : m_characters)
        if (ch && ch->GetCharacterType() != CharacterTypes::Dragon)
            ++n_rows;
    if (n_rows == 0) return;

    // 항상 4슬롯 예약 — 캐릭터 추가 여분 공간 확보, 최대 128px
    double avail_h    = PANEL_TOP - PANEL_BOT;
    int    reserve    = std::max(n_rows, 4);
    double PORT_D     = std::min((avail_h - (reserve - 1) * 4.0 - 8.0) / reserve, 128.0);
    double ICON_X0    = PAN_X + PORT_D + 4.0;
    double ROW_STEP   = PORT_D + 4.0;

    double pan_h   = reserve * PORT_D + (reserve - 1) * 4.0 + 8.0;
    double pan_w   = 300.0;
    double pan_top = PANEL_TOP;
    double pan_cy  = pan_top - pan_h * 0.5;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ PAN_X + pan_w * 0.5, pan_cy }) *
        Math::ScaleMatrix(Math::vec2{ pan_w, pan_h });
    renderer->DrawRectangle(bg, 0x1a1a2e99, 0x5555aaff, 1.5, DrawDepth::UI + 0.02f);

    double row_bot = pan_top - 4.0 - PORT_D;

    for (Character* ch : m_characters)
    {
        if (!ch || ch->GetCharacterType() == CharacterTypes::Dragon) continue;

        auto pit = portrait_textures_.find(static_cast<int>(ch->GetCharacterType()));
        if (pit != portrait_textures_.end() && pit->second)
        {
            uint32_t tint = ch->IsAlive() ? 0xFFFFFFFF : 0x666666FF;
            double scl = PORT_D / static_cast<double>(pit->second->GetSize().x);
            pit->second->Draw(
                Math::TranslationMatrix(Math::vec2{ PAN_X, row_bot }) *
                Math::ScaleMatrix(Math::vec2{ scl, scl }),
                tint, DrawDepth::UI + 0.015f);
        }

        const auto& effects = ch->GetActiveEffects();
        double icon_y = row_bot + (PORT_D - ICON_S) * 0.5;
        for (size_t ei = 0; ei < effects.size(); ++ei)
        {
            auto iit = status_icon_textures_.find(effects[ei].name);
            if (iit == status_icon_textures_.end() || !iit->second) continue;
            double icon_x = ICON_X0 + static_cast<double>(ei) * ICON_S;
            iit->second->Draw(
                Math::TranslationMatrix(Math::vec2{ icon_x, icon_y }),
                0xFFFFFFFF, DrawDepth::UI + 0.01f);
        }

        row_bot -= ROW_STEP;
    }
}

void GamePlayUIManager::DrawStatusEffectTooltip()
{
    if (hovered_effect_name_.empty()) return;

    auto& textMgr  = Engine::GetTextManager();
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    Math::vec2 mouse = m_virtual_mouse_;

    auto wit = effect_tooltip_widths_.find(hovered_effect_name_);
    double TT_W = (wit != effect_tooltip_widths_.end()) ? wit->second : 340.0;
    constexpr double LH   = 24.0;
    constexpr double PAD  = 10.0;
    constexpr double TT_H = PAD * 2.0 + LH * 3.0;

    double tip_x = mouse.x + 16.0;
    if (tip_x + TT_W > static_cast<double>(VW) - 4.0)
        tip_x = mouse.x - TT_W - 8.0;
    double tip_top = mouse.y + TT_H;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ tip_x + TT_W * 0.5, tip_top - TT_H * 0.5 }) *
        Math::ScaleMatrix(Math::vec2{ TT_W, TT_H });
    renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5, DrawDepth::UI - 0.003f);

    double ty = tip_top - PAD - 22;
    textMgr.DrawText(hovered_effect_name_,
        Math::vec2{ tip_x + PAD, ty }, Fonts::Kings, {0.5, 0.5}, CS200::GOLD, DrawDepth::UI - 0.004f);
    ty -= LH;

    textMgr.DrawText("Duration: " + std::to_string(hovered_effect_duration_) + " turn(s)",
        Math::vec2{ tip_x + PAD, ty }, Fonts::Kings, {0.4, 0.4}, CS200::YELLOW, DrawDepth::UI - 0.004f);
    ty -= LH;

    auto dit = effect_descriptions_.find(hovered_effect_name_);
    if (dit != effect_descriptions_.end())
        textMgr.DrawText(dit->second, Math::vec2{ tip_x + PAD, ty },
            Fonts::Kings, {0.35, 0.35}, CS200::WHITE, DrawDepth::UI - 0.004f);
}

void GamePlayUIManager::DrawActionLabel()
{
    std::string label;
    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();

    // Popup open = spell chosen but level not picked yet → show spell name immediately
    if (popup_open_ && !popup_spell_id_.empty())
    {
        if (spell_sys)
        {
            const SpellData* data = spell_sys->GetSpellData(popup_spell_id_);
            if (data && !data->spell_name.empty())
                label = data->spell_name;
        }
        if (label.empty()) label = popup_spell_id_;
    }
    else if (m_input_handler_ptr_)
    {
        auto state = m_input_handler_ptr_->GetCurrentState();
        if (state == PlayerInputHandler::ActionState::TargetingForAttack)
        {
            label = "Attack";
        }
        else if (state == PlayerInputHandler::ActionState::TargetingForSpell ||
                 state == PlayerInputHandler::ActionState::WallPlacementMulti ||
                 state == PlayerInputHandler::ActionState::LavaPlacementMulti)
        {
            std::string spell_id = m_input_handler_ptr_->GetSelectedSpellId();
            if (spell_sys)
            {
                const SpellData* data = spell_sys->GetSpellData(spell_id);
                if (data && !data->spell_name.empty())
                    label = data->spell_name;
            }
            if (label.empty()) label = spell_id;
        }
    }

    if (label.empty()) return;

    auto& textMgr  = Engine::GetTextManager();
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    constexpr Math::ivec2 win = { VW, VH };

    constexpr double PAD_X = 20.0;
    constexpr double H     = 30.0;
    double y_top = slot_bar_center_y_ - 48.0 - 8.0;  // below slot bar (bar bottom = center - 48)
    double cx    = win.x * 0.5;
    double approx_w = static_cast<double>(label.size()) * 10.0 + PAD_X * 2.0;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ cx, y_top - H * 0.5 }) *
        Math::ScaleMatrix(Math::vec2{ approx_w, H });
    renderer->DrawRectangle(bg, 0x1a1a4ecc, 0x8888ffff, 1.5, DrawDepth::UI + 0.001f);

    textMgr.DrawText(label,
        Math::vec2{ cx - approx_w * 0.5 + PAD_X, y_top - H + 6.0 },
        Fonts::Kings, { 0.4, 0.4 }, CS200::GOLD, DrawDepth::UI);
}

void GamePlayUIManager::DrawBattleLog()
{
  if (!show_battle_log_) return;

  auto* renderer = CS230::TextureManager::GetRenderer2D();
  auto& text_mgr = Engine::GetTextManager();

  const Math::vec2 TS = { 0.35, 0.35 };

  // 패널 배경
  Math::TransformationMatrix bg =
    Math::TranslationMatrix(Math::vec2{ LOG_PANEL_X + LOG_PANEL_W * 0.5, LOG_PANEL_Y - LOG_PANEL_H * 0.5 }) *
    Math::ScaleMatrix(Math::vec2{ LOG_PANEL_W, LOG_PANEL_H });
  renderer->DrawRectangle(bg, 0x1a1a2ecc, 0x5555aaff, 1.5, DrawDepth::UI - 0.01f);

  text_mgr.DrawText("Battle Log", Math::vec2{ LOG_PANEL_X + 8.0, LOG_PANEL_Y - 18.0 },
                    Fonts::Kings, { 0.45, 0.45 }, CS200::WHITE, DrawDepth::UI - 0.02f);

  // 콘텐츠 클립 경계
  const double clip_top    = LOG_PANEL_Y - LOG_TITLE_H;
  const double clip_bottom = LOG_PANEL_Y - LOG_PANEL_H;
  const double visible_h   = LOG_PANEL_H - LOG_TITLE_H;
  const double total_h     = ComputeLogContentHeight();

  // 순방향 렌더링: oldest → newest (위→아래), 라운드/사이드 헤더 삽입
  double cur_y        = clip_top + log_scroll_offset_;
  int    prev_round   = -1;
  bool   prev_player  = true;
  bool   first_in_round = true;

  for (const auto& entry : turn_history_)
  {
    if (cur_y <= clip_bottom) break;

    // ─── Round N ─── 헤더
    if (entry.round_number != prev_round)
    {
      if (cur_y <= clip_top && cur_y > clip_bottom)
        text_mgr.DrawText("--- Round " + std::to_string(entry.round_number) + " ---",
                          Math::vec2{ LOG_PANEL_X + 8.0, cur_y },
                          Fonts::Kings, TS, 0xaaaaaaff, DrawDepth::UI - 0.02f);
      cur_y        -= LOG_LINE_H;
      prev_round    = entry.round_number;
      first_in_round = true;
    }

    // ▷ Player Turn / ▷ Enemy Turn 헤더 (사이드 변경 시)
    if (first_in_round || entry.is_player != prev_player)
    {
      if (cur_y <= clip_top && cur_y > clip_bottom)
      {
        uint32_t    sc = entry.is_player ? 0x88ccffff : 0xff8844ff;
        std::string sh = entry.is_player ? "> Player Turn" : "> Enemy Turn";
        text_mgr.DrawText(sh, Math::vec2{ LOG_PANEL_X + 8.0, cur_y },
                          Fonts::Kings, TS, sc, DrawDepth::UI - 0.02f);
      }
      cur_y        -= LOG_LINE_H;
      prev_player   = entry.is_player;
      first_in_round = false;
    }

    // 캐릭터 이름
    if (cur_y <= clip_top && cur_y > clip_bottom)
    {
      uint32_t nc = entry.is_player ? 0x88ccffff : 0xff8844ff;
      text_mgr.DrawText(entry.actor_name,
                        Math::vec2{ LOG_PANEL_X + 8.0 + LOG_INDENT, cur_y },
                        Fonts::Kings, TS, nc, DrawDepth::UI - 0.02f);
    }
    cur_y -= LOG_LINE_H;

    // 행동 로그 줄
    for (const auto& line : entry.lines)
    {
      if (cur_y <= clip_bottom) break;
      if (cur_y <= clip_top)
        text_mgr.DrawText(line, Math::vec2{ LOG_PANEL_X + 8.0 + LOG_INDENT * 2, cur_y },
                          Fonts::Kings, TS, CS200::WHITE, DrawDepth::UI - 0.02f);
      cur_y -= LOG_LINE_H;
    }
    cur_y -= 4.0;
  }

  // 스크롤바 (콘텐츠가 패널보다 길 때만)
  if (total_h > visible_h)
  {
    double track_h    = visible_h;
    double thumb_h    = std::max(20.0, track_h * track_h / total_h);
    double max_scroll = total_h - visible_h;
    double thumb_top  = clip_top - (log_scroll_offset_ / max_scroll) * (track_h - thumb_h);

    Math::TransformationMatrix track_mat =
      Math::TranslationMatrix(Math::vec2{ LOG_SB_X + LOG_SB_W * 0.5, clip_top - track_h * 0.5 }) *
      Math::ScaleMatrix(Math::vec2{ LOG_SB_W, track_h });
    renderer->DrawRectangle(track_mat, 0x333355cc, 0x00000000, 0.0, DrawDepth::UI - 0.015f);

    uint32_t thumb_color = log_scrollbar_dragging_ ? 0xaaaaffff : 0x7777aaff;
    Math::TransformationMatrix thumb_mat =
      Math::TranslationMatrix(Math::vec2{ LOG_SB_X + LOG_SB_W * 0.5, thumb_top - thumb_h * 0.5 }) *
      Math::ScaleMatrix(Math::vec2{ LOG_SB_W, thumb_h });
    renderer->DrawRectangle(thumb_mat, thumb_color, 0x00000000, 0.0, DrawDepth::UI - 0.02f);
  }
}

void GamePlayUIManager::DrawDisableReasonTooltip()
{
    std::string reason_text = "";
    double tip_x = 0.0;
    double box_center_y = 0.0; 

    constexpr double LH   = 24.0;
    constexpr double PAD  = 10.0;
    constexpr double TT_H = PAD * 2.0 + LH; // 툴팁의 총 높이

    // 1. 팝업창 슬롯 호버링 (버튼 위에 띄우기)
    if (!popup_hover_reason_.empty()) 
    {
        reason_text = popup_hover_reason_;
        double TT_W = static_cast<double>(reason_text.size()) * 11.0 + 20.0;
        
        // 팝업 중앙 정렬
        tip_x = popup_hover_pos_.x - TT_W * 0.5;
        
        // 팝업 버튼 바로 위에 박스 안착
        double box_bottom_y = popup_hover_pos_.y;
        box_center_y = box_bottom_y + (TT_H * 0.5) + 15; 
    }
    // 2. 메인 스킬 버튼 호버링 (정립해주신 위치 적용: 버튼 아래, X + 30, Y - 60)
    else 
    {
        const Button* hovered_disabled_btn = nullptr;
        for (const auto& btn : button_manager_.GetButtons()) 
        {
            if (btn.visible && btn.disabled && btn.hovered && !btn.disable_reason.empty()) 
            {
                hovered_disabled_btn = &btn;
                break;
            }
        }
        
        if (hovered_disabled_btn) {
            reason_text = hovered_disabled_btn->disable_reason;
            double TT_W = static_cast<double>(reason_text.size()) * 11.0 + 20.0;
            
            // [적용] 유저 커스텀 X 위치
            tip_x = hovered_disabled_btn->position.x - TT_W * 0.5 + 30.0;
            
            // [적용] 유저 커스텀 Y 위치
            double target_pos_y = hovered_disabled_btn->position.y - (hovered_disabled_btn->size.y * 0.5);
            double tip_top = target_pos_y - 60.0; 
            
            // 계산된 top을 기준으로 박스 중앙값 설정
            box_center_y = tip_top - (TT_H * 0.5); 
        }
    }

    if (reason_text.empty()) return;

    auto& textMgr  = Engine::GetTextManager();
    auto* renderer = CS230::TextureManager::GetRenderer2D();

    double TT_W = static_cast<double>(reason_text.size()) * 11.0 + 20.0; 

    // 화면 밖 이탈 방지
    if (tip_x < 4.0) tip_x = 4.0;
    if (tip_x + TT_W > static_cast<double>(VW) - 4.0) tip_x = static_cast<double>(VW) - TT_W - 4.0;

    // 1. 배경 박스 그리기
    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ tip_x + TT_W * 0.5, box_center_y }) *
        Math::ScaleMatrix(Math::vec2{ TT_W, TT_H });
    
    renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5, DrawDepth::UI + 0.001f);

    // 2. 텍스트 렌더링 (텍스트 중앙 정렬)
    double ty = (box_center_y - TT_H * 0.5) + PAD + 1.0; 

    textMgr.DrawText(reason_text,
        Math::vec2{ tip_x + PAD, ty }, Fonts::Kings, {0.4, 0.4}, CS200::RED, DrawDepth::UI);
}

void GamePlayUIManager::DrawDragonWorldHoverTooltip()
{
    if (hovered_character_ != nullptr && 
        hovered_character_->IsAlive() && 
        hovered_character_->GetCharacterType() == CharacterTypes::Dragon)
    {
        if (hovered_character_->GetMovementRange() <= 0)
        {
            auto& textMgr = Engine::GetTextManager();
            auto* renderer = CS230::TextureManager::GetRenderer2D();
            
            std::string reason_text = "No MOV.";
            
            // [핵심] 드래곤의 월드 위치를 가져와 화면 위치로 변환합니다.
            Math::vec2 target_pos = m_virtual_mouse_; // (변환 실패 시 마우스 위치를 임시 백업으로 사용)
            
            auto* pos_comp = hovered_character_->GetGridPosition();
            if (pos_comp && m_camera_) 
            {
                // 1. 그리드 타일의 정중앙 월드 좌표 계산
                double tile_s = GridSystem::TILE_SIZE;
                Math::vec2 world_pos = Math::vec2{
                    static_cast<double>(pos_comp->Get().x) * tile_s + tile_s * 0.5,
                    static_cast<double>(pos_comp->Get().y) * tile_s + tile_s * 0.5
                };
                
                // 2. 카메라를 이용해 월드 좌표를 스크린 좌표로, 다시 UI용 가상 좌표로 변환
                auto actual_win = Engine::GetWindow().GetSize();
                Math::vec2 screen_pos = m_camera_->WorldToScreen(world_pos, actual_win);
                target_pos = to_virtual(screen_pos, actual_win);
                
                // 3. 드래곤 머리 위로 예쁘게 띄우기 위해 Y축 오프셋 추가
                target_pos.y += 60.0; 
            }

            double TT_W = static_cast<double>(reason_text.size()) * 11.0 + 20.0; 
            constexpr double LH   = 24.0;
            constexpr double PAD  = 10.0;
            constexpr double TT_H = PAD * 2.0 + LH;

            // X를 중앙에 배치
            double tip_x = target_pos.x - TT_W * 0.5;
            if (tip_x < 4.0) tip_x = 4.0;
            if (tip_x + TT_W > static_cast<double>(VW) - 4.0) tip_x = static_cast<double>(VW) - TT_W - 4.0;
            
            double tip_top = target_pos.y + TT_H;

            // 배경 및 텍스트 렌더링
            Math::TransformationMatrix bg =
                Math::TranslationMatrix(Math::vec2{ tip_x + TT_W * 0.5, tip_top - TT_H * 0.5 }) *
                Math::ScaleMatrix(Math::vec2{ TT_W, TT_H });
            renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5, DrawDepth::UI + 0.001f);
            
            double ty = tip_top - PAD - 22;
            textMgr.DrawText(reason_text,
                Math::vec2{ tip_x + PAD, ty }, Fonts::Kings, {0.4, 0.4}, CS200::RED, DrawDepth::UI);
        }
    }
}

void GamePlayUIManager::ShowNotice(const std::string& text)
{
    m_notice_text_  = text;
    m_notice_timer_ = NOTICE_DURATION;
}

void GamePlayUIManager::DrawNotice()
{
    if (m_notice_timer_ <= 0.0) return;

    auto* renderer = CS230::TextureManager::GetRenderer2D();
    auto& textMgr  = Engine::GetTextManager();

    constexpr double cx = VW * 0.5;
    // TurnIndicator 아래 (panel_cy ≈ 868) 에서 NOTICE_H + 8 px 아래
    constexpr double cy = static_cast<double>(VH) - 8.0 - 48.0 - 8.0 - NOTICE_H * 0.5;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ cx, cy }) *
        Math::ScaleMatrix(Math::vec2{ NOTICE_W, NOTICE_H });
    renderer->DrawRectangle(bg, 0x1a0a0aee, 0xff5533ff, 2.0, DrawDepth::UI - 0.003f);

    textMgr.DrawText(m_notice_text_,
        Math::vec2{ cx - NOTICE_W * 0.5 + 12.0, cy - 10.0 },
        Fonts::Kings, { 0.42, 0.42 }, CS200::WHITE, DrawDepth::UI - 0.004f);
}

void GamePlayUIManager::DrawHoveredTileOutline()
{
    if (!m_camera_) return;

    auto actual_win = Engine::GetWindow().GetSize();
    Math::vec2 mouse_pos = Engine::GetInput().GetMousePos();
    
    Math::vec2 world_pos = m_camera_->ScreenToWorld(mouse_pos, actual_win);
    double tile_size = GridSystem::TILE_SIZE;

    int grid_x = static_cast<int>(std::floor(world_pos.x / tile_size));
    int grid_y = static_cast<int>(std::floor(world_pos.y / tile_size));

    // ==========================================================
    // [핵심 추가] 맵 범위를 벗어난 경우 그리지 않고 렌더링 취소
    // ==========================================================
    // TODO: 프로젝트의 실제 맵 크기에 맞게 가로/세로 최대 타일 개수를 넣어주세요.
    // 만약 GridSystem 컴포넌트에서 맵 크기를 가져올 수 있다면 그 값을 사용하면 가장 좋습니다!
    int map_width  = Engine::GetGameStateManager().GetGSComponent<GridSystem>()->GetWidth(); // 예시: 맵의 가로 타일 개수
    int map_height = Engine::GetGameStateManager().GetGSComponent<GridSystem>()->GetHeight(); // 예시: 맵의 세로 타일 개수

    if (grid_x < 1 || grid_x >= map_width - 1 || 
        grid_y < 1 || grid_y >= map_height - 1) 
    {
        return; // 유효한 맵 범위가 아니면 여기서 함수를 종료하여 허공에 그리는 것을 방지합니다.
    }
    // ==========================================================

    double center_x = (static_cast<double>(grid_x) * tile_size) + (tile_size * 0.5);
    double center_y = (static_cast<double>(grid_y) * tile_size) + (tile_size * 0.5);
    
    Math::vec2 world_center = { center_x, center_y };
    Math::vec2 world_edge   = { center_x + tile_size * 0.5, center_y + tile_size * 0.5 };

    Math::vec2 screen_center = m_camera_->WorldToScreen(world_center, actual_win);
    Math::vec2 ui_center     = to_virtual(screen_center, actual_win);

    Math::vec2 screen_edge   = m_camera_->WorldToScreen(world_edge, actual_win);
    Math::vec2 ui_edge       = to_virtual(screen_edge, actual_win);

    double visual_width  = std::abs(ui_edge.x - ui_center.x) * 2.0;
    double visual_height = std::abs(ui_edge.y - ui_center.y) * 2.0;

    Math::TransformationMatrix tile_transform =
        Math::TranslationMatrix(ui_center) *
        Math::ScaleMatrix(Math::vec2{ visual_width, visual_height });

    auto* renderer = CS230::TextureManager::GetRenderer2D();
    bool mouse_is_down = Engine::GetInput().MouseDown(0);

    if (mouse_is_down)
    {
        // 클릭 중: 내부를 0x00000066 (반투명 검은색)으로 채워서 어둡게 만듭니다.
        // 테두리는 여전히 0xFFFFFFFF (흰색)으로 유지합니다.
        renderer->DrawRectangle(tile_transform, 0x00000066, 0xFFFFFFFF, 3.0, DrawDepth::UI - 0.1f);
    }
    else
    {
        // 단순 호버링: 기존처럼 내부는 0x00000000 (투명)으로 둡니다.
        renderer->DrawRectangle(tile_transform, 0x00000000, 0xFFFFFFFF, 3.0, DrawDepth::UI - 0.1f);
    }
}

// --- DragonicTactics/States/GamePlayUIManager.cpp ---

void GamePlayUIManager::DrawTileOutlineAtPosition(Math::vec2 world_pos, uint32_t border_color)
{
    if (!m_camera_) return;

    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (!grid) return; 

    auto actual_win = Engine::GetWindow().GetSize();
    double tile_size = GridSystem::TILE_SIZE;

    // 1. 전달받은 월드 좌표를 바탕으로 그리드 인덱스 계산
    int grid_x = static_cast<int>(std::floor(world_pos.x / tile_size));
    int grid_y = static_cast<int>(std::floor(world_pos.y / tile_size));

    // 2. 맵 범위 검사
    int map_width  = grid->GetWidth(); 
    int map_height = grid->GetHeight(); 

    if (grid_x < 0 || grid_x >= map_width || 
        grid_y < 0 || grid_y >= map_height) 
    {
        return; 
    }

    // 3. 타일의 중앙 및 모서리 좌표 계산
    double center_x = (static_cast<double>(grid_x) * tile_size) + (tile_size * 0.5);
    double center_y = (static_cast<double>(grid_y) * tile_size) + (tile_size * 0.5);
    
    Math::vec2 world_center = { center_x, center_y };
    Math::vec2 world_edge   = { center_x + tile_size * 0.5, center_y + tile_size * 0.5 };

    // 4. 월드 좌표 -> UI 화면 좌표 변환
    Math::vec2 screen_center = m_camera_->WorldToScreen(world_center, actual_win);
    Math::vec2 ui_center     = to_virtual(screen_center, actual_win);

    Math::vec2 screen_edge   = m_camera_->WorldToScreen(world_edge, actual_win);
    Math::vec2 ui_edge       = to_virtual(screen_edge, actual_win);

    double visual_width  = std::abs(ui_edge.x - ui_center.x) * 2.0;
    double visual_height = std::abs(ui_edge.y - ui_center.y) * 2.0;

    Math::TransformationMatrix tile_transform =
        Math::TranslationMatrix(ui_center) *
        Math::ScaleMatrix(Math::vec2{ visual_width, visual_height });

    // 5. 전달받은 색상(border_color)으로 테두리 렌더링
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    renderer->DrawRectangle(tile_transform, 0x00000000, border_color, 3.0, DrawDepth::UI - 0.1f);
}

// ─── Dragon HUD (Feature 2) ────────────────────────────────────────────
// Text in this engine is drawn with BOTTOM-LEFT at the given position and
// extends UPWARD. All rows below use explicit baselines with safe gaps so
// nothing overlaps. Layout (virtual 1600×900, y=900=top):
//
//   y=892 ┌─ Panel top ─────────────────────────┐
//   y=866 │ ┌───────┐  Dragon (name, scale .45) │  ← NAME_Y
//   y=820 │ │ Port  │  [HP ████████ 140/140  ]  │  ← HP_BAR cy
//   y=784 │ │ 64×64 │  AP 2/2    MOV 5/5         │  ← APMOV_Y
//         │ └───────┘                            │
//   y=752 │ ───────── Spell Slots ──────────      │  ← Slot header
//   y=720 │ L1:  * * * *      (4/4)              │
//   y=688 │ L2:  * * *        (3/3)              │
//   y=656 │ L3:  * *          (2/2)              │
//   y=624 │ L4:  * *          (2/2)              │
//   y=592 │ L5:  .            (0/1)              │
//   y=560 │ [Status icons]                       │
//   y=540 └──────────────────────────────────────┘
void GamePlayUIManager::DrawDragonHUD()
{
    if (!m_player_) return;

    auto* renderer = CS230::TextureManager::GetRenderer2D();
    auto& textMgr  = Engine::GetTextManager();

    constexpr double PAN_X   = 8.0;
    constexpr double PAN_W   = 320.0;
    constexpr double PAN_H   = 352.0;
    constexpr double PAN_TOP = static_cast<double>(VH) - 8.0;        // 892
    constexpr double PAN_BOT = PAN_TOP - PAN_H;                       // 540
    constexpr double PAN_CY  = PAN_TOP - PAN_H * 0.5;                 // 716

    // Panel background
    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ PAN_X + PAN_W * 0.5, PAN_CY }) *
        Math::ScaleMatrix(Math::vec2{ PAN_W, PAN_H });
    renderer->DrawRectangle(bg, 0x1a1a2ecc, 0x5555aaff, 1.5, DrawDepth::UI + 0.02f);

    // ── Portrait (64×64) at top-left ────────────────────────────────
    constexpr double PORT_D    = 64.0;
    constexpr double PORT_SCL  = PORT_D / 128.0;
    constexpr double PORT_X    = PAN_X + 10.0;                        // 18
    constexpr double PORT_TOP  = PAN_TOP - 10.0;                      // 882
    constexpr double PORT_BOT  = PORT_TOP - PORT_D;                   // 818

    auto pit = portrait_textures_.find(static_cast<int>(m_player_->GetCharacterType()));
    if (pit != portrait_textures_.end() && pit->second)
    {
        uint32_t tint = m_player_->IsAlive() ? 0xFFFFFFFF : 0x666666FF;
        pit->second->Draw(
            Math::TranslationMatrix(Math::vec2{ PORT_X, PORT_BOT }) *
            Math::ScaleMatrix(Math::vec2{ PORT_SCL, PORT_SCL }),
            tint, DrawDepth::UI + 0.015f);
    }

    // ── Right column: Name / HP bar / AP&MOV ───────────────────────
    constexpr double STAT_X    = PORT_X + PORT_D + 12.0;              // 94
    constexpr double NAME_Y    = 866.0;                               // name baseline
    constexpr double HP_BAR_CY = 826.0;                               // bar center
    constexpr double HP_BAR_W  = 200.0;
    constexpr double HP_BAR_H  = 18.0;                                // bar 817-835
    constexpr double APMOV_Y   = 786.0;                               // text baseline (text top ≈ 808)

    textMgr.DrawText(m_player_->TypeName(),
        Math::vec2{ STAT_X, NAME_Y },
        Fonts::Kings, { 0.45, 0.45 }, CS200::GOLD, DrawDepth::UI);

    const int hp     = m_player_->GetHP();
    const int hp_max = m_player_->GetMaxHP();
    const double hp_ratio = (hp_max > 0) ? std::clamp(static_cast<double>(hp) / hp_max, 0.0, 1.0) : 0.0;

    Math::TransformationMatrix hp_bg =
        Math::TranslationMatrix(Math::vec2{ STAT_X + HP_BAR_W * 0.5, HP_BAR_CY }) *
        Math::ScaleMatrix(Math::vec2{ HP_BAR_W, HP_BAR_H });
    renderer->DrawRectangle(hp_bg, 0x442222ff, 0x000000ff, 1.0, DrawDepth::UI + 0.01f);

    if (hp_ratio > 0.0)
    {
        const double fill_w = HP_BAR_W * hp_ratio;
        Math::TransformationMatrix hp_fill =
            Math::TranslationMatrix(Math::vec2{ STAT_X + fill_w * 0.5, HP_BAR_CY }) *
            Math::ScaleMatrix(Math::vec2{ fill_w, HP_BAR_H });
        renderer->DrawRectangle(hp_fill, 0x44cc44ff, 0x44cc44ff, 0.0, DrawDepth::UI + 0.005f);
    }

    // HP value text centered INSIDE the bar
    {
        std::string hp_text = "HP " + std::to_string(hp) + " / " + std::to_string(hp_max);
        constexpr double HP_TEXT_SCALE = 0.3;
        Math::vec2 sz = textMgr.CalculateTextSize(hp_text, Fonts::Kings);
        const double tx = STAT_X + HP_BAR_W * 0.5 - sz.x * HP_TEXT_SCALE * 0.5;
        const double ty = HP_BAR_CY - sz.y * HP_TEXT_SCALE * 0.5;
        textMgr.DrawText(hp_text,
            Math::vec2{ tx, ty },
            Fonts::Kings, { HP_TEXT_SCALE, HP_TEXT_SCALE }, CS200::WHITE, DrawDepth::UI - 0.002f);
    }

    // AP / MOV
    int ap_cur = m_player_->GetActionPoints();
    int ap_max = 0;
    if (auto* ap_comp = m_player_->GetActionPointsComponent())
        ap_max = ap_comp->GetMaxPoints();
    int mov_cur = m_player_->GetMovementRange();
    int mov_max = 0;
    if (auto* st = m_player_->GetStatsComponent())
        mov_max = st->GetAllStats().speed;

    std::string ap_mov = "AP " + std::to_string(ap_cur) + "/" + std::to_string(ap_max)
                       + "    MOV " + std::to_string(mov_cur) + "/" + std::to_string(mov_max);
    textMgr.DrawText(ap_mov,
        Math::vec2{ STAT_X, APMOV_Y },
        Fonts::Kings, { 0.38, 0.38 }, CS200::WHITE, DrawDepth::UI);

    // ── Divider line under header ──────────────────────────────────
    constexpr double DIV_Y = 770.0;
    Math::TransformationMatrix div =
        Math::TranslationMatrix(Math::vec2{ PAN_X + PAN_W * 0.5, DIV_Y }) *
        Math::ScaleMatrix(Math::vec2{ PAN_W - 24.0, 1.5 });
    renderer->DrawRectangle(div, 0x5555aaaa, 0x5555aaaa, 0.0, DrawDepth::UI + 0.005f);

    // ── Spell slots: one line per level, scale 0.4 (well clear of portrait) ──
    constexpr double SLOT_FIRST_Y = 738.0;
    constexpr double SLOT_STEP    = 30.0;
    double slot_y = SLOT_FIRST_Y;
    if (auto* slots = m_player_->GetSpellSlots())
    {
        const auto& max_map = slots->GetMaxSlots();
        for (const auto& [level, max_cnt] : max_map)
        {
            if (max_cnt <= 0) continue;
            int cur_cnt = slots->GetSpellSlotCount(level);
            std::string line = "L" + std::to_string(level) + ":  ";
            for (int i = 0; i < max_cnt; ++i)
                line += (i < cur_cnt) ? "* " : ". ";
            line += "  (" + std::to_string(cur_cnt) + "/" + std::to_string(max_cnt) + ")";

            textMgr.DrawText(line,
                Math::vec2{ PAN_X + 16.0, slot_y },
                Fonts::Kings, { 0.4, 0.4 }, CS200::WHITE, DrawDepth::UI);
            slot_y -= SLOT_STEP;
        }
    }

    // ── Status effect icons (bottom of HUD) ────────────────────────
    const auto& effects = m_player_->GetActiveEffects();
    if (!effects.empty())
    {
        constexpr double ICON_S = 32.0;
        constexpr double ICON_Y = PAN_BOT + 10.0;                     // 550
        for (size_t ei = 0; ei < effects.size(); ++ei)
        {
            auto iit = status_icon_textures_.find(effects[ei].name);
            if (iit == status_icon_textures_.end() || !iit->second) continue;
            const double icon_x = PAN_X + 16.0 + static_cast<double>(ei) * (ICON_S + 4.0);
            iit->second->Draw(
                Math::TranslationMatrix(Math::vec2{ icon_x, ICON_Y }),
                0xFFFFFFFF, DrawDepth::UI + 0.01f);
        }
    }
}

// ─── Right-click cancel hint (Feature 3) ───────────────────────────────
// Pulsing text just below the turn indicator (top-center).
void GamePlayUIManager::DrawCancelHint()
{
    if (!m_input_handler_ptr_) return;

    auto state = m_input_handler_ptr_->GetCurrentState();
    using AS = PlayerInputHandler::ActionState;

    bool show = (state == AS::SelectingMove ||
                 state == AS::Moving ||
                 state == AS::TargetingForAttack ||
                 state == AS::TargetingForSpell ||
                 state == AS::WallPlacementMulti ||
                 state == AS::LavaPlacementMulti);
    if (!show) return;

    std::string text;
    if (state == AS::WallPlacementMulti || state == AS::LavaPlacementMulti)
        text = "Right Click: Remove tile / Deselect";
    else
        text = "Right Click: Cancel";

    // Alpha pulse: 55% to 100%, never fully fades
    const double a = 0.55 + 0.45 * (0.5 + 0.5 * std::sin(m_cancel_hint_time_ * 2.5));
    const uint8_t a_byte = static_cast<uint8_t>(std::clamp(a, 0.0, 1.0) * 255.0);
    // GOLD = 0xffcb00ff (R=0xff G=0xcb B=0x00); replace alpha with pulse value
    const uint32_t tint = 0xffcb0000U | static_cast<uint32_t>(a_byte);

    auto& textMgr   = Engine::GetTextManager();
    auto* renderer  = CS230::TextureManager::GetRenderer2D();
    constexpr double SCALE = 0.45;
    Math::vec2 text_size = textMgr.CalculateTextSize(text, Fonts::Kings);
    const double text_w  = text_size.x * SCALE;
    const double text_h  = text_size.y * SCALE;

    // Centered under turn indicator (turn indicator bottom y ≈ 844, place hint at 810)
    const double cx = static_cast<double>(VW) * 0.5;
    const double y_baseline = 798.0;

    // Subtle pulsing background pill so the text is visible against any map color
    constexpr double PAD_X = 16.0;
    constexpr double PAD_Y = 6.0;
    const double bg_w = text_w + PAD_X * 2.0;
    const double bg_h = text_h + PAD_Y * 2.0;
    const double bg_cy = y_baseline + text_h * 0.5 - 2.0;
    // Background alpha pulses with text but starts darker
    const uint8_t bg_a = static_cast<uint8_t>(std::clamp(a * 0.65, 0.0, 1.0) * 255.0);
    const uint32_t bg_fill   = 0x1a0a0000U | static_cast<uint32_t>(bg_a);
    const uint32_t bg_border = 0xffcb0000U | static_cast<uint32_t>(bg_a);

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ cx, bg_cy }) *
        Math::ScaleMatrix(Math::vec2{ bg_w, bg_h });
    renderer->DrawRectangle(bg, bg_fill, bg_border, 1.5, DrawDepth::UI + 0.001f);

    textMgr.DrawText(text,
        Math::vec2{ cx - text_w * 0.5, y_baseline },
        Fonts::Kings, { SCALE, SCALE }, tint, DrawDepth::UI);
}

// ── Pause Menu ───────────────────────────────────────────────────────────────

void GamePlayUIManager::UpdatePauseMenu(Math::vec2 virt_mouse, bool just_pressed, bool mouse_down)
{
	using L = PauseLayout;

	// Release drag
	if (!mouse_down)
		m_pause_drag_ = PauseDrag::None;

	// Active drag update — runs every frame while button held
	if (mouse_down && m_pause_drag_ != PauseDrag::None)
	{
		double rel_x = std::clamp(virt_mouse.x - L::SLIDER_X, 0.0, L::SLIDER_W);
		int    vol   = static_cast<int>(rel_x / L::SLIDER_W * 100.0);
		if (m_pause_drag_ == PauseDrag::BGM)
		{
			Settings::s_bgm_volume = vol;
			Engine::GetSoundManager().SetBGMVolume(Settings::s_is_bgm_muted ? 0.0f : (static_cast<float>(Settings::s_bgm_volume) / 100.0f));
		}
		else
		{
			Settings::s_sfx_volume = vol;
			Engine::GetSoundManager().SetSFXVolume(Settings::s_is_sfx_muted ? 0.0f : (static_cast<float>(Settings::s_sfx_volume) / 100.0f));
		}
		return;
	}

	if (!just_pressed)
		return;

	// Outside popup → close
	if (virt_mouse.x < L::LEFT || virt_mouse.x > L::RIGHT || virt_mouse.y < L::BOTTOM || virt_mouse.y > L::TOP)
	{
		m_pause_open_ = false;
		return;
	}

	// Slider hit tests — click to jump + begin drag
	if (virt_mouse.x >= L::SLIDER_X && virt_mouse.x <= L::SLIDER_X + L::SLIDER_W)
	{
		if (std::abs(virt_mouse.y - L::BGM_Y) <= L::SLIDER_H * 1.5)
		{
			m_pause_drag_          = PauseDrag::BGM;
			double rel_x           = std::clamp(virt_mouse.x - L::SLIDER_X, 0.0, L::SLIDER_W);
			Settings::s_bgm_volume = static_cast<int>(rel_x / L::SLIDER_W * 100.0);
			Engine::GetSoundManager().SetBGMVolume(Settings::s_is_bgm_muted ? 0.0f : (static_cast<float>(Settings::s_bgm_volume) / 100.0f));
			return;
		}
		if (std::abs(virt_mouse.y - L::SFX_Y) <= L::SLIDER_H * 1.5)
		{
			m_pause_drag_          = PauseDrag::SFX;
			double rel_x           = std::clamp(virt_mouse.x - L::SLIDER_X, 0.0, L::SLIDER_W);
			Settings::s_sfx_volume = static_cast<int>(rel_x / L::SLIDER_W * 100.0);
			Engine::GetSoundManager().SetSFXVolume(Settings::s_is_sfx_muted ? 0.0f : (static_cast<float>(Settings::s_sfx_volume) / 100.0f));
			return;
		}
	}

	// Button hit tests
	for (int i = 0; i < 3; ++i)
	{
		double bcx = L::BTN_START_X + static_cast<double>(i) * (L::BTN_W + L::BTN_GAP) + L::BTN_W * 0.5;
		double bcy = L::BTN_Y;
		if (virt_mouse.x >= bcx - L::BTN_W * 0.5 && virt_mouse.x <= bcx + L::BTN_W * 0.5 &&
		    virt_mouse.y >= bcy - L::BTN_H * 0.5 && virt_mouse.y <= bcy + L::BTN_H * 0.5)
		{
			Engine::GetSoundManager().PlaySFX(SoundManager::SFX_BUTTON_CLICK);
			switch (i)
			{
				case 0: // RETURN
					m_pause_open_ = false;
					break;
				case 1: // RESTART
					GamePlay::s_should_restart = true;
					m_pause_open_              = false;
					break;
				case 2: // QUIT
					m_quit_requested_ = true;
					break;
				default:
					break;
			}
			return;
		}
	}
}

void GamePlayUIManager::DrawPauseMenu()
{
	if (!m_pause_open_)
		return;

	using L        = PauseLayout;
	auto* renderer = CS230::TextureManager::GetRenderer2D();
	auto& textMgr  = Engine::GetTextManager();

	// 1. Full-screen dimming overlay
	renderer->DrawRectangle(
	    Math::TranslationMatrix(Math::vec2{ VW * 0.5, VH * 0.5 }) *
	        Math::ScaleMatrix(Math::vec2{ static_cast<double>(VW), static_cast<double>(VH) }),
	    0x00000099, 0x00000000, 0.0, 0.005f);

	// 2. Popup box
	renderer->DrawRectangle(
	    Math::TranslationMatrix(Math::vec2{ L::CX, L::CY }) * Math::ScaleMatrix(Math::vec2{ L::PW, L::PH }),
	    0x1a1a2eee, 0x5588aaff, 1.5, 0.004f);

	// 3. Title "PAUSE"
	constexpr double TITLE_SCALE = 1.0;
	Math::vec2       title_size  = textMgr.CalculateTextSize("PAUSE", Fonts::Kings);
	double           title_w     = title_size.x * TITLE_SCALE;
	double           title_h     = title_size.y * TITLE_SCALE;
	textMgr.DrawText("PAUSE",
	    Math::vec2{ L::CX - title_w * 0.5, L::TOP - title_h - 8.0 },
	    Fonts::Kings, { TITLE_SCALE, TITLE_SCALE }, 0xFFD700FF, 0.003f);

	// 4. Volume sliders
	auto draw_slider = [&](double cy, int vol, const char* label)
	{
		constexpr double LABEL_SCALE = 0.5;
		constexpr double PCT_SCALE   = 0.45;
		Math::vec2       lbl_size    = textMgr.CalculateTextSize(label, Fonts::Kings);
		double           lbl_h       = lbl_size.y * LABEL_SCALE;

		textMgr.DrawText(label,
		    Math::vec2{ L::LABEL_X, cy - lbl_h * 0.5 },
		    Fonts::Kings, { LABEL_SCALE, LABEL_SCALE }, 0xCCCCCCFF, 0.003f);

		// Track background
		renderer->DrawRectangle(
		    Math::TranslationMatrix(Math::vec2{ L::SLIDER_X + L::SLIDER_W * 0.5, cy }) *
		        Math::ScaleMatrix(Math::vec2{ L::SLIDER_W, L::SLIDER_H }),
		    0x333333FF, 0x555555FF, 1.0, 0.003f);

		// Filled portion
		double fill_ratio = static_cast<double>(vol) / 100.0;
		double fill_w     = L::SLIDER_W * fill_ratio;
		if (fill_w > 0.0)
		{
			renderer->DrawRectangle(
			    Math::TranslationMatrix(Math::vec2{ L::SLIDER_X + fill_w * 0.5, cy }) *
			        Math::ScaleMatrix(Math::vec2{ fill_w, L::SLIDER_H }),
			    0x960000FF, 0x960000FF, 0.0, 0.003f);
		}

		// Knob — depth 0.002f 로 트랙(0.003f)보다 앞에 렌더링
		double knob_size = L::SLIDER_H * 1.5;
		renderer->DrawCircle(
		    Math::TranslationMatrix(Math::vec2{ L::SLIDER_X + fill_w, cy }) *
		        Math::ScaleMatrix(Math::vec2{ knob_size, knob_size }),
		    0xFFC800FF, 0x960000FF, 0.0, 0.002f);

		textMgr.DrawText(std::to_string(vol) + "%",
		    Math::vec2{ L::PCT_X, cy - lbl_h * 0.5 },
		    Fonts::Kings, { PCT_SCALE, PCT_SCALE }, 0xCCCCCCFF, 0.003f);
	};

	draw_slider(L::BGM_Y, Settings::s_bgm_volume, "BGM VOLUME");
	draw_slider(L::SFX_Y, Settings::s_sfx_volume, "SFX VOLUME");

	// 5. Buttons
	static const char* BTN_LABELS[3] = { "RETURN", "RESTART", "QUIT" };
	for (int i = 0; i < 3; ++i)
	{
		double bcx = L::BTN_START_X + static_cast<double>(i) * (L::BTN_W + L::BTN_GAP) + L::BTN_W * 0.5;
		double bcy = L::BTN_Y;

		bool hovered = (m_virtual_mouse_.x >= bcx - L::BTN_W * 0.5 && m_virtual_mouse_.x <= bcx + L::BTN_W * 0.5 &&
		                m_virtual_mouse_.y >= bcy - L::BTN_H * 0.5 && m_virtual_mouse_.y <= bcy + L::BTN_H * 0.5);

		uint32_t fill   = hovered ? 0x5588aaff : 0x222244ff;
		uint32_t border = hovered ? 0xFFD700FF : 0x5588aaff;

		renderer->DrawRectangle(
		    Math::TranslationMatrix(Math::vec2{ bcx, bcy }) * Math::ScaleMatrix(Math::vec2{ L::BTN_W, L::BTN_H }),
		    fill, border, 1.5, 0.003f);

		constexpr double BTN_TEXT_SCALE = 0.5;
		Math::vec2       lbl_size       = textMgr.CalculateTextSize(BTN_LABELS[i], Fonts::Kings);
		double           lbl_w          = lbl_size.x * BTN_TEXT_SCALE;
		double           lbl_h          = lbl_size.y * BTN_TEXT_SCALE;
		textMgr.DrawText(BTN_LABELS[i],
		    Math::vec2{ bcx - lbl_w * 0.5, bcy - lbl_h * 0.5 },
		    Fonts::Kings, { BTN_TEXT_SCALE, BTN_TEXT_SCALE }, hovered ? 0xFFD700FF : 0xCCCCCCFF, 0.002f);
	}
}