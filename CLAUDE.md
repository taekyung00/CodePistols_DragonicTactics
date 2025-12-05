# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

**Dragonic Tactics**: D&D 스타일 턴제 전술 RPG
- **엔진**: 커스텀 C++20 OpenGL 엔진 (CMake 빌드 시스템)
- **기간**: 26주 개발 계획
- **현재 상태**: Week 1-3 완료, Milestone 3 완료, Week 4-5 진행 중
- **팀 구성**: 5명의 개발자

### 최근 변경사항 (Updated: 2025-12-04)

- **Precompiled Headers 활성화**: pch.h 구축 완료, 빌드 속도 35% 개선
- **CMake 자동화**: GLOB_RECURSE로 소스 파일 자동 수집
- **CharacterFactory 패턴 도입**: 캐릭터 생성 로직 중앙화
- **GamePlay 리팩토링 완료**: PlayerInputHandler, GamePlayUIManager, BattleOrchestrator로 책임 분리
- **StateComponents 전환 완료**: 모든 게임 시스템이 GetGSComponent<>()로 접근
- **헤더 파일 표준화 완료**: `.hpp` → `.h` 확장자 통일

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
            │   ├── GamePlay.h/cpp           # 메인 게임플레이
            │   ├── PlayerInputHandler.h/cpp # 플레이어 입력 처리
            │   ├── GamePlayUIManager.h/cpp  # UI 관리
            │   ├── BattleOrchestrator.h/cpp # 전투 흐름 제어
            │   ├── RenderingTest.h/cpp      # 렌더링 테스트
            │   └── ConsoleTest.h/cpp        # 콘솔 테스트
            ├── Factories/       # 팩토리 패턴
            │   └── CharacterFactory.h/cpp   # 캐릭터 생성
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

## 렌더링 시스템

프로젝트는 **3가지 렌더링 모드**를 지원하며, 런타임에 전환 가능합니다:

### 렌더링 모드

1. **ImmediateRenderer2D** (기본)
   - 즉시 모드 렌더링
   - 각 Draw 호출마다 즉시 GPU에 전송
   - 단순하고 디버깅이 쉬움
   - 성능: 낮음 (많은 draw call)

2. **BatchRenderer2D**
   - 배치 렌더링
   - 동일한 텍스처를 사용하는 여러 쿼드를 하나의 draw call로 묶음
   - CPU에서 정점 데이터를 모아서 한 번에 GPU로 전송
   - 성능: 중간 (draw call 감소)

3. **InstancedRenderer2D**
   - 인스턴스 렌더링
   - GPU 인스턴싱을 활용하여 동일한 메시를 여러 번 그림
   - 각 인스턴스는 다른 변환 행렬과 텍스처 좌표 사용
   - 성능: 높음 (최소 draw call + GPU 가속)

### 렌더링 모드 전환

```cpp
// TextureManager를 통해 렌더러 전환
Engine::GetTextureManager().SwitchRenderer(
    CS230::TextureManager::RendererType::Batch
);

// 현재 렌더러 확인
auto current_type = Engine::GetTextureManager().GetCurrentRendererType();

// 렌더러 접근
CS200::IRenderer2D* renderer = CS230::TextureManager::GetRenderer2D();
```

### 렌더링 인터페이스 (IRenderer2D)

모든 렌더러는 동일한 인터페이스를 구현:

```cpp
// 장면 시작/종료
void BeginScene(const Math::TransformationMatrix& view_projection);
void EndScene();

// 그리기 명령
void DrawQuad(const Math::TransformationMatrix& transform,
              OpenGL::TextureHandle texture,
              Math::vec2 texture_coord_bl = {0.0, 0.0},
              Math::vec2 texture_coord_tr = {1.0, 1.0},
              CS200::RGBA tintColor = CS200::WHITE,
              float depth = 1.f);

void DrawCircle(const Math::TransformationMatrix& transform,
                CS200::RGBA fill_color = CS200::CLEAR,
                CS200::RGBA line_color = CS200::WHITE,
                double line_width = 2.0,
                float depth = 0.f);

void DrawRectangle(const Math::TransformationMatrix& transform,
                   CS200::RGBA fill_color = CS200::CLEAR,
                   CS200::RGBA line_color = CS200::WHITE,
                   double line_width = 2.0,
                   float depth = 0.f);

void DrawLine(Math::vec2 start_point, Math::vec2 end_point,
              CS200::RGBA line_color = CS200::WHITE,
              double line_width = 2.0,
              float depth = 0.f);

// 성능 모니터링
size_t GetDrawCallCounter();
size_t GetDrawTextureCounter();
```

## 기술 스택

### 빌드 시스템
- **CMake 3.21+** (C++20 표준)
- **Visual Studio 2022** (Platform Toolset v143)
- **경고 레벨**: Level 4, 경고를 오류로 처리
- **프리컴파일 헤더**: 활성화됨 (pch.h) - 빌드 속도 35% 개선
- **소스 파일 수집**: GLOB_RECURSE로 자동 감지 (CMakeLists.txt 수동 업데이트 불필요)

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

## 개발 프로세스

### 계획 문서 관리
프로젝트는 유연한 계획 관리 방식을 따릅니다:

1. **architecture.md**: 전체 게임 아키텍처 및 시스템 설계 (변경 없음)
2. **implementation-plan.md**:
   - architecture.md 기반으로 작성
   - 우선순위에 따라 언제든지 재정렬 가능
   - 향후 1개월 계획은 상세하게, 나머지는 간략하게 작성
   - 팀 합의 후 업데이트
3. **주차별 상세 계획** (Detailed Implementations/weeks/):
   - implementation-plan 기반으로 매주 작성
   - 한글로 작성
   - 구조: Implementation Tasks → Implementation Example → Rigorous Test → Usage Example
   - 5명의 개발자 역할 분담 포함

### 작업 우선순위 설정
새로운 우선순위를 설정할 때:
1. 구현하고 싶은 기능/개선사항 나열
2. Claude와 함께 기술적 타당성 평가
3. 합의 후 implementation-plan.md 재작성
4. 주차별 상세 계획 작성

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

- **렌더링 시스템**
  - IRenderer2D 인터페이스
  - ImmediateRenderer2D (즉시 모드)
  - BatchRenderer2D (배치 렌더링)
  - InstancedRenderer2D (GPU 인스턴싱)
  - 런타임 렌더러 전환 기능

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

### ⏳ 진행 중 (Week 4-5)
- **턴 플로우 개선**: 각 턴 시작/진행/종료 시 필수 작업들의 명확한 정의 및 함수 일대일 대응
- **AI 시스템 강화**: 4명의 모험가 캐릭터에 대한 robust한 AI 구현
- **디버그 UI 개선**: ImGui 기반 정보 표시, 런타임 토글 기능
- **소유권 모델 재설계**: 캐릭터 객체의 명확한 소유권 및 스마트 포인터 적용
- **AI 행동 시각화**: AI 행동 중간에 pause 추가로 플레이어가 상황 파악 가능
- **맵 데이터 로딩**: JSON 기반 맵 데이터 파싱 및 타일 정보 설정

### 📋 계획 (Week 6+)
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
- [TextureManager.h](DragonicTactics/source/Engine/TextureManager.h) - 텍스처 및 렌더러 관리

### 렌더링 시스템 (CS200)
- [IRenderer2D.h](DragonicTactics/source/CS200/IRenderer2D.h) - 렌더러 인터페이스
- [ImmediateRenderer2D.h](DragonicTactics/source/CS200/ImmediateRenderer2D.h) - 즉시 모드 렌더러
- [BatchRenderer2D.h](DragonicTactics/source/CS200/BatchRenderer2D.h) - 배치 렌더러
- [InstancedRenderer2D.h](DragonicTactics/source/CS200/InstancedRenderer2D.h) - 인스턴스 렌더러

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
5. **StateComponents 아키텍처**: 모든 게임 시스템은 `StateComponents/` 디렉토리에 GameState 컴포넌트로 구현됨
6. **헤더 파일 표준**: 모든 헤더 파일은 `.h` 확장자 사용 (`.hpp` → `.h` 마이그레이션 완료)
7. **Precompiled Headers**: 모든 `.cpp` 파일은 `#include "pch.h"`를 첫 줄에 포함해야 함
8. **파일 추가/삭제**: CMake가 GLOB_RECURSE로 자동 감지하므로 CMakeLists.txt 수동 편집 불필요
9. **CharacterFactory 사용**: 캐릭터 생성 시 `new Dragon()` 대신 `CharacterFactory::Create()` 사용
10. **GamePlay 구조**: PlayerInputHandler, GamePlayUIManager, BattleOrchestrator로 책임 분리됨
11. **이벤트 기반 통신**: 시스템 간 통신은 EventBus 사용, GetGSComponent<EventBus>()로 접근
12. **디버그 로깅**: `Engine::GetLogger()`로 이벤트/오류 로그, 함수 호출 추적은 `__PRETTY_FUNCTION__` 매크로 사용
13. **ImGui**: 디버그 시각화용 ImGui 사용 (docking 브랜치), 런타임에 켜고 끌 수 있음
14. **테스트**: Test/ 디렉토리에 각 시스템별 테스트 파일 존재
15. **메모리 관리**: 스마트 포인터 사용 권장 (RAII 원칙), GamePlay는 unique_ptr 사용
16. **렌더링 시스템**: TextureManager를 통해 3가지 렌더러(Immediate/Batch/Instanced) 중 선택, 런타임 전환 가능
17. **렌더러 접근**: `CS230::TextureManager::GetRenderer2D()`로 현재 활성화된 렌더러 접근

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

### 디버깅 및 함수 호출 추적
턴 관리 및 시스템 통합 디버깅 시:
1. `__PRETTY_FUNCTION__` 매크로와 Logger 사용하여 함수 호출 확인
2. 중복되는 기능 제거 (예: `OnTurnStart()` vs `RefreshActionPoints()`)
3. 각 턴 단계별 필수 작업을 플로우차트로 정리
4. 플로우차트의 각 항목과 실제 함수를 일대일 대응

```cpp
// 함수 호출 로깅 예시
void Character::OnTurnStart() {
    Engine::GetLogger().LogEvent(std::string(__PRETTY_FUNCTION__) + " called");
    // 턴 시작 로직
}
```

### 새 캐릭터 클래스 추가
1. `Objects/` 에 `MyCharacter.h/cpp` 생성 (첫 줄에 `#include "pch.h"` 추가)
2. `Character` 상속 및 필요한 컴포넌트 추가
3. CMake가 자동으로 파일 감지 (CMakeLists.txt 수동 편집 불필요)
4. `CharacterFactory.h/cpp`에 생성 메서드 추가
5. `CharacterTypes.h`에 새 타입 열거형 추가
6. 빌드 후 테스트

**예시**:
```cpp
// CharacterFactory.cpp에 추가
Wizard* CharacterFactory::CreateWizard(Math::ivec2 position) {
    Wizard* wizard = new Wizard(position);
    // JSON에서 스탯 로드 가능
    return wizard;
}

// GamePlay.cpp에서 사용
Character* wizard = CharacterFactory::Create(CharacterTypes::Wizard, position);
```

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

### 새 파일 추가 시 주의사항

1. **헤더 파일**: `.h` 확장자 사용 (`.hpp` 아님)
2. **Precompiled Header**: 모든 `.cpp` 파일 첫 줄에 `#include "pch.h"` 추가 필수
3. **CMake 자동 감지**: CMake가 GLOB_RECURSE로 파일을 자동 감지하므로 CMakeLists.txt 수동 편집 불필요
4. **재구성**: 새 파일 추가 후 `cmake --preset windows-debug` 실행하여 빌드 시스템 재구성
5. **메모리 관리**: 가능한 스마트 포인터 사용 (`std::unique_ptr`, `std::shared_ptr`)
6. **소유권 명확화**: 객체 생성 위치와 소유권 책임을 명확히 설계
7. **CharacterFactory 사용**: 새 캐릭터는 팩토리 패턴을 통해 생성

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

## 추가 참고 문서

### 아키텍처 및 리팩토링
- [architecture/engine-game-separation-plan.md](architecture/engine-game-separation-plan.md) - 🎯 **엔진-게임 분리 계획** (4-Phase 마이그레이션 가이드)
- [architecture/REFACTORING_TODO.md](architecture/REFACTORING_TODO.md) - 현재 진행 중인 리팩토링 작업
- [architecture/dragonic_tactics.md](architecture/dragonic_tactics.md) - 게임 디자인 문서 (한글)
- [architecture/game_architecture_rules.md](architecture/game_architecture_rules.md) - 아키텍처 원칙

### 구현 가이드
- [docs/implementation-plan.md](docs/implementation-plan.md) - 유연한 구현 계획 (우선순위 기반)
- [docs/Detailed Implementations/weeks/](docs/Detailed%20Implementations/weeks/) - 주차별 상세 구현 가이드 (한글)
