# StatusEffect 시스템 구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: StatusEffectComponent — 버프/디버프 현재 상태 추적 (단순 상태 저장소)
**작성일**: 2026-03-27

**관련 파일**:

- [StatusEffectComponent.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/Objects/Components/StatusEffectComponent.h) ← 신규 생성
- [Character.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/Objects/Character.h) ← HasBuff/HasDebuff/AddBuff/AddDebuff 추가
- [SpellSystem.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/SpellSystem.cpp) ← ApplySpellEffect 버프 처리 (spell_system.md 소유)
- [TurnManager.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.cpp) ← StartNextTurn에 TickDown 추가
- [CombatSystem.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/CombatSystem.cpp) ← 피해 계산에 버프/디버프 반영
- [Types/Events.h](../../../DragonicTactics/source/Game/DragonicTactics/Types/Events.h) ← 이미 이벤트 구조체 정의됨
- [spell_system.md](./spell_system.md) ← CastSpell / ApplySpellEffect 소유
- [Assets/Data/status_effect.csv](../../../DragonicTactics/Assets/Data/status_effect.csv) ← effectName 정의 파일

---

## 개요 및 역할 분담

### StatusEffectComponent의 역할

**단순 상태 저장소**. 지금 이 캐릭터에 어떤 효과가 활성화되어 있는지만 기록합니다.
효과를 "적용"하는 로직은 StatusEffectComponent의 책임이 아닙니다.

```
"공포가 붙어 있나?" → StatusEffectComponent::Has("Fear") → true / false
```

### 각 시스템의 책임 경계

| 시스템                       | 책임                                              |
| ------------------------- | ----------------------------------------------- |
| **SpellSystem**           | 스펠 실행 + 효과 적용 + 상태 기록(`AddBuff`/`AddDebuff` 호출) |
| **StatusEffectComponent** | 현재 활성 효과 목록 저장 + 쿼리 + 만료 관리                     |
| **CombatSystem**          | 상태 읽기(`HasBuff`/`HasDebuff`) + 피해 보정            |
| **TurnManager**           | 턴 시작 시 `TickDown` 호출                            |
| **Strategy**              | 상태 쿼리만 (`actor->HasBuff("Frenzy")`) — 직접 변경 없음  |

### 호출 체인

```
SpellSystem::CastSpell
  → SpellSystem::ApplySpellEffect   ← spell_system.md 소유
      → 효과 적용 (피해/회복 등)
      → character->AddBuff / AddDebuff  ← 상태 기록
          → StatusEffectComponent::AddEffect (저장)
          → StatusEffectAddedEvent 발행

TurnManager::StartNextTurn
  → StatusEffectComponent::TickDown  ← 만료 관리
      → StatusEffectRemovedEvent 발행 (만료 시)

CombatSystem::ExecuteAttack
  → attacker->HasBuff("Frenzy")  ← 상태 읽기만
  → 피해 보정 적용
```

### Events.h에 이미 정의된 이벤트 (신규 추가 불필요)

```cpp
StatusEffectAddedEvent   { target, effectName, duration, magnitude }
StatusEffectRemovedEvent { target, effectName, reason }
StatusEffectTickEvent    { target, effectName, damageOrHealing }
```

---

## spell_table.csv 기반 버프/디버프 매핑

effectName은 `status_effect.csv`의 NAME 컬럼과 **정확히 일치**해야 합니다.

| 스펠 ID       | 이름 (CSV Name)      | effectName     | 종류  | 지속 턴 | 상태 기록 위치                     |
| ----------- | ------------------ | -------------- | --- | ---- | ---------------------------- |
| `S_ENH_010` | Bloodlust          | `"Lifesteal"`  | 버프  | 1    | `AddBuff("Lifesteal", 1)`    |
| `S_ENH_020` | Frenzy             | `"Frenzy"`     | 버프  | 1    | `AddBuff("Frenzy", 1)`       |
| `S_ENH_060` | Shadow Hide        | `"Stealth"`    | 버프  | 1    | `AddBuff("Stealth", 1)`      |
| `S_BUF_010` | Divine Shield      | `"Blessing"`   | 버프  | 3    | `AddBuff("Blessing", 3)`     |
| `S_BUF_020` | Gale Step          | `"Haste"`      | 버프  | 2    | `AddBuff("Haste", 2)`        |
| `S_ENH_050` | Purify             | `"Purify"`     | 버프  | 1    | `AddBuff("Purify", 1)`       |
| `S_DEB_020` | Fearful Cry        | `"Fear"`       | 디버프 | 3    | `AddDebuff("Fear", 3)`       |
| `S_DEB_010` | Curse of Suffering | `"Curse"`      | 디버프 | 3    | `AddDebuff("Curse", 3)`      |
| `S_ATK_040` | Meteor             | `"Exhaustion"` | 디버프 | 1    | `AddDebuff("Exhaustion", 1)` |

> **effectName은 Strategy의 HasBuff/HasDebuff 인수와 일치해야 합니다.**
> `HasBuff(actor, "Blessing")` → `actor->HasBuff("Blessing")` → `StatusEffectComponent::HasBuff("Blessing")`

---

## Implementation Tasks

### Task 1: StatusEffectComponent 신규 생성

**파일**: `Objects/Components/StatusEffectComponent.h`

```cpp
/**
 * @file StatusEffectComponent.h
 * @brief 현재 활성화된 버프/디버프 상태를 추적하는 단순 저장소
 *        효과 적용 로직 없음 — 상태 기록/쿼리/만료만 담당
 */
#pragma once
#include "./Engine/Component.h"
#include <string>
#include <vector>

class EventBus;
class Character;

struct ActiveEffect
{
    std::string name;      // status_effect.csv의 NAME — "Blessing", "Fear", "Stealth" 등
    int         duration;  // 남은 턴 수
    int         magnitude; // 수치 강도 (없으면 0)
    bool        is_buff;   // true = 버프, false = 디버프
};

class StatusEffectComponent : public CS230::Component
{
public:
    void Update(double dt) override { (void)dt; }

    // --- 상태 추가 / 제거 ---
    void AddEffect(const std::string& name, int duration, int magnitude, bool is_buff);
    void RemoveEffect(const std::string& name);
    void RemoveAllDebuffs();

    // --- 상태 쿼리 ---
    bool HasBuff(const std::string& name) const;
    bool HasDebuff(const std::string& name) const;
    int  GetMagnitude(const std::string& name) const;

    // --- 턴 갱신 (TurnManager::StartNextTurn에서 호출) ---
    void TickDown(Character* owner, EventBus* bus);

private:
    std::vector<ActiveEffect> effects_;
};
```

**파일**: `Objects/Components/StatusEffectComponent.cpp`

```cpp
#include "pch.h"
#include "StatusEffectComponent.h"
#include "../../StateComponents/EventBus.h"
#include "../../Types/Events.h"
#include "../../Objects/Character.h"

void StatusEffectComponent::AddEffect(const std::string& name, int duration,
                                       int magnitude, bool is_buff)
{
    // 이미 있으면 지속시간 갱신
    for (auto& e : effects_)
    {
        if (e.name == name)
        {
            e.duration  = duration;
            e.magnitude = magnitude;
            return;
        }
    }
    effects_.push_back({ name, duration, magnitude, is_buff });
}

void StatusEffectComponent::RemoveEffect(const std::string& name)
{
    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(),
                       [&name](const ActiveEffect& e) { return e.name == name; }),
        effects_.end());
}

void StatusEffectComponent::RemoveAllDebuffs()
{
    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(),
                       [](const ActiveEffect& e) { return !e.is_buff; }),
        effects_.end());
}

bool StatusEffectComponent::HasBuff(const std::string& name) const
{
    for (const auto& e : effects_)
        if (e.name == name && e.is_buff) return true;
    return false;
}

bool StatusEffectComponent::HasDebuff(const std::string& name) const
{
    for (const auto& e : effects_)
        if (e.name == name && !e.is_buff) return true;
    return false;
}

int StatusEffectComponent::GetMagnitude(const std::string& name) const
{
    for (const auto& e : effects_)
        if (e.name == name) return e.magnitude;
    return 0;
}

void StatusEffectComponent::TickDown(Character* owner, EventBus* bus)
{
    std::vector<ActiveEffect> expired;

    for (auto& e : effects_)
    {
        if (e.duration > 0 && --e.duration == 0)
            expired.push_back(e);
    }

    for (const auto& e : expired)
    {
        RemoveEffect(e.name);
        if (bus)
            bus->Publish(StatusEffectRemovedEvent{ owner, e.name, "expired" });
    }
}
```

---

### Task 2: Character에 facade 메서드 추가

`StatusEffectComponent`를 직접 노출하지 않고, Strategy와 SpellSystem이 Character를 통해서만 접근합니다.

**Character.h 추가**:

```cpp
#include "Components/StatusEffectComponent.h"

// 생성자에서 컴포넌트 등록 (Character.cpp)
// AddGOComponent(new StatusEffectComponent());

// --- 버프/디버프 쿼리 (Strategy에서 사용) ---
bool HasBuff(const std::string& buff_name) const;
bool HasDebuff(const std::string& debuff_name) const;

// --- 버프/디버프 상태 변경 (SpellSystem과 CombatSystem에서만 호출) ---
void AddBuff(const std::string& name, int duration, int magnitude = 0);
void AddDebuff(const std::string& name, int duration, int magnitude = 0);
void RemoveBuff(const std::string& name);
void RemoveAllDebuffs();
```

**Character.cpp 구현**:

```cpp
// 생성자 내부
AddGOComponent(new StatusEffectComponent());

bool Character::HasBuff(const std::string& buff_name) const
{
    auto* se = GetGOComponent<StatusEffectComponent>();
    return se && se->HasBuff(buff_name);
}

bool Character::HasDebuff(const std::string& debuff_name) const
{
    auto* se = GetGOComponent<StatusEffectComponent>();
    return se && se->HasDebuff(debuff_name);
}

void Character::AddBuff(const std::string& name, int duration, int magnitude)
{
    auto* se = GetGOComponent<StatusEffectComponent>();
    if (!se) return;
    se->AddEffect(name, duration, magnitude, true);

    auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
    if (bus)
        bus->Publish(StatusEffectAddedEvent{ this, name, duration, magnitude });
}

void Character::AddDebuff(const std::string& name, int duration, int magnitude)
{
    auto* se = GetGOComponent<StatusEffectComponent>();
    if (!se) return;
    se->AddEffect(name, duration, magnitude, false);

    auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
    if (bus)
        bus->Publish(StatusEffectAddedEvent{ this, name, duration, magnitude });
}

void Character::RemoveBuff(const std::string& name)
{
    if (auto* se = GetGOComponent<StatusEffectComponent>())
        se->RemoveEffect(name);
}

void Character::RemoveAllDebuffs()
{
    if (auto* se = GetGOComponent<StatusEffectComponent>())
        se->RemoveAllDebuffs();
}
```

---

### Task 3: HasAttackedThisTurn 추가

RogueStrategy의 `HasAttackedThisTurn` stub 해제용.

**Character.h**:

```cpp
bool HasAttackedThisTurn() const { return has_attacked_this_turn_; }
void SetHasAttackedThisTurn(bool value) { has_attacked_this_turn_ = value; }

// private:
bool has_attacked_this_turn_ = false;
```

**Character.cpp** — `RefreshActionPoints()`에 추가:

```cpp
void Character::RefreshActionPoints()
{
    // 기존 로직 유지 ...
    has_attacked_this_turn_ = false; // 턴 시작 시 리셋
}
```

**CombatSystem::ExecuteAttack()** — 공격 후 설정:

```cpp
attacker->SetHasAttackedThisTurn(true);
```

---

### Task 4: SpellSystem::ApplySpellEffect 연동 확인

> **이 Task는 `spell_system.md`의 신형 ApplySpellEffect가 완성되면 자동으로 충족됩니다.**
> `spell_school` 기반 stub 교체 작업은 더 이상 필요하지 않습니다.

신형 `ApplySpellEffect`(spell_system.md 소유)는 Effect 필드의 `effect_status`로 분기하여
`AddBuff`/`AddDebuff`를 직접 호출합니다:

```cpp
// spell_system.md의 신형 ApplySpellEffect 내부 (참조용)
if (spell.effect_status != "Basic" && spell.effect_duration > 0)
{
    static const std::set<std::string> DEBUFFS = { "Fear", "Curse", "Exhaustion" };
    bool is_debuff = (DEBUFFS.count(spell.effect_status) > 0);

    // ... target->AddBuff(spell.effect_status, spell.effect_duration)
    //     target->AddDebuff(spell.effect_status, spell.effect_duration)
}
```

StatusEffectComponent(Task 1)와 Character facade(Task 2)가 완성되면 이 호출이 즉시 동작합니다.

**Purify 특수 처리** — `effect_status == "Purify"` 시 `RemoveAllDebuffs()` 호출로 구현:

```cpp
// ApplySpellEffect 내 Purify 처리 (spell_system.md Task 2 확장 시 추가)
if (spell.effect_status == "Purify")
{
    Character* se_target = target ? target : caster;
    if (se_target) se_target->RemoveAllDebuffs();
}
```

---

### Task 5: TurnManager::StartNextTurn에 TickDown 추가

**수정 파일**: `TurnManager.cpp`

```cpp
void TurnManager::StartNextTurn()
{
    Character* current = GetCurrentCharacter();
    if (current)
    {
        auto* se = current->GetGOComponent<StatusEffectComponent>();
        if (se)
            se->TickDown(current, eventBus); // 지속시간 감소 + 만료 제거

        current->RefreshActionPoints();      // has_attacked_this_turn_ 리셋도 여기서
    }
    // ... 기존 이벤트 발행 ...
}
```

---

### Task 6: CombatSystem에 버프/디버프 피해 보정 반영

**수정 파일**: `CombatSystem.cpp`의 `ExecuteAttack`

CombatSystem은 상태를 **읽기만** 합니다. 직접 AddBuff/AddDebuff하지 않습니다.
단, Stealth와 Frenzy는 공격 시 소모되므로 `RemoveBuff`는 호출합니다.

```cpp
void CombatSystem::ExecuteAttack(Character* attacker, Character* target)
{
    int base_damage = /* 기존 주사위 로직 */;

    // ── 피해 보정 읽기 ──
    if (attacker->HasBuff("Blessing"))
        base_damage += attacker->GetGOComponent<StatusEffectComponent>()->GetMagnitude("Blessing");

    if (attacker->HasDebuff("Fear"))
        base_damage += attacker->GetGOComponent<StatusEffectComponent>()->GetMagnitude("Fear"); // 음수

    // ── Stealth: 첫 피해 2배, 공격 시 소모 ──
    if (attacker->HasBuff("Stealth"))
    {
        base_damage *= 2;
        attacker->RemoveBuff("Stealth");
    }

    bool has_lifesteal = attacker->HasBuff("Lifesteal");
    bool has_frenzy    = attacker->HasBuff("Frenzy");

    ApplyDamage(target, base_damage);
    attacker->SetHasAttackedThisTurn(true);

    // ── Lifesteal: 피해의 절반 회복 ──
    if (has_lifesteal)
    {
        int heal = base_damage / 2;
        attacker->SetHP(std::min(attacker->GetHP() + heal, attacker->GetMaxHP()));
    }

    // ── Frenzy: 10 이상 피해 시 타겟에, 미만 시 자신에게 무작위 디버프, 소모 ──
    if (has_frenzy)
    {
        attacker->RemoveBuff("Frenzy");
        if (base_damage >= 10)
            target->AddDebuff("Curse", 1);   // TODO: 실제 무작위 디버프 (Curse/Fear/Exhaustion)
        else
            attacker->AddDebuff("Curse", 1); // TODO: 실제 무작위 디버프
    }
}
```

---

## stub 해제 목록

| 파일                    | stub 메서드                      | 해제 후                                  |
| --------------------- | ----------------------------- | ------------------------------------- |
| `FighterStrategy.cpp` | `HasBuff(actor, "Lifesteal")` | `return actor->HasBuff(buffName)`     |
| `FighterStrategy.cpp` | `HasBuff(actor, "Blessing")`  | `return actor->HasBuff(buffName)`     |
| `FighterStrategy.cpp` | `HasBuff(actor, "Frenzy")`    | `return actor->HasBuff(buffName)`     |
| `FighterStrategy.cpp` | `IsFearActive(dragon)`        | `return dragon->HasDebuff("Fear")`    |
| `RogueStrategy.cpp`   | `IsInStealth(actor)`          | `return actor->HasBuff("Stealth")`    |
| `RogueStrategy.cpp`   | `HasBuff(actor, "Haste")`     | `return actor->HasBuff(buffName)`     |
| `RogueStrategy.cpp`   | `HasAttackedThisTurn(actor)`  | `return actor->HasAttackedThisTurn()` |

---

## Rigorous Testing

### 테스트 1: AddBuff → HasBuff 즉시 확인

```cpp
fighter->AddBuff("Blessing", 3, 3);
ASSERT(fighter->HasBuff("Blessing"),    "HasBuff after AddBuff");
ASSERT(!fighter->HasDebuff("Blessing"), "Blessing is buff not debuff");
```

### 테스트 2: TickDown — 지속시간 1 → 만료

```cpp
dragon->AddDebuff("Fear", 1, -3);
auto* se = dragon->GetGOComponent<StatusEffectComponent>();
se->TickDown(dragon, eventBus);
ASSERT(!dragon->HasDebuff("Fear"), "Fear expired after 1 tick");
```

### 테스트 3: TickDown — 지속시간 3 → 2회 후 유지

```cpp
fighter->AddBuff("Blessing", 3);
auto* se = fighter->GetGOComponent<StatusEffectComponent>();
se->TickDown(fighter, eventBus);
se->TickDown(fighter, eventBus);
ASSERT(fighter->HasBuff("Blessing"), "Blessing still active after 2 ticks");
se->TickDown(fighter, eventBus);
ASSERT(!fighter->HasBuff("Blessing"), "Blessing expired after 3 ticks");
```

### 테스트 4: RemoveAllDebuffs (Purify)

```cpp
fighter->AddDebuff("Fear", 3);
fighter->AddDebuff("Curse", 2);
fighter->AddBuff("Blessing", 3);
fighter->RemoveAllDebuffs();
ASSERT(!fighter->HasDebuff("Fear"),  "Fear removed");
ASSERT(!fighter->HasDebuff("Curse"), "Curse removed");
ASSERT(fighter->HasBuff("Blessing"), "Blessing preserved");
```

### 테스트 5: HasAttackedThisTurn — 공격 후 설정, 턴 시작 시 리셋

```cpp
ASSERT(!rogue->HasAttackedThisTurn(), "False at turn start");
combat->ExecuteAttack(rogue, dragon);
ASSERT(rogue->HasAttackedThisTurn(), "True after attack");
rogue->RefreshActionPoints();
ASSERT(!rogue->HasAttackedThisTurn(), "Reset on turn start");
```

### 테스트 6: 중복 AddBuff → 지속시간 갱신

```cpp
fighter->AddBuff("Blessing", 1);
fighter->AddBuff("Blessing", 3);
auto* se = fighter->GetGOComponent<StatusEffectComponent>();
se->TickDown(fighter, eventBus);
se->TickDown(fighter, eventBus);
ASSERT(fighter->HasBuff("Blessing"), "Duration refreshed to 3");
```

### 테스트 7: StatusEffectAddedEvent 발행 확인

```cpp
bool received = false;
eventBus->Subscribe<StatusEffectAddedEvent>([&](const StatusEffectAddedEvent& e) {
    if (e.effectName == "Blessing" && e.target == fighter) received = true;
});
fighter->AddBuff("Blessing", 3, 3);
ASSERT(received, "Event published on AddBuff");
```

### 테스트 8: StatusEffectRemovedEvent 발행 확인

```cpp
bool removed = false;
eventBus->Subscribe<StatusEffectRemovedEvent>([&](const StatusEffectRemovedEvent& e) {
    if (e.effectName == "Fear" && e.reason == "expired") removed = true;
});
dragon->AddDebuff("Fear", 1);
dragon->GetGOComponent<StatusEffectComponent>()->TickDown(dragon, eventBus);
ASSERT(removed, "Event published on expiry");
```

---

## 구현 순서

```
1. StatusEffectComponent.h/cpp 신규 생성
2. Character.h/cpp에 컴포넌트 등록 + facade 추가
3. Character.h/cpp에 has_attacked_this_turn_ 추가 + RefreshActionPoints 리셋
4. CombatSystem::ExecuteAttack에 SetHasAttackedThisTurn + 피해 보정 추가
5. TurnManager::StartNextTurn에 TickDown 추가
6. spell_system.md의 신형 ApplySpellEffect 구현 (Task 4 자동 충족)
7. FighterStrategy / RogueStrategy stub 해제
8. cmake --preset windows-debug 재실행 (신규 파일 감지)
9. 테스트 실행
```

> **⚠️ 8번(CMake 재실행) 전에 빌드하면 StatusEffectComponent를 찾지 못합니다.**
