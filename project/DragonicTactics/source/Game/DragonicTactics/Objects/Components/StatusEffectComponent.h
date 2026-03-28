/**
 * @file StatusEffectComponent.h
 * @brief 현재 활성화된 버프/디버프 상태를 추적하는 단순 저장소
 *        효과 적용 로직 없음 — 상태 기록/쿼리/만료만 담당
 */
#pragma once
#include "./Engine/Component.h"
#include <string>
#include <vector>

class EventBus;
class Character;

struct ActiveEffect
{
    std::string name;      // status_effect.csv의 NAME — "Blessing", "Fear", "Stealth" 등
    int         duration;  // 남은 턴 수
    int         magnitude; // 수치 강도 (없으면 0)
    bool        is_buff;   // true = 버프, false = 디버프
};

class StatusEffectComponent : public CS230::Component
{
public:
    void Update(double dt) override { (void)dt; }

    // --- 상태 추가 / 제거 ---
    void AddEffect(const std::string& name, int duration, int magnitude, bool is_buff);
    void RemoveEffect(const std::string& name);
    void RemoveAllDebuffs();

    // --- 상태 쿼리 ---
    bool HasBuff(const std::string& name) const;
    bool HasDebuff(const std::string& name) const;
    int  GetMagnitude(const std::string& name) const;

    // --- 턴 갱신 (TurnManager::StartNextTurn에서 호출) ---
    void TickDown(Character* owner, EventBus* bus);

private:
    std::vector<ActiveEffect> effects_;
};