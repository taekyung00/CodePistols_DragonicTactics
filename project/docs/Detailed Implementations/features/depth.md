# Plan: Depth 파라미터 중앙화 정리

## Context

`Draw()` 함수 체인에 depth 파라미터가 추가되면서 각 레이어마다 다른 기본값이 분산되어 있다:

- `Texture::Draw(full)` → `0.6f`, `Texture::Draw(sprite sheet)` → `0.0f`
- `IRenderer2D::DrawQuad` → `1.0f`, `IRenderer2D::DrawRectangle` → `0.0f`
- `GridSystem.cpp` → `0.2f`, `0.1f` 매직 넘버
- `GameObject::Draw` / `Sprite::Draw` / `Particle::Draw` → `0.5f`

**두 가지 근본 문제**:

1. 하위 레이어(Texture, IRenderer2D)가 각자 다른 기본값으로 depth를 결정 — 상위에서 값을 전달해도 중간 레이어 기본값에 덮힐 위험
2. `0.2f`, `0.1f` 같은 매직 넘버가 코드에 산재 — 렌더 순서 의도가 불명확

## 수정 방법

### Step 1 — `DrawDepth.h` 신규 생성

**위치**: `DragonicTactics/source/Engine/DrawDepth.h`

```cpp
#pragma once

namespace DrawDepth {
    // 값이 작을수록 앞에 렌더링 (depth test 기준)
    constexpr float TILE      = 0.6f;  // 그리드 배경 타일
    constexpr float CHARACTER = 0.5f;  // 캐릭터·기본 오브젝트 (기본값)
    constexpr float PARTICLE  = 0.3f;  // 파티클 효과
    constexpr float OVERLAY   = 0.2f;  // 이동/스펠 범위 오버레이
    constexpr float PATH      = 0.1f;  // 경로 하이라이트
}
```

### Step 2 — 모든 레이어 기본값을 `DrawDepth::CHARACTER`로 통일

하위 레이어는 depth를 결정하는 주체가 아니다. 모든 레이어의 기본값을 `CHARACTER(0.5f)`로 통일해서, 상위에서 명시적으로 depth를 내려보내지 않아도 일관된 값이 쓰이도록 한다.

**`Engine/Texture.h`** (98, 133번 줄)

```cpp
// 현재
void Draw(..., float depth = 0.6f);   // 전체 그리기
void Draw(..., float depth = 0.f);    // 스프라이트 시트

// 수정
void Draw(..., float depth = DrawDepth::CHARACTER);  // 둘 다
```

**`CS200/IRenderer2D.h`** (35, 39번 줄)

```cpp
// 현재
DrawQuad(..., float depth = 1.f)
DrawRectangle(..., float depth = 0.f)

// 수정
DrawQuad(..., float depth = DrawDepth::CHARACTER)
DrawRectangle(..., float depth = DrawDepth::CHARACTER)
```

**`Engine/Sprite.h`** (38번 줄) — 숫자 → 이름 교체:

```cpp
void Draw(..., float depth = DrawDepth::CHARACTER);
```

**`Engine/GameObject.h`** (56번 줄) — 숫자 → 이름 교체:

```cpp
virtual void Draw(..., float depth = DrawDepth::CHARACTER);
```

**`Engine/Particle.h`** (24번 줄) — 숫자 → 이름 교체, `DrawPriority()` 오버라이드 제거:

```cpp
void Draw(..., float depth = DrawDepth::CHARACTER) override;
// DrawPriority() override 제거
```

### Step 3 — GridSystem.cpp 매직 넘버 교체

`DrawDepth.h` include 후 명시적으로 depth를 전달:

```cpp
// 타일 (현재: Texture 기본값 0.6f에 암묵적으로 의존)
stone_tile_dark->Draw(matrix, 0xFFFFFFFF, DrawDepth::TILE);
stone_tile_bright->Draw(matrix, 0xFFFFFFFF, DrawDepth::TILE);

// 이동 범위 오버레이 (현재: 0.2f)
renderer_2d->DrawRectangle(..., DrawDepth::OVERLAY);

// 경로 하이라이트 (현재: 0.1f)
renderer_2d->DrawRectangle(..., DrawDepth::PATH);
```

## 수정 파일 목록

| 파일                                                     | 변경 내용                                                      |
| ------------------------------------------------------ | ---------------------------------------------------------- |
| `Engine/DrawDepth.h` (신규)                              | 레이어별 depth 상수 정의                                           |
| `Engine/Texture.h` 98, 133번 줄                          | 기본값 → `DrawDepth::CHARACTER`                               |
| `CS200/IRenderer2D.h` 35, 39번 줄                        | 기본값 → `DrawDepth::CHARACTER`                               |
| `Engine/Sprite.h` 38번 줄                                | `0.5f` → `DrawDepth::CHARACTER`                            |
| `Engine/GameObject.h` 56번 줄                            | `0.5f` → `DrawDepth::CHARACTER`                            |
| `Engine/Particle.h` 24번 줄                              | `0.5f` → `DrawDepth::CHARACTER`, `DrawPriority()` 오버라이드 제거 |
| `StateComponents/GridSystem.cpp` 117, 119, 158, 179번 줄 | 매직 넘버 → named constant                                     |

## 검증

1. `cmake --build --preset windows-debug`로 빌드
2. 실행 후 렌더 순서 확인: 타일 → 캐릭터 → 파티클 → 오버레이 → 경로 순
3. Particle 렌더 순서가 기존과 동일한지 확인 (DrawPriority 제거 후 `DrawDepth::CHARACTER = 0.5f`로 그려지게 되므로, 만약 파티클이 캐릭터 위에 보여야 한다면 Particle::Draw 호출부에서 `DrawDepth::PARTICLE`을 명시적으로 전달하도록 추가 수정 필요)



#### 참고

- RenderingAPI.cpp 수정할것
  
  ```cpp
  // GL::Disable(GL_DEPTH_TEST);
  	GL::Enable(GL_DEPTH_TEST);
  
  void Clear() noexcept
    {
  	// GL::Clear(GL_COLOR_BUFFER_BIT);
  	GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  ```
