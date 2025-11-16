#pragma once
#include "../../../Engine/GameState.hpp"
#include "../../../Engine/Fonts.h"
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Texture.hpp"
#include "../../../CS200/RGBA.hpp"
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
    void test_turnmanager_all();


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