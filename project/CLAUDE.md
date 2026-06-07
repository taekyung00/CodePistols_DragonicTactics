# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 코드 탐색 출발점

**`DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp`가 이 프로젝트의 중심 파일이다.**
낯선 코드를 이해하려면 이 파일에서 시작해 연결을 따라가는 것이 가장 빠르다.

| 함수                      | 역할                                                                   |
| ----------------------- | -------------------------------------------------------------------- |
| `GamePlay::Load()`      | 모든 GS 컴포넌트 등록 + CSV/JSON 데이터 로드 + 캐릭터 스폰                             |
| `GamePlay::Update()`    | `PlayerInputHandler` / `BattleOrchestrator` / `GameObjectManager` 구동 |
| `GamePlay::DrawImGui()` | Map Selection / Combat Status 개발자 패널                                 |
| `GamePlay::Draw()`      | **2-패스 렌더링**: Pass 1 = 월드 공간(TacticalCamera), Pass 2 = UI 공간(가상 1600×900) |

**현재 맵 구성**: Dragon = 플레이어, Fighter + Cleric + Rogue + Wizard = AI 적 (`maps.json` spawn_points 기준, `enemys` 벡터로 관리).

---

## 목차

- [프로젝트 개요](#프로젝트-개요)
- [게임 부팅 & 상태 흐름](#게임-부팅--상태-흐름)
- [빌드](#빌드)
- [핵심 아키텍처 원칙](#핵심-아키텍처-원칙)
- [SpellSystem](#spellsystem)
- [StatusEffect 두 레이어 구조](#statuseffect-두-레이어-구조)
- [코드 스타일](#코드-스타일)
- [웹 빌드 호환 규칙](#웹-빌드-호환-규칙-emscripten--werror-위반-방지)
- [새 파일 추가 규칙](#새-파일-추가-규칙)
- [테스트](#테스트)
- [데이터 주도 설계](#데이터-주도-설계)
- [SoundManager](#soundmanager-엔진-서비스)
- [렌더링 패턴](#렌더링-패턴)
- [문서 참조](#문서-참조)

---

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG — 커스텀 C++20 OpenGL 엔진 (CMake), 5명 팀, 26주 개발.

- **폐기됨**: `Abilities/` 디렉토리 전체 (AbilityBase, MeleeAttack, ShieldBash) — `Objects/Actions/ActionAttack`만 사용
- **구현 완료**: SpellSystem (CSV 파싱 + 시전 + UI), StatusEffectHandler (9가지 효과 + OnApplied/OnRemoved), TurnManager, GridSystem, SoundManager (BGM/SFX)
  - 9가지 효과: `Lifesteal` (피해의 50% 회복), `Frenzy` (10+ 피해 시 랜덤 디버프), `Exhaustion` (다음 턴 AP·Speed=0), `Purify` (전체 효과 제거 — 스펠 효과로만 발동, 독립 버프/디버프 아님), `Blessing` (피해 수/받 ±3), `Curse` (피해 수/받 ∓3), `Haste` (Speed+1·AP+1), `Stealth` (타겟 불가·첫 공격 2배·공격 시 해제), `Fear` (피해 -3·Speed-1)
- **구현 완료**: FighterStrategy (`fighter.mmd` 플로우차트 완전 반영 — 킬루프/생존/일반교전/원거리 분기)
- **구현 완료**: ClericStrategy (`cleric.mmd` 플로우차트 완전 반영 — 킬루프/힐/버프·디버프/근접 분기) + Cleric 캐릭터 클래스
- **구현 완료**: RogueStrategy (`rouge.mmd` 플로우차트 반영 — 은신 킬루프/버프/원거리이동/근접전투 분기) + Rogue 캐릭터 클래스
- **구현 완료**: WizardStrategy (`wizard.mmd` 플로우차트 반영 — 킬루프/Sweet Spot 이동/마나 변환/Fire Bolt·Magic Missile 분기) + Wizard 캐릭터 클래스
  - `CharacterTypes` 열거 순서: `None, Dragon, Fighter, Rogue, Cleric, Wizard, Count`
  - AI 플로우차트: `architecture/character_flowchart/wizard.mmd`

---

## 게임 부팅 & 상태 흐름

문서 대부분은 전투 본편(`GamePlay`)을 다루지만, **거기에 도달하기까지의 경로**는 별도 셸 레이어다. 진입점부터 따라가야 전체 그림이 잡힌다.

**진입점**: `source/main.cpp` → `Engine::Instance().Start("Dragonic Tactics")` → `Engine::GetGameStateManager().PushState<Splash>()` 가 **유일한** 시작점. 이후 모든 화면 전환은 `GameStateManager`의 `PushState<T>()` / `PopState()`.

```
main.cpp → Splash → MainMenu ┬─ LevelGame → LevelSelect ┬─ Level 1/2/3 → GamePlay ──[게임 종료]──→ GameOver
                             ├─ Settings                 └─ Back → MainMenu                         ├─ PLAY AGAIN → GamePlay
                             ├─ Exit                                                                 └─ MAIN MENU  → MainMenu
                             └─ [DEVELOPER_VERSION 전용] DragonicTactics → GamePlay (s_level_id=0, 스펠 제한 없음)
                                                         ConsoleTest / RenderingTest
```

- **GameOver 상태**: `source/Game/GameOver.h` / `GameOver.cpp` — `source/Game/` 직하위 (MainMenu·Settings와 동일 레벨). `GameOver::s_player_won` (static bool)으로 결과를 전달받아 "PLAYER WIN"(금색) / "INVADER WIN"(빨간색) 타이틀 표시. 전환 타이밍은 `GamePlay.h`의 `static constexpr double GAME_OVER_DELAY = 0.5` (초) — 값 변경 시 이 상수만 수정.
- **셸 레이어 위치 주의**: `Splash`·`MainMenu`·`Settings`·`GameOver`·`Score`·`Background`·`Particles`·`LevelSelect` 는 `source/Game/` **직하위**에 있다 — `source/Game/DragonicTactics/` 하위가 **아니다**. 전투 본편 코드만 `DragonicTactics/` 서브트리에 있다.
- **Splash 지속시간**: `#if defined(DEVELOPER_VERSION)` → **0.3초**, `#else` → 2.0초 (`source/Game/Splash.cpp`). 릴리즈 빌드에서 2초 스플래시를 표시.
- ⚠️ `source/Game/States.h`의 `enum class State { Splash, MainMenu, Final }`는 **레거시·미사용**이다. 실제 내비게이션은 이 enum이 아니라 `GameStateManager`의 push/pop으로 동작 — 혼동 주의.
- **Settings → GamePlay 연결**: `Settings`의 맵 크기 선택이 아래 [데이터 주도 설계](#데이터-주도-설계)의 `GamePlay::s_next_map_id` / `s_should_restart` 정적 필드를 통해 로드할 맵을 결정한다.

### 사용자 설정 저장 (user_settings.json)

창 크기와 BGM 설정을 `user_settings.json` (실행 파일 옆)에 영구 저장한다. 자세한 구현은 [복원 기능.md](docs/Detailed%20Implementations/features/%EB%B3%B5%EC%9B%90%20%EA%B8%B0%EB%8A%A5.md) 참고.

**흐름**:
```
[앱 시작] main.cpp → engine.Start() → Settings::LoadUserSettings() → ForceResize(s_window_size) → PushState<Splash>
[창 리사이즈] SDL_WINDOWEVENT_SIZE_CHANGED → Window::ResizeCallback → Settings::s_window_size 갱신 + SaveUserSettings()
```

- `Settings::s_window_size` (static, 기본 1600×900) — 창 크기 공유 정적 필드
- `Settings::LoadUserSettings()` / `SaveUserSettings()` — nlohmann/json으로 `user_settings.json` 읽기/쓰기. 저장 항목: `window_width`, `window_height`, `bgm_enabled`, `bgm_volume_pct`
- `Window::SetResizeCallback(fn)` — main.cpp에서 단 1회 등록, engine→game 단방향 의존 유지
- BGM On/Off·볼륨 변경 시 `Settings::SelectOption()`이 자동으로 `SaveUserSettings()` 호출

### LevelSelect & Level Mode

`LevelSelect` (`source/Game/LevelSelect.h/.cpp`) — 레벨 선택 화면. MainMenu의 기본 옵션(LevelGame)에서 진입.

- **레벨 수**: Level 1 · 2 · 3 (Option enum 0~2), Back(3)
- **순차 잠금** (릴리즈 빌드만): `LevelSelect::s_max_unlocked_level` (기본 1) — 레벨 클리어 시 `GamePlay::CheckGameEnd`에서 `min(3, s_level_id + 1)`로 다음 레벨 해금. DEVELOPER_VERSION에서는 잠금 없음.
- **진입**: `LevelSelect::SelectOption()` → `GamePlay::s_level_id = N` (1·2·3) 설정 → `PushState<GamePlay>()`

`GamePlay::s_level_id` — 현재 레벨 모드 식별:
- `0`: 자유 모드 (DEVELOPER_VERSION 전용 DragonicTactics 메뉴), `maps.json` + 모든 스펠 허용
- `1/2/3`: 레벨 모드, `Assets/Data/Level_Map.json`에서 해당 레벨 로드 + `s_allowed_spells` 제한

`GamePlay::s_allowed_spells` (`vector<string>`) — 레벨 모드에서 허용된 스펠 ID 목록. 빈 배열이면 모든 스펠 허용. `Level_Map.json`의 `"allowed_spells"` 배열에서 로드됨.

**Level_Map.json 구조** (`Assets/Data/Level_Map.json`):

```json
{
  "levels": [
    {
      "id": "level_1",
      "name": "Level 1 (5x7)",
      "width": 5, "height": 7,
      "tiles": ["#####", "#...#", ...],
      "legend": { "#": "wall", ".": "floor" },
      "enemies": ["fighter"],
      "allowed_spells": ["S_ATK_010", "S_GEO_020"],
      "spawn_points": { "dragon": {"x":2,"y":1}, "fighter": {"x":2,"y":5} }
    }, ...
  ]
}
```

`LoadLevelMap(int level_id)` (`GamePlay.cpp:883`) — `maps.json` 대신 `Level_Map.json`을 읽어 맵을 구성하고 `enemies` 배열의 캐릭터 타입만 스폰한다.

### 컷신 시스템 (GamePlay 진입 후 전투 시작 전)

`GamePlay::Load()` 완료 직후 전투가 시작되기 전에 컷신 3장을 순서대로 표시한다. 컷신 진행 중에는 모든 게임 로직 · 입력이 차단된다.

| 상수 | 값 | 의미 |
|---|---|---|
| `CUTSCENE_DURATION` | 1.5s | 컷당 자동 전환 시간 |
| `CUTSCENE_COUNT` | 3 | 총 컷 수 |

- **이미지**: `Assets/images/cut1.png`, `cut2.png`, `cut3.png` (가상 1600×900 풀스크린 렌더)
- **조작**: 좌클릭 또는 `Space` → 현재 컷 즉시 넘기기; `Escape` → 전체 스킵
- **렌더**: `Draw()`에서 컷신 페이즈 시 Pass 2 UI NDC만 사용 (월드 패스 없음)
- ⚠️ 새 컷 추가 시 `CUTSCENE_COUNT` 상수와 `m_cutscene_textures_` 로드 라인 양쪽 수정 필요

---

## 빌드

**⚠️ 빌드 명령은 반드시 `DragonicTactics/` 디렉토리에서 실행 (프로젝트 루트 아님)**

```bash
cd DragonicTactics
cmake --preset windows-debug        # 구성
cmake --build --preset windows-debug # 빌드
build/windows-debug/dragonic_tactics.exe  # 실행 (반드시 DragonicTactics/에서)
```

프리셋: `windows-debug`, `windows-developer-release`, `windows-release`, `linux-debug`, `web-debug-on-windows` (그 외 `linux-developer-release`, `linux-release`, `web-debug`, `web-developer-release`, `web-release`)

### 코드 포맷 (clang-format)

포맷 규칙은 `DragonicTactics/.clang-format`에 정의되어 있다. 모든 명령은 **`DragonicTactics/` 디렉토리**에서 실행.

```bash
# 단일 파일 포맷 검사 (변경 없음 — 오류 시 exit 1)
clang-format --dry-run --Werror source/Game/DragonicTactics/States/GamePlay.cpp

# 단일 파일 자동 포맷 (인플레이스)
clang-format -i source/Game/DragonicTactics/States/GamePlay.cpp

# source/ 전체 일괄 포맷 (Windows PowerShell)
Get-ChildItem -Recurse -Path source -Include *.cpp,*.h | ForEach-Object { clang-format -i $_.FullName }
```

### Developer vs Release 빌드

`CMakePresets.json`의 프리셋 캐시 변수 `IS_DEVELOPER_VERSION`이 빌드 변형을 가른다. **테스트·디버그 도구의 존재 여부 자체가 여기서 결정된다.**

| 프리셋 | `IS_DEVELOPER_VERSION` | Test/ConsoleTest/RenderingTest | Windows 서브시스템 |
| ---- | ---- | ---- | ---- |
| `windows-debug` / `windows-developer-release` / `web-debug-on-windows` / `linux-debug` / `linux-developer-release` / `web-debug` / `web-developer-release` | `TRUE` | 컴파일됨 | `CONSOLE` (콘솔창 표시) |
| `windows-release` / `linux-release` / `web-release` | `FALSE` | **GLOB에서 제외** | `WINDOWS` (콘솔 없음) |

- `TRUE` 시 `source/CMakeLists.txt`가 `DEVELOPER_VERSION` + `_DEBUG` 매크로를 정의한다. 코드의 `#if defined(DEVELOPER_VERSION)` 가 ConsoleTest ImGui 패널과 MainMenu 개발자 메뉴 항목(ConsoleTest/RenderingTest)을 게이트한다.
- `FALSE` 시 `source/CMakeLists.txt`가 `/Test/`, `States/ConsoleTest.*`, `States/RenderingTest.*` 를 소스 GLOB에서 제외 → 테스트·테스트 진입 상태가 바이너리에 아예 포함되지 않는다.
- ⚠️ **테스트·디버그가 필요하면 `windows-debug`(또는 `*-developer-release`)로 빌드할 것. `windows-release`에는 존재하지 않는다.**

캐시 초기화: `rm -rf build/ && cmake --preset windows-debug`

모든 설정 일괄 빌드 (필터 옵션 있음) — 스크립트는 `DragonicTactics/scripts/` 아래:
```bash
python3 scripts/scan_build_project.py          # DragonicTactics/ 에서 실행
python3 scripts/scan_build_project.py --help
```

### 플레이테스트 릴리즈 자동화

`DragonicTactics/scripts/make_release.py` (또는 동일 위치의 `make_release.exe`) — 릴리즈 빌드 → ZIP 패키징 → GitHub Release 업로드를 자동화한다.

```bash
cd DragonicTactics

# ZIP 생성 (windows-release 빌드 포함)
python scripts/make_release.py --version v0.3.0

# 이미 빌드된 경우 빌드 생략
python scripts/make_release.py --version v0.3.0 --skip-build

# ZIP 생성 + GitHub Release 업로드
python scripts/make_release.py --version v0.3.0 --upload
```

더블클릭(`make_release.exe`) 실행 시 버전·옵션을 프롬프트로 입력받는다.
사전 점검 로직 내장: `cmake` / `gh` CLI 미설치 시 winget으로 자동 설치, `gh` 미로그인 시 `gh auth login` 자동 실행.

⚠️ **이 배포본은 git 워킹 트리가 아닐 수 있다** — zip 플레이테스트 패키지로 추출한 소스에는 `project/`·`DragonicTactics/` 어디에도 `.git`이 없어 모든 디렉토리에서 `git` 명령이 "not a git repository"로 실패한다. 버전 관리·`make_release.py --upload`의 `gh`/git 연동을 쓰려면 **정상 클론(git 루트가 존재하는 체크아웃)에서 작업**해야 하며, `make_release.py`는 `DragonicTactics/` 기준으로 실행된다. git 오류가 나면 먼저 `.git` 존재 여부(zip 배포본인지)부터 확인할 것.

ZIP 출력 위치: 프로젝트 루트(`project/DragonicTactics_vX.Y.Z_playtest.zip`) — 루트 `.gitignore`의 `*.zip` 규칙으로 자동 무시됨.

ZIP 내부 구조:
```
CODEPISTOLS_DRAGONICTACTICS/
├── dragonic_tactics.exe
├── OpenAL32.dll / SDL2.dll / soft_oal.dll
├── README.md / README(KOR).md
└── Assets/
```

릴리즈 노트는 `make_release.py` 상단 `RELEASE_NOTES` 상수에 하드코딩 — 변경 시 상수 수정 후 exe 재빌드 필요:
```bash
cd DragonicTactics
python -m PyInstaller --onefile --console --name make_release --distpath scripts scripts/make_release.py
```

### 크래시 덤프 (`source/main.cpp`)

Windows 빌드에 `SetUnhandledExceptionFilter` 기반 크래시 핸들러가 등록되어 있다. 크래시 발생 시 실행 파일 옆에 `crash_YYYYMMDD_HHMMSS.dmp`를 자동 생성한다.

- **분석**: Visual Studio → 파일 → 열기 → `.dmp` → "Debug with Native Only" — 크래시 시점의 콜스택·변수 상태 확인 가능
- **조건**: `.pdb` 파일이 `.exe`와 같은 폴더에 있어야 심볼(함수명·라인 번호) 표시됨. `windows-debug` 빌드는 자동 생성.
- `#if defined(_WIN32)` 게이팅 — Emscripten/Linux 빌드 영향 없음

---

## 핵심 아키텍처 원칙

> 전체 원칙은 [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md) 참고.

### 두 가지 컴포넌트 시스템 (혼용 금지)

| 대상             | 추가                        | 접근                    | 업데이트                        |
| -------------- | ------------------------- | --------------------- | --------------------------- |
| **GameState**  | `AddGSComponent(new X())` | `GetGSComponent<X>()` | `UpdateGSComponents(dt)`    |
| **GameObject** | `AddGOComponent(new X())` | `GetGOComponent<X>()` | `GameObject::Update()`에서 자동 |

**GameState 컴포넌트 등록 순서** (`GamePlay::Load()` 기준):

```
EventBus, DiceManager, AISystem, CombatSystem, GameObjectManager, GridSystem,
TurnManager, DebugManager, CharacterFactory, DataRegistry, util::Timer,
MapDataRegistry, SpellSystem, StatusEffectHandler,
CS230::ParticleManager<Particles::Hit>
```

- `DiceManager` — `Roll("2d6")`, `Roll("1d20+5")` 형식으로 주사위 굴림. `RollEntry { notation, rolls, total }` 구조체를 `roll_log_`(최대 200개)에 누적 — `GetRollLog()`로 전체 로그 조회. DebugVisualizer의 Dice History 탭은 `last_dice_log_read_` 인덱스로 새 항목만 읽음 (업캐스트처럼 한 프레임에 여러 번 굴리는 경우도 전부 기록)
- `CombatSystem` — 공격/방어 주사위 굴림 + 최종 데미지 계산 (StatusEffectHandler 훅 연동)
- `util::Timer` — 엔진 제공 타이머 (`Engine/Timer.h`). BattleOrchestrator는 이를 사용하지 않음 — AI 대기는 `m_wait_timer` (double, dt 카운트다운)로 처리

### ⚠️ 미수정 크래시 위험 (상세: `docs/Detailed Implementations/features/crash_risks.md`)

CombatSystem / DiceManager 관련 코드를 수정할 때 반드시 확인할 것.

| 위치 | 문제 | 심각도 |
|---|---|---|
| `StateComponents/CombatSystem.cpp` `ExecuteAttack` 첫 null 체크 | 에러 메시지에서 `attacker->TypeName()` 역참조 — attacker=null이면 crash | 🔴 CRITICAL |
| `StateComponents/StatusEffectHandler.cpp:144` | Frenzy 발동 시 `DiceManager` nullptr 체크 없음 | 🟠 HIGH |

✅ **수정 완료** (이전 :29 `CalculateDamage` null 역참조, :184 `RollAttackDamage` DiceManager nullptr — 현재 코드에서 모두 수정됨)
| `StateComponents/SpellSystem.cpp:314,350` | upcast_dice `std::stoi` 예외 미처리 | 🟠 HIGH |
| `StateComponents/SpellSystem.cpp:326` | `flat_per_level` `std::stoi` 예외 미처리 | 🟠 HIGH |

**GameObject 컴포넌트**: `GridPosition`, `ActionPoints`, `StatsComponent`, `SpellSlots`, `MovementComponent`, `StatusEffectComponent`, `ShakeComponent`

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

`Events.h`에 전투·피해·이동·스펠·상태·턴·UI 카테고리로 27개+ 이벤트 정의됨. 자주 쓰는 것:

| 이벤트 | 주요 필드 |
|---|---|
| `TurnStartedEvent` | character, turnNumber, actionPoints |
| `TurnEndedEvent` | character, actionsUsed |
| `CharacterDamagedEvent` | target, damageAmount, remainingHP, attacker, wasCritical |
| `CharacterHealedEvent` | target, healAmount, currentHP, healer |
| `CharacterDeathEvent` | character, killer |
| `CharacterMovedEvent` | character, fromGrid, toGrid |
| `SpellCastEvent` | caster, spellName, spellLevel, targetGrid |
| `StatusEffectAddedEvent` | target, effectName, duration, magnitude |
| `StatusEffectRemovedEvent` | target, effectName, reason |
| `AIDecisionEvent` | actor, decision_type, decision_target, destination |
| `BattleEndedEvent` | playerVictory, turnsElapsed |
| `UINoticeEvent` | message — 화면 상단 중앙 토스트 1.5초 표시 (재사용 가능) |

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

`AIDecision` 구조체: `type`(Move/Attack/UseAbility/EndTurn/None), `target`, `destination`, `abilityName`, `reasoning`, `lava_penalty`(이동 시 용암 타일 회피 가중치, 0 = 무시), `upcast_level`(스펠 업캐스트 레벨, 0 = 최소 레벨 자동 사용)

**AI 이동 로직 주의사항** (FighterStrategy/ClericStrategy 공통):
- `LAVA_TILE_PENALTY = 2` — 용암 경로를 비선호하나 가능하면 감수
- `FindNextMovePos()` — Dragon/target 인접 타일로 A* 탐색. **Empty + Lava** 모두 공격 위치로 허용
- `FindClosestReachableTile()` — `FindNextMovePos()`가 현재 위치를 반환(경로 차단)할 때 호출. BFS로 도달 가능 타일 중 Dragon에 최근접 타일 반환 (AI stuck 방지)
- Cleric 힐 이동 시 `FindNextMovePos(actor, target, grid, 0)` — lava_penalty=0으로 호출해 용암 완전 무시
- **공격 위치 유효성 체크 패턴** (전략 코드 내부): `IsWalkable()` 대신 명시적 타입 체크 사용
  ```cpp
  TileType t = grid->GetTileType(pos);
  bool ok = (t == TileType::Empty || t == TileType::Lava) && !grid->IsOccupied(pos);
  ```

**FighterStrategy 의사결정 구조** (`StateComponents/AI/FighterStrategy.cpp`):

```
MakeDecision
  ├── CanReachThisTurn && CanKillDragonThisTurn → MakeKillLoopDecision
  └── Phase_Decision
        ├── distance > 1 → MakeFarMoveDecision    (이동 or Fear Cry)
        ├── HP ≤ 40%     → MakeSurvivalDecision    (Bloodlust → Fear Cry → 공격)
        └── HP > 40%     → MakeNormalCombatDecision (Blessing/Frenzy/Smite/Fear Cry)
```

**Fighter 전용 스펠 ID** (`Assets/Data/spell_table.csv` 기준):

| 스펠 | ID | 레벨 | 타겟팅 |
|---|---|---|---|
| Smite | `S_ATK_050` | 1 | Enemy:Single:1 (인접 필수) |
| Bloodlust | `S_ENH_010` | 2 | Self:Single:0 |
| Frenzy | `S_ENH_020` | 2 | Self:Single:0 |
| Fearful Cry | `S_DEB_020` | 1 | Enemy:Around:3 |

**ClericStrategy 의사결정 구조** (`StateComponents/AI/ClericStrategy.cpp`):

```
MakeDecision
  ├── CanReachThisTurn && CanKillDragonThisTurn → MakeKillLoopDecision (기본 공격, 공격 스펠 없음)
  └── Phase_Decision
        ├── AP = 0 & MP > 0 → TacticalMove → EndTurn
        ├── AP = 0 & MP = 0 → EndTurn
        ├── [2순위] 힐 대상 HP < 30% + 슬롯 + 거리 ≤ 5 → S_ENH_030 Healing Touch (범위 밖이면 아군에게 이동)
        ├── 슬롯 있음 → MakeSupportDecision
        │     ├── !Cursed && 거리 ≤ 5 → S_DEB_010 Curse
        │     ├── !Blessed 아군(파이터>로그>위자드) && 거리 ≤ 4 → S_BUF_010 Divine Shield (자신 제외)
        │     └── 지원 불필요 → MakeMeleePhaseDecision
        └── 슬롯 없음 → MakeMeleePhaseDecision
              ├── 인접 → 기본 공격
              └── 비인접 & MP > 0 → 이동
```

**Cleric 스펠 ID** (`Assets/Data/spell_table.csv` 기준):

| 스펠 | ID | 레벨 | 타겟팅 | 전략 상수 |
|---|---|---|---|---|
| Healing Touch | `S_ENH_030` | 1 | Ally:Single:**5** | `HEAL_RANGE=5` |
| Divine Shield | `S_BUF_010` | 1 | Ally:Single:**4** | 파이터>로그>위자드, 자신 제외, `BLESSING_RANGE=4` |
| Curse of Suffering | `S_DEB_010` | 1 | Enemy:Single:**5** | `CURSE_RANGE=5` |

**Cleric 스탯** (`Assets/Data/characters.json`): HP 90, Speed 2, AP 1, 1d6 공격/1d8 방어, 슬롯 Lv1×3 / Lv2×2

**Rogue 스탯** (`Assets/Data/characters.json`): HP 65, Speed 4, AP 1, 1d8 공격/1d6 방어, 슬롯 Lv1×4 / Lv2×2

**Wizard 스탯** (`Assets/Data/characters.json`): HP 70, Speed 2, AP 1, 1d4 공격/1d4 방어, 슬롯 Lv1×4 / Lv2×3

**Dragon 스탯** (`Assets/Data/characters.json`): HP 140, Speed 5, AP 2, 3d6 공격/2d8 방어, 공격 범위 3, 슬롯 Lv1×4 / Lv2×3 / Lv3×2 / Lv4×2 / Lv5×1

**Dragon 스펠 ID** (`Assets/Data/spell_table.csv` 기준):

| 스펠 | ID | 레벨 | 타겟팅 | 비고 |
|---|---|---|---|---|
| Fire Bolt | `S_ATK_010` | 1 | Enemy:Single:4 | 2d8 피해, 업캐스트: +1d6/레벨 (Wizard 공유) |
| Fearful Cry | `S_DEB_020` | 1 | Enemy:Around:3 | Fear 3턴 (Fighter 공유) |
| Tail Swipe | `S_ATK_020` | 2 | Enemy:Around:2 | 1d8 피해 + knockback:2 |
| Dragon's Fury | `S_ATK_030` | 3 | Enemy:Line:4 | 4d6 피해, 업캐스트: +2d6/레벨 |
| Meteor | `S_ATK_040` | 3 | Any:OddEven:-1 | 3d20 피해 전체맵, **자신 Exhaustion 1턴**, 업캐스트: +1d20/레벨 |
| Mana Conversion | `S_ENH_040` | 0 | Self:Single:0 | 자신 HP 소모 → 슬롯 회복, 업캐스트: 회수 슬롯 레벨↑ (Wizard 공유) |
| Purify | `S_ENH_050` | 1 | Self:Single:0 | 자신 모든 상태 효과 제거 |
| Magma Blast | `S_GEO_010` | 2 | Empty:Point:6 | 빈 타일 → Lava + 3d6 피해, 업캐스트: +1d4/레벨 |
| Wall Creation | `S_GEO_020` | 1 | Empty:Point:5 | 빈 타일 → Wall, 업캐스팅 가능 |

**RogueStrategy 의사결정 구조** (`architecture/character_flowchart/rouge.mmd`, 상세: `docs/Detailed Implementations/features/rogue_strategy.md`):

```
MakeDecision
  ├── IsInStealth && CanKillWithStealth → MakeKillLoopDecision (약점 공략/일반 공격 반복)
  ├── MakeBuffPhaseDecision
  │     ├── [1] !Haste + Lv1슬롯 + IsHasteMeaningful → Gale Step
  │     └── [2] !Stealth + !공격 + AP>0 → Shadow Hide (AP>0 가드 필수 — 무한루프 방지)
  ├── [이동] IsInStealth && AP=0 && Movement>0 && dist>1 → Dragon 방향 접근
  └── Phase_Decision
        ├── AP = 0 → EndTurn
        ├── 비인접 → 이동 or (미공격 & 비은신 → Shadow Hide → 종료)
        └── 인접 → 은신 상태?
              Yes → 2레벨 슬롯 → Weakpoint Strike (2배, 은신 해제)
                    없음 & ShouldBreakStealth → 일반공격 (2배)
                    없음 & 가치 없음 → 재포지셔닝 이동
              No  → !HasAttackedThisTurn → Shadow Hide → 종료
                    이미 공격 → 일반공격
```

**이상적 콤보 패턴** (Haste 발동 첫 턴):
```
Turn N   AP=1: Gale Step → OnApplied(Haste) AP+1 즉시 → AP=1
               Shadow Hide → AP=0, Stealth 적용
               dist>1 → Dragon 방향 접근 (남은 이동력 소모)
Turn N+1 AP=2 (OnTurnStart Haste +1): Shadow Hide 재적용 → AP=1 → 이동+Weakpoint Strike 2배
```

- **`IsHasteMeaningful`**: `return AP > 0;` — 과거의 `movementRange <= 0 || AP >= 2` 조건은 Rogue max_ap=1에서 AP≥2가 절대 불가하여 이동력 소진 시에만 발동하는 문제가 있었음.
- **Haste OnApplied 즉시 AP+1**: `StatusEffectHandler::OnApplied`에서 Speed+1에 더해 `target->SetActionPoints(AP+1)` 즉시 실행 — 같은 턴 Gale Step+Shadow Hide 콤보를 가능하게 함. `OnTurnStart`의 AP+1은 별도로 유지.
- **스텔스 중 이동**: AP=0 상태에서도 이동력이 남으면 Dragon에게 접근. Dragon은 스텔스 캐릭터를 타겟할 수 없으므로 후퇴 불필요.
- **FighterStrategy 포지셔닝**: `FindNextMovePos`에서 비용이 동점일 때 `attackPos.y == targetPos.y` (Dragon 측면)를 우선. 동일 비용 경로가 없을 때는 기존과 동일.
- **RogueStrategy 포지셔닝**: `FindNextMovePos`에서 비용 동점 시 Fighter와 거리가 가장 먼 Dragon 인접 위치를 우선 (협공 포지션). `prefScore = -(Fighter까지 맨해튼 거리)`, Fighter 사망 시 0으로 fallback.

**Rogue 스펠 ID** (`Assets/Data/spell_table.csv` 기준):

| 스펠 | ID | 레벨 | 타겟팅 | 비고 |
|---|---|---|---|---|
| Weakpoint Strike | `S_ATK_070` | 2 | Enemy:Single:**1** (인접 필수) | 1d10, 은신 중 2배(2d20). `Special: If target is debuffed, damage becomes 2d20` |
| Gale Step | `S_BUF_020` | 1 | Self:Single:0 | Haste 2턴 (AP+1, MP+1) |
| Shadow Hide | `S_ENH_060` | 0 (슬롯 불필요) | Self:Single:0 | Stealth 1턴 — Around 스펠이 아니므로 `target = actor` 설정 |

**⚠️ Rogue 무한루프 방지**: Weakpoint Strike(Single, range 1 인접 필수) → 비인접 시 CanCast 실패 → AP 미소모 → 무한루프. `MakeCombatDecision`에서 `dist <= 1` 직접 체크 후 비인접이면 UseAbility 반환 금지.

**WizardStrategy 의사결정 구조** (`architecture/character_flowchart/wizard.mmd`, 구현: `StateComponents/AI/WizardStrategy.cpp`):

```
MakeDecision
  ├── 공포(Fear) 상태 확인: 피해-3 / MP-1 / 텔레포트 불가 패널티 적용
  ├── [1순위] 확정 킬 가능 → Kill_Loop
  │     ├── AP > 0 & 1레벨로 충분 → Fire Bolt (Slot1)
  │     └── AP > 0 & 2레벨 필요  → Magic Missile (Slot2)
  └── Phase_Decision
        ├── AP = 0 → EndTurn
        ├── Sweet Spot 아님 → MakeMoveDecision
        │     ├── [1] 이동력 범위 내 Sweet Spot 도달 가능 → 걷기 (AP 절약 우선)
        │     └── [2] 걷기 불가 → 텔레포트(!공포) or 걷기(공포)
        └── Sweet Spot 도달
              ├── 슬롯 > 50% → 최적 마법 시전 (Fire Bolt/Magic Missile)
              └── 슬롯 ≤ 50% → EvalConv(HP > 30%) → Mana Conversion
                              실패 → 원거리 캔트립 (Fire Bolt)
```

- **Walk-first 최적화** (`MakeMoveDecision`): `GetReachableTiles(myPos, move_range)`로 걸어서 Sweet Spot 도달 가능한지 먼저 확인 → 가능하면 이동(AP 불소모), 불가능할 때만 Teleport(AP 1 소모). Wizard는 max AP = 1이므로 AP를 아껴야 Sweet Spot 도착 후 공격 가능.

**Wizard 스펠 ID** (`Assets/Data/spell_table.csv` 기준):

| 스펠 | ID | 레벨 | 타겟팅 | 비고 |
|---|---|---|---|---|
| Fire Bolt | `S_ATK_010` | 1 | Enemy:Single:4 | 2d8 + 업캐스트:+1d6/레벨 (Dragon 공유) |
| Magic Missile | `S_ATK_060` | 2 | Enemy:Single:**-1** (무한 사거리) | flat_per_level:8, 업캐스트 가능 |
| Teleport | `S_GEO_030` | 0 (슬롯 불필요) | Empty:Point:1 | `Move to self:teleport:selected` — 공포 상태에서는 사용 불가 |
| Mana Conversion | `S_ENH_040` | 0 | Self:Single:0 | HP 소모 → 슬롯 회복 (Dragon 공유) |

**⚠️ Cleric 스펠은 자신에게 사용 불가** — Divine Shield 포함 모든 스펠이 타 아군/적 대상. `FindAllyNeedingBuff()`는 actor 제외, 파이터>로그>위자드 순으로만 반환.

**⚠️ Cleric 무한루프 방지 (기구현됨)**: Curse/Divine Shield(Single geometry) → CanCast 실패 시 AP 미소모 → 무한루프 위험. `MakeSupportDecision`에서 `dist <= SPELL_RANGE` 직접 체크 후 범위 밖이면 `MakeMeleePhaseDecision`(이동)으로 fall-through.

**FighterStrategy 미구현 분기** (`FighterStrategy.cpp` 상단 주석 블록):
- **보물 탈출**: `actor->HasTreasure()` → Exit 타일로 이동 (`grid->HasExit()`, `grid->GetExitPosition()` 필요) — 보물 시스템 구현 후 활성화 (주석 처리 중)
- **클레릭 추적**: 미구현 — fighter.mmd에 해당 분기 없음. Fighter는 위험 시 Survival 시퀀스(Bloodlust→FearCry→공격)만 실행. 클레릭이 Fighter에게 이동하는 방식이 맞음.

**⚠️ UseAbility AIDecision 작성 시 주의**:
- `AISystem::ExecuteDecision`은 `decision.target->GetGridPosition()->Get()`을 target_tile로 `CastSpell`에 전달 (`destination` 필드 무시됨)
- `Around` geometry 스펠(Fearful Cry): caster 중심 AoE → `CanCast` 범위 체크가 `caster→target_tile` 거리로 계산 → **`target = actor(자신)`** 으로 설정해야 거리=0으로 항상 통과
- `Single` geometry 스펠: CanCast 실패(사거리 초과 또는 Stealth 대상) 시 AP 미소모 → 다음 프레임에 동일 결정 반복 → **무한루프** — **Strategy에서 반드시 `distance <= SPELL_RANGE`와 스텔스 여부를 직접 체크**해야 함.

**⚠️ Stealth + Geometry 관계**:
- `Single`/`Point` geometry + `Enemy` filter: `CanCast`에서 대상이 Stealth 중이면 false 반환 → 스펠 차단
- `Around`/`Line`/`OddEven` geometry: Stealth 체크 없음 → **AoE 스펠은 은신 중 캐릭터도 타격** (Tail Swipe, Dragon's Fury 포함)
- 플레이어가 스텔스 대상을 타겟할 때: 클릭 시가 아니라 **마우스 호버 시** `UINoticeEvent` 발행 (`PlayerInputHandler::CheckStealthHoverNotice`)
  - `TargetingForAttack` 상태: 모든 타일 호버에서 체크
  - `TargetingForSpell` 상태: `Single`/`Point` geometry 스펠에서만 체크 (AoE는 알림 불필요)

### GridSystem API

```cpp
enum class TileType { Empty, Wall, Lava, Difficult, Exit, Invalid };

// 타일 조회
grid->GetTileType(pos)            // 타일 종류
grid->IsOccupied(pos)             // 캐릭터 점유 여부
grid->IsWalkable(pos)             // 이동 가능 여부
grid->IsValidTile(pos)            // 범위 내 유효 타일 여부
grid->ManhattanDistance(a, b)     // 맨해튼 거리
grid->GetWidth() / GetHeight()    // 맵 크기

// 경로 탐색
grid->FindPath(start, goal, lava_penalty)        // A* 경로 반환 (lava_penalty=0이면 용암 무시)
grid->GetReachableTiles(start, max_distance)     // BFS 기반 이동 가능 타일 목록

// 캐릭터 배치 관리
grid->AddCharacter(character, pos)
grid->RemoveCharacter(pos)
grid->MoveCharacter(old_pos, new_pos)
grid->GetCharacterAt(pos)         // → Character* (없으면 nullptr)
grid->GetAllCharacters()          // → std::vector<Character*>

// 출구 관리 (보물 탈출 시스템)
grid->HasExit()
grid->GetExitPosition()
grid->SetExitPosition(pos)

// 타일 하이라이트 시각화 (PlayerInputHandler가 제어, GridSystem::Draw()에서 렌더)
grid->EnableMovementMode(pos, range, lava_penalty = 0)  // 초록 펄스 — BFS 이동 가능 타일 + hover 경로 lava_penalty 저장
grid->DisableMovementMode()
grid->EnableSpellTargetingMode(pos, geometry, range)  // 빨간 펄스 — 스펠 타겟 타일
grid->DisableSpellTargetingMode()
grid->EnableAttackRangeMode(pos, range)   // 주황 펄스 — 공격 범위 타일 (Wall 제외, 맨해튼 거리)
grid->DisableAttackRangeMode()
grid->SetWallPreviewTiles(tiles)          // 반투명 미리보기 (WallPlacement 모드)
grid->ClearWallPreviewTiles()
grid->SetHoveredTile(tile)               // 경로 시각화
grid->ClearHoveredTile()
grid->IsMovementModeActive()             // 이동 모드 활성화 여부
grid->IsReachable(tile)                  // 특정 타일이 이동 가능한지
```

오버레이 색상 기준 (알파 80+40×sin 펄스):
- 이동 범위: 초록 `(0.0f, 0.8f, 0.0f)`
- 스펠 타겟: 빨강 `(0.8f, 0.0f, 0.0f)`
- 공격 범위: 주황 `(1.0f, 0.647f, 0.0f)`

이동 목적지 유효성 검사 패턴 (`AISystem::ExecuteDecision` 참고):
```cpp
TileType dt = grid->GetTileType(destination);
bool dest_ok = (dt == TileType::Empty || dt == TileType::Lava) && !grid->IsOccupied(destination);
```

**Dragon(플레이어) 용암 회피 이동**: `PlayerInputHandler`에서 Dragon의 이동 경로는 `PLAYER_LAVA_PENALTY = 2`를 적용해 용암 우회를 선호한다. `EnableMovementMode`와 클릭 시 `FindPath` 양쪽에 전달된다. BFS 이동 범위(초록 타일)는 변경 없음 — 용암 타일도 클릭 가능하나 경로만 우회. `GridSystem::hover_lava_penalty_`에 penalty가 저장되어 호버 경로 미리보기에도 동일하게 적용된다.

### Character 주요 API

Strategy와 시스템에서 자주 쓰는 `Character` 메서드:

```cpp
// 상태 쿼리 (Fact)
character->IsAlive()
character->GetHP() / GetMaxHP()
character->GetHPPercentage()          // 0.0 ~ 1.0
character->GetMovementRange()         // 남은 이동 타일 수
character->GetActionPoints()          // 남은 행동 포인트
character->GetAttackRange()
character->GetCharacterType()         // CharacterTypes enum
character->IsAIControlled()           // Dragon=false, Fighter/Cleric=true
character->HasAnySpellSlot()
character->GetAvailableSpellSlots(level)
character->HasAttackedThisTurn()
character->HasTreasure()

// 상태 효과
character->Has("Blessed")             // 특정 효과 보유 여부
character->AddEffect(name, duration, magnitude)
character->RemoveEffect(name)
character->RemoveAllEffects()         // 전체 제거 (Purify 등에서 사용)
character->GetActiveEffects()         // → const std::vector<ActiveEffect>&
// magnitude 조회가 필요하면 StatusEffectComponent 직접 접근:
character->GetGOComponent<StatusEffectComponent>()->GetMagnitude("Fear")  // → int (없으면 0)

// 가상 훅 (서브클래스 override 가능, TurnManager가 호출)
character->OnTurnStart()
character->OnTurnEnd()
character->TakeDamage(damage, attacker)
character->ReceiveHeal(amount)
character->PerformAction(action, target, tile_position)

// 컴포넌트 직접 접근
character->GetGridPosition()          // → GridPosition*
character->GetStatsComponent()        // → StatsComponent*
character->GetSpellSlots()            // → SpellSlots*
```

### MovementComponent API

`BattleOrchestrator`가 AI 행동 전 `IsMoving()` 체크로 애니메이션 완료를 대기한다:

```cpp
component->SetPath(vector<Math::ivec2>)  // A* 결과를 큐에 등록
component->IsMoving() const              // 이동 애니메이션 진행 중 여부
component->ClearPath()                   // 경로 강제 중단
component->SetGridSystem(GridSystem*)    // 초기화 시 주입

static constexpr double MOVE_TIME_PER_TILE = 0.2;  // 타일당 이동 시간(초)
```

`SetPath()` 호출 후 `IsMoving()`이 false로 돌아올 때까지 BattleOrchestrator는 MakeDecision을 호출하지 않는다.

### ⚠️ ActionPoints vs MovementRange (혼동 주의)

```cpp
if (actor->GetMovementRange() > 0) { /* 이동 로직 */ }  // ✅ 이동 가능 타일 수
if (actor->GetActionPoints() > 0)  { /* 공격 로직 */ }  // ✅ 턴당 행동 횟수
```

### TurnManager API

```cpp
// 전투 시작/종료
turn_manager->StartCombat()
turn_manager->EndCombat()
turn_manager->Reset()

// 턴 순서 초기화
turn_manager->InitializeTurnOrder(characters)   // 일반 / 목 캐릭터 오버로드
turn_manager->RollInitiative(characters)        // 이니셔티브 굴림 후 정렬
turn_manager->ResetInitiative()

// 이니셔티브 모드 설정
turn_manager->SetInitiativeMode(InitiativeMode::RollOnce)     // 전투 시작 시 한 번만 굴림 (D&D 기본)
turn_manager->SetInitiativeMode(InitiativeMode::RollEachRound)// 매 라운드 재굴림

// 현재 턴 상태
turn_manager->GetCurrentCharacter()            // → Character*
turn_manager->GetCurrentTurnNumber()
turn_manager->GetRoundNumber()
turn_manager->IsCombatActive()

// 턴 제어
turn_manager->StartNextTurn()
turn_manager->EndCurrentTurn()                 // BattleOrchestrator에서 AI EndTurn 시 호출

// 의존성 주입 (테스트용)
turn_manager->SetEventBus(bus)
```

`InitiativeEntry` 구조체 (`TurnManager.h`): `Character*`, `MockCharacter*`(테스트용), `int speed`

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

**BattleOrchestrator::HandleAITurn 프레임 단위 실행 패턴**:

```
매 Update 프레임:
  0. ai_character != m_last_ai_character_ → 새 턴 감지: m_think_timer_ = 1.2s (생각 중 딜레이)
     m_think_timer_ > 0 → dt 감산 후 return
  1. MovementComponent::IsMoving() → true면 즉시 return (이동 애니메이션 완료 대기)
  2. m_wait_timer > 0 → dt 감산 후 return (비차단 대기)
  3. AISystem::MakeDecision() → AIDecision 획득
  4. EndTurn → TurnManager::EndCurrentTurn() (대기 없음)
     그 외  → AISystem::ExecuteDecision() 후 m_wait_timer 설정:
              UseAbility → 0.6s (SpellDelayObject 0.5s 보장 + 여유)
              Attack     → 0.6s (AttackDelayObject 0.3s + 여유 0.3s)
              Move       → 0.3s (시각적 피드백 간격)
  5. 다음 프레임에 다시 HandleAITurn 진입 → 또 MakeDecision 반복
```

⚠️ AI는 **한 Update 프레임에 행동 1회**만 실행 후 반환 — 이동·공격·스펠 각각 별도 프레임에서 처리됨.

**GamePlay AI 캐릭터 관리** (`States/GamePlay.h`):

```cpp
Character* player = nullptr;              // Dragon (플레이어)
std::vector<Character*> enemys {};        // 모든 AI 캐릭터 (Fighter, Cleric, ...)
```

- 스폰 시 `enemys.push_back(raw_ptr)` — Fighter·Cleric 모두 동일
- `InitializeTurnOrder({ player } + enemys)` — 새 AI 추가 시 자동 반영
- `CheckGameEnd`: `std::all_of(enemys, IsAlive==false)` → 전원 사망 시 Player Win
- ⚠️ `LoadJSONMap`은 `maps.json`의 `spawn_points` 키(`"fighter"`, `"cleric"`, ...)를 찾아 스폰 — 새 캐릭터는 maps.json에 spawn_point 추가 필요
- ⚠️ **`m_confirmed_dead_` 패턴** (`GamePlay.h`): `unique_ptr`로 소유된 캐릭터가 `GameObjectManager`에서 삭제된 후 `CharacterDeathEvent` 핸들러가 뒤늦게 호출되어 댕글링 포인터가 생기는 use-after-free를 방지한다. `CheckGameEnd`는 `enemys` 포인터 값을 `m_confirmed_dead_` set에 기록해 두고, 이후 `IsAlive()` 대신 set 포함 여부로 사망 판정한다. 새 캐릭터 추가 시 동일 패턴 준수 필수 (자세한 내용: `docs/Detailed Implementations/features/character_death_crash_fix.md`).

**캐릭터 사망 시각 딜레이** (`Character.cpp`):

`TakeDamage()` → HP=0이면 그리드에서 **즉시** 제거(AI 경로탐색 차단) 후 `m_death_delay_ = 0.8` 설정. `Update()`에서 매 프레임 카운트다운 → 0 이하 시 `Destroy()`. `GamePlay`의 `CharacterDamagedEvent` 핸들러에서 `SetDeathDelay(delay)`로 덮어써 데미지 텍스트 등장 타이밍과 동기화된다.

```cpp
// CharacterDamagedEvent 핸들러 (GamePlay.cpp)
if (!event.target->IsAlive())
    event.target->SetDeathDelay(delay);  // delay = m_pending_damage_delay_ (SFX 기반)
```

- AI 공격(delay=0): `CharacterDamagedEvent` 수신 즉시 사라짐
- 플레이어 공격(delay≈0.2s): 데미지 숫자와 동시에 사라짐

**PlayerInputHandler.ActionState** (입력 상태 머신):

```
None ──[Dragon 타일 클릭]──→ SelectingMove → Moving
     ──[slot_attack 클릭]──→ TargetingForAttack
     ──[스펠 슬롯 클릭]────→ SelectingSpell → TargetingForSpell
                                            → WallPlacementMulti
                                            → LavaPlacementMulti
```

`SelectingAction` 상태는 enum에 남아있지만 실제로는 미사용. Dragon 타일 클릭으로 이동 선택 진입 (Move 버튼 없음).
Dragon(플레이어) 턴에서만 동작. AI(Fighter) 턴은 `BattleOrchestrator`가 처리.

**End Turn 이펙트 대기 잠금** (`GamePlayUIManager.cpp`):

`m_damage_texts` 중 `delay > 0`인 항목이 있으면(데미지 텍스트가 아직 화면에 나타나기 전) `slot_end_turn` 버튼을 비활성화한다. 플레이어가 공격 직후 턴을 넘기면 데미지 숫자가 적 이동 중에 나오는 현상을 방지한다. 딜레이가 만료되어 숫자가 표시되는 순간 버튼이 자동 활성화된다.

**`SoundManager::Update(dt)` 호출 위치**: `GamePlay::Update()`의 **맨 첫 줄**에서 `Engine::GetSoundManager().Update(dt)` 호출 — 컷신·게임 종료 early return 이전에 실행되어 지연 SFX 큐(`PlaySFXDelayed`)를 매 프레임 처리한다.

**⚠️ GS 컴포넌트 Update 중복 호출 금지** (`States/GamePlay.cpp`):

`UpdateGSComponents(dt)`가 등록된 모든 GS 컴포넌트(`DebugManager` 포함)의 `Update()`를 자동 호출한다. 특정 컴포넌트를 추가로 명시 호출하면 같은 프레임에 `ProcessInput()`이 두 번 실행되어 `KeyJustPressed`가 두 번 true → 토글이 ON→OFF로 즉시 복귀하는 버그 발생.

```cpp
// ❌ 잘못된 패턴 — debugMgr->Update(dt)를 명시 호출 후 UpdateGSComponents도 호출
debugMgr->Update(dt);        // 1번째 ProcessInput
UpdateGSComponents(scaledDt); // 2번째 ProcessInput → F1 두 번 처리

// ✅ 올바른 패턴
UpdateGSComponents(scaledDt); // 한 번만 호출
```

**⚠️ SpellDelayObject 타이밍 주의** (`States/BattleOrchestrator.cpp`):

`SpellSystem::CastSpell`은 효과를 **0.5초 딜레이** 후 적용(`SpellDelayObject`). `UseAbility` 결정 실행 후 `m_wait_timer = 0.6`으로 설정하면 SpellDelayObject가 완료된 뒤에 MakeDecision이 재호출된다. 이 대기가 없으면 상태 반영 전에 동일 스펠을 중복 시전한다.

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

**Effect 템플릿** (줄 순서 고정 — Line 1~4 필수, Line 5+ `Special:` 선택):

```
Deals {formula} damage.
Applies "{STATUS}" status for {N} turns [to self].
Move to {mover:move_type:distance}.
Summons {entity} at {location}.
Special: {특수 효과 설명}.   ← 선택, 복수 줄 가능 ("; "로 이어붙임)
```

**{formula} 형식** — `CalculateSpellDamage`가 처리하는 세 가지 패턴:

| 패턴 | 예시 | 계산 |
|---|---|---|
| 주사위 | `2d8`, `3d8 + Xd8` (업캐스트) | DiceManager 굴림 |
| 음수(힐) | `-(1d10 + Xd10)` | 음수 피해 = 대상 HP 회복 (`ApplySpellEffect`에서 `damage < 0` 분기) |
| 고정 배수 | `flat_per_level:8` | `8 × (upcast_level − spell_level + 1)` — Magic Missile 전용 |

Move 값: `self:stay:0` (이동 없음) | `target:knockback:N` (밀쳐냄) | `self:teleport:selected` (순간이동)

**`to self` 접미사** — Applies 줄에 `to self`를 붙이면 시전자(caster) 자신에게 효과 적용. 없으면 targets에 적용 (기본):

```csv
Applies "Exhaustion" status for 1 turn to self.   ← Dragon이 Meteor 시전 후 자신이 탈진
Applies "Blessing" status for 2 turns.             ← 피격 대상들에게 적용 (기존 방식)
```

`SpellData` 파싱 결과: `caster_effect_status` / `caster_effect_duration` 필드 (`SpellSystem.h`).

**`Special:` 줄** — `SpellData::special_effect`에 저장, `ApplySpecialEffect`에서 처리. 현재 구현된 패턴:

| 내용 | 동작 | 사용 스펠 |
|---|---|---|
| `Recover a Spell Slot of (formula) level.` | `upcast_level − spell_level + 1` 레벨 슬롯 1개 회복 | Mana Conversion |
| `If target is debuffed, damage becomes 2d20 instead.` | `Has("Curse") \|\| Has("Fear") \|\| Has("Exhaustion")` 시 2d20 교체 | Weakpoint Strike |

`SpellData::ap_cost` — AP 소모량 (기본값 1). CSV 컬럼 없음, `ParseCSVRow` 내에서 스펠 ID 기반으로 하드코딩. 현재 Meteor(`S_ATK_040`)만 `ap_cost = 3`. 새 스펠에 비표준 AP 비용이 필요하면 `ParseCSVRow` 끝부분에 `if (data.id == "S_XXX") data.ap_cost = N;`으로 추가.

**시전 흐름**:

```
CastSpell → CanCast(클래스/슬롯/Geometry/Range/AP 체크) → ConsumeSpell → AP.Consume(spell.ap_cost) → ApplySpellEffect
  ApplySpellEffect → targets 결정(Geometry) → 피해 → OnAfterAttack(Lifesteal/Frenzy/Stealth 훅) → 상태효과(targets) → 시전자 자신 효과 → ApplyMoveEffect → ApplySpecialEffect
```

**⚠️ `OnAfterAttack` 호출 위치**: `StatusEffectHandler::OnAfterAttack`은 `CombatSystem::ExecuteAttack`(기본 공격)과 `SpellSystem::ApplySpellEffect`(데미지 스펠) **양쪽에서** 호출된다. Lifesteal은 Smite 같은 데미지 스펠에도 정상 발동된다.

**넉백 시 용암 착지 → 즉시 정지 + 피해**: `ApplyMoveEffect`의 knockback 루프는 `TileType::Lava` 타일에 닿는 순간 멈추고 (`break`), 이후 `GetLavaDamageAt()`으로 피해를 즉시 적용한다. 벽(Wall)은 착지 불가, 그 앞에서 멈춤. 빈 타일은 계속 미끄러짐.

**지형 변환 스펠 (다중 타일)** — Wall Creation / Magma Blast는 단일 타일이 아닌 타일 목록을 받는 별도 API 사용:

```cpp
// 다중 Wall 배치 (WallPlacementMulti 상태에서 PlayerInputHandler가 호출)
spells->CastWalls(caster, "S_GEO_020", tiles_vector, upcast_level);

// 다중 Lava 배치 (LavaPlacementMulti 상태에서 호출)
spells->CastLavaZones(caster, "S_GEO_010", tiles_vector, upcast_level);

// 지형 효과 틱 (TurnManager::StartNextTurn에서 호출 — 용암 피해 적용 + 만료 제거)
spells->TickTerrainEffects(current_round);

// 특정 타일의 용암 피해량 조회 (SpellSystem 내부 + knockback 피해 계산)
int dmg = spells->GetLavaDamageAt(tile_pos);  // 없으면 0
```

`TerrainEffect` 구조체 (`SpellSystem.h`): `affected_tiles`, `damage_per_turn`(0이면 Wall), `created_round`, `duration_rounds`.

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
| `OnApplied(target, name)` | `AddEffect` 직후 | Fear→base speed-1, Haste→speed+1·AP+1(즉시), Purify→전체 제거 전 복원 |
| `OnRemoved(target, name)` | 효과 만료/Purify 시 | Fear→base speed+1, Haste→-1 복원                 |
| `ModifyDamageDealt`       | 피해 계산          | Blessing+3, Fear-3, Curse-3, Stealth×2         |
| `ModifyDamageTaken`       | 피해 계산          | Blessing-3, Curse+3                            |
| `OnAfterAttack`           | ApplyDamage 직후 (기본 공격 및 데미지 스펠 공통) | Stealth 소모, Lifesteal 회복, Frenzy 발동 |
| `OnTurnStart`             | 턴 시작           | Exhaustion→AP/Speed 0, Haste→AP+1              |
| `IsTargetable(target)`    | PlayerInputHandler/GridSystem 타겟 선택 시 | Stealth 중이면 false → 타겟 불가 |

**⚠️ base speed 수정 주의**:

- `StatsComponent::ModifyBaseSpeed(int delta)` → `stats.speed` 직접 변경 후 `RefreshSpeed()` 호출
- `ReduceSpeed(i)` → `m_current_speed`만 건드림 → `RefreshSpeed()`로 매 턴 초기화됨 → Fear/Haste에 사용하면 안 됨

---

## 코드 스타일

포매팅은 `DragonicTactics/.clang-format`로 강제된다 (**clang-format 14** 기준 작성). 새/수정 코드는 반드시 이 설정에 맞출 것 — 검증·정렬은 `clang-format` CLI 또는 에디터 포맷터 사용.

| 항목 | 값 |
|---|---|
| 표준 | `Standard: Latest` (C++20) |
| 중괄호 | **Allman** (`BreakBeforeBraces: Allman` — 여는 중괄호는 항상 새 줄) |
| 들여쓰기 | **탭** (`UseTab: true`, `IndentWidth/TabWidth: 4`) — ⚠️ 스페이스 아님 |
| 줄 길이 | `ColumnLimit: 200` |
| 포인터/참조 정렬 | 왼쪽 (`int* p`, `int& r` — `PointerAlignment/ReferenceAlignment: Left`) |
| 이항 연산자 | 줄바꿈 안 함 (`BreakBeforeBinaryOperators: None`) |
| 접근 지정자 | `AccessModifierOffset: -4` (클래스 본문 대비 한 단계 내어쓰기) |
| 네임스페이스 | 본문 들여쓰기 (`NamespaceIndentation: All`) |
| switch | `case` 라벨·블록 모두 들여쓰기 (`IndentCaseLabels/IndentCaseBlocks: true`) |
| include | 블록 순서 보존 + 대소문자 구분 정렬 (`IncludeBlocks: Preserve`, `SortIncludes: CaseSensitive`) |
| 연속 정렬 | 대입·선언·매크로를 주석 너머까지 정렬 (`AlignConsecutive*: AcrossComments`) — 인접 줄을 세로로 맞추므로 임의 정렬 금지 |

⚠️ 들여쓰기는 **탭**이다. 스페이스로 들여쓰면 diff 전체가 오염되니, 기존 코드의 탭 정렬을 그대로 따를 것.

---

## 웹 빌드 호환 규칙 (Emscripten `-Werror` 위반 방지)

웹 빌드(`web-release`, `web-developer-release`)는 Windows보다 엄격한 경고 플래그를 `-Werror`로 적용한다. 아래 규칙을 지키지 않으면 **웹 빌드만 깨지고 Windows 빌드는 통과**하는 현상이 발생한다.

### `-Wdouble-promotion` : float → double 암묵 변환 금지

`Math::vec2`, `Math::TranslationMatrix`, `Math::ScaleMatrix` 등 엔진 수학 타입은 내부적으로 `double`을 사용한다. 여기에 `float` 리터럴(`f` 접미사)이나 `float` 변수를 직접 전달하면 경고가 에러로 처리된다.

```cpp
// ❌ 금지
Math::vec2{ 0.0f, 0.0f }           // float → double 암묵 변환
Math::vec2{ slider_width, 0.0 }    // const float slider_width → double

// ✅ 올바른 패턴
Math::vec2{ 0.0, 0.0 }             // double 리터럴
Math::vec2{ static_cast<double>(slider_width), 0.0 }  // 명시적 캐스트
```

**주요 적용 지점:**
- `DrawRectangle` 4번째 인수(border_width)는 `double` → `1.5f` 대신 `1.5` 사용
- `ParticleManager::Emit()` 마지막 각도 인수는 `double` → `3.14159265f` 대신 `3.14159265` 사용
- `std::numbers::pi_v<float>` 대신 `std::numbers::pi_v<double>` 사용
- `ShakeComponent`, `Settings` 등의 로컬 `float` 변수를 `Math::vec2`에 넣을 때 반드시 `static_cast<double>()` 적용

### `-Wimplicit-int-float-conversion` : int → float 암묵 변환 금지

```cpp
// ❌ 금지 — int를 float 연산에 직접 사용
float x = vol / 100.0f;       // vol은 int
float y = std::rand() / 1.0f; // std::rand()는 int

// ✅ 올바른 패턴
float x = static_cast<float>(vol) / 100.0f;
float y = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
```

### `-Wsign-conversion` : int → size_t 암묵 변환 금지 (컨테이너 인덱싱)

`std::vector`, `std::array`의 `operator[]`는 `size_t` 인덱스를 받는다. `int` 변수로 직접 인덱싱하면 경고가 발생한다.

```cpp
// ❌ 금지
for (int i = 0; i < vec.size(); ++i) vec[i]; // int → size_t 변환
vec[popup_slot_index_];                       // int → size_t 변환

// ✅ 올바른 패턴 A: 루프 변수를 size_t로 선언
for (size_t i = 0; i < vec.size(); ++i) vec[i];

// ✅ 올바른 패턴 B: int 변수는 인덱싱 시 캐스트
vec[static_cast<size_t>(popup_slot_index_)];
```

**루프 범위 비교 주의**: `constexpr int N = 10`을 `size_t` 루프와 비교할 때는 `static_cast<size_t>(N)` 사용.

### `-Wold-style-cast` : C 스타일 캐스트 금지

```cpp
// ❌ 금지
(double)actual.x
(float)someValue

// ✅ 올바른 패턴
static_cast<double>(actual.x)
static_cast<float>(someValue)
```

### `-Wunused-private-field` : 선언만 하고 사용하지 않는 private 멤버 금지

클래스에 private 멤버를 선언했다면 반드시 사용해야 한다. 미래를 위해 남겨두는 필드는 웹 빌드를 깨뜨린다. 완전히 사용하지 않을 경우 제거할 것.

### 에셋 파일명 대소문자 일치 필수 (런타임 크래시 방지)

Windows는 파일시스템이 대소문자를 구분하지 않아 `Wall.png`와 `wall.png`를 동일하게 취급하지만, **Emscripten(Linux)은 대소문자를 구분**한다. 코드의 경로와 실제 파일명이 다르면 `assets::locate_asset`이 `std::runtime_error`를 throw하고 해당 GameState가 즉시 크래시된다.

```cpp
// ❌ 실제 파일이 wall.png인데 코드에서 Wall.png 참조 → Windows OK, 웹 크래시
Engine::GetTextureManager().Load("Assets/images/Wall.png");

// ✅ 실제 파일명 그대로 사용
Engine::GetTextureManager().Load("Assets/images/wall.png");
```

**규칙**: 새 에셋을 추가하거나 경로를 코드에 입력할 때 실제 파일명의 대소문자를 그대로 복사할 것. 관례상 `Assets/` 하위 파일은 모두 소문자로 이름 짓는 것을 권장한다 (`DigiPen.png`은 예외 — 실제 파일이 대문자).

### 웹 빌드 테스트 방법

```bash
# DragonicTactics/ 디렉토리에서:
wsl cmake --preset web-release          # configure (최초 1회 또는 CMake 변경 시)
wsl cmake --build build/web-release     # 빌드 (에러만 확인)
```

에러 출력이 대량으로 나올 경우, 에러만 추출:
```powershell
wsl cmake --build build/web-release 2>&1 | Where-Object { $_ -match 'error:' }
```

---

## 새 파일 추가 규칙

1. 헤더: `.h` 확장자 (`.hpp` 아님)
2. 모든 `.cpp` 첫 줄: `#include "pch.h"`
3. CMake가 GLOB_RECURSE로 자동 감지 → CMakeLists.txt 수동 편집 불필요
4. 새 파일 추가 후 `cmake --preset windows-debug` 재실행
5. 캐릭터 생성: `new Dragon()` 대신 `CharacterFactory::Create()` 사용
6. JSON 파싱: `External/json.hpp` (nlohmann/json) 사용 — `#include "Game/DragonicTactics/External/json.hpp"`

**새 AI 캐릭터 추가 체크리스트** (Cleric 추가 패턴 기준):
```
□ Objects/X.h + X.cpp            — Character 상속, IsAIControlled()=true, Action()→AISystem 위임
□ StateComponents/AI/XStrategy.h/.cpp — IAIStrategy 구현, Single 스펠에 range 상수 + 거리 체크 필수
□ StateComponents/AISystem.cpp   — m_strategies[CharacterTypes::X] = new XStrategy()
□ Factories/CharacterFactory.h   — (1) class X; 전방선언 추가, (2) private에 static unique_ptr<X> CreateX() 선언
□ Factories/CharacterFactory.cpp — (1) #include "X.h" 추가, (2) Create() switch에 case X: return CreateX(pos); 추가, (3) CreateX() 구현
□ Assets/Data/characters.json    — "X": { hp, speed, ap, attack_dice, spell_slots, ... }
□ Assets/Data/maps.json          — 각 맵 spawn_points에 "x": {"x":N, "y":N} 추가
□ States/GamePlay.cpp            — LoadJSONMap에 spawn_points.find("x") + enemys.push_back()
□ Engine/SoundManager.h          — SFX_X_ACTION / SFX_X_HURT 상수 추가
```

**캐릭터 클래스 계층**: `CS230::GameObject` ← `Character` ← `Dragon` / `Fighter` / `Cleric` / `Wizard` / `Rogue`

---

## 테스트

⚠️ **ConsoleTest의 모든 단위 테스트가 제거됐다.** `States/ConsoleTest.cpp`의 `DrawImGui()`는 현재 "All tests removed." 텍스트만 표시한다. `source/Game/DragonicTactics/Test/` 디렉토리도 존재하지 않는다. ConsoleTest 자체(DEVELOPER_VERSION 전용 GameState)는 남아있으나 기능이 없다.

**런타임 테스트 단축키 (GamePlay 상태 — 인게임 점검 기능, 별도 콘솔창 출력)**:

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

### 디버그 서브시스템

`DebugManager`는 `GamePlay::Load()`에서 등록되는 GS 컴포넌트로, 두 도구를 소유한다:

- `DebugConsole` — ImGui 런타임 명령 콘솔 (`RegisterCommand` 기반 명령 레지스트리 + 히스토리)
- `DebugVisualizer` — EventBus 구독 기반 그리드/AI 디시전 오버레이 패널

god mode·`timeScale`(빨리감기)·각종 오버레이 토글을 보유한다. 위치: `source/Game/DragonicTactics/Debugger/` (`DebugManager` / `DebugConsole` / `DebugVisualizer`). 콘솔 명령어 전체 목록은 아래 [문서 참조](#문서-참조)의 `docs/debug/commands.md` 참고.

**⚠️ F1 디버그 도구는 DEVELOPER_VERSION 없이도 동작**: `DebugManager::DrawImGui()`의 `#if defined(DEVELOPER_VERSION)` 컴파일 게이트가 제거됨 — 릴리즈 빌드에서도 F1으로 진입 가능. `GamePlay::DrawImGui()`에서 DEVELOPER_VERSION 블록 **앞에** 호출됨. 런타임 가드(`debug_mode`, `F1`·`Tab` 키체크)는 그대로 유지.

**God Mode 구현 상태** (`docs/Detailed Implementations/features/갓모드 — Dragon 데미지 무효 + AP 무제한.md`):
- 데미지 차단: ✅ `CombatSystem::ApplyDamage()` 내 early return으로 구현 완료
- AP 소모 차단: ❌ **미구현** — `CombatSystem.cpp` ExecuteAttack 하단(공격), `SpellSystem.cpp:542/802/837`(스펠 3곳) 수정 필요
- `DebugManager::IsGodModeEnabled()` → `debug_mode && god_mode` (Dragon에만 적용)

**디버그 콘솔 주요 명령어 (DebugManager::RegisterGameCommands)**:
- `set_hp <target> <value>` — value ≤ 0이면 `CombatSystem::ApplyDamage`로 즉사 처리 (CharacterDeathEvent 발행)
- `kill <target>` — `ApplyDamage`로 즉사 (DeathEvent 포함 전체 사망 체인)
- `killall` — 모든 AI 적 즉사 (Dragon을 attacker로 전체 사망 체인 발동)

**Debug Tools 패널 (F1 → Tab)** Cheats 섹션에 **Kill All Enemies** 버튼 — `killall` 콘솔 명령어 실행.

---

## 데이터 주도 설계

게임 밸런스 파라미터는 코드가 아닌 데이터 파일에서 로드 (`DataRegistry` / `MapDataRegistry`):

- `Assets/Data/characters.json` — 캐릭터 스탯 (`CharacterData`: hp, speed, ap, attack_dice, spell_slots 등)
- `Assets/Data/maps.json` — 자유 모드 맵 구성 (`s_level_id=0`일 때 `LoadJSONMap`이 사용)
- `Assets/Data/Level_Map.json` — 레벨 모드 맵 + 허용 스펠 + 적 목록 (`s_level_id>0`일 때 `LoadLevelMap`이 사용, 상세: [LevelSelect & Level Mode](#levelselect--level-mode))
- `Assets/Data/spell_table.csv` — 스펠 정의 (`SpellData`: Targeting + Effect 템플릿)
- `Assets/Data/status_effect.csv` — 상태 이상 설정

**맵 전환**: `GamePlay::s_next_map_id` (string, 기본값 `"first_map"`)와 `GamePlay::s_should_restart` (bool) 정적 필드를 GamePlay 전환 전에 설정해 어떤 맵을 로드할지 지정한다. `s_next_map_id`는 `maps.json`의 맵 ID 문자열이며, 없으면 첫 번째 맵으로 fallback한다.

**DataRegistry 핫리로드 API** (`StateComponents/DataRegistry.h`):

```cpp
GetGSComponent<DataRegistry>()->ReloadAll();          // 전체 JSON 재로드
GetGSComponent<DataRegistry>()->ReloadCharacters();   // characters.json만 재로드
GetGSComponent<DataRegistry>()->ReloadSpells();       // spell_table.csv만 재로드
```

⚠️ **`StateComponents/DataRegistry.ini`는 설정 파일이 아니다** — `.ini` 확장자를 사용하지만 실제로는 `DataRegistry.h`의 템플릿 메서드 구현체(`GetValue<T>`, `GetArray<T>`)가 담긴 C++ 파일이다. `DataRegistry.h`의 마지막 줄에서 `#include`된다. 새 파일 추가 시 이 확장자 관례를 따라서는 안 된다.

---

## SoundManager (엔진 서비스)

GameState 컴포넌트가 아닌 **엔진 레벨 서비스**. `Engine::GetSoundManager()`로 접근.

- **BGM**: OGG 파일 (`Assets/Audio/BGM/`) → 루프 재생
- **SFX**: WAV 파일 (`Assets/Audio/SFX/`) → 단발, 8채널 소스 풀

상수 경로가 헤더에 정의되어 있음: `SoundManager::BGM_MAIN_MENU`, `SoundManager::BGM_BATTLE`, `SFX_DRAGON_ACTION`, `SFX_DRAGON_HURT`, `SFX_DRAGON_WALK`, `SFX_FIGHTER_ACTION`, `SFX_FIGHTER_HURT`, `SFX_CLERIC_ACTION`, `SFX_CLERIC_HURT`, `SFX_ROGUE_ACTION`, `SFX_ROGUE_HURT`, `SFX_WIZARD_ACTION`, `SFX_WIZARD_HURT`, `SFX_HUMAN_WALK`

⚠️ **Rogue 에셋 파일명 철자 함정**: 코드 상수는 `SFX_ROGUE_*`("rog**ue**")이지만, 이 상수가 가리키는 디스크상 실제 파일은 "rou**ge**" 철자다 — `Assets/Audio/SFX/rouge_action.wav`, `rouge_hurt.wav` (`Engine/SoundManager.h`). 플로우차트 `architecture/character_flowchart/rouge.mmd`·`rouge.jpg`도 동일하게 "rouge". 따라서 **코드에서는 상수 `SFX_ROGUE_*`를 그대로 사용**(상수가 올바른 "rouge" 경로를 담고 있음), **에셋 파일을 새로 추가·교체할 때만 파일명을 "rouge"로** 작성할 것. (참조 문서는 `rogue_strategy.md`가 정본 — 아래 [문서 참조](#문서-참조) 참고.)

```cpp
Engine::GetSoundManager().PlayBGM(SoundManager::BGM_BATTLE);   // 루프 BGM 시작
Engine::GetSoundManager().StopBGM();
Engine::GetSoundManager().PlaySFX(SoundManager::SFX_HIT);      // 단발 SFX (앞 소스 slot round-robin)
Engine::GetSoundManager().PlaySFXLast(SoundManager::SFX_HIT);  // hurt SFX 전용 — 끝 소스 slot 역방향
Engine::GetSoundManager().PlaySFXDelayed(path, 0.15);          // 지연 재생 (초 단위)
Engine::GetSoundManager().ClearPendingDelayedSFX();            // 대기 중인 지연 SFX 전체 취소 (Unload 시 호출)
Engine::GetSoundManager().Update(dt);                          // GamePlay::Update 첫 줄에서 호출
Engine::GetSoundManager().SetBGMVolume(0.7f);                  // 0.0 ~ 1.0
Engine::GetSoundManager().SetSFXVolume(0.8f);                  // 0.0 ~ 1.0 (Settings::ApplySettings()에서 호출)
```

`GamePlay::Load()`에서 `PlayBGM`, `GamePlay::Unload()`에서 `StopBGM` + `ClearPendingDelayedSFX()` 호출 패턴을 따른다. `ClearPendingDelayedSFX()`를 생략하면 직전 게임플레이의 피격음이 재시작 시 재생된다.

**SFX 소스 풀 분리** (`Engine/SoundManager.cpp`):

OpenAL 소스 풀 8개를 공격 SFX와 피격 SFX가 항상 다른 슬롯을 사용하도록 분리:

| 메서드 | 탐색 방향 | 용도 |
|---|---|---|
| `PlaySFX` | 앞(0→7) round-robin | 공격·스펠·이동 SFX |
| `PlaySFXLast` | 끝(7→0) 역방향 | 피격(hurt) SFX 전용 |

같은 프레임에 두 소리가 동시 재생될 때 서로 다른 소스를 사용 → 한 소리가 다른 소리를 덮어쓰지 않음.

**SFX 재생 순서** (`States/GamePlay.cpp` EventBus 구독, `StateComponents/CombatSystem.cpp`):

| 순서 | 이벤트 | SFX | 메서드 |
|---|---|---|---|
| 1 | `CharacterAttackedEvent` | 공격자 action SFX (`SfxActionFor`) | `PlaySFX` |
| 2 | `CharacterDamagedEvent` | 피격자 hurt SFX (`SfxHurtFor`) | `PlaySFXDelayed(sfx, delay)` |

`CombatSystem::ExecuteAttack`에서 `CharacterAttackedEvent`를 `ApplyDamage` **전에** 발행해 순서 보장.

### 이펙트 타이밍 시스템 (`States/GamePlay.cpp`)

피격 이펙트(데미지 텍스트·피격음·셰이크·파티클)가 공격 SFX와 동기화되도록 딜레이를 계산한다.

**핵심 상수** (파일 상단 file-scope):
```cpp
static constexpr double SPELL_DELAY_OBJECT_SEC = 1.5; // 스펠 데미지 지연 — 조정 가능
static constexpr double EFFECT_LEAD_TIME        = 0.3; // SFX 종료 이 시간 전에 이펙트 등장
```

**`m_pending_damage_delay_`** (GamePlay 멤버) — `CharacterAttackedEvent` / `SpellCastEvent` 핸들러에서 계산:
```
플레이어 공격: max(0, SFX길이 - EFFECT_LEAD_TIME)
AI 공격:       0.0  (AttackDelayObject가 0.3초 뒤 CharacterDamagedEvent를 발행하므로 추가 딜레이 불필요)
스펠:          max(0, SFX길이 - SPELL_DELAY_OBJECT_SEC - EFFECT_LEAD_TIME)
용암 피해:     0.0  (attacker == nullptr)
```

**AI 공격 딜레이 오브젝트** (`StateComponents/CombatSystem.cpp`):
- `AttackDelayObject` — `SpellDelayObject`와 동일한 패턴. `ExecuteAttack` 내에서 AI 공격 시 `ApplyDamage` 즉시 호출 대신 `AI_ATTACK_DELAY(=0.3)초` 뒤에 실행.
- `BattleOrchestrator`의 Attack wait_timer = 0.6s (AttackDelayObject 0.3s + 이펙트 확인 여유 0.3s).

**`PendingHitEffect` 큐** (GamePlay):
```cpp
struct PendingHitEffect { Character* target; Math::vec2 world_pos; double timer; };
std::vector<PendingHitEffect> m_pending_hit_effects_;
```
`CharacterDamagedEvent` 핸들러에서 큐에 추가 → `GamePlay::Update()` 맨 앞에서 tick → timer ≤ 0이면 `StartShake` + `ParticleManager::Emit`. 사망한 target은 `m_confirmed_dead_` 체크로 skip.

**`SoundManager::GetSFXDuration(path)`** — OpenAL 버퍼 메타데이터(`AL_SIZE / (채널 × 비트 × freq)`)로 WAV 재생 시간(초) 반환. 파일 교체 시 자동 반영.

⚠️ **Settings SFX 볼륨 반영**: `Settings::ApplySettings()`가 BGM 볼륨과 함께 `SetSFXVolume()`도 호출한다. SFX 볼륨·뮤트 변경 시 반드시 `ApplySettings()`를 통할 것 — SoundManager를 직접 호출하면 Settings UI와 동기화가 깨진다.

**스펠 전용 SFX** (`GamePlay.cpp` 파일 내 `GetSpellSFX()` 함수):

`SpellCastEvent` 구독에서 `GetSpellSFX(spellName)`을 먼저 조회해 스펠별 WAV를 재생한다. 경로가 비어 있으면 캐릭터 action SFX로 fallback.

| 스펠명 | 파일 |
|---|---|
| Divine Shield, Healing Touch, Teleport, Mana Conversion, Purify | `Assets/Audio/SFX/spell/Arcane.wav` |
| Curse of Suffering | `Assets/Audio/SFX/spell/Curse.wav` |
| Fire Bolt, Dragon's Fury | `Assets/Audio/SFX/spell/Fire.wav` |
| Gale Step, Shadow Hide | `Assets/Audio/SFX/spell/Gale Step.wav` |
| Magma Blast | `Assets/Audio/SFX/spell/Lava Creation.wav` |
| Magic Missile | `Assets/Audio/SFX/spell/Magic Missile.wav` |
| Meteor | `Assets/Audio/SFX/spell/Meteor.wav` |
| Smite, Tail Swipe | `Assets/Audio/SFX/spell/Smite.wav` |
| Wall Creation | `Assets/Audio/SFX/spell/Wall Creation.wav` |
| Weakpoint Strike | `Assets/Audio/SFX/spell/Weakpoint Strike.wav` |

새 스펠을 추가할 때 대응 WAV가 있다면 `GetSpellSFX()` switch에 케이스 추가. 파일은 `GamePlay::Load()`에서 `LoadSFX(path)`로 미리 로드해야 런타임 끊김이 없다.

⚠️ `GamePlay.cpp`의 `SpellCastEvent` 구독 내부에 `std::cout` 디버그 출력이 남아 있다 — 릴리즈 전 제거 필요 (라인 338–340 근방).

---

## 렌더링 패턴

### 2-패스 렌더링 (`States/GamePlay.cpp::Draw()`)

```cpp
auto win          = Engine::GetWindow().GetSize();
auto* renderer_2d = CS230::TextureManager::GetRenderer2D();

// Pass 1: 월드 공간 — TacticalCamera 적용 (줌·패닝)
Math::TransformationMatrix world_ndc = m_camera.GetWorldMatrix(win);
renderer_2d->BeginScene(world_ndc);
GetGSComponent<GridSystem>()->Draw();
GetGSComponent<GameObjectManager>()->DrawAll(world_ndc);
renderer_2d->EndScene();

// Pass 2: UI 공간 — 가상 1600×900 좌표, 레터박스 적용
Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
Engine::GetTextureManager().SaveCurrentScene(ui_ndc);  // 폰트 캐시 미스 복원용
renderer_2d->BeginScene(ui_ndc);
m_ui_manager->Draw(ui_ndc);
renderer_2d->EndScene();
```

**TacticalCamera** (`States/GamePlay.h`):

```cpp
struct TacticalCamera {
    Math::vec2 target = { 0.0, 0.0 };
    double zoom = 1.0;
    static constexpr double ZOOM_MIN = 0.25;
    static constexpr double ZOOM_MAX = 3.0;
    static constexpr int    VIRTUAL_W = 1600;
    static constexpr int    VIRTUAL_H = 900;
    Math::TransformationMatrix GetWorldMatrix(Math::ivec2 win) const;
    Math::vec2 ScreenToWorld(Math::vec2 screen, Math::ivec2 win) const;
    Math::vec2 WorldToScreen(Math::vec2 world, Math::ivec2 win) const;
    static Math::TransformationMatrix BuildVirtualNdc(Math::ivec2 win);  // 레터박스 UI NDC
    static Math::vec2 ScreenToVirtual(Math::vec2 screen, Math::ivec2 win); // 실제 픽셀 → 가상 1600×900 (UI 히트 판정용)
};
```

**`SaveCurrentScene` 패턴** (`Engine/TextureManager.h/cpp`) — 폰트 캐시 미스 버그 대응:
- 문제: `Font::PrintToTexture()` 캐시 미스 시 `EndRenderTextureMode()`가 잘못된 NDC 행렬로 씬을 복원 → UI 깜빡임
- 해결: UI Pass 시작 직전 `SaveCurrentScene(ui_ndc)` 호출 → `EndRenderTextureMode` 내부에서 저장된 행렬로 정확히 복원

```cpp
// TextureManager.h - RenderInfo 구조체에 추가
Math::TransformationMatrix SavedCameraMatrix{};
static void SaveCurrentScene(const Math::TransformationMatrix& m);

// TextureManager.cpp - EndRenderTextureMode 수정
renderer_2d->BeginScene(render_info.SavedCameraMatrix);  // 저장된 ui_ndc 복원
```

⚠️ **CS200 소스 파일은 수정하지 않는다** — 렌더러 버그 대응은 `Engine/TextureManager.h/cpp`에서만 처리.

### 타일 텍스처 (`StateComponents/GridSystem.cpp`)

타일 텍스처는 GridSystem 생성자에서 `Engine::GetTextureManager().Load()`로 로드, `Draw()`에서 stone_tile 패턴과 동일하게 사용:

```cpp
// GridSystem.h private:
std::shared_ptr<CS230::Texture> stone_tile_bright;
std::shared_ptr<CS230::Texture> stone_tile_dark;
std::shared_ptr<CS230::Texture> lava_tile;   // Assets/images/lava.png
std::shared_ptr<CS230::Texture> wall_tile;   // Assets/images/Wall.png

// Draw() 패턴 — tile_scale = TILE_SIZE / tex->GetSize().x
tex->Draw(Math::TranslationMatrix(Math::ivec2{screen_x - TILE_SIZE, screen_y - TILE_SIZE})
          * Math::ScaleMatrix(tile_scale), 0xFFFFFFFF, DrawDepth::TILE);
```

⚠️ 모든 타일(Wall, Lava, Difficult, Empty)은 `DrawDepth::TILE`을 명시적으로 전달해야 한다. `DrawRectangle`의 기본 depth는 `DrawDepth::CHARACTER`(0.5f)로 캐릭터와 겹친다.

**DrawDepth 전체 값** (`Engine/DrawDepth.h`, 값이 작을수록 앞에 렌더링):

| 상수 | 값 | 용도 |
|---|---|---|
| `DrawDepth::UI` | 0.01f | UI 텍스트·아이콘 |
| `DrawDepth::PARTICLE` | 0.3f | 파티클 효과 |
| `DrawDepth::CHARACTER` | 0.5f | 캐릭터·기본 오브젝트 |
| `DrawDepth::PATH` | 0.7f | 경로 하이라이트 |
| `DrawDepth::OVERLAY` | 0.8f | 이동/스펠 범위 오버레이 |
| `DrawDepth::TILE` | 0.9f | 그리드 배경 타일 |

**UI 레이어 내부 depth 계층** (상태이상 패널 ↔ 툴팁 겹침 방지):

| 레이어 | depth 식 | 값 |
|---|---|---|
| 툴팁 텍스트 | `DrawDepth::UI` | 0.01f ← 가장 앞 |
| 툴팁 배경 | `DrawDepth::UI + 0.001f` | 0.011f |
| 상태이상 아이콘 | `DrawDepth::UI + 0.01f` | 0.02f |
| 포트레이트 | `DrawDepth::UI + 0.015f` | 0.025f |
| 패널 배경 | `DrawDepth::UI + 0.02f` | 0.03f ← 가장 뒤 |

⚠️ 슬롯바 아이콘은 `DrawDepth::UI - 0.005f = 0.005f` — 툴팁보다 앞이지만 슬롯바 위에는 툴팁이 표시되지 않으므로 충돌 없음. 상태이상 패널 아이콘을 이 값으로 설정하면 툴팁을 가리므로 반드시 `UI + 0.01f` 이상 사용.

### 배틀 로그 (`States/GamePlayUIManager`)

`TurnEntry` 구조체(`round_number`, `turn_number`, `actor_name`, `is_player`, `lines`)를 `std::deque<TurnEntry> turn_history_`로 관리 (최대 `MAX_LOG_ROUNDS = 5` 라운드 보관). 로그는 최신이 아래로 추가되고(`push_back`), 새 턴 시작 시 자동 하단 스크롤 (단, 마우스가 패널 위에 있고 최하단이 아니면 스크롤 유지). 라운드 헤더(`─── Round N ───`)와 사이드 헤더(`▷ Player Turn` / `▷ Enemy Turn`)가 라운드/진영 전환 시 삽입된다.

- **UINoticeEvent 토스트**: `bus->Publish(UINoticeEvent{ "message" })` 로 발행하면 `GamePlayUIManager::ShowNotice()`가 화면 상단 중앙에 1.5초 표시. 행동 차단 이유(은신·사거리 초과 등)를 플레이어에게 알릴 때 사용. 폰트가 ASCII만 지원하므로 em dash(—) 등 특수문자 사용 금지.
- **이벤트 타이밍 제약** (`TurnManager.cpp`): `PublishTurnStartEvent()`를 반드시 용암 피해 `ApplyDamage` **전에** 호출해야 함 — 배틀 로그가 `TurnStartedEvent`를 받아 새 턴 섹션을 열기 때문. 순서가 바뀌면 피해 항목이 이전 캐릭터의 섹션에 들어간다.
- **패널 레이아웃 상수**: `GamePlayUIManager.h`의 `LOG_PANEL_X/Y/W/H`, `LOG_TITLE_H`, `LOG_LINE_H`, `LOG_INDENT`, `LOG_SB_W/X`에 집중 관리됨 — 패널 위치·크기 변경 시 이 상수들만 수정.
- **스크롤**: 패널 위 마우스 휠(`GetMouseScroll()`) + 스크롤바 드래그 + **좌클릭 드래그** (`log_drag_active_`, `log_drag_prev_y_`). 패널 위에서는 카메라 줌 차단 (`IsMouseOverLogPanel()`).
- **헤더 색상**: Player 턴 = 하늘색 `0x88ccffff`, Enemy 턴 = 주황색 `0xff8844ff`.

### 슬롯 바 아이콘 (`States/GamePlayUIManager`)

슬롯 아이콘은 `slot_icons_[]`(크기 11)에 로드되고 `DrawSlotBar()`에서 오버레이된다. 인덱스 0-9는 스펠 슬롯, 인덱스 10은 End Turn 버튼:

```cpp
// InitButtons() — 아이콘 로드
slot_icons_.resize(11, nullptr);
for (int i = 0; i < 10; ++i)
    slot_icons_[i] = Engine::GetTextureManager().Load(ICON_PATHS[i]);
slot_icons_[10] = Engine::GetTextureManager().Load("Assets/images/turn_end.png");

// DrawSlotBar() — 아이콘 렌더링 (버튼 배경 위에 오버레이)
for (int i = 0; i < static_cast<int>(slot_icons_.size()); ++i)
    slot_icons_[i]->Draw(
        Math::TranslationMatrix(Math::vec2{ slot_bar_x_[i], slot_bar_center_y_ - 32.0 }),
        0xFFFFFFFF, DrawDepth::UI - 0.005f);
```

ButtonManager는 배경 사각형(`DrawRectangle`)만 담당하고, 아이콘은 항상 `DrawSlotBar()`가 그린다. `Button::image_path` 필드는 ButtonManager에 존재하지만 현재 슬롯 버튼에는 사용하지 않는다.

### 상태이상 패널 (`States/GamePlayUIManager`)

화면 좌측에 캐릭터 포트레이트 + 활성 상태이상 아이콘을 고정 표시하는 **Pass 2 UI 패널**. 카메라 줌·패닝 무관, 슬롯바와 동일한 렌더링 경로.

**레이아웃 (virtual 1600×900, y=0 = 하단)**

| 요소 | 위치 | 크기 |
|---|---|---|
| 포트레이트 | x=8, y=수직 중앙 기준 각 row | 48×48 (128px native → scale 0.375) |
| 상태이상 아이콘 | x=60+N×32, y=row_bot+8 | 32×32 |
| 패널 배경 | x=8 기준, 폭=300 | 높이 = rows×48 + (rows-1)×4 + 8 |

- 패널 수직 중심: `pan_cy = VH * 0.5 = 450`
- 첫 row_bot: `pan_cy + pan_h/2 - 4 - 48`
- Row 간격: 52 (48+4)

**`InitStatusEffectIcons()`에서 로드** (`GamePlay::Load()`에서 `InitSpellTooltips()` 직후 호출):
- 9종 아이콘 PNG (32×32): `Assets/images/{blessing,lifesteal,...}.png` → `status_icon_textures_`
- 포트레이트 PNG (128×128): `Assets/images/{dragon,fighter,cleric_p,rogue_p,wizard_p}.png` → `portrait_textures_` (key = `static_cast<int>(CharacterTypes::X)`)
- 툴팁 폭 사전 계산 → `effect_tooltip_widths_` (스펠 툴팁의 `spell_tooltip_widths_`와 동일 패턴)

**`DrawWorld()`는 현재 비어 있다** — 상태이상 아이콘이 Pass 1 월드 공간에서 `DrawStatusEffectPanel()`(Pass 2)로 이전됨. `GamePlay::Draw()`에서 `DrawWorld()` 호출은 그대로 남아 있음(내부만 빔).

**툴팁 동적 폭 계산 패턴** (`InitStatusEffectIcons()` + `InitSpellTooltips()` 공통):
```cpp
// 로드 타임에 CalculateTextSize로 최대 폭 계산 → 맵에 저장
double w = textMgr.CalculateTextSize(line, Fonts::Kings).x * scale;
widths_map[id] = std::min(max_w + PAD * 2.0, static_cast<double>(VW) - 20.0);

// 렌더 타임에 조회
auto wit = widths_map.find(hovered_id);
double TT_W = (wit != widths_map.end()) ? wit->second : 340.0;
```

**호버 감지**: Update()의 블록 3c에서 `virt_mouse`(가상 좌표)를 직접 비교. `DrawStatusEffectPanel()`의 `pan_cy`/`row_bot` 계산식과 **정확히 동일한 공식**을 사용해야 픽셀 정확도 보장.

**⚠️ `m_characters` 댕글링 포인터 방지**: `GamePlayUIManager::SetCharacters()`는 `CharacterDeathEvent`를 구독해서 사망 시 해당 슬롯을 `nullptr`로 교체한다. `DrawStatusEffectPanel()`·Update 블록 3c·`DrawCharacterStatsPanel()` 세 곳의 루프는 모두 `!ch` / `ch == nullptr` 가드를 가지므로 안전하게 skip한다. 캐릭터 사망 후 `GetActiveEffects()` 등 컴포넌트 접근 전에 이 가드가 반드시 있어야 access violation이 발생하지 않는다.

### 폰트 (`Engine/TextManager`, `Engine/Fonts.h`)

| 상수 | 파일 | 사용처 |
|---|---|---|
| `Fonts::Simple` (0) | Font_Simple.png | 미사용 |
| `Fonts::Outlined` (1) | Font_Outlined.png | 미사용 |
| `Fonts::Kings` (2) | Font_Kings.png | MainMenu + **게임플레이 전체 UI** |

**⚠️ `Fonts::Kings`가 유일한 프로젝트 표준** — GamePlayUIManager, ButtonManager 모두 Kings 사용. `Fonts::Outlined`은 레거시 코드에서 이미 교체됨. 새 UI 텍스트는 반드시 `Fonts::Kings` 사용.

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
- **게임 상태 파일**: `source/Game/DragonicTactics/States/` (GamePlay, BattleOrchestrator, PlayerInputHandler, GamePlayUIManager, ButtonManager)
- **캐릭터 엔티티**: `source/Game/DragonicTactics/Objects/`
- **캐릭터 팩토리**: `source/Game/DragonicTactics/Factories/` (CharacterFactory)
- **Actions**: `Objects/Actions/` (Action.h, ActionAttack.h)
- **타입 정의**: `source/Game/DragonicTactics/Types/` (CharacterTypes.h, GameTypes.h, Events.h)
- **SpellData 구조체**: `StateComponents/SpellSystem.h` (SpellData, SpellTargeting, SpellMove)
- **ActiveEffect 구조체**: `Objects/Components/StatusEffectComponent.h`
- **경로 탐색**: `StateComponents/AStar.cpp` (GridSystem::FindPath에서 내부 사용)
- **디버그 서브시스템**: `source/Game/DragonicTactics/Debugger/` (DebugConsole, DebugManager, DebugVisualizer)
- **JSON 데이터**: `DragonicTactics/Assets/Data/`
- **AI 플로우차트**: `architecture/character_flowchart/` (Mermaid .mmd — fighter.mmd, cleric.mmd, rouge.mmd, wizard.mmd 존재 / rouge.jpg, wizard.jpg 이미지도 있음)

엔진 접근: `Engine::GetLogger()`, `Engine::GetInput()`, `Engine::GetWindow()`, `Engine::GetGameStateManager()`

렌더러 접근: `CS230::TextureManager::GetRenderer2D()` (Immediate/Batch/Instanced 런타임 전환 가능)

---

## 문서 참조

- [docs/implementation-plan.md](docs/implementation-plan.md) — 26주 구현 계획 (우선순위 기반)
- [docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/) — 주차별 상세 가이드 (한글)
- [docs/Detailed Implementations/features/spell_system.md](docs/Detailed%20Implementations/features/spell_system.md) — SpellSystem 전체 구현 가이드 (Targeting 템플릿, Effect 파싱, 업캐스팅, Move 템플릿)
- [docs/Detailed Implementations/features/status_effect_system.md](docs/Detailed%20Implementations/features/status_effect_system.md) — StatusEffect 두 레이어 구현 가이드
- [docs/Detailed Implementations/features/terrain_spells.md](docs/Detailed%20Implementations/features/terrain_spells.md) — 지형 변환 스펠(Wall/Lava 배치) 구현 가이드
- [docs/Detailed Implementations/features/map_loading.md](docs/Detailed%20Implementations/features/map_loading.md) — maps.json 로딩 및 맵 전환 구현 가이드
- [docs/Detailed Implementations/features/button_manager.md](docs/Detailed%20Implementations/features/button_manager.md) — ButtonManager(슬롯 바 버튼 패널) 구현 가이드
- [docs/Detailed Implementations/features/UI 개선 구현점.md](docs/Detailed%20Implementations/features/UI%20개선%20구현점.md) — UI 리팩토링 실제 변경점 (bb7e32fa 커밋 대비, TacticalCamera/2-패스/슬롯바/배틀로그 플리커 수정 포함)
- [docs/Detailed Implementations/features/character_death_crash_fix.md](docs/Detailed%20Implementations/features/character_death_crash_fix.md) — 캐릭터 사망 시 비결정적 크래시 수정 (use-after-free, m_confirmed_dead_ 패턴, RemoveFromTurnOrder)
- [docs/Detailed Implementations/features/crash_risks.md](docs/Detailed%20Implementations/features/crash_risks.md) — 알려진 크래시 위험 지점 9개 정리 (🔴`CombatSystem.cpp:29` null 역참조 CRITICAL 포함)
- [docs/Detailed Implementations/features/fighter_spells_purify_check.md](docs/Detailed%20Implementations/features/fighter_spells_purify_check.md) — 알려진 CSV/전략 버그 3개: Smite `Lifesteal for 0 turns` → `Basic`, Purify `Any:Around:4` → `Self:Single:0`, FighterStrategy `HasBuff("Bloodlust")` → `HasBuff("Lifesteal")`
- [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md) — 아키텍처 원칙
- [architecture/Implementation_Checklist.md](architecture/Implementation_Checklist.md) — 진행 체크리스트
- [docs/Detailed Implementations/features/fighter_strategy.md](docs/Detailed%20Implementations/features/fighter_strategy.md) — FighterStrategy 구현 상세 가이드
- [docs/Detailed Implementations/features/rogue_strategy.md](docs/Detailed%20Implementations/features/rogue_strategy.md) — RogueStrategy 구현 가이드 (참조용)
- [docs/Detailed Implementations/weeks/cleric_implementation.md](docs/Detailed%20Implementations/weeks/cleric_implementation.md) — Cleric 구현 가이드
- [docs/debug/commands.md](docs/debug/commands.md) — 디버그 콘솔 명령어 목록
- [docs/systems/](docs/systems/) — 시스템별 상세 문서 (EventBus, Characters, Components 등)
- [DragonicTactics/README.md](DragonicTactics/README.md) — 게임 룰·조작·스킬 (영문/한글, 빌드 설명 없음 — 빌드는 위 [빌드](#빌드) 섹션)
- [docs/Detailed Implementations/features/sound_manager.md](docs/Detailed%20Implementations/features/sound_manager.md) — SoundManager 구현 상세
- [docs/Detailed Implementations/features/spell_log_ui.md](docs/Detailed%20Implementations/features/spell_log_ui.md) — 스펠/배틀 로그 UI 구현
- [docs/Detailed Implementations/features/imgui_debug_only.md](docs/Detailed%20Implementations/features/imgui_debug_only.md) — ImGui 패널 DEVELOPER_VERSION 게이팅
- [docs/Detailed Implementations/features/depth.md](docs/Detailed%20Implementations/features/depth.md) — DrawDepth 렌더 순서 가이드
- [docs/debug/](docs/debug/) — 디버그 가이드 모음 (commands.md 외 tools.md, ui.md, ARCHITECTURE_COVERAGE_ANALYSIS.md, SEPARATE_CONSOLE_WINDOW_GUIDE.md)
- [DragonicTactics/docs/DevEnvironment.md](DragonicTactics/docs/DevEnvironment.md) / [DebuggingWeb.md](DragonicTactics/docs/DebuggingWeb.md) — 개발 환경 셋업·웹 빌드 디버깅
- [docs/Detailed Implementations/features/복원 기능.md](docs/Detailed%20Implementations/features/%EB%B3%B5%EC%9B%90%20%EA%B8%B0%EB%8A%A5.md) — 창 크기·BGM 설정 user_settings.json 영구 저장 구현 (Window ResizeCallback, Settings::LoadUserSettings/SaveUserSettings)
- [docs/Detailed Implementations/features/level_game.md](docs/Detailed%20Implementations/features/level_game.md) — Level Game 모드 설계 (레벨별 적 구성·허용 스펠·맵 크기 상세)
- ⚠️ `docs/Detailed Implementations/features/`에 `rogue_strategy.md`와 `rouge_strategy.md`가 모두 존재 — **`rogue_strategy.md`가 정본**(위에서 링크 중), `rouge_strategy.md`는 구 철자 잔재이므로 참조 금지
