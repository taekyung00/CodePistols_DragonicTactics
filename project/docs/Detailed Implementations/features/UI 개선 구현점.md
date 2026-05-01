# UI 개선 구현점

`bb7e32fa` 커밋 기준 변경점 전체 기록. 계획 문서(`UI 개선 구현 계획.md`)와 달리 **실제 구현된 코드** 기준으로 작성.

---

## 변경 파일 요약

| 파일                              | 변경 성격                                          |
| ------------------------------- | ---------------------------------------------- |
| `States/GamePlay.h`             | `TacticalCamera` 구조체 신규 정의                     |
| `States/GamePlay.cpp`           | 2-pass 렌더링 / 카메라 / 데미지 포지션 / 버추얼 NDC           |
| `States/GamePlayUIManager.h`    | 멤버 변수 + private 메서드 대폭 추가                      |
| `States/GamePlayUIManager.cpp`  | InitButtons/Update/Draw 전면 재작성, 신규 Draw 메서드 5개 |
| `States/PlayerInputHandler.h`   | 카메라 포인터 / 신규 메서드 추가                            |
| `States/PlayerInputHandler.cpp` | 구 버튼 기반 메뉴 제거, Dragon 클릭 이동, 카메라 좌표 변환         |
| `Engine/TextureManager.h`       | `SavedCameraMatrix` + `SaveCurrentScene()` 추가  |
| `Engine/TextureManager.cpp`     | `EndRenderTextureMode` 버그 수정                   |

---

## 1. TacticalCamera 및 2-pass 렌더링

### 1-A. GamePlay.h — TacticalCamera 신규

```cpp
struct TacticalCamera {
    Math::vec2 target = { 0.0, 0.0 };
    double zoom = 1.0;
    static constexpr double ZOOM_MIN = 0.25;
    static constexpr double ZOOM_MAX = 3.0;
    static constexpr int    VIRTUAL_W = 1600;
    static constexpr int    VIRTUAL_H = 900;

    Math::TransformationMatrix GetWorldMatrix(Math::ivec2 win) const;
    Math::vec2 ScreenToWorld(Math::vec2 screen, Math::ivec2 win) const;
    Math::vec2 WorldToScreen(Math::vec2 world, Math::ivec2 win) const;
    static Math::TransformationMatrix BuildVirtualNdc(Math::ivec2 win); // 레터박스 UI 행렬
};
```

`GamePlay`에 추가된 멤버:

```cpp
TacticalCamera m_camera;
Math::vec2     m_prev_mouse           = { 0.0, 0.0 };
bool           m_right_mouse_was_down = false;
```

### 1-B. GamePlay.cpp — Draw() 2-pass 구조

**변경 전**: 단일 `BeginScene(build_ndc_matrix(win))` → 월드+UI 혼합 렌더

**변경 후**:

```cpp
// Pass 1: 월드 공간 (카메라 변환 적용)
renderer_2d->BeginScene(m_camera.GetWorldMatrix(win));
grid_system->Draw();
goMgr->DrawAll(Math::TransformationMatrix{});
GetGSComponent<DebugManager>()->Draw(grid_system);
renderer_2d->EndScene();

// Pass 2: UI 공간 (가상 1600×900 + 레터박스)
Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
Engine::GetTextureManager().SaveCurrentScene(ui_ndc);  // 폰트 캐시 미스 복구용
renderer_2d->BeginScene(ui_ndc);
m_ui_manager->Draw(ui_ndc);
renderer_2d->EndScene();
```

> **BuildVirtualNdc**: 실제 윈도우 크기와 관계없이 가상 1600×900 해상도를 레터박스로 맞추는 행렬.
> `CS200::build_ndc_matrix(actual)` 와 다름 — 윈도우가 1600×900이 아닐 때 스케일/오프셋이 달라짐.

### 1-C. GamePlay.cpp — 카메라 조작 (Update)

```cpp
// 우클릭 드래그 = 패닝
if (inp.MouseDown(2) && !ImGui::GetIO().WantCaptureMouse) {
    if (m_right_mouse_was_down) {
        Math::vec2 world_prev = m_camera.ScreenToWorld(m_prev_mouse, win);
        Math::vec2 world_curr = m_camera.ScreenToWorld(mouse, win);
        m_camera.target.x -= world_curr.x - world_prev.x;
        m_camera.target.y -= world_curr.y - world_prev.y;
    }
    m_right_mouse_was_down = true;
}

// 스크롤 = 마우스 포인터 기준 줌
double scroll = inp.GetMouseScroll();
if (scroll != 0.0) {
    Math::vec2 wb = m_camera.ScreenToWorld(mouse, win);
    m_camera.zoom *= (1.0 + scroll * 0.125);
    m_camera.zoom = std::clamp(m_camera.zoom, ZOOM_MIN, ZOOM_MAX);
    Math::vec2 wa = m_camera.ScreenToWorld(mouse, win);
    m_camera.target -= (wa - wb);  // 줌 후 마우스 포인터 고정
}
```

카메라 초기값은 Load()에서 그리드 중심으로 설정:

```cpp
m_camera.target = { gs->GetWidth() * TILE_SIZE * 0.5, gs->GetHeight() * TILE_SIZE * 0.5 };
m_camera.zoom   = 1.0;
m_ui_manager->SetCamera(&m_camera);
```

### 1-D. 데미지 텍스트 위치 수정

**변경 전**: `text_position = grid_pos * TILE_SIZE` (ivec2를 vec2로 단순 변환)

**변경 후**: 월드 공간 좌표 (캐릭터 타일 상단 중앙):

```cpp
Math::vec2 text_position = {
    grid_pos.x * (double)TILE_SIZE + TILE_SIZE * 0.5,  // 타일 X 중앙
    grid_pos.y * (double)TILE_SIZE + TILE_SIZE          // 타일 상단
};
```

Draw에서는 카메라 변환으로 월드→스크린 변환 후 렌더:

```cpp
if (m_camera_)
    screen_pos = m_camera_->WorldToScreen(text.position, Engine::GetWindow().GetSize());
```

---

## 2. 슬롯 바 레이아웃 및 InitButtons 재작성

### 레이아웃 공식

```
가상 해상도: 1600×900, TILE = 64, N = 10 슬롯

bar_x   = TILE               = 64
bar_bot = TILE               = 64        (Y-up: 바 하단 Y)
bar_h   = TILE * 1.5         = 96
bar_w   = win.x - 2*TILE     = 1472      (at 1600)

remaining = bar_w - N*TILE - 2*TILE      = 704
offset    = remaining / (N + 2)          ≈ 58.67

slot_x[i]      = bar_x + offset + i*(TILE + offset)
slot_x[10]     = slot_x[9] + TILE + offset   (End Turn 버튼)
slot_center_y  = bar_bot + bar_h * 0.5  = 112
```

### 슬롯 매핑 (N=10 + End Turn)

| idx | 버튼 ID            | 스펠 ID       | 아이콘 파일                       | 업캐스트       |
| --- | ---------------- | ----------- | ---------------------------- | ---------- |
| 0   | `slot_attack`    | —           | `dragon_attack.png`          | 없음         |
| 1   | `slot_S_ATK_010` | `S_ATK_010` | `dragon_firebolt.png`        | Lv1~5      |
| 2   | `slot_S_ATK_020` | `S_ATK_020` | `dragon_tail_swipe.png`      | 없음(Lv2 고정) |
| 3   | `slot_S_ATK_030` | `S_ATK_030` | `dragon_fury.png`            | Lv3~5      |
| 4   | `slot_S_ATK_040` | `S_ATK_040` | `dragon_meteor.png`          | Lv3~5      |
| 5   | `slot_S_ENH_040` | `S_ENH_040` | `dragon_mana_conversion.png` | Lv0~5      |
| 6   | `slot_S_ENH_050` | `S_ENH_050` | `dragon_purify.png`          | 없음(Lv1 고정) |
| 7   | `slot_S_DEB_020` | `S_DEB_020` | `dragon_fearful_cry.png`     | 없음(Lv1 고정) |
| 8   | `slot_S_GEO_010` | `S_GEO_010` | `dragon_magma_blast.png`     | Lv2~5      |
| 9   | `slot_S_GEO_020` | `S_GEO_020` | `dragon_wall_creation.png`   | Lv1~5      |
| —   | `slot_end_turn`  | —           | `turn_end.png`               | —          |

추가 버튼: `btn_battle_log` (우측 `win.x - 64`, `win.y*0.5 + 32`, 64×64, 토글)

### on_click 람다 패턴

```cpp
// 기본 공격
"slot_attack" → [inputHandler]() { inputHandler->OnAttackPressed(); }

// 단일 레벨 스펠 (S_ATK_020, S_ENH_050, S_DEB_020)
→ [=]() { inputHandler->SelectSpell(spell_id, turnMgr->GetCurrentCharacter(), level, btns); }

// 업캐스트 스펠 → 팝업 열기
→ [this, i, id]() {
    popup_open_        = !popup_open_;
    popup_spell_id_    = id;       // "S_ATK_010" 등 (슬롯 접두사 제거)
    popup_slot_index_  = i;
  }

// End Turn
→ [inputHandler]() { inputHandler->OnEndTurnPressed(); }

// 배틀 로그 토글
→ [this]() {
    show_battle_log_ = !show_battle_log_;
    button_manager_.SetLabel("btn_battle_log", show_battle_log_ ? ">" : "<");
  }
```

### 슬롯 비활성화 조건 (Update에서 매 프레임 갱신)

```cpp
bool no_ap = (current->GetActionPoints() == 0);
bool is_ai = current->IsAIControlled();

set_disabled("slot_attack",    no_ap || is_ai);
set_disabled("slot_end_turn",  is_ai);

// 스펠 슬롯 공통 패턴: no_ap || !HasSlot(min_lv) || is_ai
spell_disabled("slot_S_ATK_010", 1);
spell_disabled("slot_S_ATK_020", 2);
spell_disabled("slot_S_ATK_030", 3);
spell_disabled("slot_S_ATK_040", 3);
spell_disabled("slot_S_ENH_040", 1);
spell_disabled("slot_S_ENH_050", 1);
spell_disabled("slot_S_DEB_020", 1);
spell_disabled("slot_S_GEO_010", 2);
spell_disabled("slot_S_GEO_020", 1);
```

---

## 3. 신규 Draw 메서드

### DrawSlotBar()

슬롯 바 반투명 배경 패널 + 슬롯 아이콘 텍스처 오버레이.

```cpp
// 배경 패널 (중앙 기준, bar_w × 96)
renderer->DrawRectangle(bg, 0x1a1a2e99, 0x5555aaff, 1.5, DrawDepth::UI + 0.001f);

// 아이콘: slot_bar_x_[i] 기준, DrawDepth::UI - 0.005f (버튼 사각형보다 앞)
slot_icons_[i]->Draw(
    Math::TranslationMatrix({ slot_bar_x_[i], slot_bar_center_y_ - 32.0 }),
    0xFFFFFFFF, DrawDepth::UI - 0.005f);
```

아이콘은 64×64 텍스처를 슬롯 좌상단 기준으로 배치 (TranslationMatrix만, Scale 없음).

### DrawUicastPopup()

업캐스트 레벨 선택 팝업. `ButtonManager`에 등록하지 않고 `DrawRectangle` + 인라인 클릭 판별로 구현.

```
팝업 위치:
  bar_top      = slot_bar_center_y_ + 32.0   (슬롯 바 상단 Y)
  popup_bottom = bar_top + BTN_H + 8.0       (버튼 하단 = 슬롯 바 바로 위)
  start_x      = slot_bar_x_[popup_slot_index_] + 32.0 - (전체너비 / 2)  (중앙 정렬)

버튼 크기: W=48, H=36, GAP=4
색상: hover → 0x6666aaff, normal → 0x3333aacc, disabled → 0x444444cc
```

클릭 판별은 Update()에서 처리 (Draw()에서는 시각적 렌더만):

```cpp
if (popup_open_ && mouse_click) {
    // 팝업 geometry 계산 후 hit test
    // hit → SelectSpell() 호출 + popup_open_ = false
    // miss → popup_open_ = false (외부 클릭 닫기)
}
```

### DrawTurnIndicator()

화면 상단 중앙 턴 표시 패널.

```
위치: panel_cx = win.x * 0.5, panel_top = win.y - 8
크기: W=320 (5 tiles), H=48
배경: 0x1a1a2ecc / 테두리: 0x5555aaff
텍스트: "{TypeName}'s Turn" (GOLD, scale 0.45)
```

### DrawHoverTooltip()

`hovered_character_ != nullptr`일 때 마우스 우측에 스탯 툴팁 표시.

```
크기: W=256, H=200
앵커: mouse.x + 72 (화면 밖 flip: mouse.x - W - 8)
LH (줄 간격): 26px
내용 순서: TypeName(GOLD,0.5) / HP(RED,0.4) / AP(YELLOW,0.4) / Speed(GREEN,0.4)
           / Slots(ORANGE,0.4) / FX(YELLOW,0.4)
```

### DrawActionLabel()

현재 입력 상태 또는 열린 팝업에 대응하는 액션 이름 표시. 슬롯 바 아래 중앙에 표시.

```
위치: y = slot_bar_center_y_ - 48 - 8  (슬롯 바 아래 8px 여백)
크기: W = label.size() * 10 + 40 (동적), H = 30
표시 조건:
  - popup_open_ && !popup_spell_id_.empty() → 스펠 이름
  - TargetingForAttack → "Attack"
  - TargetingForSpell / WallPlacementMulti / LavaPlacementMulti → 스펠 이름
```

---

## 4. DrawBattleLog() 크기 상수 변경

```cpp
// 변경 전 (비율 기반)
constexpr double PANEL_W_RATIO = 0.30;  // win.x의 30%
constexpr double PANEL_H_RATIO = 0.60;  // win.y의 60%
constexpr double BTN_W = 40.0;

// 변경 후 (타일 기반)
constexpr double PANEL_W = 5 * 64.0;   // 320px (5 tiles)
constexpr double PANEL_H = 8 * 64.0;   // 512px (8 tiles)
constexpr double BTN_W   = 64.0;        // 1 tile
const double PANEL_X = win.x - BTN_W - MARGIN - PANEL_W - 2.0;
```

---

## 5. Update() 변경

### 처리 순서 (변경 후)

```
1. 슬롯 비활성화 갱신 (TurnManager에서 현재 캐릭터 조회)
2. 업캐스트 팝업 클릭 처리 (button_manager_.Update 전에)
3. button_manager_.Update(mouse_pos, mouse_click)
4. 호버 캐릭터 감지 (카메라로 screen→world 변환 후 GetCharacterAt)
5. Dragon 호버 → 이동 범위 미리 표시 (None 상태 + 플레이어 턴일 때만)
6. 데미지 텍스트·스펠 로그 수명 갱신·제거
```

### 카메라 보정 마우스 좌표

```cpp
Math::vec2 world_pos = mouse_pos;
if (m_camera_)
    world_pos = m_camera_->ScreenToWorld(mouse_pos, Engine::GetWindow().GetSize());
int gx = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
int gy = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
hovered_character_ = grid->GetCharacterAt({ gx, gy });
```

### Dragon 호버 → 이동 범위 미리보기

```cpp
bool in_none_state = m_input_handler_ptr_ &&
    (m_input_handler_ptr_->GetCurrentState() == PlayerInputHandler::ActionState::None);
if (grid && in_none_state && is_player_turn) {
    if (hovered_character_ && !hovered_character_->IsAIControlled())
        grid->EnableMovementMode(pos, range);
    else
        grid->DisableMovementMode();
}
```

---

## 6. PlayerInputHandler 변경

### 제거된 것

- `HideAllSpellButtons()` 정적 헬퍼 함수 전체 삭제
- `btn_move`, `btn_action`, `btn_spell`, `btn_spell_cancel` 버튼 처리 로직
- `lbl_S_ATK_010`, `S_ATK_010_lv1~5` 등 구 서브버튼 visibility 토글 코드
- 버튼 레이블 "Cancel Move" / "Cancel Action" 갱신 코드

### 추가된 것

```cpp
// PlayerInputHandler.h
void OnAttackPressed();   // slot_attack on_click에서 호출
void OnEndTurnPressed();  // slot_end_turn on_click에서 호출
std::string GetSelectedSpellId() const;  // DrawActionLabel에서 현재 스펠 이름 조회
const TacticalCamera* m_camera = nullptr;  // Update() 인자로 수신

// Update() 시그니처 변경
void Update(double dt, Character*, GridSystem*, CombatSystem*, ButtonManager&,
            const TacticalCamera* camera = nullptr);
```

### ConvertScreenToGrid() — 카메라 변환 추가

```cpp
Math::ivec2 PlayerInputHandler::ConvertScreenToGrid(Math::vec2 screen_pos)
{
    Math::vec2 world_pos = screen_pos;
    if (m_camera)
        world_pos = m_camera->ScreenToWorld(screen_pos, Engine::GetWindow().GetSize());
    return { int(world_pos.x / TILE_SIZE), int(world_pos.y / TILE_SIZE) };
}
```

### ActionState::None — Dragon 타일 클릭 → 이동 모드

```cpp
case ActionState::None:
    if (grid_pos == dragon->GetGridPosition()->Get() && dragon->GetMovementRange() > 0) {
        SetState(ActionState::SelectingMove);
        grid->EnableMovementMode(dragon->GetGridPosition()->Get(), dragon->GetMovementRange());
    }
    break;
```

### 버튼 ID rename

```
"btn_wall_confirm" → "slot_wall_confirm"
```

(Move/Action/Spell/Attack/EndTurn 버튼은 on_click 람다로 대체되어 IsPressed 체크 자체가 제거됨)

### CancelCurrentAction() — 정리 로직 강화

```cpp
if (m_state == ActionState::SelectingMove)
    grid->DisableMovementMode();
else if (m_state == TargetingForSpell || WallPlacementMulti || LavaPlacementMulti) {
    grid->DisableSpellTargetingMode();
    if (WallPlacementMulti || LavaPlacementMulti) {
        m_wall_placement_tiles.clear();
        grid->ClearWallPreviewTiles();
    }
}
m_state = ActionState::None;
```

### SelectSpell() — 이전 타겟팅 상태 정리 추가

스펠 전환 시 이전 타겟팅 모드를 먼저 정리:

```cpp
if (m_state == TargetingForSpell || WallPlacementMulti || LavaPlacementMulti) {
    g->DisableSpellTargetingMode();
    // ...
}
```

---

## 7. 배틀 로그 깜빡임 버그 수정 (Engine)

### 원인

`Font::PrintToTexture()` — 새 문자열 렌더 시 캐시 미스 → `TextureManager::EndRenderTextureMode()` 호출.  
이 함수 내부에서 씬을 복구할 때:

```cpp
// 버그: 실제 윈도우 크기 기준 NDC로 복구 (가상 NDC가 아님)
renderer_2d->BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
```

실제 윈도우 ≠ 1600×900이면 복구된 행렬이 잘못되어, 이후 DrawRectangle/DrawSDF 결과가 다른 스케일로 렌더됨.  
`ImmediateRenderer2D::EndScene()`은 no-op이므로 오염된 행렬이 즉시 모든 후속 draw call에 영향.

### 수정 (CS200 파일 무수정)

**`Engine/TextureManager.h`**:

```cpp
struct RenderInfo {
    // ...기존 필드...
    Math::TransformationMatrix SavedCameraMatrix{};  // 추가
};
static void SaveCurrentScene(const Math::TransformationMatrix& m);  // 추가
```

**`Engine/TextureManager.cpp`**:

```cpp
void TextureManager::SaveCurrentScene(const Math::TransformationMatrix& m) {
    get_render_info().SavedCameraMatrix = m;
}

// EndRenderTextureMode 내부:
// 변경 전: renderer_2d->BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
// 변경 후:
renderer_2d->BeginScene(render_info.SavedCameraMatrix);
```

**`States/GamePlay.cpp`** — UI 패스 시작 직전에 저장:

```cpp
Math::TransformationMatrix ui_ndc = TacticalCamera::BuildVirtualNdc(win);
Engine::GetTextureManager().SaveCurrentScene(ui_ndc);  // ← 추가
renderer_2d->BeginScene(ui_ndc);
```

이로써 DrawBattleLog / DrawUicastPopup 등에서 동적 텍스트 렌더 시 캐시 미스가 발생해도
`EndRenderTextureMode`가 올바른 `ui_ndc`로 씬을 복구한다.

---

## 8. Draw() 호출 순서 (최종)

```cpp
void GamePlayUIManager::Draw(Math::TransformationMatrix camera_matrix)
{
    button_manager_.Draw(camera_matrix);  // 슬롯 사각형 + End Turn + 로그 토글
    DrawSlotBar();                         // 배경 패널 + 아이콘 오버레이
    DrawUicastPopup();                     // 업캐스트 레벨 팝업 (시각화만, 클릭은 Update)
    DrawActionLabel();                     // 현재 액션 이름 레이블
    DrawTurnIndicator();                   // 상단 중앙 턴 표시
    DrawHoverTooltip();                    // 마우스 오버 스탯 툴팁

    DrawBattleLog();                       // 320×512 우측 로그 패널

    // 기존 유지
    // 데미지 플로팅 텍스트 (월드→스크린 변환 후 렌더)
    // 데미지 로그 텍스트 (화면 고정 위치)
    // game_end_text
    // m_spell_logs (X=800, Y=50 기준)
    // DrawCharacterStatsPanel (레거시, 유지)
}
```

---

## 9. 유지된 것 (변경 없음)

- `ShowDamageText`, `ShowDamageLog`, `ShowGameEnd` 인터페이스
- `m_spell_logs` 골드 텍스트 렌더 (X=800, Y=50)
- `AddSpellLog`, `OnTurnStarted`, `AddBattleLogEntry`
- `DrawBattleLog()` 내부 텍스트 루프 (패널 크기 상수만 변경)
- `DrawCharacterStatsPanel()` — 레거시 패널로 잔존, 호버 툴팁과 공존
- `GamePlay::DrawImGui()` — Map Selection + Combat Status 패널
- `TurnManager`, `GridSystem`, `SpellSystem`, `CombatSystem`, `StatusEffectHandler` — 수정 없음
- `ButtonManager.h/.cpp` — 수정 없음
