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
#include <memory>

class PlayerInputHandler;
class GamePlayUIManager;
class BattleOrchestrator;
class Fighter;
class Dragon;
struct CharacterDamagedEvent;

class GamePlay : public CS230::GameState
{
  public:
  GamePlay();
  virtual ~GamePlay() = default;

  void			Load() override;
  void			Update(double dt) override;
  void			Draw() override;
  void			Unload() override;
  void			DrawImGui() override;
  gsl::czstring GetName() const override;

  private:
  std::unique_ptr<PlayerInputHandler> m_input_handler;
  std::unique_ptr<GamePlayUIManager>  m_ui_manager;
  std::unique_ptr<BattleOrchestrator> m_orchestrator;

  void OnCharacterDamaged(const CharacterDamagedEvent& event);

  // Fighter* fighter = nullptr;
  // Dragon* dragon  = nullptr;
  Character* player	  = nullptr;
  Character* enemy	  = nullptr;
  bool		 game_end = false;
};

namespace CS230
{

}