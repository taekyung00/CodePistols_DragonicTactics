#include "pch.h"
#include "ButtonManager.h"
#include "CS200/IRenderer2D.h"
#include "Engine/Engine.h"
#include "Engine/TextureManager.h"
#include "Engine/TextManager.h"
#include "Engine/DrawDepth.h"
#include "Engine/Texture.h"

void ButtonManager::AddButton(const Button& button)
{
    auto& text_mgr = Engine::GetTextManager();
    
    // text_mgr.점(.) 찍고 나오는 함수 이름으로 바꿔주세요!
    Math::vec2 textSize = text_mgr.CalculateTextSize(button.label, Fonts::Kings);

    // 스케일 축소 비율 적용 (Draw 함수와 동일하게 0.4배)
    textSize.x *= 0.4f;
    textSize.y *= 0.4f;

    // 크기 비교 검사
    if (textSize.x > button.size.x || textSize.y > button.size.y)
    {
        Engine::GetLogger().LogError("텍스트가 버튼보다 커서 추가가 취소되었습니다. Button ID: " + button.id);
        return; 
    }

    // --- 기존 로직 ---
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
            if (btn.on_click != nullptr) 
            {
                btn.hovered = true;
            }
            
            if (mouse_just_clicked)
            {
                btn.pressed = true;
                if (btn.on_click) btn.on_click();
            }
        }
    }
}

void ButtonManager::Draw([[maybe_unused]] Math::TransformationMatrix camera_matrix) const
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
            Math::TranslationMatrix(Math::vec2{ center.x, center.y }) *
            Math::ScaleMatrix(Math::vec2{ btn.size.x, btn.size.y });

        if (!btn.image_path.empty())
        {
            auto& tex = texture_cache_[btn.image_path];
            if (!tex)
                tex = Engine::GetTextureManager().Load(btn.image_path);
            if (tex)
            {
                Math::TransformationMatrix img_transform =
                    Math::TranslationMatrix(Math::vec2{ center.x, center.y }) *
                    Math::ScaleMatrix(Math::vec2{ btn.size.x, btn.size.y });
                tex->Draw(img_transform, 0xFFFFFFFF, DrawDepth::UI);
            }
        }
        else
        {
            renderer->DrawRectangle(btn_transform, bg_color, 0x888888ff, 1.5, DrawDepth::UI);
        }

        // 텍스트 렌더링 (버튼 중앙)
        Math::vec2 text_pos = { btn.position.x + 8.0, btn.position.y - btn.size.y * 0.7 };
        CS200::RGBA tc = btn.disabled ? 0x888888ff : btn.text_color;
        text_mgr.DrawText(btn.label, text_pos, Fonts::Kings, {0.4, 0.4}, tc, DrawDepth::UI-0.001f); // UI보다 살짝 더 앞으로
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