/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Character.cpp
Project:    GAM200
Author:     Seungju Song & Gemini
Created:    Oct 08, 2025
Updated:    Oct 10, 2025
*/

#include "Character.h"
#include"../Engine/GameObject.h"
#include "StatsComponent.h" // StatsComponent를 사용하기 위해 include합니다.
#include "GridPosition.h"// GridPosition를 사용하기 위해 include합니다.

// Character 생성자: 초기화 리스트를 사용하여 멤버 변수들을 초기화합니다.
Character::Character(CharacterTypes charType, Math::ivec2 start_coordinates)
    : CS230::GameObject(Math::vec2{ 0, 0 }), // TODO: 그리드 좌표를 실제 월드 좌표로 변환하는 로직 필요
    m_character_type(charType)
{
    // 생성자에서 컴포넌트 초기화 함수를 호출합니다.
    InitializeComponents(start_coordinates);
}

// 컴포넌트들을 생성하고 GameObject에 추가하는 함수입니다.
void Character::InitializeComponents(Math::ivec2 start_coordinates) {
    // TODO: 각 캐릭터 타입(Dragon, Fighter 등)에 맞는 CharacterStats를 GDD에서 가져와야 합니다.
    // 우선 임시 스탯으로 StatsComponent를 생성합니다.
    CharacterStats initial_stats; // 임시 데이터

    // StatsComponent를 생성하고 이 GameObject에 추가합니다.
    AddGOComponent(new StatsComponent(initial_stats));
    AddGOComponent(new GridPosition(start_coordinates));
    // 다른 컴포넌트들도 여기서 추가합니다.

    // AddGOComponent(new ActionPoints(...));
}

void Character::Update(double dt) {
    // 이동 로직을 먼저 업데이트합니다.
    if (m_is_moving) {
        UpdateMovement(dt);
    }

    // 엔진의 GameObject::Update()를 호출하여 모든 컴포넌트의 Update를 실행합니다.
    CS230::GameObject::Update(dt);
}

void Character::Draw(Math::TransformationMatrix camera_matrix) {
    // 엔진의 GameObject::Draw()를 호출하여 Sprite 등을 그립니다.
    CS230::GameObject::Draw(camera_matrix);
}

// --- 핵심 행동 함수 구현 ---


void Character::PerformAttack(Character* target) {
    // TODO: 기본 공격 로직 구현
    // 1. GetStatsComponent()로 내 공격력 정보를 가져옵니다.
    // 2. DamageCalculator를 이용해 최종 피해량을 계산합니다.
    // 3. target->TakeDamage()를 호출합니다.
    // 4. GetActionPointsComponent()->Consume(1) 같은 코드로 행동력을 소모합니다.
}

void Character::PerformAction(Action* action, Character* target, Math::ivec2 tile_position) {
    // TODO: 스킬/주문 사용 로직 구현
    // 1. action->CanExecute()로 사용 가능한지 확인합니다.
    // 2. action->Execute()를 호출하여 스킬 효과를 적용합니다.
    // 3. 필요하다면 행동력과 주문 슬롯을 소모합니다.
}

void Character::ReceiveHeal(int amount) {
    // 치유 처리 로직을 StatsComponent에 위임합니다.
    if (GetStatsComponent() != nullptr) {
        GetStatsComponent()->Heal(amount);
    }
    // TODO: 치유 시각 효과(파티클) 등을 여기서 재생할 수 있습니다.
}

void Character::TakeDamage(int damage, Character* attacker) {
    // 피해 처리 로직을 StatsComponent에 위임합니다.
    if (GetStatsComponent() != nullptr) {
        GetStatsComponent()->TakeDamage(damage);
    }

    // TODO: 피격 애니메이션 재생, 사망 처리 등의 로직을 여기에 추가합니다.
    if (IsAlive() == false) {
        // Die();
    }
}

void Character::ReceiveHeal(int amount) {
    // 치유 처리 로직을 StatsComponent에 위임합니다.
    if (GetStatsComponent() != nullptr) {
        GetStatsComponent()->Heal(amount);
    }
}


// --- 이동 관련 함수 구현 ---

void Character::SetPathTo(Math::ivec2 destination) {
    // TODO: A* 같은 경로 탐색 알고리즘을 호출해야 합니다.
    // 1. Grid, 현재 위치(GetGridPosition), 목적지(destination)를 경로 탐색 함수에 넘겨줍니다.
    // 2. 반환된 경로(타일 목록)를 m_current_path에 저장합니다.
    // 3. m_is_moving = true; 로 상태를 변경합니다.
}

void Character::UpdateMovement(double dt) {
    // TODO: 부드러운 이동 애니메이션 로직 구현
    // 1. m_current_path가 비어있으면 m_is_moving = false; 하고 종료합니다.
    // 2. 다음 타일 목적지를 향해 현재 GameObject의 월드 좌표(position)를 dt만큼 이동시킵니다.
    // 3. 목적지 타일에 거의 도착했다면, 논리적 위치(GridPosition)를 업데이트하고 m_current_path에서 해당 타일을 제거합니다.
}



// --- 상태 및 자원 조회 함수 구현 ---
// 모든 조회 함수들은 이제 StatsComponent를 통해 정보를 가져옵니다.

bool Character::IsAlive() const {
    const StatsComponent* stats = GetStatsComponent();
    return (stats != nullptr) ? stats->IsAlive() : false;
}

const CharacterStats& Character::GetStats() const {
    return GetStatsComponent()->GetAllStats();
}

int Character::GetMovementRange() const {
    const StatsComponent* stats = GetStatsComponent();
    return (stats != nullptr) ? stats->GetSpeed() : 0;
}

int Character::GetActionPoints() const {
    const ActionPoints* ap = GetActionPointsComponent();
    // return (ap != nullptr) ? ap->GetCurrentPoints() : 0; // ActionPoints 컴포넌트 구현 후
    return 0; // 임시
}

bool Character::HasSpellSlot(int level) const {
    const SpellSlots* ss = GetSpellSlots();
    // return (ss != nullptr) ? ss->HasSlot(level) : false; // SpellSlots 컴포넌트 구현 후
    return false; // 임시
}


// --- 컴포넌트 편의 함수들 구현 ---

StatsComponent* Character::GetStatsComponent() const {
    // 엔진의 GetComponent 기능을 사용하여 StatsComponent 포인터를 찾아 반환합니다.
    return GetGOComponent<StatsComponent>();
}

GridPosition* Character::GetGridPosition() const {
    return GetGOComponent<GridPosition>();
}

ActionPoints* Character::GetActionPointsComponent() const {
    return GetGOComponent<ActionPoints>();
}

SpellSlots* Character::GetSpellSlots() const {
    return GetGOComponent<SpellSlots>();
}


// 다른 컴포넌트 Getter들도 비슷한 방식으로 구현합니다.
// GridPosition* Character::GetGridPosition() const {
//     return GetGOComponent<GridPosition>();
// }