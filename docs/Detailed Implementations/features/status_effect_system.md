# StatusEffect 시스템 구현 계획

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG
**기능**: StatusEffectComponent — 버프/디버프 현재 상태 추적 (단순 상태 저장소)
**작성일**: 2026-03-27

**관련 파일**:

- [StatusEffectComponent.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/Objects/Components/StatusEffectComponent.h) ← 신규 생성 (상태 저장소)
- [StatusEffectHandler.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/StatusEffectHandler.h) ← **신규 생성 (실행 로직)** ★
- [Character.h/cpp](../../../DragonicTactics/source/Game/DragonicTactics/Objects/Character.h) ← HasBuff/HasDebuff/AddBuff/AddDebuff 추가
- [SpellSystem.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/SpellSystem.cpp) ← ApplySpellEffect에 OnApplied 추가 (spell_system.md 소유)
- [TurnManager.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/TurnManager.cpp) ← StartNextTurn에 TickDown + OnTurnStart 추가
- [CombatSystem.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/CombatSystem.cpp) ← ExecuteAttack → StatusEffectHandler 위임
- [States/GamePlay.cpp](../../../DragonicTactics/source/Game/DragonicTactics/States/GamePlay.cpp) ← AddGSComponent(new StatusEffectHandler()) 추가
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

| 시스템                       | 책임                                                                          |
| ------------------------- | --------------------------------------------------------------------------- |
| **SpellSystem**           | 스펠 실행 + 상태 기록(`AddBuff`/`AddDebuff`) + `handler->OnApplied()` 호출            |
| **StatusEffectHandler** ★ | **각 효과의 실행 로직 정의** — OnApplied / ModifyDamage / OnAfterAttack / OnTurnStart |
| **StatusEffectComponent** | 현재 활성 효과 목록 저장 + 쿼리 + 만료 관리                                                 |
| **CombatSystem**          | `handler->ModifyDamage*()` / `handler->OnAfterAttack()` 호출                  |
| **TurnManager**           | 턴 시작 시 `TickDown` + `handler->OnTurnStart()` 호출                             |
| **Strategy**              | 상태 쿼리만 (`actor->HasBuff("Frenzy")`) — 직접 변경 없음                              |

> ★ **신규 클래스**: `StateComponents/StatusEffectHandler.h/cpp`

### 호출 체인

```
SpellSystem::CastSpell
  → SpellSystem::ApplySpellEffect
      → 효과 적용 (피해/회복 등)
      → character->AddEffect               ← 상태 기록
          → StatusEffectComponent::AddEffect
          → StatusEffectAddedEvent 발행
      → StatusEffectHandler::OnApplied()   ← 즉시 실행 효과 (Purify 등)

TurnManager::StartNextTurn
  → StatusEffectComponent::TickDown        ← 만료 관리
      → StatusEffectRemovedEvent 발행 (만료 시)
  → character->OnTurnStart()               ← RefreshActionPoints() + RefreshSpeed() 내부 호출
  → StatusEffectHandler::OnTurnStart()     ← Exhaustion, Haste 처리 (복원 후)

CombatSystem::ExecuteAttack
  → StatusEffectHandler::ModifyDamageDealt()   ← Blessing+3, Fear-3, Curse-3, Stealth×2
  → StatusEffectHandler::ModifyDamageTaken()   ← Blessing-3, Curse+3
  → ApplyDamage(target, final_damage)
  → StatusEffectHandler::OnAfterAttack()       ← Lifesteal 회복, Frenzy 디버프, Stealth 소모
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

| 스펠 ID       | 이름 (CSV Name)      | effectName     | 지속 턴 | 상태 기록 위치                     |
| ----------- | ------------------ | -------------- | ---- | ---------------------------- |
| `S_ENH_010` | Bloodlust          | `"Lifesteal"`  | 1    | `AddEffect("Lifesteal", 1)`  |
| `S_ENH_020` | Frenzy             | `"Frenzy"`     | 1    | `AddEffect("Frenzy", 1)`     |
| `S_ENH_060` | Shadow Hide        | `"Stealth"`    | 1    | `AddEffect("Stealth", 1)`    |
| `S_BUF_010` | Divine Shield      | `"Blessing"`   | 3    | `AddEffect("Blessing", 3)`   |
| `S_BUF_020` | Gale Step          | `"Haste"`      | 2    | `AddEffect("Haste", 2)`      |
| `S_ENH_050` | Purify             | `"Purify"`     | 1    | `AddEffect("Purify", 1)`     |
| `S_DEB_020` | Fearful Cry        | `"Fear"`       | 3    | `AddEffect("Fear", 3)`       |
| `S_DEB_010` | Curse of Suffering | `"Curse"`      | 3    | `AddEffect("Curse", 3)`      |
| `S_ATK_040` | Meteor             | `"Exhaustion"` | 1    | `AddEffect("Exhaustion", 1)` |

> **effectName은 Strategy의 `Has()` 인수와 정확히 일치해야 합니다.**
> `Has(actor, "Blessing")` → `actor->Has("Blessing")` → `StatusEffectComponent::Has("Blessing")`

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
};

class StatusEffectComponent : public CS230::Component
{
public:
    void Update(double dt) override { (void)dt; }

    // --- 상태 추가 / 제거 ---
    void AddEffect(const std::string& name, int duration, int magnitude = 0);
    void RemoveEffect(const std::string& name);
    void RemoveAllEffects();

    // --- 상태 쿼리 ---
    bool Has(const std::string& name) const;
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

void StatusEffectComponent::AddEffect(const std::string& name, int duration, int magnitude)
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
    effects_.push_back({ name, duration, magnitude });
}

void StatusEffectComponent::RemoveEffect(const std::string& name)
{
    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(),
                       [&name](const ActiveEffect& e) { return e.name == name; }),
        effects_.end());
}

void StatusEffectComponent::RemoveAllEffects()
{
    effects_.clear();
}

bool StatusEffectComponent::Has(const std::string& name) const
{
    for (const auto& e : effects_)
        if (e.name == name) return true;
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

// --- 효과 쿼리 (Strategy에서 사용) ---
bool Has(const std::string& effect_name) const;

// --- 효과 추가 / 제거 (SpellSystem, CombatSystem, StatusEffectHandler에서 호출) ---
void AddEffect(const std::string& name, int duration, int magnitude = 0);
void RemoveEffect(const std::string& name);
void RemoveAllEffects();
```

**Character.cpp 구현**:

```cpp
// 생성자 내부
AddGOComponent(new StatusEffectComponent());

bool Character::Has(const std::string& effect_name) const
{
    auto* se = GetGOComponent<StatusEffectComponent>();
    return se && se->Has(effect_name);
}

void Character::AddEffect(const std::string& name, int duration, int magnitude)
{
    auto* se = GetGOComponent<StatusEffectComponent>();
    if (!se) return;
    se->AddEffect(name, duration, magnitude);

    auto* bus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
    if (bus)
        bus->Publish(StatusEffectAddedEvent{ this, name, duration, magnitude });
}

void Character::RemoveEffect(const std::string& name)
{
    if (auto* se = GetGOComponent<StatusEffectComponent>())
        se->RemoveEffect(name);
}

void Character::RemoveAllEffects()
{
    if (auto* se = GetGOComponent<StatusEffectComponent>())
        se->RemoveAllEffects();
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

### Task 4: SpellSystem::ApplySpellEffect에 handler->OnApplied() 추가

신형 `ApplySpellEffect`(spell_system.md 소유)의 `AddBuff`/`AddDebuff` 직후, **Task 6의 StatusEffectHandler**에 즉시 실행 효과를 위임합니다.

`spell_system.md`의 상태 효과 블록 전체 (참조용):

```cpp
// spell_system.md의 신형 ApplySpellEffect 내부 — 상태 효과 블록
if (spell.effect_status != "Basic" && spell.effect_duration > 0)
{
    auto* handler    = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();
    Character* se_target = target ? target : caster;

    if (spell.target_type == "Enemies Around Caster" || ...)
    {
        for (auto* c : grid->GetAllCharacters())
        {
            // ... 거리 필터 ...
            c->AddEffect(spell.effect_status, spell.effect_duration);
            if (handler) handler->OnApplied(c, spell.effect_status); // ← 즉시 실행
        }
    }
    else if (se_target)
    {
        se_target->AddEffect(spell.effect_status, spell.effect_duration);
        if (handler) handler->OnApplied(se_target, spell.effect_status); // ← 즉시 실행
    }
}
```

> `OnApplied`는 현재 **Purify** 한 가지에만 즉시 로직이 있습니다.
> 다른 효과는 `OnApplied` 내에서 아무것도 하지 않고 통과합니다 (등록은 이미 `AddEffect`에서 완료).

StatusEffectComponent(Task 1) · Character facade(Task 2) · StatusEffectHandler(Task 6)가 완성되면 이 호출이 즉시 동작합니다.

---

### Task 5: TurnManager::StartNextTurn에 TickDown + handler→OnTurnStart 추가

**수정 파일**: `TurnManager.cpp`

현재 실제 코드의 `StartNextTurn()` 흐름:

```
죽은 캐릭터 스킵 → currentChar->OnTurnStart() → PublishTurnStartEvent()
```

`Character::OnTurnStart()` 내부에서 `RefreshActionPoints()` + `stats->RefreshSpeed()`가 호출됩니다.
`TurnManager`에서 직접 호출하지 않습니다.

추가할 위치:

1. `currentChar->OnTurnStart()` **이전** — TickDown (만료된 효과 먼저 정리)
2. `currentChar->OnTurnStart()` **이후** — handler->OnTurnStart() (AP/Speed 복원 뒤에 적용)

```cpp
void TurnManager::StartNextTurn()
{
    // ... 죽은 캐릭터 스킵 (기존) ...
    Character* currentChar = turnOrder[currentTurnIndex];

    // ── 신규: TickDown — 상태 효과 지속시간 감소 + 만료 제거 ──
    auto* se = currentChar->GetGOComponent<StatusEffectComponent>();
    if (se) se->TickDown(currentChar, eventBus);

    // ── 기존: AP/Speed 기본값 복원 (내부적으로 RefreshActionPoints + RefreshSpeed 호출) ──
    currentChar->OnTurnStart();

    // ── 신규: Exhaustion/Haste 효과 적용 (복원 후) ──
    //    Exhaustion → 복원된 AP를 0으로 덮어씀
    //    Haste     → 복원된 AP에 +1 추가
    auto* handler = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();
    if (handler) handler->OnTurnStart(currentChar);

    // ── 기존: TurnStartedEvent 발행 ──
    PublishTurnStartEvent();
}
```

> **순서 중요**: `TickDown` → `OnTurnStart(RefreshActionPoints)` → `handler->OnTurnStart()` 순서여야 합니다.
> `handler->OnTurnStart`를 `OnTurnStart()` 이전에 호출하면 `RefreshActionPoints`가 Haste/Exhaustion 효과를 덮어씁니다.

---

### Task 6: StatusEffectHandler 신규 생성

각 status effect의 **실행 로직**을 한 곳에 모아둡니다.
다른 시스템은 "언제 실행할지"만 알고, "어떻게 실행할지"는 이 클래스가 정의합니다.

**신규 파일**: `StateComponents/StatusEffectHandler.h`

```cpp
#pragma once
#include "./Engine/Component.h"
#include <string>

class Character;
class DiceManager;

/**
 * @brief 각 status effect의 실행 로직 중앙 집중 관리
 *
 * 타이밍별 훅:
 *   SpellSystem::ApplySpellEffect  → OnApplied()           즉시 실행 효과
 *   CombatSystem::ExecuteAttack    → ModifyDamage*()       피해 보정
 *                                  → OnAfterAttack()       공격 후 효과
 *   TurnManager::StartNextTurn     → OnTurnStart()         턴 시작 효과
 */
class StatusEffectHandler : public CS230::Component
{
public:
    void SetDiceManager(DiceManager* dice) { diceManager_ = dice; }

    // SpellSystem::ApplySpellEffect에서 AddBuff/AddDebuff 직후 호출
    // 현재: Purify → 즉시 모든 디버프 제거
    void OnApplied(Character* target, const std::string& effect_name);

    // CombatSystem::ExecuteAttack — 피해 계산 단계에서 호출
    int ModifyDamageDealt(Character* attacker, int base_damage) const;
    int ModifyDamageTaken(Character* defender, int base_damage) const;

    // CombatSystem::ExecuteAttack — ApplyDamage 직후 호출
    void OnAfterAttack(Character* attacker, Character* defender, int damage_dealt);

    // TurnManager::StartNextTurn — RefreshActionPoints 직후 호출
    void OnTurnStart(Character* character);

    // 타겟 가능 여부 체크 — PlayerInputHandler/GridSystem에서 호출
    // Stealth 중인 캐릭터는 타겟 불가
    bool IsTargetable(const Character* target) const;

    // 알려진 효과 이름 목록 (status_effect.csv 파싱 대체)
    // 새 효과 추가 시 KNOWN_EFFECTS와 실행 로직을 함께 업데이트
    static bool IsKnownEffect(const std::string& name);

private:
    DiceManager* diceManager_ = nullptr;

    static const std::set<std::string> KNOWN_EFFECTS;
};
```

**신규 파일**: `StateComponents/StatusEffectHandler.cpp`

```cpp
#include "pch.h"
#include "StatusEffectHandler.h"
#include "../Objects/Character.h"
#include "DiceManager.h"
#include <set>

// ──────────────────────────────────────────────
// KNOWN_EFFECTS: 알려진 효과 이름 목록 (status_effect.csv의 NAME 컬럼과 동일)
// status_effect.csv 파싱을 대체한다 — 새 효과 추가 시 이 목록도 업데이트
// ──────────────────────────────────────────────
const std::set<std::string> StatusEffectHandler::KNOWN_EFFECTS = {
    "Lifesteal", "Frenzy",  "Exhaustion", "Purify",
    "Blessing",  "Curse",   "Haste",      "Stealth", "Fear"
};

bool StatusEffectHandler::IsKnownEffect(const std::string& name)
{
    return KNOWN_EFFECTS.count(name) > 0;
}

// ──────────────────────────────────────────────
// OnApplied: AddEffect 직후 즉시 실행
// ──────────────────────────────────────────────
void StatusEffectHandler::OnApplied(Character* target, const std::string& effect_name)
{
    // Purify: 즉시 모든 효과 제거
    if (effect_name == "Purify")
        target->RemoveAllEffects();

    // 나머지 효과는 StatusEffectComponent에 등록만 됨
    // (실행은 ModifyDamage* / OnAfterAttack / OnTurnStart 타이밍에 발생)
}

// ──────────────────────────────────────────────
// ModifyDamageDealt: 공격자 측 피해 보정
// ──────────────────────────────────────────────
int StatusEffectHandler::ModifyDamageDealt(Character* attacker, int base_damage) const
{
    int damage = base_damage;
    if (attacker->Has("Blessing"))  damage += 3;  // Blessing: 피해 +3
    if (attacker->Has("Fear"))      damage -= 3;  // Fear: 피해 -3
    if (attacker->Has("Curse"))     damage -= 3;  // Curse: 피해 -3
    if (attacker->Has("Stealth"))   damage *= 2;  // Stealth: 첫 공격 2배 (평탄 보정 후)
    return std::max(0, damage);
}

// ──────────────────────────────────────────────
// ModifyDamageTaken: 수비자 측 피해 보정
// ──────────────────────────────────────────────
int StatusEffectHandler::ModifyDamageTaken(Character* defender, int base_damage) const
{
    int damage = base_damage;
    if (defender->Has("Blessing"))  damage -= 3;  // Blessing: 피해 감소 -3
    if (defender->Has("Curse"))     damage += 3;  // Curse: 피해 증가 +3
    return std::max(0, damage);
}

// ──────────────────────────────────────────────
// OnAfterAttack: ApplyDamage 이후 트리거
// ──────────────────────────────────────────────
void StatusEffectHandler::OnAfterAttack(Character* attacker, Character* defender, int damage_dealt)
{
    // ── Stealth: 공격 즉시 소모 ──
    if (attacker->Has("Stealth"))
        attacker->RemoveEffect("Stealth");

    // ── Lifesteal: 피해의 50% 회복 (내림) ──
    if (attacker->Has("Lifesteal"))
    {
        int heal = damage_dealt / 2;
        attacker->SetHP(std::min(attacker->GetHP() + heal, attacker->GetMaxHP()));
    }

    // ── Frenzy: 10 이상 피해 → 타겟에 무작위 효과, 미만 → 자신에게, 소모 ──
    if (attacker->Has("Frenzy"))
    {
        attacker->RemoveEffect("Frenzy");

        // 무작위 부정 효과: Curse(0) / Fear(1) / Exhaustion(2)
        static const std::string FRENZY_EFFECTS[] = { "Curse", "Fear", "Exhaustion" };
        int roll = diceManager_ ? (diceManager_->Roll("1d3") - 1) : 0;
        const std::string& effect = FRENZY_EFFECTS[roll];

        if (damage_dealt >= 10)
            defender->AddEffect(effect, 1);
        else
            attacker->AddEffect(effect, 1);
    }
}

// ──────────────────────────────────────────────
// OnTurnStart: RefreshActionPoints 이후 호출
// ──────────────────────────────────────────────
void StatusEffectHandler::OnTurnStart(Character* character)
{
    // ── Exhaustion: 속도/AP를 0으로 (RefreshActionPoints가 복원한 값을 덮어씀) ──
    if (character->Has("Exhaustion"))
    {
        character->SetActionPoints(0);
        // TODO: character->SetMovementRange(0) — Character::SetMovementRange() 구현 필요
    }

    // ── Haste: 행동 +1, 속도 +1 (RefreshActionPoints가 복원한 값에 추가) ──
    if (character->Has("Haste"))
    {
        character->SetActionPoints(character->GetActionPoints() + 1);
        // TODO: character->SetMovementRange(character->GetMovementRange() + 1) — 구현 필요
    }
}

// ──────────────────────────────────────────────
// IsTargetable: Stealth 중 타겟 불가
// ──────────────────────────────────────────────
bool StatusEffectHandler::IsTargetable(const Character* target) const
{
    return !target->Has("Stealth");
}
```

**GamePlay::Load() 추가 필요**:

```cpp
// GamePlay::Load() 내 AddGSComponent 목록에 추가
AddGSComponent(new StatusEffectHandler());
// ...
GetGSComponent<StatusEffectHandler>()->SetDiceManager(GetGSComponent<DiceManager>());
```

---

### Task 7: CombatSystem에서 StatusEffectHandler 위임

**수정 파일**: `CombatSystem.cpp`의 `ExecuteAttack`

CombatSystem은 직접 효과 로직을 알지 않습니다. `StatusEffectHandler`에 위임합니다.

```cpp
void CombatSystem::ExecuteAttack(Character* attacker, Character* target)
{
    auto* handler = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();

    // 1. 기본 피해 계산 (주사위)
    int base_damage = RollAttackDamage(attacker->GetAttackDice(), attacker->GetBaseDamage());

    // 2. 상태 효과 피해 보정 (Blessing, Fear, Curse, Stealth)
    if (handler)
    {
        base_damage = handler->ModifyDamageDealt(attacker, base_damage);
        base_damage = handler->ModifyDamageTaken(target, base_damage);
    }

    // 3. 피해 적용
    ApplyDamage(target, base_damage);
    attacker->SetHasAttackedThisTurn(true);

    // 4. 공격 후 효과 (Lifesteal 회복, Frenzy 디버프, Stealth 소모)
    if (handler)
        handler->OnAfterAttack(attacker, target, base_damage);
}
```

> **Stealth 타겟팅**: Stealth 중인 캐릭터는 타겟 선택 단계에서 제외해야 합니다.
> `PlayerInputHandler` 또는 `GridSystem`에서 타겟 후보를 필터링할 때:
> 
> ```cpp
> if (handler && !handler->IsTargetable(candidate)) continue;
> ```

---

## stub 해제 목록

| 파일                    | stub 메서드                      | 해제 후                                  |
| --------------------- | ----------------------------- | ------------------------------------- |
| `FighterStrategy.cpp` | `HasBuff(actor, "Lifesteal")` | `return actor->Has("Lifesteal")`      |
| `FighterStrategy.cpp` | `HasBuff(actor, "Blessing")`  | `return actor->Has("Blessing")`       |
| `FighterStrategy.cpp` | `HasBuff(actor, "Frenzy")`    | `return actor->Has("Frenzy")`         |
| `FighterStrategy.cpp` | `IsFearActive(dragon)`        | `return dragon->Has("Fear")`          |
| `RogueStrategy.cpp`   | `IsInStealth(actor)`          | `return actor->Has("Stealth")`        |
| `RogueStrategy.cpp`   | `HasBuff(actor, "Haste")`     | `return actor->Has("Haste")`          |
| `RogueStrategy.cpp`   | `HasAttackedThisTurn(actor)`  | `return actor->HasAttackedThisTurn()` |

---

## Rigorous Testing

### 테스트 1: AddEffect → Has 즉시 확인

```cpp
fighter->AddEffect("Blessing", 3);
ASSERT(fighter->Has("Blessing"), "Has after AddEffect");
```

### 테스트 2: TickDown — 지속시간 1 → 만료

```cpp
dragon->AddEffect("Fear", 1);
auto* se = dragon->GetGOComponent<StatusEffectComponent>();
se->TickDown(dragon, eventBus);
ASSERT(!dragon->Has("Fear"), "Fear expired after 1 tick");
```

### 테스트 3: TickDown — 지속시간 3 → 2회 후 유지

```cpp
fighter->AddEffect("Blessing", 3);
auto* se = fighter->GetGOComponent<StatusEffectComponent>();
se->TickDown(fighter, eventBus);
se->TickDown(fighter, eventBus);
ASSERT(fighter->Has("Blessing"), "Blessing still active after 2 ticks");
se->TickDown(fighter, eventBus);
ASSERT(!fighter->Has("Blessing"), "Blessing expired after 3 ticks");
```

### 테스트 4: RemoveAllEffects (Purify)

```cpp
fighter->AddEffect("Fear", 3);
fighter->AddEffect("Curse", 2);
fighter->AddEffect("Blessing", 3);
fighter->RemoveAllEffects();
ASSERT(!fighter->Has("Fear"),    "Fear removed");
ASSERT(!fighter->Has("Curse"),   "Curse removed");
ASSERT(!fighter->Has("Blessing"),"Blessing also removed (RemoveAllEffects는 전부 제거)");
```

### 테스트 5: HasAttackedThisTurn — 공격 후 설정, 턴 시작 시 리셋

```cpp
ASSERT(!rogue->HasAttackedThisTurn(), "False at turn start");
combat->ExecuteAttack(rogue, dragon);
ASSERT(rogue->HasAttackedThisTurn(), "True after attack");
rogue->RefreshActionPoints();
ASSERT(!rogue->HasAttackedThisTurn(), "Reset on turn start");
```

### 테스트 6: 중복 AddEffect → 지속시간 갱신

```cpp
fighter->AddEffect("Blessing", 1);
fighter->AddEffect("Blessing", 3);
auto* se = fighter->GetGOComponent<StatusEffectComponent>();
se->TickDown(fighter, eventBus);
se->TickDown(fighter, eventBus);
ASSERT(fighter->Has("Blessing"), "Duration refreshed to 3");
```

### 테스트 7: StatusEffectAddedEvent 발행 확인

```cpp
bool received = false;
eventBus->Subscribe<StatusEffectAddedEvent>([&](const StatusEffectAddedEvent& e) {
    if (e.effectName == "Blessing" && e.target == fighter) received = true;
});
fighter->AddEffect("Blessing", 3);
ASSERT(received, "Event published on AddEffect");
```

### 테스트 8: StatusEffectRemovedEvent 발행 확인

```cpp
bool removed = false;
eventBus->Subscribe<StatusEffectRemovedEvent>([&](const StatusEffectRemovedEvent& e) {
    if (e.effectName == "Fear" && e.reason == "expired") removed = true;
});
dragon->AddEffect("Fear", 1);
dragon->GetGOComponent<StatusEffectComponent>()->TickDown(dragon, eventBus);
ASSERT(removed, "Event published on expiry");
```

---

### StatusEffectHandler 테스트

```cpp
StatusEffectHandler handler;
handler.SetDiceManager(GetGSComponent<DiceManager>());
```

### 테스트 9: OnApplied("Purify") — 즉시 모든 효과 제거

```cpp
fighter->AddEffect("Fear", 3);
fighter->AddEffect("Curse", 2);
fighter->AddEffect("Blessing", 3);
handler.OnApplied(fighter, "Purify");
ASSERT(!fighter->Has("Fear"),    "Purify removes Fear");
ASSERT(!fighter->Has("Curse"),   "Purify removes Curse");
ASSERT(!fighter->Has("Blessing"),"Purify removes all effects (버프/디버프 구분 없음)");
```

### 테스트 10: ModifyDamageDealt — Blessing/Fear/Stealth 보정

```cpp
// Blessing: +3
fighter->AddEffect("Blessing", 3);
ASSERT(handler.ModifyDamageDealt(fighter, 10) == 13, "Blessing adds 3 to dealt");
fighter->RemoveEffect("Blessing");

// Fear: -3
fighter->AddEffect("Fear", 3);
ASSERT(handler.ModifyDamageDealt(fighter, 10) == 7, "Fear removes 3 from dealt");
fighter->RemoveEffect("Fear");

// Stealth: ×2 (평탄 보정 후 마지막 적용)
fighter->AddEffect("Stealth", 1);
ASSERT(handler.ModifyDamageDealt(fighter, 8) == 16, "Stealth doubles damage");
```

### 테스트 11: OnAfterAttack — Lifesteal 회복

```cpp
fighter->AddEffect("Lifesteal", 1);
int hp_before = fighter->GetHP();
handler.OnAfterAttack(fighter, dragon, 10);  // 10 피해, 회복 = 5
ASSERT(fighter->GetHP() == hp_before + 5, "Lifesteal heals 50% of damage");
ASSERT(fighter->Has("Lifesteal"), "Lifesteal is NOT removed after attack");
// (Lifesteal은 지속시간이 만료될 때까지 유지, TickDown에서 제거)
```

### 테스트 12: OnTurnStart — Exhaustion

```cpp
dragon->AddEffect("Exhaustion", 1);
dragon->RefreshActionPoints();          // AP = 기본값(예: 2)으로 복원
handler.OnTurnStart(dragon);
ASSERT(dragon->GetActionPoints() == 0, "Exhaustion sets AP to 0");
```

### 테스트 13: OnTurnStart — Haste

```cpp
fighter->AddEffect("Haste", 2);
fighter->RefreshActionPoints();         // AP = 기본값(예: 2)으로 복원
handler.OnTurnStart(fighter);
ASSERT(fighter->GetActionPoints() == 3, "Haste adds 1 to AP");
```

---

## 구현 순서

```
1. StatusEffectComponent.h/cpp 신규 생성           (Task 1)
2. Character.h/cpp에 컴포넌트 등록 + facade 추가   (Task 2)
3. Character.h/cpp에 has_attacked_this_turn_ 추가  (Task 3)
4. StatusEffectHandler.h/cpp 신규 생성             (Task 6)
5. GamePlay::Load()에 StatusEffectHandler 등록
6. TurnManager::StartNextTurn에 TickDown
         + RefreshActionPoints 순서 확인
         + OnTurnStart 추가                        (Task 5)
7. CombatSystem::ExecuteAttack — handler 위임 교체  (Task 7)
8. spell_system.md의 신형 ApplySpellEffect에
         handler->OnApplied() 추가                 (Task 4)
9. FighterStrategy / RogueStrategy stub 해제
10. cmake --preset windows-debug 재실행            (신규 파일 StatusEffectHandler 감지)
11. 테스트 실행
```

> **⚠️ 10번(CMake 재실행) 전에 빌드하면 StatusEffectHandler / StatusEffectComponent를 찾지 못합니다.**

### 각 effect 실행 위치 요약

| effect     | 타입  | 실행 위치               | 훅                                     |
| ---------- | --- | ------------------- | ------------------------------------- |
| Purify     | 버프  | 스펠 시전 즉시            | `OnApplied`                           |
| Blessing   | 버프  | 피해 계산 시             | `ModifyDamageDealt/Taken`             |
| Curse      | 디버프 | 피해 계산 시             | `ModifyDamageDealt/Taken`             |
| Fear       | 디버프 | 피해 계산 시 (공격자 측)     | `ModifyDamageDealt`                   |
| Stealth    | 버프  | 피해 계산(2배) + 공격 후 소모 | `ModifyDamageDealt` + `OnAfterAttack` |
| Lifesteal  | 버프  | 공격 후 회복             | `OnAfterAttack`                       |
| Frenzy     | 버프  | 공격 후 무작위 디버프 + 소모   | `OnAfterAttack`                       |
| Exhaustion | 디버프 | 턴 시작 (AP/Speed = 0) | `OnTurnStart`                         |
| Haste      | 버프  | 턴 시작 (+1 AP/Speed)  | `OnTurnStart`                         |
