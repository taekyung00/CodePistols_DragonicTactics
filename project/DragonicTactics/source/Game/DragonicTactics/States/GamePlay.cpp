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
#include "OpenGL/Environment.h"

#include "Game/MainMenu.h"

#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/Objects/Cleric.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"

#include "Engine/Camera.h"
#include "Engine/SoundManager.h"
#include "Game/DragonicTactics/Factories/CharacterFactory.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/StateComponents/AISystem.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/DataRegistry.h"
#include "Game/DragonicTactics/StateComponents/DiceManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/StateComponents/MapDataRegistry.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"
#include "Game/DragonicTactics/StateComponents/StatusEffectHandler.h"
#include "Game/DragonicTactics/StateComponents/TurnManager.h"

#include "../Debugger/DebugManager.h"

#include "BattleOrchestrator.h"
#include "GamePlayUIManager.h"
#include "PlayerInputHandler.h"

#include "Game/Particles.h"
#include "./Engine/Particle.h"

int  GamePlay::s_next_map_index = 0;
bool GamePlay::s_should_restart = false;

namespace
{
  const char* SfxActionFor(CharacterTypes t)
  {
    switch (t)
    {
      case CharacterTypes::Dragon:  return SoundManager::SFX_DRAGON_ACTION;
      case CharacterTypes::Fighter: return SoundManager::SFX_FIGHTER_ACTION;
      case CharacterTypes::Cleric:  return SoundManager::SFX_CLERIC_ACTION;
      default:                      return nullptr;
    }
  }

  const char* SfxHurtFor(CharacterTypes t)
  {
    switch (t)
    {
      case CharacterTypes::Dragon:  return SoundManager::SFX_DRAGON_HURT;
      case CharacterTypes::Fighter: return SoundManager::SFX_FIGHTER_HURT;
      case CharacterTypes::Cleric:  return SoundManager::SFX_CLERIC_HURT;
      default:                      return nullptr;
    }
  }
}

// Computes scale and letterbox offsets from actual window to virtual resolution
static void cam_virt_layout(Math::ivec2 actual, double& scale, double& ox, double& oy) noexcept
{
    scale = std::min(
        (double)actual.x / TacticalCamera::VIRTUAL_W,
        (double)actual.y / TacticalCamera::VIRTUAL_H);
    ox = (actual.x - TacticalCamera::VIRTUAL_W * scale) * 0.5;
    oy = (actual.y - TacticalCamera::VIRTUAL_H * scale) * 0.5;
}

Math::TransformationMatrix TacticalCamera::BuildVirtualNdc(Math::ivec2 win)
{
    double scale, ox, oy;
    cam_virt_layout(win, scale, ox, oy);
    double sx = 2.0 * scale / win.x;
    double sy = 2.0 * scale / win.y;
    double tx = 2.0 * ox / win.x - 1.0;
    double ty = 2.0 * oy / win.y - 1.0;
    return Math::TranslationMatrix(Math::vec2{ tx, ty })
         * Math::ScaleMatrix(Math::vec2{ sx, sy });
}

Math::TransformationMatrix TacticalCamera::GetWorldMatrix(Math::ivec2 win) const
{
    constexpr Math::vec2 vc = { VIRTUAL_W * 0.5, VIRTUAL_H * 0.5 };
    return BuildVirtualNdc(win)
        * Math::TranslationMatrix(vc)
        * Math::ScaleMatrix(Math::vec2{ zoom, zoom })
        * Math::TranslationMatrix(Math::vec2{ -target.x, -target.y });
}

Math::vec2 TacticalCamera::ScreenToWorld(Math::vec2 screen, Math::ivec2 win) const
{
    double scale, ox, oy;
    cam_virt_layout(win, scale, ox, oy);
    // actual → virtual
    Math::vec2 virt = { (screen.x - ox) / scale, (screen.y - oy) / scale };
    // virtual → world
    constexpr Math::vec2 vc = { VIRTUAL_W * 0.5, VIRTUAL_H * 0.5 };
    return {
        (virt.x - vc.x) / zoom + target.x,
        (virt.y - vc.y) / zoom + target.y
    };
}

Math::vec2 TacticalCamera::WorldToScreen(Math::vec2 world, [[maybe_unused]] Math::ivec2 win) const
{
    // Returns virtual-resolution coordinates (1600x900 space)
    constexpr Math::vec2 vc = { VIRTUAL_W * 0.5, VIRTUAL_H * 0.5 };
    return {
        (world.x - target.x) * zoom + vc.x,
        (world.y - target.y) * zoom + vc.y
    };
}

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
  m_ui_manager->InitButtons(m_input_handler.get());

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
  AddGSComponent(new SpellSystem());
  AddGSComponent(new StatusEffectHandler());
//   AddGSComponent(new CS230::Camera());
  AddGSComponent(new CS230::ParticleManager<Particles::Hit>());


  GetGSComponent<EventBus>()->Clear();
  GetGSComponent<DiceManager>()->SetSeed(100);
  GetGSComponent<DebugManager>()->Init();
  GetGSComponent<CombatSystem>()->SetDiceManager(GetGSComponent<DiceManager>());
  GetGSComponent<DataRegistry>()->LoadFromFile("Assets/Data/characters.json");
  GetGSComponent<DataRegistry>()->LoadAllCharacterData("Assets/Data/characters.json");
  GetGSComponent<SpellSystem>()->LoadFromCSV("Assets/Data/spell_table.csv");
  // GetGSComponent<SpellSystem>()->SetEventBus(GetGSComponent<EventBus>());

  selected_json_map_index_ = s_next_map_index;

  auto* map_registry = GetGSComponent<MapDataRegistry>();
  map_registry->LoadMaps("Assets/Data/maps.json");
  available_json_maps_ = map_registry->GetAllMapIds();

  Engine::GetLogger().LogEvent("Available maps: " + std::to_string(available_json_maps_.size()));

  if (available_json_maps_.empty())
  {
	Engine::GetLogger().LogError("No maps loaded from maps.json - returning to MainMenu");
	Engine::GetGameStateManager().PopState();
	Engine::GetGameStateManager().PushState<MainMenu>();
	return;
  }

  if (selected_json_map_index_ < 0 || selected_json_map_index_ >= static_cast<int>(available_json_maps_.size()))
  {
	Engine::GetLogger().LogError("Invalid map index " + std::to_string(selected_json_map_index_) + ", defaulting to 0");
	selected_json_map_index_ = 0;
	s_next_map_index		 = 0;
  }

  const std::string& selected_map_id = available_json_maps_[static_cast<std::size_t>(selected_json_map_index_)];
  Engine::GetLogger().LogEvent("Loading map: " + selected_map_id);
  LoadJSONMap(selected_map_id);

  if (player == nullptr || enemys.empty())
  {
	Engine::GetLogger().LogError("LoadJSONMap failed to spawn characters - returning to MainMenu");
	Engine::GetGameStateManager().PopState();
	Engine::GetGameStateManager().PushState<MainMenu>();
	return;
  }

  // Init tactical camera centered on the grid
  {
    auto* gs = GetGSComponent<GridSystem>();
    if (gs)
    {
      m_camera.target = {
        gs->GetWidth()  * static_cast<double>(GridSystem::TILE_SIZE) * 0.5,
        gs->GetHeight() * static_cast<double>(GridSystem::TILE_SIZE) * 0.5
      };
    }
    m_camera.zoom = 1.0;
    m_ui_manager->SetCamera(&m_camera);
  }

  // UI Manager에 캐릭터 등록
  std::vector<Character*> all_characters = { player };
  all_characters.insert(all_characters.end(), enemys.begin(), enemys.end());
  m_ui_manager->SetCharacters(all_characters);
  Engine::GetLogger().LogEvent("GamePlay::Load - Characters registered to UI Manager");

  // EventBus 구독을 StartCombat() 전에 등록 — 첫 TurnStartedEvent를 놓치지 않기 위함
  GetGSComponent<EventBus>()->Subscribe<TurnStartedEvent>(
	  [this](const TurnStartedEvent& e)
	  {
		if (e.character)
		  m_ui_manager->OnTurnStarted(e.character->TypeName(), e.turnNumber);
	  });

  GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>(
	  [this](const CharacterDamagedEvent& event)
	  {
		this->DisplayDamageAmount(event);
		std::string att = event.attacker ? event.attacker->TypeName() : "Lava";
		m_ui_manager->AddBattleLogEntry(
		  att + "->" + event.target->TypeName()
		  + " " + std::to_string(event.damageAmount) + "dmg"
		  + " (HP:" + std::to_string(event.remainingHP) + ")");

		if (event.target)
		{
		  if (const char* sfx = SfxHurtFor(event.target->GetCharacterType()))
			Engine::GetSoundManager().PlaySFX(sfx);
		}
	  });

  GetGSComponent<EventBus>()->Subscribe<CharacterAttackedEvent>(
	  []([[maybe_unused]] const CharacterAttackedEvent& event)
	  {
		if (event.attacker)
		{
		  if (const char* sfx = SfxActionFor(event.attacker->GetCharacterType()))
			Engine::GetSoundManager().PlaySFX(sfx);
		}
		// 미스 시 hurt 보조 — 적중 시는 CharacterDamagedEvent가 처리하므로 중복 방지
		if (event.damageAmount == 0 && event.defender)
		{
		  if (const char* sfx = SfxHurtFor(event.defender->GetCharacterType()))
			Engine::GetSoundManager().PlaySFX(sfx);
		}
	  });

  GetGSComponent<EventBus>()->Subscribe<SpellCastEvent>(
	  [this](const SpellCastEvent& event)
	  {
		if (event.caster)
		{
		  m_ui_manager->AddBattleLogEntry(
			event.caster->TypeName() + " cast " + event.spellName
			+ " Lv." + std::to_string(event.spellLevel));

		  if (const char* sfx = SfxActionFor(event.caster->GetCharacterType()))
			Engine::GetSoundManager().PlaySFX(sfx);
		}
	  });

  GetGSComponent<EventBus>()->Subscribe<CharacterDeathEvent>(
	  [this](const CharacterDeathEvent& event)
	  {
		// goMgr->UpdateAll()이 메모리를 해제하기 전에 즉시 처리
		// (dangling pointer use-after-free 방지)
		if (event.character)
		  m_confirmed_dead_.insert(event.character);

		if (auto* turnMgr = GetGSComponent<TurnManager>())
		  turnMgr->RemoveFromTurnOrder(event.character);

		this->CheckGameEnd(event);
		if (event.character)
		  m_ui_manager->AddBattleLogEntry(event.character->TypeName() + " died!");
	  });

  GetGSComponent<EventBus>()->Subscribe<CharacterHealedEvent>(
	  [this](const CharacterHealedEvent& e)
	  {
		std::string src  = e.healer ? e.healer->TypeName() + "->" : "";
		std::string line = src + e.target->TypeName()
		                 + " +" + std::to_string(e.healAmount) + "HP"
		                 + " (" + std::to_string(e.currentHP) + "/"
		                 + std::to_string(e.maxHP) + ")";
		m_ui_manager->AddBattleLogEntry(line);
	  });

  TurnManager* turnMgr = GetGSComponent<TurnManager>();
  turnMgr->SetEventBus(GetGSComponent<EventBus>());
  std::vector<Character*> turn_order = { player };
  turn_order.insert(turn_order.end(), enemys.begin(), enemys.end());
  turnMgr->InitializeTurnOrder(turn_order);
  turnMgr->StartCombat();

  GetGSComponent<EventBus>()->Subscribe<CharacterEscapedEvent>(
	  [this]([[maybe_unused]] const CharacterEscapedEvent& event)
	  {
		this->game_end	= true;
		std::string msg = "Game Over: ";
		msg += event.character->TypeName();
		msg += " has escaped.";
		Engine::GetLogger().LogDebug(msg);
	  });

  Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/SFX_test.wav");

  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_DRAGON_ACTION);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_DRAGON_HURT);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_DRAGON_WALK);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_FIGHTER_ACTION);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_FIGHTER_HURT);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_CLERIC_ACTION);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_CLERIC_HURT);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_HUMAN_WALK);

  Engine::GetSoundManager().LoadBGM("Assets/Audio/BGM/BGM_test.ogg");
  Engine::GetSoundManager().PlayBGM("Assets/Audio/BGM/BGM_test.ogg");
}


void GamePlay::DisplayDamageAmount(const CharacterDamagedEvent& event)
{
  if (event.target == nullptr) return;
  Math::vec2 size = { 1.0, 1.0 };
  const StatsComponent* stats = event.target->GetStatsComponent();
  if (stats != nullptr && stats->GetMaxHP() > 0)
  {
    float ratio = static_cast<float>(event.damageAmount) / static_cast<float>(stats->GetMaxHP());
    if      (ratio >= 0.5f)  size = { 2.5, 2.5 };
    else if (ratio >= 0.33f) size = { 2.0, 2.0 };
    else if (ratio >= 0.2f)  size = { 1.5, 1.5 };
    else if (ratio >= 0.1f)  size = { 1.2, 1.2 };
  }
  Math::ivec2 grid_pos = event.target->GetGridPosition()->Get();
  Math::vec2 text_position = {
      grid_pos.x * (double)GridSystem::TILE_SIZE,
      grid_pos.y * (double)GridSystem::TILE_SIZE + GridSystem::TILE_SIZE
  };
  m_ui_manager->ShowDamageText(event.damageAmount, text_position, size);
}

void GamePlay::CheckGameEnd(const CharacterDeathEvent& event)
{
  if (event.character == player)
  {
	m_ui_manager->ShowGameEnd("Invader Win");
	game_end = true;
	return;
  }

  bool all_enemies_dead = std::all_of(enemys.begin(), enemys.end(),
	[this](Character* c) { return c == nullptr || m_confirmed_dead_.count(c) > 0; });
  if (all_enemies_dead && !enemys.empty())
  {
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

  // Camera pan (right-drag) and zoom (scroll wheel) — runs every frame
  {
    auto&      inp    = Engine::GetInput();
    auto       win    = Engine::GetWindow().GetSize();
    Math::vec2 mouse  = inp.GetMousePos();

    if (inp.MouseDown(2) && !ImGui::GetIO().WantCaptureMouse)
    {
      if (m_right_mouse_was_down)
      {
        Math::vec2 world_prev = m_camera.ScreenToWorld(m_prev_mouse, win);
        Math::vec2 world_curr = m_camera.ScreenToWorld(mouse, win);
        m_camera.target.x -= world_curr.x - world_prev.x;
        m_camera.target.y -= world_curr.y - world_prev.y;
      }
      m_right_mouse_was_down = true;
    }
    else
    {
      m_right_mouse_was_down = false;
    }
    m_prev_mouse = mouse;

    double scroll = inp.GetMouseScroll();
    if (scroll != 0.0 && !ImGui::GetIO().WantCaptureMouse)
    {
      Math::vec2 wb = m_camera.ScreenToWorld(mouse, win);
      m_camera.zoom *= (1.0 + scroll * 0.125);
      if (m_camera.zoom < TacticalCamera::ZOOM_MIN) m_camera.zoom = TacticalCamera::ZOOM_MIN;
      if (m_camera.zoom > TacticalCamera::ZOOM_MAX) m_camera.zoom = TacticalCamera::ZOOM_MAX;
      Math::vec2 wa = m_camera.ScreenToWorld(mouse, win);
      m_camera.target.x -= wa.x - wb.x;
      m_camera.target.y -= wa.y - wb.y;
    }
  }

  TurnManager*				turnMgr		 = GetGSComponent<TurnManager>();
  GridSystem*				grid		 = GetGSComponent<GridSystem>();
  CombatSystem*				combatSystem = GetGSComponent<CombatSystem>();
  AISystem*					aiSystem	 = GetGSComponent<AISystem>();
  CS230::GameObjectManager* goMgr		 = GetGSComponent<CS230::GameObjectManager>();
  DebugManager*				debugMgr	 = GetGSComponent<DebugManager>();

  if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape))
  {
	if (turnMgr)
	  turnMgr->EndCombat();
	Engine::GetGameStateManager().PopState();
	Engine::GetGameStateManager().PushState<MainMenu>();
	return;
  }

// 수정됨: if (game_end) return; 를 여기서 바로 호출하지 않습니다.

    double scaledDt = dt * debugMgr->timeScale;

    // 1. 게임이 끝나더라도 메모리 해제(Destroy 처리)와 파티클, UI 갱신을 위해 기본 시스템 업데이트는 계속 실행합니다.
    if (goMgr) goMgr->UpdateAll(scaledDt);
    if (m_ui_manager) m_ui_manager->Update(dt);
    UpdateGSComponents(scaledDt);

    // 2. 파괴 처리를 완료한 후, 게임이 끝났다면 여기서 끊어줍니다. (추가 조작 및 AI 턴 진행 방지)
    if (game_end) return; 

    // 3. 게임이 진행 중일 때만 플레이어 조작 및 전투 흐름(Orchestrator) 로직을 실행합니다.
    Character* current = nullptr;
    if (turnMgr && turnMgr->IsCombatActive())
    {
        current = turnMgr->GetCurrentCharacter();
    }

    if (current != nullptr)
    {
        m_input_handler->Update(scaledDt, current, grid, combatSystem, m_ui_manager->GetButtons(), &m_camera);
    }
    m_orchestrator->Update(scaledDt, turnMgr, aiSystem);
}

void GamePlay::Unload()
{
  Engine::GetSoundManager().StopBGM();
  
  if (auto goMgr = GetGSComponent<CS230::GameObjectManager>())
  {
	goMgr->Unload();
  }

  ClearGSComponents();

  m_input_handler.reset();
  m_ui_manager.reset();
  m_orchestrator.reset();


  enemys.clear();
  m_confirmed_dead_.clear();
  player = nullptr;
}

void GamePlay::Draw()
{
  Engine::GetWindow().Clear(0x1a1a1aff);
  auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();
  auto win          = Engine::GetWindow().GetSize();

  // Pass 1: World space — grid, characters, debug (camera transform applied)
  renderer_2d->BeginScene(m_camera.GetWorldMatrix(win));

  GridSystem* grid_system = GetGSComponent<GridSystem>();
  if (grid_system != nullptr)
    grid_system->Draw();

  CS230::GameObjectManager* goMgr = GetGSComponent<CS230::GameObjectManager>();
  if (goMgr)
    goMgr->DrawAll(Math::TransformationMatrix{});

  GetGSComponent<DebugManager>()->Draw(grid_system);

  renderer_2d->EndScene();

  // Pass 2: UI — virtual 1600x900 coordinates, letterboxed to actual window
  Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
  // Save ui_ndc so EndRenderTextureMode (triggered by font cache misses) restores it correctly
  Engine::GetTextureManager().SaveCurrentScene(ui_ndc);
  renderer_2d->BeginScene(ui_ndc);
  m_ui_manager->Draw(ui_ndc);
  renderer_2d->EndScene();
}

void GamePlay::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
  GridSystem* grid_system = GetGSComponent<GridSystem>();
  GetGSComponent<DebugManager>()->DrawImGui(grid_system);

  ImGui::Begin("Map Selection");

  if (selected_json_map_index_ >= 0 && selected_json_map_index_ < static_cast<int>(available_json_maps_.size()))
  {
	ImGui::Text("Current Map: %s", available_json_maps_[static_cast<std::size_t>(selected_json_map_index_)].c_str());
  }

  ImGui::Separator();
  ImGui::Text("Maps:");

  for (int i = 0; i < static_cast<int>(available_json_maps_.size()); ++i)
  {
	const std::string& map_id = available_json_maps_[static_cast<std::size_t>(i)];

	bool is_selected = (s_next_map_index == i);
	if (is_selected)
	{
	  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
	}

	if (ImGui::Button(map_id.c_str()))
	{
	  s_next_map_index = i;
	  Engine::GetLogger().LogEvent("Selected map: " + map_id + " (click Restart to apply)");
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
#endif // DEVELOPER_VERSION
}

gsl::czstring GamePlay::GetName() const
{
  return "GamePlay";
}

void GamePlay::LoadJSONMap(const std::string& map_id)
{
  Engine::GetLogger().LogEvent("LoadJSONMap - BEGIN: " + map_id);

  CS230::GameObjectManager* go_manager		  = GetGSComponent<CS230::GameObjectManager>();
  GridSystem*				grid_system		  = GetGSComponent<GridSystem>();
  CharacterFactory*			character_factory = GetGSComponent<CharacterFactory>();
  MapDataRegistry*			map_registry	  = GetGSComponent<MapDataRegistry>();

  MapData map_data = map_registry->GetMapData(map_id);

  if (map_data.id.empty())
  {
	Engine::GetLogger().LogError("Failed to load map: " + map_id);
	return;
  }

  grid_system->LoadMap(map_data);

  auto dragon_it  = map_data.spawn_points.find("dragon");
  auto fighter_it = map_data.spawn_points.find("fighter");
  if (map_data.has_exit)
  {
	if (dragon_it != map_data.spawn_points.end() && dragon_it->second == map_data.exit_position)
	{
	  Engine::GetLogger().LogError("WARNING: dragon spawn overlaps exit at (" +
		std::to_string(map_data.exit_position.x) + "," + std::to_string(map_data.exit_position.y) + ")");
	}
	if (fighter_it != map_data.spawn_points.end() && fighter_it->second == map_data.exit_position)
	{
	  Engine::GetLogger().LogError("WARNING: fighter spawn overlaps exit at (" +
		std::to_string(map_data.exit_position.x) + "," + std::to_string(map_data.exit_position.y) + ")");
	}
  }

  auto dragon_spawn_it = map_data.spawn_points.find("dragon");
  if (dragon_spawn_it != map_data.spawn_points.end())
  {
	Math::ivec2 dragon_spawn = dragon_spawn_it->second;
	auto		player_ptr	 = character_factory->Create(CharacterTypes::Dragon, dragon_spawn);
	player					 = player_ptr.get();
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
	auto  enemy_ptr   = character_factory->Create(CharacterTypes::Fighter, fighter_spawn);
	auto* fighter_raw = enemy_ptr.get();
	fighter_raw->SetGridSystem(grid_system);
	go_manager->Add(std::move(enemy_ptr));
	grid_system->AddCharacter(fighter_raw, fighter_spawn);
	enemys.push_back(fighter_raw);
	Engine::GetLogger().LogEvent("Fighter spawned at: " + std::to_string(fighter_spawn.x) + ", " + std::to_string(fighter_spawn.y));
  }
  else
  {
	Engine::GetLogger().LogError("No fighter spawn point in map: " + map_id);
  }

  // Cleric
  auto cleric_spawn_it = map_data.spawn_points.find("cleric");
  if (cleric_spawn_it != map_data.spawn_points.end())
  {
	Math::ivec2 cleric_spawn = cleric_spawn_it->second;
	auto  cleric_ptr = character_factory->Create(CharacterTypes::Cleric, cleric_spawn);
	auto* cleric_raw = cleric_ptr.get();
	cleric_raw->SetGridSystem(grid_system);
	go_manager->Add(std::move(cleric_ptr));
	grid_system->AddCharacter(cleric_raw, cleric_spawn);
	enemys.push_back(cleric_raw);
	Engine::GetLogger().LogEvent("Cleric spawned at: " + std::to_string(cleric_spawn.x) + ", " + std::to_string(cleric_spawn.y));
  }

  Engine::GetLogger().LogEvent("LoadJSONMap - END: " + map_data.name);
}
