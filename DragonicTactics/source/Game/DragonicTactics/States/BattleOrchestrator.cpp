/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  BattleOrchestrator.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/

#include "pch.h"
#include "BattleOrchestrator.h"
#include "GamePlay.h"
#include "./CS200/IRenderer2D.h"
#include "./CS200/NDC.h"


#include "../Debugger/DebugManager.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/DiceManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/StateComponents/AISystem.h"
#include "Game/MainMenu.h"

#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"


void BattleOrchestrator::Update([[maybe_unused]]double dt, TurnManager* turn_manager, AISystem* ai_system, CS230::GameObjectManager* go_manager) {
    if (!turn_manager->IsCombatActive()) return;

    Character* current = turn_manager->GetCurrentCharacter();
    int current_round = turn_manager->GetRoundNumber();
    if (current_round != m_previous_round) {
        m_previous_round = current_round;
    }

    if (current->GetCharacterType() != CharacterTypes::Dragon) {
       HandleAITurn(current, turn_manager, ai_system, go_manager);
    }
}

bool BattleOrchestrator::ShouldContinueTurn(Character* current_character, AISystem* ai_system, CS230::GameObjectManager* go_manager) {
    Character* target = nullptr;

    for (const auto& obj_ptr : go_manager->GetAll()) {
        CS230::GameObject* obj = obj_ptr.get(); 

        if (obj->Type() == GameObjectTypes::Character) {
            Character* character = static_cast<Character*>(obj);
            
            if (character->GetCharacterType() == CharacterTypes::Dragon) {
                target = character;
                break;
            }
        }
    }

    if (!target) return false;

    bool shouldAttack = ai_system->ShouldAttack(current_character, target);
    bool shouldMove   = ai_system->ShouldMoveCloser(current_character, target);
    
    return shouldAttack || shouldMove;
}



void BattleOrchestrator::HandleAITurn(Character* ai_character, TurnManager* turn_manager, AISystem* ai_system, CS230::GameObjectManager* go_manager) {
    
    MovementComponent* move_comp = ai_character->GetGOComponent<MovementComponent>();
    if (move_comp && move_comp->IsMoving()) {
        return;
    }

    if (ai_character->GetCharacterType() == CharacterTypes::Fighter) {
        Fighter* fighter = static_cast<Fighter*>(ai_character);
        
        fighter->Action();

        bool keep_turn = ShouldContinueTurn(ai_character, ai_system, go_manager);

        if (!keep_turn) {
            turn_manager->EndCurrentTurn();
        }
    }

    
    // if (ai_character->GetCharacterType() == CharacterTypes::Wizard) {
    //     Wizard* wizard = static_cast<Wizard*>(ai_character);
        
    //     wizard->Action();

    //     bool keep_turn = ShouldContinueTurn(ai_character, ai_system, go_manager);

    //     if (!keep_turn) {
    //         turn_manager->EndCurrentTurn();
    //     }
    // }
}


bool BattleOrchestrator::CheckVictoryCondition() {
    return false;
}
