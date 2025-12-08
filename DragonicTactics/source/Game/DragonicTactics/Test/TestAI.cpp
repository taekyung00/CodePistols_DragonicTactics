#include "pch.h"

#include "TestAI.h"

#include "./Engine/GameStateManager.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/StateComponents/AISystem.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"

bool TestAITargetsClosestEnemy()
{
  // Test: AI should target closest enemy via MakeDecision
  auto&		  gs   = Engine::GetGameStateManager();
  GridSystem* grid = gs.GetGSComponent<GridSystem>();
  if (!grid)
  {
	std::cout << "  FAILED: GridSystem not found\n";
	return false;
  }
  grid->Reset();

  Fighter testfighter({ 1, 1 });
  testfighter.SetGridPosition({ 1, 1 });
  grid->AddCharacter(&testfighter, Math::ivec2{ 1, 1 });
  testfighter.SetActionPoints(10);

  Dragon testdragon({ 3, 3 });
  testdragon.SetGridPosition({ 3, 3 }); // Distance = 4 (Manhattan)
  grid->AddCharacter(&testdragon, Math::ivec2{ 3, 3 });

  AISystem	 ai;
  AIDecision decision = ai.MakeDecision(&testfighter);

  // Fighter should try to move closer or attack if in range
  // The target should be the dragon
  bool passed = (decision.target == &testdragon || decision.type == AIDecisionType::Move);

  if (!passed)
  {
	std::cout << "  FAILED: AI didn't target Dragon (decision type: " << (int)decision.type << ")\n";
  }

  return passed;
}

bool TestAIMovesCloserWhenOutOfRange()
{
  // Test: AI should move closer if target out of range
  auto&		  gs   = Engine::GetGameStateManager();
  GridSystem* grid = gs.GetGSComponent<GridSystem>();
  if (!grid)
  {
	std::cout << "  FAILED: GridSystem not found\n";
	return false;
  }
  grid->Reset();

  Fighter testfighter({ 1, 1 });
  testfighter.SetGridPosition({ 1, 1 });
  grid->AddCharacter(&testfighter, Math::ivec2{ 1, 1 });
  testfighter.SetAttackRange(1); // Melee
  testfighter.SetActionPoints(10);

  Dragon testdragon({ 2, 2 });
  testdragon.SetGridPosition({ 6, 6 }); // Far away
  grid->AddCharacter(&testdragon, Math::ivec2{ 6, 6 });

  AISystem	 ai;
  AIDecision decision = ai.MakeDecision(&testfighter);

  bool passed = (decision.type == AIDecisionType::Move);

  if (!passed)
  {
	std::cout << "  FAILED: AI didn't move closer (decision: " << (int)decision.type << ", reasoning: " << decision.reasoning << ")\n";
  }

  return passed;
}

bool TestAIAttacksWhenInRange()
{
  // Test: AI should attack if target in range
  auto&		  gs   = Engine::GetGameStateManager();
  GridSystem* grid = gs.GetGSComponent<GridSystem>();
  if (!grid)
  {
	std::cout << "  FAILED: GridSystem not found\n";
	return false;
  }
  grid->Reset();

  Fighter testfighter({ 1, 1 });
  testfighter.SetGridPosition({ 4, 4 });
  grid->AddCharacter(&testfighter, Math::ivec2{ 4, 4 });
  testfighter.SetActionPoints(10); // Enough for attack
  testfighter.SetAttackRange(1);

  Dragon testdragon({ 2, 2 });
  testdragon.SetGridPosition({ 4, 5 }); // Adjacent (distance = 1)
  grid->AddCharacter(&testdragon, Math::ivec2{ 4, 5 });

  AISystem	 ai;
  AIDecision decision = ai.MakeDecision(&testfighter);

  bool passed = (decision.type == AIDecisionType::Attack || decision.type == AIDecisionType::UseAbility);

  if (!passed)
  {
	std::cout << "  FAILED: AI didn't attack when in range (decision: " << (int)decision.type << ", reasoning: " << decision.reasoning << ")\n";
  }

  return passed;
}

bool TestAIUsesShieldBashWhenAdjacent()
{
  // Test: AI should use Shield Bash when adjacent to healthy target
  auto&		  gs   = Engine::GetGameStateManager();
  GridSystem* grid = gs.GetGSComponent<GridSystem>();
  if (!grid)
  {
	std::cout << "  FAILED: GridSystem not found\n";
	return false;
  }
  grid->Reset();

  Fighter testfighter({ 1, 1 });
  testfighter.SetGridPosition({ 4, 4 });
  grid->AddCharacter(&testfighter, Math::ivec2{ 4, 4 });
  testfighter.SetActionPoints(10);
  testfighter.SetAttackRange(1);

  Dragon testdragon({ 2, 2 });
  testdragon.SetGridPosition({ 4, 5 }); // Adjacent
  grid->AddCharacter(&testdragon, Math::ivec2{ 4, 5 });
  testdragon.SetHP(testdragon.GetMaxHP()); // Full HP

  AISystem	 ai;
  AIDecision decision = ai.MakeDecision(&testfighter);

  // Should use Shield Bash if fighter has the ability and target is healthy
  bool passed = (decision.type == AIDecisionType::UseAbility && decision.abilityName == "Shield Bash");

  if (!passed)
  {
	std::cout << "  FAILED: AI didn't use Shield Bash when appropriate"
			  << " (decision: " << (int)decision.type << ", ability: " << decision.abilityName << ")\n";
  }

  return passed;
}

bool TestAIEndsTurnWhenNoActions()
{
  // Test: AI should end turn if no valid actions
  auto&		  gs   = Engine::GetGameStateManager();
  GridSystem* grid = gs.GetGSComponent<GridSystem>();
  if (!grid)
  {
	std::cout << "  FAILED: GridSystem not found\n";
	return false;
  }
  grid->Reset();

  Fighter testfighter({ 1, 1 });
  testfighter.SetGridPosition({ 1, 1 });
  grid->AddCharacter(&testfighter, Math::ivec2{ 1, 1 });
  testfighter.SetActionPoints(0); // No action points

  Dragon testdragon({ 2, 2 });
  testdragon.SetGridPosition({ 2, 2 });
  grid->AddCharacter(&testdragon, Math::ivec2{ 2, 2 });

  AISystem	 ai;
  AIDecision decision = ai.MakeDecision(&testfighter);

  bool passed = (decision.type == AIDecisionType::EndTurn);

  if (!passed)
  {
	std::cout << "  FAILED: AI didn't end turn when no actions available"
			  << " (decision: " << (int)decision.type << ", reasoning: " << decision.reasoning << ")\n";
  }

  return passed;
}

void RunFighterAITests()
{
  std::cout << "\n=== FIGHTER AI TESTS ===\n";
  std::cout << (TestAITargetsClosestEnemy() ? "O" : "X") << " AI targets closest enemy\n";
  std::cout << (TestAIMovesCloserWhenOutOfRange() ? "O" : "X") << " AI moves closer when out of range\n";
  std::cout << (TestAIAttacksWhenInRange() ? "O" : "X") << " AI attacks when in range\n";
  std::cout << (TestAIUsesShieldBashWhenAdjacent() ? "O" : "X") << " AI uses Shield Bash appropriately\n";
  std::cout << (TestAIEndsTurnWhenNoActions() ? "O" : "X") << " AI ends turn when no actions\n";
  std::cout << "==========================\n";
}
