#include "pch.h"

#include "DebugVisualizer.h"
#include "DebugManager.h"
#include "./Engine/Logger.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./CS200/IRenderer2D.h"
#include "./Game/DragonicTactics/StateComponents/GridSystem.h"
#include "./Game/DragonicTactics/Objects/Character.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/StateComponents/EventBus.h"
#include "./Game/DragonicTactics/StateComponents/DiceManager.h"
#include "./Game/DragonicTactics/Types/Events.h"

void DebugVisualizer::Init()
{
    Engine::GetLogger().LogEvent("DebugVisualizer: Subscribing to events");
    
    auto* event_bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
    
    // Subscribe to combat events
    event_bus->Subscribe<CharacterDamagedEvent>([this](const CharacterDamagedEvent& e) {
        OnCharacterDamaged(e);
    });
    
    event_bus->Subscribe<CharacterDeathEvent>([this](const CharacterDeathEvent& e) {
        OnCharacterDeath(e);
    });
    
    // Subscribe to movement events
    event_bus->Subscribe<AIDecisionEvent>([this](const AIDecisionEvent& e) {
        OnAIDecision(e);
    });

    // Subscribe to turn events
    event_bus->Subscribe<TurnStartedEvent>([this](const TurnStartedEvent& e) {
        OnTurnStarted(e);
    });
    
    Engine::GetLogger().LogEvent("DebugVisualizer: Event subscriptions complete");
}

void DebugVisualizer::Update(double dt)
{
    game_time_ += dt;
    
    // Update damage number lifetimes
    for (auto& dmg : recent_damage_) {
        dmg.lifetime -= dt;
    }
    
    // Remove expired damage numbers
    recent_damage_.erase(
        std::remove_if(recent_damage_.begin(), recent_damage_.end(),
            [](const DamageInfo& d) { return d.lifetime <= 0.0; }),
        recent_damage_.end()
    );
    
    // Keep only recent events (last 10 seconds)
    while (!event_log_.empty() && (game_time_ - event_log_.front().timestamp) > 10.0) {
        event_log_.pop_front();
    }
    
    while (!recent_moves_.empty() && (game_time_ - recent_moves_.front().timestamp) > 5.0) {
        recent_moves_.pop_front();
    }
    
    // Track dice rolls from DiceManager
    auto* dice_mgr = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
    if (dice_mgr) {
        const auto& last_rolls = dice_mgr->GetLastRolls();
        if (!last_rolls.empty() && (dice_history_.empty() || dice_history_.back().rolls != last_rolls)) {
        // New roll detected - store it
        DiceRollInfo roll_info;
        roll_info.rolls = last_rolls;
        roll_info.total = 0;
        for (int r : last_rolls) roll_info.total += r;
        roll_info.notation = "Unknown"; // We don't have notation here, just the results
        roll_info.timestamp = game_time_;
        
        dice_history_.push_back(roll_info);
        
        // Keep last 20 rolls
        if (dice_history_.size() > 20) {
            dice_history_.pop_front();
        }
    }
    }
}

void DebugVisualizer::DrawGridOverlay(const GridSystem* grid)
{
    auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
    if (!enabled_ || !debug_mgr || !debug_mgr->IsGridOverlayEnabled() || grid == nullptr) {
        return;
    }
    
    auto& renderer = Engine::GetRenderer2D();
    
    const int TILE_SIZE = GridSystem::TILE_SIZE;
    const int MAP_WIDTH = 8;
    const int MAP_HEIGHT = 8;
    
    CS200::RGBA grid_color = 0xFFFFFF44; // Semi-transparent white
    
    // Vertical lines
    for (int x = 0; x <= MAP_WIDTH; ++x) {
        int screen_x = x * TILE_SIZE + TILE_SIZE;
        renderer.DrawLine(
            Math::vec2{static_cast<float>(screen_x), static_cast<float>(TILE_SIZE)},
            Math::vec2{static_cast<float>(screen_x), static_cast<float>((MAP_HEIGHT + 1) * TILE_SIZE)},
            grid_color
        );
    }
    
    // Horizontal lines
    for (int y = 0; y <= MAP_HEIGHT; ++y) {
        int screen_y = y * TILE_SIZE + TILE_SIZE;
        renderer.DrawLine(
            Math::vec2{static_cast<float>(TILE_SIZE), static_cast<float>(screen_y)},
            Math::vec2{static_cast<float>((MAP_WIDTH + 1) * TILE_SIZE), static_cast<float>(screen_y)},
            grid_color
        );
    }
}

// DrawDebugInfo removed - all debug info now in ImGui panel only

void DebugVisualizer::DrawImGuiDebugPanel(const GridSystem* grid)
{
    if (!show_debug_panel_) {
        return;
    }
    
    ImGui::SetNextWindowSize(ImVec2(650, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(300, 10), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Events & Stats", &show_debug_panel_)) {
        
        if (ImGui::BeginTabBar("DebugTabs")) {
            
            // Tab 1: Event Log
            if (ImGui::BeginTabItem("Event Log")) {
                DrawImGuiEventLog();
                ImGui::EndTabItem();
            }
            
            // Tab 2: Dice History
            if (ImGui::BeginTabItem("Dice History")) {
                DrawImGuiDiceHistory();
                ImGui::EndTabItem();
            }
            
            // Tab 3: Character Stats
            if (ImGui::BeginTabItem("Characters")) {
                DrawImGuiCharacterStats(grid);
                ImGui::EndTabItem();
            }
            
            // Tab 4: Combat Log
            if (ImGui::BeginTabItem("Combat")) {
                DrawImGuiCombatLog();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

// DrawCharacterStatusBars removed - HP bars should be part of game UI, not debug
// All debug info now shown in ImGui panel only

// === ImGui Panel Functions ===

void DebugVisualizer::DrawImGuiEventLog()
{
    ImGui::Text("Recent Events (Last 10 seconds)");
    ImGui::Separator();
    
    ImGui::BeginChild("EventLogScroll", ImVec2(0, 0), false);
    
    for (auto it = event_log_.rbegin(); it != event_log_.rend(); ++it) {
        const auto& entry = *it;
        
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (entry.event_type == "Damage") {
            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        } else if (entry.event_type == "Death") {
            color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        } else if (entry.event_type == "Spell") {
            color = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
        } else if (entry.event_type == "Turn") {
            color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
        }
        
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("[%.1fs] %s: %s", entry.timestamp, entry.event_type.c_str(), entry.details.c_str());
        ImGui::PopStyleColor();
    }
    
    ImGui::EndChild();
}

void DebugVisualizer::DrawImGuiDiceHistory()
{
    ImGui::Text("Recent Dice Rolls (Last 20)");
    ImGui::Separator();
    
    ImGui::BeginChild("DiceHistoryScroll", ImVec2(0, 0), false);
    
    for (auto it = dice_history_.rbegin(); it != dice_history_.rend(); ++it) {
        const auto& roll = *it;
        
        ImGui::Text("[%.1fs] Total: %d", roll.timestamp, roll.total);
        ImGui::SameLine(150);
        ImGui::Text("Rolls: [");
        ImGui::SameLine();
        
        for (size_t i = 0; i < roll.rolls.size(); ++i) {
            ImGui::SameLine();
            ImGui::Text("%d", roll.rolls[i]);
            if (i + 1 < roll.rolls.size()) {
                ImGui::SameLine();
                ImGui::Text(",");
            }
        }
        ImGui::SameLine();
        ImGui::Text("]");
    }
    
    ImGui::EndChild();
}

void DebugVisualizer::DrawImGuiCharacterStats(const GridSystem* grid)
{
    if (grid == nullptr) {
        ImGui::Text("No grid available");
        return;
    }
    
    ImGui::Text("All Characters on Grid");
    ImGui::Separator();
    
    const int MAP_WIDTH = 8;
    const int MAP_HEIGHT = 8;
    
    ImGui::BeginChild("CharacterStatsScroll", ImVec2(0, 0), false);
    
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Math::ivec2 grid_pos = {x, y};
            Character* character = grid->GetCharacterAt(grid_pos);
            
            if (character == nullptr) {
                continue;
            }
            
            ImGui::PushID(character);
            
            if (ImGui::CollapsingHeader(character->TypeName().c_str())) {
                ImGui::Indent();
                
                ImGui::Text("Position: (%d, %d)", grid_pos.x, grid_pos.y);
                ImGui::Text("Status: %s", character->IsAlive() ? "Alive" : "Dead");
                
                StatsComponent* stats = character->GetStatsComponent();
                if (stats != nullptr) {
                    ImGui::Text("HP: %d / %d", stats->GetCurrentHP(), stats->GetMaxHP());
					ImGui::Text("Attack: %d", stats->GetBaseAttack());
					ImGui::Text("Speed: %d", stats->GetSpeed());
                    ImGui::Text("Attack Range: %d", stats->GetAttackRange());
                    ImGui::Text("Dice: %s", stats->GetAttackDice().c_str());
                }
                
                auto* ap_comp = character->GetActionPointsComponent();
                if (ap_comp != nullptr) {
                    ImGui::Text("AP: %d / %d", ap_comp->GetCurrentPoints(), ap_comp->GetMaxPoints());
                }
                
                ImGui::Unindent();
            }
            
            ImGui::PopID();
        }
    }
    
    ImGui::EndChild();
}

void DebugVisualizer::DrawImGuiCombatLog()
{
    ImGui::Text("Combat Events");
    ImGui::Separator();
    
    ImGui::BeginChild("CombatLogScroll", ImVec2(0, 0), false);
    
    // Filter only combat-related events
    for (auto it = event_log_.rbegin(); it != event_log_.rend(); ++it) {
        const auto& entry = *it;
        
        if (entry.event_type == "Damage" || entry.event_type == "Death") {
            ImVec4 color = entry.event_type == "Death" ? 
                ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.6f, 0.4f, 1.0f);
            
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("[%.1fs] %s", entry.timestamp, entry.details.c_str());
            ImGui::PopStyleColor();
        }
    }
    
    // Also show recent damage numbers
    ImGui::Separator();
    ImGui::Text("Active Damage Numbers:");
    for (const auto& dmg : recent_damage_) {
        if (dmg.attacker != nullptr && dmg.target != nullptr) {
            ImGui::Text("  %s -> %s: %d damage (%.1fs left)", 
                dmg.attacker->TypeName().c_str(),
                dmg.target->TypeName().c_str(),
                dmg.damage,
                dmg.lifetime);
        }
    }
    
    ImGui::EndChild();
}

// === Event Handlers ===

void DebugVisualizer::OnCharacterDamaged(const CharacterDamagedEvent& event)
{
    if (event.target == nullptr) return;
    
    // Store damage info for visualization
    DamageInfo dmg;
    dmg.target = event.target;
    dmg.attacker = event.attacker;
    dmg.damage = event.damageAmount;
    dmg.position = event.target->GetGridPosition()->Get();
    dmg.lifetime = 5.0; // Show for 2 seconds
    
    recent_damage_.push_back(dmg);
    
    // Log event
    std::string attacker_name = event.attacker ? event.attacker->TypeName() : "Unknown";
    std::string details = attacker_name + " dealt " + std::to_string(event.damageAmount) + 
                         " damage to " + event.target->TypeName();
    
    EventLogEntry log;
    log.event_type = "Damage";
    log.details = details;
    log.timestamp = game_time_;
    event_log_.push_back(log);
    
    auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
    if (debug_mgr && debug_mgr->IsEventTracerEnabled()) {
        Engine::GetLogger().LogDebug("[EVENT] " + details);
    }
}

void DebugVisualizer::OnCharacterDeath(const CharacterDeathEvent& event)
{
    if (event.character == nullptr) return;
    
    std::string killer_name = event.killer ? event.killer->TypeName() : "Unknown";
    std::string details = event.character->TypeName() + " was killed by " + killer_name;
    
    EventLogEntry log;
    log.event_type = "Death";
    log.details = details;
    log.timestamp = game_time_;
    event_log_.push_back(log);
    
    auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
    if (debug_mgr && debug_mgr->IsEventTracerEnabled()) {
        Engine::GetLogger().LogDebug("[EVENT] " + details);
    }
}

void DebugVisualizer::OnTurnStarted(const TurnStartedEvent& event)
{
    if (event.character == nullptr) return;
    
    std::string details = "Turn " + std::to_string(event.turnNumber) + ": " +
                         event.character->TypeName() + " (" + 
                         std::to_string(event.actionPoints) + " AP)";
    
    EventLogEntry log;
    log.event_type = "Turn";
    log.details = details;
    log.timestamp = game_time_;
    event_log_.push_back(log);
    
    auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
    if (debug_mgr && debug_mgr->IsEventTracerEnabled()) {
        Engine::GetLogger().LogDebug("[EVENT] " + details);
    }
}

void DebugVisualizer::OnAIDecision(const AIDecisionEvent& event)
{
    AIDecisionLog log;
    log.timestamp = game_time_;
    log.actorName = event.actor ? event.actor->TypeName() : "Unknown";
    log.actionType = GetDecisionTypeString(event.decision_type);
    log.reasoning = event.decision_reasoning;
    
    // 타겟 정보 저장
    if (event.decision_type == AIDecisionType::Attack || event.decision_type == AIDecisionType::UseAbility) {
        log.targetName = event.decision_target ? event.decision_target->TypeName() : "None";
        log.destination = { -1, -1 };
    } 
    else if (event.decision_type == AIDecisionType::Move) {
        // Move 타입은 destination 정보가 필요하지만, 현재 이벤트 구조체에는 없을 수 있음.
        // (필요하다면 AIDecisionEvent 구조체에 destination 필드 추가 권장)
        log.targetName = "-";
        // log.destination = event.destination; // 구조체에 추가 후 사용
    }
    else {
        log.targetName = "-";
    }

    ai_decision_history_.push_back(log);
    
    std::string summary = "[AI] " + log.actorName + " decided to " + log.actionType;
    EventLogEntry entry { "AI", summary, game_time_ };
    event_log_.push_back(entry);
}

void DebugVisualizer::DrawImGuiAIDecisions()
{
    if (ImGui::Button("Clear Log")) {
        ai_decision_history_.clear();
    }
    
    // 테이블 컬럼 설정
    if (ImGui::BeginTable("AITable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Actor", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Decision", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Reasoning (Why?)", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        // 최신 로그가 위에 오도록 역순 순회
        for (auto it = ai_decision_history_.rbegin(); it != ai_decision_history_.rend(); ++it) {
            ImGui::TableNextRow();
            
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%.1fs", it->timestamp);
            
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%s", it->actorName.c_str()); // 파란색
            
            ImGui::TableSetColumnIndex(2);
            // 행동 타입에 따라 색상 다르게
            ImVec4 typeColor = ImVec4(1, 1, 1, 1);
            if (it->actionType == "Attack") typeColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); // 빨강
            else if (it->actionType == "Move") typeColor = ImVec4(1.0f, 1.0f, 0.4f, 1.0f); // 노랑
            else if (it->actionType == "Heal") typeColor = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); // 초록
            
            ImGui::TextColored(typeColor, "%s", it->actionType.c_str());
            if (it->targetName != "-" && it->targetName != "None") {
                ImGui::SameLine();
                ImGui::TextDisabled("-> %s", it->targetName.c_str());
            }
            
            ImGui::TableSetColumnIndex(3);
            ImGui::TextWrapped("%s", it->reasoning.c_str());
        }
        ImGui::EndTable();
    }
}

std::string DebugVisualizer::GetDecisionTypeString(AIDecisionType type) {
    switch (type) {
        case AIDecisionType::Move: return "Move";
        case AIDecisionType::Attack: return "Attack";
        case AIDecisionType::UseAbility: return "Ability";
        case AIDecisionType::EndTurn: return "EndTurn";
        default: return "None";
    }
}