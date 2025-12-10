# Dragonic Tactics - 시스템 아키텍처

> **최종 업데이트**: 2025-12-09
> **프로젝트 기간**: 1년 (GAM200: 2025 가을학기, GAM250: 2026 봄학기)
> **개발 팀**: 5명
> **엔진**: 커스텀 C++20 OpenGL 엔진

---

## 📋 목차

1. [프로젝트 개요](#프로젝트-개요)
2. [게임 컨셉](#게임-컨셉)
3. [시스템 아키텍처](#시스템-아키텍처)
4. [구현 상태 (GAM200)](#구현-상태-gam200)
5. [계획 시스템 (GAM250)](#계획-시스템-gam250)
6. [아키텍처 원칙](#아키텍처-원칙)

---

## 프로젝트 개요

**Dragonic Tactics**는 전통적인 RPG의 '영웅'들을 상대로 자신의 영역을 지켜내는 '드래곤'이 되어 즐기는 D&D 스타일 턴제 전술 RPG입니다.

### 핵심 디자인 목표

1. **전략적 위치 선정**: 지형을 만들고 없애면서 전투를 유리하게 이끄는 재미
2. **자원 관리**: 한정된 행동력과 주문슬롯을 효율적으로 사용해서 까다로운 적을 상대하는 재미
3. **비대칭적 전투**: 강력한 드래곤을 사용하는 만큼 플레이어에게 유리한 전투를 하는 재미

### 기술 스택

- **언어**: C++20
- **빌드 시스템**: CMake 3.21+
- **그래픽 API**: OpenGL (Desktop), OpenGL ES (Web)
- **플랫폼**: Windows (MSVC), Linux, WebAssembly (Emscripten)
- **외부 라이브러리**: SDL2, GLEW, Dear ImGui, nlohmann/json, GSL, STB

---

## 게임 컨셉

### 승리/패배 조건

**승리 조건**:
- 모든 모험가 캐릭터 섬멸

**패배 조건**:
- 드래곤(플레이어)의 체력이 0이 됨
- 모험가가 보물을 획득하여 출구로 탈출

### 턴 시스템

1. **시작 페이즈**: 모든 캐릭터의 속력(Speed) 비교로 턴 순서 결정
2. **캐릭터 턴**:
   - 턴 시작 시 행동력(ActionPoints)과 이동력(MovementRange) 갱신
   - 수행 가능 행동: 이동(MovementRange 소모), 공격(ActionPoints 소모), 주문 사용(ActionPoints + SpellSlot 소모)
   - 모든 행동력을 사용하거나 "턴 종료" 선택 시 다음 캐릭터로 넘어감
3. **종료 페이즈**: 턴 종료 시 발동하는 효과(독, 화상 등) 처리

### 캐릭터 시스템

모든 캐릭터는 공통적으로 다음 속성을 가집니다:

- **체력(HP)**: 최대값/현재값, 0이 되면 전투불능
- **속력(Speed)**: 턴 순서 결정 + 한 턴에 이동 가능한 타일 수
- **공격력**: 기본 공격력 + 변동 공격력 (D&D 주사위 시스템, 예: 3d6)
- **방어력**: 피해 감소 (공격력과 동일한 주사위 방식)
- **행동력(ActionPoints)**: 턴당 수행 가능한 행동 횟수
- **주문 슬롯(SpellSlots)**: 레벨 1-9까지의 주문 사용 자원, 업캐스팅 가능
- **바라보는 방향**: 동서남북, 마지막 행동 기준으로 갱신
- **공격 범위**: 공격 가능 거리

---

## 시스템 아키텍처

### 아키텍처 개요

프로젝트는 **컴포넌트 기반 아키텍처**와 **이벤트 기반 통신**을 중심으로 설계되었습니다.

```
┌─────────────────────────────────────────────────────────────┐
│                    GameState (GamePlay)                      │
│  ┌───────────────────────────────────────────────────────┐  │
│  │          GameState Components (전투 시스템)            │  │
│  │  EventBus, GridSystem, TurnManager, CombatSystem,     │  │
│  │  AISystem, DataRegistry, DiceManager, etc.            │  │
│  │                                                         │  │
│  │  접근 방법: GetGSComponent<T>()                        │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                               │
│  ┌───────────────────────────────────────────────────────┐  │
│  │           GameObject (Character 등)                    │  │
│  │  ┌─────────────────────────────────────────────────┐  │  │
│  │  │      GameObject Components (캐릭터 컴포넌트)     │  │  │
│  │  │  GridPosition, StatsComponent, ActionPoints,    │  │  │
│  │  │  SpellSlots, MovementComponent                  │  │  │
│  │  │                                                   │  │  │
│  │  │  접근 방법: GetGOComponent<T>()                  │  │  │
│  │  └─────────────────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                          ▲
                          │ EventBus를 통한 통신
                          ▼
        ┌─────────────────────────────────────────┐
        │   외부 데이터 & 디버그 도구              │
        │   characters.json, maps.json,            │
        │   DebugConsole, DebugVisualizer          │
        └─────────────────────────────────────────┘
```

### 컴포넌트 계층 구조

#### 1. GameState Components (전투 시스템)

GameState에 연결되는 시스템 레벨 컴포넌트:

- `AddGSComponent(new T())`로 추가
- `GetGSComponent<T>()`로 접근
- 전투 흐름, AI, 그리드, 데이터 등을 관리

#### 2. GameObject Components (캐릭터 컴포넌트)

각 캐릭터(GameObject)에 연결되는 엔티티 레벨 컴포넌트:

- `AddGOComponent(new T())`로 추가
- `GetGOComponent<T>()`로 접근
- 개별 캐릭터의 속성과 행동을 관리

---

## 구현 상태 (GAM200)

> **GAM200 학기 (2025 가을)** - ✅ 완료

### ✅ GameState Components (전투 시스템)

GamePlay.cpp에서 실제로 구현되어 사용 중인 시스템:

| 컴포넌트 | 설명 | 접근 방법 |
|---------|------|----------|
| **EventBus** | 시스템 간 이벤트 통신 (타입 안전) | `GetGSComponent<EventBus>()` |
| **DiceManager** | D&D 주사위 굴림 (예: "3d6+5") | `GetGSComponent<DiceManager>()` |
| **DataRegistry** | 캐릭터 JSON 데이터 로딩 | `GetGSComponent<DataRegistry>()` |
| **MapDataRegistry** | 맵 JSON 데이터 로딩 | `GetGSComponent<MapDataRegistry>()` |
| **GridSystem** | 8x8 전술 그리드 + A* 경로 찾기 | `GetGSComponent<GridSystem>()` |
| **TurnManager** | 턴 순서 관리 (OnTurnStart/End) | `GetGSComponent<TurnManager>()` |
| **CombatSystem** | 전투 해결 (공격력/방어력 계산) | `GetGSComponent<CombatSystem>()` |
| **AISystem** | AI 전략 패턴 실행 | `GetGSComponent<AISystem>()` |
| **CharacterFactory** | 캐릭터 생성 (unique_ptr 반환) | `GetGSComponent<CharacterFactory>()` |
| **DebugManager** | 디버그 모드 관리 | `GetGSComponent<DebugManager>()` |
| **GameObjectManager** | GameObject 생명주기 관리 | `GetGSComponent<CS230::GameObjectManager>()` |
| **Timer** | 시간 측정 유틸리티 | `GetGSComponent<util::Timer>()` |

**참고**: `SpellSystem`은 GamePlay.cpp Line 81에서 주석 처리되어 있어 실제 사용되지 않음.

### ✅ GameObject Components (캐릭터 컴포넌트)

각 Character 객체에 연결된 컴포넌트:

| 컴포넌트 | 설명 | 데이터 |
|---------|------|--------|
| **GridPosition** | 그리드 좌표 (Math::ivec2) | 현재 위치 |
| **StatsComponent** | 전투 스탯 | HP, 공격력, 방어력, 속도 |
| **ActionPoints** | 행동력 관리 | 최대/현재 ActionPoints |
| **SpellSlots** | 주문 슬롯 데이터 | 레벨 1-9 슬롯 개수 |
| **MovementComponent** | 이동 애니메이션 | 이동 중 시각적 보간 |

**중요**: `SpellSlots` 컴포넌트는 구현되었지만, 실제 주문 캐스팅 기능은 GAM250에 구현 예정입니다.

### ✅ 캐릭터 시스템

#### 플레이어 캐릭터

**Dragon (드래곤)**
- HP: 140, 속력: 5, 행동력: 2
- 공격력: 3d6, 방어력: 2d8, 공격 범위: 3칸
- SpellSlots: 레벨 1(4개), 2(3개), 3(2개), 4(2개), 5(1개)
- 주문 9개는 GAM250에 구현 예정

#### AI 캐릭터 (모험가)

**Fighter (파이터)**
- HP: 90, 속력: 3, 행동력: 2
- 공격력: 5+2d6, 방어력: 1d10, 공격 범위: 1칸 (근접)
- 역할: 균형잡힌 전사, 드래곤에게 접근하여 어그로 담당
- AI 전략: FighterStrategy ✅ 완료

**나머지 캐릭터 (GAM250 구현 예정)**:
- Cleric (클레릭) - HP 90, 속력 2, 힐러/서포터
- Wizard (위저드) - HP 55, 속력 1, 원거리 공격
- Rogue (로그) - HP 65, 속력 4, 암살자/기습 공격

### ✅ AI 시스템 (Strategy 패턴)

| AI 전략 | 상태 | 설명 |
|--------|------|------|
| **IAIStrategy** | ✅ 완료 | AI 전략 인터페이스 |
| **FighterStrategy** | ✅ 완료 | 전사 AI (보물 운반, HP 관리, 근접 공격) |
| **Fighter AI 플로우차트** | ✅ 완료 | Mermaid 다이어그램 (`architecture/character_flowchart/fighter.mmd`) |
| ClericStrategy | 📋 GAM250 | 성직자 AI (힐링, 버프/디버프) |
| WizardStrategy | 📋 GAM250 | 마법사 AI (원거리 주문 공격) |
| RogueStrategy | 📋 GAM250 | 도적 AI (기습 공격, 보물 훔치기) |
| Cleric/Wizard/Rogue 플로우차트 | 📋 GAM250 | **이미 작성되었지만, 검토가 필요** |

**AI 플로우차트 위치**: `architecture/character_flowchart/*.mmd`

### ✅ 액션 시스템

| 액션 | 상태 | 설명 |
|-----|------|------|
| **Action** | ✅ 완료 | 액션 베이스 클래스 |
| **ActionAttack** | ✅ 완료 | 기본 공격 (공격력 계산 + 데미지 적용) |

### ✅ 디버그 도구

| 도구 | 설명 |
|-----|------|
| **DebugConsole** | 콘솔 명령어 입력 |
| **DebugManager** | 디버그 모드 토글 및 관리 |
| **DebugVisualizer** | 그리드 시각화 (이동 가능 타일, 공격 범위) |
| **__PRETTY_FUNCTION__** | 함수 호출 추적 (Logger와 함께 사용) |

### ✅ 데이터 기반 설계

**JSON 데이터 파일**:
- `Assets/Data/characters.json` - 캐릭터 스탯, 주문 슬롯 정의
- `Assets/Data/maps.json` - 맵 레이아웃, 스폰 위치, 타일 타입

**런타임 리로드**: 게임 실행 중 JSON 파일 수정 후 리로드 가능

### ✅ 턴 플로우 구현

TurnManager가 관리하는 턴 플로우:

1. **InitializeTurnOrder()**: 속력 기준 턴 순서 정렬
2. **StartCombat()**: 전투 시작, 첫 번째 캐릭터 턴 시작
3. **OnTurnStart()** (각 Character에서 호출):
   - ActionPoints 회복
   - MovementRange 회복
   - 턴 시작 이벤트 발행
4. **플레이어/AI 행동**: 이동, 공격, 주문 사용
5. **EndCurrentTurn()**: 현재 캐릭터 턴 종료
6. **OnTurnEnd()** (각 Character에서 호출):
   - 상태 이상 효과 처리 (GAM250에 완전 구현 예정)
   - 턴 종료 이벤트 발행
7. **StartNextTurn()**: 다음 캐릭터로 턴 넘김

### ✅ 이벤트 시스템

EventBus를 통한 시스템 간 통신:

**구현된 이벤트 타입** (GamePlay.cpp에서 실제 사용):

```cpp
// Line 121-125: CharacterDamagedEvent 구독
GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>(
  [this](const CharacterDamagedEvent& event) {
    this->DisplayDamageAmount(event);
    this->DisplayDamageLog(event);
  });

// Line 127-131: CharacterDeathEvent 구독
GetGSComponent<EventBus>()->Subscribe<CharacterDeathEvent>(
  [this](const CharacterDeathEvent& event) {
    this->CheckGameEnd(event);
  });

// Line 133-141: CharacterEscapedEvent 구독
GetGSComponent<EventBus>()->Subscribe<CharacterEscapedEvent>(
  [this](const CharacterEscapedEvent& event) {
    this->game_end = true;
  });
```

**이벤트 발행 패턴**:
- CombatSystem에서 `CharacterDamagedEvent` 발행
- Character에서 `CharacterDeathEvent` 발행
- GridSystem에서 `CharacterEscapedEvent` 발행

### ✅ 렌더링 시스템

런타임에 전환 가능한 3가지 렌더링 모드:

1. **ImmediateRenderer2D**: 즉시 모드 (디버깅 용이)
2. **BatchRenderer2D**: 배치 렌더링 (draw call 감소)
3. **InstancedRenderer2D**: GPU 인스턴싱 (최고 성능)

모든 렌더러는 `IRenderer2D` 인터페이스를 구현하여 교체 가능.

---

## 계획 시스템 (GAM250)

> **GAM250 학기 (2026 봄)** - 📋 계획

### 📋 주문 시스템 (SpellSystem)

**Dragon 주문 9개** (레벨 1-5):

| 주문 이름 | 레벨 | 효과 | 범위 | 업캐스팅 |
|----------|------|------|------|----------|
| 벽 생성 | 1 | 1x1 크기의 벽 생성 | 5칸 이내 | X |
| 화염탄 | 1 | 2d8 피해 | 5칸 이내 | +1d6/레벨 |
| 용암 뿌리기 | 2 | 2x2 용암 생성 (5턴, 8 피해/턴) | 5칸 이내 | +2 피해, +1턴/레벨 |
| 용의 포효 | 2 | 공포 상태 부여 (광역) | 3칸 이내 | +1칸 범위, +1턴/레벨 |
| 꼬리 휘두르기 | 2 | 1d8 피해 + 2칸 밀쳐내기 | 후방 3칸 | X |
| 용의 분노 | 3 | 4d6 피해 | 4칸 이내 | +2d6/레벨 |
| 운석 소환 | 3 | 3d20 피해, 다음 턴 스킵 | 홀/짝 번째 칸 | +1d20/레벨 |
| 열 흡수 | 4 | 화상/용암 제거 → HP/공격력 상승 | 전체 | X |
| 열 방출 | 5 | 최대 HP 50% 피해 → 3레벨 이하 슬롯 회복 | X | X |

**업캐스팅 시스템**: 낮은 레벨 주문을 높은 레벨 슬롯으로 사용 시 효과 강화

**구현 계획**:
- SpellSystem StateComponent 활성화
- Spell 베이스 클래스 및 개별 주문 클래스 구현
- SpellEffect 시스템 (데미지, 버프/디버프, 지형 변경)
- 주문 타겟팅 UI

### 📋 상태 이상 시스템 (StatusEffect)

**StatusEffectManager** (GameState Component):

| 상태 이상 | 효과 |
|----------|------|
| 저주 | 받는 피해 증가, 주는 피해 감소 |
| 공포 | 공격력 감소, 속력 감소 |
| 속박 | 이동 불가 |
| 실명 | 모든 범위 감소 |
| 축복 | 받는 피해 감소, 주는 피해 증가 |
| 신속 | 속력 증가, 행동력 증가 |
| 화상 | 턴 시작 시 지속 피해 |

**StatusEffects** (GameObject Component):
- 각 Character가 가지는 상태 이상 컬렉션
- 턴 시작/종료 시 효과 적용 및 지속 시간 감소

### 📋 능력 시스템 (Ability)

**AbilityBase** 클래스:
- Action 시스템 확장
- 캐릭터별 고유 능력 구현

**Fighter 능력**:
- ShieldBash: 밀쳐내기 + 스턴
- SecondWind: 자가 회복

**Cleric 능력**:
- Heal: 아군 회복
- Bless: 축복 버프
- Curse: 저주 디버프

**Wizard 능력**:
- Fireball: 광역 피해
- IceWall: 벽 생성

**Rogue 능력**:
- Stealth: 은신 (대상 지정 불가)
- SneakAttack: 기습 공격 (2배 피해)
- Dash: 속력 2배

### 📋 보물 시스템 (Treasure)

**TreasureManager** (GameState Component):
- 보물 상자 배치
- 보물 획득/운반 로직
- 출구 도달 시 탈출 체크

**모험가 목표**: 보물 획득 → 출구로 탈출

### 📋 나머지 AI 캐릭터

**ClericStrategy**:
- HP 낮은 아군 우선 치료
- 드래곤에게 저주 디버프
- 아군에게 축복 버프

**WizardStrategy**:
- 안전한 거리 유지 (6칸 이상)
- 원거리 주문 공격
- HP 위험 시 후퇴

**RogueStrategy**:
- 드래곤의 측면/후방 기습
- 보물 훔치기 우선
- 은신 활용

### 📋 고급 AI 시스템

**AIDirector** (GameState Component):
- 팀 전술 조율
- 역할 분담 (탱커, 힐러, 딜러)
- Bias 시스템 (캐릭터 성향)

**AI Memory** (GameObject Component):
- 의사결정 히스토리
- 학습 데이터

### 📋 사운드 시스템

**SoundManager** (GameState Component):
- 배경 음악
- 효과음 (공격, 주문, 이동)
- 3D 사운드 (거리 기반 볼륨)

### 📋 세이브/로드 시스템

**SaveManager** (GameState Component):
- 게임 상태 저장
- 캐릭터 스탯 저장
- 맵 상태 저장

### 📋 UI 시스템 개선

**현재 구현**:
- GamePlayUIManager (데미지 텍스트, 전투 로그)
- ImGui 기반 디버그 UI

**GAM250 추가 계획**:
- HP/AP 바 (각 캐릭터 위에 표시)
- 주문 선택 UI
- 상태 이상 아이콘
- 턴 순서 표시 UI
- 보물/출구 아이콘

### 📋 캠페인 모드

**멀티 스테이지**:
- 스테이지별 난이도 증가
- 보스 전투
- 스토리 진행

---

## 아키텍처 원칙

### 1. 모듈 독립성 (Modularity)

각 시스템은 독립적으로 작동하며 교체 가능:
- EventBus를 통한 느슨한 결합
- 인터페이스 기반 설계 (IRenderer2D, IAIStrategy)
- 시스템 추가/제거가 용이

### 2. 데이터 기반 설계 (Data-Driven Design)

코드와 데이터 분리:
- JSON으로 게임 데이터 관리 (`characters.json`, `maps.json`)
- 코드 재컴파일 없이 밸런스 조정 가능
- 런타임 리로드 지원

### 3. 이벤트 기반 통신 (Event-Driven Communication)

시스템 간 통신은 EventBus 사용:
- 타입 안전 이벤트 디스패치
- 구독자는 원하는 이벤트만 수신
- 디버깅 및 로깅이 간편 (SetLogging(true))

### 4. 컴포넌트 기반 아키텍처 (Component-Based Architecture)

상속보다 조합(Composition over Inheritance):
- GameObject Components: 개별 캐릭터 속성/행동
- GameState Components: 전투 시스템
- 런타임에 컴포넌트 추가/제거 가능

### 5. RAII 및 스마트 포인터

메모리 안전성 보장:
- `std::unique_ptr`로 소유권 명확화
- CharacterFactory는 `unique_ptr<Character>` 반환
- GameObjectManager가 소유권 관리 (`std::move`)
- 다른 시스템은 raw pointer로 참조만 사용

### 6. 시뮬레이션/뷰 분리

게임 로직과 렌더링 분리:
- `Update(dt)`: 게임 로직 시뮬레이션
- `Draw()`: 렌더링 전용
- 렌더링 모드 교체 가능 (Immediate/Batch/Instanced)

---

## 개발 가이드

### 시스템 접근 패턴

```cpp
// GameState Components (전투 시스템)
EventBus* eventBus = GetGSComponent<EventBus>();
TurnManager* turnMgr = GetGSComponent<TurnManager>();

// GameObject Components (캐릭터 컴포넌트)
StatsComponent* stats = character->GetGOComponent<StatsComponent>();
GridPosition* gridPos = character->GetGOComponent<GridPosition>();
```

### 캐릭터 생성 패턴

```cpp
CharacterFactory* factory = GetGSComponent<CharacterFactory>();

// unique_ptr 반환 → GameObjectManager로 소유권 이전
auto dragonPtr = factory->Create(CharacterTypes::Dragon, {1, 1});
Character* dragon = dragonPtr.get();  // raw pointer로 참조 저장
gameObjectManager->Add(std::move(dragonPtr));  // 소유권 이전
```

### 이벤트 발행/구독 패턴

```cpp
// 구독
GetGSComponent<EventBus>()->Subscribe<CharacterDamagedEvent>(
  [this](const CharacterDamagedEvent& event) {
    // 이벤트 처리
  });

// 발행
CharacterDamagedEvent event{target, damage, attacker, damageType};
GetGSComponent<EventBus>()->Publish(event);
```

### 주사위 굴림 패턴

```cpp
DiceManager* dice = GetGSComponent<DiceManager>();

// "3d6" - 6면체 주사위 3개
int result = dice->RollDiceFromString("3d6");

// "2d8+5" - 8면체 주사위 2개 + 5
int result = dice->RollDiceFromString("2d8+5");
```

---

## 참고 문서

- [게임 디자인 문서](../architecture/dragonic_tactics.pdf) - 전체 게임 디자인
- [아키텍처 원칙](../architecture/game_architecture_rules.md) - 설계 철학
- [구현 계획](./implementation-plan.md) - 상세 개발 계획
- [AI 플로우차트](../architecture/character_flowchart/) - AI 의사결정 로직

---

**최종 업데이트**: 2025-12-09
**작성자**: Dragonic Tactics 개발팀
