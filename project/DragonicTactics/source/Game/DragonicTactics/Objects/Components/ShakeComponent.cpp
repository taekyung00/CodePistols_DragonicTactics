#include "ShakeComponent.h"
#include <cstdlib>

void ShakeComponent::Update(double deltaTime) {
    if (timer > 0.0f) {
        // 핵심 수정: double인 deltaTime을 float으로 명확하게 변환한다고 컴파일러에 알림
        timer -= static_cast<float>(deltaTime);
        
        if (timer <= 0.0f) {
            timer = 0.0f;
            offset = Math::vec2(0.0f, 0.0f);
        } else {
            float damping = timer / duration; 
            float currentIntensity = initialIntensity * damping;

            // C++ 스타일의 안전한 형변환 사용
            float randX = ((std::rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f) * currentIntensity;
            float randY = ((std::rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f) * currentIntensity;
            
            offset = Math::vec2(randX, randY);
        }
    }
}

