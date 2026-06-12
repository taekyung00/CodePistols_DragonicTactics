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
#include "Engine/Path.h"
#include "Engine/Timer.h"
#include "GamePlay.h"
#include "OpenGL/Environment.h"

#include "Game/GameOver.h"
#include "Game/LevelSelect.h"
#include "Game/MainMenu.h"

#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/Objects/Cleric.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/Objects/Wizard.h"

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
#include "Game/DragonicTactics/Types/GameTimings.h"
#include "Game/GameCursor.h"

#include "Game/Particles.h"
#include "./Engine/Particle.h"

std::string              GamePlay::s_next_map_id    = "first_map";
bool                     GamePlay::s_should_restart = false;
int                      GamePlay::s_level_id       = 0;
std::vector<std::string> GamePlay::s_allowed_spells = {};

// 타이밍 별칭 (GameTimings.h 값을 file-scope 상수로 노출)
static constexpr double SPELL_PRE_IMPACT_SEC = GameTimings::SPELL_PRE_IMPACT;
static constexpr double EFFECT_LEAD_TIME     = GameTimings::EFFECT_LEAD;
static constexpr double PROJ_DURATION        = GameTimings::PROJ_DURATION;
static constexpr double AI_HIT_DELAY         = GameTimings::ATTACK_APPLY;
static constexpr double SPELL_APPLY_SEC      = GameTimings::SPELL_APPLY;
static constexpr double SLOW_SPELL_EXTRA     = GameTimings::SLOW_SPELL_EXTRA; // Fire/MagicMissile/Meteor SFX 스펠

// 스프라이트 이펙트 상수
static constexpr int    HIT2_FRAMES   = 5;   // Hit2.png  160×32, 32px/frame
static constexpr int    HIT3_FRAMES   = 6;   // Hit3.png  192×32, 32px/frame
static constexpr int    HIT4_FRAMES   = 4;   // Hit4.png  128×32, 32px/frame
static constexpr int    PURIFY_FRAMES = 6;   // Purify_spt.png 시각 추정
static constexpr int    METEOR_FRAMES = 17;  // Meteor.png 수평 스트립 1600×900 × 17프레임
static constexpr int    MAGIC_FRAMES  = 8;   // Magic.png  시각 추정
static constexpr int    CRY_FRAMES      = 5;   // cry.png    640×128, 128px/frame
static constexpr int    MAGICHIT_FRAMES = 4;   // MagicHit.png 128×32, 32px/frame
static constexpr double SPRITE_FPS      = 10.0;
static constexpr double EFFECT_SCALE    = 1.5; // 이펙트 크기: TILE_SIZE × EFFECT_SCALE px

namespace
{
  std::string GetSpellSFX(const std::string& spellName) {
        if (spellName == "Divine Shield" || spellName == "Healing Touch" ||
            spellName == "Teleport" || spellName == "Mana Conversion" || spellName == "Purify") 
            return "Assets/Audio/SFX/spell/Arcane.wav";
        if (spellName == "Curse of Suffering") return "Assets/Audio/SFX/spell/Curse.wav";
        if (spellName == "Fire Bolt" || spellName == "Dragon's Fury") return "Assets/Audio/SFX/spell/Fire.wav";
        if (spellName == "Gale Step" || spellName == "Shadow Hide") return "Assets/Audio/SFX/spell/Gale Step.wav";
        if (spellName == "Magma Blast") return "Assets/Audio/SFX/spell/Lava Creation.wav";
        if (spellName == "Magic Missile") return "Assets/Audio/SFX/spell/Magic Missile.wav";
        if (spellName == "Meteor") return "Assets/Audio/SFX/spell/Meteor.wav";
        if (spellName == "Smite" || spellName == "Tail Swipe") return "Assets/Audio/SFX/spell/Smite.wav";
        if (spellName == "Wall Creation") return "Assets/Audio/SFX/spell/Wall Creation.wav";
        if (spellName == "Weakpoint Strike") return "Assets/Audio/SFX/spell/Weakpoint Strike.wav";
        return ""; 
    }

  const char* SfxActionFor(CharacterTypes t)
  {
    switch (t)
    {
      case CharacterTypes::Dragon:  return SoundManager::SFX_DRAGON_ACTION;
      case CharacterTypes::Fighter: return SoundManager::SFX_FIGHTER_ACTION;
      case CharacterTypes::Cleric:  return SoundManager::SFX_CLERIC_ACTION;
      case CharacterTypes::Rogue:   return SoundManager::SFX_ROGUE_ACTION;
      case CharacterTypes::Wizard:  return SoundManager::SFX_WIZARD_ACTION;
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
      case CharacterTypes::Rogue:   return SoundManager::SFX_ROGUE_HURT;
      case CharacterTypes::Wizard:  return SoundManager::SFX_WIZARD_HURT;
      default:                      return nullptr;
    }
  }
}

// Computes scale and letterbox offsets from actual window to virtual resolution
static void cam_virt_layout(Math::ivec2 actual, double& scale, double& ox, double& oy) noexcept
{
    scale = std::min(
        static_cast<double>(actual.x) / TacticalCamera::VIRTUAL_W,
        static_cast<double>(actual.y) / TacticalCamera::VIRTUAL_H);
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

Math::vec2 TacticalCamera::ScreenToVirtual(Math::vec2 screen, Math::ivec2 win)
{
    double scale, ox, oy;
    cam_virt_layout(win, scale, ox, oy);
    return { (screen.x - ox) / scale, (screen.y - oy) / scale };
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
  // if (!OpenGL::IsWebGL)
  // {
	// Engine::GetWindow().SetWindowPosition(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  // }
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
  m_ui_manager->InitSpellTooltips();
  m_ui_manager->InitStatusEffectIcons();
  // GetGSComponent<SpellSystem>()->SetEventBus(GetGSComponent<EventBus>());

  if (s_level_id > 0)
  {
	Engine::GetLogger().LogEvent("Loading level map: " + std::to_string(s_level_id));
	LoadLevelMap(s_level_id);
  }
  else
  {
	s_allowed_spells.clear();  // 레벨 모드에서 남은 스펠 제한 초기화
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

	// Resolve s_next_map_id → index in available_json_maps_
	selected_json_map_index_ = -1;
	for (int i = 0; i < static_cast<int>(available_json_maps_.size()); ++i)
	{
	  if (available_json_maps_[static_cast<std::size_t>(i)] == s_next_map_id)
	  {
		selected_json_map_index_ = i;
		break;
	  }
	}
	if (selected_json_map_index_ < 0)
	{
	  Engine::GetLogger().LogError("Map id '" + s_next_map_id + "' not found, defaulting to first available");
	  selected_json_map_index_ = 0;
	  s_next_map_id            = available_json_maps_[0];
	}

	const std::string& selected_map_id = available_json_maps_[static_cast<std::size_t>(selected_json_map_index_)];
	Engine::GetLogger().LogEvent("Loading map: " + selected_map_id);
	LoadJSONMap(selected_map_id);
  }

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
  m_ui_manager->SetPlayer(player);
  Engine::GetLogger().LogEvent("GamePlay::Load - Characters registered to UI Manager");

  // EventBus 구독을 StartCombat() 전에 등록 — 첫 TurnStartedEvent를 놓치지 않기 위함
  GetGSComponent<EventBus>()->Subscribe<TurnStartedEvent>(
	  [this](const TurnStartedEvent& e)
	  {
		if (e.character)
		{
		  int round = 1;
		  if (auto* tm = GetGSComponent<TurnManager>())
			round = tm->GetRoundNumber();
		  m_ui_manager->OnTurnStarted(e.character->TypeName(), e.turnNumber,
		                               !e.character->IsAIControlled(), round);
		}

		// 레벨3: 라운드 첫 번째 턴 시작 시 50% 확률로 랜덤 빈 타일에 용암 생성
		if (s_level_id == 3 && e.character)
		{
		  auto* tm = GetGSComponent<TurnManager>();
		  if (tm)
		  {
			int current_round = tm->GetRoundNumber();
			if (current_round != m_lava_spawn_last_round_)
			{
			  m_lava_spawn_last_round_ = current_round;
			  if (std::rand() % 2 == 0) // 50% 확률
			  {
				auto* grid   = GetGSComponent<GridSystem>();
				auto* spells = GetGSComponent<SpellSystem>();
				if (grid && spells)
				{
				  std::vector<Math::ivec2> candidates;
				  for (int y = 0; y < grid->GetHeight(); ++y)
					for (int x = 0; x < grid->GetWidth(); ++x)
					{
					  Math::ivec2 pos{ x, y };
					  if (grid->GetTileType(pos) == GridSystem::TileType::Empty && !grid->IsOccupied(pos))
						candidates.push_back(pos);
					}
				  if (!candidates.empty())
				  {
					Math::ivec2 chosen = candidates[static_cast<size_t>(std::rand()) % candidates.size()];
					spells->SpawnEnvironmentalLava(chosen, current_round);
					GetGSComponent<EventBus>()->Publish(UINoticeEvent{ "Lava erupts!" });
				  }
				}
			  }
			}
		  }
		}
	  });

  GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>(
	  [this](const CharacterDamagedEvent& event)
	  {
		// 용암 피해(attacker==nullptr)는 SFX가 없으므로 즉시 표시
		double delay = event.attacker ? m_pending_damage_delay_ : 0.0;
		this->DisplayDamageAmount(event, delay);

		// 사망 캐릭터 소멸 타이밍: hurt SFX가 끝난 뒤 사라지도록 딜레이 설정
		if (!event.target->IsAlive())
		{
		  const char* hurt_sfx = SfxHurtFor(event.target->GetCharacterType());
		  double      sfx_dur  = hurt_sfx ? Engine::GetSoundManager().GetSFXDuration(hurt_sfx) : 0.0;
		  event.target->SetDeathDelay(delay + sfx_dur);
		}
		std::string att = event.attacker ? event.attacker->TypeName() : "Lava";
		m_ui_manager->AddBattleLogEntry(
		  att + "->" + event.target->TypeName()
		  + " " + std::to_string(event.damageAmount) + "dmg"
		  + " (HP:" + std::to_string(event.remainingHP) + ")");

		if (event.target)
		{
		  // 데미지 텍스트와 동일한 딜레이 후 재생 → 텍스트·피격음 동시 등장
		  if (const char* sfx = SfxHurtFor(event.target->GetCharacterType()))
			Engine::GetSoundManager().PlaySFXDelayed(sfx, delay);

		  // 셰이크·파티클도 동일 딜레이로 큐 등록 (위치는 지금 캡처)
		  Math::vec2 hit_pos = event.target->GetPosition()
		                     + Math::vec2{ GridSystem::TILE_SIZE / 2.0, GridSystem::TILE_SIZE / 2.0 };
		  m_pending_hit_effects_.push_back({ event.target, hit_pos, delay });
		}
	  });

  GetGSComponent<EventBus>()->Subscribe<CharacterAttackedEvent>(
	  [this]([[maybe_unused]] const CharacterAttackedEvent& event)
	  {
		if (event.attacker)
		{
		  if (const char* sfx = SfxActionFor(event.attacker->GetCharacterType()))
		  {
			Engine::GetSoundManager().PlaySFX(sfx);
			// AI 공격: AttackDelayObject가 0.3초 뒤에 CharacterDamagedEvent 발행 → 추가 딜레이 불필요
			// 플레이어 공격: SFX 길이 기반 딜레이
			if (event.attacker->IsAIControlled())
			  m_pending_damage_delay_ = 0.0;
			else
			  m_pending_damage_delay_ = std::max(0.0, Engine::GetSoundManager().GetSFXDuration(sfx) - EFFECT_LEAD_TIME);
		  }
		}
		// 미스 시 hurt 보조 — 적중 시는 CharacterDamagedEvent가 처리하므로 중복 방지
		if (event.damageAmount == 0 && event.defender)
		{
		  if (const char* sfx = SfxHurtFor(event.defender->GetCharacterType()))
			Engine::GetSoundManager().PlaySFX(sfx);
		}

		// 스프라이트 이펙트: Dragon 기본공격 → Hit2, AI 기본공격 → Hit4
		if (event.attacker && event.defender)
		{
		  Math::vec2 hit_pos = event.defender->GetPosition()
		                     + Math::vec2{ GridSystem::TILE_SIZE * 0.5, GridSystem::TILE_SIZE * 0.5 };
		  SpriteEffect fx;
		  if (!event.attacker->IsAIControlled())
		  {
			fx.tex         = m_tex_hit2_;
			fx.frame_count = HIT2_FRAMES;
			fx.delay       = m_pending_damage_delay_;
		  }
		  else
		  {
			fx.tex         = m_tex_hit4_;
			fx.frame_count = HIT4_FRAMES;
			fx.delay       = AI_HIT_DELAY;
		  }
		  fx.fps       = SPRITE_FPS;
		  fx.world_pos = hit_pos;
		  m_sprite_effects_.push_back(fx);
		}
	  });

  // 리드미에 있는 추가 스펠 사운드 로드
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Arcane.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Curse.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Fire.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Gale Step.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Lava Creation.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Magic Missile.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Meteor.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Smite.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Wall Creation.wav");
    Engine::GetSoundManager().LoadSFX("Assets/Audio/SFX/spell/Weakpoint Strike.wav");

    // SpellCastEvent 구독 내용 수정
    GetGSComponent<EventBus>()->Subscribe<SpellCastEvent>(
        [this](const SpellCastEvent& event) {
            if (event.caster) {
                m_ui_manager->AddBattleLogEntry(event.caster->TypeName() + " cast " + event.spellName + " Lv." + std::to_string(event.spellLevel));
                
                // 스펠 전용 사운드가 있다면 재생
                std::string sfxPath = GetSpellSFX(event.spellName);
                
                // 🔍 [디버깅] 스펠 이름과 재생할 사운드 경로를 콘솔에 출력!
                std::cout << "[SOUND DEBUG] Spell Name: [" << event.spellName << "] | " 
                          << "Path: [" << (sfxPath.empty() ? "Empty(Default Sound)" : sfxPath) << "]" << std::endl;

                // 스펠은 SpellDelayObject가 0.5초 후 데미지를 적용하므로
                // 그 시간만큼 차감한 나머지가 CharacterDamagedEvent 시점의 잔여 SFX 길이
                auto computeSpellDelay = [&](const std::string& path) {
                    double dur = Engine::GetSoundManager().GetSFXDuration(path);
                    m_pending_damage_delay_ = std::max(0.0, dur - SPELL_PRE_IMPACT_SEC - EFFECT_LEAD_TIME);
                };
                if (!sfxPath.empty()) {
                    Engine::GetSoundManager().PlaySFX(sfxPath.c_str());
                    computeSpellDelay(sfxPath);
                } else if (const char* sfx = SfxActionFor(event.caster->GetCharacterType())) {
                    Engine::GetSoundManager().PlaySFX(sfx);
                    computeSpellDelay(sfx);
                }

                // 스프라이트 이펙트: 스펠별 분기
                auto* grid_se = GetGSComponent<GridSystem>();
                if (grid_se)
                {
                    const std::string& sn = event.spellName;
                    Math::vec2 caster_center = event.caster->GetPosition()
                                             + Math::vec2{ GridSystem::TILE_SIZE * 0.5, GridSystem::TILE_SIZE * 0.5 };

                    if (sn == "Teleport" && m_tex_teleport_)
                    {
                        // Wizard 화면 렌더 크기 = 128px (wizard.spt: 256x256, Scale=0.5)
                        // Teleport.png: 1536x256, 6프레임 수평 스트립, 각 256x256
                        static constexpr double TELEPORT_SCALE = 128.0 / 256.0;
                        Math::vec2 target_center = {
                            static_cast<double>(event.targetGrid.x) * GridSystem::TILE_SIZE + GridSystem::TILE_SIZE * 0.5,
                            static_cast<double>(event.targetGrid.y) * GridSystem::TILE_SIZE + GridSystem::TILE_SIZE * 0.5
                        };
                        // 출발지: 역방향(구체화 해제) — Wizard 숨기기
                        event.caster->SetHideSprite(true);
                        {
                            SpriteEffect fx;
                            fx.tex          = m_tex_teleport_;
                            fx.frame_count  = TELEPORT_FRAMES;
                            fx.fps          = SPRITE_FPS;
                            fx.delay        = 0.0;
                            fx.world_pos    = caster_center;
                            fx.reverse      = true;
                            fx.custom_scale = TELEPORT_SCALE;
                            fx.hide_char    = event.caster; // 완료 시 hide 해제
                            m_sprite_effects_.push_back(fx);
                        }
                        // 도착지: 정방향(구체화) — 출발지와 동시에 시작
                        {
                            SpriteEffect fx;
                            fx.tex          = m_tex_teleport_;
                            fx.frame_count  = TELEPORT_FRAMES;
                            fx.fps          = SPRITE_FPS;
                            fx.delay        = 0.0;
                            fx.world_pos    = target_center;
                            fx.reverse      = false;
                            fx.custom_scale = TELEPORT_SCALE;
                            m_sprite_effects_.push_back(fx);
                        }
                    }
                    else if (sn == "Fire Bolt")
                    {
                        // 투사체: Dragon 위치 → targetGrid 방향으로 날아감
                        Math::vec2 target_center = {
                            static_cast<double>(event.targetGrid.x) * GridSystem::TILE_SIZE + GridSystem::TILE_SIZE * 0.5,
                            static_cast<double>(event.targetGrid.y) * GridSystem::TILE_SIZE + GridSystem::TILE_SIZE * 0.5
                        };
                        Math::vec2 dir = target_center - caster_center;
                        SpriteEffect fx;
                        fx.mode          = SpriteEffect::Mode::Projectile;
                        fx.tex           = m_tex_hit3_;
                        fx.frame_count   = HIT3_FRAMES;
                        fx.fps           = SPRITE_FPS;
                        fx.proj_origin   = caster_center;
                        fx.world_pos     = target_center;
                        fx.proj_duration = PROJ_DURATION + SLOW_SPELL_EXTRA;
                        fx.angle         = std::atan2(-dir.x, dir.y);
                        m_sprite_effects_.push_back(fx);
                    }
                    else if (sn == "Dragon's Fury")
                    {
                        // Line geometry: 시전자 중심에서 상하좌우 4방향으로 동시 발사
                        // 벽을 만나면 그 직전 타일까지만 날아감 (최대 4칸)
                        static constexpr double FURY_RANGE = 4.0;
                        struct DirAngle
                        {
                            Math::vec2 dir;
                            double     angle;
                        };
                        const DirAngle dirs[4] = {
                            { {  1.0,  0.0 }, -1.5707963267948966 }, // 오른쪽 (-π/2)
                            { { -1.0,  0.0 },  1.5707963267948966 }, // 왼쪽   (+π/2)
                            { {  0.0,  1.0 },  0.0                }, // 아래   (0, 스프라이트 기본)
                            { {  0.0, -1.0 },  3.14159265358979   }  // 위     (π)
                        };
                        for (const auto& d : dirs)
                        {
                            // 방향별 마지막 통과 가능 타일 계산
                            Math::vec2 end_pos = caster_center;
                            for (int step = 1; step <= static_cast<int>(FURY_RANGE); ++step)
                            {
                                Math::vec2 probe = caster_center + d.dir * (step * static_cast<double>(GridSystem::TILE_SIZE));
                                Math::ivec2 tile = {
                                    static_cast<int>(probe.x / GridSystem::TILE_SIZE),
                                    static_cast<int>(probe.y / GridSystem::TILE_SIZE)
                                };
                                GridSystem::TileType tt = grid_se ? grid_se->GetTileType(tile) : GridSystem::TileType::Empty;
                                if (tt == GridSystem::TileType::Wall || tt == GridSystem::TileType::Invalid)
                                    break;
                                end_pos = probe;
                            }
                            if (end_pos.x == caster_center.x && end_pos.y == caster_center.y)
                                continue; // 첫 타일이 벽 → 투사체 생략

                            SpriteEffect fx;
                            fx.mode          = SpriteEffect::Mode::Projectile;
                            fx.tex           = m_tex_hit3_;
                            fx.frame_count   = HIT3_FRAMES;
                            fx.fps           = SPRITE_FPS;
                            fx.proj_origin   = caster_center;
                            fx.world_pos     = end_pos;
                            fx.proj_duration = PROJ_DURATION + SLOW_SPELL_EXTRA;
                            fx.angle         = d.angle;
                            m_sprite_effects_.push_back(fx);
                        }
                    }
                    else if (sn == "Tail Swipe")
                    {
                        // AoE Around:2 — 범위 내 전원에 Hit4 (넉백 후 위치 추적)
                        Math::ivec2 caster_grid = event.caster->GetGridPosition()->Get();
                        for (auto* ch : grid_se->GetAllCharacters())
                        {
                            if (ch == nullptr || ch == event.caster) continue;
                            if (grid_se->ManhattanDistance(caster_grid, ch->GetGridPosition()->Get()) > 2) continue;
                            SpriteEffect fx;
                            fx.tex         = m_tex_hit4_;
                            fx.frame_count = HIT4_FRAMES;
                            fx.fps         = SPRITE_FPS;
                            fx.delay       = SPELL_APPLY_SEC;
                            fx.world_pos   = ch->GetPosition()
                                           + Math::vec2{ GridSystem::TILE_SIZE * 0.5, GridSystem::TILE_SIZE * 0.5 };
                            fx.follow_char = ch; // 넉백 완료 후 위치를 실시간 추적
                            m_sprite_effects_.push_back(fx);
                        }
                    }
                    else if (sn == "Purify")
                    {
                        SpriteEffect fx;
                        fx.tex          = m_tex_purify_;
                        fx.frame_count  = PURIFY_FRAMES;
                        fx.fps          = SPRITE_FPS;
                        fx.world_pos    = caster_center;
                        // Dragon 화면 크기(64px = TILE_SIZE) / Purify 프레임 크기(128px) = 0.5
                        fx.custom_scale = static_cast<double>(GridSystem::TILE_SIZE) / 128.0;
                        m_sprite_effects_.push_back(fx);
                    }
                    else if (sn == "Meteor")
                    {
                        m_meteor_active_  = true;
                        m_meteor_elapsed_ = 0.0;
                    }
                    else if (sn == "Fearful Cry")
                    {
                        // AoE Around:3 — 범위 내 적군(반대 팀)에게만 Cry
                        Math::ivec2 caster_grid = event.caster->GetGridPosition()->Get();
                        for (auto* ch : grid_se->GetAllCharacters())
                        {
                            if (ch == nullptr || ch == event.caster) continue;
                            // 같은 팀이면 Fear 대상이 아님
                            if (ch->IsAIControlled() == event.caster->IsAIControlled()) continue;
                            if (grid_se->ManhattanDistance(caster_grid, ch->GetGridPosition()->Get()) > 3) continue;
                            SpriteEffect fx;
                            fx.tex         = m_tex_cry_;
                            fx.frame_count = CRY_FRAMES;
                            fx.fps         = SPRITE_FPS;
                            fx.delay       = SPELL_APPLY_SEC;
                            fx.world_pos   = ch->GetPosition()
                                           + Math::vec2{ GridSystem::TILE_SIZE * 0.5, GridSystem::TILE_SIZE * 0.5 };
                            fx.follow_char = ch; // 딜레이 동안 이동해도 위치 추적
                            m_sprite_effects_.push_back(fx);
                        }
                    }
                    else if (sn == "Magic Missile")
                    {
                        Math::vec2 target_center = {
                            static_cast<double>(event.targetGrid.x) * GridSystem::TILE_SIZE + GridSystem::TILE_SIZE * 0.5,
                            static_cast<double>(event.targetGrid.y) * GridSystem::TILE_SIZE + GridSystem::TILE_SIZE * 0.5
                        };
                        SpriteEffect fx;
                        fx.tex         = m_tex_magic_hit_;
                        fx.frame_count = MAGICHIT_FRAMES;
                        fx.fps         = SPRITE_FPS;
                        fx.delay       = SPELL_APPLY_SEC;
                        fx.hold_time   = SLOW_SPELL_EXTRA;
                        fx.world_pos   = target_center;
                        m_sprite_effects_.push_back(fx);
                    }
                    else if (sn == "Divine Shield" || sn == "Healing Touch" || sn == "Teleport"
                             || sn == "Mana Conversion" || sn == "Curse of Suffering"
                             || sn == "Gale Step" || sn == "Shadow Hide"
                             || sn == "Magma Blast" || sn == "Wall Creation")
                    {
                        SpriteEffect fx;
                        fx.tex         = m_tex_magic_;
                        fx.frame_count = MAGIC_FRAMES;
                        fx.fps         = SPRITE_FPS;
                        fx.world_pos   = caster_center;
                        m_sprite_effects_.push_back(fx);
                    }
                }
            }
        });

  GetGSComponent<EventBus>()->Subscribe<UINoticeEvent>(
	  [this](const UINoticeEvent& event)
	  {
		m_ui_manager->ShowNotice(event.message);
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
		  m_ui_manager->AddBattleLogEntry(event.character->TypeName() + " retired!");
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
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_ROGUE_ACTION);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_ROGUE_HURT);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_WIZARD_ACTION);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_WIZARD_HURT);
  Engine::GetSoundManager().LoadSFX(SoundManager::SFX_HUMAN_WALK);

  Engine::GetSoundManager().LoadBGM("Assets/Audio/BGM/BGM_test.ogg");
  Engine::GetSoundManager().PlayBGM("Assets/Audio/BGM/BGM_test.ogg");

  // 스프라이트 이펙트 텍스처 로드
  {
    auto& tm  = Engine::GetTextureManager();
    m_tex_hit2_   = tm.Load("Assets/images/Hit2.png");
    m_tex_hit3_   = tm.Load("Assets/images/Hit3.png");
    m_tex_hit4_   = tm.Load("Assets/images/Hit4.png");
    m_tex_purify_ = tm.Load("Assets/images/Purify_spt.png");
    m_tex_meteor_ = tm.Load("Assets/images/Meteor.png");
    m_tex_magic_  = tm.Load("Assets/images/Magic.png");
    m_tex_cry_       = tm.Load("Assets/images/cry.png");
    m_tex_magic_hit_ = tm.Load("Assets/MagicHit.png");
    m_tex_teleport_  = tm.Load("Assets/images/Teleport.png");
    m_tex_think_     = tm.Load("Assets/images/think.png");
  }
}


void GamePlay::DisplayDamageAmount(const CharacterDamagedEvent& event, double delay)
{
  if (event.target == nullptr) return;
  Math::vec2 size = { 1.0, 1.0 };
  CS200::RGBA color = CS200::WHITE;
  const StatsComponent* stats = event.target->GetStatsComponent();
  if (stats != nullptr && stats->GetMaxHP() > 0)
  {
    float ratio = static_cast<float>(event.damageAmount) / static_cast<float>(stats->GetMaxHP());
    if      (ratio >= 0.2f) { size = { 1.6, 1.6 }; color = CS200::RED; }
    else if (ratio >= 0.1f) { size = { 1.3, 1.3 }; color = CS200::YELLOW; }
  }
  Math::ivec2 grid_pos = event.target->GetGridPosition()->Get();
  Math::vec2 text_position = {
      grid_pos.x * static_cast<double>(GridSystem::TILE_SIZE),
      grid_pos.y * static_cast<double>(GridSystem::TILE_SIZE) + GridSystem::TILE_SIZE
  };
  m_ui_manager->ShowDamageText(event.damageAmount, text_position, event.target, size, color, delay);
}

void GamePlay::CheckGameEnd(const CharacterDeathEvent& event)
{
  auto* turnMgr = GetGSComponent<TurnManager>();

  // 사망 캐릭터가 화면에서 사라지는 시각 딜레이(hurt SFX 재생 시간 포함) 만큼은
  // GameOver 전환을 기다려야 attack/spell SFX 직후 곧바로 종료되지 않는다.
  double visual_delay = (event.character != nullptr) ? std::max(0.0, event.character->GetDeathDelay()) : 0.0;

  if (event.character == player)
  {
	if (turnMgr) turnMgr->EndCombat();
	game_end_player_won_ = false;
	game_end_timer_      = visual_delay;
	game_end             = true;
	return;
  }

  bool all_enemies_dead = std::all_of(enemys.begin(), enemys.end(),
	[this](Character* c) { return c == nullptr || m_confirmed_dead_.count(c) > 0; });
  if (all_enemies_dead && !enemys.empty())
  {
	if (turnMgr) turnMgr->EndCombat();
	game_end_player_won_ = true;
	game_end_timer_      = visual_delay;
	game_end             = true;

	// 릴리즈 모드 순차 잠금: 현재 레벨 클리어 시 다음 레벨 해금
#if !defined(DEVELOPER_VERSION)
	if (s_level_id > 0 && s_level_id >= LevelSelect::s_max_unlocked_level)
	  LevelSelect::s_max_unlocked_level = std::min(3, s_level_id + 1);
#endif
  }
}

void GamePlay::Update(double dt)
{
  // 지연된 SFX 큐 처리 — 컷신/종료 상태와 무관하게 매 프레임 실행
  Engine::GetSoundManager().Update(dt);

  // 피격 이펙트 (셰이크·파티클) 딜레이 처리
  {
	auto* pm = GetGSComponent<CS230::ParticleManager<Particles::Hit>>();
	for (auto& fx : m_pending_hit_effects_)
	  fx.timer -= dt;
	m_pending_hit_effects_.erase(
	  std::remove_if(m_pending_hit_effects_.begin(), m_pending_hit_effects_.end(),
		[&](const PendingHitEffect& fx)
		{
		  if (fx.timer > 0.0) return false;
		  // 캐릭터가 살아있으면 현재 위치(넉백 이후) 사용, 사망 시 캡처 위치 사용
		  Math::vec2 emit_pos = fx.world_pos;
		  if (m_confirmed_dead_.count(fx.target) == 0)
		  {
			fx.target->GetShakeComponent()->StartShake(10.0f, 0.3f);
			emit_pos = fx.target->GetPosition()
			           + Math::vec2{ GridSystem::TILE_SIZE / 2.0, GridSystem::TILE_SIZE / 2.0 };
		  }
		  if (pm)
			pm->Emit(10, emit_pos, { 0, 0 }, { 0, 100 }, 3.14159265);
		  return true;
		}),
	  m_pending_hit_effects_.end());
  }

  // 스프라이트 이펙트 틱 — 모든 프레임에서 진행
  for (auto& fx : m_sprite_effects_)
    fx.elapsed += dt;
  m_sprite_effects_.erase(
    std::remove_if(m_sprite_effects_.begin(), m_sprite_effects_.end(),
      [this](const SpriteEffect& fx)
      {
        if (fx.IsDone() && fx.hide_char
            && m_confirmed_dead_.count(fx.hide_char) == 0)
          fx.hide_char->SetHideSprite(false);
        return fx.IsDone();
      }),
    m_sprite_effects_.end());

  // Meteor 경과 업데이트 + 완료 감지
  if (m_meteor_active_)
  {
    m_meteor_elapsed_ += dt;
    if (m_meteor_elapsed_ >= static_cast<double>(METEOR_FRAMES) / SPRITE_FPS + SLOW_SPELL_EXTRA)
    {
      m_meteor_active_  = false;
      m_meteor_elapsed_ = 0.0;
    }
  }

  if (s_should_restart)
  {
	s_should_restart = false;
	Engine::GetLogger().LogEvent("=== RESTARTING GAMEPLAY ===");
	Engine::GetGameStateManager().PopState();
	Engine::GetGameStateManager().PushState<GamePlay>();
	return;
  }

  // 게임 종료 타이머: 사망 캐릭터 시각 제거(hurt SFX 포함)까지 대기 후 GameOver 상태로 전환
  if (game_end_timer_ >= 0.0)
  {
	game_end_timer_ -= dt;
	if (game_end_timer_ < 0.0)
	{
	  GameOver::s_player_won       = game_end_player_won_;
	  GameOver::s_current_level_id = s_level_id;
	  Engine::GetGameStateManager().PopState();
	  Engine::GetGameStateManager().PushState<GameOver>();
	  return;
	}
  }

  // Camera pan (right-drag) and zoom (scroll wheel) — blocked while pause menu is open
  if (!m_ui_manager->IsPauseMenuOpen())
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
      if (m_ui_manager->IsMouseOverLogPanel())
      {
        m_ui_manager->ScrollLog(scroll);
      }
      else
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
  }

  TurnManager*				      turnMgr		 = GetGSComponent<TurnManager>();
  GridSystem*				        grid		 = GetGSComponent<GridSystem>();
  CombatSystem*				      combatSystem = GetGSComponent<CombatSystem>();
  AISystem*					        aiSystem	 = GetGSComponent<AISystem>();
  CS230::GameObjectManager* goMgr		 = GetGSComponent<CS230::GameObjectManager>();
  DebugManager*				      debugMgr	 = GetGSComponent<DebugManager>();

  if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape))
	m_ui_manager->TogglePauseMenu();

// 수정됨: if (game_end) return; 를 여기서 바로 호출하지 않습니다.

    double scaledDt = dt * static_cast<double>(debugMgr->timeScale);

    // UI는 항상 업데이트 (팝업 입력 처리 포함)
    if (m_ui_manager) m_ui_manager->Update(dt);

    // Pause menu: QUIT 요청 처리
    if (m_ui_manager->IsPauseQuitRequested())
    {
        if (turnMgr) turnMgr->EndCombat();
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
        return;
    }

    // 일시정지 중에는 게임 로직 전체 정지 (AI, 타이머, 이동 애니메이션)
    if (m_ui_manager->IsPauseMenuOpen())
        return;

    // 1. 게임이 끝나더라도 메모리 해제(Destroy 처리)와 파티클 갱신을 위해 기본 시스템 업데이트는 계속 실행합니다.
    if (goMgr) goMgr->UpdateAll(scaledDt);
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
        if (!m_meteor_active_)
            m_input_handler->Update(scaledDt, current, grid, combatSystem, m_ui_manager->GetButtons(), &m_camera);
    }
    if (!m_meteor_active_)
        m_orchestrator->Update(scaledDt, turnMgr, aiSystem);
}

void GamePlay::Unload()
{
  Engine::GetSoundManager().StopBGM();
  Engine::GetSoundManager().ClearPendingDelayedSFX();
  GameCursor::Disable();
  
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

  m_ui_manager->DrawWorld();

  // Pass 1: 월드 공간 스프라이트 이펙트
  for (const auto& fx : m_sprite_effects_)
  {
    if (!fx.tex) continue;
    if (fx.elapsed < fx.delay) continue;
    double play_time = fx.elapsed - fx.delay;
    int    frame;
    if (fx.reverse)
        frame = std::max(0, fx.frame_count - 1 - static_cast<int>(play_time * fx.fps));
    else
        frame = std::min(static_cast<int>(play_time * fx.fps), fx.frame_count - 1);
    Math::ivec2 tex_size = fx.tex->GetSize();
    Math::ivec2 fs{ tex_size.x / fx.frame_count, tex_size.y };
    double scale = (fx.custom_scale > 0.0)
                 ? fx.custom_scale
                 : EFFECT_SCALE * static_cast<double>(GridSystem::TILE_SIZE)
                   / static_cast<double>(fs.x);

    // 스프라이트 중심 월드 좌표 결정
    Math::vec2 center_pos;
    if (fx.mode == SpriteEffect::Mode::Projectile)
    {
      double t = (fx.proj_duration > 0.0)
               ? std::min(1.0, play_time / fx.proj_duration)
               : 1.0;
      center_pos = fx.proj_origin + (fx.world_pos - fx.proj_origin) * t;
    }
    else if (fx.follow_char && m_confirmed_dead_.find(fx.follow_char) == m_confirmed_dead_.end())
    {
      // 넉백 등으로 이동한 캐릭터의 현재 위치를 실시간 추적
      center_pos = fx.follow_char->GetPosition()
                 + Math::vec2{ GridSystem::TILE_SIZE * 0.5, GridSystem::TILE_SIZE * 0.5 };
    }
    else
    {
      center_pos = fx.world_pos;
    }

    // 중심 기준 회전: draw_pos = center - R(angle) * half_size
    Math::vec2 half{ static_cast<double>(fs.x) * scale * 0.5,
                     static_cast<double>(fs.y) * scale * 0.5 };
    Math::vec2 draw_pos = center_pos - Math::RotationMatrix(fx.angle) * half;
    fx.tex->Draw(
      Math::TranslationMatrix(draw_pos) * Math::RotationMatrix(fx.angle) * Math::ScaleMatrix(scale),
      Math::ivec2{ frame * fs.x, 0 }, fs,
      0xFFFFFFFF, DrawDepth::PARTICLE);
  }

  GetGSComponent<DebugManager>()->Draw(grid_system);

  // think.png: AI 턴 시작 시 해당 캐릭터 머리 위에 말풍선 표시 (1.2초)
  if (m_tex_think_ && m_orchestrator)
  {
      double     think_t  = m_orchestrator->GetThinkTimer();
      Character* ai_char  = m_orchestrator->GetCurrentAICharacter();
      bool       hide_for_stealth = false;
#ifndef _DEBUG
      // [릴리즈 모드] 은신 중인 캐릭터의 "생각 중" 말풍선은 표시하지 않는다 (위치 노출 방지)
      hide_for_stealth = ai_char && ai_char->Has("Stealth");
#endif
      if (think_t > 0.0 && ai_char && m_confirmed_dead_.count(ai_char) == 0 && !hide_for_stealth)
      {
          double      elapsed_think = 1.2 - think_t;
          Math::ivec2 tex_size      = m_tex_think_->GetSize();
          Math::ivec2 tfs{ tex_size.x / THINK_FRAMES, tex_size.y };
          int         frame         = static_cast<int>(elapsed_think * SPRITE_FPS) % THINK_FRAMES;
          double      tscale        = static_cast<double>(GridSystem::TILE_SIZE) / static_cast<double>(tfs.x);
          Math::vec2  half_t{ tfs.x * tscale * 0.5, tfs.y * tscale * 0.5 };
          // 캐릭터 타일 상단(TILE_SIZE) + 말풍선 반높이 + 여백 4px = 캐릭터 바로 위에 표시
          Math::vec2  center        = ai_char->GetPosition()
                                      + Math::vec2{ static_cast<double>(GridSystem::TILE_SIZE) * 0.5,
                                                    static_cast<double>(GridSystem::TILE_SIZE) + half_t.y + 4.0 };
          m_tex_think_->Draw(
              Math::TranslationMatrix(center - half_t) * Math::ScaleMatrix(tscale),
              Math::ivec2{ frame * tfs.x, 0 }, tfs,
              0xFFFFFFFF, DrawDepth::PARTICLE);
      }
  }

  renderer_2d->EndScene();

  // Pass 2: UI — virtual 1600x900 coordinates, letterboxed to actual window
  Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
  // Save ui_ndc so EndRenderTextureMode (triggered by font cache misses) restores it correctly
  Engine::GetTextureManager().SaveCurrentScene(ui_ndc);
  renderer_2d->BeginScene(ui_ndc);
  m_ui_manager->Draw(ui_ndc);

  // Pass 2: Meteor 전체화면 이펙트 — 모든 UI 위에 덮어씀
  if (m_meteor_active_ && m_tex_meteor_)
  {
    Math::ivec2 tex_size = m_tex_meteor_->GetSize();
    // Meteor.png 수평 스트립: 프레임당 폭 = 전체 폭 / 프레임 수
    Math::ivec2 fs{ tex_size.x / METEOR_FRAMES, tex_size.y };
    int    frame = std::min(static_cast<int>(m_meteor_elapsed_ * SPRITE_FPS), METEOR_FRAMES - 1);
    double sx    = static_cast<double>(TacticalCamera::VIRTUAL_W) / static_cast<double>(fs.x);
    double sy    = static_cast<double>(TacticalCamera::VIRTUAL_H) / static_cast<double>(fs.y);
    m_tex_meteor_->Draw(
      Math::TranslationMatrix(Math::vec2{ 0.0, 0.0 })
          * Math::ScaleMatrix(Math::vec2{ sx, sy }),
      Math::ivec2{ frame * fs.x, 0 }, fs,
      0xFFFFFFFF, 0.005f);
  }

  renderer_2d->EndScene();
}

void GamePlay::DrawImGui()
{
  GridSystem* grid_system = GetGSComponent<GridSystem>();
  GetGSComponent<DebugManager>()->DrawImGui(grid_system);

#if defined(DEVELOPER_VERSION)
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

	bool is_selected = (s_next_map_id == map_id);
	if (is_selected)
	{
	  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
	}

	if (ImGui::Button(map_id.c_str()))
	{
	  s_next_map_id = map_id;
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

  // Rogue
  auto rogue_spawn_it = map_data.spawn_points.find("rogue");
  if (rogue_spawn_it != map_data.spawn_points.end())
  {
	Math::ivec2 rogue_spawn = rogue_spawn_it->second;
	auto  rogue_ptr = character_factory->Create(CharacterTypes::Rogue, rogue_spawn);
	auto* rogue_raw = rogue_ptr.get();
	rogue_raw->SetGridSystem(grid_system);
	go_manager->Add(std::move(rogue_ptr));
	grid_system->AddCharacter(rogue_raw, rogue_spawn);
	enemys.push_back(rogue_raw);
	Engine::GetLogger().LogEvent("Rogue spawned at: " + std::to_string(rogue_spawn.x) + ", " + std::to_string(rogue_spawn.y));
  }

  // Wizard
  auto wizard_spawn_it = map_data.spawn_points.find("wizard");
  if (wizard_spawn_it != map_data.spawn_points.end())
  {
	Math::ivec2 wizard_spawn = wizard_spawn_it->second;
	auto  wizard_ptr = character_factory->Create(CharacterTypes::Wizard, wizard_spawn);
	auto* wizard_raw = wizard_ptr.get();
	wizard_raw->SetGridSystem(grid_system);
	go_manager->Add(std::move(wizard_ptr));
	grid_system->AddCharacter(wizard_raw, wizard_spawn);
	enemys.push_back(wizard_raw);
	Engine::GetLogger().LogEvent("Wizard spawned at: " + std::to_string(wizard_spawn.x) + ", " + std::to_string(wizard_spawn.y));
  }

  Engine::GetLogger().LogEvent("LoadJSONMap - END: " + map_data.name);
}

void GamePlay::LoadLevelMap(int level_id)
{
  s_allowed_spells.clear();

  const std::filesystem::path level_path = assets::locate_asset("Assets/Data/Level_Map.json");
  std::ifstream               f(level_path);
  if (!f.is_open())
  {
	Engine::GetLogger().LogError("Failed to open Level_Map.json at: " + level_path.string());
	return;
  }
  nlohmann::json root;
  f >> root;

  std::string    target_id  = "level_" + std::to_string(level_id);
  nlohmann::json level_json;
  bool           found = false;
  for (auto& lv : root["levels"])
  {
	if (lv["id"].get<std::string>() == target_id)
	{
	  level_json = lv;
	  found      = true;
	  break;
	}
  }
  if (!found)
  {
	Engine::GetLogger().LogError("Level not found in Level_Map.json: " + target_id);
	return;
  }

  // MapData 구성
  MapData map_data;
  map_data.id     = level_json["id"].get<std::string>();
  map_data.name   = level_json["name"].get<std::string>();
  map_data.width  = level_json["width"].get<int>();
  map_data.height = level_json["height"].get<int>();
  for (auto& row : level_json["tiles"])
	map_data.tiles.push_back(row.get<std::string>());
  for (auto& [sym, type] : level_json["legend"].items())
	map_data.legend[sym[0]] = type.get<std::string>();
  for (auto& [key, pos] : level_json["spawn_points"].items())
	map_data.spawn_points[key] = { pos["x"].get<int>(), pos["y"].get<int>() };

  // 허용 스펠 설정 (빈 배열 = 모든 스펠 허용)
  for (auto& sp : level_json["allowed_spells"])
	s_allowed_spells.push_back(sp.get<std::string>());

  // 적 목록
  std::vector<std::string> enemy_list;
  for (auto& e : level_json["enemies"])
	enemy_list.push_back(e.get<std::string>());

  CS230::GameObjectManager* go_manager       = GetGSComponent<CS230::GameObjectManager>();
  GridSystem*               grid_system      = GetGSComponent<GridSystem>();
  CharacterFactory*         character_factory = GetGSComponent<CharacterFactory>();

  grid_system->LoadMap(map_data);

  // Dragon 스폰
  auto dragon_it = map_data.spawn_points.find("dragon");
  if (dragon_it != map_data.spawn_points.end())
  {
	Math::ivec2 spawn = dragon_it->second;
	auto        ptr   = character_factory->Create(CharacterTypes::Dragon, spawn);
	player            = ptr.get();
	player->SetGridSystem(grid_system);
	go_manager->Add(std::move(ptr));
	grid_system->AddCharacter(player, spawn);
	Engine::GetLogger().LogEvent("Dragon spawned at: " + std::to_string(spawn.x) + "," + std::to_string(spawn.y));
  }
  else
  {
	Engine::GetLogger().LogError("No dragon spawn in level " + target_id);
  }

  // 적 스폰 (enemies 배열 순서대로)
  static const std::map<std::string, CharacterTypes> name_to_type = {
	{ "fighter", CharacterTypes::Fighter },
	{ "cleric",  CharacterTypes::Cleric  },
	{ "rogue",   CharacterTypes::Rogue   },
	{ "wizard",  CharacterTypes::Wizard  }
  };

  for (const auto& enemy_name : enemy_list)
  {
	auto type_it  = name_to_type.find(enemy_name);
	auto spawn_it = map_data.spawn_points.find(enemy_name);
	if (type_it == name_to_type.end() || spawn_it == map_data.spawn_points.end())
	  continue;

	Math::ivec2 spawn     = spawn_it->second;
	auto        enemy_ptr = character_factory->Create(type_it->second, spawn);
	auto*       enemy_raw = enemy_ptr.get();
	enemy_raw->SetGridSystem(grid_system);
	go_manager->Add(std::move(enemy_ptr));
	grid_system->AddCharacter(enemy_raw, spawn);
	enemys.push_back(enemy_raw);
	Engine::GetLogger().LogEvent(enemy_name + " spawned at: " + std::to_string(spawn.x) + "," + std::to_string(spawn.y));
  }

  Engine::GetLogger().LogEvent("LoadLevelMap done: level " + std::to_string(level_id));
}
