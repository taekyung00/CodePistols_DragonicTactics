# 스펠 시스템 구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: SpellSystem - CSV 파싱, 스펠 실행, 플레이어/AI 통합
**작성일**: 2026-03-08

**관련 파일**:

- [SpellSystem.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/SpellSystem.h)
- [PlayerInputHandler.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/PlayerInputHandler.h)
- [BattleOrchestrator.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/BattleOrchestrator.h)
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

---

## Implementation Tasks

### Task 1: SpellData 구조체 확장 및 CSV 파서 구현

**목표**: `spell_table.csv`를 파싱하여 `SpellData` 객체로 변환

**파일 수정**:

```
DragonicTactics/source/Game/DragonicTactics/Types/CharacterTypes.h   (SpellData 확장)
DragonicTactics/source/Game/DragonicTactics/StateComponents/SpellSystem.h/cpp  (리팩토링)
```

**Step 1: SpellData 구조체 확장** (`CharacterTypes.h`)

현재 `SpellData`에 필드 추가:

```cpp
struct SpellData
{
    std::string id;              // "S_ATK_010"
    std::string spell_name;      // "화염탄"
    std::string spell_school;    // "공격", "버프", "디버프", "강화", "지형"
    int         spell_level;     // 요구 슬롯 레벨 (0 = 슬롯 불필요)
    std::string target_type;     // "적 단일", "아군 단일", "자신", "범위"
    std::string range;           // "4칸", "5칸", "-", "주변 타일"
    bool        upcastable;      // 업캐스트 가능 여부
    std::string base_damage;     // 효과 설명 전체 (파싱된 그대로)

    // 사용 가능 클래스 (CSV의 3번 컬럼)
    std::vector<std::string> usable_classes; // ["Dragon", "Wizard"] 등
};
```

**Step 2: SpellSystem 재구현** (`SpellSystem.h`)

MockSpellBase 의존성을 제거하고 SpellData 기반으로 전환:

```cpp
class SpellSystem : public CS230::Component
{
public:
    // CSV 로드 (GamePlay::Load()에서 호출)
    void LoadFromCSV(const std::string& csv_path);

    // 스펠 존재 여부
    bool HasSpell(const std::string& spell_id) const;

    // 특정 캐릭터가 사용 가능한 스펠 목록
    std::vector<SpellData> GetAvailableSpells(Character* caster) const;

    // 스펠 시전 가능 여부 체크 (슬롯, 사거리, 타겟 검증)
    bool CanCast(Character* caster, const std::string& spell_id, Character* target) const;
    bool CanCast(Character* caster, const std::string& spell_id, Math::ivec2 target_tile) const;

    // 스펠 실행 (데미지 계산, 슬롯 소모, 이벤트 발행)
    bool ExecuteSpell(Character* caster, const std::string& spell_id,
                      Character* target, int upcast_level = 0);

    // SpellData 조회
    const SpellData* GetSpellData(const std::string& spell_id) const;

private:
    std::map<std::string, SpellData> spells_;

    // 파싱 헬퍼
    SpellData ParseCSVRow(const std::vector<std::string>& columns) const;
    std::vector<std::string> SplitCSV(const std::string& line) const;

    // 실행 헬퍼
    void ApplySpellEffect(Character* caster, const SpellData& spell,
                          Character* target, int upcast_level);
    int  CalculateSpellDamage(const SpellData& spell, Character* caster, int upcast_level);
};
```

---

### Task 2: CSV 파서 구현 (`SpellSystem.cpp`)

**주의**: CSV가 UTF-8 또는 CP949로 저장되어 있을 수 있음. ID와 bool 컬럼만 파싱에 사용하고 한글 텍스트는 문자열로 그대로 보관.

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

        auto columns = SplitCSV(line);
        if (columns.size() < 9) continue;
        if (columns[0].empty()) continue; // ID 없는 줄 스킵 (주석 행 등)

        SpellData data = ParseCSVRow(columns);
        spells_[data.id] = data;
    }
    Engine::GetLogger().LogEvent("SpellSystem: Loaded " + std::to_string(spells_.size()) + " spells");
}

SpellData SpellSystem::ParseCSVRow(const std::vector<std::string>& col) const
{
    SpellData data;
    data.id          = col[0];
    data.spell_name  = col[1];
    data.spell_school = col[2];
    // col[3]: 사용 클래스 - 쉼표로 분리
    data.usable_classes = SplitByComma(col[3]);
    data.spell_level = col[4].empty() ? 0 : std::stoi(col[4]);
    data.target_type = col[5];
    data.range       = col[6];
    data.upcastable  = (col[7] == "TRUE" || col[7] == "true");
    data.base_damage = col[8];
    return data;
}
```

---

### Task 3: GamePlay::Load()에 SpellSystem 등록

`GamePlay.cpp`의 `Load()` 함수에서 주석 해제 및 수정:

```cpp
// GamePlay.cpp - Load()
AddGSComponent(new SpellSystem());

// 시스템 초기화 후
GetGSComponent<SpellSystem>()->LoadFromCSV("Assets/Data/spell_table.csv");
```

---

### Task 4: PlayerInputHandler에 스펠 실행 연동

현재 `TargetingForSpell` 상태에서 타겟 선택 시 `SpellSystem::ExecuteSpell()` 호출.

**파일**: `PlayerInputHandler.cpp`

```cpp
// TargetingForSpell 상태에서 마우스 클릭 처리
void PlayerInputHandler::HandleMouseClick(Math::vec2 mouse_pos, Dragon* dragon,
                                           GridSystem* grid, CombatSystem* combat)
{
    if (m_state == ActionState::TargetingForSpell)
    {
        Math::ivec2 clicked_tile = WorldToTile(mouse_pos);
        Character* target = grid->GetCharacterAt(clicked_tile);

        SpellSystem* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
        if (target && spell_sys)
        {
            // 선택된 스펠 ID는 m_selected_spell_id 멤버에 저장
            if (spell_sys->CanCast(dragon, m_selected_spell_id, target))
            {
                spell_sys->ExecuteSpell(dragon, m_selected_spell_id, target);
                m_state = ActionState::None;
            }
        }
    }
}
```

`PlayerInputHandler`에 현재 선택된 스펠 ID를 저장하는 필드 추가:

```cpp
// PlayerInputHandler.h private:
std::string m_selected_spell_id;
```

---

### Task 5: BattleOrchestrator의 AI 스펠 사용 연동

`HandleAITurn()`에서 `AIDecision::type == AIDecisionType::CastSpell`인 경우 처리:

```cpp
// BattleOrchestrator.cpp
void BattleOrchestrator::HandleAITurn(Character* ai_character,
                                       TurnManager* turn_manager,
                                       AISystem* ai_system)
{
    AIDecision decision = ai_system->MakeDecision(ai_character);

    switch (decision.type)
    {
        case AIDecisionType::Move:
            // 기존 이동 처리
            break;
        case AIDecisionType::Attack:
            // 기존 공격 처리
            break;
        case AIDecisionType::CastSpell:
        {
            SpellSystem* spell_sys = Engine::GetGameStateManager().GetGSComponent<SpellSystem>();
            if (spell_sys && decision.target)
            {
                spell_sys->ExecuteSpell(ai_character, decision.spell_id, decision.target, 0);
            }
            break;
        }
        case AIDecisionType::EndTurn:
        default:
            turn_manager->EndCurrentTurn();
            break;
    }
}
```

`IAIStrategy.h`의 `AIDecision`에 `spell_id` 필드 추가:

```cpp
struct AIDecision
{
    AIDecisionType type;
    Character*     target;
    Math::ivec2    destination;
    std::string    spell_id;   // CastSpell 시 사용
    std::string    debug_reason;
};
```

---

## Implementation Examples

### 스펠 시스템 초기화 → 실행 전체 흐름

```cpp
// 1. GamePlay::Load()에서 초기화
AddGSComponent(new SpellSystem());
GetGSComponent<SpellSystem>()->LoadFromCSV("Assets/Data/spell_table.csv");

// 2. 플레이어가 스펠 버튼 클릭 → TargetingForSpell 상태
m_input_handler->SetState(ActionState::TargetingForSpell);
m_input_handler->SetSelectedSpell("S_ATK_010"); // 화염탄

// 3. 타겟 클릭 → ExecuteSpell
SpellSystem* ss = GetGSComponent<SpellSystem>();
ss->ExecuteSpell(player, "S_ATK_010", enemy, 0);
// → 슬롯 소모 → 데미지 계산 → CharacterDamagedEvent 발행

// 4. AI 스펠 사용 (FighterStrategy → CastSpell 결정)
AIDecision decision = { AIDecisionType::CastSpell, target, {}, "S_ATK_010", "Spell attack" };
// BattleOrchestrator가 처리
```

### 스펠 데이터 조회 예시

```cpp
// 특정 캐릭터가 사용 가능한 스펠 목록
SpellSystem* ss = GetGSComponent<SpellSystem>();
auto spells = ss->GetAvailableSpells(player);
for (const auto& spell : spells)
{
    Engine::GetLogger().LogEvent("Available spell: " + spell.id + " (" + spell.spell_name + ")");
}

// 스펠 시전 가능 여부
if (ss->CanCast(player, "S_ATK_010", enemy))
{
    ss->ExecuteSpell(player, "S_ATK_010", enemy, 0);
}
```

---

## Rigorous Testing

테스트 파일: `DragonicTactics/source/Game/DragonicTactics/Test/TestSpellSystem.h/cpp` 확장

### 테스트 1: CSV 파싱 정확성

```cpp
// spell_table.csv 로드 후
SpellSystem* ss = GetGSComponent<SpellSystem>();
ss->LoadFromCSV("Assets/Data/spell_table.csv");

const SpellData* fireball = ss->GetSpellData("S_ATK_010");
ASSERT(fireball != nullptr, "S_ATK_010 must exist");
ASSERT(fireball->upcastable == true, "Fireball must be upcastable");
ASSERT(fireball->spell_level == 1, "Fireball requires level 1 slot");
```

### 테스트 2: 슬롯 소모 검증

```cpp
// Dragon이 S_ATK_010(레벨1) 시전 시 레벨1 슬롯이 소모되어야 함
int before = player->GetSpellSlotCount(1);
ss->ExecuteSpell(player, "S_ATK_010", enemy, 0);
int after = player->GetSpellSlotCount(1);
ASSERT(after == before - 1, "Spell slot must be consumed after cast");
```

### 테스트 3: 슬롯 없을 때 시전 불가

```cpp
// 슬롯 모두 소모
while (player->HasSpellSlot(1))
    ss->ExecuteSpell(player, "S_ATK_010", enemy, 0);

bool can_cast = ss->CanCast(player, "S_ATK_010", enemy);
ASSERT(can_cast == false, "Cannot cast without spell slots");
```

### 테스트 4: AI CastSpell 연동

```cpp
// FighterStrategy가 스펠 슬롯 있을 때 CastSpell 결정 반환
// (FighterStrategy::ShouldUseSpellAttack 로직 검증)
AIDecision decision = fighter_strategy.MakeDecision(fighter);
if (fighter->HasAnySpellSlot())
{
    // 적 사거리 내에 있고 슬롯 있으면 CastSpell 또는 Attack
    ASSERT(decision.type == AIDecisionType::CastSpell ||
           decision.type == AIDecisionType::Attack, "AI should act");
}
```

### 게임 내 수동 테스트 (키보드)

`GamePlay.cpp`에 테스트 키 추가:

```cpp
// Q 키: 현재 캐릭터의 사용 가능한 스펠 목록 로그
if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Q))
{
    auto* ss = GetGSComponent<SpellSystem>();
    auto spells = ss->GetAvailableSpells(current);
    for (auto& s : spells)
        Engine::GetLogger().LogEvent("Spell: " + s.id + " " + s.spell_name);
}
```
