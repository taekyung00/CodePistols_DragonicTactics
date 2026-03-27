#include "pch.h"
#include "ButtonManager.h"
#include "CS200/IRenderer2D.h"
#include "Engine/Engine.h"
#include "Engine/TextureManager.h"
#include "Engine/TextManager.h"

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
            Math::TranslationMatrix(Math::vec2{ center.x, center.y }) * // MAth 오타 수정 및 생성자 호출 간소화
            Math::ScaleMatrix(Math::vec2{ btn.size.x, btn.size.y });
        
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