#include "pch.h"

#include "./Engine/Engine.h"
#include "./Engine/Logger.h"
#include "EventBus.h"

void EventBus::Clear()
{
  subscribers.clear();
  Engine::GetLogger().LogEvent("EventBus: All subscriptions cleared");
}

void EventBus::LogEvent(const std::string& eventType, [[maybe_unused]] const void* eventData)
{
  Engine::GetLogger().LogDebug("EventBus: Publishing " + eventType);
}