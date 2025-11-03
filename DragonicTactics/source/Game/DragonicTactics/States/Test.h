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

    enum class PlayerActionState
    {
        None,          
        SelectingMove,  
        SelectingAction 
    };
    PlayerActionState currentPlayerState = PlayerActionState::None;

    enum class ButtonHoverState
    {
        None,
        Move,
        Action,
        EndTurn
    };
    ButtonHoverState currentHoverState = ButtonHoverState::None;

    static constexpr CS200::RGBA button_color_normal = 0xFFFFFFFF; // non_seleted_color
    static constexpr CS200::RGBA button_color_hover = 0x3ADF00FF;  // seleted_color
    static constexpr CS200::RGBA button_color_disabled = 0x808080FF;

    CS200::RGBA move_button_color;
    CS200::RGBA action_button_color;
    CS200::RGBA end_turn_button_color;

    //these are hard coded numbers
    const Math::vec2 move_button_pos{ 1000, 50 };    
    const Math::vec2 action_button_pos{ 1000, 100 }; 
    const Math::vec2 end_turn_button_pos{ 1000, 150 }; 
    const Math::vec2 button_size{ 250, 40 };

    void update_button_colors();
    void update_button_logic();
    
    void     Test2_subscribe_publish_singleSubscriber();
    void     Test2_multiple_subscribers_sameEvent();
    void     Test2_multiple_different_events();
    void     Test2_EventData_CompleteTransfer();
    void     Test2_EventData_MultiplePublishes();
    Fighter* fighter;
    Dragon*  dragon;
    void     LogFighterStatus();
    void     LogDragonStatus();
};