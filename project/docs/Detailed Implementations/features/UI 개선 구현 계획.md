

# UI 개선 구현 계획

## Context

현재 UI는 왼쪽 세로열에 Move/Action/End Turn 메인 버튼 + 그 위에 세로로 쌓인 스펠 목록 구조다. ui_demo.cpp(레이아웃 프로토타입), ui_draft.txt(한글 설계 명세), ui_draft_design.png(목업)를 바탕으로 모든 기존 정보는 유지하면서 위치·구성을 전면 개편한다.

주요 변경 결정:

- **Move 버튼 폐지**: Dragon 타일 위에 마우스 올리면 이동 가능 범위 미리 표시, Dragon 타일 클릭 시 이동 선택 모드 진입 (버튼 없이 직관적 UX)
- **Teleport 슬롯 없음**: Dragon은 Teleport(S_GEO_030)를 사용하지 않음
- 결과: 슬롯 정확히 10개 + End Turn → 원본 demo prototype과 완전히 동일한 N=10

---

## 영향 파일

| 파일                              | 변경 규모                                      |
| ------------------------------- | ------------------------------------------ |
| `States/GamePlayUIManager.h`    | 멤버 변수·메서드 추가                               |
| `States/GamePlayUIManager.cpp`  | `InitButtons`, `Update`, `Draw`, 신규 메서드 4개 |
| `States/PlayerInputHandler.cpp` | 버튼 ID rename + Move 버튼→Dragon클릭 트리거 교체     |
| `States/ButtonManager.h/.cpp`   | 변경 없음                                      |
| `States/GamePlay.cpp`           | 변경 없음                                      |

---

## 슬롯 바 레이아웃

**기준**: tile_size = 64, 가상 해상도 1600×900, Y-up 좌표계 (position.y = 버튼 상단)

```
슬롯바 외곽 박스:
  box_x      = 64                  (1 tile 좌측 여백)
  box_bottom = 64                  (1 tile 하단 여백, Y-up)
  bar_height = 96                  (1.5 tile)
  bar_width  = win.x - 128         (양쪽 1 tile 여백 = 1472 px at 1600)

N = 10 (Attack + 스펠 9개)
  slot_w = slot_h = 64
  end_btn_w = 128 (2 tiles)

remaining = 1472 - 10*64 - 128 = 704 px
offset    = 704 / 12 ≈ 58.67 px   (N+2=12, demo와 동일 공식)

slot_x[i]    = box_x + offset + i*(64+offset)
end_btn_x    = slot_x[9] + 64 + offset
slot_center_y = 64 + 48 = 112   (box_bottom + bar_height/2)
```

**슬롯 매핑** (N=10):

| idx | ID               | 라벨       | 아이콘                          |
| --- | ---------------- | -------- | ---------------------------- |
| 0   | `slot_attack`    | —        | `dragon_attack.png`          |
| 1   | `slot_S_ATK_010` | —        | `dragon_firebolt.png`        |
| 2   | `slot_S_ATK_020` | —        | `dragon_tail_swipe.png`      |
| 3   | `slot_S_ATK_030` | —        | `dragon_fury.png`            |
| 4   | `slot_S_ATK_040` | —        | `dragon_meteor.png`          |
| 5   | `slot_S_ENH_040` | —        | `dragon_mana_conversion.png` |
| 6   | `slot_S_ENH_050` | —        | `dragon_purify.png`          |
| 7   | `slot_S_DEB_020` | —        | `dragon_fearful_cry.png`     |
| 8   | `slot_S_GEO_010` | —        | `dragon_magma_blast.png`     |
| 9   | `slot_S_GEO_020` | —        | `dragon_wall_creation.png`   |
| —   | `slot_end_turn`  | End Turn | `turn_end.png`               |

아이콘 있는 슬롯은 `label = ""`, 텍스처가 시각적 식별자 역할.

---

## Step 1: GamePlayUIManager.h — 신규 멤버

```cpp
// 슬롯 아이콘 텍스처 (인덱스 0~9)
std::vector<std::shared_ptr<CS230::Texture>> slot_icons_;

// 슬롯 좌표 캐시 (팝업·오버레이에서 재사용)
std::array<double, 11> slot_bar_x_{};  // [0..9]=슬롯, [10]=End Turn
double slot_bar_center_y_ = 0.0;

// 업캐스트 팝업 상태
bool        popup_open_       = false;
std::string popup_spell_id_;
int         popup_slot_index_ = -1;
bool        popup_hit_this_frame_ = false;

// 호버 툴팁
Character* hovered_character_ = nullptr;

// InputHandler 포인터
PlayerInputHandler* m_input_handler_ptr_ = nullptr;

// 신규 private 메서드
void DrawSlotBar();
void DrawUicastPopup();
void DrawTurnIndicator();
void DrawHoverTooltip();
```

---

## Step 2: InitButtons() 전면 재작성

```cpp
void GamePlayUIManager::InitButtons(PlayerInputHandler* inputHandler) {
    m_input_handler_ptr_ = inputHandler;
    button_manager_.ClearAll();

    auto win = Engine::GetWindow().GetSize();
    constexpr int TILE = 64;
    const double box_x   = TILE;
    const double bar_bot = TILE;          // Y-up: 슬롯 하단 Y
    const double bar_h   = TILE * 1.5;   // 96px
    const double bar_w   = win.x - 2.0*TILE;
    constexpr int N = 10;
    const double remaining = bar_w - N*TILE - 2*TILE;
    const double offset    = remaining / (N + 2);

    slot_bar_center_y_ = bar_bot + bar_h * 0.5;  // = 64 + 48 = 112

    // 좌표 캐시
    for (int i = 0; i < N; ++i)
        slot_bar_x_[i] = box_x + offset + i * (TILE + offset);
    slot_bar_x_[N] = slot_bar_x_[N-1] + TILE + offset; // End Turn

    // 아이콘 텍스처 로드
    const std::array<std::string, 10> ICON_PATHS = {
        "Assets/images/dragon_attack.png",
        "Assets/images/dragon_firebolt.png",
        "Assets/images/dragon_tail_swipe.png",
        "Assets/images/dragon_fury.png",
        "Assets/images/dragon_meteor.png",
        "Assets/images/dragon_mana_conversion.png",
        "Assets/images/dragon_purify.png",
        "Assets/images/dragon_fearful_cry.png",
        "Assets/images/dragon_magma_blast.png",
        "Assets/images/dragon_wall_creation.png",
    };
    slot_icons_.resize(10, nullptr);
    for (int i = 0; i < 10; ++i)
        slot_icons_[i] = Engine::GetTextureManager().Load(ICON_PATHS[i]);

    // 10개 슬롯 등록
    const std::array<std::string,10> IDS = {
        "slot_attack","slot_S_ATK_010","slot_S_ATK_020","slot_S_ATK_030","slot_S_ATK_040",
        "slot_S_ENH_040","slot_S_ENH_050","slot_S_DEB_020","slot_S_GEO_010","slot_S_GEO_020"
    };
    for (int i = 0; i < N; ++i) {
        Button b;
        b.id       = IDS[i];
        b.position = { slot_bar_x_[i], slot_bar_center_y_ + TILE*0.5 }; // top-left
        b.size     = { TILE, TILE };
        b.label    = "";
        b.on_click = /* 각 슬롯별 람다 (아래 참조) */;
        button_manager_.AddButton(b);
    }

    // End Turn 버튼
    Button end;
    end.id       = "slot_end_turn";
    end.position = { slot_bar_x_[N], slot_bar_center_y_ + TILE*0.5 };
    end.size     = { TILE*2.0, TILE };
    end.label    = "End Turn";
    end.on_click = [inputHandler]() { inputHandler->OnEndTurnPressed(); };
    button_manager_.AddButton(end);

    // 배틀 로그 토글 (64×64)
    Button log_btn;
    log_btn.id       = "btn_battle_log";
    log_btn.position = { win.x - 64.0, win.y*0.5 + 32.0 };
    log_btn.size     = { 64.0, 64.0 };
    log_btn.label    = "<";
    log_btn.on_click = [this]() {
        show_battle_log_ = !show_battle_log_;
        button_manager_.SetLabel("btn_battle_log", show_battle_log_ ? ">" : "<");
    };
    button_manager_.AddButton(log_btn);
}
```

**on_click 람다 패턴**:

- `slot_attack`: `inputHandler->OnAttackPressed()`
- 단일 레벨 스펠(S_ATK_020, S_ENH_050, S_DEB_020): `inputHandler->SelectSpell(id, char, level, btns)`
- 업캐스트 스펠: `popup_open_ = !popup_open_; popup_spell_id_ = id; popup_slot_index_ = i;`

**슬롯 비활성화 조건** (Update에서 매 프레임 갱신):

| 슬롯 ID                                                | 비활성 조건                          |
| ---------------------------------------------------- | ------------------------------- |
| `slot_attack`                                        | `actor->GetActionPoints() == 0` |
| `slot_S_ATK_020`, `slot_S_ENH_050`, `slot_S_DEB_020` | `!slots->HasSlot(min_level)`    |
| `slot_S_ATK_010`                                     | `!slots->HasSlot(1)`            |
| `slot_S_ATK_030`, `slot_S_ATK_040`                   | `!slots->HasSlot(3)`            |
| `slot_S_ENH_040`, `slot_S_GEO_010`, `slot_S_GEO_020` | `!slots->HasSlot(1 or 2)`       |

---

## Step 3: PlayerInputHandler.cpp 변경

### 3-A. 버튼 ID rename (일괄 치환)

```
"btn_attack"   → "slot_attack"
"btn_end_turn" → "slot_end_turn"
"btn_wall_confirm" → "slot_wall_confirm"
```

### 3-B. Move 버튼 → Dragon 타일 클릭 트리거 교체

기존 `"btn_move"` 참조 모두 제거. 대신 `None` 상태에서 마우스 클릭 감지:

```cpp
// PlayerInputHandler::Update() 내 ActionState::None 분기에 추가
if (state_ == ActionState::None && mouse_clicked) {
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    Math::ivec2 clicked_tile = screen_to_grid(mouse_pos);   // 기존 helper 재사용
    Character* player = /* turnMgr->GetCurrentCharacter() */;

    if (player && !player->IsAIControlled() &&
        clicked_tile == player->GetGridPosition()->Get() &&
        player->GetMovementRange() > 0) {
        SetState(ActionState::SelectingMove);
        grid->EnableMovementMode(clicked_tile, player->GetMovementRange());
    }
}
```

### 3-C. 제거

- `"btn_move"`, `"btn_action"`, `"btn_spell"`, `"btn_spell_cancel"` 참조 전체 제거
- `HideAllSpellButtons()` 함수 삭제
- 구 스펠 서브버튼(`lbl_S_ATK_010`, `S_ATK_010_lv1~5` 등) 참조 전체 제거

---

## Step 4: Update() 변경

### 4-A. 슬롯 비활성화 (구 `_lv` 접미사 루프 교체)

```cpp
// 현재 턴 캐릭터의 SpellSlots 조회 후 각 슬롯별 disabled 설정
```

### 4-B. 업캐스트 팝업 외부 클릭 닫기

```cpp
popup_hit_this_frame_ = false;
// ... button_manager_.Update() 호출 ...
// ... DrawUicastPopup()에서 popup_hit_this_frame_ = true 설정 ...
if (mouse_clicked && popup_open_ && !popup_hit_this_frame_)
    popup_open_ = false;
```

### 4-C. Dragon 호버 → 이동 범위 미리 표시

```cpp
auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
bool in_move_state = (m_input_handler_ptr_ &&
    m_input_handler_ptr_->GetCurrentState() == ActionState::SelectingMove);

if (!in_move_state) {
    if (hovered_character_ && !hovered_character_->IsAIControlled()) {
        // Dragon에 호버 중 → 이동 범위 미리 보기
        grid->EnableMovementMode(
            hovered_character_->GetGridPosition()->Get(),
            hovered_character_->GetMovementRange());
    } else {
        grid->DisableMovementMode();
    }
}
// hovered_character_ 감지 (GridSystem::GetCharacterAt 사용)
hovered_character_ = nullptr;
Math::ivec2 gp = { int(mouse.x / GridSystem::TILE_SIZE),
                   int(mouse.y / GridSystem::TILE_SIZE) };
if (grid->IsValidTile(gp))
    hovered_character_ = grid->GetCharacterAt(gp);
```

---

## Step 5: DrawTurnIndicator() — 신규

```cpp
// TurnManager에서 현재 턴 캐릭터·번호·라운드 조회
// 상단 중앙 패널: W=320 (5 tiles), H=48, top = win.y - 8
// 배경: 0x1a1a2ecc, 테두리: 0x5555aaff
// 주 텍스트: "Turn 3 | Dragon's Turn" (CS200::GOLD, scale 0.45)
// 보조 텍스트: "Round 1" (CS200::WHITE, scale 0.35)
// IsCombatActive() false이면 return
```

---

## Step 6: DrawHoverTooltip() — DrawCharacterStatsPanel 교체

`DrawCharacterStatsPanel()` 본문 전체 삭제 → `DrawHoverTooltip()` 신규 작성.

```
hovered_character_ == nullptr → return
툴팁 크기: W=256, H=200
앵커: 마우스 오른쪽 +72px, 화면 밖이면 왼쪽으로 flip
배경: 0x1a1a2edd / 테두리: 0x8888aaff
내용 (LH=26px):
  Name     (scale 0.5, GOLD)
  HP: x/y  (scale 0.4, RED)
  AP: x    (scale 0.4, YELLOW)
  Speed: x (scale 0.4, GREEN)
  Slots: L1:x/y ...  (scale 0.4, ORANGE)
  FX: name(dur) ...  (scale 0.4, PINK)
```

Dragon 호버 시 이동 범위 미리보기는 Update()에서 처리 → DrawHoverTooltip에서 추가 로직 없음.

---

## Step 7: DrawUicastPopup() — 신규

```
popup_open_ false → return
업캐스트 범위 테이블:
  slot_S_ATK_010: Lv1~5
  slot_S_ATK_030: Lv3~5
  slot_S_ATK_040: Lv3~5
  slot_S_ENH_040: Lv0~5
  slot_S_GEO_010: Lv2~5
  slot_S_GEO_020: Lv1~5

팝업 버튼: W=48, H=36, GAP=4
앵커: slot_bar_x_[popup_slot_index_] + 32, 슬롯 바 상단 + 8px
배경 패널: 0x1a1a2ecc

각 레벨 버튼:
  DrawRectangle (hover: 마우스 위치로 직접 판별)
  클릭 시 → SelectSpell(spell_id, current_char, level, btns)
           → popup_open_ = false
  클릭 감지 시 popup_hit_this_frame_ = true 설정
```

`ButtonManager`에 등록하지 않고 DrawRectangle + 인라인 클릭 판별로 구현 (ButtonManager::IsPointInButton과 동일 공식: x in [left, left+w], y in [bottom-h, bottom]).

---

## Step 8: DrawSlotBar() — 신규 (아이콘 오버레이)

```cpp
void GamePlayUIManager::DrawSlotBar()
{
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    auto  win      = Engine::GetWindow().GetSize();
    constexpr double TILE = 64.0;

    // 1. 슬롯 바 배경 반투명 패널
    double bar_w = win.x - 2.0 * TILE;
    Math::TransformationMatrix bg =
        Math::TranslationMatrix(Math::vec2{ win.x * 0.5, slot_bar_center_y_ }) *
        Math::ScaleMatrix(Math::vec2{ bar_w, TILE * 1.5 });
    renderer->DrawRectangle(bg, 0x1a1a2e99, 0x5555aaff, 1.5, DrawDepth::UI + 0.001f);

    // 2. 각 슬롯 위에 아이콘 오버레이
    // 패턴: Engine::GetTextureManager().Load() + texture->Draw(TransformationMatrix)
    // (RenderingTest::Draw()의 testTexture->Draw(TranslationMatrix(...)) 참조)
    for (int i = 0; i < 10; ++i) {
        if (!slot_icons_[i]) continue;
        double cx = slot_bar_x_[i] + 32.0;  // 슬롯 좌상단 X + 32 = 중심 X
        Math::TransformationMatrix icon_t =
            Math::TranslationMatrix(Math::vec2{ cx, slot_bar_center_y_ }) *
            Math::ScaleMatrix(Math::vec2{ 60.0, 60.0 });  // 64px 슬롯 안에 4px 패딩
        slot_icons_[i]->Draw(icon_t);
    }
}
```

**로드 패턴** (InitButtons에서):

```cpp
// RenderingTest::Load()와 동일 패턴
slot_icons_[i] = Engine::GetTextureManager().Load(ICON_PATHS[i]);
```

`button_manager_.Draw()` (슬롯 색상 사각형 렌더)는 `Draw()` 맨 앞에서 호출 → 아이콘이 그 위에 올라감.

---

## Step 9: DrawBattleLog() 크기 상수 변경

```cpp
// 변경 전:
constexpr double PANEL_W_RATIO = 0.30;
constexpr double PANEL_H_RATIO = 0.60;
constexpr double BTN_W = 40.0;

// 변경 후:
constexpr double PANEL_W = 5 * 64.0;  // 320 px (5 tiles)
constexpr double PANEL_H = 8 * 64.0;  // 512 px (8 tiles)
constexpr double BTN_W   = 64.0;       // 1 tile
// PANEL_X = win.x - BTN_W - PANEL_W - 2.0  (동일 공식, 상수만 교체)
```

---

## Step 10: Draw() 호출 순서

```cpp
void GamePlayUIManager::Draw(Math::TransformationMatrix camera_matrix)
{
    button_manager_.Draw(camera_matrix);  // 슬롯 사각형 + End Turn + 로그 토글
    DrawSlotBar();                         // 배경 패널 + 아이콘 오버레이
    DrawUicastPopup();                     // 업캐스트 레벨 팝업
    DrawTurnIndicator();                   // 상단 중앙 턴 표시
    DrawHoverTooltip();                    // 마우스 오버 스탯 툴팁
    DrawBattleLog();                       // 320×512 로그 패널

    // 이하 기존 동작 유지
    // damage_texts, damage_log, game_end_text, spell_logs 렌더
}
```

---

## 유지되는 것 (변경 없음)

- `ShowDamageText`, `ShowDamageLog`, `ShowGameEnd` 및 관련 Update 루프
- `m_spell_logs` 골드 텍스트 (X=800, Y=50 기준선)
- `AddSpellLog`, `OnTurnStarted`, `AddBattleLogEntry`
- `DrawBattleLog()` 내부 텍스트 루프 (크기 상수만 변경)
- `GamePlay.cpp::DrawImGui()` — 개발자 Map Selection + Combat Status 유지
- `TurnManager`, `GridSystem`, `SpellSystem`, `CombatSystem` — 수정 없음

---

## 검증 방법

1. `cd DragonicTactics && cmake --build --preset windows-debug`
2. 실행 후:
   - 하단 슬롯 바 10개 슬롯(아이콘) + End Turn 표시 확인
   - Dragon 타일 위에 마우스 → 이동 범위 타일 하이라이트 + 스탯 툴팁 표시 확인
   - Dragon 타일 클릭 → 이동 선택 모드 진입, 목적지 클릭 시 이동 확인
   - 업캐스트 스펠 슬롯(Fire Bolt 등) 클릭 → 레벨 팝업 표시, 레벨 선택 시 시전 확인
   - 단일 레벨 스펠(Tail Swipe 등) 클릭 → 즉시 타겟팅 모드 진입 확인
   - 우측 배틀 로그 토글 버튼(64px) 클릭 → 320×512 패널 열림 확인
   - 화면 상단 중앙 턴 인디케이터 확인 (Dragon/Fighter/Cleric 턴 전환 시)
   - AI 턴 중 슬롯 비활성화(disabled 색상) 확인
