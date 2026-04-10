#pragma once
#include "Engine/Vec2.h"
#include "./Engine/Component.h"

class ShakeComponent : public CS230::Component
{
private:
    float initialIntensity;
    float duration;
    float timer;
    Math::vec2 offset;

public:
    ShakeComponent() : initialIntensity(0.0f), duration(0.0f), timer(0.0f), offset(0.0f, 0.0f) {}

    void StartShake(float intensity, float durationSeconds) {
        initialIntensity = intensity;
        duration = durationSeconds;
        timer = durationSeconds;
    }

    void Update(double deltaTime) override;

    Math::vec2 GetOffset() const { return offset; }
    
    bool IsShaking() const { return timer > 0.0f; }
};
