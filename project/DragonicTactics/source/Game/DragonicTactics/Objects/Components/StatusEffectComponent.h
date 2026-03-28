/**
 * @file StatusEffectComponent.h
 * @brief 캐릭터의 버프/디버프 상태를 추적하는 GameObject 컴포넌트
 *        TurnManager::StartNextTurn()에서 TickDown() 호출
 */
#pragma once
#include "./Engine/Component.h"
#include <string>
#include <vector>

class EventBus;
class Character;

struct ActiveEffect
{
    std::string name;      // "축복", "공포", "은신", "피의 갈망" 등
    int         duration;  // 남은 턴 수 (-1 = 영구)
    int         magnitude; // 수치 효과 강도 (없으면 0)
    bool        is_buff;   // true = 버프, false = 디버프
};

class StatusEffectComponent : public CS230::Component
{
public:
    void Update(double dt) override { (void)dt; }

    // --- 효과 추가 / 제거 ---
    void AddEffect(const std::string& name, int duration, int magnitude, bool is_buff);
    void RemoveEffect(const std::string& name);
    void RemoveAllDebuffs();  // 최후의 저항(S_ENH_070) 효과
    void RemoveAllBuffs();

    // --- 팩트 쿼리 ---
    bool Has(const std::string& name) const;
    bool HasBuff(const std::string& name) const;
    bool HasDebuff(const std::string& name) const;
    int  GetMagnitude(const std::string& name) const;

    // --- 턴 갱신 ---
    // TurnManager::StartNextTurn()에서 호출 — 지속시간 감소, 만료 효과 제거
    void TickDown(Character* owner, EventBus* bus);

    // --- 전체 조회 (UI/Debug용) ---
    const std::vector<ActiveEffect>& GetAll() const { return effects_; }

private:
    std::vector<ActiveEffect> effects_;
};