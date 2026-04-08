# 스펠 시전 UI 로그

## 개요

스펠이 시전될 때 어떤 캐릭터가 어떤 스펠을 몇 레벨로 사용했는지 화면 오른쪽 하단에 3초간 텍스트로 표시.
`TypeName()`으로 이름을 동적으로 출력하므로 Cleric/Wizard/Rogue 추가 시 코드 수정 불필요.

표시 형식: `[Dragon] Fire Bolt Lv.2`

---

## 수정된 파일

### 1. `StateComponents/SpellSystem.cpp`

`CastSpell()` 함수, `LogEvent` 라인 직후에 `SpellCastEvent` 발행 추가.

```cpp
auto& gs = Engine::GetGameStateManager();
if (auto* eventBus = gs.GetGSComponent<EventBus>())
{
    eventBus->Publish(SpellCastEvent{
        caster, spell.spell_name, upcast_level, target_tile, consume_level
    });
}
```

### 2. `States/GamePlayUIManager.h`

`SpellLog` 구조체, `AddSpellLog()` 선언, 관련 멤버 추가.

```cpp
struct SpellLog { std::string text; double lifetime; };
void AddSpellLog(const std::string& caster_name, const std::string& spell_name, int level);
std::vector<SpellLog> m_spell_logs;
static constexpr double SPELL_LOG_LIFETIME  = 3.0;
static constexpr int    SPELL_LOG_MAX_COUNT = 5;
```

### 3. `States/GamePlayUIManager.cpp`

- `Update()`: SpellLog lifetime tick + 만료된 항목 제거
- `Draw()`: DrawCharacterStatsPanel 직전에 오른쪽 하단 렌더링 (x=800, base_y=50, 줄간격 28, GOLD 색상)
- `AddSpellLog()` 구현: 최대 5개 초과 시 가장 오래된 항목 제거

### 4. `States/GamePlay.cpp`

`Load()`에서 `CharacterDamagedEvent` 구독 직후 추가:

```cpp
GetGSComponent<EventBus>()->Subscribe<SpellCastEvent>(
    [this](const SpellCastEvent& event)
    {
        if (event.caster)
            m_ui_manager->AddSpellLog(event.caster->TypeName(), event.spellName, event.spellLevel);
    });
```

---

## 동작 규칙

| 항목 | 값 |
|---|---|
| 표시 위치 | 화면 고정 (x=800, 하단 y=50부터 위로 쌓임) |
| 표시 시간 | 3초 (자동 제거) |
| 최대 개수 | 5개 (초과 시 가장 오래된 항목 제거) |
| 색상 | CS200::GOLD |
| 폰트 | Fonts::Outlined, scale 0.5 |

---

## 확장성

새 캐릭터 추가 시 별도 코드 수정 없음. `Character::TypeName()`이 캐릭터 이름을 반환하므로 자동으로 올바른 이름 표시.

---

## 관련 이벤트

`SpellCastEvent` 구조체 (`Types/Events.h`):
- `caster`: 시전자
- `spellName`: 스펠 이름
- `spellLevel`: 시전 레벨 (upcast_level)
- `targetGrid`: 타겟 타일
- `spellSlotUsed`: 소모된 슬롯 레벨
