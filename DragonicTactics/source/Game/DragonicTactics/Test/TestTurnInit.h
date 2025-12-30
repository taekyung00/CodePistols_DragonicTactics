/**
 * \file
 * \author Sangyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once


// Individual TurnManager test functions
bool TestInitiativeBasicRoll();
bool TestInitiativeTurnOrder();
bool TestInitiativeSpeedModifier();
bool TestInitiativeDeadCharacterSkipped();
bool TestInitiativeReRoll();
bool TestInitiativeModeRollOnce();
bool TestInitiativeReset();

// Run all initiative tests
void RunTurnManagerInitiativeTests();