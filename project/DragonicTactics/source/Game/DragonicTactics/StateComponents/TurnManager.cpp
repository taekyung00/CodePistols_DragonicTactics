/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "../Objects/Components/StatsComponent.h"
#include "./DiceManager.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Logger.h"
#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"
#include "Game/DragonicTactics/StateComponents/StatusEffectHandler.h"
#include "TurnManager.h"

TurnManager::TurnManager() : currentTurnIndex{}, turnNumber{}, roundNumber{}, combatActive{}, initiativeMode{ InitiativeMode::RollOnce }
{
}

void TurnManager::InitializeTurnOrder(const std::vector<Character*>& characters)
{
  if (characters.empty())
  {
	Engine::GetLogger().LogError("TurnManager: Cannot initialize with empty character list");
	return;
  }

  // ===== Sangyun: Use initiative system instead of simple array order (NEW) =====
  RollInitiative(characters);

  // Update turnOrder from initiativeOrder for backward compatibility
  turnOrder.clear();
  for (const auto& entry : initiativeOrder)
  {
	if (entry.character && entry.character->IsAlive())
	{
	  turnOrder.push_back(entry.character);
	}
  }
  // ===== End Sangyun Initiative Integration =====

  if (turnOrder.empty())
  {
	Engine::GetLogger().LogError("TurnManager: All characters are dead");
	return;
  }

  // Reset turn counters
  currentTurnIndex = 0;
  turnNumber	   = 1;
  roundNumber	   = 1;
  combatActive	   = false;

  Engine::GetLogger().LogEvent("TurnManager: Turn order initialized with " + std::to_string(turnOrder.size()) + " characters");
}

void TurnManager::StartCombat()
{
  if (turnOrder.empty())
  {
	Engine::GetLogger().LogError("TurnManager: Cannot start combat without turn order");
	return;
  }

  combatActive	   = true;
  currentTurnIndex = 0;
  turnNumber	   = 1;
  roundNumber	   = 1;

  Engine::GetLogger().LogEvent("TurnManager: Combat started");

  // Start first turn
  StartNextTurn();
}

void TurnManager::StartNextTurn()
{
    // Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - BEGIN");
    if (!combatActive)
    {
        Engine::GetLogger().LogError("TurnManager: Combat not active");
        return;
    }

    /*======================================================================================*/
    // 1. 크래시 방지: 이미 파괴되었거나 죽은 캐릭터를 턴 대기열에서 안전하게 제거합니다.
    /*======================================================================================*/
    turnOrder.erase(
        std::remove_if(turnOrder.begin(), turnOrder.end(),
            [](Character* c) {
                // 포인터가 유효하지 않거나 이미 죽은 캐릭터라면 목록에서 제거 대상으로 분류합니다.
                return c == nullptr || !c->IsAlive(); 
            }),
        turnOrder.end()
    );

    // 2. 캐릭터가 모두 죽어서 목록이 비었다면 전투를 즉시 종료합니다.
    if (turnOrder.empty())
    {
        Engine::GetLogger().LogEvent("TurnManager: All characters dead, ending combat");
        EndCombat();
        return;
    }

    // 3. 인덱스 보정: 캐릭터 삭제로 인해 리스트가 짧아졌을 경우 인덱스 초과 에러를 방지합니다.
    if (static_cast<std::size_t>(currentTurnIndex) >= turnOrder.size())
    {
        currentTurnIndex = 0;
    }

    /*======================================================================================*/
    // 정상적인 턴 시작 로직 진행
    /*======================================================================================*/
    
    // 안전하게 현재 캐릭터 가져오기
    Character* currentChar = turnOrder[static_cast<std::size_t>(currentTurnIndex)];
    Engine::GetLogger().LogEvent("TurnManager: Turn " + std::to_string(turnNumber) + " - " + currentChar->TypeName() + "'s turn");

    // [수정 후 순서 적용 완료]
    // 1. AP/Speed 리프레시
    currentChar->OnTurnStart();

    // 2. 효과 적용 (Exhaustion이 아직 존재함, duration=1)
    auto* handler = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();
    if (handler)
        handler->OnTurnStart(currentChar);

    // 3. 그 다음 duration 감소 (1 → 0 → 제거)
    auto* se = currentChar->GetGOComponent<StatusEffectComponent>();
    if (se)
        se->TickDown(currentChar, eventBus);

    // 용암 턴 시작 피해 — 현재 캐릭터가 용암 위에 있으면 피해
    {
        auto* spell_system = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
        auto* combat       = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
        
        if (spell_system && combat && currentChar->IsAlive())
        {
            GridPosition* gp = currentChar->GetGOComponent<GridPosition>();
            if (gp)
            {
                int dmg = spell_system->GetLavaDamageAt(gp->Get());
                if (dmg > 0)
                {
                    Engine::GetLogger().LogEvent(currentChar->TypeName() + " takes " + std::to_string(dmg) + " lava damage at turn start");
                    combat->ApplyDamage(nullptr, currentChar, dmg);
                }
            }
        }
    }

    // Publish turn start event
    PublishTurnStartEvent();

    // Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - END");
}

void TurnManager::EndCurrentTurn()
{
  // Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - BEGIN");
  if (!combatActive)
  {
	Engine::GetLogger().LogError("TurnManager: Combat not active");
	return;
  }

  Character* currentChar = turnOrder[static_cast<std::size_t>(currentTurnIndex)];

  // Call OnTurnEnd
  // Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - Calling OnTurnEnd");
  currentChar->OnTurnEnd();

  // Publish turn end event
  PublishTurnEndEvent();

  // Advance to next character
  currentTurnIndex = static_cast<int>((static_cast<std::size_t>(currentTurnIndex) + 1) % turnOrder.size());
  turnNumber++;

  // Check if we completed a round (all characters had a turn)
  if (currentTurnIndex == 0)
  {
	roundNumber++;
	Engine::GetLogger().LogEvent("TurnManager: Round " + std::to_string(roundNumber) + " started");

	// 지형 효과 만료 처리
	auto* spell_system = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
	if (spell_system)
	  spell_system->TickTerrainEffects(roundNumber);

	// ===== Sangyun: Re-roll initiative if variant mode enabled (NEW) =====
	if (initiativeMode == InitiativeMode::RollEachRound)
	{
	  std::vector<Character*> aliveCharacters;
	  for (const auto& entry : initiativeOrder)
	  {
		if (entry.character && entry.character->IsAlive())
		{
		  aliveCharacters.push_back(entry.character);
		}
	  }

	  if (!aliveCharacters.empty())
	  {
		RollInitiative(aliveCharacters);

		// Update turnOrder from new initiative
		turnOrder.clear();
		for (const auto& entry : initiativeOrder)
		{
		  if (entry.character && entry.character->IsAlive())
		  {
			turnOrder.push_back(entry.character);
		  }
		}

		currentTurnIndex = 0; // Reset to first in new order
	  }
	}
	// ===== End Sangyun Initiative Re-roll =====
  }

  // Start next turn
  StartNextTurn();

  // Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - END");
}

void TurnManager::EndCombat()
{
  combatActive = false;
  Engine::GetLogger().LogEvent("TurnManager: Combat ended after " + std::to_string(turnNumber) + " turns (" + std::to_string(roundNumber) + " rounds)");

  // Publish combat end event
  if (eventBus)
  {
	eventBus->Publish(CombatEndedEvent{});
  }
  else
  {
	auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
	if (bus)
	  bus->Publish(CombatEndedEvent{});
  }
}

void TurnManager::Reset()
{
  turnOrder.clear();
  initiativeOrder.clear();
  currentTurnIndex = 0;
  turnNumber	   = 0;
  roundNumber	   = 0;
  combatActive	   = false;

  Engine::GetLogger().LogEvent("TurnManager: Reset");
}

Character* TurnManager::GetCurrentCharacter() const
{
  if (turnOrder.empty() || currentTurnIndex >= static_cast<int>(turnOrder.size()))
  {
	return nullptr;
  }
  return turnOrder[static_cast<std::size_t>(currentTurnIndex)];
}

int TurnManager::GetCurrentTurnNumber() const
{
  return turnNumber;
}

int TurnManager::GetRoundNumber() const
{
  return roundNumber;
}

bool TurnManager::IsCombatActive() const
{
  return combatActive;
}

std::vector<Character*> TurnManager::GetTurnOrder() const
{
  // ===== Sangyun: Return initiative-based turn order if available (NEW) =====
  if (!initiativeOrder.empty())
  {
	std::vector<Character*> order;
	for (const auto& entry : initiativeOrder)
	{
	  if (entry.character && entry.character->IsAlive())
	  {
		order.push_back(entry.character);
	  }
	}
	return order;
  }
  // ===== Sangyun Initiative Return =====

  // Fallback to simple turnOrder
  return turnOrder;
}

int TurnManager::GetCharacterTurnIndex(Character* character) const
{
  for (size_t i = 0; i < turnOrder.size(); ++i)
  {
	if (turnOrder[i] == character)
	{
	  return static_cast<int>(i);
	}
  }
  return -1;
}

void TurnManager::PublishTurnStartEvent()
{
  Character* currentChar = GetCurrentCharacter();
  if (currentChar == nullptr)
	return;

  TurnStartedEvent event{ currentChar, turnNumber, roundNumber };
  if (eventBus)
  {
	eventBus->Publish(event);
  }
  else
  {
	auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
	if (bus)
	  bus->Publish(event);
  }
}

void TurnManager::PublishTurnEndEvent()
{
  Character* currentChar = GetCurrentCharacter();
  if (currentChar == nullptr)
	return;

  TurnEndedEvent event{ currentChar, turnNumber };
  if (eventBus)
  {
	eventBus->Publish(event);
  }
  else
  {
	auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
	if (bus)
	  bus->Publish(event);
  }
}

// ==========Sangyun: INITIATIVE SYSTEM IMPLEMENTATION (NEW) ==========


void TurnManager::RollInitiative(const std::vector<Character*>& characters)
{
  // Clear previous initiative
  initiativeOrder.clear();

  Engine::GetLogger().LogEvent("=== TURN INITIATIVE ===");

  // DiceManager& dice = Engine::GetDiceManager();

  // Roll 1d20 + speed modifier for each character
  for (Character* character : characters)
  {
	if (!character || !character->IsAlive())
	{
	  continue;
	}

	// Roll 1d20
	// int roll = dice.RollDice(1, 20);

	// Get speed from StatsComponent and calculate modifier
	StatsComponent* stats = character->GetStatsComponent();
	if (!stats)
	{
	  Engine::GetLogger().LogError("Character has no StatsComponent!");
	  continue;
	}

	int speed = stats->GetSpeed();

	// Create initiative entry
	InitiativeEntry entry(character, speed);
	initiativeOrder.push_back(entry);

	// Log result
	Engine::GetLogger().LogEvent(character->TypeName() + " 's speed is " + std::to_string(speed));

	// Publish individual initiative rolled event
	InitiativeEvent event{ character, speed };
	if (eventBus)
	{
	  eventBus->Publish(event);
	}
	else
	{
	  auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
	  if (bus)
		bus->Publish(event);
	}
  }

  // Sort by initiative (highest first)
  SortInitiativeOrder();

  // Publish turn order established event
  std::vector<Character*> sortedChars;
  for (const auto& entry : initiativeOrder)
  {
	sortedChars.push_back(entry.character);
  }

  if (eventBus)
  {
	eventBus->Publish(TurnOrderEstablishedEvent{ sortedChars });
  }
  else
  {
	auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
	if (bus)
	  bus->Publish(TurnOrderEstablishedEvent{ sortedChars });
  }

  Engine::GetLogger().LogEvent("=== TURN ORDER ESTABLISHED ===");
  for (const auto& entry : initiativeOrder)
  {
	Engine::GetLogger().LogEvent("  " + std::to_string(entry.speed) + ": " + entry.character->TypeName());
  }
}

void TurnManager::SortInitiativeOrder()
{
  // Sort by total initiative (descending)
  std::sort(
	  initiativeOrder.begin(), initiativeOrder.end(),
	  [](const InitiativeEntry& a, const InitiativeEntry& b)
	  {
		// Primary sort: Total initiative (higher goes first)
		if (a.speed != b.speed)
		{
		  return a.speed > b.speed;
		}

		// Tie-breaker 2: Pointer address (deterministic for same pointers)
		return a.character > b.character;
	  });
}

// int TurnManager::GetInitiativeValue(Character* character) const
//{
//	for (const auto& entry : initiativeOrder)
//	{
//		if (entry.character == character)
//		{
//			return entry.totalInitiative;
//		}
//	}
//	return 0; // Character not in initiative order
// }

void TurnManager::ResetInitiative()
{
  initiativeOrder.clear();
  Engine::GetLogger().LogEvent("Initiative reset for new combat");
}

// ==========Sangyun: MOCK CHARACTER SUPPORT FOR TESTING (NEW) ==========
