#pragma once
#include "../../../Engine/GameState.hpp"
#include "../StateComponents/GridSystem.h"
class Dragon;
class Fighter;

class Test : public CS230::GameState
{
public:
    Test();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    void     test_subscribe_publish_singleSubscriber();
    void     test_multiple_subscribers_sameEvent();
    void     test_multiple_different_events();
    void     test_EventData_CompleteTransfer();
    void     test_EventData_MultiplePublishes();
    Fighter* fighter;
    Dragon*  dragon;
    void     LogFighterStatus();
    void     LogDragonStatus();

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

    void test_dice_manager();
    void test_turnmanager_all();
    void test_json();
    void test_json_reload();
    void test_json_log();

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
};

class Test2 : public CS230::GameState
{
public:
    Test2();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:

    Fighter* fighter;
    Dragon*  dragon;
    void     LogFighterStatus();
    void     LogDragonStatus();

        // Ginam: Week3 Developer B - Grid-based Ability Tests
	void test_MeleeAttack_WithGrid(); // Ginam: test melee attack on grid
	void test_ShieldBash_WithGrid();  // Ginam: test shield bash knockback on grid
	void test_ShieldBash_IntoWall();  // Ginam: test shield bash blocked by wall
	//void test_Ability_VisualTest();	  // Ginam: visual test with arrow keys
};