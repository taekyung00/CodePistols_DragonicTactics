/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  BattleOrchestrator.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/

#pragma once
class TurnManager;
class Character;
class AISystem;

namespace CS230
{
  class GameObjectManager;
}

class BattleOrchestrator
{
  public:
  void Update(double dt, TurnManager* turn_manager, AISystem* ai_system);
  bool CheckVictoryCondition();

  private:
  void HandleAITurn(Character* ai_character, TurnManager* turn_manager, AISystem* ai_system);
  int  m_previous_round = 0;
};
