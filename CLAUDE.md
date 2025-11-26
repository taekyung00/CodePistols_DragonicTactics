# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG
- **엔진**: 커스텀 C++20 OpenGL 엔진 (CMake 빌드 시스템)
- **기간**: 26주 개발 계획
- **현재 상태**: Week 1-3 완료, Milestone 3 완료

## 빠른 시작

### 빌드 명령어

```bash
# 작업 디렉토리: DragonicTactics/ (프로젝트 루트 아님!)
cd DragonicTactics

# 구성 (설정)
cmake --preset windows-debug

# 빌드
cmake --build --preset windows-debug

# 실행
build/windows-debug/dragonic_tactics.exe
```

### 빌드 프리셋
- `windows-debug` - 디버그 빌드 (콘솔 출력 활성화)
- `windows-developer-release` - 최적화 + 디버그 심볼 + 콘솔
- `windows-release` - 완전 최적화 (콘솔 비활성화)
- `linux-debug` / `linux-developer-release` / `linux-release` - Linux 빌드
- `web-debug-on-windows` - Windows에서 WebAssembly 빌드
- `web-debug` / `web-developer-release` / `web-release` - Web 빌드 (Emscripten)

### 테스트 단축키 (GamePlay 상태에서)
- **F**: EventBus 테스트
- **E**: DiceManager 테스트
- **T**: Dragon 턴 상태 표시
- **Y**: Fighter 턴 상태 표시
- **D**: Dragon 공격
- **H**: Fighter 힐
- **S**: Grid 시각화
- **P**: TurnManager 테스트
- **J/R/L**: JSON 데이터 로드/리로드/로그
- **Enter**: 전체 전투 시스템 테스트
- **ESC**: 테스트 상태 종료

## 프로젝트 구조

```
CodePistols_DragonicTactics/
├── CLAUDE.md                    # 이 파일
├── docs/                        # 설계 문서
│   ├── index.md                 # 문서 네비게이션
│   ├── architecture.md          # 아키텍처 개요
│   ├── implementation-plan.md   # 26주 개발 계획
│   ├── systems/                 # 시스템별 상세 설계
│   └── Detailed Implementations/weeks/  # 주차별 구현 가이드
│
├── architecture/                # 지원 문서
│   ├── dragonic_tactics.md      # 게임 디자인 문서 (한글)
│   └── game_architecture_rules.md  # 아키텍처 원칙 (한글)
│
└── DragonicTactics/             # 메인 프로젝트
    ├── CMakeLists.txt           # CMake 설정
    ├── cmake/                   # CMake 모듈
    ├── Assets/                  # 게임 에셋 (PNG, SPT, ANM)
    └── source/                  # 소스 코드
        ├── main.cpp             # 엔트리 포인트
        ├── Engine/              # 엔진 코어 (CS230 네임스페이스)
        ├── CS200/               # 렌더링 추상화
        ├── OpenGL/              # OpenGL 래퍼
        └── Game/DragonicTactics/  # 게임 코드
            ├── Abilities/       # 캐릭터 어빌리티
            │   ├── AbilityBase.h        # 어빌리티 인터페이스
            │   ├── MeleeAttack.h/cpp    # 근접 공격
            │   └── ShieldBash.h/cpp     # 쉴드 배쉬
            ├── Objects/         # 게임 엔티티
            │   ├── Character.h/cpp      # 캐릭터 베이스 클래스
            │   ├── Dragon.h/cpp         # 플레이어 캐릭터
            │   ├── Fighter.h/cpp        # 적 캐릭터
            │   ├── Components/          # 캐릭터 컴포넌트
            │   │   ├── ActionPoints.h/cpp    # 행동 포인트
            │   │   ├── GridPosition.h/cpp    # 그리드 위치
            │   │   ├── MovementComponent.h/cpp # 이동 시스템
            │   │   ├── SpellSlots.h/cpp      # 마법 슬롯
            │   │   └── StatsComponent.h/cpp  # 전투 스탯
            │   └── Actions/             # 액션 시스템
            │       ├── Action.h/cpp          # 액션 베이스
            │       └── ActionAttack.h/cpp    # 공격 액션
            ├── StateComponents/ # 게임 상태 컴포넌트 (GameState에 연결)
            │   ├── AISystem.h/cpp       # AI 시스템
            │   ├── AStar.cpp            # A* 경로 찾기
            │   ├── CombatSystem.h/cpp   # 전투 해결
            │   ├── DataRegistry.h/cpp   # 캐릭터 데이터 로딩
            │   ├── DiceManager.h/cpp    # 주사위 굴림
            │   ├── EventBus.h/cpp       # 이벤트 시스템
            │   ├── GridSystem.h/cpp     # 8x8 전술 그리드
            │   ├── SpellSystem.h/cpp    # 마법 시스템
            │   └── TurnManager.h/cpp    # 턴 관리
            ├── States/          # 게임 상태
            │   ├── GamePlay.h/cpp       # 메인 게임플레이
            │   ├── RenderingTest.h/cpp  # 렌더링 테스트
            │   └── ConsoleTest.h/cpp    # 콘솔 테스트
            ├── Types/           # 공유 타입 정의
            │   ├── CharacterTypes.h     # 캐릭터 타입
            │   ├── Events.h             # 이벤트 타입
            │   ├── GameObjectTypes.h    # GameObject 타입
            │   └── GameTypes.h          # 게임 타입
            ├── Debugger/        # 디버그 도구
            │   ├── DebugConsole.h/cpp   # 디버그 콘솔
            │   ├── DebugManager.h/cpp   # 디버그 매니저
            │   └── DebugVisualizer.h/cpp # 시각화
            └── Test/            # 테스트 유틸리티
                ├── Week1TestMocks.h/cpp # Week 1 테스트
                ├── Week3TestMocks.h/cpp # Week 3 테스트
                ├── TestAStar.h/cpp      # A* 테스트
                ├── TestAssert.h/cpp     # Assert 테스트
                ├── TestTurnInit.h/cpp   # 턴 초기화 테스트
                ├── TestCombatSystem.h/cpp # 전투 시스템 테스트
                ├── TestEventBus.h/cpp   # 이벤트 버스 테스트
                ├── TestSpellSystem.h/cpp # 스펠 시스템 테스트
                ├── TestDataRegistry.h/cpp # 데이터 레지스트리 테스트
                ├── TestDiceManager.h/cpp # 주사위 매니저 테스트
                ├── TestTurnManager.h/cpp # 턴 매니저 테스트
                └── TestAI.h/cpp         # AI 테스트
```

## 핵심 시스템 (현재 구현됨)

**중요**: 모든 시스템은 `StateComponents/` 디렉토리에 위치하며, GameState 컴포넌트로 구현됨

### 1. 이벤트 시스템 (EventBus)
**GameState 컴포넌트**, 타입 안전 이벤트 디스패치

```cpp
// 이벤트 구독
EventBus::Instance().Subscribe<CharacterDamagedEvent>(
    [](const CharacterDamagedEvent& event) {
        // 이벤트 처리
    }
);

// 이벤트 발행
CharacterDamagedEvent event{target, damage, attacker, damage_type};
EventBus::Instance().Publish(event);
```

### 2. 주사위 시스템 (DiceManager)
**GameState 컴포넌트**, D&D 주사위 표기법 지원

```cpp
// "3d6" (6면 주사위 3개) 굴림
int result = DiceManager::Instance().RollDiceFromString("3d6");

// "2d8+5" (8면 주사위 2개 + 5) 굴림
int result = DiceManager::Instance().RollDiceFromString("2d8+5");
```

### 3. 캐릭터 시스템 (Character)
**GameObject 상속**, 컴포넌트 기반 아키텍처

**컴포넌트**:
- `GridPosition` - 8x8 그리드 좌표
- `StatsComponent` - HP, 공격력, 방어력, 속도
- `ActionPoints` - 턴당 행동 포인트
- `SpellSlots` - 마법 시스템 (레벨 1-9)

**구현된 캐릭터**:
- `Dragon` - 플레이어 캐릭터
- `Fighter` - 적 캐릭터 (근접 전투)

### 4. 그리드 시스템 (GridSystem)
**GameState 컴포넌트**, 8x8 전술 그리드

```cpp
// 타일 쿼리
bool IsWalkable(Math::ivec2 tile) const;
bool IsOccupied(Math::ivec2 tile) const;

// 캐릭터 이동
void MoveCharacter(Character* character, Math::ivec2 new_position);
Character* GetCharacterAt(Math::ivec2 position) const;
```

### 5. 턴 관리 (TurnManager)
**GameState 컴포넌트**, 속도 기반 이니셔티브

```cpp
// 턴 순서 초기화 (속도 스탯으로 정렬)
void InitializeTurnOrder(std::vector<Character*> characters);

// 턴 진행
void StartNextTurn();
void EndCurrentTurn();

Character* GetCurrentCharacter() const;
```

### 6. 전투 시스템 (CombatSystem)
**GameState 컴포넌트**, 주사위 기반 데미지 계산

```cpp
// 전체 공격 시퀀스
CombatSystem::Instance().ExecuteAttack(attacker, defender);

// 데미지 계산
int damage = CombatSystem::Instance().CalculateDamage(
    attacker, defender, damage_type
);

// 거리 계산
int distance = CombatSystem::Instance().GetDistance(pos1, pos2);
```

### 7. 마법 시스템 (SpellSystem)
**GameState 컴포넌트**, D&D 스펠 슬롯 및 캐스팅

```cpp
// 스펠 캐스트
SpellSystem::Instance().CastSpell(caster, spell_id, target);

// 스펠 슬롯 확인
bool hasSlot = character->GetSpellSlots()->HasSlot(spell_level);
```

### 8. AI 시스템 (AISystem)
**GameState 컴포넌트**, 적 AI 의사결정

```cpp
// AI 턴 실행
AISystem::Instance().ExecuteAITurn(character);
```

### 9. 데이터 레지스트리 (DataRegistry)
**GameState 컴포넌트**, JSON 기반 캐릭터 데이터 로딩

```cpp
// JSON 데이터 로드
DataRegistry::Instance().LoadCharacterData("Assets/Data/characters.json");

// 캐릭터 데이터 조회
auto charData = DataRegistry::Instance().GetCharacterData("Dragon");
```

## 개발 패턴

### 네임스페이스
- **CS230**: 엔진 코어 (Engine, GameObject, GameState, Component)
- **CS200**: 렌더링 추상화 (IRenderer2D, RenderingAPI)
- **OpenGL**: 저수준 OpenGL 래퍼
- **Math**: 수학 유틸리티 (vec2, ivec2, TransformationMatrix)

### StateComponent 시스템 접근
모든 게임 시스템은 GameState 컴포넌트로 구현되며, Instance() 메서드로 접근:

```cpp
// GameState에 추가 (AddGSComponent 사용)
void GamePlay::Load() {
    AddGSComponent(new EventBus());
    AddGSComponent(new DiceManager());
    AddGSComponent(new CombatSystem());
    AddGSComponent(new TurnManager());
    AddGSComponent(new GridSystem());
    AddGSComponent(new SpellSystem());
    AddGSComponent(new AISystem());
    AddGSComponent(new DataRegistry());
}

// 어디서든 Instance()로 접근
EventBus::Instance().Publish(event);
DiceManager::Instance().RollDice(3, 6);
CombatSystem::Instance().ExecuteAttack(attacker, defender);
TurnManager::Instance().StartNextTurn();
GridSystem::Instance().MoveCharacter(character, position);
```

### 엔진 서브시스템 접근
```cpp
Engine::GetLogger().LogEvent("메시지");
Engine::GetInput().IsKeyPressed(InputKey::Space);
Engine::GetWindow().GetSize();
Engine::GetGameStateManager().SetNextGameState<BattleState>();
```

**중요**: GameState와 GameObject는 서로 다른 컴포넌트 시스템 사용:
- **GameState**: `AddGSComponent()`, `GetGSComponent<T>()`, `UpdateGSComponents(dt)`
- **GameObject**: `AddGOComponent()`, `GetGOComponent<T>()` (Update는 GameObject::Update에서 자동 호출)

### GameObject 개발
```cpp
class MyCharacter : public CS230::GameObject {
public:
    GameObjectTypes Type() override { return GameObjectTypes::Character; }
    std::string TypeName() override { return "MyCharacter"; }

    void Update(double dt) override {
        // 게임 로직
    }

    void Draw(Math::TransformationMatrix camera_matrix) override {
        // 렌더링
    }
};
```

### 컴포넌트 개발
```cpp
class MyComponent : public CS230::Component {
public:
    void Update(double dt) override {
        // 컴포넌트 로직
    }
};

// GameObject에서 사용
void MyGameObject::Load() {
    AddGOComponent(new MyComponent());
}

void MyGameObject::Update(double dt) {
    auto comp = GetGOComponent<MyComponent>();
    comp->DoSomething();
}
```

### GameState 개발
```cpp
class MyGameState : public CS230::GameState {
public:
    void Load() override {
        // 리소스 초기화 (AddGSComponent 사용)
        AddGSComponent(new GridSystem());
    }

    void Update(double dt) override {
        UpdateGSComponents(dt);  // GameState 컴포넌트 업데이트
        game_object_manager_.UpdateAll(dt);
    }

    void Draw() override {
        game_object_manager_.DrawAll(GetCamera());
    }

    void Unload() override {
        // 정리
    }
};
```

## 기술 스택

### 빌드 시스템
- **CMake 3.21+** (C++20 표준)
- **Visual Studio 2022** (Platform Toolset v143)
- **경고 레벨**: Level 4, 경고를 오류로 처리
- **프리컴파일 헤더**: 현재 비활성화 (pch.h는 존재하지만 사용 안 함)

### 외부 의존성 (자동 다운로드)
CMake FetchContent로 자동 관리:
- **OpenGL** - 그래픽 API
- **GLEW** - OpenGL 확장
- **SDL2** - 윈도우, 입력, 플랫폼 추상화 (v2.28.5+)
- **Dear ImGui** - 디버그 UI (docking 브랜치)
- **GSL** - Guidelines Support Library (v4.0.0)
- **STB** - 이미지 로딩 (stb_image.h)
- **nlohmann/json** - JSON 파싱 (json.hpp in External/)

### 플랫폼 지원
- **Windows (Native)**: MSVC, OpenGL 직접 렌더링
- **WebAssembly**: Emscripten, SDL2 + OpenGL ES

## 문서 내비게이션

자세한 설계 문서는 [docs/index.md](docs/index.md) 참조:

### 시스템 설계
- [docs/architecture.md](docs/architecture.md) - 전체 아키텍처
- [docs/systems/characters.md](docs/systems/characters.md) - 캐릭터 시스템
- [docs/systems/singletons.md](docs/systems/singletons.md) - 싱글톤 서비스
- [docs/systems/game-state-components.md](docs/systems/game-state-components.md) - 전투 시스템
- [docs/systems/game-object-components.md](docs/systems/game-object-components.md) - 컴포넌트

### 구현 가이드
- [docs/implementation-plan.md](docs/implementation-plan.md) - 26주 개발 계획
- [docs/Detailed Implementations/weeks/week1.md](docs/Detailed%20Implementations/weeks/week1.md) - Week 1 가이드 (완료)
- [docs/Detailed Implementations/weeks/week2.md](docs/Detailed%20Implementations/weeks/week2.md) - Week 2 가이드 (완료)
- [docs/Detailed Implementations/weeks/week3.md](docs/Detailed%20Implementations/weeks/week3.md) - Week 3 가이드 (완료)

### 디버그 도구
- [docs/debug/tools.md](docs/debug/tools.md) - 디버그 시스템
- [docs/debug/commands.md](docs/debug/commands.md) - 콘솔 명령어
- [docs/debug/ui.md](docs/debug/ui.md) - 디버그 UI

## 구현 상태

### ✅ 완료 (Week 1-3, Milestone 3)
- **핵심 시스템** (모두 StateComponents로 구현)
  - EventBus (이벤트 시스템)
  - DiceManager (주사위 굴림)
  - CombatSystem (전투 해결)
  - TurnManager (턴 관리)
  - GridSystem (8x8 전술 그리드 + A* 경로 찾기)
  - SpellSystem (마법 시스템)
  - AISystem (적 AI)
  - DataRegistry (JSON 데이터 로딩)

- **캐릭터 시스템**
  - Character 베이스 클래스
  - Dragon 캐릭터 (플레이어)
  - Fighter 캐릭터 (적)
  - 컴포넌트: StatsComponent, ActionPoints, SpellSlots, GridPosition, MovementComponent

- **어빌리티 시스템**
  - AbilityBase 인터페이스
  - MeleeAttack, ShieldBash

- **액션 시스템**
  - Action 베이스 클래스
  - ActionAttack

- **디버그 도구**
  - DebugConsole (콘솔 명령어)
  - DebugManager (디버그 모드 관리)
  - DebugVisualizer (그리드 시각화)

### ⏳ 계획 (Week 4+)
- 더 많은 캐릭터 클래스
- 더 많은 어빌리티
- 고급 AI 행동
- UI 시스템
- 사운드 시스템

## 핵심 파일

### 엔트리 포인트
- [DragonicTactics/source/main.cpp](DragonicTactics/source/main.cpp)

### 엔진 코어
- [Engine.hpp](DragonicTactics/source/Engine/Engine.hpp) - 엔진 싱글톤
- [GameObject.h](DragonicTactics/source/Engine/GameObject.h) - 엔티티 베이스
- [GameState.hpp](DragonicTactics/source/Engine/GameState.hpp) - 상태 인터페이스

### 게임 시스템 (StateComponents)
- [Character.h](DragonicTactics/source/Game/DragonicTactics/Objects/Character.h) - 캐릭터 베이스
- [EventBus.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/EventBus.h) - 이벤트 시스템
- [GridSystem.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.h) - 그리드 시스템
- [DiceManager.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/DiceManager.h) - 주사위 매니저
- [CombatSystem.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/CombatSystem.h) - 전투 시스템
- [TurnManager.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.h) - 턴 관리
- [SpellSystem.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/SpellSystem.h) - 마법 시스템
- [AISystem.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/AISystem.h) - AI 시스템
- [DataRegistry.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/DataRegistry.h) - 데이터 레지스트리

## 중요 참고사항

1. **작업 디렉토리**: 모든 빌드 명령은 `DragonicTactics/` 디렉토리에서 실행 (프로젝트 루트 아님!)
2. **새 엔진**: MSBuild 기반 구 엔진이 아닌, CMake 기반 신규 엔진
3. **C++20**: C++17이 아닌 C++20 표준 사용
4. **CMake 프리셋**: `cmake --preset windows-debug` 형식으로 사용
5. **StateComponents 아키텍처**: 모든 게임 시스템은 `StateComponents/` 디렉토리에 GameState 컴포넌트로 구현됨 (Singletons 폴더 없음)
6. **Week 1-3 구현 완료**: EventBus, DiceManager, CombatSystem, TurnManager, GridSystem, SpellSystem, AISystem, DataRegistry 모두 구현됨
7. **이벤트 기반 통신**: 시스템 간 통신은 EventBus 사용
8. **디버그 로깅**: `Engine::GetLogger()`로 이벤트/오류 로그
9. **ImGui**: 디버그 시각화용 ImGui 사용 (docking 브랜치)
10. **테스트**: Test/ 디렉토리에 각 시스템별 테스트 파일 존재

## 테스트 실행

테스트는 런타임에 키보드 단축키로 실행됩니다 (GamePlay 상태에서):

```bash
# 빌드 후 실행
cd DragonicTactics
build/windows-debug/dragonic_tactics.exe

# 게임 실행 후 테스트 단축키 사용 (위 "테스트 단축키" 섹션 참조)
```

**테스트 파일 위치**: `DragonicTactics/source/Game/DragonicTactics/Test/`
- 각 시스템별로 Test*.h/cpp 파일 존재
- GamePlay.cpp에서 키보드 입력으로 테스트 함수 호출

## 일반적인 개발 워크플로우

### 새 캐릭터 클래스 추가
1. `Objects/` 에 `MyCharacter.h/cpp` 생성
2. `Character` 상속 및 필요한 컴포넌트 추가
3. `source/CMakeLists.txt`의 `SOURCE_CODE`에 파일 추가
4. `GamePlay::Load()`에서 인스턴스화 및 GridSystem에 등록
5. 빌드 후 테스트

### 새 어빌리티 추가
1. `Abilities/` 에 `MyAbility.h/cpp` 생성
2. `AbilityBase` 인터페이스 구현
3. `Character::abilities_` 벡터에 추가
4. `source/CMakeLists.txt`에 파일 추가
5. 빌드 후 GamePlay에서 테스트

### 새 이벤트 타입 추가
1. `Types/Events.h`에 이벤트 구조체 정의
2. 발행자에서 `EventBus::Instance().Publish(event)` 호출
3. 구독자에서 `EventBus::Instance().Subscribe<EventType>(callback)` 호출
4. 테스트 시 `EventBus::Instance().SetLogging(true)` 활성화

### JSON 데이터 수정
1. `Assets/Data/characters.json` 편집
2. 게임 실행 후 **R** 키로 리로드
3. **L** 키로 로드된 데이터 확인

## 문제 해결

### 빌드 실패
```bash
# CMake 캐시 정리 후 재구성
cd DragonicTactics
rm -rf build/
cmake --preset windows-debug
cmake --build --preset windows-debug
```

### 실행 파일이 Assets을 찾지 못함
- 실행 파일은 항상 `DragonicTactics/` 디렉토리에서 실행해야 함
- Assets 폴더는 `DragonicTactics/Assets/`에 위치

### Visual Studio에서 빌드 시
- CMake 프리셋을 사용하거나
- VS에서 "Open Folder"로 DragonicTactics 폴더 열기
- CMakePresets.json이 자동으로 인식됨

### 링커 오류 (unresolved external symbol)
- `source/CMakeLists.txt`의 `SOURCE_CODE`에 .cpp 파일 추가 확인
- CMake 재구성: `cmake --preset windows-debug`

## 아키텍처 원칙

프로젝트는 [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md)의 원칙을 따릅니다:

1. **모듈 독립성**: 각 시스템은 독립적으로 작동하며 교체 가능
2. **접착제 시스템**: GameState가 시스템 간 연결 담당
3. **코드와 데이터 분리**: JSON으로 게임 데이터 관리 (Assets/Data/)
4. **이벤트 기반 통신**: EventBus를 통한 느슨한 결합
5. **시뮬레이션/뷰 분리**: 게임 로직과 렌더링 분리
