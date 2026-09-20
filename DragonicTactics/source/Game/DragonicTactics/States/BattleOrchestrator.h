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
  void HandleAITurn(Character* ai_character, TurnManager* turn_manager, AISystem* ai_system, double dt);
  int        m_previous_round      = 0;
  double     m_wait_timer          = 0.0;
  Character* m_last_ai_character_  = nullptr;  // 새 AI 턴 감지용
  double     m_think_timer_        = 0.0;      // 턴 시작 "생각 중" 대기

  public:
  double     GetThinkTimer()         const { return m_think_timer_; }
  Character* GetCurrentAICharacter() const { return m_last_ai_character_; }
};
