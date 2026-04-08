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
#include "./Engine/Matrix.h"

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

    std::vector<Button>& GetButtons() { return buttons_; }
    const std::vector<Button>& GetButtons() const { return buttons_; }
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