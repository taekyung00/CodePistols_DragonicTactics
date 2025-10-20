#pragma once
#include <functional>
#include <map>
#include <typeindex>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <vector>
#include <typeindex>
#include <string>

class EventBus {
public:
    EventBus() = default;
    ~EventBus() = default;

    // Subscribe to event type T with callback function
    template<typename T>
    void Subscribe(std::function<void(const T&)> callback) {
        auto typeIndex = std::type_index(typeid(T));
        subscribers[typeIndex].push_back([callback](const void* data) {
            callback(*static_cast<const T*>(data));
            });
    }

    // Publish event of type T
    template<typename T>
    void Publish(const T& event) {
        auto typeIndex = std::type_index(typeid(T));

        // Optional: Log event for debugging
        if (loggingEnabled) {
            LogEvent(typeid(T).name(), &event);
        }

        // Call all subscribers for this event type
        if (subscribers.find(typeIndex) != subscribers.end()) {
            for (auto& callback : subscribers[typeIndex]) {
                callback(&event);
            }
        }
    }

    // Unsubscribe all listeners (used for cleanup)
    void Clear();

    // Enable/disable event logging
    void SetLogging(bool enabled) { loggingEnabled = enabled; }

private:
    
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    // Type-erased callback wrapper
    using CallbackWrapper = std::function<void(const void*)>;

    // Map of event type -> list of callbacks
    std::map<std::type_index, std::vector<CallbackWrapper>> subscribers;

    bool loggingEnabled = false;

    void LogEvent(const std::string& eventType, const void* eventData);
};
