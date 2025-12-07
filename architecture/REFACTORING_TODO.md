# 리팩토링 To-Do 리스트

**작성일**: 2025년 11월 19일
**기준**: Week 4 완료 시점
**목표**: Week 5 시작 전 주요 기술 부채 해결

---

## 📌 중요도 순 Task 목록

### 🔴 Critical - 즉시 해결 필요 (Week 5 시작 전)

---

## Task 1: 엔진-게임 의존성 역전 ⚡ 최우선

**중요도**: 🔴 Critical
**예상 시간**: 2-3시간
**담당자**: 전체 팀 (리팩토링 후 모든 코드 영향)

### What (무엇을)?

Engine.cpp가 게임 시스템(EventBus, CombatSystem 등)을 직접 소유하는 구조를 제거하고, 이들을 GameState 컴포넌트로 이동.

**현재 문제**:

```cpp
// Engine.cpp - 엔진이 게임에 의존
#include "Game/DragonicTactics/Singletons/CombatSystem.h"  // ❌
#include "Game/DragonicTactics/Singletons/EventBus.h"      // ❌

class Engine::Impl {
    EventBus eventbus{};         // ❌
    CombatSystem combatsystem{}; // ❌
};
```

**목표 구조**:

```
Engine (범용) ← 게임 의존성 제거
GameState (게임 로직) ← 게임 시스템 소유
```

### How (어떻게)?

#### Step 1: Engine.hpp에서 forward declaration 제거

**파일**: `DragonicTactics/source/Engine/Engine.hpp`

**변경 사항**:

```cpp
// Engine.hpp:36-42 - 삭제
class EventBus;         // ❌ 삭제
class CombatSystem;     // ❌ 삭제
class DiceManager;      // ❌ 삭제
class SpellSystem;      // ❌ 삭제
class DebugManager;     // ❌ 삭제
class DataRegistry;     // ❌ 삭제
class AISystem;         // ❌ 삭제
```

#### Step 2: Engine.cpp에서 include 제거

**파일**: `DragonicTactics/source/Engine/Engine.cpp`

**변경 사항**:

```cpp
// Engine.cpp:10-16 - 모두 삭제
#include "Game/DragonicTactics/Singletons/CombatSystem.h"   // ❌
#include "Game/DragonicTactics/Singletons/EventBus.h"       // ❌
#include "Game/DragonicTactics/Singletons/DiceManager.h"    // ❌
#include "Game/DragonicTactics/Singletons/SpellSystem.h"    // ❌
#include "Game/DragonicTactics/Debugger/DebugManager.h"     // ❌
#include "Game/DragonicTactics/Singletons/DataRegistry.h"   // ❌
#include "Game/DragonicTactics/Singletons/AISystem.h"       // ❌
```

#### Step 3: Engine::Impl 멤버 변수 제거

**파일**: `DragonicTactics/source/Engine/Engine.cpp`

**변경 사항**:

```cpp
// Engine.cpp:110-116 - 모두 삭제
class Engine::Impl {
    // EventBus eventbus{};          // ❌ 삭제
    // CombatSystem combatsystem{};  // ❌ 삭제
    // DiceManager dicemanager{};    // ❌ 삭제
    // SpellSystem spellsystem{};    // ❌ 삭제
    // DebugManager debugmanager{};  // ❌ 삭제
    // DataRegistry dataregistry{};  // ❌ 삭제
    // AISystem AIsystem{};          // ❌ 삭제
};
```

#### Step 4: Engine Getter 함수 제거

**파일**: `DragonicTactics/source/Engine/Engine.cpp` (하단)

**변경 사항**:

```cpp
// Engine.cpp - 아래 함수들 모두 삭제
EventBus& Engine::GetEventBus() { ... }           // ❌
CombatSystem& Engine::GetCombatSystem() { ... }   // ❌
DiceManager& Engine::GetDiceManager() { ... }     // ❌
SpellSystem& Engine::GetSpellSystem() { ... }     // ❌
DebugManager& Engine::GetDebugManager() { ... }   // ❌
DataRegistry& Engine::GetDataRegistry() { ... }   // ❌
AISystem& Engine::GetAISystem() { ... }           // ❌
```

#### Step 5: GamePlay.cpp에서 시스템 생성

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp`

**변경 사항**:

```c
// GamePlay.cpp::Load() - 추가
void GamePlay::Load() {
    // ✅ 게임 시스템을 GameState 컴포넌트로 추가
    AddGSComponent(new EventBus());
    AddGSComponent(new CombatSystem());
    AddGSComponent(new DiceManager());
    AddGSComponent(new AISystem());
    AddGSComponent(new SpellSystem());
    AddGSComponent(new DebugManager());
    AddGSComponent(new DataRegistry());

    // 기존 컴포넌트
    AddGSComponent(new GridSystem());
    AddGSComponent(new TurnManager());
    AddGSComponent(new CS230::GameObjectManager());

    // 이벤트 버스는 가장 먼저 초기화
    EventBus* event_bus = GetGSComponent<EventBus>();
    event_bus->Clear();

    // 나머지 로직...
}
```

#### Step 6: 모든 Engine::Get* 호출 변경

**영향받는 파일** (grep 결과 기준):

- `GamePlay.cpp`: ~30곳
- `Fighter.cpp`: ~10곳
- `Dragon.cpp`: ~5곳
- `Test/*.cpp`: ~20곳

**변경 방법**:

**변경 전**:

```cpp
Engine::GetCombatSystem().ExecuteAttack(dragon, target);
Engine::GetEventBus().Publish(event);
Engine::GetDiceManager().RollDice(3, 6);
```

**변경 후**:

```cpp
// GetGSComponent로 접근
CombatSystem* combat = GetGSComponent<CombatSystem>();
combat->ExecuteAttack(dragon, target);

EventBus* event_bus = GetGSComponent<EventBus>();
event_bus->Publish(event);

DiceManager* dice = GetGSComponent<DiceManager>();
int result = dice->RollDice(3, 6);
```

**일괄 변경 스크립트** (PowerShell):

```powershell
# 모든 .cpp 파일에서 Engine::Get* 패턴 찾기
Get-ChildItem -Path "DragonicTactics/source/Game" -Recurse -Filter *.cpp |
  Select-String "Engine::Get(CombatSystem|EventBus|DiceManager|AISystem|SpellSystem|DebugManager|DataRegistry)" |
  Format-Table Path, LineNumber, Line -AutoSize
```

#### Step 7: 컴파일 및 테스트

```bash
# 클린 빌드
cmake --build --preset windows-debug --clean-first

# 실행 테스트
./build/windows-debug/dragonic_tactics.exe

# 모든 테스트 실행 (GamePlay에서 F, E, T, Y, D, H, S, P 키)
```

### 체크리스트

- [ ] Engine.hpp에서 게임 시스템 forward declaration 제거
- [ ] Engine.cpp에서 게임 시스템 include 제거
- [ ] Engine::Impl에서 멤버 변수 제거
- [ ] Engine Getter 함수 제거
- [ ] GamePlay::Load()에 시스템 생성 추가
- [ ] GamePlay.cpp의 Engine::Get* 호출 변경 (~30곳)
- [ ] Fighter.cpp의 Engine::Get* 호출 변경 (~10곳)
- [ ] Dragon.cpp의 Engine::Get* 호출 변경 (~5곳)
- [ ] Test/*.cpp의 Engine::Get* 호출 변경 (~20곳)
- [ ] 컴파일 확인
- [ ] 런타임 테스트 (모든 키 입력 테스트)

---

## Task 2: Precompiled Header 구축 🚀 컴파일 속도 개선

**중요도**: 🔴 Critical (Task 1 완료 후)
**예상 시간**: 1-2시간
**담당자**: 빌드 담당자

### What (무엇을)?

자주 사용되는 헤더(STL, Engine, 게임 타입)를 미리 컴파일하여 컴파일 시간을 30-40% 단축.

**현재 문제**:

- GamePlay.cpp만 26개 include
- 전체 프로젝트 ~512개 include
- Clean Build: 85초

**목표**:

- GamePlay.cpp: 26개 → 8개 include (70% 감소)
- Clean Build: 85초 → 55초 (35% 단축)

### How (어떻게)?

#### Step 1: pch.h 파일 생성

**파일**: `DragonicTactics/source/pch.h` (신규)

**내용**:

```cpp
#pragma once

// ========== STL 헤더 (거의 변경 없음) ==========
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <array>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

// ========== 엔진 핵심 (변경 적음) ==========
#include "Engine/Engine.hpp"
#include "Engine/GameObject.h"
#include "Engine/GameState.hpp"
#include "Engine/Component.h"
#include "Engine/ComponentManager.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Input.hpp"
#include "Engine/Window.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Sprite.h"
#include "Engine/Animation.h"

// ========== 수학 (거의 변경 없음) ==========
#include "Engine/Vec2.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Rect.hpp"

// ========== 게임 타입 정의 (자주 사용) ==========
#include "Game/DragonicTactics/Types/GameTypes.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"
#include "Game/DragonicTactics/Types/GameObjectTypes.h"
#include "Game/DragonicTactics/Types/Events.h"

// ========== 외부 라이브러리 ==========
#include <SDL.h>
#include <imgui.h>

// ========== GSL (Guidelines Support Library) ==========
#include <gsl/gsl>
```

#### Step 2: CMakeLists.txt에 PCH 설정 추가

**파일**: `DragonicTactics/source/CMakeLists.txt`

**변경 사항**:

```cmake
# 기존 add_executable 아래에 추가

# Precompiled Header 적용
target_precompile_headers(dragonic_tactics PRIVATE pch.h)

# PCH에서 제외할 파일 (main.cpp 등)
set_source_files_properties(
    main.cpp
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)
```

#### Step 3: 기존 파일에 pch.h 추가

**모든 .cpp 파일의 첫 줄에 추가**:

```cpp
// GamePlay.cpp
#include "pch.h"  // ← 가장 먼저 추가
#include "GamePlay.h"

// 이제 중복 include 제거 가능
// #include <vector>              // ❌ 제거 (pch.h에 있음)
// #include "Engine/Engine.hpp"   // ❌ 제거
// #include "Engine/Logger.hpp"   // ❌ 제거
// #include "Engine/Input.hpp"    // ❌ 제거

// 특수한 헤더만 남김
#include "./CS200/IRenderer2D.hpp"
#include "../StateComponents/GridSystem.h"
#include "../Objects/Dragon.h"
```

#### Step 4: 일괄 변경 스크립트

**PowerShell 스크립트**:

```powershell
# 모든 .cpp 파일 첫 줄에 #include "pch.h" 추가
Get-ChildItem -Path "DragonicTactics/source/Game" -Recurse -Filter *.cpp | ForEach-Object {
    $content = Get-Content $_.FullName
    if ($content[0] -notlike '*pch.h*') {
        $newContent = '#include "pch.h"', '', $content
        Set-Content -Path $_.FullName -Value $newContent
    }
}
```

#### Step 5: 빌드 및 속도 측정

```bash
# 첫 번째 클린 빌드 (PCH 생성)
cmake --build --preset windows-debug --clean-first
# 예상: 95초 (+10초)

# 두 번째 클린 빌드 (PCH 재사용)
cmake --build --preset windows-debug --clean-first
# 예상: 55초 (35% 빠름)

# 증분 빌드 (파일 1개 수정 후)
# 예상: 8초 (기존 12초 → 33% 빠름)
```

### 체크리스트

- [ ] pch.h 파일 생성
- [ ] CMakeLists.txt에 `target_precompile_headers` 추가
- [ ] 모든 .cpp에 `#include "pch.h"` 추가 (스크립트 사용)
- [ ] 중복 include 제거 (vector, string, Engine.hpp 등)
- [ ] 첫 번째 클린 빌드 (PCH 생성 확인)
- [ ] 두 번째 클린 빌드 (속도 측정)
- [ ] 런타임 테스트 (정상 작동 확인)

---

## Task 3: GamePlay 리팩토링 (Mediator 패턴) 🏗️

**중요도**: 🟠 High
**예상 시간**: 4-5시간
**담당자**: GamePlay 담당자

### What (무엇을)?

GamePlay 클래스(330줄)를 3개의 책임별 클래스로 분할:

1. PlayerInputHandler - 플레이어 입력 처리
2. GamePlayUIManager - UI 관리
3. BattleOrchestrator - 전투 흐름 제어

**현재 문제**:

```cpp
// GamePlay.cpp - 330줄의 God Class
void GamePlay::Update(double dt) {
    // 1. 입력 처리 (130줄)
    if (input.MouseJustPressed(0)) { ... }
    switch (currentPlayerState) { ... }

    // 2. AI 처리 (30줄)
    switch (currentCharacter->GetCharacterType()) { ... }

    // 3. UI 업데이트 (20줄)
    for (auto& text : damage_texts) { ... }

    // 4. 컴포넌트 업데이트
    UpdateGSComponents(dt);
}
```

**목표 구조**:

```cpp
void GamePlay::Update(double dt) {
    m_input_handler->Update(dt, current_character);    // 입력만
    m_orchestrator->Update(dt);                        // 전투만
    m_ui_manager->Update(dt);                          // UI만
    UpdateGSComponents(dt);                            // 컴포넌트만
}
```

### How (어떻게)?

#### Step 1: PlayerInputHandler 클래스 생성

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/PlayerInputHandler.h` (신규)

```cpp
#pragma once
#include <memory>
#include "Engine/Vec2.hpp"

class Character;
class Dragon;
class GridSystem;

class PlayerInputHandler {
public:
    enum class ActionState {
        None,
        SelectingMove,
        Moving,
        SelectingAction,
        TargetingForAttack,
        TargetingForSpell
    };

    PlayerInputHandler();
    ~PlayerInputHandler() = default;

    void Update(double dt, Character* current_character, GridSystem* grid);
    void CancelCurrentAction();

    ActionState GetCurrentState() const { return m_state; }

private:
    ActionState m_state = ActionState::None;

    void HandleDragonInput(double dt, Dragon* dragon, GridSystem* grid);
    void HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon, GridSystem* grid);
    void HandleRightClick(Dragon* dragon);
};
```

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/PlayerInputHandler.cpp` (신규)

```cpp
#include "pch.h"
#include "PlayerInputHandler.h"
#include "../Objects/Dragon.h"
#include "../Objects/Components/MovementComponent.h"
#include "../StateComponents/GridSystem.h"

PlayerInputHandler::PlayerInputHandler() : m_state(ActionState::None) {}

void PlayerInputHandler::Update(double dt, Character* current_character, GridSystem* grid) {
    if (current_character->GetCharacterType() != CharacterTypes::Dragon) {
        return;  // AI 캐릭터는 무시
    }

    Dragon* dragon = static_cast<Dragon*>(current_character);
    HandleDragonInput(dt, dragon, grid);
}

void PlayerInputHandler::HandleDragonInput(double dt, Dragon* dragon, GridSystem* grid) {
    auto& input = Engine::GetInput();
    bool is_clicking_ui = ImGui::GetIO().WantCaptureMouse;

    // 우클릭: 취소
    if (input.MouseJustPressed(2)) {
        HandleRightClick(dragon);
        return;
    }

    // 이동 중 체크
    if (m_state == ActionState::Moving) {
        MovementComponent* move_comp = dragon->GetGOComponent<MovementComponent>();
        if (move_comp && !move_comp->IsMoving()) {
            Engine::GetLogger().LogEvent("Movement finished.");
            m_state = ActionState::None;
        }
        return;
    }

    // 좌클릭: 액션 실행
    if (input.MouseJustPressed(0) && !is_clicking_ui) {
        Math::vec2 mouse_pos = input.GetMousePos();
        HandleMouseClick(mouse_pos, dragon, grid);
    }
}

void PlayerInputHandler::HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon, GridSystem* grid) {
    // GamePlay.cpp의 mouse click 로직 이동
    Math::ivec2 grid_pos = ConvertScreenToGrid(mouse_pos);

    switch (m_state) {
        case ActionState::SelectingMove:
            // 이동 로직
            if (grid->IsWalkable(grid_pos)) {
                auto path = grid->FindPath(dragon->GetGridPosition()->Get(), grid_pos);
                dragon->SetPath(std::move(path));
                m_state = ActionState::Moving;
            }
            break;

        case ActionState::TargetingForAttack:
            // 공격 로직
            Character* target = grid->GetCharacterAt(grid_pos);
            if (target && target != dragon) {
                auto* combat = GetGSComponent<CombatSystem>();
                combat->ExecuteAttack(dragon, target);
                m_state = ActionState::None;
            }
            break;

        // ... 다른 케이스
    }
}

void PlayerInputHandler::HandleRightClick(Dragon* dragon) {
    if (m_state == ActionState::Moving) {
        dragon->GetGOComponent<MovementComponent>()->ClearPath();
    }
    m_state = ActionState::None;
}

void PlayerInputHandler::CancelCurrentAction() {
    m_state = ActionState::None;
}
```

#### Step 2: GamePlayUIManager 클래스 생성

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/GamePlayUIManager.h` (신규)

```cpp
#pragma once
#include <vector>
#include <string>
#include "Engine/Vec2.hpp"
#include "Engine/Matrix.hpp"

class GamePlayUIManager {
public:
    void ShowDamageText(int damage, Math::vec2 position, Math::vec2 size);
    void Update(double dt);
    void Draw(Math::TransformationMatrix camera_matrix);

private:
    struct DamageText {
        std::string text;
        Math::vec2 position;
        Math::vec2 size;
        double lifetime;
    };

    std::vector<DamageText> m_damage_texts;
};
```

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/GamePlayUIManager.cpp` (신규)

```cpp
#include "pch.h"
#include "GamePlayUIManager.h"

void GamePlayUIManager::ShowDamageText(int damage, Math::vec2 position, Math::vec2 size) {
    DamageText text{
        std::to_string(damage),
        position,
        size,
        1.0  // 1초 지속
    };
    m_damage_texts.push_back(text);
}

void GamePlayUIManager::Update(double dt) {
    // lifetime 감소
    for (auto& text : m_damage_texts) {
        text.lifetime -= dt;
    }

    // 만료된 텍스트 제거
    m_damage_texts.erase(
        std::remove_if(m_damage_texts.begin(), m_damage_texts.end(),
            [](const DamageText& text) { return text.lifetime <= 0; }),
        m_damage_texts.end()
    );
}

void GamePlayUIManager::Draw(Math::TransformationMatrix camera_matrix) {
    for (const auto& text : m_damage_texts) {
        // 텍스트 렌더링 로직
        Engine::GetTextManager().Draw(text.text, text.position, text.size);
    }
}
```

#### Step 3: BattleOrchestrator 클래스 생성

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/BattleOrchestrator.h` (신규)

```cpp
#pragma once
class TurnManager;

class BattleOrchestrator {
public:
    void Update(double dt, TurnManager* turn_manager);
    void HandleAITurn(Character* ai_character);
    bool CheckVictoryCondition();

private:
    int m_previous_round = 0;
};
```

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/BattleOrchestrator.cpp` (신규)

```cpp
#include "pch.h"
#include "BattleOrchestrator.h"
#include "../StateComponents/TurnManager.h"
#include "../Objects/Fighter.h"

void BattleOrchestrator::Update(double dt, TurnManager* turn_manager) {
    if (!turn_manager->IsCombatActive()) {
        return;
    }

    Character* current = turn_manager->GetCurrentCharacter();

    // 라운드 변경 체크
    int current_round = turn_manager->GetRoundNumber();
    if (current_round != m_previous_round) {
        // 라운드 시작 처리
        m_previous_round = current_round;
    }

    // AI 턴 처리
    if (current->GetCharacterType() != CharacterTypes::Dragon) {
        HandleAITurn(current);
    }
}

void BattleOrchestrator::HandleAITurn(Character* ai_character) {
    // GamePlay.cpp의 AI 로직 이동
    if (ai_character->GetCharacterType() == CharacterTypes::Fighter) {
        Fighter* fighter = static_cast<Fighter*>(ai_character);
        fighter->Action();

        // AI 행동 완료 체크
        if (!ShouldContinueTurn(fighter)) {
            auto* turn_mgr = GetGSComponent<TurnManager>();
            turn_mgr->EndCurrentTurn();
        }
    }
}

bool BattleOrchestrator::CheckVictoryCondition() {
    // 승리 조건 체크 로직
    return false;
}
```

#### Step 4: GamePlay.h 수정

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/GamePlay.h`

**변경 전**:

```cpp
class GamePlay : public CS230::GameState {
private:
    enum class PlayerActionState { ... };  // ❌ 제거
    PlayerActionState currentPlayerState;  // ❌ 제거

    struct DamageText { ... };             // ❌ 제거
    std::vector<DamageText> damage_texts;  // ❌ 제거

    Fighter* fighter;
    Dragon* dragon;
};
```

**변경 후**:

```cpp
#include <memory>
class PlayerInputHandler;
class GamePlayUIManager;
class BattleOrchestrator;

class GamePlay : public CS230::GameState {
public:
    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    void DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    // ✅ 책임 분리
    std::unique_ptr<PlayerInputHandler> m_input_handler;
    std::unique_ptr<GamePlayUIManager> m_ui_manager;
    std::unique_ptr<BattleOrchestrator> m_orchestrator;

    void OnCharacterDamaged(const CharacterDamagedEvent& event);

    Fighter* fighter = nullptr;
    Dragon* dragon = nullptr;
    bool game_end = false;
};
```

#### Step 5: GamePlay.cpp 수정

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp`

**변경**:

```cpp
#include "pch.h"
#include "GamePlay.h"
#include "PlayerInputHandler.h"
#include "GamePlayUIManager.h"
#include "BattleOrchestrator.h"

void GamePlay::Load() {
    // 헬퍼 클래스 초기화
    m_input_handler = std::make_unique<PlayerInputHandler>();
    m_ui_manager = std::make_unique<GamePlayUIManager>();
    m_orchestrator = std::make_unique<BattleOrchestrator>();

    // 기존 로직...
    AddGSComponent(new EventBus());
    // ...

    // 이벤트 구독
    auto* event_bus = GetGSComponent<EventBus>();
    event_bus->Subscribe<CharacterDamagedEvent>(
        [this](const CharacterDamagedEvent& event) {
            this->OnCharacterDamaged(event);
        }
    );
}

void GamePlay::Update(double dt) {
    auto* turn_mgr = GetGSComponent<TurnManager>();
    auto* grid = GetGSComponent<GridSystem>();
    Character* current = turn_mgr->GetCurrentCharacter();

    // ✅ 간결해진 Update
    m_input_handler->Update(dt, current, grid);      // 입력 처리
    m_orchestrator->Update(dt, turn_mgr);            // 전투 흐름
    m_ui_manager->Update(dt);                        // UI 업데이트
    UpdateGSComponents(dt);                          // 컴포넌트

    if (game_end) {
        // 종료 처리
    }
}

void GamePlay::Draw() {
    DrawGSComponents();
    m_ui_manager->Draw(GetCamera());
}

void GamePlay::OnCharacterDamaged(const CharacterDamagedEvent& event) {
    // 데미지 크기 계산
    float ratio = static_cast<float>(event.damageAmount) / event.target->GetMaxHP();
    Math::vec2 size = CalculateDamageTextSize(ratio);

    Math::vec2 position = event.target->GetGridPosition()->Get() * GridSystem::TILE_SIZE;
    m_ui_manager->ShowDamageText(event.damageAmount, position, size);
}

void GamePlay::Unload() {
    // unique_ptr 자동 정리
}
```

### 체크리스트

- [ ] PlayerInputHandler.h/cpp 생성
- [ ] GamePlayUIManager.h/cpp 생성
- [ ] BattleOrchestrator.h/cpp 생성
- [ ] GamePlay.h 멤버 변수 정리
- [ ] GamePlay.cpp::Load() 헬퍼 초기화
- [ ] GamePlay.cpp::Update() 간소화
- [ ] GamePlay.cpp::Draw() 수정
- [ ] 컴파일 확인
- [ ] 런타임 테스트
- [ ] 코드 라인 수 측정 (330줄 → ~100줄 확인)

---

## Task 4: CMakeLists.txt 자동화 ⚙️

**중요도**: 🟠 High
**예상 시간**: 30분
**담당자**: 빌드 담당자

### What (무엇을)?

파일 추가/삭제 시 CMakeLists.txt를 수동으로 수정하는 대신, GLOB_RECURSE로 자동 감지.

**현재 문제**:

- 새 파일 추가 시 CMakeLists.txt에 수동 추가 필요
- 파일 삭제 시 빌드 에러
- 협업 시 merge conflict 빈번

**목표**:

- 파일 추가/삭제 자동 감지
- CMakeLists.txt merge conflict 제거

### How (어떻게)?

#### Step 1: CMakeLists.txt 백업

```bash
cp DragonicTactics/source/CMakeLists.txt DragonicTactics/source/CMakeLists.txt.backup
```

#### Step 2: CMakeLists.txt 수정

**파일**: `DragonicTactics/source/CMakeLists.txt`

**변경 전** (추정):

```cmake
add_executable(dragonic_tactics
    main.cpp
    Engine/Engine.cpp
    Engine/GameObject.cpp
    # ... 수동으로 나열
)
```

**변경 후**:

```cmake
# ========== 소스 파일 자동 수집 ==========
file(GLOB_RECURSE ENGINE_SOURCES
    "Engine/*.cpp"
    "Engine/*.h"
    "Engine/*.hpp"
)

file(GLOB_RECURSE CS200_SOURCES
    "CS200/*.cpp"
    "CS200/*.hpp"
)

file(GLOB_RECURSE OPENGL_SOURCES
    "OpenGL/*.cpp"
    "OpenGL/*.hpp"
)

file(GLOB_RECURSE GAME_SOURCES
    "Game/DragonicTactics/*.cpp"
    "Game/DragonicTactics/*.h"
)

# ========== 실행 파일 생성 ==========
add_executable(dragonic_tactics
    main.cpp
    ${ENGINE_SOURCES}
    ${CS200_SOURCES}
    ${OPENGL_SOURCES}
    ${GAME_SOURCES}
)

# ========== IDE에서 폴더 구조 유지 ==========
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ENGINE_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CS200_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${OPENGL_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GAME_SOURCES})
```

#### Step 3: 빌드 테스트

```bash
# CMake 재구성
cmake --preset windows-debug

# 빌드
cmake --build --preset windows-debug

# 실행
./build/windows-debug/dragonic_tactics.exe
```

#### Step 4: 새 파일 추가 테스트

```cpp
// Test 1: 새 파일 추가
touch DragonicTactics/source/Game/DragonicTactics/Test/TestNew.cpp

// Test 2: CMake 재구성 (자동 감지 확인)
cmake --preset windows-debug

// Test 3: 빌드 (새 파일 포함 확인)
cmake --build --preset windows-debug
```

### 체크리스트

- [ ] CMakeLists.txt 백업
- [ ] GLOB_RECURSE로 소스 수집
- [ ] source_group으로 폴더 구조 유지
- [ ] 빌드 테스트
- [ ] 새 파일 추가 테스트
- [ ] 파일 삭제 테스트

---

### 🟡 Medium - 중기 개선 (첫 플레이테스트 후)

---

## Task 5: Character 중복 포인터 정리 🔧

**중요도**: 🟡 Medium
**예상 시간**: 1-2시간
**담당자**: Character 시스템 담당자

### What (무엇을)?

Character 클래스의 `m_movement_component` 포인터를 제거하고, 필요할 때 `GetGOComponent`로 접근.

**현재 문제**:

```cpp
// Character.h:102
MovementComponent* m_movement_component = nullptr;  // ❌ Dangling Pointer 위험
```

**문제 시나리오**:

```cpp
AddGOComponent(new MovementComponent(this));        // ComponentManager 소유
m_movement_component = GetGOComponent<MovementComponent>();  // Character도 참조

RemoveGOComponent<MovementComponent>();             // Manager가 delete
m_movement_component->SetPath(...);                 // 💥 Dangling Pointer!
```

**목표**:

- 안전한 컴포넌트 접근
- Single Source of Truth (ComponentManager만 소유)

### How (어떻게)?

#### Step 1: Character.h 수정

**파일**: `DragonicTactics/source/Game/DragonicTactics/Objects/Character.h`

**변경 사항**:

```cpp
// Line 102 - 삭제
class Character : public CS230::GameObject {
protected:
    // MovementComponent* m_movement_component = nullptr;  // ❌ 삭제
};
```

#### Step 2: Character.cpp 수정

**파일**: `DragonicTactics/source/Game/DragonicTactics/Objects/Character.cpp`

**변경 전**:

```cpp
// Line 101-104
void Character::SetPath(std::vector<Math::ivec2> path) {
    if (m_movement_component) {
        m_movement_component->SetPath(std::move(path));
    }
}
```

**변경 후**:

```cpp
void Character::SetPath(std::vector<Math::ivec2> path) {
    MovementComponent* movement = GetGOComponent<MovementComponent>();
    if (movement) {
        movement->SetPath(std::move(path));
    }
}
```

#### Step 3: 모든 m_movement_component 사용처 찾기

```bash
grep -rn "m_movement_component" DragonicTactics/source/Game/DragonicTactics/
```

**예상 결과**:

- Character.h:102 (선언)
- Character.cpp:101 (SetPath)
- Character.cpp:88 (SetGridSystem)

**모두 변경**:

```cpp
// SetGridSystem도 수정
void Character::SetGridSystem(GridSystem* grid) {
    m_gridSystem = grid;

    MovementComponent* move_comp = GetGOComponent<MovementComponent>();  // ✅ 변경
    if (move_comp) {
        move_comp->SetGridSystem(grid);
    }
}
```

### 체크리스트

- [ ] Character.h에서 `m_movement_component` 선언 제거
- [ ] Character.cpp::SetPath 수정
- [ ] Character.cpp::SetGridSystem 수정
- [ ] grep으로 모든 사용처 확인
- [ ] 컴파일 확인
- [ ] 런타임 테스트 (이동 기능 정상 작동 확인)

---

## Task 6: 헤더 확장자 통일 📄

**중요도**: 🟡 Medium
**예상 시간**: 2-3시간
**담당자**: 전체 팀

### What (무엇을)?

Engine 폴더의 `.hpp` 파일을 `.h`로 통일.

**현재 상황**:

```
Engine/Engine.hpp       ← .hpp (혼재)
Engine/GameObject.h     ← .h
Engine/GameState.hpp    ← .hpp
Engine/Input.hpp        ← .hpp
```

**목표**:

```
Engine/Engine.h         ← .h (통일)
Engine/GameObject.h     ← .h
Engine/GameState.h      ← .h
Engine/Input.h          ← .h
```

### How (어떻게)?

#### Step 1: 변경할 파일 목록 작성

```bash
find DragonicTactics/source/Engine -name "*.hpp" > hpp_files.txt
cat hpp_files.txt
```

**예상 결과** (19개):

```
Engine/Engine.hpp
Engine/GameState.hpp
Engine/Input.hpp
Engine/Logger.hpp
Engine/Matrix.hpp
Engine/Vec2.hpp
Engine/Rect.hpp
Engine/Font.hpp
Engine/Error.hpp
Engine/FPS.hpp
Engine/Path.hpp
Engine/Random.hpp
Engine/Texture.hpp
Engine/TextureManager.hpp
Engine/TextManager.hpp
Engine/Window.hpp
Engine/GameStateManager.hpp
# ... 등
```

#### Step 2: 파일 이름 변경 스크립트

**PowerShell**:

```powershell
# Engine 폴더의 모든 .hpp를 .h로 변경
Get-ChildItem -Path "DragonicTactics/source/Engine" -Filter *.hpp | ForEach-Object {
    $newName = $_.Name -replace '\.hpp$', '.h'
    Rename-Item -Path $_.FullName -NewName $newName
    Write-Host "Renamed: $($_.Name) -> $newName"
}
```

#### Step 3: Include 경로 일괄 변경

**모든 소스 파일에서 Engine/*.hpp → Engine/*.h**:

```powershell
# 모든 .cpp와 .h 파일에서 Engine/*.hpp 패턴 찾아서 변경
Get-ChildItem -Path "DragonicTactics/source" -Recurse -Include *.cpp,*.h | ForEach-Object {
    (Get-Content $_.FullName) -replace 'Engine/(\w+)\.hpp', 'Engine/$1.h' |
    Set-Content $_.FullName
}
```

#### Step 4: 빌드 및 확인

```bash
# CMake 재구성
cmake --preset windows-debug

# 빌드
cmake --build --preset windows-debug

# 변경 확인
find DragonicTactics/source/Engine -name "*.hpp"
# 결과: (empty) - 모두 .h로 변경됨
```

### 체크리스트

- [ ] hpp_files.txt 생성 (변경 대상 확인)
- [ ] 파일 이름 변경 스크립트 실행
- [ ] Include 경로 일괄 변경
- [ ] 빌드 확인
- [ ] 수동 확인: `grep -r "Engine/.*\.hpp" DragonicTactics/source/`
- [ ] Git commit

---

## Task 7: CharacterFactory 구현 🏭

**중요도**: 🟡 Medium
**예상 시간**: 3-4시간
**담당자**: Character 시스템 담당자

### What (무엇을)?

현재 `new Dragon()`, `new Fighter()`로 직접 생성하는 것을 팩토리 패턴으로 변경.

**현재 문제**:

```cpp
// GamePlay.cpp - 하드코딩
dragon = new Dragon(current_pos);
fighter = new Fighter(current_pos);
```

**목표**:

```cpp
// CharacterFactory 사용
dragon = CharacterFactory::Create(CharacterType::Dragon, current_pos);
fighter = CharacterFactory::Create(CharacterType::Fighter, current_pos);
```

**장점**:

- JSON에서 스탯 로드
- 일관된 초기화
- 테스트 용이성

### How (어떻게)?

#### Step 1: CharacterFactory.h 생성

**파일**: `DragonicTactics/source/Game/DragonicTactics/Factories/CharacterFactory.h` (신규)

```cpp
#pragma once
#include <memory>
#include "Engine/Vec2.hpp"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

class Character;
class Dragon;
class Fighter;

class CharacterFactory {
public:
    static Character* Create(CharacterTypes type, Math::ivec2 start_position);

    // 스탯 커스터마이징
    static Character* CreateWithStats(
        CharacterTypes type,
        Math::ivec2 start_position,
        const CharacterStats& custom_stats
    );

private:
    static Dragon* CreateDragon(Math::ivec2 position);
    static Fighter* CreateFighter(Math::ivec2 position);
    // 나중에 추가: Cleric, Wizard, Rogue
};
```

#### Step 2: CharacterFactory.cpp 생성

**파일**: `DragonicTactics/source/Game/DragonicTactics/Factories/CharacterFactory.cpp` (신규)

```cpp
#include "pch.h"
#include "CharacterFactory.h"
#include "../Objects/Dragon.h"
#include "../Objects/Fighter.h"
#include "../Singletons/DataRegistry.h"

Character* CharacterFactory::Create(CharacterTypes type, Math::ivec2 start_position) {
    switch (type) {
        case CharacterTypes::Dragon:
            return CreateDragon(start_position);

        case CharacterTypes::Fighter:
            return CreateFighter(start_position);

        // TODO: Week 5 이후 추가
        // case CharacterTypes::Cleric:
        //     return CreateCleric(start_position);

        default:
            Engine::GetLogger().LogError("Unknown character type in CharacterFactory");
            return nullptr;
    }
}

Character* CharacterFactory::CreateWithStats(
    CharacterTypes type,
    Math::ivec2 start_position,
    const CharacterStats& custom_stats
) {
    Character* character = Create(type, start_position);
    if (character) {
        // 커스텀 스탯 적용
        auto* stats_comp = character->GetStatsComponent();
        if (stats_comp) {
            stats_comp->SetStats(custom_stats);
        }
    }
    return character;
}

Dragon* CharacterFactory::CreateDragon(Math::ivec2 position) {
    // JSON에서 Dragon 스탯 로드
    // DataRegistry* registry = GetGSComponent<DataRegistry>();
    // CharacterStats stats = registry->GetCharacterStats("Dragon");

    // 현재는 기본값 사용
    Dragon* dragon = new Dragon(position);

    Engine::GetLogger().LogDebug("CharacterFactory: Created Dragon at (" +
        std::to_string(position.x) + ", " + std::to_string(position.y) + ")");

    return dragon;
}

Fighter* CharacterFactory::CreateFighter(Math::ivec2 position) {
    // JSON에서 Fighter 스탯 로드
    Fighter* fighter = new Fighter(position);

    Engine::GetLogger().LogDebug("CharacterFactory: Created Fighter at (" +
        std::to_string(position.x) + ", " + std::to_string(position.y) + ")");

    return fighter;
}
```

#### Step 3: GamePlay.cpp에서 사용

**파일**: `DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp`

**변경 전**:

```cpp
case 'd':
    dragon = new Dragon(current_pos);
    dragon->SetGridSystem(grid_system);
    go_manager->Add(dragon);
    break;

case 'f':
    fighter = new Fighter(current_pos);
    fighter->SetGridSystem(grid_system);
    go_manager->Add(fighter);
    break;
```

**변경 후**:

```cpp
#include "../Factories/CharacterFactory.h"

// ...

case 'd':
    dragon = static_cast<Dragon*>(
        CharacterFactory::Create(CharacterTypes::Dragon, current_pos)
    );
    dragon->SetGridSystem(grid_system);
    go_manager->Add(dragon);
    break;

case 'f':
    fighter = static_cast<Fighter*>(
        CharacterFactory::Create(CharacterTypes::Fighter, current_pos)
    );
    fighter->SetGridSystem(grid_system);
    go_manager->Add(fighter);
    break;
```

#### Step 4: DataRegistry 연동 (선택 사항)

**JSON 파일 예시**: `DragonicTactics/Assets/Data/characters.json`

```json
{
    "Dragon": {
        "max_hp": 140,
        "current_hp": 140,
        "base_attack": 3,
        "attack_dice": "3d6",
        "base_defend": 2,
        "defend_dice": "2d6",
        "speed": 5,
        "attack_range": 1
    },
    "Fighter": {
        "max_hp": 90,
        "current_hp": 90,
        "base_attack": 2,
        "attack_dice": "2d8",
        "base_defend": 1,
        "defend_dice": "1d6",
        "speed": 3,
        "attack_range": 1
    }
}
```

**CharacterFactory.cpp 수정**:

```cpp
Dragon* CharacterFactory::CreateDragon(Math::ivec2 position) {
    // DataRegistry에서 스탯 로드
    DataRegistry* registry = /* GetGSComponent or Singleton */;
    CharacterStats stats = registry->LoadCharacterStats("Dragon");

    // 커스텀 스탯으로 생성
    Dragon* dragon = new Dragon(position);
    dragon->GetStatsComponent()->SetStats(stats);

    return dragon;
}
```

### 체크리스트

- [ ] CharacterFactory.h 생성
- [ ] CharacterFactory.cpp 생성
- [ ] GamePlay.cpp에서 팩토리 사용
- [ ] 컴파일 확인
- [ ] 런타임 테스트 (캐릭터 생성 확인)
- [ ] (선택) JSON 연동 구현
- [ ] (선택) characters.json 파일 작성

---

### 🟢 Low - 장기 개선 (Week 10 이후)

---

## Task 8-11: 장기 계획

아래 작업들은 Week 10 이후 여유가 있을 때 진행:

### Task 8: StatsComponent 강화

- TakeDamage(), ReceiveHeal() 로직을 StatsComponent로 이동
- Character는 StatsComponent 호출만

### Task 9: ActionComponent 분리

- PerformAction() 로직을 ActionComponent로 분리
- m_action_list도 ActionComponent로 이동

### Task 10: 메모리 관리 현대화

- `new/delete` → `std::unique_ptr/shared_ptr`
- GameObjectManager에 스마트 포인터 도입

### Task 11: EventBus 구독 해제 RAII

- EventSubscription 클래스 생성
- 자동 구독 해제 메커니즘

---

## 📊 전체 진행 상황 추적

### Week 5 목표

| Task                  | 우선순위        | 예상 시간 | 상태        | 담당자 |
| --------------------- | ----------- | ----- | --------- | --- |
| Task 1: 의존성 역전        | 🔴 Critical | 2-3h  | ⬜ Pending | -   |
| Task 2: PCH 구축        | 🔴 Critical | 1-2h  | ⬜ Pending | -   |
| Task 3: GamePlay 리팩토링 | 🟠 High     | 4-5h  | ⬜ Pending | -   |
| Task 4: CMake 자동화     | 🟠 High     | 30m   | ⬜ Pending | -   |

**총 예상 시간**: 8-10.5시간

### 체크리스트 진행률

- Critical Tasks: 0 / 2 (0%)
- High Tasks: 0 / 2 (0%)
- Medium Tasks: 0 / 3 (0%)

---

## 🎯 작업 순서 권장

### Day 1 (4-5시간)

1. ✅ Task 1: 엔진-게임 의존성 역전 (2-3h)
2. ✅ Task 2: Precompiled Header 구축 (1-2h)

### Day 2 (5-6시간)

3. ✅ Task 3: GamePlay 리팩토링 (4-5h)
4. ✅ Task 4: CMake 자동화 (30m)

### Day 3 (필요 시)

5. ✅ Task 5: Character 포인터 정리 (1-2h)
6. ✅ Task 6: 헤더 확장자 통일 (2-3h)

---

**작성일**: 2025-11-19
**다음 업데이트**: Task 1-4 완료 후
**문의**: 팀 리드에게 Slack으로 연락
