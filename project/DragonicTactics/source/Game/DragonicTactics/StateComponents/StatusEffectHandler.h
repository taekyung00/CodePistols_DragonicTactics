#pragma once
#include "./Engine/Component.h"
#include <string>

class Character;

/**
 * @brief 각 status effect의 실행 로직 중앙 집중 관리
 *
 * 타이밍별 훅:
 *   SpellSystem::ApplySpellEffect  → OnApplied()           즉시 실행 효과
 *   CombatSystem::ExecuteAttack    → ModifyDamage*()       피해 보정
 *                                  → OnAfterAttack()       공격 후 효과
 *   TurnManager::StartNextTurn     → OnTurnStart()         턴 시작 효과
 */
class StatusEffectHandler : public CS230::Component
{
public:

    // SpellSystem::ApplySpellEffect에서 AddBuff/AddDebuff 직후 호출
    void OnApplied(Character* target, const std::string& effect_name);

    // StatusEffectComponent::TickDown에서 효과 만료 시, 또는 Purify 적용 전 호출
    // Fear/Haste 적용으로 변경된 base speed를 복원
    void OnRemoved(Character* target, const std::string& effect_name);

    // CombatSystem::ExecuteAttack — 피해 계산 단계에서 호출
    int ModifyDamageDealt(Character* attacker, int base_damage) const;
    int ModifyDamageTaken(Character* defender, int base_damage) const;

    // CombatSystem::ExecuteAttack — ApplyDamage 직후 호출
    void OnAfterAttack(Character* attacker, Character* defender, int damage_dealt);

    // TurnManager::StartNextTurn — RefreshActionPoints 직후 호출
    void OnTurnStart(Character* character);

    // 타겟 가능 여부 체크 — PlayerInputHandler/GridSystem에서 호출
    // Stealth 중인 캐릭터는 타겟 불가
    bool IsTargetable(Character* target) ;

    // 알려진 효과 이름 목록 (status_effect.csv 파싱 대체)
    // 새 효과 추가 시 KNOWN_EFFECTS와 실행 로직을 함께 업데이트
    static bool IsKnownEffect(const std::string& name);

    static constexpr int NUM_EFFECTS = 9;
    static const std::pair<std::string,std::string> KNOWN_EFFECTS[NUM_EFFECTS];
private:
};