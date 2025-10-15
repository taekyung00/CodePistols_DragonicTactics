/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Dragon.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 15, 2025
*/

#include "Dragon.h"
#include "StatsComponent.h"
#include "ActionPoints.h"
#include "ActionAttack.h" // 기본 공격을 추가하기 위해 포함
#include "../Engine/Engine.h"    // 로그 출력을 위해 포함

Dragon::Dragon(Math::ivec2 start_coordinates)
// 부모인 Character의 생성자를 호출합니다.
// GDD에 명시된 드래곤의 스탯을 정확히 전달합니다.
    : Character(
        CharacterTypes::Dragon,        // 캐릭터 타입
        start_coordinates,             // 시작 위치
        3,                             // 최대 행동력 (GDD에 따르면 3)
        { {1, 4}, {2, 3}, {3, 2}, {4, 2}, {5, 1} } // 1~5레벨 주문 슬롯
    )
{
    // 드래곤의 고유 스탯으로 덮어씁니다.
    CharacterStats dragon_stats;
    dragon_stats.max_hp = 250;
    dragon_stats.current_hp = 250;
    dragon_stats.speed = 4;
    dragon_stats.base_attack = 10;
    dragon_stats.attack_dice = "3d8";
    dragon_stats.base_defend = 5;
    dragon_stats.defend_dice = "2d10";
    dragon_stats.attack_range = 2;

    *GetStatsComponent() = StatsComponent(dragon_stats);

    // 드래곤에게 '기본 공격' 액션을 추가합니다.
    m_action_list.push_back(new ActionAttack());
    // TODO: 나중에 '화염 브레스' 같은 드래곤 전용 Action을 추가해야 합니다.
}

void Dragon::OnTurnStart() {
    // 턴이 시작되면 행동력을 회복합니다.
    ActionPoints* ap = GetActionPointsComponent();
    if (ap != nullptr) {
        ap->Refresh();
    }

    // AI의 두뇌를 깨웁니다.
    DecideAction();
}

void Dragon::OnTurnEnd() {
    // 턴 종료 로직 (예: '화염 지대' 데미지 처리 등)
}

void Dragon::TakeDamage(int damage, Character* attacker) {
    // TODO: 드래곤만의 특별한 피격 로직을 추가합니다.
    // (예: 비행 중일 때 원거리 공격 데미지 반감)

    // 일단은 Character의 기본 TakeDamage 로직을 그대로 사용합니다.
    Character::TakeDamage(damage, attacker);
}

void Dragon::DecideAction() {
    // TODO: dragon.mmd 플로우차트의 복잡한 AI 로직을 여기에 구현해야 합니다.
    // 지금은 가장 단순한 로직만 구현합니다: "무조건 파이터를 공격"

    m_turn_goal = TurnGoal::Attack;
    Engine::GetLogger().LogEvent("Dragon roars and decides to attack!");

    // TODO: m_turn_target을 설정하는 로직 필요
}