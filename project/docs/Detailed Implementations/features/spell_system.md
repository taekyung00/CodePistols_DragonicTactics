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

---

## 개요

현재 `SpellSystem`은 `MockSpellBase`를 사용하는 테스트 목 구조로만 존재하며, `GamePlay::Load()`에서 주석 처리되어 있습니다.
이 문서는 CSV 기반 실제 스펠 데이터 파싱부터 플레이어 입력/AI 턴 통합까지의 전체 구현을 다룹니다.

### spell_table.csv 컬럼 구조

```
ID, 이름, 계열, 사용 클래스, 요구 슬롯 레벨, 타겟, 사거리, 업캐스트 가능, 효과 설명
```

예시:

- `S_ATK_010` = 화염탄, 공격형, Dragon/Wizard, 레벨 1, 범위 공격, 4칸, 업캐스트 가능
- `S_BUF_010` = 신성한 보호, 버프형, Cleric, 레벨 1, 아군 대상, 4칸

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

- `CharacterTypes.h`의 `SpellData` 구조체에 CSV 9개 컬럼에 대응하는 필드를 추가한다. 사용 가능 클래스(`usable_classes`)는 `/` 구분 파싱이 필요하므로 `std::vector<std::string>`으로 선언한다. 사거리(`range`)는 `"4칸"` 형태이므로 `int`로 파싱해서 저장한다.
- `SpellSystem.h`에서 `MockSpellBase` 의존성을 완전히 제거하고 `SpellData` 기반 API로 교체한다.
- 메인 시전 함수명은 `CastSpell`을 유지한다. `AISystem::ExecuteDecision`이 이미 이 이름으로 호출하고 있어 수정이 불필요하다.
- private에 파싱 헬퍼(`SplitByDelimiter`, `ParseRange`, `ParseCSVRow`)와 실행 헬퍼(`ApplySpellEffect`, `CalculateSpellDamage`)를 선언한다.

---

### Task 2: CSV 파서 및 실행 로직 구현 (`SpellSystem.cpp`)

**수정 파일**: `SpellSystem.cpp`

- `LoadFromCSV`: 파일을 줄 단위로 읽어 첫 줄(헤더)을 스킵하고, 각 줄을 `SplitByDelimiter`로 분리한 뒤 `ParseCSVRow`로 `SpellData`를 만들어 `spells_` 맵에 저장한다. CSV가 UTF-8 또는 CP949로 저장될 수 있으므로 ID·bool 컬럼만 로직에 사용하고 한글 텍스트는 문자열로 그대로 보관한다.
- `SplitByDelimiter(str, delim)`: 지정된 구분자로 문자열을 분리하고 각 토큰의 앞뒤 공백을 제거한다. CSV 줄 분리(`,`)와 `usable_classes` 파싱(`/`) 양쪽에 모두 사용한다.
- `ParseRange(range_str)`: `"4칸"` 같은 문자열에서 숫자만 추출해 `int`로 반환한다. `"-"` 또는 빈 문자열은 `0`으로 처리한다.
- `ParseCSVRow(col)`: 9개 컬럼 벡터를 받아 각 인덱스를 `SpellData` 필드에 매핑한다. `upcastable`은 `"TRUE"` / `"O"` 모두 `true`로 처리한다.
- `CalculateSpellDamage`: `base_damage` 필드의 주사위 표기법(`"2d6"`, `"3d8+2"`)을 `DiceManager::RollDiceFromString`으로 계산한다. 업캐스트 레벨이 기본 레벨보다 높을 경우 레벨 차이만큼 추가 피해를 적용한다.
- `ApplySpellEffect`: `spell_school` 값에 따라 분기한다. `"공격"` 계열은 `CalculateSpellDamage`로 피해를 계산하고 `CombatSystem::ApplyDamage`와 `CharacterDamagedEvent` 발행으로 처리한다. `"버프"` 계열은 Week 6+ StatusEffect 시스템 연동 전까지 로그만 출력하는 스텁으로 둔다.
- `CastSpell`: 스펠 존재 확인 → `CanCast` 검증 → `ConsumeSpell`로 슬롯 소모 → `ApplySpellEffect` 순서로 실행한다.

---

### Task 3: GamePlay::Load()에 SpellSystem 등록

**수정 파일**: `GamePlay.cpp`

`Load()` 함수에서 `SpellSystem` 관련 주석을 해제하고, 등록 직후 `LoadFromCSV`를 호출해 스펠 데이터를 로드한다.

---

### Task 4: PlayerInputHandler에 스펠 실행 연동

**수정 파일**: `PlayerInputHandler.h`, `PlayerInputHandler.cpp`

- `PlayerInputHandler.h` private에 현재 선택된 스펠 ID를 저장할 `std::string m_selected_spell_id` 필드를 추가한다.
- UI에서 스펠 버튼을 눌렀을 때 `m_selected_spell_id`를 설정하고 상태를 `TargetingForSpell`로 전환하는 메서드를 추가한다.
- `TargetingForSpell` 상태에서 마우스 클릭 시 클릭된 타일을 `WorldToTile`로 변환하고, `SpellSystem::CanCast` 검증 후 `CastSpell`을 호출한다. 시전 성공 시 상태를 `None`으로 되돌린다.

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

### SpellSystem 헤더 (`SpellSystem.h`)

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

### SplitByDelimiter

CSV 행 분리(`,`)와 사용 클래스 분리(`/`) 양쪽에 사용하는 범용 헬퍼:

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

### ParseRange

`"4칸"` → `4`, `"-"` → `0` 변환:

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

### LoadFromCSV 및 ParseCSVRow

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

### CalculateSpellDamage

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

### ApplySpellEffect

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

### CastSpell

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

### PlayerInputHandler 연동

`PlayerInputHandler.h` private에 추가:

```cpp
std::string m_selected_spell_id;
```

`PlayerInputHandler.cpp` 마우스 클릭 처리:

```cpp
void PlayerInputHandler::HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon,
                                           GridSystem* grid)
{
    if (m_state == ActionState::TargetingForSpell)
    {
        Math::ivec2  clicked_tile = WorldToTile(mouse_pos);
        SpellSystem* spell_sys   =
            Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
        if (spell_sys && spell_sys->CanCast(dragon, m_selected_spell_id, clicked_tile))
        {
            spell_sys->CastSpell(dragon, m_selected_spell_id, clicked_tile);
            m_state = ActionState::None;
        }
    }
}
```

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

### 테스트 1: CSV 파싱 정확성

```cpp
SpellSystem* ss = GetGSComponent<SpellSystem>();
ss->LoadFromCSV("Assets/Data/spell_table.csv");

const SpellData* fireball = ss->GetSpellData("S_ATK_010");
ASSERT(fireball != nullptr,          "S_ATK_010 must exist");
ASSERT(fireball->upcastable == true, "Fireball must be upcastable");
ASSERT(fireball->spell_level == 1,   "Fireball requires level 1 slot");
ASSERT(fireball->range > 0,          "Fireball must have positive range");
```

### 테스트 2: 슬롯 소모 검증

```cpp
int before = player->GetSpellSlotCount(1);
ss->CastSpell(player, "S_ATK_010", enemy_tile, 0);
int after = player->GetSpellSlotCount(1);
ASSERT(after == before - 1, "Spell slot must be consumed after cast");
```

### 테스트 3: 슬롯 없을 때 시전 불가

```cpp
while (player->HasSpellSlot(1))
    ss->CastSpell(player, "S_ATK_010", enemy_tile, 0);

bool can_cast = ss->CanCast(player, "S_ATK_010", enemy_tile);
ASSERT(can_cast == false, "Cannot cast without spell slots");
```

### 테스트 4: AI UseAbility → CastSpell 연동

```cpp
AIDecision decision;
decision.type        = AIDecisionType::UseAbility;
decision.abilityName = "S_ATK_010";
decision.target      = enemy;

int hp_before = enemy->GetHP();
ai_system->ExecuteDecision(wizard, decision);
ASSERT(enemy->GetHP() < hp_before, "AI spell must deal damage");
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
