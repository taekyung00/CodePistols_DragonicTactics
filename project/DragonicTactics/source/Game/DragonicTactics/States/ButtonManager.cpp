/**
 * \date 2026 Spring
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"
#include "ButtonManager.h"
#include "CS200/IRenderer2D.h"
#include "Engine/Engine.h"
#include "Engine/TextureManager.h"
#include "Engine/TextManager.h"
#include "Engine/DrawDepth.h"
#include "Engine/Texture.h"
#include <chrono>

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
    bool mouse_is_down = Engine::GetInput().MouseDown(0);

    for (auto& btn : buttons_)
    {
        btn.pressed = false;
        btn.hovered = false;
        btn.held    = false;
        
        if (btn.press_timer > 0) btn.press_timer--;

        if (!btn.visible) continue;

        if (IsPointInButton(btn, mouse_pos))
        {
            btn.hovered = true;
            btn.hover_timer++; // [추가됨] 마우스가 올라가 있는 동안 프레임마다 증가

            if (mouse_is_down) btn.held = true;
            
            if (mouse_just_clicked)
            {
                btn.pressed = true;
                btn.press_timer = 10;
                
                if (!btn.disabled && btn.on_click != nullptr) 
                {
                    btn.on_click();
                }
            }
        }
        else
        {
            btn.hover_timer = 0; // [추가됨] 마우스가 벗어나면 카운터 초기화
        }
    }
}


// --- DragonicTactics/States/ButtonManager.cpp ---

void ButtonManager::Draw([[maybe_unused]] Math::TransformationMatrix camera_matrix) const
{
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    auto& text_mgr = Engine::GetTextManager();

    for (const auto& btn : buttons_)
    {
        if (!btn.visible) continue;

        // 버튼이 시각적으로 눌린 상태인지 확인 (꾹 누르고 있거나 클릭 직후)
        bool is_visually_pressed = (btn.held || btn.press_timer > 0);

        // 1. 기본 색상 설정
        CS200::RGBA bg_color = btn.color_normal;
        if (btn.disabled)      bg_color = btn.color_disabled;
        else if (btn.hovered)  bg_color = btn.color_hover;

        Math::vec2 center = { btn.position.x + btn.size.x * 0.5,
                              btn.position.y - btn.size.y * 0.5 };

        Math::TransformationMatrix btn_transform =
            Math::TranslationMatrix(Math::vec2{ center.x, center.y }) *
            Math::ScaleMatrix(Math::vec2{ btn.size.x, btn.size.y });

        // 2. 버튼 기본 배경(또는 이미지) 렌더링
        if (!btn.image_path.empty())
        {
            auto& tex = texture_cache_[btn.image_path];
            if (!tex) tex = Engine::GetTextureManager().Load(btn.image_path);
            
            if (tex)
            {
                uint32_t tint_color = btn.disabled ? 0x666666FF : 0xFFFFFFFF;
                tex->Draw(btn_transform, tint_color, DrawDepth::UI);
            }
        }
        else
        {
            renderer->DrawRectangle(btn_transform, bg_color, 0x888888ff, 1.5, DrawDepth::UI);
        }

        // ==========================================================
        // [테두리 피드백 렌더링 로직]
        // ==========================================================
        if (is_visually_pressed)
        {
            // 클릭 시 투명도 0의 순수 하얀색 굵은 테두리 (4.0 굵기)
            renderer->DrawRectangle(btn_transform, 0x00000000, 0xFFFFFFFF, 4.0, DrawDepth::UI - 0.0005f);
        }
        else if (btn.hovered)
        {
            // 1. 시스템의 현재 절대 시간을 초(Second) 단위로 가져오기
            auto now = std::chrono::steady_clock::now().time_since_epoch();
            double current_time = std::chrono::duration<double>(now).count();

            // 2. 시간에 기반한 부드러운 사인파 계산 
            // current_time * 5.0 에서 '5.0'이 깜빡임 속도를 결정해! (숫자가 작을수록 느려짐)
            int alpha = static_cast<int>(127 + 127 * std::sin(current_time * 5.0));
            
            // 3. 0xFFFFFF00(순수 하얀색)에 alpha(투명도) 값을 합성
            uint32_t blink_color = 0xFFFFFF00 | static_cast<uint32_t>(alpha);
            
            // 4. 두께 2.0으로 부드럽게 깜빡이는 테두리 렌더링
            renderer->DrawRectangle(btn_transform, 0x00000000, blink_color, 2.0, DrawDepth::UI - 0.0005f);
        }
        // ==========================================================
        
        // 3. 텍스트 렌더링
        Math::vec2 text_pos = { btn.position.x + 8.0, btn.position.y - btn.size.y * 0.7 };
        CS200::RGBA tc = btn.disabled ? 0x888888ff : btn.text_color;
        text_mgr.DrawText(btn.label, text_pos, Fonts::Kings, {0.4, 0.4}, tc, DrawDepth::UI - 0.001f);
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

// [추가] 함수 구현
void ButtonManager::SetDisableReason(const std::string& id, const std::string& reason)
{
    if (auto* btn = FindButton(id)) btn->disable_reason = reason;
}

std::string ButtonManager::GetDisableReason(const std::string& id) const
{
    if (const auto* btn = FindButton(id)) return btn->disable_reason;
    return "";
}