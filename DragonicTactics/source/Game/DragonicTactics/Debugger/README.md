# Debug System - ImGui 기반 통합 디버그 도구

## 개요
ImGui를 활용한 직관적이고 컴팩트한 디버그 시스템입니다.
이벤트 버스 구독 방식으로 작동하며 게임 코드 수정 없이 모든 디버그 정보를 자동 수집합니다.

**특징:**
- 🎮 **3개의 ImGui 창**으로 모든 기능 제어
- 🎯 **버튼/체크박스 기반** - 직관적인 UI
- 🔧 **비침투적** - 게임 코드 수정 불필요
- 📊 **이벤트 기반** - 자동 데이터 수집

## 시스템 구조

```
DebugManager (Singleton)
├── DebugConsole (owned)
│   ├── ImGui 입력창
│   ├── 명령어 시스템
│   └── 출력 로그 (색상 코딩)
│
└── DebugVisualizer (owned)
    ├── 이벤트 구독 (EventBus)
    ├── 데이터 수집 (자동)
    ├── 그리드 오버레이 (게임 화면)
    └── ImGui 통계 패널
```

## 초기화

게임 시작 시 한 번만 호출:

```cpp
void GamePlay::Load() {
    // 디버그 시스템 초기화 (이벤트 구독)
    Engine::GetDebugManager().Init();
}
```

## 통합 방법

### Update
```cpp
void GamePlay::Update(double dt) {
    // 디버그 시스템 업데이트
    Engine::GetDebugManager().Update(dt);
}
```

### Draw (게임 화면)
```cpp
void GamePlay::Draw() {
    // 그리드 오버레이만 게임 화면에 표시
    Engine::GetDebugManager().Draw(GetGSComponent<GridSystem>());
}
```

### DrawImGui (ImGui 창들)
```cpp
void GamePlay::DrawImGui() {
    // 모든 ImGui 디버그 창 표시
    Engine::GetDebugManager().DrawImGui(GetGSComponent<GridSystem>());
}
```

## 📦 3개의 ImGui 창

### 1️⃣ Debug Tools (메인 컨트롤 패널)
**위치:** 좌측 상단 (10, 10) | **크기:** 280x450

간결한 컨트롤 패널 - 모든 디버그 기능을 여기서 제어:

```
┌─ Debug Tools ────────────┐
│  [DEBUG MODE]  ← 큰 버튼 │
│                          │
│  Visualization           │
│  □ Grid Overlay          │
│  □ Collision Boxes       │
│  □ AI Paths              │
│  □ Status Info           │
│                          │
│  Panels                  │
│  □ Events & Stats        │
│  □ Console (Tab)         │
│                          │
│  Cheats                  │
│  □ God Mode              │
│                          │
│  Tab: Toggle Console     │
│  F1: Toggle This Panel   │
└──────────────────────────┘
```

**기능:**
- **DEBUG MODE 버튼**: 전체 디버그 ON/OFF (초록/회색)
- **체크박스**: 각 기능 개별 토글
- **직관적**: 클릭만으로 모든 제어

---

### 2️⃣ Events & Stats (통계 패널)
**위치:** 중앙 (300, 10) | **크기:** 650x600

4개 탭으로 구성된 정보 패널:

#### 📋 Event Log
- 최근 이벤트 (10초)
- 색상 구분 (Damage: 빨강, Death: 진빨강, Spell: 파랑, Turn: 녹색)
- 예: `[5.3s] Damage: Dragon dealt 15 damage to Fighter`

#### 🎲 Dice History
- 최근 20회 주사위 굴림
- 개별 값 + 합계
- 예: `[12.5s] Total: 18  Rolls: [6, 5, 4, 3]`

#### 👥 Characters
- 모든 캐릭터 상세 정보
- Position, HP, Attack, AP 등

#### ⚔️ Combat
- 전투 이벤트만 필터링
- 데미지/사망 로그

---

### 3️⃣ Console (명령어 창)
**위치:** 우측 하단 (660, 520) | **크기:** 500x300

깔끔한 명령어 실행 도구:

```
┌─ Console ────────────────────┐
│ > help                       │
│ Available commands:          │
│   help - Show commands       │
│   clear - Clear output       │
│   echo - Echo text           │
│                              │
│ [Input________]              │
└──────────────────────────────┘
```

**기본 명령어:**
- `help` - 명령어 목록
- `clear` - 출력 지우기
- `echo [text]` - 텍스트 출력

**특징:**
- ↑↓ 키로 명령어 히스토리
- 색상 코딩 (명령: 초록, 에러: 빨강, 경고: 노랑)
- 게임별 커스텀 명령어 추가 가능

---

## 🎮 단축키

| 키 | 기능 |
|---|---|
| **Tab** | Console 토글 |
| **F1** | Debug Tools 패널 토글 |

---

## 🎨 창 배치

```
┌─────────────┬──────────────────────────┐
│Debug Tools  │  Events & Stats          │
│  280x450    │     650x600              │
│  (10,10)    │    (300,10)              │
│             │                          │
│             │                          │
│             │                          │
│             │                          │
│             │                          │
│             │  ┌────────────────┐      │
│             │  │   Console      │      │
│             │  │   500x300      │      │
└─────────────┴──┤   (660,520)    │──────┘
                 └────────────────┘
```

---

## 🎯 게임 화면 오버레이

**Grid Overlay만 게임 화면에 표시됩니다:**
- 타일 경계선 (반투명 흰색)
- 8x8 그리드
- Debug Tools에서 체크박스로 ON/OFF

**나머지 모든 디버그 정보는 ImGui 창에만 표시됩니다.**

## 🔄 자동 수집 이벤트

### 1. CharacterDamagedEvent
- 공격자, 피해자, 데미지량 기록
- 이벤트 로그 자동 추가
- Combat 탭에 표시

### 2. CharacterMovedEvent
- 이동 경로 추적 (5초간)
- 이동 비용(AP) 기록

### 3. DiceManager (쿼리)
- 최근 20회 굴림 자동 저장
- Dice History 탭에 표시

### 4. SpellCastEvent
- 시전자, 주문명, 레벨 기록
- Event Log에 표시

### 5. TurnStartedEvent
- 턴 번호, 캐릭터, AP 정보
- 턴 흐름 추적

### 6. CharacterDeathEvent
- 사망자, 킬러 정보
- Combat 탭에 표시

---

## 🎛️ 기능 제어

모든 기능은 **Debug Tools 패널의 체크박스**로 제어:

| 체크박스 | 기능 |
|---------|------|
| Grid Overlay | 그리드 라인 표시 |
| Collision Boxes | 충돌 박스 시각화 |
| AI Paths | AI 경로 표시 |
| Status Info | 캐릭터 상태 정보 |
| Events & Stats | 통계 패널 열기 |
| Console | 콘솔 창 열기 |
| God Mode | 무적 모드 |

## ✨ 장점

### 1. 직관적인 UI
- 🎯 버튼/체크박스로 즉시 제어
- 📊 정보가 깔끔하게 정리됨
- 🖱️ 마우스로 창 이동/크기 조절 가능

### 2. 비침투적
- ✅ 게임 코드 수정 불필요
- ✅ 이벤트만 구독
- ✅ 자동 데이터 수집

### 3. 확장 가능
**콘솔 명령어 추가:**
```cpp
// DebugConsole::RegisterDefaultCommands()에서
RegisterCommand("spawn", [](std::vector<std::string> args) {
    // 캐릭터 소환 로직
}, "Spawn a character");
```

**새 이벤트 구독:**
```cpp
// DebugVisualizer::Init()에서
event_bus.Subscribe<NewEvent>([this](const NewEvent& e) {
    // 이벤트 처리
});
```

### 4. 성능 최적화
- 디버그 OFF 시 렌더링 스킵
- 오래된 데이터 자동 정리
- 메모리 사용량 제한

## 🚀 커스터마이징 예시

### 콘솔 명령어 추가
```cpp
// DebugConsole.cpp의 RegisterDefaultCommands()
RegisterCommand("tp", [](std::vector<std::string> args) {
    if (args.size() >= 2) {
        int x = std::stoi(args[0]);
        int y = std::stoi(args[1]);
        // 플레이어 텔레포트 로직
        output_log_.push_back("Teleported to (" + args[0] + ", " + args[1] + ")");
    }
}, "tp <x> <y> - Teleport player");
```

### 게임 상태 쿼리
```cpp
RegisterCommand("stat", [](std::vector<std::string>) {
    auto* grid = Engine::GetCurrentGameState()->GetGSComponent<GridSystem>();
    int alive = 0;
    // 살아있는 캐릭터 수 계산
    output_log_.push_back("Alive characters: " + std::to_string(alive));
}, "Show game statistics");
```

---

## ⚠️ 주의사항

1. **초기화 순서**
   - EventBus 준비 후 `Init()` 호출
   - 너무 일찍 호출하면 구독 실패

2. **포인터 안전성**
   - 캐릭터 삭제 후 이벤트 참조 주의
   - weak_ptr 사용 권장

3. **메모리 관리**
   - 이벤트 로그 자동 정리 (10초)
   - 주사위 히스토리 20개 제한

---

## 💡 팁

- **창 레이아웃 저장**: ImGui가 자동으로 `imgui.ini`에 저장
- **창 크기 조절**: 모서리 드래그
- **창 이동**: 타이틀바 드래그
- **탭 전환**: Events & Stats 창에서 탭 클릭
