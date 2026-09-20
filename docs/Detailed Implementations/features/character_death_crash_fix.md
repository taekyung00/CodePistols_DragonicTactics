# 캐릭터 사망 시 크래시 버그 수정

## 1. 작업 동기

`2e1662a` (`add range of basic attack`) 커밋 이후 QA 과정에서 비결정적(non-deterministic) 크래시가 발견되었다.
Fighter 또는 Cleric이 죽을 때, 또는 두 번째 적이 죽을 때 게임이 갑자기 종료되는 현상이었다.

- 어떨 때는 첫 번째 적이 죽을 때 발생
- 어떨 때는 두 번째 적이 죽을 때 발생
- 어떨 때는 두 명 모두 죽을 때까지 발생하지 않음

이 **비결정성**이 버그의 핵심 단서였다. 타이밍에 의존하는 크래시 = use-after-free 패턴.

---

## 2. 원인 분석

### 2-1. 엔진의 오브젝트 삭제 메커니즘

`GameObjectManager::UpdateAll(double dt)` ([Engine/GameObjectManager.cpp](../../../DragonicTactics/source/Engine/GameObjectManager.cpp))는
`unique_ptr` 기반으로 오브젝트를 관리한다.

```cpp
// GameObjectManager.cpp
for (auto it = objects.begin(); it != objects.end(); ++it)
{
    (*it)->Update(dt);          // 업데이트 중 Destroy() 호출 가능
    if ((*it)->Destroyed())
        destroy_iterators.push_back(it);
}
for (auto it : destroy_iterators)
    objects.erase(it);          // unique_ptr 소멸 → 메모리 즉시 해제
```

`Character::TakeDamage()`가 `Destroy()`를 호출하면 해당 프레임에서는 메모리가 살아 있지만,
**다음 프레임 `UpdateAll()` 진입 시 메모리가 해제**된다.

### 2-2. GamePlay::Update() 실행 순서

```cpp
// GamePlay::Update()
goMgr->UpdateAll(scaledDt);      // ① 죽은 캐릭터 메모리 해제
// ...
m_orchestrator->Update(...);     // ② AI 턴 → EndCurrentTurn → StartNextTurn
```

### 2-3. 크래시 경로 A — turnOrder dangling pointer

| 프레임       | 이벤트                                                                              |
| --------- | -------------------------------------------------------------------------------- |
| Frame N   | AI가 Fighter를 공격 → `Fighter->Destroy()` 마킹. `turnOrder`에 포인터 잔존                   |
| Frame N+1 | `goMgr->UpdateAll()` → **Fighter 메모리 해제**. `turnOrder`의 포인터가 dangling            |
| Frame N+1 | `m_orchestrator->Update()` → AI EndTurn → `EndCurrentTurn()` → `StartNextTurn()` |
| Frame N+1 | `StartNextTurn()`의 `remove_if` 안에서 `c->IsAlive()` 호출 → **crash**                 |

크래시 발생 지점:

```cpp
// TurnManager::StartNextTurn()
turnOrder.erase(
    std::remove_if(turnOrder.begin(), turnOrder.end(),
        [](Character* c) {
            return c == nullptr || !c->IsAlive();  // ← freed memory 역참조
        }),
    turnOrder.end()
);
```

**비결정적인 이유**: Dragon이 Fighter를 죽인 뒤 같은 프레임에 End Turn을 하면 안전하고,
한 프레임 이후에 End Turn을 하면 크래시. 플레이어가 얼마나 빨리 버튼을 누르느냐에 달렸다.

### 2-4. 크래시 경로 B — enemys dangling pointer

두 번째 적이 사망할 때 `CheckGameEnd()`가 이미 죽어서 메모리가 해제된 첫 번째 적의 `IsAlive()`를 호출한다.

```cpp
// GamePlay::CheckGameEnd()
bool all_enemies_dead = std::all_of(enemys.begin(), enemys.end(),
    [](Character* c) { return c == nullptr || !c->IsAlive(); });
//                                                ^^^^^^^^^^ freed memory 역참조
```

콜스택 (실제 크래시 캡처):

```
CS230::ComponentManager::GetComponent<StatsComponent>() Line 42
CS230::GameObject::GetGOComponent<StatsComponent>() Line 69
Character::GetStatsComponent() Line 263
Character::IsAlive() Line 233
GamePlay::CheckGameEnd::<lambda_1>::operator()(Character * c) Line 376
GamePlay::CheckGameEnd(const CharacterDeathEvent & event) Line 375
GamePlay::Load::<lambda_5>::operator()(const CharacterDeathEvent & event) Line 296
EventBus::Publish<CharacterDeathEvent>(const CharacterDeathEvent & event) Line 46
CombatSystem::ApplyDamage(...) Line 99
SpellSystem::ApplySpellEffect(...) Line 468
SpellDelayObject::Update(double dt) Line 142
CS230::GameObjectManager::UpdateAll(double dt) Line 43
GamePlay::Update(double dt) Line 452
```

예외 주소 `0xFFFFFFFFFFFFEF` — MSVC 디버그 힙의 freed memory 패턴.

---

## 3. 진단 과정

### 3-1. 진단용 로그 추가

`TurnManager::StartNextTurn()`의 `remove_if` 직전에 진단 코드 삽입:

```cpp
// 1단계: nullptr 체크
for (auto* c : turnOrder)
{
    if (c == nullptr)
        Engine::GetLogger().LogError("[TurnManager] nullptr in turnOrder");
    // 2단계: dangling pointer 체크
    else if (!c->IsAlive())
        Engine::GetLogger().LogError("[TurnManager] DEAD CHAR: " + c->TypeName()
            + " (ptr=" + std::to_string(reinterpret_cast<uintptr_t>(c)) + ")");
}
```

결과: `c->IsAlive()` 자체에서 read access violation 발생.
로그가 출력조차 안 되고 크래시 → `c`는 nullptr이 아닌 dangling pointer 확정.

### 3-2. 진단 결과

| 포인터 값              | 의미                                                |
| ------------------ | ------------------------------------------------- |
| `nullptr`          | 정상 null check로 처리 가능                              |
| `0xFFFFFFFFFFFFEF` | freed memory (dangling) — **IsAlive() 호출 자체가 UB** |

---

## 4. 수정 내용

### 4-1. 수정 파일 목록

| 파일                                | 변경                                                               |
| --------------------------------- | ---------------------------------------------------------------- |
| `StateComponents/TurnManager.h`   | `RemoveFromTurnOrder(Character*)` 선언 추가                          |
| `StateComponents/TurnManager.cpp` | `RemoveFromTurnOrder()` 구현, 진단 루프 제거                             |
| `States/GamePlay.h`               | `m_confirmed_dead_: std::set<Character*>` 멤버 추가                  |
| `States/GamePlay.cpp`             | `CharacterDeathEvent` 핸들러 수정, `CheckGameEnd()` 수정, `Unload()` 수정 |

### 4-2. TurnManager::RemoveFromTurnOrder() (신규)

```cpp
void TurnManager::RemoveFromTurnOrder(Character* character)
{
    if (!character) return;

    // initiativeOrder에서도 제거
    initiativeOrder.erase(
        std::remove_if(initiativeOrder.begin(), initiativeOrder.end(),
            [character](const InitiativeEntry& e) { return e.character == character; }),
        initiativeOrder.end()
    );

    auto it = std::find(turnOrder.begin(), turnOrder.end(), character);
    if (it == turnOrder.end()) return;

    int removed_index = static_cast<int>(std::distance(turnOrder.begin(), it));
    turnOrder.erase(it);

    if (turnOrder.empty()) return;

    // currentTurnIndex 보정
    if (removed_index < currentTurnIndex)
        --currentTurnIndex;
    else if (currentTurnIndex >= static_cast<int>(turnOrder.size()))
        currentTurnIndex = 0;
}
```

`CharacterDeathEvent`가 발생하는 시점은 `Destroy()` 직후이며, 아직 `goMgr->UpdateAll()`의 erase 루프가 실행되지 않은 상태다. 즉 메모리가 해제되기 **전**에 호출하면 안전하게 포인터 비교만으로 제거 가능하다.

### 4-3. GamePlay — CharacterDeathEvent 핸들러

```cpp
// Before
GetGSComponent<EventBus>()->Subscribe<CharacterDeathEvent>(
    [this](const CharacterDeathEvent& event)
    {
        this->CheckGameEnd(event);
        if (event.character)
            m_ui_manager->AddBattleLogEntry(event.character->TypeName() + " died!");
    });

// After
GetGSComponent<EventBus>()->Subscribe<CharacterDeathEvent>(
    [this](const CharacterDeathEvent& event)
    {
        // goMgr->UpdateAll()이 메모리를 해제하기 전에 즉시 처리
        if (event.character)
            m_confirmed_dead_.insert(event.character);

        if (auto* turnMgr = GetGSComponent<TurnManager>())
            turnMgr->RemoveFromTurnOrder(event.character);

        this->CheckGameEnd(event);
        if (event.character)
            m_ui_manager->AddBattleLogEntry(event.character->TypeName() + " died!");
    });
```

### 4-4. GamePlay::CheckGameEnd() — IsAlive() 제거

```cpp
// Before — freed memory 역참조 가능
bool all_enemies_dead = std::all_of(enemys.begin(), enemys.end(),
    [](Character* c) { return c == nullptr || !c->IsAlive(); });

// After — 포인터 값 비교만 수행 (역참조 없음)
bool all_enemies_dead = std::all_of(enemys.begin(), enemys.end(),
    [this](Character* c) { return c == nullptr || m_confirmed_dead_.count(c) > 0; });
```

`m_confirmed_dead_`는 포인터 주소값만 저장하며 역참조하지 않는다.
메모리가 해제된 뒤에도 `std::set::count()`는 포인터 비교만 수행하므로 안전하다.

### 4-5. 설계 핵심 원칙

> **"메모리가 해제되기 전(same-frame)에 포인터를 제거하거나 기록하고,  
> 이후에는 절대 역참조하지 않는다."**

| 시점                                  | 동작                                                     | 안전 여부 |
| ----------------------------------- | ------------------------------------------------------ | ----- |
| `CharacterDeathEvent` 발생 (메모리 해제 전) | `m_confirmed_dead_.insert()` + `RemoveFromTurnOrder()` | ✅ 안전  |
| 다음 프레임 `goMgr->UpdateAll()`         | 메모리 해제                                                 | —     |
| 이후 모든 체크                            | `m_confirmed_dead_.count()` (역참조 없음)                   | ✅ 안전  |

---

## 5. 테스트 과정

### 5-1. 1차 진단 (로그 추가 후)

로그 코드를 삽입한 뒤 재빌드 및 실행.

- `c->IsAlive()` 라인 자체에서 read access violation 발생
- 로그 출력 없이 크래시 → `c`가 nullptr이 아닌 freed memory 포인터임을 확정
- 예외 주소 `0xFFFFFFFFFFFFEF` 확인 (MSVC freed heap 패턴)

### 5-2. 1차 수정 후 재테스트

`RemoveFromTurnOrder()` + 핸들러 수정 적용 후 시나리오 4(두 명 모두 처치) 진행.

- 두 번째 적 사망 시 새로운 크래시 발생
- 콜스택: `GamePlay::CheckGameEnd` → `Character::IsAlive()` → `ComponentManager::GetComponent<StatsComponent>()` → crash
- `enemys` 벡터의 raw pointer도 동일한 dangling pointer 문제임을 확인

### 5-3. 2차 수정 후 최종 테스트

`m_confirmed_dead_` 기반으로 `CheckGameEnd()` 수정 후 다음 시나리오 검증:

| #   | 시나리오                                                               | 결과      |
| --- | ------------------------------------------------------------------ | ------- |
| 1   | Dragon이 Fighter 처치 후 즉시 End Turn 안 하고 대기 → 이후 End Turn             | ✅ 정상    |
| 2   | Dragon이 Fighter 처치 후 Cleric도 처치 → Player Win 표시                    | ✅ 정상    |
| 3   | Dragon의 HP를 낮춘 뒤 AI(Fighter/Cleric) 턴에서 Dragon 사망 → Invader Win 표시 | ✅ 정상    |
| 4   | 두 명 모두 처치하는 전체 게임 3회 반복                                            | ✅ 모두 정상 |

용암 데미지에 의한 사망은 별도 테스트 예정.

### 5-4. 확인 로그

수정 이후 정상 동작 시 게임 로그에서 다음 패턴 확인:

```
[TurnManager] Removed Fighter from turnOrder on death. Remaining: 1
[TurnManager] Removed Cleric from turnOrder on death. Remaining: 0
```

사망 직후 즉시 출력되며, 이후 게임 진행에 이상 없음.

---

## 6. 영향 범위

- 기존 게임플레이 로직 변경 없음
- `turnOrder`와 `enemys`의 순서, 턴 흐름 변동 없음
- `currentTurnIndex` 보정 로직 추가로 사망 직후 턴 인덱스 오동작 방지
- `m_confirmed_dead_`는 포인터 추적용 집합으로 게임 재시작 시 `Unload()`에서 초기화

---

## 7. 참고

- 관련 커밋: `2e1662a` (add range of basic attack) — 이 커밋 이후 현재 작업까지 uncommitted
- 관련 파일:
  - [`States/GamePlay.h`](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.h)
  - [`States/GamePlay.cpp`](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp)
  - [`StateComponents/TurnManager.h`](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.h)
  - [`StateComponents/TurnManager.cpp`](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.cpp)
  - [`Engine/GameObjectManager.cpp`](../../../DragonicTactics/source/Engine/GameObjectManager.cpp) — 삭제 메커니즘 참고
