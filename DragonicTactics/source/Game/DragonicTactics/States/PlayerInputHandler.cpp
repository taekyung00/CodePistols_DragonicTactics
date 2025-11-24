/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  PlayerInputHandler.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/

//#include "pch.h"
#include "PlayerInputHandler.h"
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

Math::ivec2 ConvertScreenToGrid(Math::vec2 world_pos)
{
	int grid_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
	int grid_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
	return { grid_x, grid_y };
}

PlayerInputHandler::PlayerInputHandler() : m_state(ActionState::None) {}

void PlayerInputHandler::Update(double dt, Character* current_character, GridSystem* grid) {
    if (current_character->GetCharacterType() != CharacterTypes::Dragon) {
        return; 
    }

    Dragon* dragon = static_cast<Dragon*>(current_character);
    HandleDragonInput(dt, dragon, grid);
}

void PlayerInputHandler::HandleDragonInput(double dt, Dragon* dragon, GridSystem* grid) {
    auto& input = Engine::GetInput();
    bool is_clicking_ui = ImGui::GetIO().WantCaptureMouse;

    if (input.MouseJustPressed(2)) {
        HandleRightClick(dragon);
        return;
    }

    if (m_state == ActionState::Moving) {
        MovementComponent* move_comp = dragon->GetGOComponent<MovementComponent>();
        if (move_comp && !move_comp->IsMoving()) {
            Engine::GetLogger().LogEvent("Movement finished.");
            m_state = ActionState::None;
        }
        return;
    }

    if (input.MouseJustPressed(0) && !is_clicking_ui) {
        Math::vec2 mouse_pos = input.GetMousePos();
        HandleMouseClick(mouse_pos, dragon, grid);
    }
}

void PlayerInputHandler::HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon, GridSystem* grid) {
    Math::ivec2 grid_pos = ConvertScreenToGrid(mouse_pos);

    switch (m_state) {
        case ActionState::SelectingMove:
            if (grid->IsWalkable(grid_pos)) {
                auto path = grid->FindPath(dragon->GetGridPosition()->Get(), grid_pos);
                dragon->SetPath(std::move(path));
                m_state = ActionState::Moving;
            }
            break;

        case ActionState::TargetingForAttack:
            Character* target = grid->GetCharacterAt(grid_pos);
            if (target && target != dragon) {
                auto* combat = GetGSComponent<CombatSystem>();
                combat->ExecuteAttack(dragon, target);
                m_state = ActionState::None;
            }
            break;

        case PlayerActionState::TargetingForSpell:
							{
								Engine::GetLogger().LogEvent("Map clicked for Spell at (" + std::to_string(grid_pos.x) + ", " + std::to_string(grid_pos.y) + ")");

								if (grid_system->IsWalkable(grid_pos))
								{
									Engine::GetLogger().LogEvent("Spell CAST! On tile.");

									// grid_system->ApplyEffectToTile(grid_pos, "Fire");

									currentPlayerState = PlayerActionState::None;
								}
								else
								{
									Engine::GetLogger().LogEvent("Spell FAILED. Invalid tile.");
								}
								break;
							}
						case PlayerActionState::None: break;
    }
}

void PlayerInputHandler::HandleRightClick(Dragon* dragon) {
    if (m_state == ActionState::Moving) {
        dragon->GetGOComponent<MovementComponent>()->ClearPath();
    }
    m_state = ActionState::None;
}

void PlayerInputHandler::CancelCurrentAction() {
    m_state = ActionState::None;
}
