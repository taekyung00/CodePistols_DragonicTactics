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