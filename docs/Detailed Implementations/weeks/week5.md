# Week 5 구현 계획 - 시스템 안정화 및 UI 구축

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**단계**: 시스템 안정화, 게임 UI 구현, 리팩토링
**기간**: Week 5 / 26주 (마감: 2025-12-09)
**전략**: 5명의 개발자가 병렬로 핵심 안정화 작업 수행

**최종 업데이트**: 2025-11-27
**Week 4 상태**: Week 1-3 완료, REFACTORING_TODO.md 완료

**관련 문서**:

- [Week 1-4](week1.md) - 이전 주차 구현
- [docs/implementation-plan.md](../../implementation-plan.md) - 전체 26주 타임라인
- [architecture/REFACTORING_TODO.md](../../../architecture/REFACTORING_TODO.md) - 리팩토링 완료 사항

---

## 📋 목차

- [Week 5 개요](#week-5-개요)
- [개발자 A: 턴 플로우 시스템 명확화](#개발자-a-턴-플로우-시스템-명확화)
- [개발자 B: AI 시스템 강화](#개발자-b-ai-시스템-강화)
- [개발자 C: 게임 UI 시스템 구현](#개발자-c-게임-ui-시스템-구현)
- [개발자 D: 캐릭터 소유권 모델 재설계](#개발자-d-캐릭터-소유권-모델-재설계)
- [개발자 E: AI 행동 시각화 및 맵 데이터 로딩](#개발자-e-ai-행동-시각화-및-맵-데이터-로딩)
- [Week 5 통합 테스트](#week-5-통합-테스트)
- [Week 5 산출물 및 검증](#week-5-산출물-및-검증)

---

## Week 5 개요

Week 5는 게임의 **핵심 안정화** 및 **플레이어 경험 개선**에 집중합니다. Week 1-3에서 구축한 시스템들의 문제점을 해결하고, 실제 플레이 가능한 상태로 만듭니다.

### 핵심 목표

1. **턴 시스템 안정화**: 모든 턴 작업이 명확하게 정의되고 실행됨을 보장
2. **AI 시스템 강화**: 4명의 모험가 캐릭터에 대한 robust한 AI 구현
3. **게임 UI 구축**: 플레이어가 게임 상태를 파악할 수 있는 UI (ImGui 아님!)
4. **메모리 안정성**: 스마트 포인터 도입으로 메모리 누수 방지
5. **플레이 경험 개선**: AI pause 시스템 및 맵 로딩

### 주요 변경사항 (Week 4 대비)

- **리팩토링 완료**: Engine-Game 의존성 역전, PCH 구축, GamePlay 리팩토링 모두 완료
- **UI 명확화**: ImGui (디버그용) vs 게임 UI (플레이용) 구분
- **메모리 관리**: Raw 포인터 → 스마트 포인터 전환

### 통합 목표 (금요일)

- **플레이 가능한 데모**: Dragon vs Fighter 전투가 완전히 플레이 가능
- **UI 완비**: 게임 UI로 모든 정보 확인 가능
- **AI 작동**: Fighter가 자율적으로 전투
- **안정성**: 메모리 누수 없음, 크래시 없음

---

## 개발자 A: 턴 플로우 시스템 명확화

**목표**: 매 턴마다 실행되어야 하는 작업들의 명확한 정의 및 함수 일대일 대응

**기초 지식**:

- Week 3에서 TurnManager 구현 완료
- 현재 문제: 함수 중복 호출, 누락된 작업, 호출 순서 불명확

**파일 수정 목록**:

```
DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.h/cpp
DragonicTactics/source/Game/DragonicTactics/Objects/Character.h/cpp
DragonicTactics/source/Game/DragonicTactics/Objects/Dragon.h/cpp
DragonicTactics/source/Game/DragonicTactics/Objects/Fighter.h/cpp
DragonicTactics/source/Game/DragonicTactics/Test/TestTurnFlow.h/cpp (신규)
docs/turn-flow-chart.md (신규)
```

---

### 구현 작업 (턴 플로우 시스템)

#### **Task 1: 턴 플로우 차트 작성** (Day 1-2)

**목표**: 턴 시스템의 모든 작업을 시각적으로 정리

**단계**:

1. **턴 시작 시 필수 작업 목록 작성**
   
   ```
   1. StatusEffectManager - 시작 시 효과 적용 (Burn 데미지 등)
   2. ActionPoints - Refresh() 호출
   3. MovementComponent - 이동 가능 범위 초기화
   4. Character - OnTurnStart() 호출 (캐릭터별 커스텀 로직)
   5. EventBus - TurnStartedEvent 발행
   ```

2. **턴 진행 중 허용 작업**
   
   ```
   - 이동 (AP 소모)
   - 공격 (AP 소모)
   - 스펠 캐스팅 (AP + 스펠 슬롯 소모)
   - 대기 (턴 종료)
   ```

3. **턴 종료 시 필수 작업**
   
   ```
   1. StatusEffectManager - 종료 시 효과 적용
   2. Character - OnTurnEnd() 호출
   3. ActionPoints - 남은 AP 기록 (디버그용)
   4. EventBus - TurnEndedEvent 발행
   5. TurnManager - 다음 캐릭터로 이동
   ```

4. **라운드 전환 시 작업**
   
   ```
   1. TurnManager - 라운드 번호 증가
   2. EventBus - RoundStartedEvent 발행
   3. BattleManager - 승리 조건 체크 (선택사항)
   ```

**플로우 차트 (Mermaid 형식)**:

```markdown
# docs/turn-flow-chart.md

## 턴 플로우 차트

\`\`\`mermaid
graph TD
    A[전투 시작] --> B[TurnManager::InitializeTurnOrder]
    B --> C[라운드 1 시작]
    C --> D{턴 시작}

    D --> E[StatusEffectManager::ApplyStartOfTurnEffects]
    E --> F[ActionPoints::Refresh]
    F --> G[MovementComponent::ResetMovementRange]
    G --> H[Character::OnTurnStart]
    H --> I[EventBus::Publish TurnStartedEvent]

    I --> J{플레이어 or AI?}
    J -->|플레이어| K[플레이어 입력 대기]
    J -->|AI| L[AISystem::ExecuteAITurn]

    K --> M{행동 선택}
    L --> M

    M -->|이동| N[MovementComponent::Move]
    M -->|공격| O[CombatSystem::ExecuteAttack]
    M -->|스펠| P[SpellSystem::CastSpell]
    M -->|대기| Q[턴 종료]

    N --> R{AP 남음?}
    O --> R
    P --> R

    R -->|Yes| M
    R -->|No| Q

    Q --> S[StatusEffectManager::ApplyEndOfTurnEffects]
    S --> T[Character::OnTurnEnd]
    T --> U[EventBus::Publish TurnEndedEvent]
    U --> V[TurnManager::AdvanceTurn]

    V --> W{모든 캐릭터 턴 완료?}
    W -->|No| D
    W -->|Yes| X[라운드 종료]

    X --> Y{전투 종료?}
    Y -->|No| C
    Y -->|Yes| Z[전투 종료]
\`\`\`
```

**중요 원칙**:

- 각 박스 = 하나의 함수 호출
- 함수 중복 호출 금지
- 모든 경로에서 필수 작업 누락 없음

---

#### **Task 2: 함수 일대일 대응** (Day 2-3)

**목표**: 플로우 차트의 각 항목과 실제 함수를 1:1 매핑, 중복 제거

**현재 문제**:

```cpp
// 문제 1: Character::OnTurnStart()가 호출되지 않음
void Character::OnTurnStart() {
    // 이 함수는 아무도 호출하지 않음!
    GetGOComponent<ActionPoints>()->Refresh();  // ← 중복!
}

// 문제 2: TurnManager에서 직접 호출
void TurnManager::StartNextTurn() {
    Character* current = GetCurrentCharacter();
    current->RefreshActionPoints();  // ← 중복!
}
```

**해결 방법**:

**Step 1: 중복 함수 제거**

```cpp
// Character.h - OnTurnStart 제거 또는 수정
class Character : public CS230::GameObject {
public:
    // ❌ 제거: void OnTurnStart();
    // ✅ 유지하되, RefreshActionPoints는 제거
    virtual void OnTurnStart() {
        // 캐릭터별 커스텀 로직만 (예: Dragon의 특수 능력)
        // ActionPoints는 TurnManager가 관리
    }
};
```

```cpp
// TurnManager.cpp - 명확한 책임 분리
void TurnManager::StartNextTurn() {
    Character* current = GetCurrentCharacter();

    // 1. StatusEffects 먼저
    StatusEffectManager::Instance().ApplyStartOfTurnEffects(current);

    // 2. ActionPoints 리프레시
    current->GetGOComponent<ActionPoints>()->Refresh();

    // 3. MovementRange 초기화
    current->GetGOComponent<MovementComponent>()->ResetMovementRange();

    // 4. 캐릭터별 커스텀 로직 (virtual 함수)
    current->OnTurnStart();

    // 5. 이벤트 발행
    TurnStartedEvent event{current, current_turn_, round_number_};
    EventBus::Instance().Publish(event);

    Engine::GetLogger().LogEvent("Turn started for " + current->TypeName());
}
```

**Step 2: 함수 호출 체크리스트 작성**

```cpp
// TurnManager.h - 체크리스트 주석 추가
class TurnManager : public CS230::Component {
public:
    void StartNextTurn();  // 턴 시작 체크리스트:
                           // [1] ApplyStartOfTurnEffects
                           // [2] Refresh ActionPoints
                           // [3] Reset MovementRange
                           // [4] Character::OnTurnStart (virtual)
                           // [5] Publish TurnStartedEvent

    void EndCurrentTurn(); // 턴 종료 체크리스트:
                           // [1] ApplyEndOfTurnEffects
                           // [2] Character::OnTurnEnd (virtual)
                           // [3] Publish TurnEndedEvent
                           // [4] AdvanceTurn
};
```

---

#### **Task 3: 디버그 로깅 시스템** (Day 3-4)

**목표**: `__PRETTY_FUNCTION__` 매크로로 함수 호출 추적

**구현 예시**:

```cpp
// TurnManager.cpp
void TurnManager::StartNextTurn() {
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - BEGIN");

    Character* current = GetCurrentCharacter();

    // 1. StatusEffects
    Engine::GetLogger().LogDebug("  [1/5] ApplyStartOfTurnEffects");
    StatusEffectManager::Instance().ApplyStartOfTurnEffects(current);

    // 2. ActionPoints
    Engine::GetLogger().LogDebug("  [2/5] Refresh ActionPoints");
    current->GetGOComponent<ActionPoints>()->Refresh();

    // 3. MovementRange
    Engine::GetLogger().LogDebug("  [3/5] Reset MovementRange");
    current->GetGOComponent<MovementComponent>()->ResetMovementRange();

    // 4. OnTurnStart
    Engine::GetLogger().LogDebug("  [4/5] Character::OnTurnStart");
    current->OnTurnStart();

    // 5. Event
    Engine::GetLogger().LogDebug("  [5/5] Publish TurnStartedEvent");
    TurnStartedEvent event{current, current_turn_, round_number_};
    EventBus::Instance().Publish(event);

    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - END");
}
```

**Character.cpp에도 적용**:

```cpp
void Dragon::OnTurnStart() {
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - Dragon 턴 시작");
    // Dragon 특수 로직 (예: 드래곤 브레스 쿨다운 감소)
}

void Fighter::OnTurnStart() {
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - Fighter 턴 시작");
    // Fighter 특수 로직 (예: 방어 태세 초기화)
}
```

**로그 출력 예시**:

```
[DEBUG] TurnManager::StartNextTurn - BEGIN
[DEBUG]   [1/5] ApplyStartOfTurnEffects
[DEBUG] StatusEffectManager::ApplyStartOfTurnEffects - Applying Burn to Fighter
[DEBUG]   [2/5] Refresh ActionPoints
[DEBUG] ActionPoints::Refresh - AP set to 2
[DEBUG]   [3/5] Reset MovementRange
[DEBUG]   [4/5] Character::OnTurnStart
[DEBUG] Fighter::OnTurnStart - Fighter 턴 시작
[DEBUG]   [5/5] Publish TurnStartedEvent
[DEBUG] TurnManager::StartNextTurn - END
```

---

#### **Task 4: 테스트 및 검증** (Day 4-5)

**테스트 파일**: `DragonicTactics/source/Game/DragonicTactics/Test/TestTurnFlow.h/cpp`

**테스트 시나리오**:

```cpp
// TestTurnFlow.cpp
#include "pch.h"
#include "TestTurnFlow.h"
#include "../StateComponents/TurnManager.h"
#include "../Objects/Dragon.h"
#include "../Objects/Fighter.h"

void TestTurnFlow::TestSingleTurn() {
    // Setup
    TurnManager* turn_mgr = GetGSComponent<TurnManager>();
    Dragon* dragon = CreateTestDragon();

    // 로그 활성화
    Engine::GetLogger().SetLogLevel(LogLevel::Debug);

    // Test
    turn_mgr->StartNextTurn();

    // Verify
    // 1. ActionPoints가 리프레시되었는가?
    auto ap = dragon->GetGOComponent<ActionPoints>();
    assert(ap->GetCurrent() == 2);  // Dragon은 AP 2

    // 2. OnTurnStart가 호출되었는가? (로그 확인)
    // 3. TurnStartedEvent가 발행되었는가?

    turn_mgr->EndCurrentTurn();

    // Verify
    // 1. OnTurnEnd가 호출되었는가?
    // 2. TurnEndedEvent가 발행되었는가?
    // 3. 다음 캐릭터로 이동했는가?
}

void TestTurnFlow::TestFullRound() {
    // 전체 라운드 테스트 (Dragon → Fighter)
    TurnManager* turn_mgr = GetGSComponent<TurnManager>();

    // 턴 1: Dragon
    turn_mgr->StartNextTurn();
    turn_mgr->EndCurrentTurn();

    // 턴 2: Fighter
    turn_mgr->StartNextTurn();
    turn_mgr->EndCurrentTurn();

    // Verify: 라운드 2로 진행되었는가?
    assert(turn_mgr->GetRoundNumber() == 2);
}

void TestTurnFlow::TestEdgeCases() {
    // 엣지 케이스 테스트

    // 1. 첫 턴 (라운드 1, 턴 1)
    // 2. 마지막 턴 (모든 캐릭터 턴 완료 후)
    // 3. 캐릭터 사망 시 (턴 스킵)
}
```

**GamePlay.cpp에서 테스트 단축키 추가**:

```cpp
void GamePlay::Update(double dt) {
    auto& input = Engine::GetInput();

    if (input.IsKeyPressed(InputKey::F9)) {
        // F9: 턴 플로우 테스트 실행
        Engine::GetLogger().LogEvent("=== Turn Flow Test START ===");
        TestTurnFlow::TestSingleTurn();
        TestTurnFlow::TestFullRound();
        TestTurnFlow::TestEdgeCases();
        Engine::GetLogger().LogEvent("=== Turn Flow Test END ===");
    }

    // ...
}
```

---

### 구현 예시 (턴 플로우 시스템)

**파일**: `TurnManager.cpp` (개선된 버전)

```cpp
#include "pch.h"
#include "TurnManager.h"
#include "../Objects/Character.h"
#include "StatusEffectManager.h"
#include "EventBus.h"

void TurnManager::StartNextTurn() {
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - BEGIN");

    if (current_turn_index_ >= turn_order_.size()) {
        StartNewRound();
        return;
    }

    Character* current = turn_order_[current_turn_index_];

    // 체크리스트 [1/5]: StatusEffects 시작 효과
    Engine::GetLogger().LogDebug("  [1/5] ApplyStartOfTurnEffects");
    StatusEffectManager::Instance().ApplyStartOfTurnEffects(current);

    // 체크리스트 [2/5]: ActionPoints 리프레시
    Engine::GetLogger().LogDebug("  [2/5] Refresh ActionPoints");
    auto ap = current->GetGOComponent<ActionPoints>();
    if (ap) {
        ap->Refresh();
    }

    // 체크리스트 [3/5]: MovementRange 초기화
    Engine::GetLogger().LogDebug("  [3/5] Reset MovementRange");
    auto movement = current->GetGOComponent<MovementComponent>();
    if (movement) {
        movement->ResetMovementRange();
    }

    // 체크리스트 [4/5]: 캐릭터별 OnTurnStart (virtual)
    Engine::GetLogger().LogDebug("  [4/5] Character::OnTurnStart (virtual)");
    current->OnTurnStart();

    // 체크리스트 [5/5]: 이벤트 발행
    Engine::GetLogger().LogDebug("  [5/5] Publish TurnStartedEvent");
    TurnStartedEvent event{
        current,
        static_cast<int>(current_turn_index_),
        round_number_
    };
    EventBus::Instance().Publish(event);

    is_turn_active_ = true;

    Engine::GetLogger().LogEvent("Turn started: " + current->TypeName() +
                                  " (Round " + std::to_string(round_number_) +
                                  ", Turn " + std::to_string(current_turn_index_ + 1) + ")");
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - END");
}

void TurnManager::EndCurrentTurn() {
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - BEGIN");

    if (!is_turn_active_) {
        Engine::GetLogger().LogWarning("EndCurrentTurn called but no turn is active");
        return;
    }

    Character* current = turn_order_[current_turn_index_];

    // 체크리스트 [1/3]: StatusEffects 종료 효과
    Engine::GetLogger().LogDebug("  [1/3] ApplyEndOfTurnEffects");
    StatusEffectManager::Instance().ApplyEndOfTurnEffects(current);

    // 체크리스트 [2/3]: 캐릭터별 OnTurnEnd (virtual)
    Engine::GetLogger().LogDebug("  [2/3] Character::OnTurnEnd (virtual)");
    current->OnTurnEnd();

    // 체크리스트 [3/3]: 이벤트 발행
    Engine::GetLogger().LogDebug("  [3/3] Publish TurnEndedEvent");
    TurnEndedEvent event{
        current,
        static_cast<int>(current_turn_index_)
    };
    EventBus::Instance().Publish(event);

    is_turn_active_ = false;
    current_turn_index_++;

    Engine::GetLogger().LogEvent("Turn ended: " + current->TypeName());
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - END");
}

void TurnManager::StartNewRound() {
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - BEGIN");

    round_number_++;
    current_turn_index_ = 0;

    Engine::GetLogger().LogEvent("=== Round " + std::to_string(round_number_) + " START ===");

    RoundStartedEvent event{round_number_};
    EventBus::Instance().Publish(event);

    // 첫 캐릭터 턴 시작
    StartNextTurn();

    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - END");
}
```

---

### 엄격한 테스트 (턴 플로우 시스템)

**테스트 목표**: 모든 필수 함수가 정확히 한 번씩 호출되는지 확인

**방법 1: 로그 분석**

```bash
# 게임 실행 후 로그 파일 확인
grep "PRETTY_FUNCTION" DragonicTactics/log.txt

# 예상 출력:
# TurnManager::StartNextTurn - BEGIN
# TurnManager::StartNextTurn - END
# Fighter::OnTurnStart
# TurnManager::EndCurrentTurn - BEGIN
# TurnManager::EndCurrentTurn - END
```

**방법 2: 이벤트 카운터**

```cpp
// TestTurnFlow.cpp
class EventCounter {
private:
    int turn_started_count = 0;
    int turn_ended_count = 0;

public:
    void Setup() {
        EventBus::Instance().Subscribe<TurnStartedEvent>(
            [this](const TurnStartedEvent&) {
                turn_started_count++;
            }
        );

        EventBus::Instance().Subscribe<TurnEndedEvent>(
            [this](const TurnEndedEvent&) {
                turn_ended_count++;
            }
        );
    }

    void Verify(int expected_starts, int expected_ends) {
        assert(turn_started_count == expected_starts);
        assert(turn_ended_count == expected_ends);
    }
};

void TestTurnFlow::TestEventCounts() {
    EventCounter counter;
    counter.Setup();

    TurnManager* turn_mgr = GetGSComponent<TurnManager>();

    // 2턴 실행
    turn_mgr->StartNextTurn();
    turn_mgr->EndCurrentTurn();
    turn_mgr->StartNextTurn();
    turn_mgr->EndCurrentTurn();

    // Verify: 2번의 Start, 2번의 End
    counter.Verify(2, 2);
}
```

**방법 3: ActionPoints 검증**

```cpp
void TestTurnFlow::TestActionPointsRefresh() {
    Dragon* dragon = CreateTestDragon();
    auto ap = dragon->GetGOComponent<ActionPoints>();

    // 초기 상태: AP 2
    assert(ap->GetCurrent() == 2);

    // AP 소모
    ap->Spend(2);
    assert(ap->GetCurrent() == 0);

    // 턴 시작 → AP 리프레시되어야 함
    TurnManager::Instance().StartNextTurn();
    assert(ap->GetCurrent() == 2);  // ✅ 리프레시 확인
}
```

---

### 사용 예시 (턴 플로우 시스템)

**GamePlay.cpp에서 턴 관리**:

```cpp
void GamePlay::Update(double dt) {
    TurnManager* turn_mgr = GetGSComponent<TurnManager>();
    Character* current = turn_mgr->GetCurrentCharacter();

    if (!current) return;

    // 플레이어 턴 (Dragon)
    if (current->GetCharacterType() == CharacterTypes::Dragon) {
        // 플레이어 입력 처리
        if (player_action_complete) {
            turn_mgr->EndCurrentTurn();  // ✅ 체크리스트 자동 실행
        }
    }
    // AI 턴 (Fighter)
    else {
        AISystem::Instance().ExecuteAITurn(current);
        if (ai_action_complete) {
            turn_mgr->EndCurrentTurn();  // ✅ 체크리스트 자동 실행
        }
    }
}
```

**디버깅 예시**:

```cpp
// F9 키로 턴 플로우 테스트 실행
if (Engine::GetInput().IsKeyPressed(InputKey::F9)) {
    Engine::GetLogger().SetLogLevel(LogLevel::Debug);  // 상세 로그 활성화

    TurnManager::Instance().StartNextTurn();
    // 로그 확인:
    // [DEBUG] TurnManager::StartNextTurn - BEGIN
    // [DEBUG]   [1/5] ApplyStartOfTurnEffects
    // ...
    // [DEBUG] TurnManager::StartNextTurn - END

    TurnManager::Instance().EndCurrentTurn();
    // 로그 확인:
    // [DEBUG] TurnManager::EndCurrentTurn - BEGIN
    // ...
}
```

---

### 일일 작업 분배 (개발자 A)

| 일차      | 작업           | 예상 시간 | 산출물                |
| ------- | ------------ | ----- | ------------------ |
| Day 1   | 턴 플로우 차트 작성  | 4h    | turn-flow-chart.md |
| Day 1-2 | 필수 작업 목록 정리  | 4h    | 체크리스트 문서           |
| Day 2   | 중복 함수 제거     | 4h    | TurnManager.cpp 수정 |
| Day 3   | 함수 일대일 대응 완료 | 4h    | Character.cpp 수정   |
| Day 3   | 디버그 로깅 추가    | 4h    | 모든 함수에 로깅          |
| Day 4   | 테스트 코드 작성    | 4h    | TestTurnFlow.cpp   |
| Day 4-5 | 통합 테스트 및 검증  | 8h    | 테스트 레포트            |

**총 예상 시간**: 32시간 (4일)

---

## 개발자 B: AI 시스템 강화

**목표**: 4명의 모험가 캐릭터에 대한 robust한 AI 구현

**파일 수정 목록**:

```
DragonicTactics/source/Game/DragonicTactics/StateComponents/AISystem.h/cpp
DragonicTactics/source/Game/DragonicTactics/Objects/Fighter.h/cpp
DragonicTactics/source/Game/DragonicTactics/Test/TestAI.h/cpp
docs/ai-decision-tree.md (신규)
```

---

### 구현 작업 (AI 시스템 강화)

#### **Task 1: 공통 AI 프레임워크** (Day 1-2)

**목표**: 모든 AI 캐릭터가 공유하는 기본 결정 트리

**AI 결정 단계**:

```
1. Evaluate Situation (상황 평가)
   - 자신의 HP, AP
   - 적의 위치, HP
   - 거리 계산
   - 위협도 평가

2. Generate Actions (행동 후보 생성)
   - 이동 가능 위치
   - 공격 가능 대상
   - 스펠 캐스팅 가능 여부

3. Score Actions (행동 점수 계산)
   - 각 행동의 기대 효과
   - Bias 적용 (공격성, 방어성)

4. Select Best Action (최적 행동 선택)
   - 가장 높은 점수의 행동 실행
```

**구현 예시**:

```cpp
// AISystem.h
class AISystem : public CS230::Component {
public:
    struct SituationEvaluation {
        Character* self;
        Character* primary_target;
        std::vector<Character*> all_enemies;

        int self_hp_percent;
        int target_hp_percent;
        int distance_to_target;
        int threat_level;  // 0-100
    };

    struct AIAction {
        enum Type { Move, Attack, CastSpell, Defend };

        Type type;
        Math::ivec2 target_position;
        Character* target_character;
        int spell_id;

        float score;  // 행동 점수
        std::string reasoning;  // 디버그용
    };

    // 공통 AI 프레임워크
    SituationEvaluation EvaluateSituation(Character* ai_character);
    std::vector<AIAction> GenerateActions(Character* ai_character, const SituationEvaluation& eval);
    void ScoreActions(std::vector<AIAction>& actions, const SituationEvaluation& eval, float aggression_bias);
    AIAction SelectBestAction(const std::vector<AIAction>& actions);
    void ExecuteAction(Character* ai_character, const AIAction& action);

    // 캐릭터별 AI (virtual로 오버라이드 가능)
    virtual void ExecuteFighterAI(Character* fighter);
    virtual void ExecuteClericAI(Character* cleric);  // 향후 구현
    virtual void ExecuteWizardAI(Character* wizard);  // 향후 구현
    virtual void ExecuteRogueAI(Character* rogue);    // 향후 구현
};
```

**상황 평가 구현**:

```cpp
AISystem::SituationEvaluation AISystem::EvaluateSituation(Character* ai_character) {
    SituationEvaluation eval;
    eval.self = ai_character;

    // 1. 자신의 상태
    auto stats = ai_character->GetGOComponent<StatsComponent>();
    eval.self_hp_percent = (stats->GetCurrentHP() * 100) / stats->GetMaxHP();

    // 2. 적 탐색
    GridSystem* grid = GetGSComponent<GridSystem>();
    eval.all_enemies = grid->GetCharactersByType(CharacterTypes::Dragon);  // Dragon이 적

    if (eval.all_enemies.empty()) {
        eval.primary_target = nullptr;
        return eval;
    }

    // 3. 주요 타겟 선택 (가장 가까운 적)
    eval.primary_target = FindClosestEnemy(ai_character, eval.all_enemies);

    // 4. 거리 계산
    Math::ivec2 self_pos = ai_character->GetGOComponent<GridPosition>()->Get();
    Math::ivec2 target_pos = eval.primary_target->GetGOComponent<GridPosition>()->Get();
    eval.distance_to_target = std::abs(target_pos.x - self_pos.x) + std::abs(target_pos.y - self_pos.y);

    // 5. 타겟 HP
    auto target_stats = eval.primary_target->GetGOComponent<StatsComponent>();
    eval.target_hp_percent = (target_stats->GetCurrentHP() * 100) / target_stats->GetMaxHP();

    // 6. 위협도 평가 (타겟이 강할수록 높음)
    eval.threat_level = eval.target_hp_percent + (eval.distance_to_target < 3 ? 30 : 0);

    Engine::GetLogger().LogDebug("AI Evaluation: HP=" + std::to_string(eval.self_hp_percent) +
                                  "%, Distance=" + std::to_string(eval.distance_to_target) +
                                  ", Threat=" + std::to_string(eval.threat_level));

    return eval;
}
```

---

#### **Task 2: 캐릭터별 행동 전략** (Day 2-4)

**Fighter AI 전략**:

```cpp
void AISystem::ExecuteFighterAI(Character* fighter) {
    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - BEGIN");

    // 1. 상황 평가
    SituationEvaluation eval = EvaluateSituation(fighter);

    if (!eval.primary_target) {
        Engine::GetLogger().LogWarning("Fighter AI: No target found");
        return;
    }

    // 2. 행동 후보 생성
    std::vector<AIAction> actions = GenerateActions(fighter, eval);

    // 3. Fighter 특화 bias (공격적)
    float aggression_bias = 0.8f;  // 0.0 (방어적) ~ 1.0 (공격적)
    ScoreActions(actions, eval, aggression_bias);

    // 4. 최적 행동 선택
    AIAction best_action = SelectBestAction(actions);

    Engine::GetLogger().LogEvent("Fighter AI Decision: " + best_action.reasoning);

    // 5. 행동 실행
    ExecuteAction(fighter, best_action);

    Engine::GetLogger().LogDebug(std::string(__PRETTY_FUNCTION__) + " - END");
}
```

**행동 후보 생성**:

```cpp
std::vector<AISystem::AIAction> AISystem::GenerateActions(
    Character* ai_character,
    const SituationEvaluation& eval
) {
    std::vector<AIAction> actions;

    GridSystem* grid = GetGSComponent<GridSystem>();
    Math::ivec2 self_pos = ai_character->GetGOComponent<GridPosition>()->Get();
    Math::ivec2 target_pos = eval.primary_target->GetGOComponent<GridPosition>()->Get();

    // 1. 이동 액션 (타겟에게 가까이)
    if (eval.distance_to_target > 1) {
        AIAction move_action;
        move_action.type = AIAction::Move;
        move_action.target_position = GetPositionCloserTo(self_pos, target_pos);
        move_action.reasoning = "Move closer to target";
        actions.push_back(move_action);
    }

    // 2. 공격 액션 (사거리 내)
    int attack_range = ai_character->GetGOComponent<StatsComponent>()->GetAttackRange();
    if (eval.distance_to_target <= attack_range) {
        AIAction attack_action;
        attack_action.type = AIAction::Attack;
        attack_action.target_character = eval.primary_target;
        attack_action.reasoning = "Melee attack on target";
        actions.push_back(attack_action);
    }

    // 3. 방어 액션 (HP 낮을 때)
    if (eval.self_hp_percent < 30) {
        AIAction defend_action;
        defend_action.type = AIAction::Defend;
        defend_action.reasoning = "Defend (low HP)";
        actions.push_back(defend_action);
    }

    return actions;
}
```

**행동 점수 계산**:

```cpp
void AISystem::ScoreActions(
    std::vector<AIAction>& actions,
    const SituationEvaluation& eval,
    float aggression_bias
) {
    for (auto& action : actions) {
        float score = 0.0f;

        switch (action.type) {
            case AIAction::Move:
                // 이동: 타겟에 가까워질수록 높은 점수
                score = 50.0f + (10.0f - eval.distance_to_target) * 5.0f;
                score *= (1.0f + aggression_bias);  // 공격성 반영
                break;

            case AIAction::Attack:
                // 공격: 타겟 HP가 낮을수록 높은 점수
                score = 70.0f + (100 - eval.target_hp_percent) * 0.3f;
                score *= (1.0f + aggression_bias * 0.5f);
                break;

            case AIAction::Defend:
                // 방어: 자신의 HP가 낮을수록 높은 점수
                score = 40.0f + (100 - eval.self_hp_percent) * 0.5f;
                score *= (1.0f - aggression_bias);  // 공격성 낮을수록 선호
                break;
        }

        action.score = score;
    }

    // 점수 순으로 정렬
    std::sort(actions.begin(), actions.end(), [](const AIAction& a, const AIAction& b) {
        return a.score > b.score;
    });
}
```

---

#### **Task 3: Bias 시스템** (Day 4-5)

**목표**: 런타임에 AI 성향 조정 가능

```cpp
// AISystem.h
class AISystem : public CS230::Component {
public:
    struct AIBias {
        float aggression = 0.5f;  // 0.0 (방어적) ~ 1.0 (공격적)
        float teamwork = 0.5f;    // 0.0 (개인) ~ 1.0 (팀플레이)
        float risk_taking = 0.5f; // 0.0 (안전) ~ 1.0 (위험 감수)
    };

    void SetBias(Character* ai_character, const AIBias& bias);
    AIBias GetBias(Character* ai_character) const;

private:
    std::map<Character*, AIBias> bias_map_;
};
```

**사용 예시**:

```cpp
// GamePlay.cpp - AI 난이도 조절
void GamePlay::Load() {
    // ...

    // Fighter: 공격적인 AI
    AISystem::AIBias fighter_bias;
    fighter_bias.aggression = 0.8f;  // 높은 공격성
    fighter_bias.teamwork = 0.3f;
    fighter_bias.risk_taking = 0.6f;
    AISystem::Instance().SetBias(fighter, fighter_bias);

    // Cleric: 방어적인 AI (향후)
    AISystem::AIBias cleric_bias;
    cleric_bias.aggression = 0.2f;  // 낮은 공격성
    cleric_bias.teamwork = 0.9f;    // 높은 팀워크
    cleric_bias.risk_taking = 0.3f;
    // AISystem::Instance().SetBias(cleric, cleric_bias);
}
```

**ImGui로 런타임 조정** (디버그용):

```cpp
void GamePlay::DrawImGui() {
    if (ImGui::Begin("AI Bias Tuning")) {
        AISystem::AIBias bias = AISystem::Instance().GetBias(fighter);

        ImGui::SliderFloat("Aggression", &bias.aggression, 0.0f, 1.0f);
        ImGui::SliderFloat("Teamwork", &bias.teamwork, 0.0f, 1.0f);
        ImGui::SliderFloat("Risk Taking", &bias.risk_taking, 0.0f, 1.0f);

        if (ImGui::Button("Apply")) {
            AISystem::Instance().SetBias(fighter, bias);
            Engine::GetLogger().LogEvent("AI bias updated");
        }
    }
    ImGui::End();
}
```

---

#### **Task 4: 테스트 및 밸런싱** (Day 5-6)

**AI vs AI 테스트**:

```cpp
// TestAI.cpp
void TestAI::TestFighterAI() {
    // Setup: Fighter vs Fighter
    Fighter* fighter1 = CreateTestFighter({0, 0});
    Fighter* fighter2 = CreateTestFighter({7, 7});

    TurnManager::Instance().InitializeTurnOrder({fighter1, fighter2});

    // 10턴 시뮬레이션
    for (int turn = 0; turn < 10; ++turn) {
        TurnManager::Instance().StartNextTurn();

        Character* current = TurnManager::Instance().GetCurrentCharacter();
        AISystem::Instance().ExecuteFighterAI(current);

        TurnManager::Instance().EndCurrentTurn();

        // 전투 종료 체크
        if (fighter1->GetHP() <= 0 || fighter2->GetHP() <= 0) {
            break;
        }
    }

    // Verify: 둘 중 하나는 승리
    assert((fighter1->GetHP() > 0) != (fighter2->GetHP() > 0));
}
```

**밸런스 테스트**:

```cpp
void TestAI::TestDragonVsFighter() {
    Dragon* dragon = CreateTestDragon({4, 4});
    Fighter* fighter = CreateTestFighter({0, 0});

    // Dragon AI는 플레이어가 조작하므로, Fighter AI만 테스트
    TurnManager::Instance().InitializeTurnOrder({dragon, fighter});

    // Dragon 턴: 수동 행동
    TurnManager::Instance().StartNextTurn();
    // ... Dragon 행동 ...
    TurnManager::Instance().EndCurrentTurn();

    // Fighter 턴: AI 행동
    TurnManager::Instance().StartNextTurn();
    AISystem::Instance().ExecuteFighterAI(fighter);
    TurnManager::Instance().EndCurrentTurn();

    // Verify: Fighter가 Dragon에게 접근했는가?
    Math::ivec2 fighter_pos = fighter->GetGOComponent<GridPosition>()->Get();
    Math::ivec2 dragon_pos = dragon->GetGOComponent<GridPosition>()->Get();
    int distance = std::abs(fighter_pos.x - dragon_pos.x) + std::abs(fighter_pos.y - dragon_pos.y);
    assert(distance < 7);  // 초기 거리보다 가까워져야 함
}
```

---

### 엄격한 테스트 (AI 시스템)

**테스트 케이스**:

1. **행동 생성 테스트**: 모든 가능한 행동이 생성되는가?
2. **점수 계산 테스트**: Bias에 따라 점수가 달라지는가?
3. **행동 실행 테스트**: 선택된 행동이 정확히 실행되는가?
4. **엣지 케이스**: AP 부족, 타겟 없음, 이동 불가능 등

---

### 일일 작업 분배 (개발자 B)

| 일차      | 작업               | 예상 시간 | 산출물                           |
| ------- | ---------------- | ----- | ----------------------------- |
| Day 1-2 | 공통 AI 프레임워크      | 8h    | AISystem.cpp 기본 구조            |
| Day 2-3 | Fighter AI 전략 구현 | 8h    | ExecuteFighterAI 완성           |
| Day 3-4 | 행동 생성 및 점수 계산    | 8h    | GenerateActions, ScoreActions |
| Day 4-5 | Bias 시스템 구현      | 4h    | AIBias 구조체 및 적용               |
| Day 5-6 | 테스트 및 밸런싱        | 12h   | TestAI.cpp, 밸런스 조정            |

**총 예상 시간**: 40시간 (5일)

---

## 개발자 C: 게임 UI 시스템 구현

**목표**: 실제 플레이용 게임 UI 구현 및 개발자용 디버그 UI 개선

**중요 구분**:

- **게임 UI** (GameUIManager): 플레이어가 게임 플레이 시 보는 UI (크고, 가독성 높음, 예쁨)
- **디버그 UI** (DebugUIManager): 개발자가 디버깅 시 보는 UI (ImGui 사용, 작고, 기능 중심)

**파일 목록**:

```
DragonicTactics/source/Game/DragonicTactics/UI/GameUIManager.h/cpp (신규)
DragonicTactics/source/Game/DragonicTactics/UI/DebugUIManager.h/cpp (신규)
DragonicTactics/source/Game/DragonicTactics/UI/UIComponents/ (신규 폴더)
  ├── HPBar.h/cpp
  ├── APDisplay.h/cpp
  ├── TurnOrderPanel.h/cpp
  └── ActionLog.h/cpp
```

---

### 구현 작업 (게임 UI 시스템)

#### **Part A: GameUIManager - 실제 플레이용** (Day 1-3)

**Task 1: GameUIManager 클래스 생성** (Day 1)

**목표**: EventBus에서 정보를 받아 화면에 렌더링

```cpp
// GameUIManager.h
#pragma once
#include "Engine/Component.h"
#include "Engine/Vec2.hpp"
#include <vector>
#include <string>

class Character;

class GameUIManager : public CS230::Component {
public:
    GameUIManager();
    ~GameUIManager();

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix);

    // EventBus에서 호출될 콜백
    void OnTurnStarted(const TurnStartedEvent& event);
    void OnCharacterDamaged(const CharacterDamagedEvent& event);
    void OnCharacterHealed(const CharacterHealedEvent& event);
    void OnSpellCast(const SpellCastEvent& event);

private:
    // UI 컴포넌트들
    struct HPBarUI {
        Character* character;
        Math::vec2 screen_position;
        float current_hp_ratio;  // 0.0 ~ 1.0
        float target_hp_ratio;   // 애니메이션용
    };
    std::vector<HPBarUI> hp_bars_;

    struct ActionLogEntry {
        std::string message;
        double lifetime;
        Math::vec2 position;
    };
    std::vector<ActionLogEntry> action_log_;

    struct TurnOrderUI {
        std::vector<Character*> turn_order;
        int current_turn_index;
    };
    TurnOrderUI turn_order_ui_;

    // 렌더링 헬퍼
    void DrawHPBars(Math::TransformationMatrix camera_matrix);
    void DrawActionLog(Math::TransformationMatrix camera_matrix);
    void DrawTurnOrder(Math::TransformationMatrix camera_matrix);
    void DrawAPDisplay(Character* character, Math::TransformationMatrix camera_matrix);
};
```

**구현 예시**:

```cpp
// GameUIManager.cpp
#include "pch.h"
#include "GameUIManager.h"
#include "../StateComponents/EventBus.h"
#include "../Objects/Character.h"
#include "Engine/TextManager.hpp"
#include "CS200/IRenderer2D.hpp"

GameUIManager::GameUIManager() {
    // EventBus 구독
    EventBus::Instance().Subscribe<TurnStartedEvent>(
        [this](const TurnStartedEvent& event) {
            this->OnTurnStarted(event);
        }
    );

    EventBus::Instance().Subscribe<CharacterDamagedEvent>(
        [this](const CharacterDamagedEvent& event) {
            this->OnCharacterDamaged(event);
        }
    );

    // ... 다른 이벤트 구독
}

void GameUIManager::OnTurnStarted(const TurnStartedEvent& event) {
    // 액션 로그에 추가
    ActionLogEntry entry;
    entry.message = event.character->TypeName() + "'s Turn";
    entry.lifetime = 3.0;  // 3초간 표시
    entry.position = Math::vec2{50, 100};
    action_log_.push_back(entry);

    // 턴 순서 업데이트
    turn_order_ui_.current_turn_index = event.turn_index;
}

void GameUIManager::OnCharacterDamaged(const CharacterDamagedEvent& event) {
    // HP 바 업데이트
    for (auto& hp_bar : hp_bars_) {
        if (hp_bar.character == event.target) {
            float new_ratio = static_cast<float>(event.new_hp) / event.target->GetMaxHP();
            hp_bar.target_hp_ratio = new_ratio;
        }
    }

    // 데미지 텍스트 표시
    ActionLogEntry entry;
    entry.message = event.target->TypeName() + " took " + std::to_string(event.damageAmount) + " damage!";
    entry.lifetime = 2.0;
    entry.position = Math::vec2{50, 150};
    action_log_.push_back(entry);
}

void GameUIManager::Update(double dt) {
    // HP 바 애니메이션 (부드럽게 감소)
    for (auto& hp_bar : hp_bars_) {
        if (hp_bar.current_hp_ratio != hp_bar.target_hp_ratio) {
            float diff = hp_bar.target_hp_ratio - hp_bar.current_hp_ratio;
            hp_bar.current_hp_ratio += diff * 5.0f * static_cast<float>(dt);  // 부드러운 전환
        }
    }

    // 액션 로그 lifetime 감소
    for (auto& entry : action_log_) {
        entry.lifetime -= dt;
    }

    // 만료된 로그 제거
    action_log_.erase(
        std::remove_if(action_log_.begin(), action_log_.end(),
            [](const ActionLogEntry& entry) { return entry.lifetime <= 0; }),
        action_log_.end()
    );
}

void GameUIManager::Draw(Math::TransformationMatrix camera_matrix) {
    DrawHPBars(camera_matrix);
    DrawActionLog(camera_matrix);
    DrawTurnOrder(camera_matrix);

    // 현재 턴 캐릭터의 AP 표시
    if (turn_order_ui_.current_turn_index >= 0 &&
        turn_order_ui_.current_turn_index < turn_order_ui_.turn_order.size()) {
        Character* current = turn_order_ui_.turn_order[turn_order_ui_.current_turn_index];
        DrawAPDisplay(current, camera_matrix);
    }
}
```

**HP 바 렌더링**:

```cpp
void GameUIManager::DrawHPBars(Math::TransformationMatrix camera_matrix) {
    auto* renderer = CS200::IRenderer2D::GetActiveRenderer();

    for (const auto& hp_bar : hp_bars_) {
        // 캐릭터 위치 가져오기
        Math::vec2 char_pos = hp_bar.character->GetPosition();

        // HP 바 위치 (캐릭터 위 50px)
        Math::vec2 bar_pos = char_pos + Math::vec2{0, 50};

        // 배경 (빨간색)
        renderer->DrawRect(
            bar_pos,
            Math::vec2{60, 8},  // 너비 60px, 높이 8px
            0.0f,  // 회전 없음
            CS200::RGBA{200, 0, 0, 255}  // 빨간색
        );

        // HP 바 (녹색)
        float bar_width = 60.0f * hp_bar.current_hp_ratio;
        renderer->DrawRect(
            bar_pos,
            Math::vec2{bar_width, 8},
            0.0f,
            CS200::RGBA{0, 200, 0, 255}  // 녹색
        );

        // HP 텍스트 (숫자)
        int current_hp = hp_bar.character->GetCurrentHP();
        int max_hp = hp_bar.character->GetMaxHP();
        std::string hp_text = std::to_string(current_hp) + "/" + std::to_string(max_hp);

        Engine::GetTextManager().Draw(
            hp_text,
            bar_pos + Math::vec2{70, -3},  // 바 옆에 표시
            Math::vec2{1.0f, 1.0f}  // 크기
        );
    }
}
```

---

**Task 2: 액션 로그 시스템** (Day 2)

```cpp
void GameUIManager::DrawActionLog(Math::TransformationMatrix camera_matrix) {
    float y_offset = 100.0f;

    for (const auto& entry : action_log_) {
        // 페이드 아웃 효과 (lifetime에 따라)
        int alpha = static_cast<int>(entry.lifetime * 127.5f);  // 0 ~ 255
        alpha = std::min(alpha, 255);

        Engine::GetTextManager().Draw(
            entry.message,
            entry.position + Math::vec2{0, y_offset},
            Math::vec2{1.5f, 1.5f},  // 큰 텍스트
            CS200::RGBA{255, 255, 255, static_cast<unsigned char>(alpha)}
        );

        y_offset += 30.0f;
    }
}
```

---

**Task 3: 턴 순서 패널** (Day 2-3)

```cpp
void GameUIManager::DrawTurnOrder(Math::TransformationMatrix camera_matrix) {
    auto* renderer = CS200::IRenderer2D::GetActiveRenderer();

    Math::vec2 panel_pos{10, Engine::GetWindow().GetSize().y - 100};  // 좌측 상단

    // 패널 배경
    renderer->DrawRect(
        panel_pos,
        Math::vec2{200, 80},
        0.0f,
        CS200::RGBA{50, 50, 50, 200}  // 반투명 회색
    );

    // 턴 순서 텍스트
    Engine::GetTextManager().Draw(
        "Turn Order:",
        panel_pos + Math::vec2{10, 60},
        Math::vec2{1.2f, 1.2f}
    );

    // 캐릭터 목록
    float x_offset = 10.0f;
    for (size_t i = 0; i < turn_order_ui_.turn_order.size(); ++i) {
        Character* character = turn_order_ui_.turn_order[i];

        // 현재 턴 캐릭터는 하이라이트
        CS200::RGBA color = (i == turn_order_ui_.current_turn_index) ?
            CS200::RGBA{255, 255, 0, 255} :  // 노란색
            CS200::RGBA{200, 200, 200, 255};  // 회색

        std::string name = character->TypeName().substr(0, 3);  // "Dra", "Fig" 등
        Engine::GetTextManager().Draw(
            name,
            panel_pos + Math::vec2{x_offset, 30},
            Math::vec2{1.0f, 1.0f},
            color
        );

        x_offset += 50.0f;
    }
}
```

---

#### **Part B: DebugUIManager - 개발자용** (Day 3-5)

**목표**: ImGui 기반 디버그 패널

```cpp
// DebugUIManager.h
#pragma once
#include "Engine/Component.h"
#include <imgui.h>

class DebugUIManager : public CS230::Component {
public:
    DebugUIManager();

    void DrawImGui();  // ImGui 렌더링

    // 토글 플래그
    bool show_grid_info = false;
    bool show_ai_debug = false;
    bool show_performance = false;
    bool show_event_log = false;

private:
    void DrawGridInfoPanel();
    void DrawAIDebugPanel();
    void DrawPerformancePanel();
    void DrawEventLogPanel();

    // 성능 메트릭
    float fps_ = 0.0f;
    size_t memory_usage_ = 0;

    // 이벤트 로그
    std::vector<std::string> event_log_;
    const size_t max_log_entries_ = 100;
};
```

**ImGui 패널 구현**:

```cpp
void DebugUIManager::DrawImGui() {
    // F1: Grid Info
    if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
        show_grid_info = !show_grid_info;
    }

    // F4: AI Debug
    if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
        show_ai_debug = !show_ai_debug;
    }

    // 패널 렌더링
    if (show_grid_info) {
        DrawGridInfoPanel();
    }

    if (show_ai_debug) {
        DrawAIDebugPanel();
    }

    if (show_performance) {
        DrawPerformancePanel();
    }

    if (show_event_log) {
        DrawEventLogPanel();
    }
}

void DebugUIManager::DrawGridInfoPanel() {
    ImGui::Begin("Grid Information", &show_grid_info);

    GridSystem* grid = GetGSComponent<GridSystem>();

    // 마우스 위치 타일 좌표
    Math::vec2 mouse_pos = Engine::GetInput().GetMousePos();
    Math::ivec2 tile_pos = grid->ScreenToGrid(mouse_pos);

    ImGui::Text("Mouse Tile: (%d, %d)", tile_pos.x, tile_pos.y);

    // 타일 점유 상태
    Character* occupant = grid->GetCharacterAt(tile_pos);
    if (occupant) {
        ImGui::Text("Occupied by: %s", occupant->TypeName().c_str());
        ImGui::Text("HP: %d/%d", occupant->GetCurrentHP(), occupant->GetMaxHP());
    } else {
        ImGui::Text("Empty");
    }

    // 타일 타입
    bool walkable = grid->IsWalkable(tile_pos);
    ImGui::Text("Walkable: %s", walkable ? "Yes" : "No");

    ImGui::End();
}

void DebugUIManager::DrawAIDebugPanel() {
    ImGui::Begin("AI Debug", &show_ai_debug);

    AISystem* ai_system = GetGSComponent<AISystem>();

    // 현재 AI 결정 정보
    ImGui::Text("Current AI Decision:");
    // ... AI 결정 트리 시각화 ...

    // Bias 조정
    if (ImGui::CollapsingHeader("AI Bias Tuning")) {
        // ... Bias 슬라이더 ...
    }

    ImGui::End();
}
```

---

### 엄격한 테스트 (UI 시스템)

**테스트 시나리오**:

1. **HP 바 테스트**: 데미지 받을 때 부드럽게 감소하는가?
2. **액션 로그 테스트**: 이벤트 발생 시 로그가 표시되는가?
3. **턴 순서 패널 테스트**: 현재 턴 캐릭터가 하이라이트되는가?
4. **디버그 UI 토글 테스트**: F-키로 패널이 켜지고 꺼지는가?

---

### 일일 작업 분배 (개발자 C)

| 일차      | 작업                  | 예상 시간 | 산출물                 |
| ------- | ------------------- | ----- | ------------------- |
| Day 1   | GameUIManager 기본 구조 | 4h    | GameUIManager.h/cpp |
| Day 1-2 | HP 바 시스템            | 4h    | DrawHPBars 완성       |
| Day 2   | 액션 로그 시스템           | 4h    | DrawActionLog 완성    |
| Day 2-3 | 턴 순서 패널             | 4h    | DrawTurnOrder 완성    |
| Day 3   | AP 표시               | 2h    | DrawAPDisplay 완성    |
| Day 3-4 | DebugUIManager 구현   | 6h    | ImGui 패널들           |
| Day 4-5 | 통합 및 테스트            | 8h    | 전체 UI 테스트           |

**총 예상 시간**: 32시간 (4일)

---

## 개발자 D: GameObjectManager Smart Pointer 전환

**목표**: GameObjectManager를 raw pointer에서 `std::unique_ptr`로 변환하여 명확한 소유권 관리 및 메모리 안전성 확보

**기초 지식**:
- GameObjectManager는 현재 raw pointer로 GameObject를 관리
- Unload()에서 메모리 누수 발생 (clear()만 호출, delete 없음)
- 소유권이 불명확하여 dangling pointer 위험

**파일 수정 목록**:

```
DragonicTactics/source/Engine/GameObjectManager.h/cpp
DragonicTactics/source/Game/DragonicTactics/Factories/CharacterFactory.h/cpp
DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp
DragonicTactics/source/Game/DragonicTactics/States/BattleOrchestrator.cpp
DragonicTactics/source/Engine/Particle.h
DragonicTactics/source/Game/DragonicTactics/Test/TestMemory.h/cpp (신규)
```

---

### 구현 작업 (Smart Pointer 전환)

#### **Task 1: 설계 결정 및 소유권 다이어그램** (Day 1, 4시간)

**핵심 설계 결정**:

1. **이중 API 전략**
   - `GetAll()`: unique_ptr list const 참조 반환 (내부 사용)
   - `GetAllRaw()`: raw pointer vector 반환 (외부 시스템용)

2. **소유권 모델**
   - **GameObjectManager**: unique_ptr로 소유
   - **비소유 시스템** (TurnManager, GridSystem, Events): raw pointer 참조
   - **캐시된 포인터** (GamePlay.player/enemy): raw pointer, "비소유 캐시"로 문서화

3. **현재 문제 분석**:

```cpp
// GameObjectManager.cpp - 메모리 누수!
void GameObjectManager::Unload() {
    objects.clear();  // ❌ delete 없이 clear만 → 메모리 누수!
}

// GamePlay.cpp - 소유권 불명확
Character* enemy = CharacterFactory::Create(...);  // ❌ 누가 소유?
go_manager->Add(enemy);  // ❌ 소유권 이전인가, 공유인가?
```

**목표 구조**:

```
GameObjectManager (소유자)
├── unique_ptr<Dragon>     (소유)
├── unique_ptr<Fighter>    (소유)
└── ...

다른 시스템들 (참조자)
├── GamePlay.player:  Character*           (비소유 캐시)
├── TurnManager:      vector<Character*>   (비소유 참조)
├── GridSystem:       Character*[8][8]     (비소유 참조)
└── Events:           vector<Character*>   (비소유 참조)
```

---

#### **Task 2: GameObjectManager 헤더 및 구현 수정** (Day 1-2, 8시간)

**Step 1: GameObjectManager.h 수정**

```cpp
// GameObjectManager.h
namespace CS230 {
    class GameObjectManager : public CS230::Component {
    public:
        // NEW: unique_ptr로 소유권 이전 (move semantics)
        void Add(std::unique_ptr<GameObject> object);

        // NEW: unique_ptr list 반환 (내부 반복용)
        const std::list<std::unique_ptr<GameObject>>& GetAll() const {
            return objects;
        }

        // NEW: 비소유 참조자를 위한 헬퍼
        std::vector<GameObject*> GetAllRaw() const;

    private:
        std::list<std::unique_ptr<GameObject>> objects;  // CHANGED
    };
}
```

**Step 2: GameObjectManager.cpp 수정**

핵심 변경사항:
- `Add()`: `std::move()` 사용하여 소유권 이전
- `Unload()`: `clear()` → unique_ptr 자동 삭제로 메모리 누수 수정
- `UpdateAll()`: iterator 패턴으로 안전한 삭제
- `CollisionTest()`: unique_ptr 순회로 수정, `.get()`으로 raw pointer 전달
- `GetAllRaw()`: 새 헬퍼 메서드 구현

```cpp
void GameObjectManager::Add(std::unique_ptr<GameObject> object) {
    objects.emplace_back(std::move(object));
}

void GameObjectManager::Unload() {
    objects.clear();  // ✅ unique_ptr가 자동 삭제 (누수 수정!)
}

void GameObjectManager::UpdateAll(double dt) {
    // 안전한 삭제를 위한 iterator 수집
    std::vector<std::list<std::unique_ptr<GameObject>>::iterator> destroy_iterators;

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        (*it)->Update(dt);
        if ((*it)->Destroyed()) {
            destroy_iterators.push_back(it);
        }
    }

    for (auto it : destroy_iterators) {
        objects.erase(it);  // unique_ptr 소멸자가 delete 처리
    }
}

void GameObjectManager::CollisionTest() {
    // CHANGED: unique_ptr 순회, .get()으로 raw pointer 전달
    for (const auto& object1 : objects) {
        for (const auto& object2 : objects) {
            if (object1.get() != object2.get() && object1->CanCollideWith(object2->Type())) {
                if (object1->IsCollidingWith(object2.get())) {
                    Engine::GetLogger().LogEvent("Collision Detected: " +
                        object1->TypeName() + " and " + object2->TypeName());
                    object1->ResolveCollision(object2.get());
                }
            }
        }
    }
}

std::vector<GameObject*> GameObjectManager::GetAllRaw() const {
    std::vector<GameObject*> raw_pointers;
    raw_pointers.reserve(objects.size());
    for (const auto& obj_ptr : objects) {
        raw_pointers.push_back(obj_ptr.get());
    }
    return raw_pointers;
}
```

---

#### **Task 3: CharacterFactory 수정** (Day 2, 4시간)

**CharacterFactory.h**:

```cpp
class CharacterFactory {
public:
    // CHANGED: unique_ptr 반환으로 소유권 이전 표현
    static std::unique_ptr<Character> Create(
        CharacterTypes type,
        Math::ivec2 start_position
    );

private:
    static std::unique_ptr<Dragon> CreateDragon(Math::ivec2 position);
    static std::unique_ptr<Fighter> CreateFighter(Math::ivec2 position);
};
```

**CharacterFactory.cpp 구현 패턴**:

```cpp
std::unique_ptr<Dragon> CharacterFactory::CreateDragon(Math::ivec2 position) {
    std::unique_ptr<Dragon> dragon = std::make_unique<Dragon>(position);

    // 컴포넌트 추가...

    return dragon;  // move semantics
}
```

---

#### **Task 4: 호출 지점 업데이트** (Day 3, 8시간)

**GamePlay.cpp - 캐시-then-move 패턴**:

```cpp
// GamePlay.cpp (lines 83-95)
case 'f':
    grid_system->SetTileType(current_pos, GridSystem::TileType::Empty);
    {
        auto enemy_ptr = CharacterFactory::Create(CharacterTypes::Fighter, current_pos);
        enemy = enemy_ptr.get();  // 비소유 캐시 (move 전에!)
        enemy->SetGridSystem(grid_system);
        go_manager->Add(std::move(enemy_ptr));  // 소유권 이전
        grid_system->AddCharacter(enemy, current_pos);
    }
    break;
```

**BattleOrchestrator.cpp - GetAllRaw() 사용**:

```cpp
bool BattleOrchestrator::ShouldContinueTurn(...) {
    // CHANGED: GetAllRaw() 사용 (비소유 반복)
    std::vector<CS230::GameObject*> objects = go_manager->GetAllRaw();

    for (const auto& obj_ptr : objects) {
        if (obj_ptr->Type() == GameObjectTypes::Character) {
            Character* character = static_cast<Character*>(obj_ptr);
            // 처리...
        }
    }
}
```

**Particle.h - 엣지 케이스**:

```cpp
for (int i = 0; i < T::MaxCount; ++i) {
    std::unique_ptr<T> new_particle = std::make_unique<T>();
    T* particle_ptr = new_particle.get();  // 로컬 캐시

    go_manager->Add(std::move(new_particle));  // 소유권 이전
    particles.push_back(particle_ptr);  // 비소유 참조 저장
}
```

---

#### **Task 5: 테스트 및 검증** (Day 3-4, 4시간)

**TestMemory.cpp 작성**:

```cpp
void TestMemory::TestOwnershipTransfer() {
    auto go_manager = std::make_unique<GameObjectManager>();

    auto character = CharacterFactory::Create(CharacterTypes::Dragon, {0, 0});
    Character* raw_ptr = character.get();

    go_manager->Add(std::move(character));

    // Verify: character는 nullptr
    assert(character == nullptr);

    // Verify: raw_ptr은 여전히 유효
    assert(raw_ptr != nullptr);

    // Verify: GetAllRaw()에 포함됨
    auto objects = go_manager->GetAllRaw();
    assert(objects.size() == 1);
    assert(objects[0] == raw_ptr);
}

void TestMemory::TestUnloadNoLeak() {
    auto go_manager = std::make_unique<GameObjectManager>();

    // 5개 캐릭터 추가
    for (int i = 0; i < 5; ++i) {
        auto character = CharacterFactory::Create(CharacterTypes::Fighter, {i, 0});
        go_manager->Add(std::move(character));
    }

    assert(go_manager->GetAllRaw().size() == 5);

    // Unload → unique_ptr가 자동 삭제
    go_manager->Unload();

    assert(go_manager->GetAllRaw().size() == 0);
    // ✅ 메모리 누수 없음!
}
```

**빌드 및 통합 테스트**:

```bash
cd DragonicTactics
cmake --preset windows-debug
cmake --build --preset windows-debug

# 실행 테스트
build/windows-debug/dragonic_tactics.exe
```

**체크리스트**:
- [ ] 캐릭터 생성 정상 작동
- [ ] 턴 시스템 정상 작동
- [ ] 전투 시스템 정상 작동
- [ ] 캐릭터 사망 시 정리 확인
- [ ] Unload 후 메모리 누수 없음 (Visual Studio Profiler)
- [ ] GamePlay 종료 시 크래시 없음

---

### 일일 작업 분배 (개발자 D)

| 일차      | 작업                   | 예상 시간 | 산출물                  |
| ------- | -------------------- | ----- | -------------------- |
| Day 1   | 소유권 분석 및 다이어그램       | 4h    | ownership-diagram.md |
| Day 2   | GameObjectManager 수정 | 4h    | unique_ptr 적용        |
| Day 3   | CharacterFactory 수정  | 4h    | unique_ptr 반환        |
| Day 4   | GamePlay 수정          | 4h    | 소유권 이전 코드            |
| Day 4-5 | 메모리 테스트              | 8h    | TestMemory.cpp, 레포트  |

**총 예상 시간**: 24시간 (3일)

---

## 개발자 E: AI 행동 시각화 및 맵 데이터 로딩

**목표**: AI pause 시스템 + JSON 맵 로딩

**파일 목록**:

```
DragonicTactics/source/Game/DragonicTactics/StateComponents/AIPauseSystem.h/cpp (신규)
DragonicTactics/source/Game/DragonicTactics/StateComponents/MapDataRegistry.h/cpp (신규)
DragonicTactics/Assets/Data/maps.json (신규)
DragonicTactics/source/Game/DragonicTactics/Test/TestMapLoading.h/cpp (신규)
```

---

### 구현 작업 (AI 행동 시각화)

#### **Task 1: AI Pause 시스템** (Day 1-3)

**목표**: AI 행동 중간에 pause를 추가하여 플레이어가 상황 파악 가능

```cpp
// AIPauseSystem.h
class AIPauseSystem : public CS230::Component {
public:
    void Update(double dt) override;

    // AI 행동 단계별 pause
    void PauseBeforeAction(Character* ai_character, const std::string& action_description);
    void PauseAfterAction(Character* ai_character);

    // Pause 시간 설정
    void SetPauseDuration(double seconds);
    double GetPauseDuration() const { return pause_duration_; }

    bool IsPaused() const { return is_paused_; }

private:
    bool is_paused_ = false;
    double pause_timer_ = 0.0;
    double pause_duration_ = 1.0;  // 기본 1초

    std::string current_action_description_;
    Character* current_ai_character_ = nullptr;
};
```

**사용 예시**:

```cpp
// AISystem.cpp
void AISystem::ExecuteFighterAI(Character* fighter) {
    AIPauseSystem* pause_system = GetGSComponent<AIPauseSystem>();

    // 1. 행동 전 pause (플레이어가 "Fighter가 뭔가 하려고 한다"는 것을 인지)
    pause_system->PauseBeforeAction(fighter, "Fighter is thinking...");

    // 대기 중...
    if (pause_system->IsPaused()) {
        return;  // 다음 프레임에 계속
    }

    // 2. 행동 결정
    AIAction action = DecideAction(fighter);

    // 3. 행동 실행 전 pause (플레이어가 행동 내용을 인지)
    std::string action_desc = "Fighter will " + action.reasoning;
    pause_system->PauseBeforeAction(fighter, action_desc);

    if (pause_system->IsPaused()) {
        return;
    }

    // 4. 행동 실행
    ExecuteAction(fighter, action);

    // 5. 행동 후 pause (결과를 확인할 시간)
    pause_system->PauseAfterAction(fighter);
}
```

**UI 표시** (GameUIManager 연동):

```cpp
// GameUIManager.cpp - AI 행동 표시
void GameUIManager::DrawAIActionIndicator() {
    AIPauseSystem* pause_system = GetGSComponent<AIPauseSystem>();

    if (pause_system->IsPaused()) {
        std::string action_desc = pause_system->GetCurrentActionDescription();

        // 화면 중앙에 큰 텍스트로 표시
        Math::vec2 screen_center = Engine::GetWindow().GetSize() / 2.0f;

        Engine::GetTextManager().Draw(
            action_desc,
            screen_center,
            Math::vec2{2.0f, 2.0f},  // 큰 크기
            CS200::RGBA{255, 255, 0, 255}  // 노란색
        );
    }
}
```

---

### 구현 작업 (맵 데이터 로딩)

#### **Task 2: maps.json 스키마 설계** (Day 3)

```json
// DragonicTactics/Assets/Data/maps.json
{
    "maps": [
        {
            "id": "arena_01",
            "name": "Basic Arena",
            "width": 8,
            "height": 8,
            "tiles": [
                "########",
                "#......#",
                "#......#",
                "#...W..#",
                "#...W..#",
                "#......#",
                "#......#",
                "########"
            ],
            "legend": {
                "#": "wall",
                ".": "floor",
                "W": "water",
                "L": "lava"
            },
            "spawn_points": {
                "dragon": {"x": 4, "y": 4},
                "fighter": {"x": 1, "y": 1},
                "cleric": {"x": 6, "y": 1},
                "wizard": {"x": 6, "y": 6},
                "rogue": {"x": 1, "y": 6}
            }
        },
        {
            "id": "lava_chamber",
            "name": "Lava Chamber",
            "width": 8,
            "height": 8,
            "tiles": [
                "########",
                "#......#",
                "#.LLLL.#",
                "#.LLLL.#",
                "#.LLLL.#",
                "#.LLLL.#",
                "#......#",
                "########"
            ],
            "spawn_points": {
                "dragon": {"x": 2, "y": 2},
                "fighter": {"x": 5, "y": 5}
            }
        }
    ]
}
```

---

#### **Task 3: MapDataRegistry 구현** (Day 3-4)

```cpp
// MapDataRegistry.h
struct MapData {
    std::string id;
    std::string name;
    int width;
    int height;
    std::vector<std::string> tiles;
    std::map<char, std::string> legend;  // 타일 기호 → 타일 타입
    std::map<std::string, Math::ivec2> spawn_points;
};

class MapDataRegistry : public CS230::Component {
public:
    void LoadMaps(const std::string& json_path);
    MapData GetMapData(const std::string& map_id) const;
    std::vector<std::string> GetAllMapIds() const;

private:
    std::map<std::string, MapData> maps_;
};
```

**구현**:

```cpp
// MapDataRegistry.cpp
#include "pch.h"
#include "MapDataRegistry.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

void MapDataRegistry::LoadMaps(const std::string& json_path) {
    Engine::GetLogger().LogEvent("MapDataRegistry: Loading " + json_path);

    std::ifstream file(json_path);
    if (!file.is_open()) {
        Engine::GetLogger().LogError("Failed to open " + json_path);
        return;
    }

    json j;
    file >> j;

    for (const auto& map_json : j["maps"]) {
        MapData map_data;
        map_data.id = map_json["id"];
        map_data.name = map_json["name"];
        map_data.width = map_json["width"];
        map_data.height = map_json["height"];

        // 타일 데이터
        for (const auto& row : map_json["tiles"]) {
            map_data.tiles.push_back(row);
        }

        // 범례
        for (const auto& [key, value] : map_json["legend"].items()) {
            map_data.legend[key[0]] = value;
        }

        // 스폰 포인트
        for (const auto& [char_type, pos] : map_json["spawn_points"].items()) {
            Math::ivec2 spawn_pos{pos["x"], pos["y"]};
            map_data.spawn_points[char_type] = spawn_pos;
        }

        maps_[map_data.id] = map_data;
        Engine::GetLogger().LogEvent("Loaded map: " + map_data.id);
    }
}

MapData MapDataRegistry::GetMapData(const std::string& map_id) const {
    auto it = maps_.find(map_id);
    if (it != maps_.end()) {
        return it->second;
    }

    Engine::GetLogger().LogError("Map not found: " + map_id);
    return MapData{};
}
```

---

#### **Task 4: GridSystem에 맵 적용** (Day 4-5)

```cpp
// GridSystem.h
class GridSystem : public CS230::Component {
public:
    void LoadMap(const MapData& map_data);

    // 타일 타입 쿼리
    std::string GetTileType(Math::ivec2 tile_pos) const;
    bool IsWalkable(Math::ivec2 tile_pos) const override;

private:
    std::map<Math::ivec2, std::string> tile_types_;  // 위치 → 타입 ("wall", "floor", "lava")
};
```

**구현**:

```cpp
void GridSystem::LoadMap(const MapData& map_data) {
    Engine::GetLogger().LogEvent("GridSystem: Loading map " + map_data.id);

    tile_types_.clear();

    // 타일 데이터 파싱
    for (int y = 0; y < map_data.height; ++y) {
        const std::string& row = map_data.tiles[y];

        for (int x = 0; x < map_data.width; ++x) {
            char tile_char = row[x];

            // 범례에서 타일 타입 조회
            auto it = map_data.legend.find(tile_char);
            if (it != map_data.legend.end()) {
                Math::ivec2 pos{x, y};
                tile_types_[pos] = it->second;
            }
        }
    }

    Engine::GetLogger().LogEvent("GridSystem: Loaded " + std::to_string(tile_types_.size()) + " tiles");
}

bool GridSystem::IsWalkable(Math::ivec2 tile_pos) const {
    auto it = tile_types_.find(tile_pos);
    if (it == tile_types_.end()) {
        return false;  // 맵 밖
    }

    const std::string& tile_type = it->second;
    return (tile_type == "floor" || tile_type == "water");  // wall, lava는 불가
}
```

---

#### **Task 5: GamePlay에서 맵 로딩** (Day 5)

```cpp
// GamePlay.cpp
void GamePlay::Load() {
    // 맵 데이터 로드
    auto* map_registry = AddGSComponent(new MapDataRegistry());
    map_registry->LoadMaps("Assets/Data/maps.json");

    // 맵 선택
    MapData arena_map = map_registry->GetMapData("arena_01");

    // GridSystem에 맵 적용
    auto* grid_system = GetGSComponent<GridSystem>();
    grid_system->LoadMap(arena_map);

    // 스폰 포인트에서 캐릭터 생성
    Math::ivec2 dragon_spawn = arena_map.spawn_points["dragon"];
    auto dragon = CharacterFactory::CreateDragon(dragon_spawn);
    // ...

    Engine::GetLogger().LogEvent("Map loaded: " + arena_map.name);
}
```

---

### 일일 작업 분배 (개발자 E)

| 일차      | 작업                 | 예상 시간 | 산출물                 |
| ------- | ------------------ | ----- | ------------------- |
| Day 1-2 | AIPauseSystem 구현   | 8h    | AIPauseSystem.cpp   |
| Day 2-3 | AI 행동 UI 표시        | 4h    | GameUIManager 연동    |
| Day 3   | maps.json 스키마 설계   | 2h    | maps.json (2개 맵)    |
| Day 3-4 | MapDataRegistry 구현 | 6h    | MapDataRegistry.cpp |
| Day 4-5 | GridSystem 맵 로딩    | 6h    | GridSystem::LoadMap |
| Day 5   | 통합 테스트             | 6h    | TestMapLoading.cpp  |

**총 예상 시간**: 32시간 (4일)

---

## Week 5 통합 테스트

**금요일 오후 (2-3시간)**

### 통합 테스트 시나리오

**시나리오 1: 완전한 전투 플레이**

```
1. 게임 실행 → "arena_01" 맵 로드
2. Dragon vs Fighter 배치 (스폰 포인트에서)
3. 턴 1 (Dragon):
   - GameUI: HP 바, AP 표시, 턴 순서 확인
   - 플레이어가 이동 또는 공격
   - 로그: 모든 턴 체크리스트 함수 호출 확인
4. 턴 2 (Fighter):
   - AI Pause: "Fighter is thinking..." 표시 (1초)
   - AI Pause: "Fighter will move closer" 표시 (1초)
   - Fighter가 Dragon에게 접근
   - GameUI: Fighter 행동 로그 표시
5. 반복 (승자 결정까지)
```

**검증 항목**:

- [ ] 맵이 JSON에서 로드됨
- [ ] 캐릭터가 스폰 포인트에 생성됨
- [ ] 턴 플로우 체크리스트 모두 실행됨 (로그 확인)
- [ ] GameUI: HP 바가 부드럽게 감소
- [ ] GameUI: 액션 로그가 표시됨
- [ ] GameUI: 턴 순서 패널이 업데이트됨
- [ ] AI가 자율적으로 행동
- [ ] AI pause가 작동 (행동 표시)
- [ ] 메모리 누수 없음 (Visual Studio Profiler)
- [ ] 크래시 없음

---

**시나리오 2: 디버그 UI 테스트**

```
1. F1 키 → Grid Info 패널 표시
2. 마우스 호버 → 타일 좌표 표시
3. F4 키 → AI Debug 패널 표시
4. Fighter 턴 → AI 결정 트리 시각화
5. ImGui Bias 슬라이더 → 실시간 AI 조정
```

---

## Week 5 산출물 및 검증

### 최종 산출물 목록

**문서**:

- [ ] turn-flow-chart.md (턴 플로우 차트)
- [ ] ai-decision-tree.md (AI 결정 트리)
- [ ] ownership-diagram.md (소유권 다이어그램)
- [ ] maps.json (2개 이상의 맵)

**코드**:

- [ ] TurnManager.cpp (체크리스트 적용)
- [ ] AISystem.cpp (robust AI)
- [ ] GameUIManager.cpp (게임 UI)
- [ ] DebugUIManager.cpp (디버그 UI)
- [ ] GameObjectManager.cpp (스마트 포인터)
- [ ] CharacterFactory.cpp (unique_ptr 반환)
- [ ] AIPauseSystem.cpp (AI pause)
- [ ] MapDataRegistry.cpp (맵 로딩)

**테스트**:

- [ ] TestTurnFlow.cpp
- [ ] TestAI.cpp
- [ ] TestMemory.cpp
- [ ] TestMapLoading.cpp

### 검증 체크리스트

**기능 검증**:

- [ ] 턴 시스템이 안정적으로 작동 (함수 호출 로그 확인)
- [ ] AI가 자율적으로 전투 (플레이어 개입 없이)
- [ ] 게임 UI로 모든 정보 확인 가능
- [ ] 디버그 UI로 개발 정보 확인 가능
- [ ] AI pause로 행동 파악 가능
- [ ] 맵이 JSON에서 로드됨

**품질 검증**:

- [ ] 메모리 누수 없음 (Visual Studio Memory Profiler)
- [ ] 크래시 없음 (10분 플레이 테스트)
- [ ] 로그에 오류 없음
- [ ] 성능 문제 없음 (60 FPS 유지)

---

**최종 업데이트**: 2025-11-27
**다음 단계**: Week 5 완료 후 우선순위 재논의 (Week 6 계획)
