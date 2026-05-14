# Map Loading 단일화 + GridSystem 가변 크기 지원

## 배경

현재 `GamePlay`는 두 가지 경로로 맵을 로드한다:

1. **하드코딩 경로** — [`GamePlay::LoadFirstMap()`](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp)
   문자열 배열(`"wwwwwwww"`, `"xeefeeew"` …)이 함수 안에 박혀 있고, 타일 파싱·좌표 반전·캐릭터 스폰을 전부 `GamePlay`가 직접 수행.
2. **JSON 경로** — [`GamePlay::LoadJSONMap()`](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp)
   `MapDataRegistry`가 `maps.json`을 파싱해 `MapData`를 반환 → `GridSystem::LoadMap(map_data)`로 위임 → `spawn_points` 딕셔너리로 캐릭터 스폰.

분기는 `GamePlay::s_next_map_source` (`MapSource::First` / `MapSource::Json`) 로 결정된다.

### 문제점

1. **책임 분리 불균형**: JSON 경로는 깔끔히 `GridSystem`에 위임되는 반면, 하드코딩 경로는 `GamePlay`에 타일 루프/스폰 로직이 섞여 있음.
2. **타일 지원 불일치**: 하드코딩은 `w/e/x/d/f`만, JSON은 `wall/floor/lava/water`만 지원. Exit 타일은 JSON 경로에 없음.
3. **GridSystem 이 8x8 로 컴파일 타임 고정**:
   - [GridSystem.h:91-94](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.h#L91-L94):
     
     ```cpp
     static const int MAP_WIDTH  = 8;
     static const int MAP_HEIGHT = 8;
     TileType     tile_grid[MAP_HEIGHT][MAP_WIDTH];
     Character*   character_grid[MAP_HEIGHT][MAP_WIDTH];
     ```
   - 10x10/12x12 맵을 로드하면 `SetTileType` 이 고정 배열 범위를 벗어나 쓰기 → undefined behavior.
   - `GetWidth()`/`GetHeight()` 는 이 상수들을 반환 — 주석에는 "가변 크기 전환 대비"라고 돼있지만 아직 미구현.
4. **`TILE_SIZE` 이름이 실제 의미와 다름**: [GridSystem.h:142](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.h#L142) 의 `TILE_SIZE = MAP_WIDTH * MAP_HEIGHT` 는 이름상 "타일 개수"처럼 보이지만 실제 코드는 이걸 **픽셀 크기(=64)** 로 사용. 8x8에서 우연히 동작.

---

## 목표

1. **하드코딩 맵 로딩 경로를 완전히 제거**하고 모든 맵 로딩을 JSON 경로로 통일.
2. **`GridSystem` 을 런타임 가변 크기**로 리팩토링 — 10x10, 12x12 맵이 실제로 동작하도록.
3. **`TILE_SIZE` 를 독립 상수(=64)로 재정의** — 수식 `MAP_WIDTH * MAP_HEIGHT` 의존성 제거.
4. `maps.json` 에서 기존 샘플(`arena_01`, `lava_chamber`)을 제거하고 **세 가지 크기의 기본 맵** 추가 (`first_map` 8x8 / `medium_map` 10x10 / `large_map` 12x12).
5. **Exit 타일** 을 JSON 스키마에 별도 필드로 추가.
6. `GamePlay::DrawImGui` 의 "First" 버튼 및 `MapSource` enum 제거 → JSON 맵 리스트만 표시.

---

## 작업 순서 (2단계)

### Phase A: JSON 경로 통일 (first_map 하나만)

- 하드코딩 맵을 `first_map` 으로 이식
- `LoadFirstMap()` / `MapSource` enum 제거
- `maps.json` 에 `first_map` 1개만 두고 나머지(`arena_01`, `lava_chamber`) 삭제
- Exit 필드 지원 추가 (`MapData` / `MapDataRegistry` / `GridSystem::LoadMap`)
- 빌드 & 실행 확인 → 기존 게임플레이가 `first_map` 으로 정상 작동해야 함

**Phase A 완료 기준**: 기존 8x8 맵 동작이 JSON 경로로 완전히 대체되어도 전혀 차이가 없어야 함 (HP, AI, spell, exit 전부).

### Phase B: GridSystem 가변 크기 + 큰 맵 추가

- `tile_grid` / `character_grid` 를 `std::vector<std::vector<...>>` 로 교체
- `MAP_WIDTH` / `MAP_HEIGHT` 를 런타임 멤버 (`map_width_` / `map_height_`) 로 변경
- `TILE_SIZE` 를 독립 상수 `= 64` 로 재정의 (이름 유지, 값 그대로)
- 모든 `GridSystem` 내부 루프/bounds check 를 런타임 값으로 교체
- `LoadMap()` 에서 `map_data.width`/`height` 에 맞춰 스토리지 resize
- `maps.json` 에 `medium_map`(10x10), `large_map`(12x12) 추가
- `DrawImGui` 맵 선택 패널에 3개 버튼 표시
- 빌드 & 실행 → 세 맵 모두 정상 작동 확인

---

## 스키마 변경 사항 (Phase A)

### `maps.json` 엔트리 포맷 (신규 `exit` 필드)

```json
{
    "id": "first_map",
    "name": "First Map (8x8)",
    "width": 8,
    "height": 8,
    "tiles": [
        "########",
        "#......#",
        "#......#",
        "#......#",
        "#......#",
        "#......#",
        "#......#",
        "########"
    ],
    "legend": {
        "#": "wall",
        ".": "floor"
    },
    "spawn_points": {
        "dragon":  {"x": 3, "y": 1},
        "fighter": {"x": 3, "y": 6}
    },
    "exit": {"x": 0, "y": 6}
}
```

- `exit` 는 **선택 필드**. 없으면 그 맵에는 Exit 타일이 없음.
- `spawn_points` / `exit` 모두 **world 좌표** (y=0이 맨 아래).
- `tiles` 배열은 문자열 첫 줄이 맵 위쪽(높은 y).
- `tiles` 에는 캐릭터/Exit 심볼을 넣지 않음 — 바닥/벽/위험 지형만.

### `MapData` 구조체 변경

[`MapDataRegistry.h`](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/MapDataRegistry.h):

```cpp
struct MapData {
    std::string id;
    std::string name;
    int width;
    int height;
    std::vector<std::string> tiles;
    std::map<char, std::string> legend;
    std::map<std::string, Math::ivec2> spawn_points;
    bool has_exit = false;          // 신규
    Math::ivec2 exit_position{0,0}; // 신규
};
```

---

## Phase A 코드 변경 사항

### A-1. `MapDataRegistry.cpp` — `exit` 필드 파싱

`LoadMaps()` 루프에서 `spawn_points` 파싱 직후:

```cpp
if (map_json.contains("exit")) {
    const auto& exit_json = map_json["exit"];
    map_data.exit_position = Math::ivec2{exit_json["x"], exit_json["y"]};
    map_data.has_exit = true;
}
```

### A-2. `GridSystem::LoadMap()` — Exit 자동 설정

[GridSystem.cpp:508](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/GridSystem.cpp#L508) 의 타일 루프 종료 직후:

```cpp
if (map_data.has_exit) {
    SetTileType(map_data.exit_position, TileType::Exit);
    SetExitPosition(map_data.exit_position);
    Engine::GetLogger().LogEvent("GridSystem::LoadMap - Exit set at (" +
        std::to_string(map_data.exit_position.x) + "," +
        std::to_string(map_data.exit_position.y) + ")");
}
```

### A-3. `GamePlay.h` — `MapSource` 제거

제거:

```cpp
enum class MapSource { First, Json };
static MapSource s_next_map_source;
MapSource current_map_source_;
void LoadFirstMap();
```

유지:

```cpp
static int s_next_map_index;
int selected_json_map_index_;
std::vector<std::string> available_json_maps_;
void LoadJSONMap(const std::string& map_id);
```

### A-4. `GamePlay.cpp::Load()` — 분기 제거 + 안전망 강화

[GamePlay.cpp:98-124](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp#L98-L124) 교체:

```cpp
selected_json_map_index_ = s_next_map_index;

auto* map_registry = GetGSComponent<MapDataRegistry>();
map_registry->LoadMaps("Assets/Data/maps.json");
available_json_maps_ = map_registry->GetAllMapIds();

if (available_json_maps_.empty())
{
    Engine::GetLogger().LogError("No maps loaded from maps.json — returning to MainMenu");
    Engine::GetGameStateManager().PopState();
    Engine::GetGameStateManager().PushState<MainMenu>();
    return;
}

int index = selected_json_map_index_;
if (index < 0 || index >= static_cast<int>(available_json_maps_.size()))
{
    Engine::GetLogger().LogError("Invalid map index " + std::to_string(index) + ", defaulting to 0");
    index = 0;
    s_next_map_index = 0;
    selected_json_map_index_ = 0;
}

const std::string& selected_map_id = available_json_maps_[static_cast<std::size_t>(index)];
Engine::GetLogger().LogEvent("Loading map: " + selected_map_id);
LoadJSONMap(selected_map_id);

if (player == nullptr || enemy == nullptr)
{
    Engine::GetLogger().LogError("LoadJSONMap failed to spawn characters — returning to MainMenu");
    Engine::GetGameStateManager().PopState();
    Engine::GetGameStateManager().PushState<MainMenu>();
    return;
}
```

### A-5. `GamePlay.cpp::LoadFirstMap()` 전체 삭제

[GamePlay.cpp:575-627](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp#L575-L627) 제거.

### A-6. `GamePlay.cpp::LoadJSONMap()` — spawn/exit 중복 경고

caster spawn 좌표가 exit 좌표와 같은 경우 경고 로그 (실수 방지용). 실제로 스폰 직후:

```cpp
if (map_data.has_exit &&
    (dragon_spawn == map_data.exit_position || fighter_spawn == map_data.exit_position))
{
    Engine::GetLogger().LogError("WARNING: spawn point overlaps exit at (" +
        std::to_string(map_data.exit_position.x) + "," +
        std::to_string(map_data.exit_position.y) + ")");
}
```

### A-7. `GamePlay.cpp::DrawImGui()` — 맵 선택 패널 정리

[GamePlay.cpp:324-371](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp#L324-L371):

- `MapSource::First` 분기·"First Map" 버튼 전부 제거
- `current_map_source_` 텍스트 제거
- 루프는 `available_json_maps_` 만 순회
- 선택된 버튼은 `s_next_map_index == i` 로 하이라이트
- 버튼 클릭: `s_next_map_index = i;` + `s_should_restart = true;`

(Phase A 종료 시점 `available_json_maps_` 는 `first_map` 1개만 포함)

### A-8. `GamePlay.cpp` 상단 정적 필드

```cpp
// 제거
GamePlay::MapSource GamePlay::s_next_map_source = GamePlay::MapSource::First;
// 유지
int GamePlay::s_next_map_index = 0;
```

### A-9. `MainMenu` 등에서 `MapSource` 참조 제거

grep 후 발견 시 삭제 (현 시점에서는 `GamePlay.h/cpp` 외 참조 없을 것으로 예상).

### A-10. `maps.json` 전체 교체 (Phase A 버전)

```json
{
    "maps": [
        {
            "id": "first_map",
            "name": "First Map (8x8)",
            "width": 8,
            "height": 8,
            "tiles": [
                "########",
                "#......#",
                "#......#",
                "#......#",
                "#......#",
                "#......#",
                "#......#",
                "########"
            ],
            "legend": {
                "#": "wall",
                ".": "floor"
            },
            "spawn_points": {
                "dragon":  {"x": 3, "y": 1},
                "fighter": {"x": 3, "y": 6}
            },
            "exit": {"x": 0, "y": 6}
        }
    ]
}
```

### Phase A 검증

```bash
cd DragonicTactics
cmake --build --preset windows-debug
build/windows-debug/dragonic_tactics.exe
```

체크리스트:

- [ ] 시작 시 `first_map` 자동 로드, Dragon/Fighter/Exit 위치가 이전과 동일
- [ ] Map Selection 패널에 **first_map 버튼 1개만** 표시
- [ ] Fighter AI 턴 정상 동작
- [ ] Dragon 의 Wall/Lava Creation spell 정상 작동 (타일 배치 + 만료 후 복구)
- [ ] Dragon 이 Exit 로 이동하면 `CharacterEscapedEvent` 발생, 기존 탈출 로직 작동
- [ ] 로그에 `GridSystem::LoadMap - Exit set at ...` 출력 확인

---

## Phase B 코드 변경 사항

### B-1. `GridSystem.h` — 저장소/상수 개편

**제거**:

```cpp
static const int MAP_WIDTH  = 8;
static const int MAP_HEIGHT = 8;
TileType     tile_grid[MAP_HEIGHT][MAP_WIDTH];
Character*   character_grid[MAP_HEIGHT][MAP_WIDTH];
static const int TILE_SIZE = MAP_WIDTH * MAP_HEIGHT;
```

**추가**:

```cpp
// 타일 당 픽셀 크기 (월드 좌표 변환 상수). 맵 크기와 무관.
static const int TILE_SIZE = 64;

// 최대 맵 크기 상한 (안전용. 필요시 조정)
static const int MAX_MAP_DIM = 32;

// 런타임 맵 크기
int map_width_  = 8;
int map_height_ = 8;

// 동적 저장소
std::vector<std::vector<TileType>>   tile_grid_;
std::vector<std::vector<Character*>> character_grid_;
```

**`GetWidth()`/`GetHeight()` 시그니처 유지, 반환값만 런타임으로**:

```cpp
int GetWidth()  const { return map_width_; }
int GetHeight() const { return map_height_; }
```

> **포인트**: `TILE_SIZE` 라는 이름은 그대로 유지. 값도 64 그대로. 외부 19개 call site 전부 수정 불필요. 이 상수의 **정의 수식**만 `MAP_WIDTH * MAP_HEIGHT` 에서 독립 `64` 로 바뀜.

### B-2. `GridSystem.cpp` — 내부 모든 사용처 런타임화

**생성자**: 기본 크기 8x8 로 `tile_grid_` / `character_grid_` 초기화

```cpp
GridSystem::GridSystem()
{
    ResizeGrid(8, 8);
    Reset();
}

void GridSystem::ResizeGrid(int w, int h)
{
    map_width_ = w;
    map_height_ = h;
    tile_grid_.assign(h, std::vector<TileType>(w, TileType::Empty));
    character_grid_.assign(h, std::vector<Character*>(w, nullptr));
}
```

**업데이트 대상 함수** (GridSystem.cpp 내부, 41개 사용처):

| 함수                                                                          | 변경                                                                          |
| --------------------------------------------------------------------------- | --------------------------------------------------------------------------- |
| `Reset()` [lines 44-46]                                                     | `MAP_WIDTH`/`HEIGHT` → `map_width_`/`map_height_`                           |
| `IsValidTile()` [line 57]                                                   | 동일                                                                          |
| `SetTileType/GetTileType` [lines 67, 76]                                    | `tile_grid[y][x]` → `tile_grid_[y][x]`                                      |
| `IsOccupied()` [line 85]                                                    | `character_grid_[y][x]`                                                     |
| `Draw()` [lines 93-206 부근, 27 usages]                                       | 루프 bounds 전부 `map_width_/height_`. `TILE_SIZE` 는 그대로 (이미 64)                |
| `EnableSpellTargetingMode()` [lines 229-247]                                | 루프 bounds 런타임화                                                              |
| `AddCharacter/RemoveCharacter/MoveCharacter/GetCharacterAt` [lines 315-342] | 저장소 이름 변경                                                                   |
| `GetAllCharacters()` [lines 361-365]                                        | 루프 + 저장소 이름 변경                                                              |
| `LoadMap()` [lines 508-567]                                                 | 맨 앞에 `ResizeGrid(map_data.width, map_data.height)` 호출. 루프 내부도 런타임 bounds 사용 |

### B-3. `DebugVisualizer.cpp` — 이미 runtime-safe

[DebugVisualizer.cpp:111-113](../../../DragonicTactics/source/Game/DragonicTactics/Debugger/DebugVisualizer.cpp#L111-L113) 가 이미 `grid->GetWidth()/GetHeight()` 을 지역 변수로 캐시 사용 → **accessor 반환값이 런타임화되면 자동 동작**.

단, [lines 291-298](../../../DragonicTactics/source/Game/DragonicTactics/Debugger/DebugVisualizer.cpp#L291-L298) 의 `DrawTileInfo()` 가 여전히 `MAP_WIDTH/MAP_HEIGHT` 상수를 참조하면 컴파일 에러 → 지역 변수(`grid->GetWidth()` / `GetHeight()`)로 교체.

### B-4. 외부 파일 (Character / MovementComponent / PlayerInputHandler / GamePlay / SpellSystem / TestAbility)

- 전부 `GridSystem::TILE_SIZE` 를 픽셀 변환에 사용 → **값이 64 그대로 유지**되므로 **수정 불필요** ✓
- `MAP_WIDTH`/`MAP_HEIGHT` 를 직접 참조하는 곳이 있으면 `grid->GetWidth()/GetHeight()` 로 교체 (조사 결과 `DebugVisualizer.cpp` 외엔 없음)

### B-5. `AStar.cpp` — 변경 불필요 ✓

이미 `IsValidTile()` / `GetNeighbors()` 만 사용. 런타임 safe.

### B-6. Test 파일

`TestAbility.cpp` 는 `TILE_SIZE` 만 사용 → 수정 불필요.
`Week1/Week3TestMocks` 는 별도 mock grid 를 가질 수 있음 — 컴파일 에러 발생 시에만 대응.

### B-7. `maps.json` 확장 (Phase B 최종)

```json
{
    "maps": [
        {
            "id": "first_map",
            "name": "First Map (8x8)",
            "width": 8,
            "height": 8,
            "tiles": [
                "########",
                "#......#",
                "#......#",
                "#......#",
                "#......#",
                "#......#",
                "#......#",
                "########"
            ],
            "legend": { "#": "wall", ".": "floor" },
            "spawn_points": {
                "dragon":  {"x": 3, "y": 1},
                "fighter": {"x": 3, "y": 6}
            },
            "exit": {"x": 0, "y": 6}
        },
        {
            "id": "medium_map",
            "name": "Medium Map (10x10)",
            "width": 10,
            "height": 10,
            "tiles": [
                "##########",
                "#........#",
                "#........#",
                "#........#",
                "#........#",
                "#........#",
                "#........#",
                "#........#",
                "#........#",
                "##########"
            ],
            "legend": { "#": "wall", ".": "floor" },
            "spawn_points": {
                "dragon":  {"x": 4, "y": 1},
                "fighter": {"x": 4, "y": 8}
            },
            "exit": {"x": 0, "y": 8}
        },
        {
            "id": "large_map",
            "name": "Large Map (12x12)",
            "width": 12,
            "height": 12,
            "tiles": [
                "############",
                "#..........#",
                "#..........#",
                "#..........#",
                "#..........#",
                "#..........#",
                "#..........#",
                "#..........#",
                "#..........#",
                "#..........#",
                "#..........#",
                "############"
            ],
            "legend": { "#": "wall", ".": "floor" },
            "spawn_points": {
                "dragon":  {"x": 5, "y": 1},
                "fighter": {"x": 5, "y": 10}
            },
            "exit": {"x": 0, "y": 10}
        }
    ]
}
```

세 맵 모두 바깥 한 줄 벽 + 내부 전부 floor — 크기 스케일링 검증이 목적이므로 Lava/Difficult 없음.

### Phase B 검증

체크리스트:

- [ ] 빌드 성공 (컴파일 에러 없음)
- [ ] `first_map` 기존 동작 유지 (회귀 없음)
- [ ] `medium_map` (10x10) 클릭 → 재시작 후 그리드 10x10 정상 렌더링, Dragon/Fighter 배치 확인
- [ ] `large_map` (12x12) 동일 검증 — 12*64=768px 이므로 1200x800 창 안에 들어와야 함
- [ ] 큰 맵에서 Fighter AI 이동/A\* 경로 탐색 정상
- [ ] 큰 맵에서 Dragon 의 Wall/Lava Creation spell 정상 (새 좌표 범위 내 배치 가능)
- [ ] 큰 맵에서 Exit 이동 정상
- [ ] ImGui 패널(`Map Selection`, `Player Actions`, `Action List`, `Spell List`) 이 12x12 그리드를 가리지 않는지 육안 확인
- [ ] 타일 클릭/hover 가 큰 맵에서도 정확 (PlayerInputHandler 의 `world_pos / TILE_SIZE` 변환)

---

## 범위 밖 (이번 작업 포함 안 함)

- BGM `StopBGM` 추가 여부 — 이전에 무음 버그 발생했으므로 별도 태스크
- 창 크기 자동 조정 — 조사 결과 12x12 (768px) 가 기본 창(1200x800) 안에 들어오므로 불필요
- 맵별 가용 캐릭터 확장 (Cleric/Wizard/Rogue)
- Exit 복수 지원
- Lava/Difficult 타일 포함 지형 맵 추가
- `"water"` → `"difficult"` 명명 정리 (삭제할 맵에만 있던 이름이라 무시)
