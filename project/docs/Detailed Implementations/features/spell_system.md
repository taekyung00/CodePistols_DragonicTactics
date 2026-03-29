# 스펠 시스템 구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: SpellSystem - CSV 파싱, 스펠 실행, 플레이어/AI 통합
**작성일**: 2026-03-08

**관련 파일**:

- [SpellSystem.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/SpellSystem.h)
- [AISystem.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AISystem.cpp)
- [IAIStrategy.h](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/IAIStrategy.h)
- [PlayerInputHandler.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/PlayerInputHandler.h)
- [Assets/Data/spell_table.csv](../../../DragonicTactics/Assets/Data/spell_table.csv)
- [Assets/Data/status_effect.csv](../../../DragonicTactics/Assets/Data/status_effect.csv)

---

## 개요

현재 `SpellSystem`은 `MockSpellBase`를 사용하는 테스트 목 구조로만 존재하며, `GamePlay::Load()`에서 주석 처리되어 있습니다.
이 문서는 CSV 기반 실제 스펠 데이터 파싱부터 플레이어 입력/AI 턴 통합까지의 전체 구현을 다룹니다.

### CSV 형식 변경 이력

`spell_table.csv`의 컬럼 명칭과 Effect 필드 형식이 변경되었습니다.
파싱 로직과 `SpellData` 구조체도 함께 변경됩니다. 하단 구현 예시에서 구형/신형이 병기됩니다.

| 항목               | 구형                      | 신형                                |
| ---------------- | ----------------------- | --------------------------------- |
| 컬럼 이름 언어         | 한국어                     | 영어                                |
| Classes 구분자      | `/` (`"Dragon/Wizard"`) | `, ` (`"Dragon, Wizard"`)         |
| 사거리 형식           | `"4칸"`                  | `"4 spaces"`                      |
| Effect 필드        | 자유 텍스트                  | 4줄 고정 템플릿                         |
| `upcastable` 허용값 | `"TRUE"` / `"O"`        | `"TRUE"`                          |
| SpellData 분기 필드  | `spell_school`          | `damage_formula`, `effect_status` |

---

### spell_table.csv 컬럼 구조

#### 구형 컬럼 (참조용)

```
ID, 이름, 계열, 사용 클래스, 요구 슬롯 레벨, 타겟, 사거리, 업캐스트 가능, 효과 설명
```

예시:

- `S_ATK_010` = 화염탄, 공격형, Dragon/Wizard, 레벨 1, 범위 공격, 4칸, 업캐스트 가능
- `S_BUF_010` = 신성한 보호, 버프형, Cleric, 레벨 1, 아군 대상, 4칸

#### 신형 컬럼 (현재)

```
col[0] ID              — "S_ATK_050"
col[1] Name            — "Smite"
col[2] Category        — "Attack", "Buff", "Terrain Change" (빈 값 허용)
col[3] Classes         — "Fighter" / "Dragon, Fighter" (쉼표+공백 구분)
col[4] Required Slot   — "1", "0" (level)
col[5] Target          — "Single Enemy", "Self", "Enemies Around Caster" 등
col[6] Range           — "1 spaces", "4 spaces", "-" (없음)
col[7] Upcasting       — "TRUE" / "FALSE"
col[8] Effect          — 아래 4줄 템플릿 (quoted multiline)
```

**Effect 필드 템플릿** (모든 스펠에 동일한 구조):

```
Deals {damage} damage.
Applies "{STATUS}" status for {N} turns.
Move to {location}.
Summons {entity} at {location}.
```

- `{damage}` : `"3d8 + (Spell Level - Required Spell Level)d8"`, `"0"`, `"-(1d10+Xd10)"` 등
- `"{STATUS}"` : `status_effect.csv`에 정의된 영문 이름. `"Basic"` = 상태 효과 없음 (0턴)
- `{location}` : `"current location"` (제자리), `"furthest position from the Dragon within X spaces"` 등
- `{entity}` : `"NULL"` (없음), `"Lava Zone"`, `"Wall"` 등

예시 (`S_ATK_050 Smite`):

```
Deals 3d8 + (Spell Level - Required Spell Level)d8 damage.
Applies "Lifesteal" status for 0 turns.
Move to current location.
Summons NULL at current location.
```

> **참고 — duration=0 처리**: Smite는 `effect_status = "Lifesteal"`, `effect_duration = 0`으로 파싱된다.
> `ApplySpellEffect`의 조건 `spell.effect_duration > 0`이 거짓이므로 상태 효과는 실제로 적용되지 않는다. ✓

> **⚠️ 파싱 주의 — S_ENH_010 Bloodlust**: CSV의 Bloodlust Effect는 표준 템플릿과 일부 다르다.
> 
> - `Apply "Lifesteal" status for 1 turn` — `Applies` 아님, `turns` 아닌 `turn` (단수)
> - `Move to current position` — `current location` 아님
> 
> `ParseEffectField`의 duration 파싱은 `" turns"`를 찾으므로 `"turn"`(단수)에서 매칭 실패 →
> `effect_duration = 0` → Lifesteal 미적용 버그 발생.
> **해결**: `ParseEffectField`에서 `" turns"` 대신 `" turn"` 검색으로 변경하거나 CSV를 `turns`(복수)로 통일.

---

### status_effect.csv 구조

`spell_table.csv`의 Effect에서 참조하는 상태 효과를 정의하는 companion 파일:

```
NAME, DESCRIPTION
```

현재 정의된 상태 효과:

| NAME         | 설명 (status_effect.csv)                                                                                 |
| ------------ | ------------------------------------------------------------------------------------------------------ |
| `Lifesteal`  | Recover 50% of damage dealt this turn (round down)                                                     |
| `Frenzy`     | If next attack deals 10+ damage, target receives random debuff (Curse/Fear/Exhaustion); otherwise self |
| `Exhaustion` | Speed and Action points become 0 next turn                                                             |
| `Purify`     | Removes all status effects from self                                                                   |
| `Blessing`   | All damage taken -3, all damage dealt +3                                                               |
| `Curse`      | All damage taken +3, all damage dealt -3                                                               |
| `Haste`      | Speed +1, Actions +1                                                                                   |
| `Stealth`    | Untargetable. First damage dealt next turn is doubled. Removed upon attacking.                         |
| `Fear`       | all damage dealt -3, speed -1                                                                          |

> **effectName은 이 표의 NAME 컬럼과 정확히 일치해야 합니다.**
> Strategy의 `HasBuff(actor, "Lifesteal")`, `HasDebuff(actor, "Fear")` 등의 인수가 여기서 옵니다.

---

### AI 스펠 실행 경로

`AISystem::ExecuteDecision()`의 `UseAbility` 케이스가 스펠을 직접 실행합니다.
AI가 스펠을 사용하려면 Strategy에서 `decision.type = AIDecisionType::UseAbility`,
`decision.abilityName = spell_id`를 반환하기만 하면 됩니다.
`AISystem::ExecuteDecision`은 `CastSpell(caster, abilityName, target->GetGridPosition()->Get())`를
호출하므로, 새 SpellSystem의 메인 시전 함수명은 **`CastSpell`**이어야 합니다.

---

## Implementation Tasks

### Task 1: SpellData 구조체 확장 및 SpellSystem 헤더 재설계

**수정 파일**: `CharacterTypes.h`, `SpellSystem.h`

- `CharacterTypes.h`의 `SpellData` 구조체에 CSV 9개 컬럼 + Effect 템플릿 파싱 결과 필드를 추가한다.
  - `usable_classes`는 `", "` 구분 파싱 → `std::vector<std::string>`
  - `range`는 `"4 spaces"` 형태에서 숫자만 추출 → `int`
  - Effect 필드는 4줄 템플릿을 `ParseEffectField`로 분해 → `damage_formula`, `effect_status`, `effect_duration`, `move_type`, `summon_type` 저장
- `SpellSystem.h`에서 `MockSpellBase` 의존성을 완전히 제거하고 `SpellData` 기반 API로 교체한다.
- 메인 시전 함수명은 `CastSpell`을 유지한다. `AISystem::ExecuteDecision`이 이미 이 이름으로 호출하고 있어 수정이 불필요하다.
- private에 파싱 헬퍼(`SplitByDelimiter`, `ParseRange`, `ParseCSVRow`, `ParseEffectField`, `ReadCSVRecord`)와 실행 헬퍼(`ApplySpellEffect`, `CalculateSpellDamage`)를 선언한다.

---

### Task 2: CSV 파서 및 실행 로직 구현 (`SpellSystem.cpp`)

**수정 파일**: `SpellSystem.cpp`

- `ReadCSVRecord(file)`: 파일에서 레코드 하나(quoted multiline 포함)를 읽어 컬럼 벡터로 반환한다. Effect 필드는 `"..."` 형태로 개행을 포함할 수 있으므로, 닫는 따옴표를 만날 때까지 줄을 이어 붙인다.
- `LoadFromCSV`: 헤더 레코드를 스킵하고 `ReadCSVRecord`를 반복 호출해 `ParseCSVRow`로 `SpellData`를 만들어 `spells_` 맵에 저장한다.
- `SplitByDelimiter(str, delim)`: 구분자로 문자열을 분리하고 앞뒤 공백을 제거한다. Classes 파싱(`,`) 양쪽에 사용한다.
- `ParseRange(range_str)`: `"4 spaces"` 형태에서 숫자만 추출해 `int`로 반환한다. `"-"` 또는 빈 문자열은 `0`으로 처리한다.
- `ParseCSVRow(col)`: 9개 컬럼 벡터를 `SpellData` 필드에 매핑한다. Classes는 `SplitByDelimiter(col[3], ',')`. `upcastable`은 `"TRUE"`일 때 `true`.
- `ParseEffectField(effect_str, data)`: Effect 4줄 템플릿을 파싱해 `SpellData`의 `damage_formula`, `effect_status`, `effect_duration`, `move_type`, `summon_type`을 채운다.
- `CalculateSpellDamage`: `damage_formula`의 주사위 표기법(`"3d8"`, `"2d8"`)을 `DiceManager::RollDiceFromString`으로 계산한다. 업캐스트 레벨만큼 추가 다이스를 굴린다. `"0"`이면 0 반환.
- `ApplySpellEffect`: `spell.damage_formula != "0"` → 피해 적용. `spell.effect_status != "Basic"` → `AddEffect` 호출 + `StatusEffectHandler::OnApplied()` 위임. `spell.summon_type != "NULL"` → 지형 생성. `spell.move_type != "current location"` → 특수 이동. 분기 기준을 `spell_school`이 아닌 **파싱된 Effect 필드**로 결정한다.
- `CastSpell`: 스펠 존재 확인 → `CanCast` 검증 → `ConsumeSpell`로 슬롯 소모 → `ApplySpellEffect` 순서로 실행한다.

---

### Task 3: GamePlay::Load()에 SpellSystem 등록

**수정 파일**: `GamePlay.cpp`

`Load()` 함수에서 `SpellSystem` 관련 주석을 해제하고, 등록 직후 `LoadFromCSV`를 호출해 스펠 데이터를 로드한다.

---

### Task 4: PlayerInputHandler에 스펠 실행 연동

**수정 파일**: `PlayerInputHandler.h`, `PlayerInputHandler.cpp`

신규 추가 항목:

| 항목                                | 위치                             | 설명                 |
| --------------------------------- | ------------------------------ | ------------------ |
| `std::string m_selected_spell_id` | `PlayerInputHandler.h` private | 현재 선택된 스펠 ID 저장    |
| `void SelectSpell(spell_id)`      | `PlayerInputHandler.h` public  | UI에서 스펠 선택 시 호출    |
| `Math::ivec2 WorldToTile(vec2)`   | `PlayerInputHandler.h` private | 마우스 좌표 → 타일 인덱스 변환 |
| `TargetingForSpell` case 구현       | `PlayerInputHandler.cpp`       | stub 교체            |

#### `PlayerInputHandler.h` 수정

```cpp
#pragma once
#include "Engine/Vec2.h"
#include <memory>
#include <string>                  // 신규 추가
#include "./Engine/Component.h"

class Character;
class Dragon;
class GridSystem;
class CombatSystem;

class PlayerInputHandler
{
public:
    enum class ActionState { /* 기존 그대로 */ };

    // ... 기존 메서드 ...

    // 신규 추가: UI에서 스펠 선택 시 호출
    void SelectSpell(const std::string& spell_id)
    {
        m_selected_spell_id = spell_id;
        m_state = ActionState::TargetingForSpell;
    }

private:
    ActionState m_state = ActionState::None;
    std::string m_selected_spell_id;          // 신규 추가

    // ... 기존 private 메서드 ...

    // 신규 추가: 마우스 월드 좌표 → 타일 인덱스
    Math::ivec2 WorldToTile(Math::vec2 world_pos) const;
};
```

#### `WorldToTile` 구현 (`PlayerInputHandler.cpp`)

```cpp
// GridSystem::TILE_SIZE = MAP_WIDTH * MAP_HEIGHT = 8 * 8 = 64 픽셀
Math::ivec2 PlayerInputHandler::WorldToTile(Math::vec2 world_pos) const
{
    int tile_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
    int tile_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
    return { tile_x, tile_y };
}
```

#### `TargetingForSpell` case 구현 (`PlayerInputHandler.cpp`)

현재 stub:

```cpp
case ActionState::TargetingForSpell:
    // 스펠 타겟 선택 (미구현)
    break;
```

교체:

```cpp
case ActionState::TargetingForSpell:
{
    Math::ivec2  clicked_tile = WorldToTile(mouse_pos);
    SpellSystem* spell_sys    =
        Engine::GetGameStateManager().GetGSComponent<SpellSystem>();

    if (spell_sys && spell_sys->CanCast(dragon, m_selected_spell_id, clicked_tile))
    {
        spell_sys->CastSpell(dragon, m_selected_spell_id, clicked_tile);
        m_state = ActionState::None;
    }
    else
    {
        Engine::GetLogger().LogDebug(
            "Cannot cast " + m_selected_spell_id + " at ("
            + std::to_string(clicked_tile.x) + ", "
            + std::to_string(clicked_tile.y) + ")");
    }
    break;
}
```

---

### Task 5: AI 스펠 사용 — AISystem::ExecuteDecision 연동

**수정 파일**: `AISystem.cpp` 수정 불필요. 각 Strategy 파일만 수정.

`AISystem::ExecuteDecision`의 `UseAbility` 케이스는 이미 `CastSpell`을 호출한다. AI가 스펠을 사용하려면 각 Strategy의 `MakeDecision`에서 `AIDecisionType::UseAbility`를 반환하기만 하면 된다. `decision.abilityName`에 `spell_table.csv`의 ID를 설정하고, `decision.target`에 대상 캐릭터를 지정한다.

실행 흐름:

```
Strategy::MakeDecision() → AIDecision{UseAbility, "S_ATK_010", target}
    → AISystem::ExecuteDecision()
        → SpellSystem::CastSpell(actor, "S_ATK_010", target->GetGridPosition()->Get())
```

---

## Implementation Examples

### SpellData 구조체 (`CharacterTypes.h`)

#### 구형 SpellData (CSV 형식 변경 전)

```cpp
struct SpellData
{
    std::string id;           // "S_ATK_010"
    std::string spell_name;   // "화염탄"
    std::string spell_school; // "공격", "버프", "디버프", "강화", "지형"
    int         spell_level;  // 요구 슬롯 레벨 (0 = 슬롯 불필요)
    std::string target_type;  // "적 단일", "아군 단일", "자신", "범위"
    int         range;        // 타일 수 ("-"는 0으로 처리)
    bool        upcastable;   // 업캐스트 가능 여부
    std::string effect_desc;  // 효과 설명 전체
    std::string base_damage;  // "2d6", "3d8+2" 형식 (없으면 빈 문자열)

    std::vector<std::string> usable_classes; // ["Dragon", "Wizard"] — '/' 구분 파싱
};
```

#### 신형 SpellData (현재)

```cpp
struct SpellData
{
    // ── CSV 컬럼 (col[0]~col[8]) ──
    std::string id;           // col[0]  "S_ATK_050"
    std::string spell_name;   // col[1]  "Smite"
    std::string category;     // col[2]  "Attack", "Buff", "Terrain Change" (빈 값 허용)
    int         spell_level;  // col[4]  요구 슬롯 레벨 (0 = 슬롯 불필요)
    std::string target_type;  // col[5]  "Single Enemy", "Self", "Enemies Around Caster"
    int         range;        // col[6]  타일 수 ("-" → 0)
    bool        upcastable;   // col[7]  TRUE / FALSE

    std::vector<std::string> usable_classes; // col[3]  ["Dragon", "Fighter"] — ", " 구분

    // ── Effect 템플릿 파싱 결과 (col[8] → ParseEffectField) ──
    std::string damage_formula;  // "3d8", "0", "-(1d10)", "8 * (Spell Level + 1 - ...)"
    std::string effect_status;   // status_effect.csv의 NAME. "Basic" = 상태 없음
    int         effect_duration; // 상태 지속 턴 ("Basic"이면 0)
    std::string move_type;       // "current location", "furthest position from the Dragon..."
    std::string summon_type;     // "NULL", "Lava Zone", "Wall"

    std::string effect_raw;      // 파싱 전 원본 Effect 문자열 (디버그/툴팁용)
};
```

---

### SpellSystem 헤더 (`SpellSystem.h`)

#### 구형 SpellSystem.h (참조용)

```cpp
class SpellSystem : public CS230::Component
{
public:
    void LoadFromCSV(const std::string& csv_path);

    bool HasSpell(const std::string& spell_id) const;
    std::vector<std::string> GetAvailableSpells(Character* caster) const;
    bool CanCast(Character* caster, const std::string& spell_id,
                 Math::ivec2 target_tile) const;

    // AISystem::ExecuteDecision이 이 이름으로 호출 — 이름 변경 금지
    bool CastSpell(Character* caster, const std::string& spell_id,
                   Math::ivec2 target_tile, int upcast_level = 0);

    const SpellData* GetSpellData(const std::string& spell_id) const;

private:
    std::map<std::string, SpellData> spells_;

    SpellData                ParseCSVRow(const std::vector<std::string>& columns) const;
    std::vector<std::string> SplitByDelimiter(const std::string& str, char delim) const;
    int                      ParseRange(const std::string& range_str) const;

    void ApplySpellEffect(Character* caster, const SpellData& spell,
                          Math::ivec2 target_tile, int upcast_level);
    int  CalculateSpellDamage(const SpellData& spell, Character* caster, int upcast_level);
};
```

#### 신형 SpellSystem.h (현재)

```cpp
class SpellSystem : public CS230::Component
{
public:
    void LoadFromCSV(const std::string& csv_path);

    bool HasSpell(const std::string& spell_id) const;
    std::vector<std::string> GetAvailableSpells(Character* caster) const;
    bool CanCast(Character* caster, const std::string& spell_id,
                 Math::ivec2 target_tile) const;

    // AISystem::ExecuteDecision이 이 이름으로 호출 — 이름 변경 금지
    bool CastSpell(Character* caster, const std::string& spell_id,
                   Math::ivec2 target_tile, int upcast_level = 0);

    const SpellData* GetSpellData(const std::string& spell_id) const;

private:
    std::map<std::string, SpellData> spells_;

    // 파싱 헬퍼
    std::vector<std::string> ReadCSVRecord(std::ifstream& file) const;
    SpellData                ParseCSVRow(const std::vector<std::string>& columns) const;
    void                     ParseEffectField(const std::string& effect_str,
                                              SpellData& data) const;
    std::vector<std::string> SplitByDelimiter(const std::string& str, char delim) const;
    int                      ParseRange(const std::string& range_str) const;

    // 실행 헬퍼
    void ApplySpellEffect(Character* caster, const SpellData& spell,
                          Math::ivec2 target_tile, int upcast_level);
    int  CalculateSpellDamage(const SpellData& spell, int upcast_level);
};
```

---

### SplitByDelimiter

구형/신형 동일. CSV 행 분리(`,`)와 사용 클래스 분리(`/` 또는 `,`) 양쪽에 사용하는 범용 헬퍼:

```cpp
std::vector<std::string> SpellSystem::SplitByDelimiter(const std::string& str,
                                                        char delim) const
{
    std::vector<std::string> result;
    std::stringstream        ss(str);
    std::string              token;
    while (std::getline(ss, token, delim))
    {
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end   = token.find_last_not_of(" \t\r\n");
        if (start != std::string::npos)
            result.push_back(token.substr(start, end - start + 1));
    }
    return result;
}
```

---

### ParseRange

#### 구형: `"4칸"` → `4`, `"-"` → `0`

#### 신형: `"4 spaces"` → `4`, `"-"` → `0`

구현 로직 동일 (숫자만 추출):

```cpp
int SpellSystem::ParseRange(const std::string& range_str) const
{
    if (range_str.empty() || range_str == "-")
        return 0;

    std::string digits;
    for (char c : range_str)
    {
        if (std::isdigit(static_cast<unsigned char>(c)))
            digits += c;
    }
    return digits.empty() ? 0 : std::stoi(digits);
}
```

---

### ReadCSVRecord — quoted multiline 처리 (신형 전용)

Effect 필드는 `"..."` 형태의 quoted multiline이므로 단순 `getline`으로는 파싱 불가:

```cpp
// 파일에서 CSV 레코드 1개를 읽어 컬럼 벡터로 반환
// quoted field 내의 개행("\n")을 올바르게 처리
std::vector<std::string> SpellSystem::ReadCSVRecord(std::ifstream& file) const
{
    std::vector<std::string> columns;
    std::string              cell;
    bool                     in_quotes = false;
    char                     c;

    while (file.get(c))
    {
        if (c == '"')
        {
            if (in_quotes && file.peek() == '"') // escaped quote ""
            {
                cell += '"';
                file.get(c); // 두 번째 " 소비
            }
            else
            {
                in_quotes = !in_quotes;
            }
        }
        else if (c == ',' && !in_quotes)
        {
            columns.push_back(cell);
            cell.clear();
        }
        else if (c == '\n' && !in_quotes)
        {
            columns.push_back(cell);
            return columns; // 레코드 완료
        }
        else if (c == '\r')
        {
            // skip
        }
        else
        {
            cell += c;
        }
    }

    if (!cell.empty() || !columns.empty())
        columns.push_back(cell); // 마지막 컬럼

    return columns;
}
```

---

### LoadFromCSV

#### 구형 LoadFromCSV (단순 getline, 구 CSV 형식)

```cpp
void SpellSystem::LoadFromCSV(const std::string& csv_path)
{
    std::ifstream file(csv_path);
    if (!file.is_open())
    {
        Engine::GetLogger().LogError("SpellSystem: Failed to open " + csv_path);
        return;
    }

    std::string line;
    bool        first_line = true;
    while (std::getline(file, line))
    {
        if (first_line) { first_line = false; continue; } // 헤더 스킵
        if (line.empty()) continue;

        auto columns = SplitByDelimiter(line, ',');
        if (columns.size() < 9 || columns[0].empty()) continue;

        SpellData data   = ParseCSVRow(columns);
        spells_[data.id] = data;
    }
    Engine::GetLogger().LogEvent(
        "SpellSystem: Loaded " + std::to_string(spells_.size()) + " spells");
}
```

#### 신형 LoadFromCSV (ReadCSVRecord 기반)

```cpp
void SpellSystem::LoadFromCSV(const std::string& csv_path)
{
    std::ifstream file(csv_path);
    if (!file.is_open())
    {
        Engine::GetLogger().LogError("SpellSystem: Failed to open " + csv_path);
        return;
    }

    ReadCSVRecord(file); // 헤더 스킵

    while (file.good())
    {
        auto columns = ReadCSVRecord(file);
        if (columns.size() < 9 || columns[0].empty()) continue;

        SpellData data   = ParseCSVRow(columns);
        spells_[data.id] = data;
    }
    Engine::GetLogger().LogEvent(
        "SpellSystem: Loaded " + std::to_string(spells_.size()) + " spells");
}
```

> **status_effect.csv 파싱 불필요**: 효과 이름 목록과 실행 로직은 `StatusEffectHandler::KNOWN_EFFECTS`가 소유한다.
> `status_effect.csv`는 설계 참조 문서로만 존재한다.

---

### ParseCSVRow

#### 구형 ParseCSVRow (`/` 구분, `spell_school` 기반)

```cpp
SpellData SpellSystem::ParseCSVRow(const std::vector<std::string>& col) const
{
    SpellData data;
    data.id             = col[0];
    data.spell_name     = col[1];
    data.spell_school   = col[2];
    data.usable_classes = SplitByDelimiter(col[3], '/'); // "Dragon/Wizard" → vector
    data.spell_level    = col[4].empty() ? 0 : std::stoi(col[4]);
    data.target_type    = col[5];
    data.range          = ParseRange(col[6]);
    data.upcastable     = (col[7] == "TRUE" || col[7] == "true" || col[7] == "O");
    data.effect_desc    = col[8];
    return data;
}
```

#### 신형 ParseCSVRow + ParseEffectField (`, ` 구분, 4줄 템플릿 파싱)

```cpp
SpellData SpellSystem::ParseCSVRow(const std::vector<std::string>& col) const
{
    SpellData data;
    data.id             = col[0];
    data.spell_name     = col[1];
    data.category       = col[2];
    data.usable_classes = SplitByDelimiter(col[3], ','); // "Dragon, Fighter" → vector
    data.spell_level    = col[4].empty() ? 0 : std::stoi(col[4]);
    data.target_type    = col[5];
    data.range          = ParseRange(col[6]);             // "4 spaces" → 4
    data.upcastable     = (col[7] == "TRUE");
    data.effect_raw     = col[8];

    ParseEffectField(col[8], data);
    return data;
}

// Effect 4줄 템플릿 파싱:
//   Line 1: Deals {damage} damage.
//   Line 2: Applies "{STATUS}" status for {N} turns.
//   Line 3: Move to {location}.
//   Line 4: Summons {entity} at {location}.
void SpellSystem::ParseEffectField(const std::string& effect_str, SpellData& data) const
{
    std::istringstream ss(effect_str);
    std::string        line;

    // Line 1: "Deals X damage."
    if (std::getline(ss, line))
    {
        // "Deals " 이후 " damage." 이전 문자열 추출
        const std::string prefix = "Deals ";
        const std::string suffix = " damage.";
        auto p = line.find(prefix);
        auto s = line.find(suffix);
        if (p != std::string::npos && s != std::string::npos)
            data.damage_formula = line.substr(p + prefix.size(),
                                              s - (p + prefix.size()));
        else
            data.damage_formula = "0";
    }

    // Line 2: Applies "STATUS" status for N turns.
    if (std::getline(ss, line))
    {
        // 따옴표 사이의 STATUS 이름 추출
        auto q1 = line.find('"');
        auto q2 = line.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos)
            data.effect_status = line.substr(q1 + 1, q2 - q1 - 1);
        else
            data.effect_status = "Basic";

        // "for N turns" 에서 N 추출
        auto ft = line.find("for ");
        auto tt = line.find(" turns", ft);
        if (ft != std::string::npos && tt != std::string::npos)
        {
            std::string n_str = line.substr(ft + 4, tt - (ft + 4));
            data.effect_duration = n_str.empty() ? 0 : std::stoi(n_str);
        }
        else
            data.effect_duration = 0;
    }

    // Line 3: "Move to {location}."
    if (std::getline(ss, line))
    {
        const std::string prefix = "Move to ";
        auto p = line.find(prefix);
        if (p != std::string::npos)
        {
            std::string loc = line.substr(p + prefix.size());
            if (!loc.empty() && loc.back() == '.') loc.pop_back();
            data.move_type = loc; // "current location", "furthest position..."
        }
    }

    // Line 4: "Summons {entity} at {location}."
    if (std::getline(ss, line))
    {
        const std::string prefix = "Summons ";
        auto p = line.find(prefix);
        auto at = line.find(" at ");
        if (p != std::string::npos && at != std::string::npos)
            data.summon_type = line.substr(p + prefix.size(),
                                           at - (p + prefix.size()));
        else
            data.summon_type = "NULL";
    }
}
```

---

### CalculateSpellDamage

#### 구형 CalculateSpellDamage (`base_damage` 필드, `Character*` 파라미터 포함)

```cpp
int SpellSystem::CalculateSpellDamage(const SpellData& spell,
                                       Character* caster, int upcast_level)
{
    if (spell.base_damage.empty())
        return 0;

    auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
    if (!dice)
        return 0;

    int base = dice->RollDiceFromString(spell.base_damage);

    // 업캐스트: 레벨 차이당 1d6 추가 (스펠별로 규칙이 다르면 이 부분을 수정)
    if (upcast_level > spell.spell_level)
    {
        int extra = upcast_level - spell.spell_level;
        base += dice->RollDiceFromString("1d6") * extra;
    }

    return base;
}
```

#### 신형 CalculateSpellDamage (`damage_formula` 필드, `Character*` 파라미터 제거)

```cpp
int SpellSystem::CalculateSpellDamage(const SpellData& spell, int upcast_level)
{
    if (spell.damage_formula == "0" || spell.damage_formula.empty())
        return 0;

    auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
    if (!dice) return 0;

    // 단순 주사위 표기법만 RollDiceFromString으로 처리
    // 업캐스트 수식("(Spell Level - Required Spell Level)d8" 등)은 레벨 차이로 추가 다이스 계산
    int level_diff = std::max(0, upcast_level - spell.spell_level);

    // 기본 damage_formula에서 순수 주사위 부분만 추출 (첫 번째 공백 전까지)
    std::string base_dice = spell.damage_formula.substr(0, spell.damage_formula.find(' '));
    int base = dice->RollDiceFromString(base_dice);

    // 업캐스트 추가 다이스: 수식에서 "Xd8" 패턴을 파싱하거나 고정 1d8로 적용
    if (level_diff > 0 && spell.upcastable)
        base += dice->RollDiceFromString("1d8") * level_diff; // TODO: 스펠별 업캐스트 다이스 파싱

    return base;
}
```

---

### ApplySpellEffect

#### 구형 ApplySpellEffect (`spell_school` 분기)

```cpp
void SpellSystem::ApplySpellEffect(Character* caster, const SpellData& spell,
                                    Math::ivec2 target_tile, int upcast_level)
{
    auto* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    auto* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
    if (!grid || !combat)
        return;

    Character* target = grid->GetCharacterAt(target_tile);

    if (spell.spell_school == "공격" && target)
    {
        int   damage = CalculateSpellDamage(spell, caster, upcast_level);
        combat->ApplyDamage(target, damage);

        auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
        if (bus)
            bus->Publish(CharacterDamagedEvent{ target, damage, caster, "spell" });
    }
    else if (spell.spell_school == "버프" && target)
    {
        // TODO: Week 6+ StatusEffect 시스템 연동 후 구현
        Engine::GetLogger().LogEvent("SpellSystem: Buff stub — " + spell.id);
    }
}
```

#### 신형 ApplySpellEffect (Effect 필드 파싱 결과 분기)

Effect 필드에서 파싱된 데이터로 분기 (`spell_school` 대신 `effect_status`, `damage_formula` 사용):

```cpp
void SpellSystem::ApplySpellEffect(Character* caster, const SpellData& spell,
                                    Math::ivec2 target_tile, int upcast_level)
{
    auto* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    auto* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
    auto* bus    = Engine::GetGameStateManager().GetGSComponent<EventBus>();
    if (!grid || !combat) return;

    Character* target = grid->GetCharacterAt(target_tile);

    // ── 피해 ──
    if (spell.damage_formula != "0" && !spell.damage_formula.empty())
    {
        int damage = CalculateSpellDamage(spell, upcast_level);

        if (damage > 0 && target)
        {
            combat->ApplyDamage(target, damage);
            if (bus)
                bus->Publish(CharacterDamagedEvent{ target, damage,
                                                    target->GetHP(), caster, false });
        }
        else if (damage < 0 && target) // 음수 = 회복 (S_ENH_030 Healing Touch)
        {
            int heal = -damage;
            target->SetHP(std::min(target->GetHP() + heal, target->GetMaxHP()));
        }
    }

    // ── 상태 효과 ──
    if (spell.effect_status != "Basic" && spell.effect_duration > 0)
    {
        auto* handler    = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();
        Character* se_target = target ? target : caster;

        if (spell.target_type == "Enemies Around Caster" || spell.target_type == "All Enemies in Straight Line")
        {
            // 범위 효과: 시전자 주변 spell.range칸 내 모든 적
            for (auto* c : grid->GetAllCharacters())
            {
                if (!c || !c->IsAlive() || c == caster) continue;
                int dist = grid->ManhattanDistance(
                    caster->GetGridPosition()->Get(), c->GetGridPosition()->Get());
                if (dist <= spell.range)
                {
                    c->AddEffect(spell.effect_status, spell.effect_duration);
                    // 즉시 실행 효과 (Purify 등) 위임
                    if (handler) handler->OnApplied(c, spell.effect_status);
                }
            }
        }
        else if (se_target)
        {
            se_target->AddEffect(spell.effect_status, spell.effect_duration);
            // 즉시 실행 효과 (Purify 등) 위임
            if (handler) handler->OnApplied(se_target, spell.effect_status);
        }
    }

    // ── 지형 생성 (summon_type != "NULL") ──
    if (spell.summon_type != "NULL" && !spell.summon_type.empty())
    {
        // TODO: Terrain 시스템 구현 후 연동
        Engine::GetLogger().LogEvent(
            "SpellSystem: Summon " + spell.summon_type + " at ("
            + std::to_string(target_tile.x) + ", " + std::to_string(target_tile.y) + ")");
    }

    // ── 특수 이동 ──
    if (spell.move_type != "current location" && !spell.move_type.empty())
    {
        // TODO: 텔레포트 등 특수 이동 구현 (S_GEO_030)
        Engine::GetLogger().LogEvent("SpellSystem: Special move — " + spell.move_type);
    }
}
```

---

### CastSpell

구형/신형 동일:

```cpp
bool SpellSystem::CastSpell(Character* caster, const std::string& spell_id,
                              Math::ivec2 target_tile, int upcast_level)
{
    if (!caster)
        return false;

    auto it = spells_.find(spell_id);
    if (it == spells_.end())
    {
        Engine::GetLogger().LogError("SpellSystem: Unknown spell id " + spell_id);
        return false;
    }

    const SpellData& spell = it->second;

    if (!CanCast(caster, spell_id, target_tile))
        return false;

    caster->ConsumeSpell(spell.spell_level);
    ApplySpellEffect(caster, spell, target_tile, upcast_level);

    Engine::GetLogger().LogEvent(
        caster->TypeName() + " cast " + spell.spell_name + " [" + spell_id + "]");
    return true;
}
```

---

### PlayerInputHandler 연동

`PlayerInputHandler.h` 추가:

```cpp
// private
std::string m_selected_spell_id;
Math::ivec2 WorldToTile(Math::vec2 world_pos) const;

// public
void SelectSpell(const std::string& spell_id)
{
    m_selected_spell_id = spell_id;
    m_state = ActionState::TargetingForSpell;
}
```

`PlayerInputHandler.cpp` 추가:

```cpp
Math::ivec2 PlayerInputHandler::WorldToTile(Math::vec2 world_pos) const
{
    int tile_x = static_cast<int>(world_pos.x / GridSystem::TILE_SIZE);
    int tile_y = static_cast<int>(world_pos.y / GridSystem::TILE_SIZE);
    return { tile_x, tile_y };
}

// HandleMouseClick의 TargetingForSpell case
case ActionState::TargetingForSpell:
{
    Math::ivec2  clicked_tile = WorldToTile(mouse_pos);
    SpellSystem* spell_sys    =
        Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    if (spell_sys && spell_sys->CanCast(dragon, m_selected_spell_id, clicked_tile))
    {
        spell_sys->CastSpell(dragon, m_selected_spell_id, clicked_tile);
        m_state = ActionState::None;
    }
    break;
}
```

---

### AI Strategy에서 스펠 결정 반환

```cpp
// 예: WizardStrategy::MakeDecision()
if (actor->HasAnySpellSlot() && IsInRange(actor, target))
{
    AIDecision decision;
    decision.type        = AIDecisionType::UseAbility;
    decision.abilityName = "S_ATK_010"; // spell_table.csv의 ID
    decision.target      = target;
    decision.reasoning   = "Wizard: spell attack in range";
    return decision;
}
```

---

## Rigorous Testing

테스트 파일: `DragonicTactics/source/Game/DragonicTactics/Test/TestSpellSystem.h/cpp` 확장

### 테스트 1: CSV 파싱 — 기본 필드 검증 (구형 기준)

```cpp
SpellSystem* ss = GetGSComponent<SpellSystem>();
ss->LoadFromCSV("Assets/Data/spell_table.csv");

const SpellData* fireball = ss->GetSpellData("S_ATK_010");
ASSERT(fireball != nullptr,          "S_ATK_010 must exist");
ASSERT(fireball->upcastable == true, "Fire Bolt must be upcastable");
ASSERT(fireball->spell_level == 1,   "Fire Bolt requires level 1 slot");
ASSERT(fireball->range > 0,          "Fire Bolt must have positive range");
```

### 테스트 2: CSV 파싱 — Effect 템플릿 파싱 검증 (신형)

```cpp
SpellSystem* ss = GetGSComponent<SpellSystem>();
ss->LoadFromCSV("Assets/Data/spell_table.csv");

// S_ATK_010 Fire Bolt: damage_formula="2d8", effect_status="Basic", range=4
const SpellData* fb = ss->GetSpellData("S_ATK_010");
ASSERT(fb != nullptr,                    "S_ATK_010 must exist");
ASSERT(fb->upcastable == true,           "Fire Bolt is upcastable");
ASSERT(fb->spell_level == 1,             "Fire Bolt requires level 1 slot");
ASSERT(fb->range == 4,                   "Fire Bolt range = 4");
ASSERT(fb->effect_status == "Basic",     "Fire Bolt has no status effect");
ASSERT(fb->damage_formula.find("2d8") != std::string::npos, "damage formula contains 2d8");

// S_DEB_020 Fearful Cry: effect_status="Fear", duration=3, range=3
const SpellData* fear = ss->GetSpellData("S_DEB_020");
ASSERT(fear->effect_status == "Fear",    "Fearful Cry applies Fear");
ASSERT(fear->effect_duration == 3,       "Fear lasts 3 turns");
ASSERT(fear->range == 3,                 "Fear range = 3");

// S_ENH_010 Bloodlust: effect_status="Lifesteal"
// ⚠️ CSV가 "for 1 turn"(단수)을 사용 → ParseEffectField가 " turns"를 못 찾으면 duration=0
// ParseEffectField가 "turn" 단수도 처리하도록 수정된 경우에만 아래 ASSERT 통과
const SpellData* bl = ss->GetSpellData("S_ENH_010");
ASSERT(bl->effect_status == "Lifesteal", "Bloodlust applies Lifesteal");
ASSERT(bl->effect_duration == 1,         "Lifesteal lasts 1 turn (requires 'turn' singular fix)");
ASSERT(bl->damage_formula == "0",        "Bloodlust deals 0 damage");

// S_ENH_020 Frenzy: effect_status="Frenzy", duration=1, level=0 (슬롯 불필요)
const SpellData* frenzy = ss->GetSpellData("S_ENH_020");
ASSERT(frenzy != nullptr,                  "S_ENH_020 must exist");
ASSERT(frenzy->effect_status == "Frenzy",  "Frenzy spell applies Frenzy");
ASSERT(frenzy->effect_duration == 1,       "Frenzy lasts 1 turn");
ASSERT(frenzy->damage_formula == "0",      "Frenzy deals 0 damage");
ASSERT(frenzy->spell_level == 0,           "Frenzy requires no spell slot");
```

### 테스트 3: 슬롯 소모 검증

```cpp
int before = player->GetSpellSlotCount(1);
ss->CastSpell(player, "S_ATK_010", enemy_tile, 0);
int after = player->GetSpellSlotCount(1);
ASSERT(after == before - 1, "Spell slot must be consumed after cast");
```

### 테스트 5: 슬롯 없을 때 시전 불가

```cpp
while (player->HasSpellSlot(1))
    ss->CastSpell(player, "S_ATK_010", enemy_tile, 0);

bool can_cast = ss->CanCast(player, "S_ATK_010", enemy_tile);
ASSERT(can_cast == false, "Cannot cast without spell slots");
```

### 테스트 6: AI UseAbility → CastSpell 연동

```cpp
AIDecision decision;
decision.type        = AIDecisionType::UseAbility;
decision.abilityName = "S_ATK_010"; // Fire Bolt
decision.target      = enemy;

int hp_before = enemy->GetHP();
ai_system->ExecuteDecision(wizard, decision);
ASSERT(enemy->GetHP() < hp_before, "AI spell must deal damage");
```

### 테스트 7: 상태 효과 적용 — Fear (S_DEB_020)

```cpp
// Fearful Cry: 시전자 주변 3칸 적에게 Fear 3턴 적용
ss->CastSpell(fighter, "S_DEB_020", dragon_tile, 0);
ASSERT(dragon->Has("Fear"), "Dragon must have Fear after Fearful Cry");
```

### 게임 내 수동 테스트 (키보드)

`GamePlay.cpp`에 테스트 키 추가:

```cpp
// Q 키: 현재 캐릭터의 사용 가능한 스펠 목록 로그
if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Q))
{
    auto* ss = GetGSComponent<SpellSystem>();
    for (const auto& id : ss->GetAvailableSpells(current))
        Engine::GetLogger().LogEvent("Available: " + id);
}
```

---

## 스펠 선택 UI 구현 (Dragon 턴)

Dragon이 자신이 사용 가능한 스펠 목록을 보고 선택할 수 있도록 하는 구현 가이드.

### 현재 문제 목록

| 파일                     | 문제                                                              |
| ---------------------- | --------------------------------------------------------------- |
| `GamePlay.cpp`         | `AddGSComponent(new SpellSystem())` 없음 → 스펠 시스템 미등록             |
| `GamePlay.cpp`         | `LoadFromCSV()` 호출 없음 → 스펠 데이터 비어 있음                            |
| `GamePlay.cpp`         | "Spell" 버튼이 바로 `TargetingForSpell`로 전환 → 어떤 스펠인지 선택 불가          |
| `PlayerInputHandler.h` | `SelectingSpell` 상태 없음                                          |
| `SpellSystem.cpp`      | `GetAvailableSpells`가 `ivec2(0,0)`을 타겟으로 전달 → 사거리 체크에서 모든 스펠 실패 |

---

### 수정 1: SpellSystem.cpp — `GetAvailableSpells` 버그 수정

`GetAvailableSpells`는 타겟 위치 없이 "시전 가능 여부"만 판단해야 한다.
현재 `CanCast(caster, id, ivec2(0,0))`을 호출하면 시전자 위치에서 `(0,0)`까지 거리를 계산해
사거리 초과로 대부분의 스펠이 걸러진다.

**수정**: 타겟 위치에 시전자 자신의 위치를 전달 → 거리 = 0 → 사거리 체크 항상 통과

```cpp
// SpellSystem.cpp — GetAvailableSpells
std::vector<std::string> SpellSystem::GetAvailableSpells(Character* caster) const
{
    std::vector<std::string> available_spells;
    for (const auto& pair : spells_)
    {
        // 타겟 미확정이므로 시전자 위치를 사용 → 거리=0, 사거리 체크 항상 통과
        if (CanCast(caster, pair.first, caster->GetGridPosition()->Get()))
        {
            available_spells.push_back(pair.first);
        }
    }
    return available_spells;
}
```

---

### 수정 2: PlayerInputHandler.h — `SelectingSpell` 상태 추가

```cpp
enum class ActionState
{
    None,
    SelectingMove,
    Moving,
    SelectingAction,
    SelectingSpell,       // ← 신규: 스펠 목록에서 선택 중
    TargetingForAttack,
    TargetingForSpell
};
```

---

### 수정 3: PlayerInputHandler.cpp — switch에 케이스 추가

`HandleMouseClick`의 switch에 `SelectingSpell` 케이스 추가 (클릭 무시):

```cpp
case ActionState::SelectingSpell:
    // 스펠 목록 선택 중에는 타일 클릭 무시
    break;
```

---

### 수정 4: GamePlay.cpp — SpellSystem 등록 및 CSV 로드

`GamePlay::Load()`의 `AddGSComponent` 목록에 추가:

```cpp
AddGSComponent(new SpellSystem());
AddGSComponent(new StatusEffectHandler());  // 기존 — 위치 참고용
```

설정 코드 추가 (기존 `SetDiceManager` 라인 근처):

```cpp
GetGSComponent<SpellSystem>()->LoadFromCSV("Assets/Data/spell_table.csv");
GetGSComponent<SpellSystem>()->SetEventBus(GetGSComponent<EventBus>());
```

> 기존 주석 처리된 `// GetGSComponent<SpellSystem>()->SetEventBus(...)` 줄을 교체.

---

### 수정 5: GamePlay.cpp — DrawImGui() 수정

#### 5-1. Action 버튼: SelectingSpell 상태도 취소 가능하게

```cpp
// 수정 전
const char* action_text     = (currentState == ActionState::SelectingAction) ? "Cancel Action" : "Action";
bool  is_action_disabled    = (currentState != ActionState::None && currentState != ActionState::SelectingAction);

// 수정 후
const char* action_text     = (currentState == ActionState::SelectingAction ||
                                currentState == ActionState::SelectingSpell) ? "Cancel Action" : "Action";
bool  is_action_disabled    = (currentState != ActionState::None &&
                                currentState != ActionState::SelectingAction &&
                                currentState != ActionState::SelectingSpell);
```

#### 5-2. Action 버튼 클릭 핸들러: SelectingSpell도 취소 처리

```cpp
if (ImGui::Button(action_text))
{
    if (currentState == ActionState::SelectingAction ||
        currentState == ActionState::SelectingSpell)      // ← 추가
    {
        m_input_handler->CancelCurrentAction();
        Engine::GetLogger().LogEvent("UI: 'Cancel Action' button clicked.");
    }
    else
    {
        m_input_handler->SetState(ActionState::SelectingAction);
        Engine::GetLogger().LogEvent("UI: 'Action' button clicked.");
    }
}
```

#### 5-3. Action List 패널: "Spell" 버튼 → SelectingSpell으로 전환

```cpp
if (currentState == ActionState::SelectingAction)
{
    ImGui::Begin("Action List");

    if (ImGui::Button("Attack"))
    {
        Engine::GetLogger().LogEvent("UI: 'Attack' selected. Now targeting.");
        m_input_handler->SetState(ActionState::TargetingForAttack);
    }

    if (ImGui::Button("Spell"))
    {
        Engine::GetLogger().LogEvent("UI: 'Spell' selected. Showing spell list.");
        m_input_handler->SetState(ActionState::SelectingSpell);  // ← TargetingForSpell → SelectingSpell
    }

    ImGui::End();
}
```

#### 5-4. Spell List 패널 신규 추가 (Action List 패널 바로 뒤)

```cpp
if (currentState == ActionState::SelectingSpell)
{
    ImGui::Begin("Spell List");

    SpellSystem* spell_sys = GetGSComponent<SpellSystem>();
    Character*   current   = turnMgr ? turnMgr->GetCurrentCharacter() : nullptr;

    if (spell_sys && current)
    {
        auto available = spell_sys->GetAvailableSpells(current);

        if (available.empty())
        {
            ImGui::Text("No spells available.");
        }

        for (const auto& spell_id : available)
        {
            const SpellData* spell = spell_sys->GetSpellData(spell_id);
            if (!spell) continue;

            // 버튼 레이블: "Fire Bolt (Lv.1)"
            // ##spell_id 로 ImGui 내부 ID 충돌 방지
            std::string label = spell->spell_name
                              + " (Lv." + std::to_string(spell->spell_level) + ")"
                              + "##" + spell_id;

            if (ImGui::Button(label.c_str()))
            {
                Engine::GetLogger().LogEvent("UI: Spell '" + spell->spell_name + "' selected. Now targeting.");
                m_input_handler->SelectSpell(spell_id);  // → TargetingForSpell로 전환
            }
        }
    }

    if (ImGui::Button("Cancel"))
    {
        m_input_handler->CancelCurrentAction();
    }

    ImGui::End();
}
```

> `SpellData`는 `SpellSystem.h`를 include하면 사용 가능. `GamePlay.cpp`에는 이미 include되어 있음.

---

### 구현 순서

```
1. SpellSystem.cpp — GetAvailableSpells 한 줄 수정
2. PlayerInputHandler.h — SelectingSpell 추가
3. PlayerInputHandler.cpp — switch case 추가
4. GamePlay.cpp — AddGSComponent(new SpellSystem()) + LoadFromCSV + SetEventBus
5. GamePlay.cpp — DrawImGui 수정 (5-1 ~ 5-4)
6. cmake --preset windows-debug 재실행 (SpellSystem이 새로 링크됨)
7. 빌드 후 Dragon 턴에서 Action → Spell 클릭 → 스펠 목록 확인
```

---

## 캐릭터 스탯 패널에 스펠 슬롯 / 상태 효과 표시

`GamePlayUIManager::DrawCharacterStatsPanel()`에 스펠 레벨별 슬롯 잔여량과 현재 활성 상태 효과를 추가 표시한다.

### 사전 작업: 필요한 getter 추가

#### A. SpellSlots — 최대 슬롯 수 반환

현재 `GetSpellSlotCount(level)`는 현재 잔여 슬롯만 반환한다. `현재/최대` 형식으로 표시하려면 최대 슬롯도 필요하다.

**`SpellSlots.h` 추가**:

```cpp
int GetMaxSlotCount(int level) const;
const std::map<int, int>& GetMaxSlots() const { return max_slots; }
```

**`SpellSlots.cpp` 추가**:

```cpp
int SpellSlots::GetMaxSlotCount(int level) const
{
    auto it = max_slots.find(level);
    return (it != max_slots.end()) ? it->second : 0;
}
```

#### B. StatusEffectComponent — 전체 활성 효과 목록 반환

현재 `Has(name)` / `GetMagnitude(name)`만 있고, 목록 전체를 순회할 방법이 없다.

**`StatusEffectComponent.h` 추가**:

```cpp
// --- 전체 효과 목록 (UI 표시용) ---
const std::vector<ActiveEffect>& GetAllEffects() const { return effects_; }
```

#### C. Character — facade 추가

**`Character.h` 추가**:

```cpp
const std::vector<ActiveEffect>& GetActiveEffects() const;
```

**`Character.cpp` 추가**:

```cpp
const std::vector<ActiveEffect>& Character::GetActiveEffects() const
{
    static const std::vector<ActiveEffect> empty;
    const auto* se = GetGOComponent<StatusEffectComponent>();
    return se ? se->GetAllEffects() : empty;
}
```

---

### DrawCharacterStatsPanel 수정

기존 코드에서 Speed 다음에 두 블록을 추가한다.

**추가 위치**: 각 캐릭터 루프 안, Speed 텍스트 드로우 직후 / `current_y` 이동 직전

```cpp
// (기존) Speed 텍스트 드로우 ...
Engine::GetTextManager().DrawText(speed_text, ..., CS200::GREEN);

// ── 신규 블록 1: 스펠 슬롯 ──
SpellSlots* slots = character->GetSpellSlots();
if (slots)
{
    std::string slot_text = "Slots:";
    for (int lv = 1; lv <= 5; ++lv)
    {
        int max_count = slots->GetMaxSlotCount(lv);
        if (max_count == 0) continue;              // 이 레벨 슬롯 없으면 스킵

        int cur_count = slots->GetSpellSlotCount(lv);
        slot_text += " Lv" + std::to_string(lv)
                   + ":" + std::to_string(cur_count)
                   + "/" + std::to_string(max_count);
    }
    Engine::GetTextManager().DrawText(
        slot_text,
        Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 4.0) },
        Fonts::Outlined, text_scale, CS200::CYAN);
}

// ── 신규 블록 2: 활성 상태 효과 ──
const auto& effects = character->GetActiveEffects();
if (!effects.empty())
{
    std::string fx_text = "FX:";
    for (const auto& e : effects)
        fx_text += " " + e.name + "(" + std::to_string(e.duration) + ")";

    Engine::GetTextManager().DrawText(
        fx_text,
        Math::vec2{ text_x_pos, current_y + panel_height_per_char - (first_line_y + line_height * 5.0) },
        Fonts::Outlined, text_scale, CS200::YELLOW);
}

// (기존) current_y -= panel_height_per_char + 40.0;
```

> **패널 높이 조정 필요**: 두 줄이 추가되므로 `panel_height_per_char`를 기존 `90.0`에서 `150.0` 내외로 늘려야 글자가 잘리지 않는다.

---

### 표시 결과 예시

```
Dragon
HP: 210 / 250
AP: 2
Speed: 4
Slots: Lv1:3/4  Lv2:2/3  Lv3:1/2  Lv4:2/2  Lv5:1/1
FX: Fear(2) Blessing(1)

Fighter
HP: 180 / 200
AP: 1
Speed: 3
Slots: Lv1:4/4  Lv2:3/3
FX:
```

---

### 구현 순서 (스탯 패널)

```
1. SpellSlots.h/cpp — GetMaxSlotCount() 추가
2. StatusEffectComponent.h — GetAllEffects() 추가 (1줄 inline)
3. Character.h/cpp — GetActiveEffects() facade 추가
4. GamePlayUIManager.cpp — DrawCharacterStatsPanel에 두 블록 추가
5. panel_height_per_char 값 조정 (90 → 150)
```

---

## 스펠 시전 시 AP 1 소모

스펠은 일반 공격과 동일하게 AP 1을 소모해야 한다. 모든 캐릭터에 동일하게 적용.

**관련 파일**: `StateComponents/SpellSystem.cpp`

이미 존재하는 API:

- `Character::GetActionPoints()` → 현재 AP 조회
- `Character::GetActionPointsComponent()->Consume(1)` → AP 1 소모 (`ActionPoints::Consume` 구현됨)

---

### 수정 1: CanCast — AP 체크 추가

AP가 없으면 스펠 목록에 아예 뜨지 않아야 한다.
`GetAvailableSpells`는 `CanCast`를 통해 필터링하므로 이 한 곳만 수정하면 된다.

```cpp
// SpellSystem.cpp — CanCast 함수 끝 부분
// 기존: 클래스 체크 → 슬롯 체크 → 사거리 체크 → return true

// 사거리 체크 이후, return true 바로 앞에 추가
if (caster->GetActionPoints() < 1)
    return false;

return true;
```

---

### 수정 2: CastSpell — AP 소모 추가

슬롯 소모(`ConsumeSpell`) 직후에 AP를 차감한다.

```cpp
// SpellSystem.cpp — CastSpell 함수

caster->ConsumeSpell(spell.spell_level);  // 슬롯 소모 (기존)

// AP 소모 (신규) — CanCast에서 이미 >= 1 보장됨
caster->GetActionPointsComponent()->Consume(1);

ApplySpellEffect(caster, spell, target_tile, upcast_level);  // 기존
```

---

### 구현 순서

```
1. SpellSystem.cpp — CanCast의 return true 바로 앞에 AP 체크 추가
2. SpellSystem.cpp — CastSpell의 ConsumeSpell 직후에 Consume(1) 추가
```

### 검증

- Dragon 턴 AP 2: 스펠 사용 후 AP 1 감소 → 스탯 패널에서 확인
- Dragon 턴 AP 0: Action → Spell → Spell List 패널이 빈 목록 표시
- 슬롯은 있으나 AP 없을 때: CanCast = false 확인

---

## 스펠 타겟팅 가능 위치 빨간색 표시

### 목표

플레이어가 스펠을 선택해 `TargetingForSpell` 상태가 되면,
해당 스펠의 사거리 내 타일을 **빨간색 반투명 오버레이**로 표시한다.
이동 모드의 초록색 시각화(`movement_mode_active_` / `reachable_tiles_`)를 그대로 모방한다.

### 구현 파일

- `GridSystem.h` — 필드·메서드 선언 추가
- `GridSystem.cpp` — 메서드 구현, `Draw()`, `Update()` 수정
- `PlayerInputHandler.h` — `SelectSpell` 시그니처 변경
- `PlayerInputHandler.cpp` — 타일 모드 시작/종료 호출 추가
- `GamePlayUIManager.cpp` — 버튼 클릭 시 caster 전달

---

### 수정 1: GridSystem.h — 필드 및 메서드 추가

기존 `movement_mode_active_` 필드 블록 바로 아래에 추가:

```cpp
// ─ 스펠 타겟팅 시각화 ─
bool                    spell_targeting_mode_active_ = false;
std::set<Math::ivec2>   spell_targetable_tiles_;
```

`public` 메서드 선언 추가 (`EnableMovementMode` / `DisableMovementMode` 바로 아래):

```cpp
/// @brief 스펠 타겟팅 가능 타일 계산 (맨해튼 거리 기반)
/// @param center  시전자 위치
/// @param range   스펠 사거리
void EnableSpellTargetingMode(Math::ivec2 center, int range);

/// @brief 스펠 타겟팅 모드 해제 (시각화 데이터 초기화)
void DisableSpellTargetingMode();
```

---

### 수정 2: GridSystem.cpp — 메서드 구현

```cpp
void GridSystem::EnableSpellTargetingMode(Math::ivec2 center, int range)
{
    spell_targeting_mode_active_ = true;
    spell_targetable_tiles_.clear();

    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            Math::ivec2 tile{ x, y };
            if (IsValidTile(tile) && ManhattanDistance(center, tile) <= range)
                spell_targetable_tiles_.insert(tile);
        }
    }
}

void GridSystem::DisableSpellTargetingMode()
{
    spell_targeting_mode_active_ = false;
    spell_targetable_tiles_.clear();
}
```

> 이동 가능 타일은 BFS(벽 우회)로 계산하지만,
> 스펠 사거리는 벽을 무시한 맨해튼 거리로 계산한다.

---

### 수정 3: GridSystem.cpp — Draw() 빨간색 타일 렌더링

이동 타일 시각화 블록(`// 2. 이동 가능 타일 시각화`) 바로 뒤에 추가:

```cpp
// ========================================
// 3. 스펠 타겟팅 가능 타일 시각화 (빨간색)
// ========================================
if (spell_targeting_mode_active_)
{
    int alpha = static_cast<int>(80 + 40 * std::sin(pulse_timer_ * 3.0));
    for (const auto& tile : spell_targetable_tiles_)
    {
        int screen_x = tile.x * TILE_SIZE + TILE_SIZE;
        int screen_y = tile.y * TILE_SIZE + TILE_SIZE;
        renderer_2d->DrawRectangle(
            Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE),
            CS200::pack_color({ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f, alpha / 255.0f }), // 빨간색
            0U
        );
    }
}
```

> 알파값은 이동 타일과 동일한 `80 + 40*sin(t)` 공식을 사용해 부드럽게 깜빡인다.

---

### 수정 4: GridSystem.cpp — Update() pulse_timer_ 범위 확장

기존 `if (movement_mode_active_)` 조건에 `|| spell_targeting_mode_active_` 추가:

```cpp
void GridSystem::Update(double dt)
{
    if (movement_mode_active_ || spell_targeting_mode_active_)
    {
        pulse_timer_ += dt;
    }
    else
    {
        pulse_timer_ = 0.0;
    }
}
```

---

### 수정 5: PlayerInputHandler.h — SelectSpell 인라인 → 선언으로

```cpp
// 변경 전 (인라인)
void SelectSpell(const std::string& spell_id)
{
    m_selected_spell_id = spell_id;
    m_state = ActionState::TargetingForSpell;
}

// 변경 후 (선언만)
void SelectSpell(const std::string& spell_id, Character* caster);
```

---

### 수정 6: PlayerInputHandler.cpp — SelectSpell 구현

```cpp
void PlayerInputHandler::SelectSpell(const std::string& spell_id, Character* caster)
{
    m_selected_spell_id = spell_id;
    m_state = ActionState::TargetingForSpell;

    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto* grid      = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (spell_sys && grid && caster)
    {
        const SpellData* spell = spell_sys->GetSpellData(spell_id);
        if (spell)
            grid->EnableSpellTargetingMode(caster->GetGridPosition()->Get(), spell->range);
    }
}
```

---

### 수정 7: PlayerInputHandler.cpp — TargetingForSpell 시전 성공 시 모드 해제

`HandleMouseClick`의 `TargetingForSpell:` case — 시전 성공 시 `DisableSpellTargetingMode` 추가:

```cpp
case ActionState::TargetingForSpell:
{
    Math::ivec2  clicked_tile = ConvertScreenToGrid(mouse_pos);
    SpellSystem* spell_sys    = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto*        grid         = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    if (spell_sys && spell_sys->CanCast(dragon, m_selected_spell_id, clicked_tile))
    {
        spell_sys->CastSpell(dragon, m_selected_spell_id, clicked_tile);
        m_state = ActionState::None;
        if (grid) grid->DisableSpellTargetingMode();  // ← 추가
    }
    else
    {
        Engine::GetLogger().LogDebug("Cannot cast " + m_selected_spell_id + " at ...");
        // 범위 밖 클릭은 타겟팅 모드 유지 — 다른 타일 재시도 가능
    }
}
break;
```

---

### 수정 8: PlayerInputHandler.cpp — 우클릭 취소 시 모드 해제

`HandleRightClick` 안, 이동 모드 취소 블록 아래에 추가:

```cpp
if (m_state == ActionState::TargetingForSpell)
{
    auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (grid) grid->DisableSpellTargetingMode();
}
```

---

### 수정 9: GamePlayUIManager.cpp — Spell List 버튼에 caster 전달

DrawImGui Spell List 패널의 스펠 버튼 클릭 핸들러:

```cpp
// 변경 전
if (ImGui::Button(spell_button_label.c_str()))
    input_handler->SelectSpell(spell_id);

// 변경 후
if (ImGui::Button(spell_button_label.c_str()))
    input_handler->SelectSpell(spell_id, current_character);
```

`current_character`는 Spell List 패널 위쪽에서 이미 사용 중인 포인터를 그대로 전달한다.

---

### 구현 순서

```
1. GridSystem.h     — spell_targeting_mode_active_, spell_targetable_tiles_ 필드 추가
2. GridSystem.h     — EnableSpellTargetingMode / DisableSpellTargetingMode 선언 추가
3. GridSystem.cpp   — 두 메서드 구현
4. GridSystem.cpp   — Draw(): 빨간색 타일 블록 추가
5. GridSystem.cpp   — Update(): || spell_targeting_mode_active_ 조건 추가
6. PlayerInputHandler.h  — SelectSpell 인라인 제거 → 선언만
7. PlayerInputHandler.cpp — SelectSpell 구현 (EnableSpellTargetingMode 호출)
8. PlayerInputHandler.cpp — HandleMouseClick TargetingForSpell: DisableSpellTargetingMode 추가
9. PlayerInputHandler.cpp — HandleRightClick: DisableSpellTargetingMode 추가
10. GamePlayUIManager.cpp — SelectSpell(spell_id) → SelectSpell(spell_id, current_character)
```

### 검증

- Dragon이 스펠 선택 → 빨간 타일이 사거리 내 모든 타일에 깜빡이며 표시
- 빨간 타일 클릭 → 스펠 시전 + 빨간 타일 사라짐, 상태 None 복귀
- 범위 밖 클릭 → 빨간 타일 유지 (로그에 "Cannot cast" 출력, 재시도 가능)
- 우클릭 취소 → 빨간 타일 사라짐, 상태 None 복귀
- 이동 모드(초록 타일) 중에는 빨간 타일 미표시

> **주의**: 이 섹션은 아래 "Targeting 템플릿 시스템" 구현 이후 `EnableSpellTargetingMode` 시그니처가 바뀜.
> 수정 6~7을 해당 섹션의 수정 내용으로 대체할 것.

---

## Targeting 템플릿 시스템

### 문제

현재 CSV의 Target 컬럼과 Range 컬럼은 각각 자유 텍스트/숫자로 분리되어 있어 다음 문제가 생긴다:

| 스펠            | Target (현재)                  | Range (현재) | 문제                             |
| ------------- | ---------------------------- | ---------- | ------------------------------ |
| Meteor        | All Odd or Even Tiles        | `-`        | ParseRange → 0 → CanCast 항상 실패 |
| Dragon's Fury | All Enemies in Straight Line | 4          | 직선 AOE 기하학 정보 소실               |
| Tail Swipe    | Enemies Around Caster        | 2          | 원형 AOE인지 단일 타겟인지 코드가 모름        |

근본 원인: Range에 `-`가 들어오면 `ParseRange` → 0 → `ManhattanDistance(caster, target) > 0` → 모든 범위 외 클릭 실패.

---

### 해결: Targeting 템플릿 컬럼

Target + Range 두 컬럼을 **하나의 Targeting 컬럼**으로 통합한다.
포맷: `{Filter}:{Geometry}:{Range}`

| 항목       | 허용 값                                                   |
| -------- | ------------------------------------------------------ |
| Filter   | `Enemy` \| `Ally` \| `Self` \| `Any` \| `Empty`        |
| Geometry | `Single` \| `Around` \| `Line` \| `OddEven` \| `Point` |
| Range    | 정수 (타일 수) \| `-1` (무한)                                 |

**필드 의미**:

- **Filter**: 어떤 대상을 타겟으로 삼는가 (`Enemy`=적, `Ally`=아군, `Self`=자신, `Any`=전체, `Empty`=빈 타일)
- **Geometry**: 공간 형태 (`Single`=단일 선택, `Around`=원형 AOE, `Line`=4방향 직선, `OddEven`=바둑판 패턴, `Point`=지형 대상 단일 선택)
- **Range**: 사거리 (`-1`=무한, `0`=자신)

---

### 새 CSV 컬럼 구조

```
ID, Name, Category, Classes, SpellLevel, Targeting, Upcastable, Effect
 0    1       2        3          4           5           6         7
```

> Range 컬럼 제거 → 총 8컬럼 (기존 9컬럼). `columns.size() < 8` 체크로 변경.

---

### 전체 스펠 Targeting 값 재정의

| ID        | 스펠                 | 기존 Target / Range                | 새 Targeting       |
| --------- | ------------------ | -------------------------------- | ----------------- |
| S_ATK_010 | Fire Bolt          | Single Enemy / 4                 | `Enemy:Single:4`  |
| S_ATK_020 | Tail Swipe         | Enemies Around Caster / 2        | `Enemy:Around:2`  |
| S_ATK_030 | Dragon's Fury      | All Enemies in Straight Line / 4 | `Enemy:Line:4`    |
| S_ATK_040 | Meteor             | All Odd or Even Tiles / -        | `Any:OddEven:-1`  |
| S_ATK_050 | Smite              | Single Enemy / 1                 | `Enemy:Single:1`  |
| S_ATK_060 | Magic Missile      | Single Enemy / -                 | `Enemy:Single:-1` |
| S_ATK_070 | Weakpoint Strike   | Single Enemy / 1                 | `Enemy:Single:1`  |
| S_BUF_010 | Divine Shield      | Single Ally / 4                  | `Ally:Single:4`   |
| S_BUF_020 | Gale Step          | Self / -                         | `Self:Single:0`   |
| S_DEB_010 | Curse of Suffering | Single Enemy / 5                 | `Enemy:Single:5`  |
| S_DEB_020 | Fearful Cry        | Enemies Around Caster / 3        | `Enemy:Around:3`  |
| S_ENH_010 | Bloodlust          | Self / -                         | `Self:Single:0`   |
| S_ENH_020 | Frenzy             | Self / -                         | `Self:Single:0`   |
| S_ENH_030 | Healing Touch      | Single Ally / 5                  | `Ally:Single:5`   |
| S_ENH_040 | Mana Conversion    | Self / -                         | `Self:Single:0`   |
| S_ENH_050 | Purify             | All Units Around Caster / 4      | `Any:Around:4`    |
| S_ENH_060 | Shadow Hide        | Self / -                         | `Self:Single:0`   |
| S_GEO_010 | Magma Blast        | Empty Tile (Point) / 6           | `Empty:Point:6`   |
| S_GEO_020 | Wall Creation      | Empty Tile (Point) / 5           | `Empty:Point:5`   |
| S_GEO_030 | Teleport           | Empty Tile (Point) / 1           | `Empty:Point:1`   |

---

### 수정 1: SpellSystem.h — SpellTargeting 구조체 추가, SpellData 수정

```c
// SpellData 위에 추가
struct SpellTargeting
{
    std::string filter;   // "Enemy", "Ally", "Self", "Any", "Empty"
    std::string geometry; // "Single", "Around", "Line", "OddEven", "Point"
    int         range;    // 타일 수. -1 = 무한
};

struct SpellData
{
    std::string id;
    std::string spell_name;
    std::string category;
    std::vector<std::string> usable_classes;
    int             spell_level;
    SpellTargeting  targeting;   // ← target_type + range 를 대체
    bool            upcastable;

    // Effect 파싱 결과
    std::string damage_formula;
    std::string effect_status;
    int         effect_duration;
    std::string move_type;
    std::string summon_type;
    std::string effect_raw;
};
```

SpellSystem 클래스에 헬퍼 선언 추가:

```cpp
private:
    // 기존 헬퍼 유지
    SpellTargeting ParseTargeting(const std::string& targeting_str) const; // ← 신규
    // ParseRange 제거 (더 이상 사용 안 함)
```

---

### 수정 2: SpellSystem.cpp — ParseTargeting 구현

```cpp
SpellTargeting SpellSystem::ParseTargeting(const std::string& targeting_str) const
{
    // "Enemy:Single:4"  →  filter=Enemy, geometry=Single, range=4
    // "Any:OddEven:-1"  →  filter=Any,   geometry=OddEven, range=-1
    auto parts = SplitByDelimiter(targeting_str, ':');

    SpellTargeting t;
    t.filter   = (parts.size() > 0) ? parts[0] : "Any";
    t.geometry = (parts.size() > 1) ? parts[1] : "Single";
    if (parts.size() > 2 && !parts[2].empty())
        t.range = std::stoi(parts[2]);
    else
        t.range = -1; // 파싱 실패 → 무한으로 처리
    return t;
}
```

---

### 수정 3: SpellSystem.cpp — ParseCSVRow 수정

```cpp
SpellData SpellSystem::ParseCSVRow(const std::vector<std::string>& col) const
{
    // 컬럼 수 체크: 8 → (기존 9에서 변경)
    SpellData data;
    data.id            = col[0];
    data.spell_name    = col[1];
    data.category      = col[2];
    data.usable_classes = SplitByDelimiter(col[3], ',');
    data.spell_level   = col[4].empty() ? 0 : std::stoi(col[4]);
    data.targeting     = ParseTargeting(col[5]);  // ← 신규
    data.upcastable    = (col[6] == "TRUE");       // col[7] → col[6]
    data.effect_raw    = col[7];                   // col[8] → col[7]

    ParseEffectField(col[7], data);
    return data;
}
```

`LoadFromCSV` 의 크기 체크도 변경:

```cpp
// 기존
if (columns.size() < 9 || columns[0].empty())

// 변경
if (columns.size() < 8 || columns[0].empty())
```

---

### 수정 4: SpellSystem.cpp — CanCast 사거리 체크 수정

```cpp
bool SpellSystem::CanCast(Character* caster, const std::string& spell_id, Math::ivec2 target_tile) const
{
    // ... 기존 클래스 체크, 슬롯 체크 ...

    const SpellTargeting& t = spell.targeting;

    // Self / OddEven 은 사거리 무관 (항상 허용)
    bool range_ok = (t.geometry == "Self" || t.geometry == "OddEven" || t.range < 0);
    if (!range_ok)
    {
        auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
        if (!grid) return false;
        int dist = grid->ManhattanDistance(caster->GetGridPosition()->Get(), target_tile);
        if (dist > t.range) return false;
    }

    // AP 체크 (기존)
    if (caster->GetActionPoints() < 1)
        return false;

    return true;
}
```

> 기존 `if (!grid) return false;` + `int dist = ...` 블록을 위 코드로 교체한다.

---

### 수정 5: SpellSystem.cpp — ApplySpellEffect 기하학 처리

```cpp
void SpellSystem::ApplySpellEffect(Character* caster, const SpellData& spell,
                                    Math::ivec2 target_tile, int upcast_level)
{
    auto* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    auto* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
    if (!grid || !combat) return;

    // ─────────────────────────────────────────────────
    // 피해 대상 목록 결정 (Geometry 기반)
    // ─────────────────────────────────────────────────
    std::vector<Character*> targets;
    const SpellTargeting& t = spell.targeting;

    if (t.geometry == "Self")
    {
        targets.push_back(caster);
    }
    else if (t.geometry == "Single" || t.geometry == "Point")
    {
        // 단일 타일 선택 — 기존 동작
        Character* hit = grid->GetCharacterAt(target_tile);
        if (hit) targets.push_back(hit);
    }
    else if (t.geometry == "Around")
    {
        // 시전자 중심 원형 AOE
        int radius = (t.range < 0) ? 99 : t.range;
        for (auto* c : grid->GetAllCharacters())
        {
            if (!c || !c->IsAlive()) continue;
            if (t.filter == "Self" && c != caster) continue;
            if (t.filter == "Enemy" && c == caster) continue; // 간단 필터 (팀 구분은 추후)
            int dist = grid->ManhattanDistance(caster->GetGridPosition()->Get(),
                                               c->GetGridPosition()->Get());
            if (dist <= radius)
                targets.push_back(c);
        }
    }
    else if (t.geometry == "Line")
    {
        // 4방향 직선 (십자 모양)
        const Math::ivec2 dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        int reach = (t.range < 0) ? 99 : t.range;
        for (const auto& dir : dirs)
        {
            for (int step = 1; step <= reach; ++step)
            {
                Math::ivec2 tile = caster->GetGridPosition()->Get()
                                   + Math::ivec2{ dir.x * step, dir.y * step };
                if (!grid->IsValidTile(tile)) break;
                // 벽 통과 불가
                if (grid->GetTileType(tile) == GridSystem::TileType::Wall) break;
                Character* hit = grid->GetCharacterAt(tile);
                if (hit && hit->IsAlive()) targets.push_back(hit);
            }
        }
    }
    else if (t.geometry == "OddEven")
    {
        // 바둑판 패턴 — 홀수 or 짝수 타일의 모든 캐릭터
        // (플레이어가 홀/짝 선택하는 기능은 추후 구현, 현재는 전체 대상)
        for (auto* c : grid->GetAllCharacters())
        {
            if (!c || !c->IsAlive() || c == caster) continue;
            targets.push_back(c);
        }
    }

    // ─────────────────────────────────────────────────
    // 피해 적용
    // ─────────────────────────────────────────────────
    if (spell.damage_formula != "0" && !spell.damage_formula.empty())
    {
        int damage = CalculateSpellDamage(spell, upcast_level);
        for (auto* tgt : targets)
        {
            if (damage > 0)  combat->ApplyDamage(caster, tgt, damage);
            else if (damage < 0)
                tgt->SetHP(std::min(tgt->GetHP() + (-damage), tgt->GetMaxHP()));
        }
    }

    // ─────────────────────────────────────────────────
    // 상태 효과 적용 (기존 로직 유지, targets 루프로 통합)
    // ─────────────────────────────────────────────────
    if (spell.effect_status != "Basic" && spell.effect_duration > 0)
    {
        auto* handler = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();
        // Around/Line/OddEven은 이미 targets에 대상 목록이 있음
        // Single/Self 도 동일 루프 사용
        for (auto* tgt : targets)
        {
            tgt->AddEffect(spell.effect_status, spell.effect_duration);
            if (handler) handler->OnApplied(tgt, spell.effect_status);
        }
        // Around 타겟 없고 filter==Self 인 경우 (ex. Purify) 이미 Self geometry로 처리됨
    }

    // ─────────────────────────────────────────────────
    // 지형 / 특수 이동 (기존 유지)
    // ─────────────────────────────────────────────────
    if (spell.summon_type != "NULL" && !spell.summon_type.empty())
        Engine::GetLogger().LogEvent("SpellSystem: Summon " + spell.summon_type + " at ...");

    if (spell.move_type != "current location" && !spell.move_type.empty())
        Engine::GetLogger().LogEvent("SpellSystem: Special move — " + spell.move_type);
}
```

---

### 수정 6: GridSystem.h — EnableSpellTargetingMode 시그니처 변경

```cpp
// 변경 전
void EnableSpellTargetingMode(Math::ivec2 center, int range);

// 변경 후
void EnableSpellTargetingMode(Math::ivec2 center, const std::string& geometry, int range);
```

또한 Line 기하학용 헬퍼 추가:

```cpp
/// @brief 4방향 직선 타일 목록 반환 (Line 기하학 시각화용)
std::vector<Math::ivec2> GetLineTiles(Math::ivec2 center, int reach) const;
```

---

### 수정 7: GridSystem.cpp — EnableSpellTargetingMode 구현 변경

```cpp
void GridSystem::EnableSpellTargetingMode(Math::ivec2 center,
                                           const std::string& geometry, int range)
{
    spell_targeting_mode_active_ = true;
    spell_targetable_tiles_.clear();

    if (geometry == "Self")
    {
        spell_targetable_tiles_.insert(center);
    }
    else if (geometry == "Line")
    {
        for (const auto& tile : GetLineTiles(center, (range < 0 ? MAP_HEIGHT : range)))
            spell_targetable_tiles_.insert(tile);
    }
    else if (geometry == "OddEven")
    {
        // 전체 타일 표시
        for (int y = 0; y < MAP_HEIGHT; ++y)
            for (int x = 0; x < MAP_WIDTH; ++x)
                spell_targetable_tiles_.insert({ x, y });
    }
    else
    {
        // Single / Around / Point — 맨해튼 거리 이내 모든 타일
        int r = (range < 0) ? MAP_HEIGHT + MAP_WIDTH : range;
        for (int y = 0; y < MAP_HEIGHT; ++y)
        {
            for (int x = 0; x < MAP_WIDTH; ++x)
            {
                Math::ivec2 tile{ x, y };
                if (IsValidTile(tile) && ManhattanDistance(center, tile) <= r)
                    spell_targetable_tiles_.insert(tile);
            }
        }
    }
}

std::vector<Math::ivec2> GridSystem::GetLineTiles(Math::ivec2 center, int reach) const
{
    std::vector<Math::ivec2> result;
    const Math::ivec2 dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
    for (const auto& dir : dirs)
    {
        for (int step = 1; step <= reach; ++step)
        {
            Math::ivec2 tile{ center.x + dir.x * step, center.y + dir.y * step };
            if (!IsValidTile(tile)) break;
            if (GetTileType(tile) == TileType::Wall) break;
            result.push_back(tile);
        }
    }
    return result;
}
```

---

### 수정 8: PlayerInputHandler.cpp — SelectSpell에서 geometry 전달

```cpp
void PlayerInputHandler::SelectSpell(const std::string& spell_id, Character* caster)
{
    m_selected_spell_id = spell_id;
    m_state = ActionState::TargetingForSpell;

    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto* grid      = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (spell_sys && grid && caster)
    {
        const SpellData* spell = spell_sys->GetSpellData(spell_id);
        if (spell)
            grid->EnableSpellTargetingMode(
                caster->GetGridPosition()->Get(),
                spell->targeting.geometry,   // ← geometry 전달
                spell->targeting.range
            );
    }
}
```

---

### 수정 9: spell_table.csv 갱신

헤더 행:

```
ID,Name,Category,Classes,Required Slot Level,Targeting,Upcasting Effect,Effect
```

각 행 Targeting 컬럼 (col[5]) 값:

| ID        | 새 Targeting 값     |
| --------- | ----------------- |
| S_ATK_010 | `Enemy:Single:4`  |
| S_ATK_020 | `Enemy:Around:2`  |
| S_ATK_030 | `Enemy:Line:4`    |
| S_ATK_040 | `Any:OddEven:-1`  |
| S_ATK_050 | `Enemy:Single:1`  |
| S_ATK_060 | `Enemy:Single:-1` |
| S_ATK_070 | `Enemy:Single:1`  |
| S_BUF_010 | `Ally:Single:4`   |
| S_BUF_020 | `Self:Single:0`   |
| S_DEB_010 | `Enemy:Single:5`  |
| S_DEB_020 | `Enemy:Around:3`  |
| S_ENH_010 | `Self:Single:0`   |
| S_ENH_020 | `Self:Single:0`   |
| S_ENH_030 | `Ally:Single:5`   |
| S_ENH_040 | `Self:Single:0`   |
| S_ENH_050 | `Any:Around:4`    |
| S_ENH_060 | `Self:Single:0`   |
| S_GEO_010 | `Empty:Point:6`   |
| S_GEO_020 | `Empty:Point:5`   |
| S_GEO_030 | `Empty:Point:1`   |

---

### 구현 순서

```
1. SpellSystem.h    — SpellTargeting 구조체 추가, SpellData 필드 교체
                      (target_type + range → targeting: SpellTargeting)
                      ParseRange 선언 제거, ParseTargeting 선언 추가
2. SpellSystem.cpp  — ParseTargeting 구현
3. SpellSystem.cpp  — ParseCSVRow: col[5]=targeting, col[6]=upcastable, col[7]=effect
                      LoadFromCSV: columns.size() < 8 로 변경
4. SpellSystem.cpp  — CanCast: geometry/range 기반 체크로 교체
5. SpellSystem.cpp  — ApplySpellEffect: geometry 기반 targets 목록 생성으로 교체
6. GridSystem.h     — EnableSpellTargetingMode 시그니처 변경, GetLineTiles 선언 추가
7. GridSystem.cpp   — EnableSpellTargetingMode 구현 교체, GetLineTiles 구현 추가
8. PlayerInputHandler.cpp — SelectSpell: geometry 전달
9. spell_table.csv  — 헤더 변경 + 각 행 Targeting 컬럼 값 갱신
```

### 검증

- **Meteor** (`Any:OddEven:-1`): 스펠 선택 시 맵 전체 타일 빨간색 → 클릭하면 사거리 체크 통과
- **Dragon's Fury** (`Enemy:Line:4`): 스펠 선택 시 시전자 기준 상하좌우 4칸 십자 빨간색 표시
- **Fire Bolt** (`Enemy:Single:4`): 반경 4 이내 타일 빨간색
- **Tail Swipe** (`Enemy:Around:2`): 반경 2 원형 → 시전 시 반경 내 모든 적 피해
- **Magic Missile** (`Enemy:Single:-1`): 맵 전체 타일 빨간색 (무한 사거리)

---

## Effect Special 파트 처리

### 문제

Mana Conversion의 Effect 필드는 4줄 템플릿 이후 5번째 줄에 `Special:` 항목이 존재한다:

```
Deals (Spell Level - Required Spell Level + 1)d10 damage.
Applies "Basic" status for 0 turns.
Move to current location.
Summons NULL at current location.
Special: Recover a Spell Slot of (Spell Level - Required Spell Level + 1) level.
```

현재 `ParseEffectField`는 4줄만 읽고 종료하므로 Special 줄이 완전히 무시된다.

---

### 설계

- `SpellData`에 `special_effect` 필드 추가 (raw 문자열, 없으면 빈 문자열)
- `ParseEffectField`: 4줄 이후 잔여 줄 전체를 while로 읽어 `"Special:"` 접두어 줄 저장
- `SpellSlots`에 `RestoreOne(int level)` 메서드 추가
  (기존 `Recover(max_level)`은 지정 레벨 이하 전체 복구라 부적합)
- `ApplySpellEffect` 끝에서 `ApplySpecialEffect` 호출 (키워드 디스패치)

> 향후 Special 종류가 늘어나도 `ApplySpecialEffect` 안에 케이스만 추가하면 된다.

---

### 수정 1: SpellData — special_effect 필드 추가

```cpp
struct SpellData
{
    // ... 기존 필드 ...
    std::string special_effect; // "Special:" 줄 내용. 없으면 빈 문자열
};
```

---

### 수정 2: SpellSystem.cpp — ParseEffectField 끝에 Special 파싱 추가

4줄 블록(Summons 파싱) 직후에 추가:

```cpp
// Line 5+: "Special: ..." (선택적, 복수 줄 가능)
while (std::getline(ss, line))
{
    const std::string special_prefix = "Special:";
    if (line.rfind(special_prefix, 0) == 0)
    {
        std::string content = line.substr(special_prefix.size());
        // 앞 공백 제거
        auto start = content.find_first_not_of(" \t");
        if (start != std::string::npos)
            content = content.substr(start);

        // 복수 Special 줄은 "; "로 이어붙임
        if (data.special_effect.empty())
            data.special_effect = content;
        else
            data.special_effect += "; " + content;
    }
}
```

---

### 수정 3: SpellSlots.h / .cpp — RestoreOne 추가

```cpp
// SpellSlots.h
/// @brief 특정 레벨 슬롯 1개 복구 (max_slots 초과 불가)
void RestoreOne(int level);
```

```cpp
// SpellSlots.cpp
void SpellSlots::RestoreOne(int level)
{
    auto it = current_slots.find(level);
    if (it == current_slots.end()) return;

    auto max_it = max_slots.find(level);
    int  cap    = (max_it != max_slots.end()) ? max_it->second : 0;
    if (it->second < cap)
        it->second++;
}
```

---

### 수정 4: SpellSystem.h — ApplySpecialEffect 선언 추가

```cpp
private:
    // ... 기존 헬퍼 ...
    void ApplySpecialEffect(Character* caster, const SpellData& spell, int upcast_level);
```

---

### 수정 5: SpellSystem.cpp — ApplySpecialEffect 구현

```cpp
void SpellSystem::ApplySpecialEffect(Character* caster,
                                      const SpellData& spell,
                                      int upcast_level)
{
    if (spell.special_effect.empty()) return;

    // ── "Recover a Spell Slot of (formula) level." ──
    if (spell.special_effect.find("Recover a Spell Slot") != std::string::npos)
    {
        // 공식: Spell Level - Required Spell Level + 1
        int used_level    = (upcast_level > 0) ? upcast_level : spell.spell_level;
        int restore_level = used_level - spell.spell_level + 1;
        restore_level     = std::max(1, restore_level); // 최소 1레벨

        SpellSlots* slots = caster->GetSpellSlots();
        if (slots)
        {
            slots->RestoreOne(restore_level);
            Engine::GetLogger().LogEvent(
                caster->TypeName() + " recovered 1 level-"
                + std::to_string(restore_level) + " slot via Mana Conversion");
        }
    }
    // 향후 다른 Special 패턴은 여기 추가
}
```

---

### 수정 6: SpellSystem.cpp — ApplySpellEffect 끝에 Special 호출

기존 지형/이동 처리 블록 **맨 끝**에 한 줄 추가:

```cpp
// ... summon / move 로그 ...

// Special 효과 처리
ApplySpecialEffect(caster, spell, upcast_level);
```

---

### 구현 순서

```
1. SpellSystem.h      — SpellData에 special_effect 추가
                        ApplySpecialEffect 선언 추가
2. SpellSlots.h       — RestoreOne(int level) 선언 추가
3. SpellSlots.cpp     — RestoreOne 구현
4. SpellSystem.cpp    — ParseEffectField: while 루프로 Special 라인 파싱 추가
5. SpellSystem.cpp    — ApplySpecialEffect 구현
6. SpellSystem.cpp    — ApplySpellEffect 끝에 ApplySpecialEffect 호출 추가
```

### 검증

- Mana Conversion 시전 전: Dragon 레벨 1 슬롯 수 기록
- 시전 후 (upcast_level=0): 레벨 1 슬롯 1개 복구 확인 (스탯 패널에서 `Lv1:N/M` 증가)
- 이미 풀인 슬롯에 시전: 초과 복구 안 됨 확인
- 업캐스트 (레벨 2 슬롯 사용, upcast_level=2): 레벨 3 슬롯 복구 확인
- Special 없는 스펠 (Fire Bolt 등): `ApplySpecialEffect` 즉시 반환, 부작용 없음

---

## 업캐스팅 UI / 흐름 구현

### 문제

- Spell List 패널에서 업캐스트 가능 여부가 표시되지 않음
- 스펠 클릭 시 슬롯 레벨 선택 없이 무조건 `upcast_level=0`으로 시전됨
- `CalculateSpellDamage`가 업캐스트 다이스를 하드코딩(`1d8`)으로 계산 (TODO 주석 있음)

---

### 업캐스팅 흐름

```
SelectingSpell
  │
  ├─ 비업캐스트 스펠 클릭 → SelectSpell(id, caster, spell_level)
  │                         → TargetingForSpell
  │
  └─ 업캐스트 스펠의 레벨 버튼 클릭 → SelectSpell(id, caster, chosen_level)
                                       → TargetingForSpell
```

Spell List 패널에서 업캐스트 가능 스펠은 슬롯 레벨 버튼(Lv1~Lv5)을 인라인으로 표시한다.
새 `ActionState`는 불필요 — `SelectingSpell` 상태 안에서 UI만 확장한다.

---

### 수정 1: SpellData — upcast_dice 필드 추가

```cpp
struct SpellData
{
    // ... 기존 필드 ...
    std::string upcast_dice; // 레벨 차이당 굴리는 주사위. "1d6", "2d6" 등. 없으면 빈 문자열
};
```

> `damage_formula`는 **베이스 주사위만** 저장하도록 변경됨.
> 업캐스트 부분(`+ (...)d6`)은 `ParseDamageFormula` 헬퍼가 분리해 `upcast_dice`에 저장.

---

### 수정 2: SpellSystem.h — ParseDamageFormula 선언 추가

```cpp
private:
    // 기존 헬퍼들 ...
    void ParseDamageFormula(const std::string& formula_str, SpellData& data) const;
```

---

### 수정 3: SpellSystem.cpp — ParseDamageFormula 구현

`damage_formula` 원본 문자열을 받아 `data.damage_formula`(베이스)와 `data.upcast_dice`(레벨당 주사위)로 분리한다.

```cpp
void SpellSystem::ParseDamageFormula(const std::string& formula_str, SpellData& data) const
{
    // 힐링: "-(...)" — 음수 래퍼 처리
    bool negative = (formula_str.size() >= 2
                     && formula_str[0] == '-' && formula_str[1] == '(');
    std::string f = negative
                    ? formula_str.substr(2, formula_str.size() - 3) // "-(X)" → "X"
                    : formula_str;

    // " + " 로 베이스와 업캐스트 분리
    auto plus_pos = f.find(" + ");
    if (plus_pos == std::string::npos)
    {
        // 업캐스트 없음: 전체가 베이스
        data.damage_formula = negative ? "-(" + f + ")" : f;
        data.upcast_dice    = "";
        return;
    }

    std::string base_part   = f.substr(0, plus_pos);      // "2d8", "(...)d10" 등
    std::string upcast_part = f.substr(plus_pos + 3);     // "(...)d6" 또는 "(...) * 2d6"

    data.damage_formula = negative ? "-(" + base_part + ")" : base_part;

    // 업캐스트 주사위 추출
    // 패턴 A: "(...) * NdX" → "NdX"
    auto star_pos = upcast_part.find("* ");
    if (star_pos != std::string::npos)
    {
        data.upcast_dice = upcast_part.substr(star_pos + 2); // "2d6", "1d20"
        return;
    }
    // 패턴 B: "(...)dX" → "1dX"
    auto d_pos = upcast_part.rfind('d');
    if (d_pos != std::string::npos)
    {
        data.upcast_dice = "1" + upcast_part.substr(d_pos); // "1d6", "1d8"
        return;
    }

    data.upcast_dice = "";
}
```

**파싱 예시**:

| 원본 damage_formula     | damage_formula (저장) | upcast_dice |
| --------------------- | ------------------- | ----------- |
| `2d8 + (...)d6`       | `2d8`               | `1d6`       |
| `4d6 + (...) * 2d6`   | `4d6`               | `2d6`       |
| `3d20 + (...) * 1d20` | `3d20`              | `1d20`      |
| `-(1d10 + (...)d10)`  | `-(1d10)`           | `1d10`      |
| `0`                   | `0`                 | ``          |

---

### 수정 4: SpellSystem.cpp — ParseEffectField에서 ParseDamageFormula 호출

Line 1 파싱 블록을 교체:

```cpp
// Line 1: "Deals X damage." — 전체 formula 문자열 추출 후 ParseDamageFormula에 위임
if (std::getline(ss, line))
{
    const std::string prefix = "Deals ";
    const std::string suffix = " damage.";
    auto p = line.find(prefix);
    auto s = line.rfind(suffix);          // rfind: suffix가 문장 끝에 있음
    if (p != std::string::npos && s != std::string::npos)
    {
        std::string full_formula = line.substr(p + prefix.size(),
                                               s - (p + prefix.size()));
        ParseDamageFormula(full_formula, data);
    }
    else
    {
        data.damage_formula = "0";
        data.upcast_dice    = "";
    }
}
```

---

### 수정 5: SpellSystem.cpp — CalculateSpellDamage 개선

```cpp
int SpellSystem::CalculateSpellDamage(const SpellData& spell, int upcast_level)
{
    if (spell.damage_formula == "0" || spell.damage_formula.empty())
        return 0;

    auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
    if (!dice) return 0;

    int  level_diff = std::max(0, upcast_level - spell.spell_level);
    bool negative   = (!spell.damage_formula.empty() && spell.damage_formula[0] == '-');

    // 베이스 다이스 굴림 (힐링이면 음수 래퍼 제거 후 굴림)
    std::string base_str = spell.damage_formula;
    if (negative)
        base_str = base_str.substr(2, base_str.size() - 3); // "-(X)" → "X"

    int total = dice->RollDiceFromString(base_str);

    // 업캐스트 보너스 다이스
    if (level_diff > 0 && spell.upcastable && !spell.upcast_dice.empty())
        total += level_diff * dice->RollDiceFromString(spell.upcast_dice);

    return negative ? -total : total;
}
```

---

### 수정 6: SpellSystem.h — CanCast 시그니처 변경

```cpp
// 변경 전
bool CanCast(Character* caster, const std::string& spell_id, Math::ivec2 target_tile) const;

// 변경 후
bool CanCast(Character* caster, const std::string& spell_id,
             Math::ivec2 target_tile, int upcast_level = 0) const;
```

---

### 수정 7: SpellSystem.cpp — CanCast 슬롯 체크 수정

```cpp
// 기존: if (spell.spell_level > 0 && !caster->HasSpellSlot(spell.spell_level)) return false;
// 교체:

if (upcast_level > 0)
{
    // 특정 레벨로 시전: 해당 레벨 >= spell_level 이고 슬롯 존재 확인
    if (upcast_level < spell.spell_level) return false;
    if (!caster->HasSpellSlot(upcast_level))   return false;
}
else if (spell.spell_level > 0)
{
    // GetAvailableSpells 호출 (레벨 미지정): spell_level 이상 슬롯 하나라도 있으면 OK
    bool has_any = false;
    for (int lv = spell.spell_level; lv <= 5; ++lv)
        if (caster->HasSpellSlot(lv)) { has_any = true; break; }
    if (!has_any) return false;
}
```

---

### 수정 8: SpellSystem.cpp — CastSpell 슬롯 소모 수정

```cpp
// 기존: caster->ConsumeSpell(spell.spell_level);
// 교체:
int consume_level = (upcast_level > 0) ? upcast_level : spell.spell_level;
if (consume_level > 0)
    caster->ConsumeSpell(consume_level);
```

---

### 수정 9: PlayerInputHandler.h — upcast_level 필드 추가, SelectSpell 시그니처 변경

```cpp
private:
    ActionState m_state              = ActionState::None;
    std::string m_selected_spell_id;
    int         m_selected_upcast_level = 0;             // ← 신규

public:
    // 변경 전: void SelectSpell(const std::string& spell_id, Character* caster);
    // 변경 후:
    void SelectSpell(const std::string& spell_id, Character* caster, int upcast_level);
```

---

### 수정 10: PlayerInputHandler.cpp — SelectSpell upcast_level 저장

```cpp
void PlayerInputHandler::SelectSpell(const std::string& spell_id,
                                      Character* caster, int upcast_level)
{
    m_selected_spell_id     = spell_id;
    m_selected_upcast_level = upcast_level;              // ← 저장
    m_state = ActionState::TargetingForSpell;

    auto* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
    auto* grid      = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    if (spell_sys && grid && caster)
    {
        const SpellData* spell = spell_sys->GetSpellData(spell_id);
        if (spell)
            grid->EnableSpellTargetingMode(
                caster->GetGridPosition()->Get(),
                spell->targeting.geometry,
                spell->targeting.range);
    }
}
```

---

### 수정 11: PlayerInputHandler.cpp — TargetingForSpell에서 upcast_level 전달

```cpp
case ActionState::TargetingForSpell:
{
    // ...
    if (spell_sys && spell_sys->CanCast(dragon, m_selected_spell_id,
                                         clicked_tile, m_selected_upcast_level)) // ← 추가
    {
        spell_sys->CastSpell(dragon, m_selected_spell_id,
                              clicked_tile, m_selected_upcast_level);             // ← 추가
        m_state = ActionState::None;
        if (grid) grid->DisableSpellTargetingMode();
    }
    // ...
}
```

---

### 수정 12: GamePlay.cpp — Spell List 패널 업캐스트 UI

```cpp
for (const auto& spell_id : available)
{
    const SpellData* spell = spell_sys->GetSpellData(spell_id);
    if (!spell) continue;

    if (!spell->upcastable)
    {
        // ── 비업캐스트: 기존 단일 버튼 ──
        std::string label = spell->spell_name
                          + " (Lv." + std::to_string(spell->spell_level) + ")"
                          + "##" + spell_id;
        if (ImGui::Button(label.c_str()))
            m_input_handler->SelectSpell(spell_id, current, spell->spell_level);
    }
    else
    {
        // ── 업캐스트 가능: 이름 표시 + 레벨 버튼 ──
        ImGui::Text("%s (Lv.%d ↑)", spell->spell_name.c_str(), spell->spell_level);
        ImGui::SameLine();

        SpellSlots* slots = current->GetSpellSlots();
        for (int lv = spell->spell_level; lv <= 5; ++lv)
        {
            bool has_slot = slots && slots->HasSlot(lv);
            if (!has_slot) ImGui::BeginDisabled();

            std::string lv_label = "Lv" + std::to_string(lv)
                                 + "##" + spell_id + std::to_string(lv);
            if (ImGui::Button(lv_label.c_str()))
                m_input_handler->SelectSpell(spell_id, current, lv);

            if (!has_slot) ImGui::EndDisabled();
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
}
```

> `spell_level=1` 이고 Lv1 슬롯이 없고 Lv2만 있으면 `Lv1` 버튼은 비활성, `Lv2` 버튼만 활성.

---

### 구현 순서

```
1. SpellData (SpellSystem.h)     — upcast_dice 필드 추가
2. SpellSystem.h                 — ParseDamageFormula 선언 추가
                                   CanCast 시그니처에 upcast_level=0 추가
3. SpellSystem.cpp               — ParseDamageFormula 구현
4. SpellSystem.cpp               — ParseEffectField Line 1: ParseDamageFormula 호출로 교체
5. SpellSystem.cpp               — CalculateSpellDamage: upcast_dice 사용으로 교체
6. SpellSystem.cpp               — CanCast: 슬롯 체크 로직 교체
7. SpellSystem.cpp               — CastSpell: consume_level = upcast_level > 0 ? ... 교체
8. PlayerInputHandler.h          — m_selected_upcast_level 추가, SelectSpell 시그니처 변경
9. PlayerInputHandler.cpp        — SelectSpell: m_selected_upcast_level 저장
10. PlayerInputHandler.cpp       — TargetingForSpell case: upcast_level 전달
11. GamePlay.cpp                 — Spell List 패널: 업캐스트 레벨 버튼 UI 추가
```

### 검증

- **Fire Bolt** (Lv.1 required, upcastable):
  - Spell List에 `Fire Bolt (↑)` + `[Lv1][Lv2][Lv3][Lv4][Lv5]` 버튼 표시
  - 없는 슬롯 버튼은 회색(비활성)
  - Lv1 선택 → 시전 시 2d8 데미지, Lv1 슬롯 소모
  - Lv3 선택 → 시전 시 2d8 + 2×1d6 데미지, Lv3 슬롯 소모
- **Tail Swipe** (Lv.2 required, not upcastable):
  - `Tail Swipe (Lv.2)` 단일 버튼만 표시
- **슬롯 소모 확인**: 시전 후 스탯 패널의 해당 레벨 슬롯 수 감소 확인
