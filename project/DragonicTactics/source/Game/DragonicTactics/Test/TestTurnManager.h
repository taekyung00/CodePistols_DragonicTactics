/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
bool Test_TurnManager_Initialize();
bool Test_TurnManager_Initialize_Empty();
bool Test_TurnManager_Initialize_DeadCharacters();
bool Test_TurnManager_StartCombat();
bool Test_TurnManager_EndCurrentTurn();
bool Test_TurnManager_RoundProgression();
bool Test_TurnManager_ActionPointRefresh();
bool Test_TurnManager_SkipDeadCharacter();
bool Test_TurnManager_AllCharactersDead();
bool Test_TurnManager_GetCharacterTurnIndex();
void test_turnmanager_all();

extern bool TestTrunManager;