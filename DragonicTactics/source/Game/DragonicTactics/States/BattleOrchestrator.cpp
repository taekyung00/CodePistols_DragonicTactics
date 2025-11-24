/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  BattleOrchestrator.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/


#include "BattleOrchestrator.h"
#include "GamePlay.h"
#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/TextManager.hpp"
#include "./Engine/Window.hpp"

#include "../Debugger/DebugManager.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/Singletons/CombatSystem.h"
#include "Game/DragonicTactics/Singletons/DiceManager.h"
#include "Game/DragonicTactics/Singletons/EventBus.h"
#include "Game/MainMenu.h"

#include "Engine/Input.hpp"
#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/Singletons/SpellSystem.h"
#include <imgui.h>

void BattleOrchestrator::Update(double dt, TurnManager* turn_manager) {
    if (!turn_manager->IsCombatActive()) {
        return;
    }

    Character* current = turn_manager->GetCurrentCharacter();

    int current_round = turn_manager->GetRoundNumber();
    if (current_round != m_previous_round) {

        m_previous_round = current_round;
    }

    if (current->GetCharacterType() != CharacterTypes::Dragon) {
        HandleAITurn(current);
    }
}

void BattleOrchestrator::HandleAITurn(Character* ai_character) {

    if (ai_character->GetCharacterType() == CharacterTypes::Fighter) {
        Fighter* fighter = static_cast<Fighter*>(ai_character);
        fighter->Action();

        if (!ShouldContinueTurn(fighter)) {
            auto* turn_mgr = GetGSComponent<TurnManager>();
            turn_mgr->EndCurrentTurn();
        }
    }
}

bool BattleOrchestrator::CheckVictoryCondition() {
    return false;
}
