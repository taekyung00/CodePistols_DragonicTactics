# UI 개선 구현 계획

## Context

현재 UI는 왼쪽 세로열에 Move/Action/End Turn 메인 버튼 + 그 위에 세로로 쌓인 스펠 목록 구조다. ui_demo.cpp(레이아웃 프로토타입), ui_draft.txt(한글 설계 명세), ui_draft_design.png(목업)를 바탕으로 모든 기존 정보는 유지하면서 위치·구성을 전면 개편한다.

목표:

- 화면 하단 **수평 슬롯 바**: 모든 Dragon 액션을 한 줄로 나열
- 캐릭터 **호버 툴팁**: 항상-표시 스탯 패널 → 마우스 오버 시에만 표시
- **배틀 로그** 크기 재조정: 5타일×8타일(320×512px), 토글 버튼 1타일(64×64px)
- **턴 인디케이터**: 화면 상단 중앙, 항상 표시 (기존에는 개발자 ImGui에만 표시)

---

## 영향 파일

| 파일                              | 변경 규모                                      |
| ------------------------------- | ------------------------------------------ |
| `States/GamePlayUIManager.h`    | 멤버 변수·메서드 추가                               |
| `States/GamePlayUIManager.cpp`  | `InitButtons`, `Update`, `Draw`, 3개 신규 메서드 |
| `States/PlayerInputHandler.cpp` | 버튼 ID 일괄 rename + 구 스펠 서브메뉴 제거             |
| `States/ButtonManager.h/.cpp`   | 변경 없음                                      |
| `States/GamePlay.cpp`           | 변경 없음 (DrawImGui 유지)                       |

---

## 슬롯 바 레이아웃 (핵심)

**기준**: tile_size = 64, 가상 해상도 1600×900, Y-up 좌표계 (position.y = 버튼 상단)

```
슬롯바 외곽 박스:
  box_x      = 64                   (1 tile 좌측 여백)
  box_bottom = 64                   (1 tile 하단 여백, Y-up 기준)
  bar_height = 96                   (1.5 tile)
  bar_width  = win.x - 128          (양쪽 1 tile 여백)

슬롯 개수 N = 12 (Move, Attack, 스펠 10개)
  slot_w = slot_h = 64
  end_btn_w = 128  (2 tiles)

remaining = bar_width - N*64 - 128
          = (1600-128) - 768 - 128 = 576 px
offset    = remaining / (N+2) = 576/14 ≈ 41 px

slot_x[i] = box_x + offset + i*(64+offset)
end_btn_x  = slot_x[11] + 64 + offset
slot_center_y = 64 + 48   (box_bottom + half bar_height = 112)
```

**슬롯 매핑** (인덱스 0~11, 왼→오른):

| idx | ID               | 라벨            | 아이콘                          |
| --- | ---------------- | ------------- | ---------------------------- |
| 0   | `slot_move`      | Move          | —                            |
| 1   | `slot_attack`    | Attack        | `dragon_attack.png`          |
| 2   | `slot_S_ATK_010` | Fire Bolt     | `dragon_firebolt.png`        |
| 3   | `slot_S_ATK_020` | Tail Swipe    | `dragon_tail_swipe.png`      |
| 4   | `slot_S_ATK_030` | Dragon's Fury | `dragon_fury.png`            |
| 5   | `slot_S_ATK_040` | Meteor        | `dragon_meteor.png`          |
| 6   | `slot_S_ENH_040` | Mana Conv.    | `dragon_mana_conversion.png` |
| 7   | `slot_S_ENH_050` | Purify        | `dragon_purify.png`          |
| 8   | `slot_S_DEB_020` | Fearful Cry   | `dragon_fearful_cry.png`     |
| 9   | `slot_S_GEO_010` | Magma Blast   | `dragon_magma_blast.png`     |
| 10  | `slot_S_GEO_020` | Wall Creation | `dragon_wall_creation.png`   |
| 11  | `slot_S_GEO_030` | Teleport      | — (이미지 없음)                   |
| —   | `slot_end_turn`  | End Turn      | `turn_end.png`               |

---

## Step 1: GamePlayUIManager.h — 신규 멤버 추가

```cpp
// 슬롯 바 아이콘 (인덱스 0~11)
std::vector<std::shared_ptr<CS230::Texture>> slot_icons_;
// 슬롯 X 좌표 캐시 (popup, confirm overlay에서 재사용)
std::array<double, 13> slot_bar_x_{};
double slot_bar_center_y_ = 0.0;

// 업캐스트 팝업 상태
bool        popup_open_       = false;
std::string popup_spell_id_;
int         popup_slot_index_ = -1;

// 호버 툴팁
Character* hovered_character_ = nullptr;

// InputHandler 포인터 (Draw에서 SelectSpell 호출용)
PlayerInputHandler* m_input_handler_ptr_ = nullptr;

// 신규 private 메서드
void DrawSlotBar();
void DrawUicastPopup();
void DrawTurnIndicator();
void DrawHoverTooltip();
```

---

## Step 2: InitButtons() 전면 재작성

1. `m_input_handler_ptr_ = inputHandler` 저장
2. `button_manager_.ClearAll()` 호출
3. 런타임 `win = Engine::GetWindow().GetSize()` 기준으로 offset 계산
4. `slot_bar_x_`, `slot_bar_center_y_` 멤버에 좌표 캐시
5. 아이콘 텍스처 로드 → `slot_icons_` (없는 슬롯은 nullptr)
6. 13개 Button 등록 (slot_move ~ slot_end_turn)
   - 슬롯: size={64,64}, label="" (아이콘이 대신), on_click = 람다
   - End Turn: size={128,64}, label="End Turn"
7. 배틀 로그 토글 버튼: size={64,64}, 우측 중앙 (기존 40×40 → 64×64)
8. **삭제**: 구 `lbl_S_ATK_010`, `S_ATK_010_lv1`~5 등 모든 스펠 서브버튼

**슬롯 비활성화 매핑** (Update에서 사용):

| 슬롯 ID            | 비활성 조건               |
| ---------------- | -------------------- |
| `slot_S_ATK_020` | `!slots->HasSlot(2)` |
| `slot_S_ENH_050` | `!slots->HasSlot(1)` |
| `slot_S_DEB_020` | `!slots->HasSlot(1)` |
| `slot_S_ATK_010` | `!slots->HasSlot(1)` |
| `slot_S_ATK_030` | `!slots->HasSlot(3)` |
| `slot_S_ATK_040` | `!slots->HasSlot(3)` |
| `slot_S_ENH_040` | 항상 활성 (슬롯 0도 허용)     |
| `slot_S_GEO_010` | `!slots->HasSlot(2)` |
| `slot_S_GEO_020` | `!slots->HasSlot(1)` |
| `slot_S_GEO_030` | 항상 활성 (슬롯 0도 허용)     |

---

## Step 3: PlayerInputHandler.cpp — ID rename & 구 서브메뉴 제거

- `"btn_move"` → `"slot_move"`, `"btn_attack"` → `"slot_attack"`, `"btn_end_turn"` → `"slot_end_turn"` 일괄 치환
- `"btn_action"`, `"btn_spell"`, `"btn_spell_cancel"` 참조 모두 제거
- `HideAllSpellButtons()` 함수 삭제
- `"btn_wall_confirm"` → `"slot_wall_confirm"` (InitButtons에서 숨겨진 버튼으로 등록, WallPlacementMulti 진입 시 slot_S_GEO_020 위치에 show, 나머지 때 hide)
- `SetVisible/SetDisabled` 호출에서 구 ID 참조 정리

---

## Step 4: Update() — 팝업 제어 + 호버 감지

```cpp
// 기존 _lv 접미사 비활성화 루프 → 위 슬롯별 조건으로 교체

// 업캐스트 팝업 트리거
static const std::map<std::string, std::pair<int,int>> UPCAST_RANGES = {
    {"slot_S_ATK_010",{1,5}},{"slot_S_ATK_030",{3,5}},{"slot_S_ATK_040",{3,5}},
    {"slot_S_ENH_040",{0,5}},{"slot_S_GEO_010",{2,5}},
    {"slot_S_GEO_020",{1,5}},{"slot_S_GEO_030",{0,5}},
};
for (auto& [id, range] : UPCAST_RANGES) {
    if (button_manager_.IsPressed(id)) {
        popup_open_ = !(popup_open_ && popup_spell_id_ == id);
        popup_spell_id_   = id;
        popup_slot_index_ = /* id → index 매핑 */;
    }
}

// 단일 레벨 스펠 즉시 발동 (팝업 없음)
// → slot_S_ATK_020, slot_S_ENH_050, slot_S_DEB_020의 on_click 람다에서 직접 SelectSpell 호출

// 팝업 외부 클릭 시 닫기 (DrawUicastPopup에서 팝업 버튼 히트 여부 플래그 설정 후 판단)
if (mouse_clicked && popup_open_ && !popup_hit_this_frame_)
    popup_open_ = false;

// 호버 툴팁 감지
hovered_character_ = nullptr;
auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
if (grid) {
    auto mouse = Engine::GetInput().GetMousePos();
    Math::ivec2 gp = { int(mouse.x / GridSystem::TILE_SIZE),
                       int(mouse.y / GridSystem::TILE_SIZE) };
    if (grid->IsValidTile(gp))
        hovered_character_ = grid->GetCharacterAt(gp);
}
```

---

## Step 5: DrawTurnIndicator() — 신규

```cpp
// TurnManager에서 현재 턴 캐릭터·번호·라운드 조회
// 상단 중앙 패널: W=320, H=48, Y-up top = win.y - 8
// 배경: 0x1a1a2ecc, 테두리: 0x5555aaff
// 텍스트: "Turn 3 | Dragon's Turn" (CS200::GOLD, 0.45x)
// 하단 작은 텍스트: "Round 1" (CS200::WHITE, 0.35x)
```

---

## Step 6: DrawHoverTooltip() — DrawCharacterStatsPanel 교체

`DrawCharacterStatsPanel()` 본문 전체 삭제 → `DrawHoverTooltip()` 신규 작성.

```cpp
// hovered_character_ == nullptr이면 return
// 툴팁 크기: W=256, H=200
// 앵커: 마우스 오른쪽 1 tile (72px), 화면 밖이면 왼쪽으로 flip
// 배경: 0x1a1a2edd, 테두리: 0x8888aaff
// 내용 (위→아래, LH=26):
//   Name  (0.5x, GOLD)
//   HP: x/y  (0.4x, RED)
//   AP: x    (0.4x, YELLOW)
//   Speed: x (0.4x, GREEN)
//   Slots: L1:x/y ...  (0.4x, ORANGE)
//   FX: Blessed(2) ...  (0.4x, PINK)
```

---

## Step 7: DrawUicastPopup() — 신규

```cpp
// popup_open_이 false면 return
// 팝업 버튼: W=48, H=36, GAP=4
// 앵커: slot_bar_x_[popup_slot_index_] + 32, 슬롯 바 상단 Y + 8px
// 배경 패널: 0x1a1a2ecc
// 레벨 버튼: UPCAST_RANGES[popup_spell_id_] 범위만큼 DrawRectangle + 클릭 검사
//   클릭 시 → m_input_handler_ptr_->SelectSpell(spell_id, char, level, btns)
//           → popup_open_ = false
// popup_hit_this_frame_ 플래그 설정 (Update에서 외부 클릭 닫기에 사용)
```

---

## Step 8: DrawSlotBar() — 신규 (아이콘 확정)

아이콘 로드 (InitButtons에서 한 번):

```cpp
// Engine의 TextureManager로 텍스처 로드 (GridSystem 패턴 참조)
// Assets/images/ 경로 기준
const std::vector<std::string> ICON_PATHS = {
    "",                                      // 0: Move (아이콘 없음)
    "Assets/images/dragon_attack.png",       // 1: Attack
    "Assets/images/dragon_firebolt.png",     // 2: Fire Bolt
    "Assets/images/dragon_tail_swipe.png",   // 3: Tail Swipe
    "Assets/images/dragon_fury.png",         // 4: Dragon's Fury
    "Assets/images/dragon_meteor.png",       // 5: Meteor
    "Assets/images/dragon_mana_conversion.png", // 6: Mana Conv.
    "Assets/images/dragon_purify.png",       // 7: Purify
    "Assets/images/dragon_fearful_cry.png",  // 8: Fearful Cry
    "Assets/images/dragon_magma_blast.png",  // 9: Magma Blast
    "Assets/images/dragon_wall_creation.png",// 10: Wall Creation
    "",                                      // 11: Teleport (없음)
};
slot_icons_.resize(12, nullptr);
for (int i = 0; i < 12; ++i) {
    if (!ICON_PATHS[i].empty())
        slot_icons_[i] = Engine::GetTextureManager().Load(ICON_PATHS[i]);
}
```

DrawSlotBar() 렌더링:

```cpp
void GamePlayUIManager::DrawSlotBar()
{
    auto* renderer = CS230::TextureManager::GetRenderer2D();
    // 1. 슬롯 바 배경 반투명 패널
    Math::TransformationMatrix bg = ...;
    renderer->DrawRectangle(bg, 0x1a1a2e99, 0x5555aaff, 1.5, DrawDepth::UI + 0.001f);

    // 2. 각 슬롯 위에 아이콘 오버레이
    for (int i = 0; i < 12; ++i) {
        if (!slot_icons_[i]) continue;
        double cx = slot_bar_x_[i] + 32.0;  // 슬롯 중심 X
        Math::TransformationMatrix icon_t =
            Math::TranslationMatrix({cx, slot_bar_center_y_}) *
            Math::ScaleMatrix({60.0, 60.0});           // 64px 슬롯 안에 약간 패딩
        slot_icons_[i]->Draw(icon_t, DrawDepth::UI - 0.002f);
    }
}
```

텍스처 없는 슬롯(Move, Teleport)은 ButtonManager가 그린 색상 사각형 + 텍스트 라벨로 표시됨. Move 슬롯과 Teleport 슬롯에 한해서만 `label = "Move"` / `label = "Teleport"` 설정. 아이콘 있는 슬롯은 `label = ""` (아이콘이 대신 표현).

---

## Step 9: DrawBattleLog() 크기 상수 변경

```cpp
// 기존:
constexpr double PANEL_W_RATIO = 0.30;
constexpr double PANEL_H_RATIO = 0.60;
constexpr double BTN_W = 40.0;

// 변경:
constexpr double PANEL_W = 5 * 64.0;  // 320px
constexpr double PANEL_H = 8 * 64.0;  // 512px
constexpr double BTN_W   = 64.0;
// PANEL_X, PANEL_Y 계산식 동일, 상수만 교체
```

---

## Step 10: Draw() 호출 순서 변경

```cpp
// 기존:
button_manager_.Draw(camera_matrix);
// ... damage texts ...
DrawCharacterStatsPanel(camera_matrix);  // 제거
DrawBattleLog();

// 변경:
button_manager_.Draw(camera_matrix);   // 슬롯 사각형 + End Turn + 로그 토글
DrawSlotBar();                          // 슬롯 아이콘 오버레이 + 슬롯 바 배경
DrawUicastPopup();                      // 업캐스트 레벨 선택 팝업
DrawTurnIndicator();                    // 상단 중앙 턴 표시
DrawHoverTooltip();                     // 마우스 오버 스탯 툴팁
DrawBattleLog();                        // 320×512 로그 패널
// damage texts, spell logs — 변경 없음
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

1. `cd DragonicTactics && cmake --build --preset windows-debug` 빌드 확인
2. 실행 후:
   - 하단 슬롯 바 12개 슬롯 + End Turn 표시 확인
   - Dragon 턴에서 슬롯 클릭 → Move/Attack/Spell 동작 확인
   - 업캐스트 스펠(Fire Bolt 등) 클릭 → 레벨 선택 팝업 확인
   - 캐릭터 위에 마우스 오버 → 스탯 툴팁 표시 확인
   - 우측 배틀 로그 토글 버튼(64px) 클릭 → 로그 패널 확인
   - 화면 상단 중앙 턴 인디케이터 확인
   - AI 턴(Fighter/Cleric) 중 슬롯 비활성화 확인
