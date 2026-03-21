# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG — 커스텀 C++20 OpenGL 엔진 (CMake), 5명 팀, 26주 개발.

- **현재 상태**: Week 4-5 진행 중 (Week 1-3, Milestone 3 완료)
- **진행 중**: ClericStrategy, WizardStrategy, RogueStrategy 구현
- **폐기 예정**: `Abilities/` 디렉토리 전체 (AbilityBase, MeleeAttack, ShieldBash) — 기본 공격(ActionAttack)만 사용

---

## 빌드

**⚠️ 빌드 명령은 반드시 `DragonicTactics/` 디렉토리에서 실행 (프로젝트 루트 아님)**

```bash
cd DragonicTactics
cmake --preset windows-debug        # 구성
cmake --build --preset windows-debug # 빌드
build/windows-debug/dragonic_tactics.exe  # 실행 (반드시 DragonicTactics/에서)
```

프리셋: `windows-debug`, `windows-developer-release`, `windows-release`, `linux-debug`, `web-debug-on-windows`

캐시 초기화: `rm -rf build/ && cmake --preset windows-debug`

---

## 핵심 아키텍처 원칙

### 두 가지 컴포넌트 시스템 (혼용 금지)

| 대상 | 추가 | 접근 | 업데이트 |
|------|------|------|----------|
| **GameState** | `AddGSComponent(new X())` | `GetGSComponent<X>()` | `UpdateGSComponents(dt)` |
| **GameObject** | `AddGOComponent(new X())` | `GetGOComponent<X>()` | `GameObject::Update()`에서 자동 |

### StateComponent 싱글톤 패턴

모든 게임 시스템(`GridSystem`, `TurnManager`, `CombatSystem`, `EventBus` 등)은 GameState 컴포넌트이며 `Instance()`로 접근:

```cpp
// GamePlay::Load()에서 등록
AddGSComponent(new EventBus());
AddGSComponent(new GridSystem());

// 어디서든 접근
EventBus::Instance().Publish(event);
GridSystem::Instance().MoveCharacter(character, position);
```

### AI Strategy 패턴

"사실은 Character가, 판단은 Strategy가":
- `Character`: `GetHPPercentage()` → 사실 반환 (0.35f)
- `Strategy`: `IsInDanger()` → 판단 (0.35f ≤ 0.4f → true)

새 전략 추가 시: `architecture/character_flowchart/`에 Mermaid 플로우차트 → `StateComponents/AI/`에 `IAIStrategy` 구현 → `AISystem`에 등록.

### ⚠️ ActionPoints vs MovementRange (혼동 주의)

```cpp
if (actor->GetMovementRange() > 0) { /* 이동 로직 */ }  // ✅ 이동 가능 타일 수
if (actor->GetActionPoints() > 0)  { /* 공격 로직 */ }  // ✅ 턴당 행동 횟수
```

---

## 새 파일 추가 규칙

1. 헤더: `.h` 확장자 (`.hpp` 아님)
2. 모든 `.cpp` 첫 줄: `#include "pch.h"`
3. CMake가 GLOB_RECURSE로 자동 감지 → CMakeLists.txt 수동 편집 불필요
4. 새 파일 추가 후 `cmake --preset windows-debug` 재실행
5. 캐릭터 생성: `new Dragon()` 대신 `CharacterFactory::Create()` 사용

---

## 테스트 단축키 (GamePlay 상태)

| 키 | 동작 |
|----|------|
| F | EventBus 테스트 |
| E | DiceManager 테스트 |
| T/Y | Dragon/Fighter 턴 상태 표시 |
| D/H | Dragon 공격 / Fighter 힐 |
| S | Grid 시각화 |
| P | TurnManager 테스트 |
| J/R/L | JSON 로드 / 리로드 / 로그 |
| Enter | 전체 전투 시스템 테스트 |

---

## 네임스페이스

- `CS230` — 엔진 코어 (Engine, GameObject, GameState, Component)
- `CS200` — 렌더링 추상화 (IRenderer2D, RenderingAPI)
- `OpenGL` — 저수준 OpenGL 래퍼
- `Math` — 수학 유틸리티 (vec2, ivec2, TransformationMatrix)

---

## 주요 시스템 위치

- **게임 시스템**: `source/Game/DragonicTactics/StateComponents/`
- **AI 전략**: `StateComponents/AI/` (IAIStrategy 인터페이스 + 캐릭터별 전략)
- **캐릭터 엔티티**: `source/Game/DragonicTactics/Objects/`
- **타입 정의**: `source/Game/DragonicTactics/Types/`
- **JSON 데이터**: `DragonicTactics/Assets/Data/`
- **AI 플로우차트**: `architecture/character_flowchart/` (Mermaid .mmd)

엔진 접근: `Engine::GetLogger()`, `Engine::GetInput()`, `Engine::GetWindow()`, `Engine::GetGameStateManager()`

렌더러 접근: `CS230::TextureManager::GetRenderer2D()` (Immediate/Batch/Instanced 런타임 전환 가능)

---

## 문서 참조

- [docs/implementation-plan.md](docs/implementation-plan.md) — 26주 구현 계획 (우선순위 기반)
- [docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/) — 주차별 상세 가이드 (한글)
- [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md) — 아키텍처 원칙
