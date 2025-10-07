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
#include "../Engine/Timer.h"
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

    // GameObject로부터 상속받은 순수 가상 함수들은 그대로 유지
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
    GameObjectTypes Type() override { return GameObjectTypes::Character; }
    std::string TypeName() override = 0;

    // 캐릭터 인터페이스
    CharacterTypes GetCharacterType() const { return m_character_type; }
    virtual void OnTurnStart() = 0;
    virtual void OnTurnEnd() = 0;
    virtual void TakeDamage(int damage, Character* attacker) = 0;
    bool IsAlive() const { return m_stats.current_hp > 0; }
    const CharacterStats& GetStats() const { return m_stats; }

protected:
    Character(CharacterTypes charType, Math::ivec2 start_coordinates);

    // 컴포넌트 편의 함수들
    GridPosition* GetGridPosition() const;
    ActionPoints* GetActionPoints() const;
    StatusEffects* GetStatusEffects() const;

    void InitializeComponents();
    virtual void DecideAction() = 0; // [제안] AI의 두뇌 역할을 자식 클래스가 구현하도록 순수 가상 함수로 만듭니다.

    // --- 멤버 변수 ---
    CharacterTypes m_character_type;
    CharacterStats m_stats;

    // AI 관련 변수 (네이밍 컨벤션 통일)
    GameObject* m_turn_target = nullptr;
    TurnGoal m_turn_goal = TurnGoal::None;

private:
    // private 멤버가 필요할 경우 여기에 추가
};