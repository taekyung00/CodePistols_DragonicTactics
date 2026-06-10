/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 12, 2025
*/

#pragma once
#include <array>
#include <vector>
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"

class Splash : public CS230::GameState
{
  public:
  // Splash();
  void			Load() override;
  void			Update(double dt) override;
  void			Unload() override;
  void			Draw() override;
  void			DrawImGui() override;
  gsl::czstring GetName() const override;


  private:
  enum class Phase
  {
    DigiPen,
    Logo,
    Cutscene
  };

  double						  counter = 0;
  std::shared_ptr<CS230::Texture> texture;

  Phase m_phase_ = Phase::DigiPen;

  // CodePistols 스튜디오 로고 애니메이션 (Release 빌드 전용 — DigiPen 다음, 컷신 이전)
  static constexpr int LOGO_FRAME_COUNT = 18;
  static constexpr std::array<double, LOGO_FRAME_COUNT> LOGO_FRAME_DURATIONS = {
    0.08, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04,
    0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.29
  };
  std::vector<std::shared_ptr<CS230::Texture>> m_logo_textures_;
  int    m_logo_idx_   = 0;
  double m_logo_timer_ = 0.0;

  static constexpr double CUTSCENE_DURATION = 1.5;
  static constexpr int    CUTSCENE_COUNT    = 4;
  std::vector<std::shared_ptr<CS230::Texture>> m_cutscene_textures_;
  int    m_cutscene_idx_   = 0;
  double m_cutscene_timer_ = 0.0;
};
