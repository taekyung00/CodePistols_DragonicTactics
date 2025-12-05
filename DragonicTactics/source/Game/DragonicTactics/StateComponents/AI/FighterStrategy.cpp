/**
 * @file FighterStrategy.cpp
 * @author Sangyun Lee
 * @brief 파이터 AI 구현: 드래곤에게 접근하여 공격
 * @date 2025-12-04
 */
#include "pch.h"
#include "FighterStrategy.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "../../StateComponents/GridSystem.h"
#include "../../StateComponents/CombatSystem.h"
#include "../../Objects/Components/GridPosition.h"
#include "../../Objects/Components/StatsComponent.h"

AIDecision FighterStrategy::MakeDecision(Character* actor) {
    // 0. 타겟 설정 (오직 Dragon만 타겟팅)
    Character* target = FindDragon();
    if (!target) return { AIDecisionType::EndTurn, nullptr, {}, "", "No Dragon found" };

    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    

    // 1. 위험 감지 (HP 30% 이하) - 현재는 공격 유지하지만 추후 도망 로직 추가 가능
    float hpPercent = actor->GetStatsComponent()->GetHealthPercentage();
    if (hpPercent <= 0.3f) {
        // 방어적 행동 로직 (Item 사용 등) 들어갈 자리
    }
    
    // 거리 계산
    int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(), target->GetGridPosition()->Get());
    int attackRange = actor->GetAttackRange();
    bool onTarget = (distance <= attackRange); // 사거리 안인가?

    // 2. 공격 판단 (사거리 내 && AP 있음)
    if (onTarget) {
        if (actor->GetActionPoints() > 0) {
            // 스킬: Shield Bash (인접 시 사용)
            if (distance == 1 && actor->HasSpell("Shield Bash")) {
                 return { AIDecisionType::UseAbility, target, {}, "Shield Bash", "Stunning Dragon" };
            }
            // 일반 공격
            return { AIDecisionType::Attack, target, {}, "", "Basic Attack" };
        }
    } 
    // 3. 이동 판단 (사거리 밖 && AP 있음)
    else {
        if (actor->GetActionPoints() > 0) {
            // 드래곤의 위치가 아니라, 공격 가능한 '빈 옆자리'를 찾음
            Math::ivec2 movePos = FindNextMovePos(actor, target, grid);
            
            // 제자리가 아니라면 이동
            if (movePos != actor->GetGridPosition()->Get()) {
                return { AIDecisionType::Move, nullptr, movePos, "", "Approaching Dragon" };
            }
        }
    }

    return { AIDecisionType::EndTurn, nullptr, {}, "", "End Turn" };
}

Character* FighterStrategy::FindDragon() {
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    auto allChars = grid->GetAllCharacters();
    
    for (auto* c : allChars) {
        // 살아있는 드래곤만 찾음
        if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Dragon) {
            return c;
        }
    }
    return nullptr;
}

Math::ivec2 FighterStrategy::FindNextMovePos(Character* actor, Character* target, GridSystem* grid) {
    Math::ivec2 targetPos = target->GetGridPosition()->Get();
    Math::ivec2 myPos = actor->GetGridPosition()->Get();

    std::vector<Math::ivec2> bestPath;
    int bestPathCost = 999999;

    // 드래곤의 상하좌우 4방향 중 갈 수 있는 가장 가까운 곳 탐색
    static const Math::ivec2 offsets[4] = { {0, 1}, {0, -1}, {-1, 0}, {1, 0} };

    for (const auto& offset : offsets) {
        Math::ivec2 attackPos = targetPos + offset;

        // 맵 밖이거나 막힌 곳이면 패스
        if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos)) {
            continue;
        }

        // 해당 위치까지 경로 계산
        std::vector<Math::ivec2> currentPath = grid->FindPath(myPos, attackPos);

        // 경로가 있고 더 짧다면 갱신
        if (!currentPath.empty() && (int)currentPath.size() < bestPathCost) {
            bestPathCost = (int)currentPath.size();
            bestPath = currentPath;
        }
    }

    // 경로가 존재한다면
    if (!bestPath.empty()) {
        // 내 이동력(Speed) 한계 내에서 가장 멀리 갈 수 있는 칸 선택
        int maxReach = std::min((int)bestPath.size(), actor->GetMovementRange());
        int destIndex = maxReach - 1; 
        
        if (destIndex >= 0) {
            return bestPath[destIndex];
        }
    }

    return myPos; // 갈 곳 없으면 제자리 반환
}