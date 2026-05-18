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
#include <sstream>

// Virtual resolution helpers — mirrors the demo's letterbox approach
static constexpr int VW = TacticalCamera::VIRTUAL_W;
static constexpr int VH = TacticalCamera::VIRTUAL_H;

static Math::vec2 to_virtual(Math::vec2 actual, Math::ivec2 actual_win) noexcept
{
    double scale = std::min((double)actual_win.x / VW, (double)actual_win.y / VH);
    double ox    = (actual_win.x - VW * scale) * 0.5;
    double oy    = (actual_win.y - VH * scale) * 0.5;
    return { (actual.x - ox) / scale, (actual.y - oy) / scale };
}

void GamePlayUIManager::SetCamera(const TacticalCamera* camera)
{
    m_camera_ = camera;
}

void GamePlayUIManager::ShowDamageText(int damage, Math::vec2 position, Math::vec2 size)
{
  m_damage_texts.push_back({ std::to_string(damage), position, size, 0.5 });
}

void GamePlayUIManager::ShowGameEnd(std::string&& text)
{
  game_end_text = std::make_unique<std::string>(text);
}

void GamePlayUIManager::Update(double)
{
    Math::vec2 mouse_pos   = Engine::GetInput().GetMousePos();
    bool       mouse_click = Engine::GetInput().MouseJustPressed(0);

    // 실제 화면 마우스를 가상 1600x900 좌표로 변환
    auto actual_win       = Engine::GetWindow().GetSize();
    Math::vec2 virt_mouse = to_virtual(mouse_pos, actual_win);
    m_virtual_mouse_      = virt_mouse;

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
            set_disabled("slot_end_turn", is_ai || game_end_text != nullptr, is_ai ? "Enemy's turn." : "");

            // 스펠 슬롯 비활성화 판별 함수 (is_fixed로 팝업과 고정 스킬 구분)
            auto spell_disabled = [&](const std::string& id, int min_lv, bool is_fixed) {
                bool has_any_slot = false;
                if (slots) {
                    for (int lv = min_lv; lv <= 5; ++lv) {
                        if (slots->HasSlot(lv)) {
                            has_any_slot = true;
                            break;
                        }
                    }
                }
                
                // 팝업 스펠(!is_fixed)은 슬롯이 없어도 버튼은 눌리게 둔다(팝업을 띄우기 위함)
                bool disabled = is_ai || no_ap || (is_fixed && !has_any_slot);
                
                std::string reason = "";
                if (disabled) {
                    if (is_ai) reason = "Enemy's turn.";
                    else if (no_ap) reason = "No AP."; // 우선순위 1: AP
                    else if (is_fixed && !has_any_slot) reason = "No Spell slot."; // 우선순위 2: 고정 스펠의 슬롯
                }
                set_disabled(id, disabled, reason);
            };

            spell_disabled("slot_S_ATK_010", 1, false);
            spell_disabled("slot_S_ATK_020", 2, true);  // 고정
            spell_disabled("slot_S_ATK_030", 3, false); 
            spell_disabled("slot_S_ATK_040", 3, false);
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
        double start_x      = slot_bar_x_[popup_slot_index_] + 32.0
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
        for (int i = 0; i < 9; ++i)
        {
            if (button_manager_.IsHovered(std::string("slot_") + SPELL_IDS[i]))
            {
                hovered_spell_id_ = SPELL_IDS[i];
                hovered_slot_cx_  = slot_bar_x_[i + 1] + 32.0; 
                break;
            }
        }
    }

    // ── 3c. 상태이상 아이콘 호버 감지 ─────────────────────────────
    hovered_effect_name_.clear();
    hovered_effect_duration_ = 0;
    {
        constexpr double PORT_H   = 48.0;
        constexpr double ICON_S   = 32.0;
        constexpr double ICON_X0  = 60.0;  
        constexpr double ROW_STEP = 52.0;  

        int    n_rows  = static_cast<int>(m_characters.size());
        double pan_h   = n_rows * PORT_H + (n_rows - 1) * 4.0 + 8.0;
        double pan_top = static_cast<double>(VH) * 0.5 + pan_h * 0.5;
        double row_bot = pan_top - 4.0 - PORT_H;

        for (Character* ch : m_characters)
        {
            if (ch)
            {
                double icon_y = row_bot + (PORT_H - ICON_S) * 0.5;
                const auto& effects = ch->GetActiveEffects();
                for (int ei = 0; ei < static_cast<int>(effects.size()); ++ei)
                {
                    double icon_x = ICON_X0 + ei * ICON_S;
                    if (virt_mouse.x >= icon_x && virt_mouse.x < icon_x + ICON_S &&
                        virt_mouse.y >= icon_y && virt_mouse.y < icon_y + ICON_S)
                    {
                        hovered_effect_name_     = effects[ei].name;
                        hovered_effect_duration_ = effects[ei].duration;
                        break;
                    }
                }
            }
            if (!hovered_effect_name_.empty()) break;
            row_bot -= ROW_STEP;
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
}

void GamePlayUIManager::Draw([[maybe_unused]] Math::TransformationMatrix camera_matrix)
{
    button_manager_.Draw(camera_matrix);
    DrawSlotBar();
    DrawUicastPopup();
    DrawActionLabel();
    DrawTurnIndicator();
    DrawHoverTooltip();
    DrawSpellTooltip();
    DrawStatusEffectPanel();
    DrawStatusEffectTooltip();
    DrawBattleLog();
    DrawDisableReasonTooltip();
    DrawDragonWorldHoverTooltip();

    auto& textMng = Engine::GetTextManager();

    for (const auto& text : m_damage_texts)
    {
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

}

void GamePlayUIManager::SetCharacters(const std::vector<Character*>& characters)
{
  m_characters = characters;
  Engine::GetLogger().LogEvent("GamePlayUIManager: Tracking " + std::to_string(m_characters.size()) + " characters for stats display");
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

    for (int i = 0; i < N; ++i)
        slot_bar_x_[i] = box_x + offset + i * (TILE + offset);
    slot_bar_x_[N] = slot_bar_x_[N - 1] + TILE + offset;

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
    for (int i = 0; i < 10; ++i)
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        b.id       = SLOT_IDS[idx];
        b.position = { slot_bar_x_[idx], slot_bar_center_y_ + TILE * 0.5 };
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
        end.position   = { slot_bar_x_[N], slot_bar_center_y_ + TILE * 0.5 };
        
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
    for (int i = 0; i < static_cast<int>(slot_icons_.size()); ++i)
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

    double start_x = slot_bar_x_[popup_slot_index_] + 32.0
                     - (num_levels * BTN_W + (num_levels - 1) * GAP) * 0.5;

    // 배경 패널
    double panel_w  = num_levels * BTN_W + (num_levels - 1) * GAP + 8.0;
    double panel_cx = slot_bar_x_[popup_slot_index_] + 32.0;
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

    std::string spd_str = "Speed: " + std::to_string(hovered_character_->GetMovementRange());
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

	std::string speed_text = "Speed: " + std::to_string(character->GetMovementRange());
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
    renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5f, DrawDepth::UI + 0.001f);

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
        {"Exhaustion", "Speed and Action points become 0 next turn"},
        {"Purify",     "Removes all status effects from self"},
        {"Blessing",   "All damage taken -3, all damage dealt +3"},
        {"Curse",      "All damage taken +3, all damage dealt -3"},
        {"Haste",      "Speed +1, Actions +1"},
        {"Stealth",    "Untargetable. First damage next turn doubled. Removed on attack."},
        {"Fear",       "All damage dealt -3, speed -1"},
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
    // Status effect icons are now rendered in DrawStatusEffectPanel() (Pass 2 UI)
}

void GamePlayUIManager::DrawStatusEffectPanel()
{
    if (m_characters.empty()) return;

    auto* renderer = CS230::TextureManager::GetRenderer2D();
    constexpr double PORT_D   = 48.0;
    constexpr double PORT_SCL = PORT_D / 128.0;  // 0.375
    constexpr double ICON_S   = 32.0;
    constexpr double PAN_X    = 8.0;
    constexpr double ICON_X0  = PAN_X + PORT_D + 4.0;  // 60
    constexpr double ROW_STEP = PORT_D + 4.0;           // 52

    int    n_rows  = static_cast<int>(m_characters.size());
    double pan_h   = n_rows * PORT_D + (n_rows - 1) * 4.0 + 8.0;
    double pan_w   = 300.0;
    double pan_cy  = static_cast<double>(VH) * 0.5;
    double pan_top = pan_cy + pan_h * 0.5;

    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ PAN_X + pan_w * 0.5, pan_cy }) *
        Math::ScaleMatrix(Math::vec2{ pan_w, pan_h });
    renderer->DrawRectangle(bg, 0x1a1a2e99, 0x5555aaff, 1.5, DrawDepth::UI + 0.02f);

    double row_bot = pan_top - 4.0 - PORT_D;

    for (Character* ch : m_characters)
    {
        if (!ch) { row_bot -= ROW_STEP; continue; }

        auto pit = portrait_textures_.find(static_cast<int>(ch->GetCharacterType()));
        if (pit != portrait_textures_.end() && pit->second)
        {
            uint32_t tint = ch->IsAlive() ? 0xFFFFFFFF : 0x666666FF;
            pit->second->Draw(
                Math::TranslationMatrix(Math::vec2{ PAN_X, row_bot }) *
                Math::ScaleMatrix(Math::vec2{ PORT_SCL, PORT_SCL }),
                tint, DrawDepth::UI + 0.015f);
        }

        const auto& effects = ch->GetActiveEffects();
        double icon_y = row_bot + (PORT_D - ICON_S) * 0.5;
        for (int ei = 0; ei < static_cast<int>(effects.size()); ++ei)
        {
            auto iit = status_icon_textures_.find(effects[ei].name);
            if (iit == status_icon_textures_.end() || !iit->second) continue;
            double icon_x = ICON_X0 + ei * ICON_S;
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
    renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5f, DrawDepth::UI + 0.001f);

    double ty = tip_top - PAD - 22;
    textMgr.DrawText(hovered_effect_name_,
        Math::vec2{ tip_x + PAD, ty }, Fonts::Kings, {0.5, 0.5}, CS200::GOLD, DrawDepth::UI);
    ty -= LH;

    textMgr.DrawText("Duration: " + std::to_string(hovered_effect_duration_) + " turn(s)",
        Math::vec2{ tip_x + PAD, ty }, Fonts::Kings, {0.4, 0.4}, CS200::YELLOW, DrawDepth::UI);
    ty -= LH;

    auto dit = effect_descriptions_.find(hovered_effect_name_);
    if (dit != effect_descriptions_.end())
        textMgr.DrawText(dit->second, Math::vec2{ tip_x + PAD, ty },
            Fonts::Kings, {0.35, 0.35}, CS200::WHITE, DrawDepth::UI);
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
    
    renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5f, DrawDepth::UI + 0.001f);

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
            
            std::string reason_text = "No Speed.";
            
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
            renderer->DrawRectangle(bg, 0x0d0d1eee, 0x6688bbff, 1.5f, DrawDepth::UI + 0.001f);
            
            double ty = tip_top - PAD - 22;
            textMgr.DrawText(reason_text,
                Math::vec2{ tip_x + PAD, ty }, Fonts::Kings, {0.4, 0.4}, CS200::RED, DrawDepth::UI);
        }
    }
}