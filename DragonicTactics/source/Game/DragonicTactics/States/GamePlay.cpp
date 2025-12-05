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

#include "../Debugger/DebugManager.h"

#include "BattleOrchestrator.h"
#include "GamePlayUIManager.h"
#include "PlayerInputHandler.h"

GamePlay::GamePlay() // : fighter(nullptr), dragon(nullptr)
{
}

void GamePlay::Load()
{
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

  GetGSComponent<EventBus>()->Clear();
  GetGSComponent<DiceManager>()->SetSeed(100);
  GetGSComponent<DebugManager>()->Init();
  GetGSComponent<CombatSystem>()->SetDiceManager(GetGSComponent<DiceManager>());
  GetGSComponent<DataRegistry>()->LoadFromFile("Assets/Data/characters.json");
  GetGSComponent<DataRegistry>()->LoadAllCharacterData("Assets/Data/characters.json");
  // GetGSComponent<SpellSystem>()->SetEventBus(GetGSComponent<EventBus>());

  CS230::GameObjectManager* go_manager		  = GetGSComponent<CS230::GameObjectManager>();
  GridSystem*				grid_system		  = GetGSComponent<GridSystem>();
  CharacterFactory*			character_factory = GetGSComponent<CharacterFactory>();

  const std::vector<std::string> map_data = { "wwwwwwww", "weefeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weedeeew", "wwwwwwww" };

  for (int y = 0; y < map_data.size(); ++y)
  {
	for (int x = 0; x < map_data[y].length(); ++x)
	{
	  char		  tile_char	  = map_data[y][x];
	  Math::ivec2 current_pos = { x, static_cast<int>(map_data.size()) - 1 - y };
	  switch (tile_char)
	  {
		case 'w': grid_system->SetTileType(current_pos, GridSystem::TileType::Wall); break;
		case 'e': grid_system->SetTileType(current_pos, GridSystem::TileType::Empty); break;
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

  TurnManager* turnMgr = GetGSComponent<TurnManager>();
  turnMgr->SetEventBus(GetGSComponent<EventBus>());
  turnMgr->InitializeTurnOrder(std::vector<Character*>{ player, enemy });
  turnMgr->StartCombat();

  GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& event) { this->OnCharacterDamaged(event); });
  GetGSComponent<EventBus>()->Subscribe<CharacterDeathEvent>([this]([[maybe_unused]] const CharacterDeathEvent& event) { this->game_end = true; });
}

void GamePlay::OnCharacterDamaged(const CharacterDamagedEvent& event)
{
  Engine::GetLogger().LogDebug("Damage Event! " + std::to_string(event.damageAmount));
  Math::vec2 size = { 1.0f, 1.0f };
  if (event.target != nullptr)
  {
	const StatsComponent* stats = event.target->GetStatsComponent();
	if (stats != nullptr && stats->GetMaxHP() > 0)
	{
	  float damage_ratio = static_cast<float>(event.damageAmount) / static_cast<float>(stats->GetMaxHP());
	  if (damage_ratio >= 0.5)
		size = { 2.5, 2.5 };
	  else if (damage_ratio >= 0.33)
		size = { 2.0, 2.0 };
	  else if (damage_ratio >= 0.2)
		size = { 1.5, 1.5 };
	  else if (damage_ratio >= 0.1)
		size = { 1.2, 1.2 };
	}
  }
  Math::vec2 text_position = event.target->GetGridPosition()->Get();
  text_position *= GridSystem::TILE_SIZE;

  m_ui_manager->ShowDamageText(event.damageAmount, text_position, size);
}

// Math::ivec2 ConvertScreenToGrid(Math::vec2 world_pos)
// {
// 	int grid_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
// 	int grid_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
// 	return { grid_x, grid_y };
// }

void GamePlay::Update(double dt)
{
  TurnManager*				turnMgr		 = GetGSComponent<TurnManager>();
  GridSystem*				grid		 = GetGSComponent<GridSystem>();
  CombatSystem*				combatSystem = GetGSComponent<CombatSystem>();
  AISystem*					aiSystem	 = GetGSComponent<AISystem>();
  CS230::GameObjectManager* goMgr		 = GetGSComponent<CS230::GameObjectManager>();
  DebugManager*				debugMgr	 = GetGSComponent<DebugManager>();

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

  if (game_end || Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape))
  {
	if (turnMgr)
	  turnMgr->EndCombat();
	Engine::GetGameStateManager().PopState();
	Engine::GetGameStateManager().PushState<MainMenu>();
	return;
  }
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
	  Engine::GetLogger().LogEvent("UI: 'Cancel Move' button clicked.");
	}
	else
	{
	  m_input_handler->SetState(ActionState::SelectingMove);
	  Engine::GetLogger().LogEvent("UI: 'Move' button clicked.");
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
