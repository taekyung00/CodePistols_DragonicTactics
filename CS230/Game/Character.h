/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Character.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 08, 2025
Updated:    Oct 09, 2025
*/

#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/GameObjectManager.h"
#include "../Engine/Matrix.h"
#include "../Engine/Input.h"
#include "CharacterTypes.h"
#include "GameTypes.h"  


// --- 컴포넌트 전방 선언 (Forward Declarations) ---
class GridPosition;
class ActionPoints;
class SpellSlots;//개발완료
class StatusEffects;
class DamageCalculator;
class TargetingSystem;
class AIMemory;
class GridFootprint;
class Action;
class StatsComponent;

class Character : public CS230::GameObject {
public:
    virtual ~Character() = default;

    // --- 엔진 연동 필수 함수 ---
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
    GameObjectTypes Type() override { return GameObjectTypes::Character; }
    std::string TypeName() override = 0;

    // --- 턴제 로직 인터페이스 ---
    // TurnManager가 턴의 시작과 끝을 알려주기 위해 호출합니다.
    virtual void OnTurnStart() = 0;
    virtual void OnTurnEnd() = 0;

    // --- 핵심 행동 함수 ---
    // 캐릭터가 수행할 수 있는 주요 행동들입니다.
    virtual void PerformAttack(Character* target);
    virtual void PerformAction(Action* action, Character* target, Math::ivec2 tile_position);
    virtual void TakeDamage(int damage, Character* attacker);
    virtual void ReceiveHeal(int amount);

    // --- 이동 관련 함수 ---
    // 목표 지점까지의 경로를 계산하고 이동을 준비합니다.
    void SetPathTo(Math::ivec2 destination);

    // --- 상태 및 자원 조회 함수 (Getters) ---
    // AI나 UI가 캐릭터의 상태를 확인하여 의사결정을 내릴 때 사용합니다.
    CharacterTypes GetCharacterType() const { return m_character_type; }
    bool IsAlive() const;
    const CharacterStats& GetStats() const;
    int GetMovementRange() const; // 속력(speed)을 기반으로 현재 이동 가능한 거리를 반환합니다.
    int GetActionPoints() const; // 현재 남은 행동 횟수를 반환합니다.
    bool HasSpellSlot(int level) const; // 해당 레벨의 주문 슬롯이 남아있는지 확인합니다.

protected:
    Character(CharacterTypes charType, Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);

    // --- 컴포넌트 편의 함수들 (Getters) ---
    // 파생 클래스에서 자신의 컴포넌트에 쉽게 접근하기 위해 사용합니다.
    GridPosition* GetGridPosition() const;
    StatsComponent* GetStatsComponent() const;
    ActionPoints* GetActionPointsComponent() const;
    SpellSlots* GetSpellSlots() const;
    //StatusEffects* GetStatusEffects() const;
    //GridFootprint* GetGridFootprint() const;

    // --- AI 및 이동 관련 함수 (Protected) ---
    void InitializeComponents(Math::ivec2 start_coordinates, int max_action_points, const std::map<int, int>& max_slots_per_level);
    virtual void DecideAction() = 0; // AI의 두뇌. 이번 턴에 무엇을 할지 결정합니다.
    void UpdateMovement(double dt); // 경로를 따라 한 칸씩 부드럽게 이동하는 과정을 업데이트합니다.

    // --- 멤버 변수 ---

    CharacterTypes m_character_type;

    // AI 관련 변수
    GameObject* m_turn_target = nullptr;
    TurnGoal m_turn_goal = TurnGoal::None;

    // 이동 관련 변수
    std::vector<Math::ivec2> m_current_path;
    bool m_is_moving = false;

    // 액션(스킬) 목록
    std::vector<Action*> m_action_list;

private:
    // private 멤버가 필요할 경우 여기에 추가
};