#pragma once
#include "../../../Engine/GameState.hpp"
#include "../../../Engine/Fonts.h"
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Texture.hpp"
#include "../../../CS200/RGBA.hpp"
#include "../StateComponents/GridSystem.h"

class Dragon;
class Fighter;

class GamePlay : public CS230::GameState
{
public:
    GamePlay();
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

    Fighter* fighter;
    Dragon* dragon;


};