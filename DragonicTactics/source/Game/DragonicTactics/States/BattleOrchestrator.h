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

class BattleOrchestrator {
public:
    void Update(double dt, TurnManager* turn_manager);
    void HandleAITurn(Character* ai_character);
    bool CheckVictoryCondition();

private:
    int m_previous_round = 0;
};
