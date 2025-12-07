# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG
- **엔진**: 커스텀 C++20 OpenGL 엔진 (CMake 빌드 시스템)
- **기간**: 26주 개발 계획
- **현재 상태**: Week 1-2 완료, Week 3 진행 중

## 빠른 시작

### 빌드 명령어

```bash
# 구성 (설정)
cmake --preset windows-debug

# 빌드
cmake --build --preset windows-debug

# 실행
./build/windows-debug/dragonic_tactics.exe
```

### 빌드 프리셋
- `windows-debug` - 디버그 빌드 (콘솔 출력)
- `windows-developer-release` - 최적화 + 디버그 심볼
- `windows-release` - 완전 최적화
- `web-debug-on-windows` - WebAssembly 빌드

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
            ├── Objects/         # 게임 엔티티
            │   ├── Character.h/cpp      # 캐릭터 베이스 클래스
            │   ├── Dragon.h/cpp         # 플레이어 캐릭터
            │   ├── Fighter.h/cpp        # 적 캐릭터
            │   ├── Components/          # 캐릭터 컴포넌트
            │   └── Actions/             # 액션 시스템
            ├── Singletons/      # 전역 서비스
            │   ├── EventBus.h/cpp       # 이벤트 시스템
            │   ├── DiceManager.h/cpp    # 주사위 굴림
            │   ├── CombatSystem.h/cpp   # 전투 해결
            │   └── TurnManager.h/cpp    # 턴 관리 싱글톤
            ├── StateComponents/ # 게임 상태 컴포넌트
            │   ├── GridSystem.h/cpp     # 8x8 전술 그리드
            │   └── TurnManager.h/cpp    # 턴 관리 컴포넌트
            ├── States/          # 게임 상태
            │   ├── GamePlay.h/cpp       # 메인 게임플레이
            │   └── Test_*.cpp           # 개발자별 테스트 상태
            ├── Types/           # 공유 타입 정의
            ├── Debugger/        # 디버그 도구
            └── Test/            # 테스트 유틸리티
```

## 핵심 시스템 (현재 구현됨)

### 1. 이벤트 시스템 (EventBus)
**싱글톤 패턴**, 타입 안전 이벤트 디스패치

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
**싱글톤 패턴**, D&D 주사위 표기법 지원

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
**싱글톤 패턴**, 속도 기반 이니셔티브

```cpp
// 턴 순서 초기화 (속도 스탯으로 정렬)
void InitializeTurnOrder(std::vector<Character*> characters);

// 턴 진행
void StartNextTurn();
void EndCurrentTurn();

Character* GetCurrentCharacter() const;
```

### 6. 전투 시스템 (CombatSystem)
**싱글톤 패턴**, 주사위 기반 데미지 계산

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

## 개발 패턴

### 네임스페이스
- **CS230**: 엔진 코어 (Engine, GameObject, GameState, Component)
- **CS200**: 렌더링 추상화 (IRenderer2D, RenderingAPI)
- **OpenGL**: 저수준 OpenGL 래퍼
- **Math**: 수학 유틸리티 (vec2, ivec2, TransformationMatrix)

### 싱글톤 접근
```cpp
EventBus::Instance().Publish(event);
DiceManager::Instance().RollDice(3, 6);
CombatSystem::Instance().ExecuteAttack(attacker, defender);
```

### 엔진 서브시스템 접근
```cpp
Engine::GetLogger().LogEvent("메시지");
Engine::GetInput().IsKeyPressed(InputKey::Space);
Engine::GetWindow().GetSize();
Engine::GetGameStateManager().SetNextGameState<BattleState>();
```

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
        // 리소스 초기화
        AddComponent(new GridSystem());
    }

    void Update(double dt) override {
        UpdateComponents(dt);
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

### 외부 의존성 (자동 다운로드)
CMake FetchContent로 자동 관리:
- **OpenGL** - 그래픽 API
- **GLEW** - OpenGL 확장
- **SDL2** - 윈도우, 입력, 플랫폼 추상화 (v2.28.5+)
- **Dear ImGui** - 디버그 UI (docking 브랜치)
- **GSL** - Guidelines Support Library (v4.0.0)
- **STB** - 이미지 로딩 (stb_image.h)

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
- [docs/Detailed Implementations/weeks/week3.md](docs/Detailed%20Implementations/weeks/week3.md) - Week 3 가이드 (진행중)

### 디버그 도구
- [docs/debug/tools.md](docs/debug/tools.md) - 디버그 시스템
- [docs/debug/commands.md](docs/debug/commands.md) - 콘솔 명령어
- [docs/debug/ui.md](docs/debug/ui.md) - 디버그 UI

## 구현 상태

### ✅ 완료 (Week 1-2)
- EventBus (이벤트 시스템)
- DiceManager (주사위 굴림)
- Character 베이스 클래스
- GridSystem (8x8 전술 그리드)
- TurnManager (턴 관리)
- StatsComponent (전투 스탯)
- ActionPoints (행동 포인트)
- SpellSlots (마법 슬롯)
- Dragon 캐릭터 (플레이어)
- Fighter 캐릭터 (적)
- 그리드 통합 및 캐릭터 이동
- 캐릭터 스프라이트 렌더링

### 🚧 진행 중 (Week 3)
- 전투 시스템 정제
- 스펠 시스템 구현
- AI 시스템 기초

### ⏳ 계획 (Week 4-5)
- 스펠 확장
- 고급 전투 기능
- 폴리싱 및 첫 플레이테스트

## 핵심 파일

### 엔트리 포인트
- [DragonicTactics/source/main.cpp](DragonicTactics/source/main.cpp)

### 엔진 코어
- [Engine.hpp](DragonicTactics/source/Engine/Engine.hpp) - 엔진 싱글톤
- [GameObject.h](DragonicTactics/source/Engine/GameObject.h) - 엔티티 베이스
- [GameState.hpp](DragonicTactics/source/Engine/GameState.hpp) - 상태 인터페이스

### 게임 시스템
- [Character.h](DragonicTactics/source/Game/DragonicTactics/Objects/Character.h) - 캐릭터 베이스
- [EventBus.h](DragonicTactics/source/Game/DragonicTactics/Singletons/EventBus.h) - 이벤트 시스템
- [GridSystem.h](DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.h) - 그리드 시스템
- [DiceManager.h](DragonicTactics/source/Game/DragonicTactics/Singletons/DiceManager.h) - 주사위 매니저
- [CombatSystem.h](DragonicTactics/source/Game/DragonicTactics/Singletons/CombatSystem.h) - 전투 시스템

## 중요 참고사항

1. **새 엔진**: MSBuild 기반 구 엔진이 아닌, CMake 기반 신규 엔진
2. **C++20**: C++17이 아닌 C++20 표준 사용
3. **CMake**: MSBuild가 아닌 CMake 빌드 시스템
4. **Week 1-2 구현 완료**: 문서뿐만 아니라 실제 코드 구현됨
5. **이벤트 기반 통신**: 시스템 간 통신은 EventBus 사용
6. **디버그 로깅**: `Engine::GetLogger()`로 이벤트/오류 로그
7. **ImGui**: 디버그 시각화용 ImGui 사용
