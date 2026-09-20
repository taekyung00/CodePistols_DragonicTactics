# Terrain Spells 구현 가이드 — Magma Blast & Wall Creation

## 개요

두 스펠 모두 맵 타일을 변경하는 "지형 변경" 스펠이다.
SpellSystem의 summon 처리 스텁을 실제 로직으로 교체하고,
지형 효과의 생명주기(피해/소멸)를 관리하는 시스템을 추가한다.

---

## 스펠 스펙

### Magma Blast (S_GEO_010)

| 항목 | 값 |
|------|----|
| 타겟팅 | `Empty:Point:6` (빈 타일 1칸 선택, 6칸 이내) |
| 생성 범위 | 1×1 (선택한 단일 타일) |
| 피해 | 3d6/라운드 |
| 업캐스트 | +1d4/레벨 (`3d6 + (Spell Level - Required Spell Level)d4`) |
| 지속 | 3라운드 후 소멸 → 빈 타일 복원 |
| 피해 타이밍 1 | **턴 시작**: 해당 캐릭터 턴이 시작될 때 용암 타일 위에 있으면 피해 |
| 피해 타이밍 2 | **이동 중**: 이동 경로에서 용암 타일을 밟을 때마다 피해 (통과 가능) |

**CSV Effect 컬럼 변경:**
```
Deals 3d6 + (Spell Level - Required Spell Level)d4 damage.
Applies "Basic" status for 3 turns.
Move to current location.
Summons Lava Zone at selected location.
```
> `damage_formula` → 라운드당 피해로 재활용, `effect_duration` → 지속 라운드로 재활용

---

### Wall Creation (S_GEO_020)

| 항목 | 값 |
|------|----|
| 타겟팅 | `Empty:Point:5` (빈 타일 선택, 5칸 이내) |
| 기본 (레벨 1 슬롯) | 1클릭 → 벽 1개 |
| 업캐스트 | 레벨당 +1벽 (멀티클릭 방식) |
| 지속 | 2라운드 후 소멸 → 빈 타일 복원 |
| 입력 방식 | 업캐스트 개수만큼 순서대로 클릭 → 확인 버튼으로 시전 완료 |
| 슬롯 소모 | 시전 시 1회만 소모 (클릭 횟수와 무관) |

**CSV Effect 컬럼 변경:**
```
Deals 0 damage.
Applies "Basic" status for 2 turns.
Move to current location.
Summons Wall at selected location.
```
> `effect_duration` → 지속 라운드로 재활용

---

## 수정 파일 목록

| 파일 | 변경 내용 |
|------|-----------|
| `Assets/Data/spell_table.csv` | 두 스펠 Effect 컬럼 수정 |
| `StateComponents/SpellSystem.h` | `TerrainEffect` 구조체 + `TickTerrainEffects()` + `GetLavaDamageAt()` + `CastWalls()` + `m_terrain_effects` 추가 |
| `StateComponents/SpellSystem.cpp` | summon 스텁 → 실제 타일 변경 로직 + `TickTerrainEffects()` 구현 + `#include "TurnManager.h"` 추가 |
| `StateComponents/GridSystem.cpp` | `GetReachableTiles()` BFS에서 Lava 타일 통과 허용 |
| `StateComponents/AStar.cpp` | `FindPath()` — goal 체크 + neighbor 체크에서 Lava 통과 허용 |
| `StateComponents/AISystem.cpp` | `ExecuteDecision()` Move 분기의 `IsWalkable` 목적지 체크를 Lava 허용으로 변경 |
| `StateComponents/TurnManager.cpp` | 라운드 증가 시점에서 `TickTerrainEffects()` 호출 + `#include "SpellSystem.h"`, `#include "CombatSystem.h"` 추가 |
| `Objects/Components/MovementComponent.cpp` | 이동 중 Lava 타일 통과 허용 + 피해 적용 + `#include "SpellSystem.h"`, `#include "CombatSystem.h"` 추가 |
| `States/PlayerInputHandler.h` | `WallPlacementMulti` ActionState 추가 + 멀티클릭 관련 멤버 추가 |
| `States/PlayerInputHandler.cpp` | Wall Creation 멀티클릭 + 확인 버튼 처리 + `HandleRightClick` WallPlacementMulti 케이스 추가 + `HideAllSpellButtons`에 `btn_wall_confirm` 숨김 추가 |

---

## 구현 상세

### 1. spell_table.csv

위 스펙대로 두 스펠의 Effect 컬럼 수정.

---

### 2. SpellSystem.h

```cpp
// SpellData 구조체 위에 추가
struct TerrainEffect
{
    std::vector<Math::ivec2> affected_tiles;
    int                      damage_per_turn; // 0이면 벽 (피해 없음)
    int                      created_round;   // 생성된 라운드 번호
    int                      duration_rounds; // 지속 라운드 수
};
```

SpellSystem public 섹션에 추가:
```cpp
// TurnManager의 라운드 증가 시점에 호출
void TickTerrainEffects(int current_round);

// MovementComponent가 이동 중 타일 진입 시 호출
int  GetLavaDamageAt(Math::ivec2 tile) const;

// PlayerInputHandler가 Wall Creation 확인 버튼 시 호출 (슬롯 1회만 소모)
bool CastWalls(Character* caster, const std::string& spell_id,
               const std::vector<Math::ivec2>& tiles, int upcast_level);
```

private 섹션에 추가:
```cpp
std::vector<TerrainEffect> m_terrain_effects;
```

---

### 3. SpellSystem.cpp — summon 스텁 교체

현재:
```cpp
if (spell.summon_type != "NULL" && !spell.summon_type.empty())
    Engine::GetLogger().LogEvent("SpellSystem: Summon " + spell.summon_type + " at ...");
```

교체:
```cpp
if (spell.summon_type == "Wall")
{
    // CastWalls() 경로에서만 벽을 생성하므로 여기서는 단일 타일 처리
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    int current_round = Engine::GetGameStateManager().GetGSComponent<TurnManager>()->GetRoundNumber();
    if (grid && grid->IsValidTile(target_tile)
             && grid->GetTileType(target_tile) == GridSystem::TileType::Empty
             && !grid->IsOccupied(target_tile))
    {
        grid->SetTileType(target_tile, GridSystem::TileType::Wall);
        m_terrain_effects.push_back({ {target_tile}, 0, current_round, spell.effect_duration });
    }
}
else if (spell.summon_type == "Lava Zone")
{
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (!grid || !grid->IsValidTile(target_tile)) return;

    int current_round = Engine::GetGameStateManager().GetGSComponent<TurnManager>()->GetRoundNumber();
    int damage        = CalculateSpellDamage(spell, upcast_level);
    int duration      = spell.effect_duration;

    grid->SetTileType(target_tile, GridSystem::TileType::Lava);
    m_terrain_effects.push_back({ {target_tile}, damage, current_round, duration });
}
```

---

### 4. SpellSystem.cpp — TickTerrainEffects 구현

```cpp
void SpellSystem::TickTerrainEffects(int current_round)
{
    auto& gs     = Engine::GetGameStateManager();
    auto* grid   = gs.GetGSComponent<GridSystem>();
    auto* combat = gs.GetGSComponent<CombatSystem>();
    if (!grid || !combat) return;

    // 만료된 효과 제거 + 타일 복원
    m_terrain_effects.erase(
        std::remove_if(m_terrain_effects.begin(), m_terrain_effects.end(),
            [&](const TerrainEffect& e) {
                if (current_round >= e.created_round + e.duration_rounds)
                {
                    for (const auto& tile : e.affected_tiles)
                        grid->SetTileType(tile, GridSystem::TileType::Empty);
                    return true;
                }
                return false;
            }),
        m_terrain_effects.end());
}
```

---

### 5. SpellSystem.cpp — GetLavaDamageAt 구현

```cpp
int SpellSystem::GetLavaDamageAt(Math::ivec2 tile) const
{
    for (const auto& effect : m_terrain_effects)
    {
        if (effect.damage_per_turn <= 0) continue;
        for (const auto& t : effect.affected_tiles)
            if (t.x == tile.x && t.y == tile.y)
                return effect.damage_per_turn;
    }
    return 0;
}
```

---

### 6. SpellSystem.cpp — CastWalls 구현

슬롯은 1회만 소모하고 여러 타일에 벽 생성.

```cpp
bool SpellSystem::CastWalls(Character* caster, const std::string& spell_id,
                             const std::vector<Math::ivec2>& tiles, int upcast_level)
{
    if (!caster || tiles.empty()) return false;

    auto it = spells_.find(spell_id);
    if (it == spells_.end()) return false;
    const SpellData& spell = it->second;

    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    auto* tm   = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
    if (!grid || !tm) return false;

    // 스펠 슬롯 소모 (1회)
    caster->ConsumeSpell(upcast_level > 0 ? upcast_level : spell.spell_level);

    // AP 소모
    caster->GetActionPointsComponent()->Consume(1);

    int current_round = tm->GetRoundNumber();

    for (const auto& tile : tiles)
    {
        if (grid->IsValidTile(tile)
                && grid->GetTileType(tile) == GridSystem::TileType::Empty
                && !grid->IsOccupied(tile))
        {
            grid->SetTileType(tile, GridSystem::TileType::Wall);
            m_terrain_effects.push_back({ {tile}, 0, current_round, spell.effect_duration });
        }
    }
    return true;
}
```

---

### 7. TurnManager.cpp — 두 곳에 호출 분리

#### 7-A. 라운드 증가 시점 — 만료 처리만

`EndCurrentTurn()` 내 `currentTurnIndex == 0` 블록에 추가:

```cpp
if (currentTurnIndex == 0)
{
    roundNumber++;
    Engine::GetLogger().LogEvent("TurnManager: Round " + std::to_string(roundNumber) + " started");

    // 지형 효과 만료 처리 (피해 없음)
    auto* spell_system = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    if (spell_system)
        spell_system->TickTerrainEffects(roundNumber);

    // ... 기존 Initiative re-roll 코드 ...
}
```

#### 7-B. 턴 시작 시점 — 현재 캐릭터만 피해

`StartNextTurn()` 내 `handler->OnTurnStart(currentChar)` 이후에 추가:

```cpp
// 용암 턴 시작 피해 — 현재 캐릭터가 용암 위에 있으면 피해
auto* spell_system = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
auto* combat       = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
if (spell_system && combat && currentChar->IsAlive())
{
    int dmg = spell_system->GetLavaDamageAt(currentChar->GetGridPosition()->Get());
    if (dmg > 0)
        combat->ApplyDamage(nullptr, currentChar, dmg);
}
```

> 결과: Dragon 턴 시작 → Dragon만 피해 체크. Fighter 턴 시작 → Fighter만 피해 체크.

---

### 8. AStar.cpp — FindPath Lava 통과 허용

`GetReachableTiles`(BFS)와 달리 `FindPath`(A*)도 `IsWalkable`을 두 곳에서 사용한다.
둘 다 수정하지 않으면 BFS에는 Lava가 표시되지만 실제 경로 계산이 실패한다.

**goal 체크 (line 53 근처):**

현재:
```cpp
if (!IsWalkable(goal))
{
    Engine::GetLogger().LogError("GridSystem : Goal is not walkable");
    return {};
}
```

변경:
```cpp
{
    TileType goal_type = GetTileType(goal);
    bool goal_passable = (goal_type == TileType::Empty || goal_type == TileType::Lava);
    if (!goal_passable || IsOccupied(goal))
    {
        Engine::GetLogger().LogError("GridSystem : Goal is not walkable");
        return {};
    }
}
```

**neighbor 체크 (line 103 근처):**

현재:
```cpp
if (!IsWalkable(neighborPos))
    continue;
```

변경:
```cpp
{
    TileType n_type = GetTileType(neighborPos);
    bool n_passable = (n_type == TileType::Empty || n_type == TileType::Lava)
                      && !IsOccupied(neighborPos);
    if (!n_passable)
        continue;
}
```

> `IsWalkable()` 자체는 변경하지 않는다. knockback/teleport 등 IsWalkable에 의존하는 다른 로직에 영향 없음.

---

### 8-B. AISystem.cpp — Lava 목적지 IsWalkable 체크 수정

`ExecuteDecision()` Move 분기 (AISystem.cpp):

현재:
```cpp
if (grid && grid->IsWalkable(decision.destination))
{
    path = grid->FindPath(actor->GetGridPosition()->Get(), decision.destination);
    ...
}
```

변경:
```cpp
if (grid)
{
    GridSystem::TileType dt = grid->GetTileType(decision.destination);
    bool dest_ok = (dt == GridSystem::TileType::Empty || dt == GridSystem::TileType::Lava)
                   && !grid->IsOccupied(decision.destination);
    if (dest_ok)
    {
        path = grid->FindPath(actor->GetGridPosition()->Get(), decision.destination);
        ...
    }
}
```

---

### 9. GridSystem.cpp — GetReachableTiles BFS Lava 통과 허용

현재 BFS 조건:
```cpp
if (GetTileType(neighbor) == TileType::Empty && !IsOccupied(neighbor))
```

변경:
```cpp
TileType t = GetTileType(neighbor);
if ((t == TileType::Empty || t == TileType::Lava) && !IsOccupied(neighbor))
```

> `IsWalkable()`은 건드리지 않음 — 다른 시스템(벽 차단 등)에 영향 없음

---

### 10. MovementComponent.cpp — Lava 통과 + 피해

`Update()` 내 이동 처리 부분 변경:

현재:
```cpp
if (m_gridSystem->IsWalkable(next_pos))
{
    // 이동 처리
}
else
{
    m_current_path.clear(); // 막힘
}
```

변경:
```cpp
GridSystem::TileType tile_type = m_gridSystem->GetTileType(next_pos);
bool can_enter = (tile_type == GridSystem::TileType::Empty || tile_type == GridSystem::TileType::Lava)
                 && !m_gridSystem->IsOccupied(next_pos);

if (can_enter)
{
    m_gridSystem->MoveCharacter(current_pos, next_pos);
    m_gridPosition->Set(next_pos);
    m_owner->SetPosition({ ... });
    m_stats->ReduceSpeed();

    // 용암 피해
    if (tile_type == GridSystem::TileType::Lava)
    {
        auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
        auto* combat    = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
        if (spell_sys && combat)
        {
            int dmg = spell_sys->GetLavaDamageAt(next_pos);
            if (dmg > 0)
                combat->ApplyDamage(nullptr, static_cast<Character*>(m_owner), dmg);
        }
    }
}
else
{
    m_current_path.clear();
}
```

---

### 11. PlayerInputHandler — Wall Creation 멀티클릭

**PlayerInputHandler.h에 추가:**
```cpp
enum class ActionState
{
    None,
    SelectingMove,
    Moving,
    SelectingAction,
    TargetingForAttack,
    SelectingSpell,
    TargetingForSpell,
    WallPlacementMulti   // ← 추가
};

// 멤버 추가
std::vector<Math::ivec2> m_wall_placement_tiles;
int                      m_wall_placement_count = 0;
```

**SelectSpell() 수정:**
Wall Creation(`S_GEO_020`) 선택 시 `WallPlacementMulti` 상태로 진입:
```cpp
if (spell_id == "S_GEO_020")
{
    m_wall_placement_tiles.clear();
    m_wall_placement_count = upcast_level - spell->spell_level + 1; // 필요한 벽 개수
    m_state = ActionState::WallPlacementMulti;
    grid->EnableSpellTargetingMode(...);
}
else
{
    m_state = ActionState::TargetingForSpell;
    // 기존 코드
}
```

**HandleMouseClick()에 WallPlacementMulti 케이스 추가:**
```cpp
case ActionState::WallPlacementMulti:
{
    if (grid->IsValidTile(grid_pos)
            && grid->GetTileType(grid_pos) == GridSystem::TileType::Empty
            && !grid->IsOccupied(grid_pos))
    {
        // 중복 타일 방지
        bool already_selected = std::find(m_wall_placement_tiles.begin(),
                                          m_wall_placement_tiles.end(),
                                          grid_pos) != m_wall_placement_tiles.end();
        if (!already_selected)
        {
            m_wall_placement_tiles.push_back(grid_pos);
            Engine::GetLogger().LogEvent("Wall placement: " + std::to_string(m_wall_placement_tiles.size())
                + "/" + std::to_string(m_wall_placement_count));
        }
    }
    break;
}
```

**확인 버튼 처리 (ImGui 또는 ButtonManager):**
```cpp
if (btns.IsPressed("btn_wall_confirm") && m_state == ActionState::WallPlacementMulti)
{
    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto* grid_sys  = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (spell_sys && !m_wall_placement_tiles.empty())
    {
        spell_sys->CastWalls(dragon, m_selected_spell_id,
                             m_wall_placement_tiles, m_selected_upcast_level);
    }
    m_wall_placement_tiles.clear();
    m_state = ActionState::None;
    btns.SetVisible("btn_wall_confirm", false);
    if (grid_sys) grid_sys->DisableSpellTargetingMode();
}
```

**HandleRightClick에 WallPlacementMulti 케이스 추가:**

`HandleRightClick()` 내 `TargetingForSpell` 블록 아래에 추가:
```cpp
if (m_state == ActionState::WallPlacementMulti)
{
    m_wall_placement_tiles.clear();
    btns.SetVisible("btn_wall_confirm", false);  // ← btns를 인자로 받도록 시그니처 수정 필요
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (grid) grid->DisableSpellTargetingMode();
}
```

> `HandleRightClick(Dragon* dragon)` 시그니처에 `ButtonManager& btns` 인자를 추가하거나,
> `Engine::GetGameStateManager()`로 btns를 직접 접근하는 대신 WallPlacementMulti 상태 정리만 하고
> 버튼 숨김은 `Update()` 루프에서 상태 기반으로 처리하는 방식도 가능.

**HideAllSpellButtons에 btn_wall_confirm 추가:**

`HideAllSpellButtons()` 함수 내 첫 줄 근처에 추가:
```cpp
btns.SetVisible("btn_wall_confirm", false);
```

---

## 턴 시작 피해 흐름

```
Dragon 턴 시작  → Dragon 위치가 용암? → Dragon만 피해
Fighter 턴 시작 → Fighter 위치가 용암? → Fighter만 피해
라운드 종료     → TickTerrainEffects → 만료된 용암/벽 타일 복원
```

`TickTerrainEffects`는 만료 처리 전용. 피해는 `StartNextTurn()`에서 현재 캐릭터에게만 적용.

---

---

## 설계 이슈 (버그 아님, 확인 필요)

**Lava 피해는 생성 시 한 번 계산된 값을 고정 사용**

`CalculateSpellDamage(spell, upcast_level)` 를 Lava 생성 시점에 한 번만 호출하여 `damage_per_turn`에 저장한다.
즉, "3d6/라운드"라고 써있지만 실제로는 "생성 시 3d6 굴려서 나온 값을 매 라운드 동일하게 적용"이다.
매 라운드 새로 굴리는 것이 의도라면 `TickTerrainEffects` 내부에서 매번 dice를 굴려야 한다.

---

## 검증 시나리오

1. **Magma Blast 기본**: 레벨 2 슬롯으로 시전 → 선택 타일이 용암(주황)으로 변경 → 3라운드 후 빈 타일 복원
2. **Magma Blast 피해**: 용암 위 캐릭터 → 라운드 넘어갈 때 3d6 피해
3. **Magma Blast 이동 피해**: 캐릭터가 용암 타일을 지나갈 때마다 3d6 피해, 경로 차단 없음
4. **Magma Blast 업캐스트**: 레벨 3 슬롯 → 피해 = 3d6 + 1d4/라운드
5. **Wall Creation 기본**: 레벨 1 슬롯 → 1클릭 → 확인 → 벽 1개, 슬롯 1개 소모 → 2라운드 후 소멸
6. **Wall Creation 업캐스트**: 레벨 3 슬롯 → 3클릭 → 확인 → 벽 3개, 슬롯 1개 소모
7. **이동 차단**: Wall 타일로 이동 불가 (GridSystem 자동 처리)
8. **용암 Pathfinding**: 용암 타일을 경유하는 경로 계산 가능
