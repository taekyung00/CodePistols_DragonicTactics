# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 코드 탐색 출발점

**`DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp`가 이 프로젝트의 중심 파일이다.**
낯선 코드를 이해하려면 이 파일에서 시작해 연결을 따라가는 것이 가장 빠르다.

| 함수                      | 역할                                                                   |
| ----------------------- | -------------------------------------------------------------------- |
| `GamePlay::Load()`      | 모든 GS 컴포넌트 등록 + CSV/JSON 데이터 로드 + 캐릭터 스폰                             |
| `GamePlay::Update()`    | `PlayerInputHandler` / `BattleOrchestrator` / `GameObjectManager` 구동 |
| `GamePlay::DrawImGui()` | Player Actions / Action List / Spell List / Map Selection 패널         |
| `GamePlay::Draw()`      | `GridSystem::Draw()` + `GameObjectManager::DrawAll()`                |

**현재 맵 구성**: Dragon = 플레이어(좌측 하단 `'d'`), Fighter = AI 적(좌측 상단 `'f'`).

---

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG — 커스텀 C++20 OpenGL 엔진 (CMake), 5명 팀, 26주 개발.

- **폐기됨**: `Abilities/` 디렉토리 전체 (AbilityBase, MeleeAttack, ShieldBash) — `Objects/Actions/ActionAttack`만 사용
- **구현 완료**: SpellSystem (CSV 파싱 + 시전 + UI), StatusEffectHandler (9가지 효과 + OnApplied/OnRemoved), TurnManager, GridSystem, SoundManager (BGM/SFX)
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

| 대상             | 추가                        | 접근                    | 업데이트                        |
| -------------- | ------------------------- | --------------------- | --------------------------- |
| **GameState**  | `AddGSComponent(new X())` | `GetGSComponent<X>()` | `UpdateGSComponents(dt)`    |
| **GameObject** | `AddGOComponent(new X())` | `GetGOComponent<X>()` | `GameObject::Update()`에서 자동 |

**GameState 컴포넌트 등록 순서** (`GamePlay::Load()` 기준):

```
EventBus, DiceManager, AISystem, CombatSystem, GameObjectManager, GridSystem,
TurnManager, DebugManager, CharacterFactory, DataRegistry, Timer,
MapDataRegistry, SpellSystem, StatusEffectHandler
```

- `DiceManager` — `Roll("2d6")`, `Roll("1d20+5")` 형식으로 주사위 굴림
- `CombatSystem` — 공격/방어 주사위 굴림 + 최종 데미지 계산 (StatusEffectHandler 훅 연동)

**GameObject 컴포넌트**: `GridPosition`, `ActionPoints`, `StatsComponent`, `SpellSlots`, `MovementComponent`, `StatusEffectComponent`

### GS 컴포넌트 접근 패턴

GS 컴포넌트에는 `Instance()` 메서드가 없다. 접근 방법:

```cpp
// GameState 내부 (GamePlay, BattleOrchestrator 등)
GetGSComponent<EventBus>()->Publish(event);

// GameState 외부 (Strategy, System 등)
auto& gs = Engine::GetGameStateManager();
GridSystem* grid = gs.GetGSComponent<GridSystem>();
```

**EventBus 구독/발행 패턴** (`Events.h`에 이벤트 타입 정의):

```cpp
// 구독 (GamePlay::Load()에서)
GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>(
    [](const CharacterDamagedEvent& e) {
        Engine::GetSoundManager().PlaySFX(SoundManager::SFX_HIT);
    });

// 발행 (외부 시스템에서)
Engine::GetGameStateManager().GetGSComponent<EventBus>()->Publish(
    CharacterDamagedEvent{target, damage});
```

`Events.h`에 전투·피해·이동·스펠·상태·턴·UI 카테고리로 27개+ 이벤트 정의됨.

### AI Strategy 패턴

"사실은 Character가, 판단은 Strategy가":

- `Character`: `GetHPPercentage()` → 사실 반환 (0.35f)
- `Strategy`: `IsInDanger()` → 판단 (0.35f ≤ 0.4f → true)

새 전략 추가 시:

1. `architecture/character_flowchart/`에 Mermaid 플로우차트 작성
2. `StateComponents/AI/`에 `IAIStrategy` 구현 (`FighterStrategy`가 참조 구현)
3. `AISystem::Init()` (`StateComponents/AISystem.cpp`)에 등록:
   ```cpp
   m_strategies[CharacterTypes::X] = new XStrategy();
   ```

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
  ├── PlayerInputHandler  — Dragon 턴: 플레이어 입력 → Action 실행
  ├── BattleOrchestrator  — AI 턴 감지 → HandleAITurn → AISystem::MakeDecision → ExecuteDecision
  ├── GamePlayUIManager   — UI 패널 관리
  └── TurnManager → AISystem → Strategy::MakeDecision()
```

**PlayerInputHandler.ActionState** (입력 상태 머신):

```
None → SelectingMove → Moving
     → SelectingAction → TargetingForAttack
                       → SelectingSpell → TargetingForSpell
                                        → WallPlacementMulti
                                        → LavaPlacementMulti
```

Dragon(플레이어) 턴에서만 동작. AI(Fighter) 턴은 `BattleOrchestrator`가 처리.

---

## SpellSystem

`SpellSystem` (GS 컴포넌트)은 `Assets/Data/spell_table.csv`에서 스펠 데이터를 로드·관리한다.
`SpellData`, `SpellTargeting`, `SpellMove` 구조체는 **`SpellSystem.h`에 정의** (`CharacterTypes.h` 아님).

**CSV 컬럼** (8열):

```
ID, Name, Category, Classes, Required Slot Level, Targeting, Upcasting Effect, Effect
```

**Targeting 컬럼 포맷** — `{Filter}:{Geometry}:{Range}`:

| 항목       | 허용 값                                                   |
| -------- | ------------------------------------------------------ |
| Filter   | `Enemy` \| `Ally` \| `Self` \| `Any` \| `Empty`        |
| Geometry | `Single` \| `Around` \| `Line` \| `OddEven` \| `Point` |
| Range    | 정수 (`-1` = 무한, `0` = 자신)                               |

**Effect 4줄 템플릿**:

```
Deals {formula} damage.
Applies "{STATUS}" status for {N} turns.
Move to {mover:move_type:distance}.
Summons {entity} at {location}.
```

Move 값: `self:stay:0` (이동 없음) | `target:knockback:N` (밀쳐냄) | `self:teleport:selected` (순간이동)

**시전 흐름**:

```
CastSpell → CanCast(클래스/슬롯/Geometry/Range/AP 체크) → ConsumeSpell → AP.Consume(1) → ApplySpellEffect
  ApplySpellEffect → targets 결정(Geometry) → 피해 → 상태효과 → ApplyMoveEffect → ApplySpecialEffect
```

---

## StatusEffect 두 레이어 구조

상태 이상은 두 개의 클래스가 역할을 나눠 담당한다 (혼동 주의):

| 클래스                     | 종류          | 역할                                                          |
| ----------------------- | ----------- | ----------------------------------------------------------- |
| `StatusEffectComponent` | GOComponent | **데이터 저장소** — `ActiveEffect` 목록 보관, `Has()`/`TickDown()` 제공 |
| `StatusEffectHandler`   | GSComponent | **실행 로직** — `OnApplied`/`OnRemoved`/`ModifyDamage*` 훅 중앙 관리 |

`Character::AddEffect(name, duration, magnitude)` 호출 흐름:
1. `StatusEffectComponent::AddEffect()` → 리스트에 추가
2. `StatusEffectHandler::OnApplied()` → 즉시 실행 효과 처리 (Fear: base speed -1 등)

`ActiveEffect` 구조체 (`StatusEffectComponent.h`에 정의): `name`, `duration`, `magnitude`

### StatusEffectHandler 훅

| 훅                         | 호출 시점          | 주요 동작                                          |
| ------------------------- | -------------- | ---------------------------------------------- |
| `OnApplied(target, name)` | `AddEffect` 직후 | Fear→base speed-1, Haste→+1, Purify→전체 제거 전 복원 |
| `OnRemoved(target, name)` | 효과 만료/Purify 시 | Fear→base speed+1, Haste→-1 복원                 |
| `ModifyDamageDealt`       | 피해 계산          | Blessing+3, Fear-3, Curse-3, Stealth×2         |
| `ModifyDamageTaken`       | 피해 계산          | Blessing-3, Curse+3                            |
| `OnAfterAttack`           | ApplyDamage 직후 | Stealth 소모, Lifesteal 회복, Frenzy 발동            |
| `OnTurnStart`             | 턴 시작           | Exhaustion→AP/Speed 0, Haste→AP+1              |

**⚠️ base speed 수정 주의**:

- `StatsComponent::ModifyBaseSpeed(int delta)` → `stats.speed` 직접 변경 후 `RefreshSpeed()` 호출
- `ReduceSpeed(i)` → `m_current_speed`만 건드림 → `RefreshSpeed()`로 매 턴 초기화됨 → Fear/Haste에 사용하면 안 됨

---

## 새 파일 추가 규칙

1. 헤더: `.h` 확장자 (`.hpp` 아님)
2. 모든 `.cpp` 첫 줄: `#include "pch.h"`
3. CMake가 GLOB_RECURSE로 자동 감지 → CMakeLists.txt 수동 편집 불필요
4. 새 파일 추가 후 `cmake --preset windows-debug` 재실행
5. 캐릭터 생성: `new Dragon()` 대신 `CharacterFactory::Create()` 사용

---

## 테스트

**자체 테스트 프레임워크** (외부 라이브러리 없음): `Test/TestAssert.h` + 목 객체 (`Week1TestMocks`, `Week3TestMocks`)

테스트 파일: `source/Game/DragonicTactics/Test/` — `TestEventBus`, `TestAI`, `TestAStar`, `TestCombatSystem`, `TestDiceManager`, `TestTurnManager`, `TestDataRegistry`, `TestAbility`, `TestMemory`, `TestNew`, `TestTurnInit`

**런타임 테스트 단축키 (GamePlay 상태)**:

| 키     | 동작                     |
| ----- | ---------------------- |
| F     | EventBus 테스트           |
| E     | DiceManager 테스트        |
| T/Y   | Dragon/Fighter 턴 상태 표시 |
| D/H   | Dragon 공격 / Fighter 힐  |
| S     | Grid 시각화               |
| P     | TurnManager 테스트        |
| J/R/L | JSON 로드 / 리로드 / 로그     |
| Enter | 전체 전투 시스템 테스트          |

---

## 데이터 주도 설계

게임 밸런스 파라미터는 코드가 아닌 데이터 파일에서 로드 (`DataRegistry` / `MapDataRegistry`):

- `Assets/Data/characters.json` — 캐릭터 스탯 (`CharacterData`: hp, speed, ap, attack_dice, spell_slots 등)
- `Assets/Data/maps.json` — 맵 구성
- `Assets/Data/spell_table.csv` — 스펠 정의 (`SpellData`: Targeting + Effect 템플릿)
- `Assets/Data/status_effect.csv` — 상태 이상 설정

**맵 전환**: `GamePlay::s_next_map_source`와 `GamePlay::s_next_map_index` 정적 필드를 GamePlay 전환 전에 설정해 어떤 맵을 로드할지 지정한다 (`MapSource::First` = 하드코딩 맵, `MapSource::JSON` = maps.json 선택).

---

## SoundManager (엔진 서비스)

GameState 컴포넌트가 아닌 **엔진 레벨 서비스**. `Engine::GetSoundManager()`로 접근.

- **BGM**: OGG 파일 (`Assets/Audio/BGM/`) → 루프 재생
- **SFX**: WAV 파일 (`Assets/Audio/SFX/`) → 단발, 8채널 소스 풀

상수 경로가 헤더에 정의되어 있음: `SoundManager::BGM_MAIN_MENU`, `SoundManager::BGM_BATTLE`, `SoundManager::SFX_HIT`

```cpp
Engine::GetSoundManager().PlayBGM(SoundManager::BGM_BATTLE);   // 루프 BGM 시작
Engine::GetSoundManager().StopBGM();
Engine::GetSoundManager().PlaySFX(SoundManager::SFX_HIT);      // 단발 SFX
Engine::GetSoundManager().SetBGMVolume(0.7f);                  // 0.0 ~ 1.0
```

`GamePlay::Load()`에서 `PlayBGM`, `GamePlay::Unload()`에서 `StopBGM` 호출 패턴을 따른다.

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
- **SpellData 구조체**: `StateComponents/SpellSystem.h` (SpellData, SpellTargeting, SpellMove)
- **ActiveEffect 구조체**: `Objects/Components/StatusEffectComponent.h`
- **경로 탐색**: `StateComponents/AStar.cpp` (GridSystem::FindPath에서 내부 사용)
- **JSON 데이터**: `DragonicTactics/Assets/Data/`
- **AI 플로우차트**: `architecture/character_flowchart/` (Mermaid .mmd)

엔진 접근: `Engine::GetLogger()`, `Engine::GetInput()`, `Engine::GetWindow()`, `Engine::GetGameStateManager()`

렌더러 접근: `CS230::TextureManager::GetRenderer2D()` (Immediate/Batch/Instanced 런타임 전환 가능)

---

## 문서 참조

- [docs/implementation-plan.md](docs/implementation-plan.md) — 26주 구현 계획 (우선순위 기반)
- [docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/) — 주차별 상세 가이드 (한글)
- [docs/Detailed Implementations/features/spell_system.md](docs/Detailed%20Implementations/features/spell_system.md) — SpellSystem 전체 구현 가이드 (Targeting 템플릿, Effect 파싱, 업캐스팅, Move 템플릿)
- [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md) — 아키텍처 원칙
- [architecture/Implementation_Checklist.md](architecture/Implementation_Checklist.md) — 진행 체크리스트
