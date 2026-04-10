#pragma once

namespace DrawDepth {
    // 값이 작을수록 앞에 렌더링 (depth test 기준)
    constexpr float TILE      = 0.9f;  // 그리드 배경 타일  
    constexpr float CHARACTER = 0.5f;  // 캐릭터·기본 오브젝트 
    constexpr float PARTICLE  = 0.3f;  // 파티클 효과      
    constexpr float OVERLAY   = 0.8f;  // 이동/스펠 범위 오버레이 
    constexpr float PATH      = 0.7f;  // 경로 하이라이트  
    constexpr float UI        = 0.01f;  // UI 요소 (텍스트, 아이콘 등)
}