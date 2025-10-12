/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Fighter.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 12, 2025
*/

#include "Fighter.h"
#include "ActionPoints.h" // OnTurnStart에서 Refresh를 호출하기 위해 필요
#include "StatsComponent.h" // 스탯을 초기화하기 위해 필요
#include "../Engine/Engine.h" // 로그 출력을 위해 포함

Fighter::Fighter(Math::ivec2 start_coordinates)
// 부모인 Character의 생성자를 호출합니다.
// GDD에 명시된 파이터의 스탯을 정확히 전달합니다.
    : Character(
      CharacterTypes::Fighter,      // 캐릭터 타입
      start_coordinates,            // 시작 위치
      2,                            // 최대 행동력
      { { 1, 2 }, { 2, 2 }}
    )              // 주문 슬롯: 1레벨 2개, 2레벨 2개
{
    // 컴포넌트 초기화가 끝난 후, 파이터의 고유 스탯으로 덮어씁니다.
    // TODO: 나중에 이 부분을 JSON 같은 데이터 파일에서 스탯을 읽어오는
    //       '데이터 로딩 시스템'으로 대체해야 합니다.
    CharacterStats fighter_stats;
    fighter_stats.max_hp = 90; 
    fighter_stats.current_hp = 90;
    fighter_stats.speed = 3;
    fighter_stats.base_attack = 5; 
    fighter_stats.attack_dice = "2d6";
    fighter_stats.base_defend = 0; // GDD에는 기본 방어력이 명시되어 있지 않습니다.
    fighter_stats.defend_dice = "1d10";
    fighter_stats.attack_range = 1;

        // 이미 생성된 StatsComponent를 찾아서 스탯을 덮어씁니다.
        // (StatsComponent에 SetStats 같은 함수를 만들면 더 깔끔해집니다.)
        * GetStatsComponent() = StatsComponent(fighter_stats);
}

void Fighter::OnTurnStart() {
    // 턴이 시작되면 행동력을 회복합니다.
    ActionPoints* ap = GetActionPointsComponent();
    if (ap != nullptr) {
        ap->Refresh();
    }

    // AI의 두뇌를 깨워 이번 턴에 무엇을 할지 결정하게 합니다.
    DecideAction();
}

void Fighter::OnTurnEnd() {
    // 턴 종료 시 특별한 로직이 있다면 여기에 작성합니다.
    // (예: 턴 종료 시 발동하는 패시브 스킬)
}

void Fighter::TakeDamage(int damage, Character* attacker) {
    // Character의 기본 TakeDamage 로직을 먼저 호출합니다.
    Character::TakeDamage(damage, attacker);

    // 파이터만의 특별한 피격 로직이 있다면 여기에 추가합니다.
    // (예: 체력이 50% 이하일 때 '분노' 버프 획득)
}

void Fighter::DecideAction() {
    // fighter.mmd 플로우차트의 AI 로직을 여기에 구현합니다. [cite: 1-14]

    // --- 1. 목표 설정 단계 ---

    // TODO: "도망쳐야 하는 상황인가?"를 판단하는 로직이 필요합니다.
    // bool is_flee_condition_met = ...;
    // if (is_flee_condition_met) {
    //     m_turn_goal = TurnGoal::Flee;
    //     Engine::GetLogger().LogEvent("Fighter: Time to run!");
    //     return;
    // }

    // 내 체력이 30% 이하인가?
    if (GetStatsComponent()->GetHealthPercentage() <= 0.3f) {
       
        // TODO: "클레릭이 살아 있는가?"를 판단하는 로직이 필요합니다.
        //       (TargetingSystem 컴포넌트가 이 역할을 할 수 있습니다.)
        // bool is_cleric_alive = ...;
        // if (is_cleric_alive) {
        //     m_turn_goal = TurnGoal::HealAlly; // 목표 = 클레릭에게 가서 치유 요청
        //     Engine::GetLogger().LogEvent("Fighter: I need healing!");
        // } else {
        //     m_turn_goal = TurnGoal::Attack; // 목표 = 드래곤 공격
        //     Engine::GetLogger().LogEvent("Fighter: For glory!");
        // }
    }
    else {
        // 내 체력이 충분하면 목표는 항상 드래곤 공격입니다
        m_turn_goal = TurnGoal::Attack;
        Engine::GetLogger().LogEvent("Fighter: Charging the Dragon!");
    }

    // TODO: m_turn_target을 설정하는 로직이 필요합니다.
    // (예: m_turn_target = FindObjectByType<Dragon>();)

    // --- 2. 행동 실행 단계 ---
    // 실제 이동 및 공격 로직은 Character::Update() 안에서 
    // m_turn_goal과 m_turn_target을 보고 수행하게 됩니다.
}