#include "EventBus.h"
#include "../../Engine/Engine.h"

EventBus& EventBus::Instance() {
    static EventBus instance;
    return instance;
}

void EventBus::Clear() {
    subscribers.clear();
    Engine::GetLogger().LogEvent("EventBus: All subscriptions cleared");
}

void EventBus::LogEvent(const std::string& eventType, [[maybe_unused]]const void* eventData) {
    Engine::GetLogger().LogDebug("EventBus: Publishing " + eventType);
}