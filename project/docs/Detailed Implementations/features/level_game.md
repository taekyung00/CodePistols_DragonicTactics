# Level Game 설계 문서

## 개요

메인메뉴에 **Level Game** 모드를 추가한다. 각 레벨마다 고정된 적 구성, 맵 크기, Dragon이 사용 가능한 스펠이 제한된다.

---

## 메인메뉴 변경

| 빌드 | 버튼 목록 |
|---|---|
| Release (`windows-release`) | Setting / **Level Game** / Exit |
| Debug (`windows-debug` / developer) | Setting / Play Game / **Level Game** / Exit |

- Release에서는 "Play Game" 버튼을 제거하고 "Level Game"으로 대체
- Debug에서는 둘 다 표시 (`#if defined(DEVELOPER_VERSION)` 게이팅)

---

## 데이터 파일

`Assets/Data/Level_Map.json` — 레벨 정의 파일 (새로 생성)

### JSON 구조

기존 `maps.json`과 동일한 포맷 유지 + `enemies`, `allowed_spells` 필드 추가.

```json
{
  "levels": [
    {
      "id": "level_1",
      "name": "Level 1 (5x7)",
      "width": 5,
      "height": 7,
      "tiles": [
        "#####",
        "#...#",
        "#...#",
        "#...#",
        "#...#",
        "#...#",
        "#####"
      ],
      "legend": { "#": "wall", ".": "floor" },
      "enemies": ["fighter"],
      "allowed_spells": ["S_ATK_010", "S_GEO_020"],
      "spawn_points": {
        "dragon":  {"x": 2, "y": 1},
        "fighter": {"x": 2, "y": 5}
      }
    }
  ]
}
```

### 필드 설명

| 필드 | 타입 | 설명 |
|---|---|---|
| `id` | string | 레벨 식별자 (`"level_1"`, `"level_2"`, `"level_3"`) |
| `name` | string | 표시용 이름 |
| `width` / `height` | int | 맵 전체 크기 (벽 포함) |
| `tiles` | string[] | 타일 배치 (`#`=wall, `.`=floor) — 기존 maps.json과 동일 포맷 |
| `legend` | object | 타일 기호 정의 |
| `enemies` | string[] | 스폰할 AI 캐릭터 목록 |
| `allowed_spells` | string[] | Dragon이 사용 가능한 스펠 ID 목록. **빈 배열 = 모든 스펠 허용** |
| `spawn_points` | object | 각 캐릭터 초기 위치 (0,0 = 맵 좌상단, 벽 포함 기준) |

---

## 레벨별 상세

### 레벨 1 — Dragon vs Fighter

| 항목 | 값 |
|---|---|
| 맵 크기 | 5×7 (내부 플레이 공간 3×5) |
| 적 | Fighter |
| Dragon 스폰 | (2, 1) |
| Fighter 스폰 | (2, 5) |

**Dragon 허용 스펠:**

| 스펠 | ID |
|---|---|
| Fire Bolt | `S_ATK_010` |
| Wall Creation | `S_GEO_020` |

기본 공격(ActionAttack)은 스펠이 아니므로 항상 사용 가능.

---

### 레벨 2 — Dragon vs Fighter + Cleric

| 항목 | 값 |
|---|---|
| 맵 크기 | 8×8 (내부 플레이 공간 6×6) |
| 적 | Fighter, Cleric |
| Dragon 스폰 | (3, 1) |
| Fighter 스폰 | (3, 6) |
| Cleric 스폰 | (4, 6) |

**Dragon 허용 스펠 (레벨1 포함):**

| 스펠 | ID |
|---|---|
| Fire Bolt | `S_ATK_010` |
| Wall Creation | `S_GEO_020` |
| Dragon's Fury | `S_ATK_030` |
| Fearful Cry | `S_DEB_020` |
| Purify | `S_ENH_050` |
| Tail Swipe | `S_ATK_020` |

---

### 레벨 3 — Dragon vs All

| 항목 | 값 |
|---|---|
| 맵 크기 | 10×10 (내부 플레이 공간 8×8) |
| 적 | Fighter, Cleric, Rogue, Wizard |
| Dragon 스폰 | (4, 1) |
| Fighter 스폰 | (3, 8) |
| Cleric 스폰 | (4, 8) |
| Rogue 스폰 | (5, 8) |
| Wizard 스폰 | (6, 8) |

**Dragon 허용 스펠:** 모든 스펠 (`allowed_spells` 빈 배열)

---

## 구현 체크리스트

```
□ Assets/Data/Level_Map.json 생성
□ MainMenu.cpp — "Level Game" 버튼 추가 (release: "Play Game" 대체, debug: 둘 다 표시)
□ source/Game/ 직하위 — LevelSelect GameState 생성 (레벨 선택 화면)
□ GamePlay 또는 DataRegistry — Level_Map.json 로딩 로직
□ SpellSystem / PlayerInputHandler — allowed_spells 화이트리스트 적용
□ GamePlay 정적 필드 — s_level_id (현재 진행 레벨 전달용)
□ 맵 타일 — Level_Map.json의 width/height로 동적 생성 (테두리 #, 내부 .)
```

---

## 스펠 제한 적용 방식 (설계 메모)

- `allowed_spells`가 비어 있으면 모든 스펠 허용
- 비어 있지 않으면 해당 ID 목록만 허용
- 기본 공격(`ActionAttack`)은 스펠 시스템과 무관 → 항상 허용
- 제한 적용 위치: `PlayerInputHandler`의 스펠 슬롯 클릭 처리 또는 `SpellSystem::CanCast`
