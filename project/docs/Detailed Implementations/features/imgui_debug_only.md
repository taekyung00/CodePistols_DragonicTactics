# ImGui 디버그 전용 설정 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: ImGui 창을 DEVELOPER_VERSION 빌드에서만 활성화
**작성일**: 2026-03-08

**관련 파일**:
- [GamePlay.cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp)
- [DebugManager.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/Debugger/DebugManager.h)
- [source/CMakeLists.txt](../../../DragonicTactics/source/CMakeLists.txt)

---

## 개요

현재 ImGui 창(Map Selection, Player Actions, Combat Status 등)이 **모든 빌드**에서 표시됩니다.
릴리즈 빌드에서는 ImGui 창이 보이지 않아야 합니다.

### 현재 CMake 매크로 구조

```cmake
# CMakeLists.txt (라인 169-172)
if (IS_DEVELOPER_VERSION)
    target_compile_definitions(dragonic_tactics PRIVATE DEVELOPER_VERSION _DEBUG)
endif()
```

| 빌드 프리셋 | `IS_DEVELOPER_VERSION` | `DEVELOPER_VERSION` 정의 |
|---|---|---|
| `windows-debug` | ON | ✅ 정의됨 |
| `windows-developer-release` | ON | ✅ 정의됨 |
| `windows-release` | OFF | ❌ 미정의 |
| `linux-debug` | ON | ✅ |
| `linux-release` | OFF | ❌ |

→ **`#if defined(DEVELOPER_VERSION)`** 매크로로 ImGui 블록을 감싸면 됩니다.

---

## Implementation Tasks

### Task 1: GamePlay::DrawImGui() 래핑

**파일**: `GamePlay.cpp`

현재:
```cpp
void GamePlay::DrawImGui()
{
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    GetGSComponent<DebugManager>()->DrawImGui(grid_system);

    ImGui::Begin("Map Selection");
    // ...
    ImGui::End();

    ImGui::Begin("Player Actions");
    // ...
    ImGui::End();
}
```

변경 후:
```cpp
void GamePlay::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
    GridSystem* grid_system = GetGSComponent<GridSystem>();
    GetGSComponent<DebugManager>()->DrawImGui(grid_system);

    ImGui::Begin("Map Selection");
    // ... (기존 코드 그대로)
    ImGui::End();

    ImGui::Begin("Player Actions");
    // ... (기존 코드 그대로)
    ImGui::End();

    ImGui::Begin("Combat Status");
    // ... (기존 코드 그대로)
    ImGui::End();

    ImGui::Begin("Action List");
    // ... (기존 코드 그대로)
    ImGui::End();
#endif // DEVELOPER_VERSION
}
```

**주의**: `DrawImGui()` 함수 자체는 남겨두고 내부만 매크로로 감쌉니다.
엔진의 `Engine::Update()`에서 `state_manager.DrawImGui()`를 호출하는 구조는 유지됩니다.

---

### Task 2: DebugManager::DrawImGui() 래핑

**파일**: `DebugManager.cpp`

```cpp
void DebugManager::DrawImGui(GridSystem* grid_system)
{
#if defined(DEVELOPER_VERSION)
    ImGui::Begin("Debug Manager");
    // ... 기존 디버그 UI
    ImGui::End();

    if (grid_system)
    {
        ImGui::Begin("Grid Debug");
        // ... 그리드 디버그 정보
        ImGui::End();
    }
#endif // DEVELOPER_VERSION
}
```

---

### Task 3: 다른 GameState의 DrawImGui()도 동일하게 적용

프로젝트 내 `DrawImGui()`를 구현하는 모든 GameState에 동일하게 적용:

```
검색 대상: DrawImGui() 구현 파일들
```

```bash
# 해당 파일 탐색
grep -r "DrawImGui" source/ --include="*.cpp" -l
```

찾은 파일들 각각:
```cpp
void SomeState::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
    // ... ImGui 코드
#endif
}
```

---

### Task 4: 릴리즈 빌드에서 ImGui 헤더 포함 최소화 (선택 사항)

현재 pch.h에 imgui가 포함되어 있다면, 릴리즈에서도 헤더가 컴파일됩니다.
이는 기능 자체에는 문제없으나, 빌드 시간 최적화를 원한다면:

**파일**: `pch.h` 확인 및 조건부 포함:

```cpp
// pch.h에서
#if defined(DEVELOPER_VERSION)
    #include <imgui.h>
    #include <imgui_internal.h>
#endif
```

**주의**: 이 변경은 `DrawImGui()` 함수 내 `ImGui::` 코드가 이미 `#if DEVELOPER_VERSION`으로
감싸진 후에만 적용하세요. 그렇지 않으면 릴리즈 빌드에서 컴파일 오류가 발생합니다.

---

## Implementation Examples

### 올바른 패턴

```cpp
// ✅ 올바름: 함수 시그니처 유지, 내부만 조건부
void GamePlay::DrawImGui()
{
#if defined(DEVELOPER_VERSION)
    ImGui::Begin("Debug Info");
    ImGui::Text("Turn: %d", turn_number);
    ImGui::End();
#endif
}
```

### 잘못된 패턴

```cpp
// ❌ 잘못됨: 함수 전체를 매크로로 감싸면 가상 함수 재정의가 사라짐
#if defined(DEVELOPER_VERSION)
void GamePlay::DrawImGui() { ... }
#endif
// → GameState::DrawImGui()가 호출될 때 아무것도 없어 문제 발생 가능
```

### 빌드별 동작 확인

```bash
# 디버그 빌드 (ImGui 표시됨)
cmake --preset windows-debug
cmake --build --preset windows-debug
build/windows-debug/dragonic_tactics.exe  # ImGui 창 표시

# 릴리즈 빌드 (ImGui 숨김)
cmake --preset windows-release
cmake --build --preset windows-release
build/windows-release/dragonic_tactics.exe  # ImGui 창 없음
```

---

## Rigorous Testing

### 테스트 1: Developer 빌드에서 ImGui 표시 확인
```
1. cmake --preset windows-debug
2. cmake --build --preset windows-debug
3. 실행 → Map Selection, Player Actions, Combat Status 창이 표시되어야 함
4. ✅ 표시되면 통과
```

### 테스트 2: Release 빌드에서 ImGui 미표시 확인
```
1. cmake --preset windows-release
2. cmake --build --preset windows-release
3. 실행 → 어떤 ImGui 창도 표시되지 않아야 함
4. ✅ 창 없으면 통과
```

### 테스트 3: 컴파일 오류 없음 확인
```bash
# 두 프리셋 모두 오류 없이 빌드되어야 함
cmake --build --preset windows-debug          # ✅
cmake --build --preset windows-release        # ✅
cmake --build --preset windows-developer-release  # ✅ (ImGui 표시됨)
```

### 테스트 4: 게임 기능 정상 동작 (릴리즈)
릴리즈 빌드에서 ImGui 없이도:
- 플레이어 이동 (ButtonManager 기반)
- 공격 액션
- AI 자동 턴
- 전투 종료 판정
이 모두 정상 동작해야 합니다.
ButtonManager 구현 완료 후 릴리즈 빌드 테스트 권장.

---

## 참고: DEVELOPER_VERSION 프리셋별 설정

```json
// CMakePresets.json에서
{
    "name": "windows-debug",
    "cacheVariables": {
        "IS_DEVELOPER_VERSION": true   // DEVELOPER_VERSION 정의됨
    }
},
{
    "name": "windows-release",
    "cacheVariables": {
        "IS_DEVELOPER_VERSION": false  // DEVELOPER_VERSION 미정의
    }
}
```
