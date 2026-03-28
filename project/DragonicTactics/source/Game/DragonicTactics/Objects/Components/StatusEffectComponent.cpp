#include "pch.h"
#include "StatusEffectComponent.h"
#include "../../StateComponents/EventBus.h"
#include "../../Types/Events.h"
#include "../../Objects/Character.h"

void StatusEffectComponent::AddEffect(const std::string& name, int duration, int magnitude)
{
    // 이미 있으면 지속시간 갱신
    for (auto& e : effects_)
    {
        if (e.name == name)
        {
            e.duration  = duration;
            e.magnitude = magnitude;
            return;
        }
    }
    effects_.push_back({ name, duration, magnitude });
}

void StatusEffectComponent::RemoveEffect(const std::string& name)
{
    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(),
                       [&name](const ActiveEffect& e) { return e.name == name; }),
        effects_.end());
}

void StatusEffectComponent::RemoveAllEffects()
{
    effects_.clear();
}

bool StatusEffectComponent::Has(const std::string& name) const
{
    for (const auto& e : effects_)
        if (e.name == name) return true;
    return false;
}

int StatusEffectComponent::GetMagnitude(const std::string& name) const
{
    for (const auto& e : effects_)
        if (e.name == name) return e.magnitude;
    return 0;
}

void StatusEffectComponent::TickDown(Character* owner, EventBus* bus)
{
    std::vector<ActiveEffect> expired;

    for (auto& e : effects_)
    {
        if (e.duration > 0 && --e.duration == 0)
            expired.push_back(e);
    }

    for (const auto& e : expired)
    {
        RemoveEffect(e.name);
        if (bus)
            bus->Publish(StatusEffectRemovedEvent{ owner, e.name, "expired" });
    }
}