/**
 * \date 2026 Spring
 * \copyright DigiPen Institute of Technology
 */
#include "ShakeComponent.h"
#include <cstdlib>

void ShakeComponent::Update(double deltaTime) {
    if (timer > 0.0f) {
        // 핵심 수정: double인 deltaTime을 float으로 명확하게 변환한다고 컴파일러에 알림
        timer -= static_cast<float>(deltaTime);
        
        if (timer <= 0.0f) {
            timer = 0.0f;
            offset = Math::vec2(0.0, 0.0);
        } else {
            float damping = timer / duration;
            float currentIntensity = initialIntensity * damping;

            float randX = ((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f) * currentIntensity;
            float randY = ((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f) * currentIntensity;

            offset = Math::vec2(static_cast<double>(randX), static_cast<double>(randY));
        }
    }
}

