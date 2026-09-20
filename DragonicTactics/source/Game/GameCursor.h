/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameCursor.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    June 9, 2026
*/
#pragma once

// 커스텀 마우스 커서 전역 유틸리티
// - Enable() : OS 커서 숨김 + ImGui 커서 변경 억제
// - Disable(): OS 커서 복원 (프로세스 종료 시 자동 복원되므로 필수는 아님)
// - Draw()   : BeginScene/EndScene 블록 내에서 현재 마우스 위치에 커서 렌더링
//              depth 0.001f — 모든 UI 레이어(슬롯 아이콘 0.005f 포함) 앞에 표시
//
// 사용 패턴:
//   Load()  → GameCursor::Enable()
//   Draw()  → (BeginScene 내, EndScene 직전) GameCursor::Draw() 또는 GameCursor::Draw(true) 호버 시
//   Unload() → (선택) GameCursor::Disable()

class GameCursor
{
  public:
  static void Enable();                    // OS 커서 숨김 + ImGui 억제
  static void Disable();                   // OS 커서 복원
  static void Draw(bool hovered = false);  // 현재 마우스 위치에 커서 텍스처 렌더
};
