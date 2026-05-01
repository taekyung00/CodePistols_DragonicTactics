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
    Math::vec2 mouse_pos   = Engine::GetInput().GetMousePos();
    bool       mouse_click = Engine::GetInput().MouseJustPressed(0);

    // Convert actual screen mouse to virtual 1600x900 coordinates
    auto actual_win      = Engine::GetWindow().GetSize();
    Math::vec2 virt_mouse = to_virtual(mouse_pos, actual_win);
    m_virtual_mouse_      = virt_mouse;

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

            auto set_disabled = [&](const std::string& id, bool cond) {
                button_manager_.SetDisabled(id, cond);
            };

            set_disabled("slot_attack",    no_ap || is_ai);
            set_disabled("slot_end_turn",  is_ai);

            auto spell_disabled = [&](const std::string& id, int min_lv) {
                bool no_slot = !slots || !slots->HasSlot(min_lv);
                set_disabled(id, no_ap || no_slot || is_ai);
            };

            spell_disabled("slot_S_ATK_010", 1);
            spell_disabled("slot_S_ATK_020", 2);
            spell_disabled("slot_S_ATK_030", 3);
            spell_disabled("slot_S_ATK_040", 3);
            spell_disabled("slot_S_ENH_040", 1);
            spell_disabled("slot_S_ENH_050", 1);
            spell_disabled("slot_S_DEB_020", 1);
            spell_disabled("slot_S_GEO_010", 2);
            spell_disabled("slot_S_GEO_020", 1);
        }
    }

    // ── 2. 업캐스트 팝업 클릭 처리 (button_manager_.Update 보다 먼저) ──
    if (popup_open_ && mouse_click)
    {
        // 팝업 버튼 geometry 재계산 후 클릭 판별
        int lv_min = 1, lv_max = 5;
        if      (popup_spell_id_ == "S_ATK_010") { lv_min = 1; lv_max = 5; }
        else if (popup_spell_id_ == "S_ATK_030") { lv_min = 3; lv_max = 5; }
        else if (popup_spell_id_ == "S_ATK_040") { lv_min = 3; lv_max = 5; }
        else if (popup_spell_id_ == "S_ENH_040") { lv_min = 0; lv_max = 5; }
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
        for (int i = 0; i < num_levels && !hit; ++i)
        {
            int    lv = lv_min + i;
            double bx = start_x + i * (BTN_W + GAP);
            double by = popup_bottom; // top of button

            if (virt_mouse.x >= bx && virt_mouse.x <= bx + BTN_W &&
                virt_mouse.y >= by - BTN_H && virt_mouse.y <= by)
            {
                hit = true;
                // spell slot 보유 여부 확인
                SpellSlots* slots = nullptr;
                if (turnMgr)
                {
                    Character* c = turnMgr->GetCurrentCharacter();
                    if (c) slots = c->GetSpellSlots();
                }
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

        if (!hit)
            popup_open_ = false;
    }

    // ── 3. 버튼 업데이트 ─────────────────────────────────────────
    button_manager_.Update(virt_mouse, mouse_click);

    // ── 4. 호버 캐릭터 감지 ──────────────────────────────────────
    hovered_character_ = nullptr;
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (grid)
    {
        Math::vec2 world_pos = mouse_pos;
        if (m_camera_)
            world_pos = m_camera_->ScreenToWorld(mouse_pos, Engine::GetWindow().GetSize());
        int gx = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
        int gy = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
        Math::ivec2 gp{ gx, gy };
        if (grid->IsValidTile(gp))
            hovered_character_ = grid->GetCharacterAt(gp);
    }

    // ── 5. Dragon 호버 → 이동 범위 미리 표시 ─────────────────────
    // None 상태 + 플레이어 턴일 때만, 이전 호버 상태 변화 시에만 EnableMovement
    bool in_none_state = m_input_handler_ptr_ &&
                         (m_input_handler_ptr_->GetCurrentState() == PlayerInputHandler::ActionState::None);
    if (grid && in_none_state && turnMgr)
    {
        Character* current_actor = turnMgr->GetCurrentCharacter();
        bool is_player_turn = current_actor && !current_actor->IsAIControlled();
        if (is_player_turn)
        {
            if (hovered_character_ && !hovered_character_->IsAIControlled())
            {
                grid->EnableMovementMode(
                    hovered_character_->GetGridPosition()->Get(),
                    hovered_character_->GetMovementRange());
            }
            else if (grid->IsMovementModeActive())
            {
                grid->DisableMovementMode();
            }
        }
    }

    // ── 6. 데미지 텍스트 수명 갱신 ─────────────────────────────
    for (auto& text : m_damage_texts) text.lifetime -= dt;
    for (auto& text : m_damage_log)   text.lifetime -= dt;

    m_damage_texts.erase(
        std::remove_if(m_damage_texts.begin(), m_damage_texts.end(),
            [](const DamageText& t) { return t.lifetime <= 0; }),
        m_damage_texts.end());

    m_damage_log.erase(
        std::remove_if(m_damage_log.begin(), m_damage_log.end(),
            [](const DamageText& t) { return t.lifetime <= 0; }),
        m_damage_log.end());

    for (auto& log : m_spell_logs) log.lifetime -= dt;
    m_spell_logs.erase(
        std::remove_if(m_spell_logs.begin(), m_spell_logs.end(),
            [](const SpellLog& l) { return l.lifetime <= 0.0; }),
        m_spell_logs.end());
}

void GamePlayUIManager::Draw([[maybe_unused]] Math::TransformationMatrix camera_matrix)
{
    button_manager_.Draw(camera_matrix);
    DrawSlotBar();
    DrawUicastPopup();
    DrawActionLabel();
    DrawTurnIndicator();
    DrawHoverTooltip();
    DrawBattleLog();

    auto& textMng = Engine::GetTextManager();

    for (const auto& text : m_damage_texts)
    {
        Math::vec2 screen_pos = text.position;
        if (m_camera_)
            screen_pos = m_camera_->WorldToScreen(text.position, Engine::GetWindow().GetSize());
        textMng.DrawText(text.text, screen_pos, Fonts::Outlined, text.size, CS200::VIOLET);
    }

    int i = 0;
    for (const auto& text : m_damage_log)
    {
        textMng.DrawText(text.text, text.position + Math::vec2{ 0.0, 30.0 * i },
                         Fonts::Outlined, text.size, CS200::RED);
        ++i;
    }

    if (game_end_text)
    {
        auto size = Engine::GetWindow().GetSize();
        textMng.DrawText(*game_end_text, Math::ivec2{ 0, size.y / 2 },
                         Fonts::Outlined,
                         Math::vec2{ GAME_END_TEXT_SIZE, GAME_END_TEXT_SIZE },
                         CS200::WHITE);
    }

    const float LOG_X      = 800.0f;
    const float LOG_BASE_Y = 50.0f;
    const float LOG_LINE_H = 28.0f;

    for (int li = 0; li < static_cast<int>(m_spell_logs.size()); ++li)
    {
        Math::vec2 pos = { LOG_X, LOG_BASE_Y + LOG_LINE_H * li };
        textMng.DrawText(m_spell_logs[li].text, pos, Fonts::Outlined,
                         { 0.5f, 0.5f }, CS200::GOLD);
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
    const double box_x    = TILE;
    const double bar_bot  = TILE;
    const double bar_h    = TILE * 1.5;
    const double bar_w    = static_cast<double>(win.x) - 2.0 * TILE;
    constexpr int N       = 10;
    const double end_btn_w = TILE * 2.0;
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
    slot_icons_.resize(10, nullptr);
    for (int i = 0; i < 10; ++i)
        slot_icons_[i] = Engine::GetTextureManager().Load(ICON_PATHS[i]);

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

    // slot 5: Mana Conversion (upcast Lv0~5)
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
        end.id       = "slot_end_turn";
        end.position = { slot_bar_x_[N], slot_bar_center_y_ + TILE * 0.5 };
        end.size     = { end_btn_w, static_cast<double>(TILE) };
        end.label    = "End Turn";
        end.on_click = [inputHandler]() { inputHandler->OnEndTurnPressed(); };
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

void GamePlayUIManager::AddSpellLog(const std::string& caster_name, const std::string& spell_name, int level)
{
  std::string text = "[" + caster_name + "] " + spell_name + " Lv." + std::to_string(level);
  m_spell_logs.push_back({ text, SPELL_LOG_LIFETIME });

  if (static_cast<int>(m_spell_logs.size()) > SPELL_LOG_MAX_COUNT)
	  m_spell_logs.erase(m_spell_logs.begin());
}

ButtonManager& GamePlayUIManager::GetButtons()
{
  return button_manager_;
}

void GamePlayUIManager::OnTurnStarted(const std::string& actor_name, int turn_number)
{
  turn_history_.push_back({ turn_number, actor_name, {} });
  if (static_cast<int>(turn_history_.size()) > MAX_LOG_TURNS)
    turn_history_.pop_front();
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

    double bar_w = static_cast<double>(win.x) - 2.0 * TILE;
    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ win.x * 0.5, slot_bar_center_y_ }) *
        Math::ScaleMatrix(Math::vec2{ bar_w, TILE * 1.5 });
    renderer->DrawRectangle(bg, 0x1a1a2e99, 0x5555aaff, 1.5, DrawDepth::UI + 0.001f);

    // 아이콘은 버튼(UI=0.01f)보다 앞에 와야 보임 → UI - 0.005f = 0.005f
    for (int i = 0; i < 10; ++i)
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
    else if (popup_spell_id_ == "S_ENH_040") { lv_min = 0; lv_max = 5; }
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
            Fonts::Outlined, { 0.35, 0.35 },
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
        Fonts::Outlined, { 0.45, 0.45 }, CS200::GOLD, DrawDepth::UI);
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
        Math::vec2{ tip_x + 8.0, ty }, Fonts::Outlined,
        { 0.5, 0.5 }, CS200::GOLD, DrawDepth::UI);
    ty -= LH;

    std::string hp_str = "HP: " + std::to_string(hovered_character_->GetHP())
                       + "/" + std::to_string(hovered_character_->GetMaxHP());
    textMgr.DrawText(hp_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Outlined,
        { 0.4, 0.4 }, CS200::RED, DrawDepth::UI);
    ty -= LH;

    std::string ap_str = "AP: " + std::to_string(hovered_character_->GetActionPoints());
    textMgr.DrawText(ap_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Outlined,
        { 0.4, 0.4 }, CS200::YELLOW, DrawDepth::UI);
    ty -= LH;

    std::string spd_str = "Speed: " + std::to_string(hovered_character_->GetMovementRange());
    textMgr.DrawText(spd_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Outlined,
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
        textMgr.DrawText(slot_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Outlined,
            { 0.4, 0.4 }, CS200::ORANGE, DrawDepth::UI);
        ty -= LH;
    }

    const auto& effects = hovered_character_->GetActiveEffects();
    if (!effects.empty())
    {
        std::string fx_str = "FX:";
        for (const auto& e : effects)
            fx_str += " " + e.name + "(" + std::to_string(e.duration) + ")";
        textMgr.DrawText(fx_str, Math::vec2{ tip_x + 8.0, ty }, Fonts::Outlined,
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
	    Fonts::Outlined, text_scale, CS200::WHITE);

	std::string hp_text = "HP: " + std::to_string(character->GetHP())
	                    + " / " + std::to_string(character->GetMaxHP());
	Engine::GetTextManager().DrawText(hp_text,
	    Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 1.0) },
	    Fonts::Outlined, text_scale, CS200::RED);

	std::string ap_text = "AP: " + std::to_string(character->GetActionPoints());
	Engine::GetTextManager().DrawText(ap_text,
	    Math::vec2{ text_x_pos + 50.0, current_y + panel_height_per_char - (first_line_y + line_height * 2.0) },
	    Fonts::Outlined, text_scale, CS200::YELLOW);

	std::string speed_text = "Speed: " + std::to_string(character->GetMovementRange());
	Engine::GetTextManager().DrawText(speed_text,
	    Math::vec2{ text_x_pos + 30.0, current_y + panel_height_per_char - (first_line_y + line_height * 3.0) },
	    Fonts::Outlined, text_scale, CS200::GREEN);

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
	      Fonts::Outlined, text_scale, CS200::ORANGE);
	}

	const auto& effects = character->GetActiveEffects();
	std::string fx_text = "FX:";
	for (const auto& e : effects)
	  fx_text += " " + e.name + "(" + std::to_string(e.duration) + ")";
	Engine::GetTextManager().DrawText(fx_text,
	    Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 5.0) },
	    Fonts::Outlined, text_scale, CS200::YELLOW);

	current_y -= panel_height_per_char + 40.0;
  }
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
        Fonts::Outlined, { 0.4, 0.4 }, CS200::GOLD, DrawDepth::UI);
}

void GamePlayUIManager::DrawBattleLog()
{
  if (!show_battle_log_) return;

  auto* renderer = CS230::TextureManager::GetRenderer2D();
  auto& text_mgr = Engine::GetTextManager();
  constexpr Math::ivec2 win = { VW, VH };

  constexpr double BTN_W  = 64.0;
  constexpr double MARGIN = 10.0;
  constexpr double PANEL_W = 5 * 64.0;  // 320px (5 tiles)
  constexpr double PANEL_H = 8 * 64.0;  // 512px (8 tiles)
  constexpr double LINE_H  = 22.0;
  constexpr double INDENT  = 12.0;
  const Math::vec2 TS      = { 0.35, 0.35 };

  const double PANEL_X = static_cast<double>(win.x) - BTN_W - MARGIN - PANEL_W - 2.0;
  const double PANEL_Y = static_cast<double>(win.y) * 0.5 + PANEL_H * 0.5;

  Math::TransformationMatrix bg =
    Math::TranslationMatrix(Math::vec2{ PANEL_X + PANEL_W * 0.5, PANEL_Y - PANEL_H * 0.5 }) *
    Math::ScaleMatrix(Math::vec2{ PANEL_W, PANEL_H });
  renderer->DrawRectangle(bg, 0x1a1a2ecc, 0x5555aaff, 1.5, DrawDepth::UI - 0.01f);

  text_mgr.DrawText("Battle Log", Math::vec2{ PANEL_X + 8.0, PANEL_Y - 18.0 },
                    Fonts::Outlined, { 0.45, 0.45 }, CS200::WHITE, DrawDepth::UI - 0.02f);

  double cur_y = PANEL_Y - 45.0;
  for (auto it = turn_history_.rbegin(); it != turn_history_.rend(); ++it)
  {
    if (cur_y < PANEL_Y - PANEL_H + LINE_H) break;

    std::string header = "Turn " + std::to_string(it->turn_number) + ": " + it->actor_name;
    text_mgr.DrawText(header, Math::vec2{ PANEL_X + 8.0, cur_y },
                      Fonts::Outlined, TS, CS200::GOLD, DrawDepth::UI - 0.02f);
    cur_y -= LINE_H;

    for (const auto& line : it->lines)
    {
      if (cur_y < PANEL_Y - PANEL_H + LINE_H) break;
      text_mgr.DrawText(line, Math::vec2{ PANEL_X + 8.0 + INDENT, cur_y },
                        Fonts::Outlined, TS, CS200::WHITE, DrawDepth::UI - 0.02f);
      cur_y -= LINE_H;
    }
    cur_y -= 4.0;
  }
}
