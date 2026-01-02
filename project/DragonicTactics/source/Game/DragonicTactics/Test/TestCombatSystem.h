/**
 * \file
 * \author Sangyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
// Individual CombatSystem test functions
bool Test_CombatSystem_CalculateDamage();
bool Test_CombatSystem_CalculateDamage_MinRoll();
bool Test_CombatSystem_CalculateDamage_MaxRoll();
bool Test_CombatSystem_ApplyDamage();
bool Test_CombatSystem_ApplyDamage_Negative();
bool Test_CombatSystem_ExecuteAttack_Valid();
bool Test_CombatSystem_ExecuteAttack_OutOfRange();
bool Test_CombatSystem_ExecuteAttack_NotEnoughAP();
bool Test_CombatSystem_IsInRange_Adjacent();
bool Test_CombatSystem_IsInRange_TooFar();
bool Test_CombatSystem_GetDistance();

extern bool TestCombatSystem;