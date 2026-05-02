# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 코드 탐색 출발점

**`DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp`가 이 프로젝트의 중심 파일이다.**
낯선 코드를 이해하려면 이 파일에서 시작해 연결을 따라가는 것이 가장 빠르다.

| 함수                      | 역할                                                                   |
| ----------------------- | -------------------------------------------------------------------- |
| `GamePlay::Load()`      | 모든 GS 컴포넌트 등록 + CSV/JSON 데이터 로드 + 캐릭터 스폰                             |
| `GamePlay::Update()`    | `PlayerInputHandler` / `BattleOrchestrator` / `GameObjectManager` 구동 |
| `GamePlay::DrawImGui()` | Map Selection / Combat Status 패널 (Move·Action·Spell UI는 ButtonManager가 담당) |
| `GamePlay::Draw()`      | `GridSystem::Draw()` + `GameObjectManager::DrawAll()`                |

**현재 맵 구성**: Dragon = 플레이어, Fighter + Cleric = AI 적 (`maps.json` spawn_points 기준, `enemys` 벡터로 관리).

---

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG — 커스텀 C++20 OpenGL 엔진 (CMake), 5명 팀, 26주 개발.

- **폐기됨**: `Abilities/` 디렉토리 전체 (AbilityBase, MeleeAttack, ShieldBash) — `Objects/Actions/ActionAttack`만 사용
- **구현 완료**: SpellSystem (CSV 파싱 + 시전 + UI), StatusEffectHandler (9가지 효과 + OnApplied/OnRemoved), TurnManager, GridSystem, SoundManager (BGM/SFX)
- **구현 완료**: FighterStrategy (`fighter.mmd` 플로우차트 완전 반영 — 킬루프/생존/일반교전/원거리 분기)
- **구현 완료**: ClericStrategy (`cleric.mmd` 플로우차트 완전 반영 — 킬루프/힐/버프·디버프/근접 분기) + Cleric 캐릭터 클래스
- **미구현**: WizardStrategy, RogueStrategy — 캐릭터 클래스 및 characters.json 스탯도 없음 (`FighterStrategy`/`ClericStrategy`가 참조 구현)

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

모든 설정 일괄 빌드 (필터 옵션 있음) — 스크립트는 `DragonicTactics/scripts/` 아래:
```bash
python3 scripts/scan_build_project.py          # DragonicTactics/ 에서 실행
python3 scripts/scan_build_project.py --help
```

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

- `DiceManager` — `Roll("2d6")`, `Roll("1d20+5")` 형식으로 주사위 굴림
- `CombatSystem` — 공격/방어 주사위 굴림 + 최종 데미지 계산 (StatusEffectHandler 훅 연동)
- `util::Timer` — 엔진 제공 타이머 (`Engine/Timer.h`). `GetElapsedSeconds()` / `Reset()`으로 AI 재호출 간격 측정에 사용

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

`AIDecision` 구조체: `type`(Move/Attack/UseAbility/EndTurn/None), `target`, `destination`, `abilityName`, `reasoning`, `lava_penalty`(이동 시 용암 타일 회피 가중치, 0 = 무시)

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

**Cleric 스탯** (`Assets/Data/characters.json`): HP 90, Speed 2, AP 1, 1d6 공격, 슬롯 Lv1×3 / Lv2×2

**⚠️ Cleric 스펠은 자신에게 사용 불가** — Divine Shield 포함 모든 스펠이 타 아군/적 대상. `FindAllyNeedingBuff()`는 actor 제외, 파이터>로그>위자드 순으로만 반환.

**⚠️ Cleric 무한루프 방지 (기구현됨)**: Curse/Divine Shield(Single geometry) → CanCast 실패 시 AP 미소모 → 무한루프 위험. `MakeSupportDecision`에서 `dist <= SPELL_RANGE` 직접 체크 후 범위 밖이면 `MakeMeleePhaseDecision`(이동)으로 fall-through.

**FighterStrategy 미구현 분기** (`FighterStrategy.cpp` 상단 주석 블록):
- **보물 탈출**: `actor->HasTreasure()` → Exit 타일로 이동 (`grid->HasExit()`, `grid->GetExitPosition()` 필요) — 보물 시스템 구현 후 활성화 (주석 처리 중)
- **클레릭 추적**: 미구현 — fighter.mmd에 해당 분기 없음. Fighter는 위험 시 Survival 시퀀스(Bloodlust→FearCry→공격)만 실행. 클레릭이 Fighter에게 이동하는 방식이 맞음.

**⚠️ UseAbility AIDecision 작성 시 주의**:
- `AISystem::ExecuteDecision`은 `decision.target->GetGridPosition()->Get()`을 target_tile로 `CastSpell`에 전달 (`destination` 필드 무시됨)
- `Around` geometry 스펠(Fearful Cry): caster 중심 AoE → `CanCast` 범위 체크가 `caster→target_tile` 거리로 계산 → **`target = actor(자신)`** 으로 설정해야 거리=0으로 항상 통과
- `Single` geometry 스펠: CanCast 실패(사거리 초과) 시 AP 미소모 → 다음 프레임에 동일 결정 반복 → **무한루프** — **Strategy에서 반드시 `distance <= SPELL_RANGE`를 직접 체크**해야 함. 범위 밖이면 UseAbility 반환 금지, `MakeMeleePhaseDecision`(이동)으로 fall-through.

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
```

이동 목적지 유효성 검사 패턴 (`AISystem::ExecuteDecision` 참고):
```cpp
TileType dt = grid->GetTileType(destination);
bool dest_ok = (dt == TileType::Empty || dt == TileType::Lava) && !grid->IsOccupied(destination);
```

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
  1. MovementComponent::IsMoving() → true면 즉시 return (이동 애니메이션 완료 대기)
  2. timer 0.6s 바쁜 대기 (SpellDelayObject 0.5s 보장 + 여유)
  3. AISystem::MakeDecision() → AIDecision 획득
  4. EndTurn → TurnManager::EndCurrentTurn()
     그 외  → AISystem::ExecuteDecision() 후 return
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

**PlayerInputHandler.ActionState** (입력 상태 머신):

```
None → SelectingMove → Moving
     → SelectingAction → TargetingForAttack
                       → SelectingSpell → TargetingForSpell
                                        → WallPlacementMulti
                                        → LavaPlacementMulti
```

Dragon(플레이어) 턴에서만 동작. AI(Fighter) 턴은 `BattleOrchestrator`가 처리.

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

`SpellSystem::CastSpell`은 효과를 **0.5초 딜레이** 후 적용(`SpellDelayObject`). AI 재호출 간격이 이보다 짧으면 상태 반영 전에 MakeDecision이 재호출되어 같은 스펠을 중복 시전한다.

```cpp
// BattleOrchestrator::HandleAITurn 실제 구현 — 메인 스레드를 0.6s 블로킹
timer->ResetTimeStamp();
while (timer->GetElapsedSeconds() < 0.6) {}  // busy-wait (0.5s SpellDelay 보장)
```

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
Applies "{STATUS}" status for {N} turns [to self].
Move to {mover:move_type:distance}.
Summons {entity} at {location}.
```

Move 값: `self:stay:0` (이동 없음) | `target:knockback:N` (밀쳐냄) | `self:teleport:selected` (순간이동)

**`to self` 접미사** — Applies 줄에 `to self`를 붙이면 시전자(caster) 자신에게 효과 적용. 없으면 targets에 적용 (기본):

```csv
Applies "Exhaustion" status for 1 turn to self.   ← Dragon이 Meteor 시전 후 자신이 탈진
Applies "Blessing" status for 2 turns.             ← 피격 대상들에게 적용 (기존 방식)
```

`SpellData` 파싱 결과: `caster_effect_status` / `caster_effect_duration` 필드 (`SpellSystem.h`).

**시전 흐름**:

```
CastSpell → CanCast(클래스/슬롯/Geometry/Range/AP 체크) → ConsumeSpell → AP.Consume(1) → ApplySpellEffect
  ApplySpellEffect → targets 결정(Geometry) → 피해 → 상태효과(targets) → 시전자 자신 효과 → ApplyMoveEffect → ApplySpecialEffect
```

**넉백 시 용암 허용**: `ApplyMoveEffect`의 knockback 루프는 `TileType::Lava`를 유효 착지 타일로 허용. 벽(Wall)은 여전히 차단.

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
6. JSON 파싱: `External/json.hpp` (nlohmann/json) 사용 — `#include "Game/DragonicTactics/External/json.hpp"`

**새 AI 캐릭터 추가 체크리스트** (Cleric 추가 패턴 기준):
```
□ Objects/X.h + X.cpp            — Character 상속, IsAIControlled()=true, Action()→AISystem 위임
□ StateComponents/AI/XStrategy.h/.cpp — IAIStrategy 구현, Single 스펠에 range 상수 + 거리 체크 필수
□ StateComponents/AISystem.cpp   — m_strategies[CharacterTypes::X] = new XStrategy()
□ Factories/CharacterFactory.h   — class X; + CreateX() 선언
□ Factories/CharacterFactory.cpp — #include X.h + Create() switch case + CreateX() 구현
□ Assets/Data/characters.json    — "X": { hp, speed, ap, attack_dice, spell_slots, ... }
□ Assets/Data/maps.json          — 각 맵 spawn_points에 "x": {"x":N, "y":N} 추가
□ States/GamePlay.cpp            — LoadJSONMap에 spawn_points.find("x") + enemys.push_back()
```

**캐릭터 클래스 계층**: `CS230::GameObject` ← `Character` ← `Dragon` / `Fighter` / `Cleric` / `Wizard` / `Rogue`

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

상수 경로가 헤더에 정의되어 있음: `SoundManager::BGM_MAIN_MENU`, `SoundManager::BGM_BATTLE`, `SFX_DRAGON_ACTION`, `SFX_DRAGON_HURT`, `SFX_FIGHTER_ACTION`, `SFX_FIGHTER_HURT`, `SFX_CLERIC_ACTION`, `SFX_CLERIC_HURT`, `SFX_HUMAN_WALK`

```cpp
Engine::GetSoundManager().PlayBGM(SoundManager::BGM_BATTLE);   // 루프 BGM 시작
Engine::GetSoundManager().StopBGM();
Engine::GetSoundManager().PlaySFX(SoundManager::SFX_HIT);      // 단발 SFX
Engine::GetSoundManager().SetBGMVolume(0.7f);                  // 0.0 ~ 1.0
```

`GamePlay::Load()`에서 `PlayBGM`, `GamePlay::Unload()`에서 `StopBGM` 호출 패턴을 따른다.

**SFX 재생 순서** (`States/GamePlay.cpp` EventBus 구독, `StateComponents/CombatSystem.cpp`):

| 순서 | 이벤트 | SFX |
|---|---|---|
| 1 | `CharacterAttackedEvent` | 공격자 action SFX (`SfxActionFor`) |
| 2 | `CharacterDamagedEvent` | 피격자 hurt SFX (`SfxHurtFor`) |

`CombatSystem::ExecuteAttack`에서 `CharacterAttackedEvent`를 `ApplyDamage` **전에** 발행해 순서 보장 (구현 완료). 이 순서를 바꾸면 hurt SFX가 먼저 재생되므로 변경 금지.

---

## 렌더링 패턴

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
- **게임 상태 파일**: `source/Game/DragonicTactics/States/` (GamePlay, BattleOrchestrator, PlayerInputHandler, GamePlayUIManager)
- **캐릭터 엔티티**: `source/Game/DragonicTactics/Objects/`
- **캐릭터 팩토리**: `source/Game/DragonicTactics/Factories/` (CharacterFactory)
- **Actions**: `Objects/Actions/` (Action.h, ActionAttack.h)
- **타입 정의**: `source/Game/DragonicTactics/Types/` (CharacterTypes.h, GameTypes.h, Events.h)
- **SpellData 구조체**: `StateComponents/SpellSystem.h` (SpellData, SpellTargeting, SpellMove)
- **ActiveEffect 구조체**: `Objects/Components/StatusEffectComponent.h`
- **경로 탐색**: `StateComponents/AStar.cpp` (GridSystem::FindPath에서 내부 사용)
- **디버그 서브시스템**: `source/Game/DragonicTactics/Debugger/` (DebugConsole, DebugManager, DebugVisualizer)
- **JSON 데이터**: `DragonicTactics/Assets/Data/`
- **AI 플로우차트**: `architecture/character_flowchart/` (Mermaid .mmd — fighter.mmd, cleric.mmd, rouge.mmd 존재 / wizard.mmd 미생성, wizard.jpg만 있음)

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
- [docs/Detailed Implementations/features/button_manager.md](docs/Detailed%20Implementations/features/button_manager.md) — ButtonManager(Move·Action·Spell UI 버튼 패널) 구현 가이드
- [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md) — 아키텍처 원칙
- [architecture/Implementation_Checklist.md](architecture/Implementation_Checklist.md) — 진행 체크리스트
- [docs/Detailed Implementations/features/fighter_strategy.md](docs/Detailed%20Implementations/features/fighter_strategy.md) — FighterStrategy 구현 상세 가이드
- [docs/Detailed Implementations/features/rogue_strategy.md](docs/Detailed%20Implementations/features/rogue_strategy.md) — RogueStrategy 구현 가이드 (참조용)
- [docs/Detailed Implementations/weeks/cleric_implementation.md](docs/Detailed%20Implementations/weeks/cleric_implementation.md) — Cleric 구현 가이드
- [docs/debug/commands.md](docs/debug/commands.md) — 디버그 콘솔 명령어 목록
- [docs/systems/](docs/systems/) — 시스템별 상세 문서 (EventBus, Characters, Components 등)
- [DragonicTactics/README.md](DragonicTactics/README.md) — 빌드 셋업 (영문/한글)
