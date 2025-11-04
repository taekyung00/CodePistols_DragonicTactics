// --- 기존 Include (TextManager.hpp만 제거) ---
#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Window.hpp"
// #include "./Engine/TextManager.hpp" // <-- [제거]
#include "GamePlay.h"

// --- 기존 Include 경로 (그대로 유지) ---
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h" // MainMenu Push를 위해 필요
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/MainMenu.h" // Escape 키 로직을 위해 필요

// --- [신규] ImGui와 Input 헤더 추가 ---
#include <imgui.h>
#include "./Engine/Input.hpp"

// [참고] GamePlay.cpp에 Character/Dragon 헤더가 없었지만,
// Load()에서 사용되므로 추가합니다. (기존 Test_Ginam.cpp 참고)
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Fighter.h"
#include "./Game/DragonicTactics/Objects/Dragon.h"


GamePlay::GamePlay():fighter(nullptr), dragon(nullptr){}

void GamePlay::Load(){
    // --- 기존 Load() 로직 (그대로 유지) ---
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
                    fighter = new Fighter(current_pos); // 기존 테스트 생성자 유지
                    go_manager->Add(fighter);
                    grid_system->AddCharacter(fighter, current_pos);
                    break;
                case 'd':
                    grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
                    dragon = new Dragon(current_pos); // 기존 테스트 생성자 유지
                    go_manager->Add(dragon);
                    grid_system->AddCharacter(dragon, current_pos);
                    break;
            }
        }
    }
}

// [제거] void GamePlay::update_button_logic()
// [제거] void GamePlay::update_button_colors()

void GamePlay::Update(double dt){
    // [제거] update_button_logic();
    // [제거] update_button_colors(); 

    CS230::Input& input = Engine::GetInput();

    // [신규] ImGui가 마우스를 사용 중인지 확인
    bool is_clicking_ui = ImGui::GetIO().WantCaptureMouse;

    if (input.MouseJustPressed(0))
    {
        // [수정] ImGui UI를 클릭한 것이 아닐 때만 맵 클릭 로직 실행
        if (!is_clicking_ui)
        {
            // Math::ivec2 grid_pos = ConvertScreenToGrid(mouse_pos);
            switch (currentPlayerState)
            {
                case PlayerActionState::SelectingMove:
                    Engine::GetLogger().LogEvent("Map clicked while in SelectingMove state.");
                    // player->MoveTo(grid_pos);
                    // currentPlayerState = PlayerActionState::None; 
                    break;
                case PlayerActionState::SelectingAction:
                    Engine::GetLogger().LogEvent("Map clicked while in SelectingAction state.");
                    // player->PerformAction(grid_pos);
                    // currentPlayerState = PlayerActionState::None; 
                    break;
                case PlayerActionState::None:
                    break;
            }
        }
    }
    
    // --- 나머지 Update() 로직 (그대로 유지) ---
    if (input.MouseJustPressed(1)) 
    {
        if (currentPlayerState != PlayerActionState::None)
        {
            Engine::GetLogger().LogEvent("Action cancelled via Right Click.");
            currentPlayerState = PlayerActionState::None;
        }
    }

    if (dragon != nullptr && dragon->IsAlive()) // IsAlive()는 전체 코드 분석상 StatsComponent에 있음
    {
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
                // LogDragonStatus(); // 주석 처리
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
    // [수정] 파트너님의 피드백대로 "Unload" 이름을 유지합니다.
    GetGSComponent<CS230::GameObjectManager>()->Unload(); 
    fighter = nullptr;
    dragon  = nullptr;
}

void GamePlay::Draw(){
    // [제거] TextManager 및 textManager.DrawText() 관련 로직 모두 제거

    Engine::GetWindow().Clear(0x1a1a1aff);
    auto& renderer_2d = Engine::GetRenderer2D();
    
    // [신규] DrawAll과 GridSystem::Draw를 위해 카메라 매트릭스가 필요합니다.
    renderer_2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
    
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    if (grid_system != nullptr)
    {
        grid_system->Draw(); 
    }


    renderer_2d.EndScene();
}

void GamePlay::DrawImGui(){
    // --- [신규] ImGui UI 로직 ---
    ImGui::Begin("Player Actions");

    // --- 1. Move 버튼 ---
    const char* move_text = (currentPlayerState == PlayerActionState::SelectingMove) ? "Cancel Move" : "Move";
    bool is_move_disabled = (currentPlayerState == PlayerActionState::SelectingAction);
    
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

    // --- 2. Action 버튼 ---
    const char* action_text = (currentPlayerState == PlayerActionState::SelectingAction) ? "Cancel Action" : "Action";
    bool is_action_disabled = (currentPlayerState == PlayerActionState::SelectingMove);

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

    // --- 3. End Turn 버튼 ---
    bool is_end_turn_disabled = (currentPlayerState != PlayerActionState::None);
    if (is_end_turn_disabled) ImGui::BeginDisabled();
    if (ImGui::Button("End Turn")) {
        Engine::GetLogger().LogEvent("UI: 'End Turn' button clicked.");
        // (Phase 2에서 여기에 로직 추가)
    }
    if (is_end_turn_disabled) ImGui::EndDisabled();

    ImGui::End();
}

gsl::czstring GamePlay::GetName() const{
    return "GamePlay";
}