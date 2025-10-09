/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Character.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 08, 2025
*/

#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/GameObjectManager.h"
#include "../Engine/Matrix.h"
#include "../Engine/Input.h"
#include "CharacterTypes.h"
#include "GameTypes.h"  

class GridPosition;
class ActionPoints;
class SpellSlots;
class StatusEffects;
class DamageCalculator;
class TargetingSystem;
class AIMemory;
class GridFootprint;
class Ability; 

struct CharacterStats {
    int max_hp = 10;
    int current_hp = 10;
    int attack = 1;
    int defend = 1;
    int speed = 5;
    int attack_range = 1;
};

class Character : public CS230::GameObject {
public:
    virtual ~Character() = default;

    // --- 엔진 연동 필수 함수 ---
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
    GameObjectTypes Type() override { return GameObjectTypes::Character; }
    std::string TypeName() override = 0;

    // --- 턴제 로직 인터페이스 ---
    // TurnManager가 턴의 시작과 끝을 알려주기 위해 호출하는 이벤트 함수들입니다.
    virtual void OnTurnStart() = 0;
    virtual void OnTurnEnd() = 0;

    // --- 핵심 행동 함수 ---
    // 캐릭터가 수행할 수 있는 주요 행동들입니다.
    virtual void PerformAttack(Character* target);
    virtual void UseAbility(Ability* ability, Character* target, Math::ivec2 tile_position); // 타겟이 캐릭터가 아닌 타일일 수도 있음
    virtual void TakeDamage(int damage, Character* attacker) = 0;
    virtual void ReceiveHeal(int amount);

    // --- 상태 및 자원 조회 함수 (Getters) ---
    // AI나 UI가 캐릭터의 상태를 확인하여 의사결정을 내릴 때 사용합니다.
    CharacterTypes GetCharacterType() const { return m_character_type; }
    bool IsAlive() const { return m_stats.current_hp > 0; }
    const CharacterStats& GetStats() const { return m_stats; }
    int GetMovementRange() const; // 속력(speed)을 기반으로 현재 이동 가능한 거리를 반환
    bool HasSpellSlot(int level) const; // 해당 레벨의 주문 슬롯이 남아있는지 확인

protected:
    Character(CharacterTypes charType, Math::ivec2 start_coordinates);

    // --- 컴포넌트 편의 함수들 (Getters) ---
    // 파생 클래스에서 자신의 컴포넌트에 쉽게 접근하기 위해 사용합니다.
    GridPosition* GetGridPosition() const;
    ActionPoints* GetActionPoints() const;
    SpellSlots* GetSpellSlots() const;
    StatusEffects* GetStatusEffects() const;
    GridFootprint* GetGridFootprint() const;

    // --- AI 및 이동 관련 함수 (Protected) ---
    virtual void DecideAction() = 0; // AI의 두뇌. 이번 턴에 무엇을 할지 결정합니다.
    void SetPathTo(Math::ivec2 destination); // 목표 지점까지의 경로를 계산하여 저장합니다.
    void UpdateMovement(double dt); // 경로를 따라 한 칸씩 부드럽게 이동하는 과정을 업데이트합니다.

    void InitializeComponents();

    // --- 멤버 변수 ---
    CharacterTypes m_character_type;
    CharacterStats m_stats;

    // AI 관련 변수
    GameObject* m_turn_target = nullptr;
    TurnGoal m_turn_goal = TurnGoal::None;

    // 이동 관련 변수
    std::vector<Math::ivec2> m_current_path;
    bool m_is_moving = false;

private:
    // private 멤버가 필요할 경우 여기에 추가
};