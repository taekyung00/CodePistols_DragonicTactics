#include "pch.h"

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlay.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 5, 2025
*/
#include "CS200/IRenderer2D.h"
#include "CS200/NDC.h"
#include "Engine/Timer.h"
#include "OpenGL/Environment.h"
#include "GamePlay.h"

#include "Game/MainMenu.h"

#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"

#include "Game/DragonicTactics/Factories/CharacterFactory.h"
#include "Game/DragonicTactics/StateComponents/AISystem.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/DataRegistry.h"
#include "Game/DragonicTactics/StateComponents/DiceManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"
#include "Game/DragonicTactics/StateComponents/TurnManager.h"
#include "Game/DragonicTactics/StateComponents/MapDataRegistry.h"

#include "../Debugger/DebugManager.h"

#include "BattleOrchestrator.h"
#include "GamePlayUIManager.h"
#include "PlayerInputHandler.h"

GamePlay::MapSource GamePlay::s_next_map_source = GamePlay::MapSource::First;
int GamePlay::s_next_map_index = 0;
bool GamePlay::s_should_restart = false;

GamePlay::GamePlay() // : fighter(nullptr), dragon(nullptr)
{
}

GamePlay::~GamePlay() = default; // Must be defined here where unique_ptr member types are complete

void GamePlay::Load()
{

	if (!OpenGL::IsWebGL)
  {
	Engine::GetWindow().ForceResize(default_window_size.x, default_window_size.y);
	Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  }
  m_input_handler = std::make_unique<PlayerInputHandler>();
  m_ui_manager	  = std::make_unique<GamePlayUIManager>();
  m_orchestrator  = std::make_unique<BattleOrchestrator>();

  AddGSComponent(new EventBus());
  AddGSComponent(new DiceManager());
  AddGSComponent(new AISystem());
  AddGSComponent(new CombatSystem());
  AddGSComponent(new CS230::GameObjectManager());
  AddGSComponent(new GridSystem());
  AddGSComponent(new TurnManager());
  AddGSComponent(new DebugManager());
  AddGSComponent(new CharacterFactory());
  AddGSComponent(new DataRegistry());
  AddGSComponent(new util::Timer());
  AddGSComponent(new MapDataRegistry());

  GetGSComponent<EventBus>()->Clear();
  GetGSComponent<DiceManager>()->SetSeed(100);
  GetGSComponent<DebugManager>()->Init();
  GetGSComponent<CombatSystem>()->SetDiceManager(GetGSComponent<DiceManager>());
  GetGSComponent<DataRegistry>()->LoadFromFile("Assets/Data/characters.json");
  GetGSComponent<DataRegistry>()->LoadAllCharacterData("Assets/Data/characters.json");
  // GetGSComponent<SpellSystem>()->SetEventBus(GetGSComponent<EventBus>());

  current_map_source_ = s_next_map_source;
  selected_json_map_index_ = s_next_map_index;

  auto* map_registry = GetGSComponent<MapDataRegistry>();
  map_registry->LoadMaps("Assets/Data/maps.json");
  available_json_maps_ = map_registry->GetAllMapIds();

  Engine::GetLogger().LogEvent("Available maps: " + std::to_string(available_json_maps_.size()));
  if (current_map_source_ == MapSource::First)
  {
    Engine::GetLogger().LogEvent("Loading First map");
    LoadFirstMap();
  }
  else
  {
    if (!available_json_maps_.empty() && selected_json_map_index_ < static_cast<int>(available_json_maps_.size()))
    {
      std::string selected_map_id = available_json_maps_[static_cast<std::size_t>(selected_json_map_index_)];
      Engine::GetLogger().LogEvent("Loading JSON map: " + selected_map_id);
      LoadJSONMap(selected_map_id);
    }
    else
    {
      Engine::GetLogger().LogError("Invalid JSON map selection, falling back to hardcoded");
      LoadFirstMap();
    }
  }


  TurnManager* turnMgr = GetGSComponent<TurnManager>();
  turnMgr->SetEventBus(GetGSComponent<EventBus>());
  turnMgr->InitializeTurnOrder(std::vector<Character*>{ player, enemy });
  turnMgr->StartCombat();

  // 신규 추가: UI Manager에 캐릭터 등록
  m_ui_manager->SetCharacters({ player, enemy });
  Engine::GetLogger().LogEvent("GamePlay::Load - Characters registered to UI Manager");

  GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>(
    [this](const CharacterDamagedEvent& event) { 
      this->DisplayDamageAmount(event);
      this->DisplayDamageLog(event);
    });

  GetGSComponent<EventBus>()->Subscribe<CharacterDeathEvent>(
	  [this]([[maybe_unused]] const CharacterDeathEvent& event)
	  {
		  this->CheckGameEnd(event);
	  });

  GetGSComponent<EventBus>()->Subscribe<CharacterEscapedEvent>(
	  [this]([[maybe_unused]] const CharacterEscapedEvent& event)
	  {
		this->game_end = true;
        std::string msg = "Game Over: ";
        msg += event.character->TypeName();
        msg += " has escaped.";
		Engine::GetLogger().LogDebug(msg);
	  });
}



void GamePlay::DisplayDamageAmount(const CharacterDamagedEvent& event)
{
  Engine::GetLogger().LogDebug("Damage Event! " + std::to_string(event.damageAmount));
  Math::vec2 size = { 1.0, 1.0 };
  if (event.target != nullptr)
  {
	const StatsComponent* stats = event.target->GetStatsComponent();
	if (stats != nullptr && stats->GetMaxHP() > 0)
	{
	  float damage_ratio = static_cast<float>(event.damageAmount) / static_cast<float>(stats->GetMaxHP());
	  if (damage_ratio >= 0.5f)
		size = { 2.5, 2.5 };
	  else if (damage_ratio >= 0.33f)
		size = { 2.0, 2.0 };
	  else if (damage_ratio >= 0.2f)
		size = { 1.5, 1.5 };
	  else if (damage_ratio >= 0.1f)
		size = { 1.2, 1.2 };
	}
  }
  Math::vec2 text_position = event.target->GetGridPosition()->Get();
  text_position *= GridSystem::TILE_SIZE;

  m_ui_manager->ShowDamageText(event.damageAmount, text_position, size);
}

void GamePlay::DisplayDamageLog(const CharacterDamagedEvent& event){
    std::string str = event.target->TypeName() + " took " + std::to_string(event.damageAmount) + " damage from " + event.attacker->TypeName() + "(HP: " + std::to_string(event.remainingHP) + ")";
    auto size = GetGSComponent<GridSystem>()->TILE_SIZE;
    auto position = Math::vec2{9.0 * size, 1.0 * size};
    m_ui_manager->ShowDamageLog(str, position, Math::vec2{0.5, 0.5});
}

//  ======== TODO : we have to make it for loop to check all enemy is retired ========
void GamePlay::CheckGameEnd(const CharacterDeathEvent& event){
    if (event.character == player){
        m_ui_manager->ShowGameEnd("Invader Win");
        game_end = true;
        return;
    }

    if(event.character == enemy) {
        m_ui_manager->ShowGameEnd("Player Win");
        game_end = true;
    }
}



void GamePlay::Update(double dt)
{

    if (s_should_restart)
  {
    s_should_restart = false;
    Engine::GetLogger().LogEvent("=== RESTARTING GAMEPLAY ===");
    Engine::GetGameStateManager().PopState();
    Engine::GetGameStateManager().PushState<GamePlay>();
    return;
  }
  
  TurnManager*				turnMgr		 = GetGSComponent<TurnManager>();
  GridSystem*				grid		 = GetGSComponent<GridSystem>();
  CombatSystem*				combatSystem = GetGSComponent<CombatSystem>();
  AISystem*					aiSystem	 = GetGSComponent<AISystem>();
  CS230::GameObjectManager* goMgr		 = GetGSComponent<CS230::GameObjectManager>();
  DebugManager*				debugMgr	 = GetGSComponent<DebugManager>();

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape))
    {
        if (turnMgr) turnMgr->EndCombat();
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
        return;
    }

    if (game_end)
    {
        return;
    }

  Character* current = nullptr;
  if (turnMgr && turnMgr->IsCombatActive())
  {
	current = turnMgr->GetCurrentCharacter();
  }

  if (debugMgr)
	debugMgr->Update(dt);

  if (current != nullptr)
  {
	m_input_handler->Update(dt, current, grid, combatSystem);
	m_orchestrator->Update(dt, turnMgr, aiSystem);
	m_ui_manager->Update(dt);
  }

  goMgr->UpdateAll(dt);
  UpdateGSComponents(dt);

}

void GamePlay::Unload()
{
  if (auto goMgr = GetGSComponent<CS230::GameObjectManager>())
  {
	goMgr->Unload();
  }

  ClearGSComponents();

  m_input_handler.reset();
  m_ui_manager.reset();
  m_orchestrator.reset();


  enemy	 = nullptr;
  player = nullptr;
}

void GamePlay::Draw()
{
  Engine::GetWindow().Clear(0x1a1a1aff);
  auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();

  Math::TransformationMatrix camera_matrix = CS200::build_ndc_matrix(Engine::GetWindow().GetSize());
  renderer_2d->BeginScene(camera_matrix);

  GridSystem* grid_system = GetGSComponent<GridSystem>();
  if (grid_system != nullptr)
  {
	grid_system->Draw();
  }

  CS230::GameObjectManager* goMgr = GetGSComponent<CS230::GameObjectManager>();
  if (goMgr)
  {
	goMgr->DrawAll(camera_matrix);
  }

  m_ui_manager->Draw(camera_matrix);

  GetGSComponent<DebugManager>()->Draw(grid_system);

  renderer_2d->EndScene();
}

void GamePlay::DrawImGui()
{
  GridSystem* grid_system = GetGSComponent<GridSystem>();
  GetGSComponent<DebugManager>()->DrawImGui(grid_system);

  ImGui::Begin("Map Selection");

  const char* current_source = (current_map_source_ == MapSource::First) ? "First" : "JSON";
  ImGui::Text("Current Map: %s", current_source);

  if (current_map_source_ == MapSource::Json && selected_json_map_index_ < static_cast<int>(available_json_maps_.size()))
  {
    ImGui::Text("Map ID: %s", available_json_maps_[static_cast<std::size_t>(selected_json_map_index_)].c_str());
  }

  ImGui::Separator();

  bool is_first_selected = (s_next_map_source == MapSource::First);
  if (is_first_selected)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
  }

  if (ImGui::Button("First Map"))
  {
    s_next_map_source = MapSource::First;
    Engine::GetLogger().LogEvent("Selected: First map (click Restart to apply)");
  }

  if (is_first_selected)
  {
    ImGui::PopStyleColor();
  }

  ImGui::Separator();

  ImGui::Text("JSON Maps:");

  for (int i = 0; i < static_cast<int>(available_json_maps_.size()); ++i)
  {
    const std::string& map_id = available_json_maps_[static_cast<std::size_t>(i)];

    bool is_selected = (s_next_map_source == MapSource::Json && s_next_map_index == i);
    if (is_selected)
    {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    }

    if (ImGui::Button(map_id.c_str()))
    {
      s_next_map_source = MapSource::Json;
      s_next_map_index = i;
      Engine::GetLogger().LogEvent("Selected JSON map: " + map_id + " (click Restart to apply)");
    }

    if (is_selected)
    {
      ImGui::PopStyleColor();
    }
  }

  ImGui::Separator();

  if (ImGui::Button("Restart with Selected Map"))
  {
    Engine::GetLogger().LogEvent("Restart requested - will execute on next frame");
    s_should_restart = true;
  }

  ImGui::End();

  ImGui::Begin("Player Actions");
  TurnManager* turnMgr = GetGSComponent<TurnManager>();

  if (turnMgr && turnMgr->IsCombatActive())
  {
	ImGui::Begin("Combat Status");
	Character* current = turnMgr->GetCurrentCharacter();
	if (current)
	{
	  ImGui::Text("Current Turn: %s", current->TypeName().c_str());
	  ImGui::Text("Turn #%d | Round #%d", turnMgr->GetCurrentTurnNumber(), turnMgr->GetRoundNumber());
	}
	ImGui::End();
  }


  using ActionState		   = PlayerInputHandler::ActionState;
  ActionState currentState = m_input_handler->GetCurrentState();

  // Move Button
  const char* move_text		   = (currentState == ActionState::SelectingMove) ? "Cancel Move" : "Move";
  bool		  is_move_disabled = (currentState != ActionState::None && currentState != ActionState::SelectingMove);

  if (is_move_disabled)
	ImGui::BeginDisabled();
  if (ImGui::Button(move_text))
  {
	if (currentState == ActionState::SelectingMove)
	{
	  m_input_handler->CancelCurrentAction();
	  // 이동 모드 비활성화
	  if (grid_system)
	  {
		Engine::GetLogger().LogEvent("UI: 'Cancel Move' button clicked.");
		grid_system->DisableMovementMode();
	  }
	}
	else
	{
	  m_input_handler->SetState(ActionState::SelectingMove);
	  Engine::GetLogger().LogEvent("UI: 'Move' button clicked.");

	  if (turnMgr && grid_system)
	  {
		Character* current = turnMgr->GetCurrentCharacter();
		if (current)
		{
		  Math::ivec2 current_pos	 = current->GetGridPosition()->Get();
		  int		  movement_range = current->GetMovementRange();

		  grid_system->EnableMovementMode(current_pos, movement_range);

		  Engine::GetLogger().LogEvent("UI: 'Move' button clicked. Movement mode enabled.");
		}
	  }
	}
  }
  if (is_move_disabled)
	ImGui::EndDisabled();

  // Action Button
  const char* action_text		 = (currentState == ActionState::SelectingAction) ? "Cancel Action" : "Action";
  bool		  is_action_disabled = (currentState != ActionState::None && currentState != ActionState::SelectingAction);

  if (is_action_disabled)
	ImGui::BeginDisabled();
  if (ImGui::Button(action_text))
  {
	if (currentState == ActionState::SelectingAction)
	{
	  m_input_handler->CancelCurrentAction();
	  Engine::GetLogger().LogEvent("UI: 'Cancel Action' button clicked.");
	}
	else
	{
	  m_input_handler->SetState(ActionState::SelectingAction);
	  Engine::GetLogger().LogEvent("UI: 'Action' button clicked.");
	}
  }
  if (is_action_disabled)
	ImGui::EndDisabled();

  // End Turn Button
  bool is_end_turn_disabled = (currentState != ActionState::None);
  if (is_end_turn_disabled)
	ImGui::BeginDisabled();
  if (ImGui::Button("End Turn"))
  {
	Engine::GetLogger().LogEvent("UI: 'End Turn' button clicked.");
	if (turnMgr && turnMgr->IsCombatActive())
	{
	  turnMgr->EndCurrentTurn();
	}
  }
  if (is_end_turn_disabled)
	ImGui::EndDisabled();

  ImGui::End();

  if (currentState == ActionState::SelectingAction)
  {
	ImGui::Begin("Action List");

	if (ImGui::Button("Attack"))
	{
	  Engine::GetLogger().LogEvent("UI: 'Attack' selected. Now targeting.");
	  m_input_handler->SetState(ActionState::TargetingForAttack);
	}

	if (ImGui::Button("Spell"))
	{
	  Engine::GetLogger().LogEvent("UI: 'Spell' selected. Now targeting.");
	  m_input_handler->SetState(ActionState::TargetingForSpell);
	}

	ImGui::End();
  }
}

gsl::czstring GamePlay::GetName() const
{
  return "GamePlay";
}

void GamePlay::LoadFirstMap()
{
  Engine::GetLogger().LogEvent("LoadHardcodedMap - BEGIN");

  CS230::GameObjectManager* go_manager		  = GetGSComponent<CS230::GameObjectManager>();
  GridSystem*				grid_system		  = GetGSComponent<GridSystem>();
  CharacterFactory*			character_factory = GetGSComponent<CharacterFactory>();

  const std::vector<std::string> map_data = { "wwwwwwww",
											  "xeefeeew", // new exit tile 'x'
											  "weeeeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weedeeew", "wwwwwwww" };

  for (int y = 0; y < static_cast<int>(map_data.size()); ++y)
  {
	for (int x = 0; x < static_cast<int>(map_data[static_cast<std::size_t>(y)].length()); ++x)
	{
	  char		  tile_char	  = map_data[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
	  Math::ivec2 current_pos = { x, static_cast<int>(map_data.size()) - 1 - y };
	  switch (tile_char)
	  {
		case 'w': grid_system->SetTileType(current_pos, GridSystem::TileType::Wall); break;
		case 'e': grid_system->SetTileType(current_pos, GridSystem::TileType::Empty); break;
		case 'x': // 'x'를 출구로 사용 (exit)
		  grid_system->SetTileType(current_pos, GridSystem::TileType::Exit);
		  grid_system->SetExitPosition(current_pos);
		  Engine::GetLogger().LogEvent("Exit set at position: " + std::to_string(current_pos.x) + ", " + std::to_string(current_pos.y));
		  break;
		case 'f':
		  grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
		  // fighter = new Fighter(current_pos);
		  {
			auto enemy_ptr = character_factory->Create(CharacterTypes::Fighter, current_pos);
			enemy		   = enemy_ptr.get();
			enemy->SetGridSystem(grid_system);
			go_manager->Add(std::move(enemy_ptr));
			grid_system->AddCharacter(enemy, current_pos);
		  }
		  break;
		case 'd':
		  grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
		  {
			auto player_ptr = character_factory->Create(CharacterTypes::Dragon, current_pos);
			player			= player_ptr.get();
			player->SetGridSystem(grid_system);
			go_manager->Add(std::move(player_ptr));
			grid_system->AddCharacter(player, current_pos);
		  }
		  break;
	  }
	}
  }
  Engine::GetLogger().LogEvent("First map loaded.");
}

void GamePlay::LoadJSONMap(const std::string& map_id)
{
  Engine::GetLogger().LogEvent("LoadJSONMap - BEGIN: " + map_id);

  CS230::GameObjectManager* go_manager = GetGSComponent<CS230::GameObjectManager>();
  GridSystem* grid_system = GetGSComponent<GridSystem>();
  CharacterFactory* character_factory = GetGSComponent<CharacterFactory>();
  MapDataRegistry* map_registry = GetGSComponent<MapDataRegistry>();

  MapData map_data = map_registry->GetMapData(map_id);

  if (map_data.id.empty())
  {
    Engine::GetLogger().LogError("Failed to load map: " + map_id + ", falling back to hardcoded");
    LoadFirstMap();
    return;
  }

  grid_system->LoadMap(map_data);

  auto dragon_spawn_it = map_data.spawn_points.find("dragon");
  if (dragon_spawn_it != map_data.spawn_points.end())
  {
    Math::ivec2 dragon_spawn = dragon_spawn_it->second;
    auto player_ptr = character_factory->Create(CharacterTypes::Dragon, dragon_spawn);
    player = player_ptr.get();
    player->SetGridSystem(grid_system);
    go_manager->Add(std::move(player_ptr));
    grid_system->AddCharacter(player, dragon_spawn);
    Engine::GetLogger().LogEvent("Dragon spawned at: " + std::to_string(dragon_spawn.x) + ", " + std::to_string(dragon_spawn.y));
  }
  else
  {
    Engine::GetLogger().LogError("No dragon spawn point in map: " + map_id);
  }

  // Fighter
  auto fighter_spawn_it = map_data.spawn_points.find("fighter");
  if (fighter_spawn_it != map_data.spawn_points.end())
  {
    Math::ivec2 fighter_spawn = fighter_spawn_it->second;
    auto enemy_ptr = character_factory->Create(CharacterTypes::Fighter, fighter_spawn);
    enemy = enemy_ptr.get();
    enemy->SetGridSystem(grid_system);
    go_manager->Add(std::move(enemy_ptr));
    grid_system->AddCharacter(enemy, fighter_spawn);
    Engine::GetLogger().LogEvent("Fighter spawned at: " + std::to_string(fighter_spawn.x) + ", " + std::to_string(fighter_spawn.y));
  }
  else
  {
    Engine::GetLogger().LogError("No fighter spawn point in map: " + map_id);
  }

  Engine::GetLogger().LogEvent("LoadJSONMap - END: " + map_data.name);
}
