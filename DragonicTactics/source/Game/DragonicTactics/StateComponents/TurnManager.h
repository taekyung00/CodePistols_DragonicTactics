#pragma once
#include "../Objects/Character.h"
#include "../Test/Week1TestMocks.h"
#include "../Types/Events.h"
#include "./EventBus.h"
#include <queue>
#include <vector>

// Forward declarations
class EventBus;

// ===== Week 4: Initiative System (NEW) =====
// Initiative tracking structure
struct InitiativeEntry
{
  Character*	 character;
  MockCharacter* mockCharacter; // For testing
  int			 speed;

  // Constructor for real characters
  InitiativeEntry(Character* ch, int sp) : character(ch), mockCharacter(nullptr), speed(sp)
  {
  }

  // Constructor for mock characters (testing)
  InitiativeEntry(MockCharacter* ch, int sp) : character(nullptr), mockCharacter(ch), speed(sp)
  {
  }
};

// Initiative mode options
enum class InitiativeMode
{
  RollOnce,		// Roll at combat start (D&D 5e default)
  RollEachRound // Re-roll each round (variant rule)
};

// ===== End Sangyun Initiative System =====

class TurnManager : public CS230::Component
{
  public:
  TurnManager();

  // Dependency injection for testing
  void SetEventBus(EventBus* bus)
  {
	eventBus = bus;
  }

  // Turn management
  void InitializeTurnOrder(const std::vector<Character*>& characters);
  void InitializeTurnOrder(const std::vector<MockCharacter*>& characters);
  void StartNextTurn();
  void EndCurrentTurn();

  // ===== Sangyun: Initiative System (NEW) =====
  void RollInitiative(const std::vector<Character*>& characters);
  void ResetInitiative();

  // int GetInitiativeValue(Character* character) const;
  void SetInitiativeMode(InitiativeMode mode)
  {
	initiativeMode = mode;
  }

  // Test support (MockCharacter overloads)

  // ===== End Sangyun Initiative System =====

  // Turn state
  Character* GetCurrentCharacter() const;
  int		 GetCurrentTurnNumber() const;
  int		 GetRoundNumber() const;
  bool		 IsCombatActive() const;

  // Turn order
  std::vector<Character*> GetTurnOrder() const;
  int					  GetCharacterTurnIndex(Character* character) const;

  // Combat state
  void StartCombat();
  void EndCombat();
  void Reset();

  private:
  TurnManager(const TurnManager&)			 = delete;
  TurnManager& operator=(const TurnManager&) = delete;

  std::vector<Character*> turnOrder;
  int					  currentTurnIndex;
  int					  turnNumber;
  int					  roundNumber;
  bool					  combatActive;

  // Dependency injection
  EventBus* eventBus = nullptr;

  // ===== Sangyun: Initiative System (NEW) =====
  std::vector<InitiativeEntry> initiativeOrder;
  InitiativeMode			   initiativeMode;
  // ===== End Sangyun Initiative System =====

  void PublishTurnStartEvent();
  void PublishTurnEndEvent();

  // ===== Sangyun: Initiative Helper Methods (NEW) =====
  void SortInitiativeOrder();
  // ===== End Sangyun Initiative System =====
};