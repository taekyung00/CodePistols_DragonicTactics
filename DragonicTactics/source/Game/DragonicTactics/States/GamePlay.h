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
  virtual ~GamePlay(); // Must be defined in .cpp where unique_ptr member types are complete

  void			Load() override;
  void			Update(double dt) override;
  void			Draw() override;
  void			Unload() override;
  void			DrawImGui() override;
  gsl::czstring GetName() const override;

  enum class MapSource{
    First,
    Json
  };

  static MapSource s_next_map_source;
  static int s_next_map_index;
  static bool s_should_restart;

  private:
  static constexpr Math::ivec2				  default_window_size = { 1200, 800 };
  std::unique_ptr<PlayerInputHandler> m_input_handler;
  std::unique_ptr<GamePlayUIManager>  m_ui_manager;
  std::unique_ptr<BattleOrchestrator> m_orchestrator;

  void DisplayDamageAmount(const CharacterDamagedEvent& event);
	void DisplayDamageLog(const CharacterDamagedEvent& event);

	void CheckGameEnd(const CharacterDeathEvent& event);


  // Fighter* fighter = nullptr;
  // Dragon* dragon  = nullptr;
  Character* player	  = nullptr;
  Character* enemy	  = nullptr; // TODO : we have to make it vector
  bool		 game_end = false;

  MapSource current_map_source_ = MapSource::First;
  int selected_json_map_index_ = 0;
  std::vector<std::string> available_json_maps_;

  void LoadFirstMap();
  void LoadJSONMap(const std::string& map_id);
};

namespace CS230
{
  
}