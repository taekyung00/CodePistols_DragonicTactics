# 엔진-게임 분리 계획 (Engine-Game Separation Plan)

**작성일**: 2025-12-05
**목표**: DragonicTactics 게임 코드를 제거하고 다른 게임을 즉시 개발할 수 있도록 엔진과 게임의 완전한 분리

---

## 📋 목차

1. [현재 문제점 분석](#1-현재-문제점-분석)
2. [분리 전략 개요](#2-분리-전략-개요)
3. [Phase 1: GameObjectTypes 추상화](#phase-1-gameobjecttypes-추상화)
4. [Phase 2: PCH 분리](#phase-2-pch-분리)
5. [Phase 3: CMake 타겟 분리](#phase-3-cmake-타겟-분리)
6. [Phase 4: 디렉토리 구조 재편](#phase-4-디렉토리-구조-재편)
7. [검증 체크리스트](#검증-체크리스트)
8. [마이그레이션 가이드](#마이그레이션-가이드)

---

## 1. 현재 문제점 분석

### 🔴 Critical Issues (즉시 수정 필요)

#### 1.1 pch.h의 게임 의존성

**위치**: `source/pch.h:35-38`

```cpp
// ❌ 문제: 모든 엔진 코드가 게임 타입에 의존
#include "Game/DragonicTactics/Types/GameTypes.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"
#include "Game/DragonicTactics/Types/GameObjectTypes.h"
#include "Game/DragonicTactics/Types/Events.h"
```

**영향**:

- 엔진 코드가 컴파일되려면 DragonicTactics 게임 코드가 필수
- 다른 게임 개발 시 기존 게임 타입들을 모두 남겨둬야 함
- 빌드 시간 증가 (불필요한 헤더 포함)

#### 1.2 GameObject.h의 게임 타입 의존성

**위치**: `source/Engine/GameObject.h:12`

```cpp
// ❌ 문제: 엔진 코어가 게임 전용 타입을 직접 참조
#include "Game/DragonicTactics/Types/GameObjectTypes.h"

// 사용처:
virtual GameObjectTypes Type() = 0;
virtual bool CanCollideWith(GameObjectTypes other_object_type);
```

**영향**:

- GameObject는 엔진의 핵심 클래스인데 게임에 종속적
- GameObjectTypes가 DragonicTactics 전용으로 설계됨
- 다른 게임은 이 enum을 재사용하거나 수정해야 함

### 🟡 Medium Issues (점진적 개선 필요)

#### 1.3 엔진-게임 코드 혼재

**현재 구조**:

```
source/
├── Engine/          # 엔진 코드
├── CS200/           # 렌더링 시스템
├── OpenGL/          # OpenGL 래퍼
└── Game/            # 게임 코드
    ├── DragonicTactics/  # 특정 게임
    ├── Background.cpp    # 공통 게임 코드?
    ├── MainMenu.cpp      # 공통?
    └── Splash.cpp        # 공통?
```

**문제**:

- 어떤 파일이 엔진이고 어떤 파일이 게임인지 불명확
- `Game/` 최상위의 파일들이 공통인지 DragonicTactics 전용인지 모호

---

## 2. 분리 전략 개요

### 목표 아키텍처

```
DragonicTactics/
├── Engine/                    # ✅ 엔진 라이브러리 (재사용 가능)
│   ├── include/               # 공개 API
│   │   └── engine_pch.h
│   ├── src/                   # 엔진 구현
│   └── CMakeLists.txt
│
├── Game/                      # ✅ 게임 프로젝트 (교체 가능)
│   ├── DragonicTactics/       # 현재 게임
│   │   ├── include/
│   │   │   └── game_pch.h
│   │   ├── src/
│   │   └── CMakeLists.txt
│   │
│   └── MyNextGame/            # 🎯 미래 게임 (쉽게 추가)
│       └── ...
│
└── CMakeLists.txt             # 루트 빌드 설정
```

### 핵심 원칙

1. **엔진은 게임을 몰라야 함** (Engine doesn't know Game)
2. **게임은 엔진 API만 사용** (Game uses Engine API only)
3. **PCH는 계층적** (engine_pch ← game_pch)
4. **타입 시스템은 확장 가능** (Type system is extensible)

---

## Phase 1: GameObjectTypes 추상화

### 목표

GameObject의 타입 시스템을 게임 독립적으로 만들기

### 1.1 옵션 A: 문자열 기반 타입 시스템 (권장)

**장점**: 완전히 확장 가능, 컴파일 타임 의존성 없음
**단점**: 런타임 오버헤드 약간 증가

**구현**:

```cpp
// Engine/GameObject.h
class GameObject {
public:
    // ✅ 문자열 기반 타입
    virtual std::string Type() const = 0;
    virtual std::string TypeName() const = 0;

    // ✅ 타입 비교 헬퍼
    bool IsType(const std::string& type) const {
        return Type() == type;
    }

    // ✅ 충돌 검사 (문자열 기반)
    virtual bool CanCollideWith(const std::string& other_type) const {
        return false; // 기본적으로 충돌 안함
    }
};
```

**게임 코드**:

```cpp
// Game/DragonicTactics/Objects/Dragon.h
class Dragon : public CS230::GameObject {
public:
    std::string Type() const override { return "Character"; }
    std::string TypeName() const override { return "Dragon"; }

    bool CanCollideWith(const std::string& other_type) const override {
        return other_type == "Character" || other_type == "Obstacle";
    }
};
```

### 1.2 옵션 B: 타입 ID + 등록 시스템

**장점**: 성능 최적화, 타입 안전성
**단점**: 구현 복잡도 증가

**구현**:

```cpp
// Engine/TypeRegistry.h
class TypeRegistry {
public:
    using TypeID = uint32_t;

    static TypeID RegisterType(const std::string& name) {
        static TypeID next_id = 0;
        type_names[next_id] = name;
        return next_id++;
    }

    static const std::string& GetTypeName(TypeID id);

private:
    static std::unordered_map<TypeID, std::string> type_names;
};

// Game/DragonicTactics/Types/GameObjectTypes.h
namespace DragonicTypes {
    inline const auto Character = TypeRegistry::RegisterType("Character");
    inline const auto Dragon = TypeRegistry::RegisterType("Dragon");
    inline const auto Fighter = TypeRegistry::RegisterType("Fighter");
}
```

### 1.3 마이그레이션 단계

1. **새 타입 시스템 구현** (옵션 A 또는 B)
2. **GameObject.h에서 GameObjectTypes.h 제거**
3. **모든 게임 오브젝트 업데이트**
4. **충돌 검사 로직 수정**
5. **테스트 및 검증**

---

## Phase 2: PCH 분리

### 목표

엔진과 게임의 프리컴파일 헤더를 완전히 분리

### 2.1 엔진 PCH (engine_pch.h)

**위치**: `source/Engine/engine_pch.h` (새 파일)

```cpp
#pragma once

// ========================================
// 표준 라이브러리 (STL)
// ========================================
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <array>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

// ========================================
// 외부 라이브러리
// ========================================
#include <SDL.h>
#include <imgui.h>
#include <gsl/gsl>

// ========================================
// 엔진 코어 (자주 사용되는 헤더만)
// ========================================
#include "Engine/Vec2.h"
#include "Engine/Matrix.h"
#include "Engine/Rect.h"
#include "Engine/Logger.h"

// ❌ 게임 관련 헤더는 절대 포함하지 않음!
// ❌ Engine/GameObject.h 같은 큰 헤더도 제외 (필요시 개별 포함)
```

### 2.2 게임 PCH (game_pch.h)

**위치**: `source/Game/DragonicTactics/game_pch.h` (새 파일)

```cpp
#pragma once

// ========================================
// 엔진 PCH 상속
// ========================================
#include "Engine/engine_pch.h"

// ========================================
// 엔진 헤더 (게임에서 자주 사용)
// ========================================
#include "Engine/Engine.h"
#include "Engine/GameObject.h"
#include "Engine/GameState.h"
#include "Engine/Component.h"
#include "Engine/ComponentManager.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.h"
#include "Engine/Input.h"
#include "Engine/Window.h"
#include "Engine/TextureManager.h"
#include "Engine/Sprite.h"
#include "Engine/Animation.h"

// ========================================
// 게임 타입 정의 (DragonicTactics 전용)
// ========================================
#include "Game/DragonicTactics/Types/GameTypes.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"
#include "Game/DragonicTactics/Types/GameObjectTypes.h"
#include "Game/DragonicTactics/Types/Events.h"
```

### 2.3 기존 pch.h 처리

**옵션 1**: 삭제 후 마이그레이션

```bash
# 1. pch.h 삭제
rm source/pch.h

# 2. 모든 엔진 .cpp 파일 수정
find source/Engine -name "*.cpp" -exec sed -i '1s/pch.h/Engine\/engine_pch.h/' {} \;
find source/CS200 -name "*.cpp" -exec sed -i '1s/pch.h/Engine\/engine_pch.h/' {} \;
find source/OpenGL -name "*.cpp" -exec sed -i '1s/pch.h/Engine\/engine_pch.h/' {} \;

# 3. 모든 게임 .cpp 파일 수정
find source/Game/DragonicTactics -name "*.cpp" -exec sed -i '1s/pch.h/Game\/DragonicTactics\/game_pch.h/' {} \;
```

**옵션 2**: 점진적 마이그레이션 (안전)

```cpp
// source/pch.h (임시 - 경고 포함)
#pragma once

#warning "pch.h is deprecated! Use engine_pch.h or game_pch.h"

// 엔진 코드용 (조건부 포함)
#if defined(ENGINE_CODE)
    #include "Engine/engine_pch.h"
#else
    #include "Game/DragonicTactics/game_pch.h"
#endif
```

---

## Phase 3: CMake 타겟 분리

### 목표

엔진을 별도 라이브러리로, 게임을 실행 파일로 분리

### 3.1 루트 CMakeLists.txt

**위치**: `DragonicTactics/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.21)
project(DragonicTactics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Dependencies.cmake)

# ========================================
# 엔진 라이브러리
# ========================================
add_subdirectory(Engine)

# ========================================
# 게임 프로젝트
# ========================================
add_subdirectory(Game)
```

### 3.2 엔진 CMakeLists.txt

**위치**: `DragonicTactics/Engine/CMakeLists.txt` (새 파일)

```cmake
# ========================================
# CS230 Engine Library
# ========================================
project(CS230Engine)

# 엔진 소스 파일 수집
file(GLOB_RECURSE ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/Engine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/CS200/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/OpenGL/*.cpp"
)

file(GLOB_RECURSE ENGINE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/Engine/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/CS200/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/OpenGL/*.h"
)

# 정적 라이브러리 생성
add_library(CS230Engine STATIC
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

# 프리컴파일 헤더 설정
target_precompile_headers(CS230Engine
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/source/Engine/engine_pch.h"
)

# 인클루드 디렉토리
target_include_directories(CS230Engine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/source/Engine"
)

# 외부 라이브러리 링크
target_link_libraries(CS230Engine
    PUBLIC
        SDL2::SDL2
        GLEW::GLEW
        imgui
        Microsoft.GSL::GSL
)

# 컴파일 옵션
target_compile_definitions(CS230Engine
    PRIVATE
        ENGINE_CODE  # 엔진 코드임을 표시
)
```

### 3.3 게임 CMakeLists.txt

**위치**: `DragonicTactics/Game/CMakeLists.txt` (새 파일)

```cmake
# ========================================
# DragonicTactics Game
# ========================================
project(DragonicTactics_Game)

# 게임 소스 파일 수집
file(GLOB_RECURSE GAME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/DragonicTactics/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"  # MainMenu, Splash 등
)

file(GLOB_RECURSE GAME_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/DragonicTactics/**/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# main.cpp 추가
list(APPEND GAME_SOURCES "${CMAKE_SOURCE_DIR}/source/main.cpp")

# 실행 파일 생성
add_executable(dragonic_tactics
    ${GAME_SOURCES}
    ${GAME_HEADERS}
)

# 프리컴파일 헤더 설정
target_precompile_headers(dragonic_tactics
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/DragonicTactics/game_pch.h"
)

# 엔진 라이브러리 링크
target_link_libraries(dragonic_tactics
    PRIVATE
        CS230Engine
)

# 인클루드 디렉토리
target_include_directories(dragonic_tactics
    PRIVATE
        "${CMAKE_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Assets 복사
add_custom_command(TARGET dragonic_tactics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:dragonic_tactics>/Assets"
)
```

---

## Phase 4: 디렉토리 구조 재편

### 4.1 최종 디렉토리 구조

```
CodePistols_DragonicTactics/
├── CLAUDE.md
├── architecture/
│   ├── engine-game-separation-plan.md  # 이 문서
│   └── ...
│
└── DragonicTactics/
    ├── CMakeLists.txt                  # 루트 빌드 설정
    ├── CMakePresets.json
    ├── cmake/
    │
    ├── Engine/                          # ✅ 엔진 모듈
    │   ├── CMakeLists.txt
    │   └── source/
    │       ├── Engine/
    │       │   ├── engine_pch.h         # 엔진 PCH
    │       │   ├── Engine.h
    │       │   ├── GameObject.h         # ❌ GameObjectTypes 의존성 제거됨
    │       │   └── ...
    │       ├── CS200/
    │       └── OpenGL/
    │
    ├── Game/                            # ✅ 게임 모듈
    │   ├── CMakeLists.txt
    │   └── DragonicTactics/
    │       ├── game_pch.h               # 게임 PCH
    │       ├── Abilities/
    │       ├── Objects/
    │       ├── StateComponents/
    │       ├── States/
    │       ├── Types/
    │       │   ├── GameTypes.h
    │       │   ├── CharacterTypes.h
    │       │   ├── GameObjectTypes.h   # 게임 전용
    │       │   └── Events.h
    │       └── ...
    │
    ├── Assets/                          # 게임 에셋
    └── build/                           # 빌드 출력
```

### 4.2 마이그레이션 스크립트

```bash
#!/bin/bash
# migrate-structure.sh

echo "Creating new directory structure..."

# 1. 엔진 디렉토리 생성
mkdir -p Engine/source
mv source/Engine Engine/source/
mv source/CS200 Engine/source/
mv source/OpenGL Engine/source/

# 2. 게임 디렉토리 정리
mkdir -p Game
mv source/Game/DragonicTactics Game/
mv source/Game/*.cpp Game/  # MainMenu, Splash 등

# 3. main.cpp 위치 조정 (Game으로)
mv source/main.cpp Game/

# 4. 정리
rm -rf source/

echo "Migration complete!"
```

---

## 검증 체크리스트

### Phase 1 완료 기준

- [ ] GameObject.h에서 GameObjectTypes.h include 제거
- [ ] GameObject::Type()이 문자열 또는 TypeID 반환
- [ ] 모든 게임 오브젝트가 새 타입 시스템 사용
- [ ] 충돌 검사 로직이 새 타입으로 작동
- [ ] 기존 테스트가 모두 통과

### Phase 2 완료 기준

- [ ] engine_pch.h 생성 및 엔진 코드 적용
- [ ] game_pch.h 생성 및 게임 코드 적용
- [ ] 기존 pch.h 삭제 또는 deprecated 처리
- [ ] 모든 .cpp 파일이 올바른 PCH 사용
- [ ] 빌드 성공 (엔진 + 게임)
- [ ] 빌드 시간 측정 (35% 개선 유지 확인)

### Phase 3 완료 기준

- [ ] Engine/CMakeLists.txt 생성
- [ ] Game/CMakeLists.txt 생성
- [ ] 루트 CMakeLists.txt 업데이트
- [ ] CS230Engine.lib 생성 확인
- [ ] dragonic_tactics.exe 빌드 성공
- [ ] 게임 실행 및 기능 확인

### Phase 4 완료 기준

- [ ] 디렉토리 구조 재편 완료
- [ ] 모든 include 경로 수정
- [ ] Assets 경로 수정
- [ ] 빌드 및 실행 성공
- [ ] 기존 기능 모두 작동

### 최종 검증

- [ ] `Game/DragonicTactics` 디렉토리 삭제 가능
- [ ] `Game/MyNewGame` 생성하여 새 게임 시작 가능
- [ ] 엔진 코드가 게임 헤더 포함 없이 컴파일
- [ ] 문서 업데이트 (CLAUDE.md)

---

## 마이그레이션 가이드

### 새 게임 프로젝트 시작하기

**1단계: 게임 디렉토리 생성**

```bash
cd DragonicTactics/Game
cp -r DragonicTactics MyNewGame  # 템플릿으로 복사
cd MyNewGame
```

**2단계: 게임 고유 파일 정리**

```bash
# DragonicTactics 전용 파일 삭제
rm -rf Objects/Dragon.* Objects/Fighter.*
rm -rf StateComponents/EventBus.*
rm -rf States/GamePlay.*

# 필요한 것만 남기기
# - Types/ (타입 정의)
# - Factories/ (팩토리 패턴)
# - States/ (기본 상태들)
```

**3단계: game_pch.h 수정**

```cpp
// Game/MyNewGame/game_pch.h
#pragma once

#include "Engine/engine_pch.h"
#include "Engine/Engine.h"
// ... 엔진 헤더들 ...

// 🎯 MyNewGame 전용 타입
#include "Game/MyNewGame/Types/GameTypes.h"
#include "Game/MyNewGame/Types/MyObjectTypes.h"
```

**4단계: CMakeLists.txt 업데이트**

```cmake
# Game/CMakeLists.txt에 추가
add_subdirectory(MyNewGame)
```

**5단계: 빌드 및 실행**

```bash
cd ../..  # DragonicTactics/ 루트로
cmake --preset windows-debug
cmake --build --preset windows-debug
build/windows-debug/my_new_game.exe
```

### 기존 코드 이식하기

**GameObject 타입 시스템 변경**:

```cpp
// ❌ 이전 (enum 기반)
GameObjectTypes Type() override { return GameObjectTypes::Character; }
bool CanCollideWith(GameObjectTypes type) override {
    return type == GameObjectTypes::Enemy;
}

// ✅ 이후 (문자열 기반)
std::string Type() const override { return "Character"; }
bool CanCollideWith(const std::string& type) const override {
    return type == "Enemy" || type == "Obstacle";
}
```

**PCH include 경로 변경**:

```cpp
// ❌ 이전
#include "pch.h"

// ✅ 엔진 코드
#include "Engine/engine_pch.h"

// ✅ 게임 코드
#include "Game/DragonicTactics/game_pch.h"
```

---

## 예상 문제 및 해결책

### 문제 1: 순환 의존성

**증상**: `Engine/GameObject.h` ↔ `Game/Types/GameObjectTypes.h`

**해결**: Forward declaration + pimpl 패턴

```cpp
// Engine/GameObject.h
class GameObject {
    virtual std::string Type() const = 0;  // 구현은 게임에서
};
```

### 문제 2: 빌드 시간 증가

**원인**: PCH 분리로 인한 중복 컴파일

**해결**: Unity build 사용

```cmake
# Engine/CMakeLists.txt
set_target_properties(CS230Engine PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 16
)
```

### 문제 3: 링커 오류

**증상**: `undefined reference to Engine::GetLogger()`

**해결**: 올바른 링크 순서 확인

```cmake
target_link_libraries(dragonic_tactics
    PRIVATE
        CS230Engine  # 엔진이 먼저!
        SDL2::SDL2
)
```

---

## 다음 단계

1. **Phase 1 구현**: GameObjectTypes 추상화 (1-2일)
2. **Phase 2 구현**: PCH 분리 (1일)
3. **Phase 3 구현**: CMake 타겟 분리 (1일)
4. **Phase 4 구현**: 디렉토리 재편 (0.5일)
5. **테스트 및 문서화**: 전체 검증 (1일)

**예상 총 소요 시간**: 4.5 - 5.5일

---

## 참고 자료

- [CMake Modern Practices](https://cliutils.gitlab.io/modern-cmake/)
- [C++ Core Guidelines - Module Design](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-module)
- [Game Engine Architecture - 3rd Edition](https://www.gameenginebook.com/) - Chapter 15: Runtime Engine Architecture
