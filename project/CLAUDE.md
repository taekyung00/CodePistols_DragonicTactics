# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG — 커스텀 C++20 OpenGL 엔진 (CMake), 5명 팀, 26주 개발.

- **폐기됨**: `Abilities/` 디렉토리 전체 (AbilityBase, MeleeAttack, ShieldBash) — `Objects/Actions/ActionAttack`만 사용
- **진행 중**: ClericStrategy, WizardStrategy, RogueStrategy 구현 (`FighterStrategy`가 참조 구현)

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

**GameState 컴포넌트 (싱글톤)**: `EventBus`, `GridSystem`, `TurnManager`, `AISystem`, `CombatSystem`, `DiceManager`, `SpellSystem`, `DataRegistry`, `MapDataRegistry`

**GameObject 컴포넌트**: `GridPosition`, `ActionPoints`, `StatsComponent`, `SpellSlots`, `MovementComponent`

### StateComponent 싱글톤 패턴

모든 게임 시스템은 GameState 컴포넌트이며 `Instance()`로 접근:

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

새 전략 추가 시:
1. `architecture/character_flowchart/`에 Mermaid 플로우차트 작성
2. `StateComponents/AI/`에 `IAIStrategy` 구현 (`FighterStrategy`가 참조 구현)
3. `AISystem`에 `RegisterStrategy(CharacterTypes::X, new XStrategy())` 등록

`AIDecision` 구조체: `type`(Move/Attack/UseAbility/EndTurn/None), `target`, `destination`, `abilityName`, `reasoning`

### ⚠️ ActionPoints vs MovementRange (혼동 주의)

```cpp
if (actor->GetMovementRange() > 0) { /* 이동 로직 */ }  // ✅ 이동 가능 타일 수
if (actor->GetActionPoints() > 0)  { /* 공격 로직 */ }  // ✅ 턴당 행동 횟수
```

### Action 시스템 (`Abilities/` 대체)

```cpp
// Action 인터페이스 (Objects/Actions/Action.h)
action->GetActionPointCost()
action->CanExecute(performer, context)    // 사전 검사
action->Execute(performer, target, tile)  // 실행
```

현재 구현: `ActionAttack` (근거리/원거리 공격). 새 Action은 `Action`을 상속해 구현.

### 게임 플로우 (주요 클래스 협력)

```
GamePlay (GameState)
  ├── PlayerInputHandler  — 플레이어 입력 → Action 실행
  ├── BattleOrchestrator  — 전투 흐름 제어 (턴 시작/종료 조율)
  ├── GamePlayUIManager   — UI 패널 관리
  └── TurnManager::Instance() → AISystem::Instance() → Strategy::MakeDecision()
```

---

## 새 파일 추가 규칙

1. 헤더: `.h` 확장자 (`.hpp` 아님)
2. 모든 `.cpp` 첫 줄: `#include "pch.h"`
3. CMake가 GLOB_RECURSE로 자동 감지 → CMakeLists.txt 수단 편집 불필요
4. 새 파일 추가 후 `cmake --preset windows-debug` 재실행
5. 캐릭터 생성: `new Dragon()` 대신 `CharacterFactory::Create()` 사용

---

## 테스트

**자체 테스트 프레임워크** (외부 라이브러리 없음): `Test/TestAssert.h` + 목 객체 (`Week1TestMocks`, `Week3TestMocks`)

테스트 파일: `source/Game/DragonicTactics/Test/` — `TestEventBus`, `TestAI`, `TestAStar`, `TestCombatSystem`, `TestDiceManager`, `TestSpellSystem`, `TestTurnManager`, `TestDataRegistry`

**런타임 테스트 단축키 (GamePlay 상태)**:

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

## 데이터 주도 설계

게임 밸런스 파라미터는 코드가 아닌 데이터 파일에서 로드 (`DataRegistry` / `MapDataRegistry`):
- `Assets/Data/characters.json` — 캐릭터 스탯
- `Assets/Data/maps.json` — 맵 구성
- `Assets/Data/spell_table.csv` — 스펠 정의

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
- **Actions**: `Objects/Actions/` (Action.h, ActionAttack.h)
- **타입 정의**: `source/Game/DragonicTactics/Types/` (CharacterTypes.h, GameTypes.h, Events.h)
- **JSON 데이터**: `DragonicTactics/Assets/Data/`
- **AI 플로우차트**: `architecture/character_flowchart/` (Mermaid .mmd)

엔진 접근: `Engine::GetLogger()`, `Engine::GetInput()`, `Engine::GetWindow()`, `Engine::GetGameStateManager()`

렌더러 접근: `CS230::TextureManager::GetRenderer2D()` (Immediate/Batch/Instanced 런타임 전환 가능)

---

## 문서 참조

- [docs/implementation-plan.md](docs/implementation-plan.md) — 26주 구현 계획 (우선순위 기반)
- [docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/) — 주차별 상세 가이드 (한글)
- [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md) — 아키텍처 원칙
- [architecture/Implementation_Checklist.md](architecture/Implementation_Checklist.md) — 진행 체크리스트
