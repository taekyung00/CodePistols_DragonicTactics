# 버튼 매니저 구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: ButtonManager - ImGui 대체 커스텀 버튼 시스템
**작성일**: 2026-03-08

**관련 파일**:
- [GamePlayUIManager.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlayUIManager.h)
- [PlayerInputHandler.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/PlayerInputHandler.h)
- [IRenderer2D.h](../../../DragonicTactics/source/CS200/IRenderer2D.h)
- [GamePlay.cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp)

---

## 개요

현재 Move/Action/End Turn/Attack/Spell 버튼이 **ImGui 창 안에** 구현되어 있어 위치 자유도가 없습니다.
`ButtonManager`는 게임 월드 좌표 어디에든 버튼을 배치할 수 있는 커스텀 UI 시스템을 제공합니다.

### 요구사항
- 원하는 위치에 버튼 생성/제거
- 호버(Hover), 클릭(Pressed), 비활성화(Disabled) 상태 처리
- 마우스 입력 감지
- `IRenderer2D::DrawRectangle()` + `TextManager::DrawText()`로 렌더링

---

## Implementation Tasks

### Task 1: Button 구조체 및 ButtonManager 설계

**파일 생성**: `DragonicTactics/source/Game/DragonicTactics/States/ButtonManager.h`

```cpp
/**
 * \file ButtonManager.h
 * \brief 커스텀 2D 버튼 시스템 - 게임 월드 임의 위치에 버튼 배치
 */
#pragma once
#include "CS200/RGBA.h"
#include "Engine/Vec2.h"
#include <functional>
#include <string>
#include <vector>
#include <map>

/// @brief 단일 버튼 데이터
struct Button
{
    // 식별자
    std::string id;           // 고유 ID (예: "btn_move", "btn_attack")

    // 위치 및 크기 (픽셀 단위)
    Math::vec2 position;      // 버튼 좌상단 좌표
    Math::vec2 size;          // 버튼 너비, 높이

    // 표시 텍스트
    std::string label;

    // 상태
    bool visible  = true;
    bool disabled = false;
    bool hovered  = false;
    bool pressed  = false;    // 이번 프레임에 클릭됨

    // 콜백
    std::function<void()> on_click;

    // 색상 (상태별)
    CS200::RGBA color_normal   = 0x3a3a5cff;  // 기본 (어두운 보라)
    CS200::RGBA color_hover    = 0x5a5a8cff;  // 호버 (밝은 보라)
    CS200::RGBA color_disabled = 0x2a2a2aff;  // 비활성화 (회색)
    CS200::RGBA color_pressed  = 0x7a7aacff;  // 클릭 순간 (더 밝은)
    CS200::RGBA text_color     = 0xffffffff;  // 텍스트 흰색
};

/// @brief 버튼 집합 관리 클래스
class ButtonManager
{
public:
    // ============================================================
    // 버튼 생명주기
    // ============================================================

    /// @brief 버튼 추가. 같은 ID면 덮어씀.
    void AddButton(const Button& button);

    /// @brief 버튼 제거
    void RemoveButton(const std::string& id);

    /// @brief 모든 버튼 제거
    void ClearAll();

    // ============================================================
    // 버튼 상태 제어
    // ============================================================

    void SetVisible(const std::string& id, bool visible);
    void SetDisabled(const std::string& id, bool disabled);
    void SetLabel(const std::string& id, const std::string& label);

    /// @brief 이번 프레임에 버튼이 클릭되었는가
    bool IsPressed(const std::string& id) const;

    /// @brief 버튼이 호버 중인가
    bool IsHovered(const std::string& id) const;

    // ============================================================
    // 프레임 루프
    // ============================================================

    /// @brief 마우스 입력 처리, pressed/hovered 상태 갱신
    void Update(Math::vec2 mouse_pos, bool mouse_just_clicked);

    /// @brief 버튼 렌더링
    void Draw(Math::TransformationMatrix camera_matrix) const;

private:
    std::vector<Button> buttons_;  // 등록 순서 유지

    Button*       FindButton(const std::string& id);
    const Button* FindButton(const std::string& id) const;

    bool IsPointInButton(const Button& btn, Math::vec2 point) const;
};
```

---

### Task 2: ButtonManager 구현

**파일 생성**: `DragonicTactics/source/Game/DragonicTactics/States/ButtonManager.cpp`

```cpp
#include "pch.h"
#include "ButtonManager.h"
#include "CS200/IRenderer2D.h"
#include "Engine/Engine.h"
#include "Engine/TextureManager.h"

void ButtonManager::AddButton(const Button& button)
{
    // 기존 ID면 덮어씀
    for (auto& b : buttons_)
    {
        if (b.id == button.id) { b = button; return; }
    }
    buttons_.push_back(button);
}

void ButtonManager::RemoveButton(const std::string& id)
{
    buttons_.erase(
        std::remove_if(buttons_.begin(), buttons_.end(),
                       [&](const Button& b) { return b.id == id; }),
        buttons_.end()
    );
}

void ButtonManager::ClearAll()
{
    buttons_.clear();
}

void ButtonManager::SetVisible(const std::string& id, bool visible)
{
    if (auto* btn = FindButton(id)) btn->visible = visible;
}

void ButtonManager::SetDisabled(const std::string& id, bool disabled)
{
    if (auto* btn = FindButton(id)) btn->disabled = disabled;
}

void ButtonManager::SetLabel(const std::string& id, const std::string& label)
{
    if (auto* btn = FindButton(id)) btn->label = label;
}

bool ButtonManager::IsPressed(const std::string& id) const
{
    if (const auto* btn = FindButton(id)) return btn->pressed;
    return false;
}

bool ButtonManager::IsHovered(const std::string& id) const
{
    if (const auto* btn = FindButton(id)) return btn->hovered;
    return false;
}

void ButtonManager::Update(Math::vec2 mouse_pos, bool mouse_just_clicked)
{
    for (auto& btn : buttons_)
    {
        btn.pressed = false;
        btn.hovered = false;

        if (!btn.visible || btn.disabled) continue;

        if (IsPointInButton(btn, mouse_pos))
        {
            btn.hovered = true;
            if (mouse_just_clicked)
            {
                btn.pressed = true;
                if (btn.on_click) btn.on_click();
            }
        }
    }
}

void ButtonManager::Draw(Math::TransformationMatrix camera_matrix) const
{
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    auto& text_mgr = Engine::GetTextManager();

    for (const auto& btn : buttons_)
    {
        if (!btn.visible) continue;

        // 배경 색상 결정
        CS200::RGBA bg_color = btn.color_normal;
        if (btn.disabled)      bg_color = btn.color_disabled;
        else if (btn.pressed)  bg_color = btn.color_pressed;
        else if (btn.hovered)  bg_color = btn.color_hover;

        // 버튼 사각형 그리기
        // DrawRectangle은 중심 좌표 기준 → 변환
        Math::vec2 center = { btn.position.x + btn.size.x * 0.5,
                               btn.position.y - btn.size.y * 0.5 };

        Math::TransformationMatrix btn_transform =
            Math::TransformationMatrix::build_translation(center.x, center.y) *
            Math::TransformationMatrix::build_scale(btn.size.x, btn.size.y);

        renderer->DrawRectangle(btn_transform, bg_color, 0x888888ff, 1.5);

        // 텍스트 렌더링 (버튼 중앙)
        Math::vec2 text_pos = { btn.position.x + 8.0, btn.position.y - btn.size.y * 0.7 };
        CS200::RGBA tc = btn.disabled ? 0x888888ff : btn.text_color;
        text_mgr.DrawText(btn.label, text_pos, Fonts::Outlined, {0.4, 0.4}, tc);
    }
}

// ============================================================
// 헬퍼
// ============================================================

Button* ButtonManager::FindButton(const std::string& id)
{
    for (auto& b : buttons_)
        if (b.id == id) return &b;
    return nullptr;
}

const Button* ButtonManager::FindButton(const std::string& id) const
{
    for (const auto& b : buttons_)
        if (b.id == id) return &b;
    return nullptr;
}

bool ButtonManager::IsPointInButton(const Button& btn, Math::vec2 point) const
{
    // position = 좌상단, size = 너비/높이
    return point.x >= btn.position.x &&
           point.x <= btn.position.x + btn.size.x &&
           point.y <= btn.position.y &&
           point.y >= btn.position.y - btn.size.y;
}
```

---

### Task 3: GamePlayUIManager에 ButtonManager 통합

**파일**: `GamePlayUIManager.h`

```cpp
#include "ButtonManager.h"

class GamePlayUIManager
{
public:
    // ... 기존 선언

    void InitButtons();          // 버튼 초기 배치
    ButtonManager& GetButtons(); // PlayerInputHandler에서 접근용

private:
    ButtonManager button_manager_;
    // ... 기존 멤버
};
```

**파일**: `GamePlayUIManager.cpp`

```cpp
void GamePlayUIManager::InitButtons()
{
    // 화면 하단 중앙에 버튼 배치 (1200x800 기준)
    // 좌상단 원점 기준, y가 아래로 증가하는 화면 좌표

    constexpr double BTN_W = 120.0;  // 버튼 너비
    constexpr double BTN_H = 40.0;   // 버튼 높이
    constexpr double BTN_Y = 750.0;  // 하단에서 50px 위 (y=800기준)
    constexpr double GAP   = 10.0;   // 버튼 간격
    constexpr double START_X = 400.0; // 시작 x

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

void GamePlayUIManager::Update(double dt)
{
    // 마우스 입력 가져오기
    Math::vec2 mouse_pos = Engine::GetInput().GetMousePos();
    bool mouse_clicked   = Engine::GetInput().MouseButtonJustPressed(0); // 좌클릭

    button_manager_.Update(mouse_pos, mouse_clicked);

    // ... 기존 데미지 텍스트 업데이트
}

void GamePlayUIManager::Draw(Math::TransformationMatrix camera_matrix)
{
    button_manager_.Draw(camera_matrix);
    // ... 기존 UI 렌더링
}
```

---

### Task 4: PlayerInputHandler에서 ButtonManager 연동

**파일**: `PlayerInputHandler.cpp`의 `Update()`

```cpp
void PlayerInputHandler::Update(double dt, Character* current_character,
                                  GridSystem* grid, CombatSystem* combat_system)
{
    // ButtonManager 참조 (GamePlayUIManager에서)
    ButtonManager& btns = m_ui_manager->GetButtons();

    // Move 버튼 클릭
    if (btns.IsPressed("btn_move"))
    {
        if (m_state == ActionState::SelectingMove)
        {
            CancelCurrentAction();
            grid->DisableMovementMode();
        }
        else
        {
            SetState(ActionState::SelectingMove);
            // 이동 가능 타일 계산
            grid->EnableMovementMode(current_character->GetGridPosition()->Get(),
                                      current_character->GetMovementRange());
        }
    }

    // Action 버튼 클릭
    if (btns.IsPressed("btn_action"))
    {
        if (m_state == ActionState::SelectingAction)
        {
            CancelCurrentAction();
            btns.SetVisible("btn_attack", false);
            btns.SetVisible("btn_spell", false);
        }
        else
        {
            SetState(ActionState::SelectingAction);
            btns.SetVisible("btn_attack", true);
            btns.SetVisible("btn_spell", true);
        }
    }

    // Attack 서브 버튼
    if (btns.IsPressed("btn_attack"))
    {
        SetState(ActionState::TargetingForAttack);
        btns.SetVisible("btn_attack", false);
        btns.SetVisible("btn_spell", false);
    }

    // Spell 서브 버튼
    if (btns.IsPressed("btn_spell"))
    {
        SetState(ActionState::TargetingForSpell);
        btns.SetVisible("btn_attack", false);
        btns.SetVisible("btn_spell", false);
    }

    // End Turn 버튼
    if (btns.IsPressed("btn_end_turn"))
    {
        TurnManager* tm = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
        if (tm) tm->EndCurrentTurn();
    }

    // 버튼 비활성화 상태 갱신
    bool in_action = (m_state != ActionState::None);
    btns.SetDisabled("btn_move", in_action && m_state != ActionState::SelectingMove);
    btns.SetDisabled("btn_action", in_action && m_state != ActionState::SelectingAction);
    btns.SetDisabled("btn_end_turn", in_action);

    // Move/Action 버튼 레이블 갱신 (Cancel 표시)
    btns.SetLabel("btn_move",   (m_state == ActionState::SelectingMove)   ? "Cancel Move"   : "Move");
    btns.SetLabel("btn_action", (m_state == ActionState::SelectingAction) ? "Cancel Action" : "Action");

    // 마우스 클릭 처리 (타일 선택)
    // ... 기존 HandleMouseClick 로직
}
```

---

## Implementation Examples

### 버튼 동적 생성/제거

```cpp
// 특정 스펠 사용 가능 시 스펠 버튼 추가
SpellSystem* ss = GetGSComponent<SpellSystem>();
auto available = ss->GetAvailableSpells(current_character);

// 기존 스펠 버튼 제거
for (const auto& spell : cached_spell_buttons_)
    button_manager_.RemoveButton("spell_" + spell.id);

// 새 스펠 버튼 생성
for (int i = 0; i < static_cast<int>(available.size()); ++i)
{
    button_manager_.AddButton({
        "spell_" + available[i].id,
        { 500.0 + i * 130.0, 700.0 },
        { 120.0, 35.0 },
        available[i].spell_name,
        false,  // 초기 visible
        current_character->GetSpellSlotCount(available[i].spell_level) == 0  // 슬롯 없으면 disabled
    });
}
```

---

## Rigorous Testing

### 테스트 1: 버튼 클릭 감지
```cpp
ButtonManager btns;
btns.AddButton({ "test_btn", {100.0, 100.0}, {80.0, 30.0}, "Test" });

// 버튼 내부 좌표로 클릭
btns.Update({130.0, 85.0}, true);  // 클릭
ASSERT(btns.IsPressed("test_btn"), "Button inside click must register as pressed");

// 버튼 외부 좌표로 클릭
btns.Update({50.0, 50.0}, true);
ASSERT(!btns.IsPressed("test_btn"), "Button outside click must not register");
```

### 테스트 2: 비활성화 상태
```cpp
btns.SetDisabled("test_btn", true);
btns.Update({130.0, 85.0}, true);
ASSERT(!btns.IsPressed("test_btn"), "Disabled button must not respond to clicks");
```

### 테스트 3: 가시성 토글
```cpp
btns.SetVisible("test_btn", false);
btns.Update({130.0, 85.0}, true);
ASSERT(!btns.IsPressed("test_btn"), "Hidden button must not respond to clicks");
```

### 테스트 4: 레이블 변경
```cpp
btns.SetLabel("btn_move", "Cancel Move");
// Draw()에서 "Cancel Move"가 표시되어야 함 (시각적 확인)
```

### 게임 내 수동 테스트
1. 게임 실행 후 하단 버튼 영역 확인
2. Move 버튼 클릭 → 이동 가능 타일 하이라이트
3. Action 버튼 클릭 → Attack/Spell 서브 버튼 표시
4. Attack 클릭 → 타겟 선택 모드
5. End Turn 클릭 → Fighter 턴으로 전환
