/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlay.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 5, 2025
*/
#pragma once
#include "Engine/GameState.h"
#include "Engine/Matrix.h"
#include "Engine/Vec2.h"
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace CS230 { class Texture; }

struct TacticalCamera {
    Math::vec2 target = { 0.0, 0.0 };
    double zoom = 1.0;
    static constexpr double ZOOM_MIN = 0.25;
    static constexpr double ZOOM_MAX = 3.0;
    static constexpr int    VIRTUAL_W = 1600;
    static constexpr int    VIRTUAL_H = 900;

    Math::TransformationMatrix GetWorldMatrix(Math::ivec2 win) const;
    Math::vec2 ScreenToWorld(Math::vec2 screen, Math::ivec2 win) const;
    Math::vec2 WorldToScreen(Math::vec2 world, Math::ivec2 win) const;

    // Build letterboxed virtual-resolution NDC matrix for UI pass
    static Math::TransformationMatrix BuildVirtualNdc(Math::ivec2 win);
    // Convert actual screen pixel → virtual 1600×900 coordinate (for UI hit detection)
    static Math::vec2 ScreenToVirtual(Math::vec2 screen, Math::ivec2 win);
};

class PlayerInputHandler;
class GamePlayUIManager;
class BattleOrchestrator;
class ButtonManager;
class Fighter;
class Dragon;
struct CharacterDamagedEvent;

class GamePlay : public CS230::GameState
{
  public:
  GamePlay();
  virtual ~GamePlay(); // Must be defined in .cpp where unique_ptr member types are complete

  void			Load() override;
  void			Update(double dt) override;
  void			Draw() override;
  void			Unload() override;
  void			DrawImGui() override;
  gsl::czstring GetName() const override;

  static std::string s_next_map_id;
  static bool s_should_restart;

  private:
  static constexpr Math::ivec2				  default_window_size = { TacticalCamera::VIRTUAL_W, TacticalCamera::VIRTUAL_H };
  std::unique_ptr<PlayerInputHandler> m_input_handler;
  std::unique_ptr<GamePlayUIManager>  m_ui_manager;
  std::unique_ptr<BattleOrchestrator> m_orchestrator;

  void DisplayDamageAmount(const CharacterDamagedEvent& event);
	void CheckGameEnd(const CharacterDeathEvent& event);


  Character* player  = nullptr;
  std::vector<Character*> enemys {};
  static constexpr double GAME_OVER_DELAY = 0.5;  // 게임 종료 후 GameOver 화면 전환까지 대기 시간(초)

  bool   game_end             = false;
  bool   game_end_player_won_ = false;
  double game_end_timer_      = -1.0;  // >= 0 이면 카운트다운 중

  // 메모리 해제 후 IsAlive() 호출을 피하기 위해 포인터 값으로만 추적
  std::set<Character*> m_confirmed_dead_;

  int selected_json_map_index_ = 0;
  std::vector<std::string> available_json_maps_;

  // Cutscene
  static constexpr double CUTSCENE_DURATION = 1.5;
  static constexpr int    CUTSCENE_COUNT    = 3;
  int    m_cutscene_index_ = 0;
  double m_cutscene_timer_ = 0.0;
  std::vector<std::shared_ptr<CS230::Texture>> m_cutscene_textures_;

  TacticalCamera m_camera;
  Math::vec2     m_prev_mouse            = { 0.0, 0.0 };
  bool           m_right_mouse_was_down  = false;

  void LoadJSONMap(const std::string& map_id);
};

namespace CS230
{
  
}