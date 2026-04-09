#pragma once

namespace DrawDepth {
    // 값이 작을수록 앞에 렌더링 (depth test 기준)
    // DrawPriority와 관계: depth = 1.0f - (priority / 100.0f)
    constexpr float TILE      = 0.6f;  // 그리드 배경 타일  (DrawPriority 40)
    constexpr float CHARACTER = 0.5f;  // 캐릭터·기본 오브젝트 (DrawPriority 50 = DRAWPRIORITY)
    constexpr float PARTICLE  = 0.3f;  // 파티클 효과      (DrawPriority 70)
    constexpr float OVERLAY   = 0.2f;  // 이동/스펠 범위 오버레이 (DrawPriority 80)
    constexpr float PATH      = 0.1f;  // 경로 하이라이트  (DrawPriority 90)
}