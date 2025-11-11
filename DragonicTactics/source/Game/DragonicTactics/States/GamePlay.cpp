/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlay.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 5, 2025
*/
#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Window.hpp"
#include "GamePlay.h"

#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h" 
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/MainMenu.h" 

#include <imgui.h>
#include "./Engine/Input.hpp"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"

std::vector<Math::ivec2> GamePlay::CalculateSimplePath(Math::ivec2 start, Math::ivec2 end)
{
    std::vector<Math::ivec2> path;
    Math::ivec2 current = start;


    GridSystem* grid = GetGSComponent<GridSystem>();
    if (grid == nullptr)
    {
        Engine::GetLogger().LogError("CalculateSimplePath: GridSystem is null!");
        return path;
    }


    int safety_break = 0; 
    while (current != end && safety_break < 500)
    {
        safety_break++;
        Math::ivec2 next_step = current;


        if (current.x < end.x) {
            next_step.x++;
        } else if (current.x > end.x) {
            next_step.x--;
        }

        else if (current.y < end.y) {
            next_step.y++;
        } else if (current.y > end.y) {
            next_step.y--;
        }


        if (grid->IsWalkable(next_step))
        {
            path.push_back(next_step);
            current = next_step;
        }
        else
        {

            Engine::GetLogger().LogError("Simple path blocked at (" + 
                std::to_string(next_step.x) + ", " + std::to_string(next_step.y) + "). Stopping path.");
            return path;
        }
    }
    
    if (safety_break >= 500) {
        Engine::GetLogger().LogError("CalculateSimplePath: Hit safety break, possible infinite loop!");
    }


    return path;
}

GamePlay::GamePlay():fighter(nullptr), dragon(nullptr){}

void GamePlay::Load(){
    AddGSComponent(new CS230::GameObjectManager());
    
    AddGSComponent(new GridSystem());
    CS230::GameObjectManager* go_manager = GetGSComponent<CS230::GameObjectManager>();

    GridSystem* grid_system = GetGSComponent<GridSystem>();

    const std::vector<std::string> map_data = { "wwwwwwww", "weefeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weeeeeew", "weedeeew", "wwwwwwww" };

    for (int y = 0; y < map_data.size(); ++y)
    {
        for (int x = 0; x < map_data[y].length(); ++x)
        {
            char tile_char = map_data[y][x];
            Math::ivec2 current_pos = { x, static_cast<int>(map_data.size()) - 1 - y };
            switch (tile_char)
            {
                case 'w': grid_system->SetTileType(current_pos, GridSystem::TileType::Wall); break;
                case 'e': grid_system->SetTileType(current_pos, GridSystem::TileType::Empty); break;
                case 'f':
                    grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
                    fighter = new Fighter(current_pos); 
                    fighter->SetGridSystem(grid_system);
                    go_manager->Add(fighter);
                    grid_system->AddCharacter(fighter, current_pos);
                    break;
                case 'd':
                    grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
                    dragon = new Dragon(current_pos); 
                    dragon->SetGridSystem(grid_system);
                    go_manager->Add(dragon);
                    grid_system->AddCharacter(dragon, current_pos);
                    break;
            }
        }
    }
    
    AddGSComponent(new TurnManager());
    GetGSComponent<TurnManager>()->InitializeTurnOrder({fighter, dragon});
}

Math::ivec2 ConvertScreenToGrid(Math::vec2 world_pos)
{
    int grid_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
    int grid_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
    return { grid_x, grid_y };
}

void GamePlay::Update(double dt){

    CS230::Input& input = Engine::GetInput();
    bool is_clicking_ui = ImGui::GetIO().WantCaptureMouse;

    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    bool isPlayerTurn = false;
    if (turnMgr && turnMgr->IsCombatActive()) {
        Character* current = turnMgr->GetCurrentCharacter();
        isPlayerTurn = (current == dragon);
    }

    if(!isPlayerTurn){
        if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<MainMenu>();
        }

        GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
        return;
    }

    if (input.MouseJustPressed(0))
    {
        Engine::GetLogger().LogEvent("Map clicked at (" + std::to_string(input.GetMousePos().x) + ", " + std::to_string(input.GetMousePos().y) + ")");

        if (!is_clicking_ui)
        {
            Math::vec2 mouse_pos = input.GetMousePos();
            Math::ivec2 grid_pos = ConvertScreenToGrid(mouse_pos);
            
            GridSystem* grid_system = GetGSComponent<GridSystem>();
            switch (currentPlayerState)
            {
                case PlayerActionState::SelectingMove:
                    Engine::GetLogger().LogEvent("Map clicked while in SelectingMove state.");
                    // player->MoveTo(grid_pos);
                    // currentPlayerState = PlayerActionState::None; 
                    if (grid_system->IsWalkable(grid_pos))
                    {
                        std::vector<Math::ivec2> new_path = CalculateSimplePath(dragon->GetGridPosition()->Get(), grid_pos);
                        dragon->SetPath(std::move(new_path));
                    }
                    currentPlayerState = PlayerActionState::None;
                    break;
                case PlayerActionState::SelectingAction:
                    Engine::GetLogger().LogEvent("Map clicked while in SelectingAction state.");
                    // player->PerformAction(grid_pos);
                    // currentPlayerState = PlayerActionState::None; 
                    break;
                case PlayerActionState::TargetingForAttack:
                {
                    Engine::GetLogger().LogEvent("Map clicked for Attack at (" + std::to_string(grid_pos.x) + ", " + std::to_string(grid_pos.y) + ")");
                    Character* target = grid_system->GetCharacterAt(grid_pos);

                    if (target != nullptr && target != dragon)
                    {
                        if (dragon->GetActionPoints() > 0)
                        {
                            dragon->PerformAttack(fighter);
                        }
                        else
                        {
                            Engine::GetLogger().LogDebug("Dragon has no Action Points to attack!");
                        }
                        currentPlayerState = PlayerActionState::None; 
                    }
                    else
                    {
                        Engine::GetLogger().LogEvent("Attack FAILED. Invalid target.");
                    }
                    break;
                }

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
                case PlayerActionState::None:
                    break;
            }
        }
    }
    
    if (input.MouseJustPressed(1)) 
    {
        if (currentPlayerState != PlayerActionState::None)
        {
            Engine::GetLogger().LogEvent("Action cancelled via Right Click.");
            currentPlayerState = PlayerActionState::None;
        }
    }

    if (dragon != nullptr && dragon->IsAlive()) {
        Math::ivec2 current_pos    = dragon->GetGridPosition()->Get();
        Math::ivec2 target_pos     = current_pos;
        bool        move_requested = false;

        if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
            target_pos.y++; move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Down)) {
            target_pos.y--; move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Left)) {
            target_pos.x--; move_requested = true;
        }
        else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Right)) {
            target_pos.x++; move_requested = true;
        }

        if (move_requested) {
            GridSystem* grid = GetGSComponent<GridSystem>();
            if (grid != nullptr && grid->IsWalkable(target_pos)) {
                grid->MoveCharacter(current_pos, target_pos);
                dragon->GetGridPosition()->Set(target_pos);
                dragon->SetPosition({ static_cast<double>(target_pos.x * GridSystem::TILE_SIZE), static_cast<double>(target_pos.y * GridSystem::TILE_SIZE) });
                Engine::GetLogger().LogEvent("Dragon moved to (" + std::to_string(target_pos.x) + ", " + std::to_string(target_pos.y) + ")");
               
            }
            else {
                Engine::GetLogger().LogEvent("Dragon cannot move there! (Wall or Occupied)");
            }
        }
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}

void GamePlay::Unload(){
    GetGSComponent<CS230::GameObjectManager>()->Unload(); 
    fighter = nullptr;
    dragon  = nullptr;
}

void GamePlay::Draw(){

    Engine::GetWindow().Clear(0x1a1a1aff);
    auto& renderer_2d = Engine::GetRenderer2D();
    
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
    
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    if (grid_system != nullptr)
    {
        grid_system->Draw(); 
    }


    renderer_2d.EndScene();
}

void GamePlay::DrawImGui(){
    TurnManager* turnMgr = GetGSComponent<TurnManager>();
    if (turnMgr && turnMgr->IsCombatActive()) {
        ImGui::Begin("Combat Status");
        Character* current = turnMgr->GetCurrentCharacter();
        if (current) {
            ImGui::Text("Current Turn: %s", current->TypeName().c_str());
            ImGui::Text("Turn #%d | Round #%d", 
                       turnMgr->GetCurrentTurnNumber(), 
                       turnMgr->GetRoundNumber());
            ImGui::Text("Initiative: %d", 
                       turnMgr->GetInitiativeValue(current));
        }
        ImGui::End();
    }

    ImGui::Begin("Player Actions");

    const char* move_text = (currentPlayerState == PlayerActionState::SelectingMove) ? "Cancel Move" : "Move";
    bool is_move_disabled = (currentPlayerState != PlayerActionState::None && 
                             currentPlayerState != PlayerActionState::SelectingMove); 
    if (is_move_disabled) ImGui::BeginDisabled();
    if (ImGui::Button(move_text)) {
        if (currentPlayerState == PlayerActionState::SelectingMove) {
            currentPlayerState = PlayerActionState::None;
            Engine::GetLogger().LogEvent("UI: 'Cancel Move' button clicked.");
        } else {
            currentPlayerState = PlayerActionState::SelectingMove;
            Engine::GetLogger().LogEvent("UI: 'Move' button clicked.");
        }
    }
    if (is_move_disabled) ImGui::EndDisabled();

    const char* action_text = (currentPlayerState == PlayerActionState::SelectingAction) ? "Cancel Action" : "Action";
    bool is_action_disabled = (currentPlayerState != PlayerActionState::None && 
                               currentPlayerState != PlayerActionState::SelectingAction);
    if (is_action_disabled) ImGui::BeginDisabled();
    if (ImGui::Button(action_text)) {
        if (currentPlayerState == PlayerActionState::SelectingAction) {
            currentPlayerState = PlayerActionState::None;
            Engine::GetLogger().LogEvent("UI: 'Cancel Action' button clicked.");
        } else {
            currentPlayerState = PlayerActionState::SelectingAction;
            Engine::GetLogger().LogEvent("UI: 'Action' button clicked.");
        }
    }
    if (is_action_disabled) ImGui::EndDisabled();

    bool is_end_turn_disabled = (currentPlayerState != PlayerActionState::None);
    if (is_end_turn_disabled) ImGui::BeginDisabled();
    if (ImGui::Button("End Turn")) {
        Engine::GetLogger().LogEvent("UI: 'End Turn' button clicked.");
        if (turnMgr && turnMgr->IsCombatActive()) {
            turnMgr->EndCurrentTurn();
        }
    }
    if (is_end_turn_disabled) ImGui::EndDisabled();

    ImGui::End();
    if (currentPlayerState == PlayerActionState::SelectingAction)
    {
        ImGui::Begin("Action List");

        if (ImGui::Button("Attack"))
        {
            Engine::GetLogger().LogEvent("UI: 'Attack' selected. Now targeting.");
            currentPlayerState = PlayerActionState::TargetingForAttack;
        }

        if (ImGui::Button("Spell"))
        {
            Engine::GetLogger().LogEvent("UI: 'Spell' selected. Now targeting.");
            currentPlayerState = PlayerActionState::TargetingForSpell;
        }

        ImGui::End();
    }
}

gsl::czstring GamePlay::GetName() const{
    return "GamePlay";
}