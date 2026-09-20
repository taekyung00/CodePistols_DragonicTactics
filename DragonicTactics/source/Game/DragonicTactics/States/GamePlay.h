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
#include "Engine/Texture.h"
#include "Engine/Vec2.h"
#include <memory>
#include <set>
#include <string>
#include <vector>

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
  static int s_level_id;                        // 0 = 일반 모드, 1/2/3 = 레벨 모드
  static std::vector<std::string> s_allowed_spells;  // 빈 배열 = 모든 스펠 허용

  private:
  static constexpr Math::ivec2				  default_window_size = { TacticalCamera::VIRTUAL_W, TacticalCamera::VIRTUAL_H };
  std::unique_ptr<PlayerInputHandler> m_input_handler;
  std::unique_ptr<GamePlayUIManager>  m_ui_manager;
  std::unique_ptr<BattleOrchestrator> m_orchestrator;

  void DisplayDamageAmount(const CharacterDamagedEvent& event, double delay);
	void CheckGameEnd(const CharacterDeathEvent& event);


  // 직전 공격/스펠 SFX 재생 시간 → CharacterDamagedEvent의 데미지 텍스트·피격음 딜레이에 사용
  double m_pending_damage_delay_ = 0.0;

  // 딜레이 후 실행할 피격 이펙트 (셰이크 + 파티클)
  struct PendingHitEffect
  {
    Character*  target;    // 셰이크 대상 — m_confirmed_dead_ 체크 후 사용
    Math::vec2  world_pos; // 파티클 위치 — 이벤트 시점에 캡처
    double      timer;
  };
  std::vector<PendingHitEffect> m_pending_hit_effects_;

  // 스프라이트 시트 애니메이션 이펙트
  struct SpriteEffect
  {
    enum class Mode { Static, Projectile } mode = Mode::Static;
    std::shared_ptr<CS230::Texture> tex;
    int        frame_count  = 1;
    double     fps          = 10.0;
    double     elapsed      = 0.0;
    double     delay        = 0.0;
    double     angle        = 0.0;   // 회전 각도 (라디안). 0 = 오른쪽 방향
    Math::vec2 world_pos;            // Static: 표시 위치(fallback 포함) | Projectile: 목적지
    Math::vec2 proj_origin;          // Projectile 출발 위치
    double     proj_duration   = 0.4;
    Character* follow_char = nullptr; // non-null 이면 매 프레임 이 캐릭터의 현재 위치 사용
    bool       reverse      = false;   // true = 역방향 프레임 재생 (last→0)
    double     custom_scale = -1.0;   // 양수면 이 값으로 scale 직접 지정, 음수면 기본 EFFECT_SCALE 사용
    Character* hide_char    = nullptr; // non-null이면 완료 시 SetHideSprite(false) 호출
    double     hold_time    = 0.0;    // 마지막 프레임을 이 시간(초) 동안 추가로 유지
    bool IsDone() const
    {
        double anim_end = static_cast<double>(frame_count) / fps + hold_time;
        if (mode == Mode::Projectile)
            anim_end = std::max(anim_end, proj_duration);
        return elapsed >= delay + anim_end;
    }
  };
  std::vector<SpriteEffect> m_sprite_effects_;

  // Meteor 전체화면 이펙트
  bool   m_meteor_active_  = false;
  double m_meteor_elapsed_ = 0.0;

  // 레벨3 환경 용암: 라운드당 1회만 체크하기 위한 마지막 처리 라운드
  int m_lava_spawn_last_round_ = -1;

  // 스프라이트 이펙트 텍스처
  std::shared_ptr<CS230::Texture> m_tex_hit2_;
  std::shared_ptr<CS230::Texture> m_tex_hit3_;
  std::shared_ptr<CS230::Texture> m_tex_hit4_;
  std::shared_ptr<CS230::Texture> m_tex_purify_;
  std::shared_ptr<CS230::Texture> m_tex_meteor_;
  std::shared_ptr<CS230::Texture> m_tex_magic_;
  std::shared_ptr<CS230::Texture> m_tex_cry_;
  std::shared_ptr<CS230::Texture> m_tex_magic_hit_;
  std::shared_ptr<CS230::Texture> m_tex_teleport_;
  std::shared_ptr<CS230::Texture> m_tex_think_;

  Character* player  = nullptr;
  std::vector<Character*> enemys {};
  static constexpr int    THINK_FRAMES     = 3;    // think.png 프레임 수 (•, ••, •••)
  static constexpr int    TELEPORT_FRAMES  = 6;    // Teleport.png 프레임 수

  bool   game_end             = false;
  bool   game_end_player_won_ = false;
  double game_end_timer_      = -1.0;  // >= 0 이면 카운트다운 중

  // 메모리 해제 후 IsAlive() 호출을 피하기 위해 포인터 값으로만 추적
  std::set<Character*> m_confirmed_dead_;

  int selected_json_map_index_ = 0;
  std::vector<std::string> available_json_maps_;

  TacticalCamera m_camera;
  Math::vec2     m_prev_mouse            = { 0.0, 0.0 };
  bool           m_right_mouse_was_down  = false;

  void LoadJSONMap(const std::string& map_id);
  void LoadLevelMap(int level_id);
};

namespace CS230
{
  
}