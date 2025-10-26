#include "EventBus.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"



void EventBus::Clear() {
    subscribers.clear();
    Engine::GetLogger().LogEvent("EventBus: All subscriptions cleared");
}

void EventBus::LogEvent(const std::string& eventType, [[maybe_unused]]const void* eventData) {
    Engine::GetLogger().LogDebug("EventBus: Publishing " + eventType);
}