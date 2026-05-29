# Crash / Abort 위험 지점 분석

조사 범위: `DragonicTactics/source/Game/DragonicTactics/`

---

## 🔴 CRITICAL — 조건 충족 시 반드시 crash

### 1. `CombatSystem.cpp:29` — null 체크 후 즉시 역참조

```cpp
if (attacker == nullptr || defender == nullptr)
{
    Engine::GetLogger().LogError("CombatSystem: Null "
        + attacker->TypeName()     // ← attacker가 null이면 여기서 crash
        + " or " + defender->TypeName()); // ← defender가 null이면 여기서 crash
    return 0;
}
```

null을 잡아내려 했지만, 에러 메시지 생성에서 null을 역참조하는 패턴.  
`CombatSystem::CalculateDamage`가 null 인자로 호출되는 순간 확정 crash.

**수정 방법:**
```cpp
if (attacker == nullptr || defender == nullptr)
{
    Engine::GetLogger().LogError("CombatSystem: Null attacker or defender in CalculateDamage");
    return 0;
}
```

---

### 2. `CombatSystem.cpp:184` — DiceManager nullptr 체크 없음

```cpp
int CombatSystem::RollAttackDamage(const std::string& damageDice, int baseDamage)
{
    int diceRoll = Engine::GetGameStateManager()
        .GetGSComponent<DiceManager>()     // ← nullptr 가능
        ->RollDiceFromString(damageDice);  // ← nullptr이면 crash
    return diceRoll + baseDamage;
}
```

`CalculateDamage`(줄 35–47)에는 `if (diceManager)` 분기가 있지만 `RollAttackDamage`에는 없음.

**수정 방법:**
```cpp
auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
int diceRoll = dice ? dice->RollDiceFromString(damageDice) : 0;
```

---

## 🟠 HIGH — 실제 트리거 가능성 있음

### 3. `StatusEffectHandler.cpp:144` — Frenzy 발동 시 DiceManager nullptr

```cpp
int roll = Engine::GetGameStateManager()
    .GetGSComponent<DiceManager>()  // ← nullptr 체크 없음
    ->RollDice(1, 2) - 1;
```

Fighter가 10 이상 피해를 입힐 때마다 실행되는 경로.  
같은 파일의 Lifesteal 경로는 `if (eventBus)` 체크를 하지만 이 줄은 체크 없음.

**수정 방법:**
```cpp
auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
int roll = dice ? dice->RollDice(1, 2) - 1 : 0;
```

---

### 4. `SpellSystem.cpp:314, 350` — upcast_dice stoi 예외 미처리

```cpp
int per_level = (d_pos > 0)
    ? std::stoi(spell.upcast_dice.substr(0, d_pos))  // ← try/catch 없음
    : 1;
```

`d_pos > 0` 체크만 있고 `substr` 결과가 숫자인지 보장 없음.  
`spell_table.csv`에서 upcast dice 필드가 `"1d6"` 형태가 아닌 경우 `std::invalid_argument` → terminate.

---

### 5. `SpellSystem.cpp:326` — flat_per_level multiplier stoi 예외 미처리

```cpp
if (spell.damage_formula.rfind("flat_per_level:", 0) == 0)
{
    int multiplier = std::stoi(spell.damage_formula.substr(15));  // ← try/catch 없음
```

`"flat_per_level:"` 이 15자이므로 `substr(15)` 이후가 빈 문자열이거나 숫자가 아니면 예외.  
CSV에서 `flat_per_level:` 만 쓰고 숫자를 빠뜨리면 즉시 crash.

---

### 6. `SpellSystem.cpp:833` — ParseMoveField stoi 예외 미처리

```cpp
m.distance = std::stoi(parts[2]);  // ← try/catch 없음
```

Move 필드 `"target:knockback:2"` 파싱 시 parts[2]가 숫자가 아니면 예외.  
CSV를 수작업으로 편집할 때 트리거.

---

## 🟡 MEDIUM — 특정 상황에서만 발생

### 7. `DebugManager.cpp` — 디버그 콘솔 stoi 예외 미처리

```
줄 419:  ch->SetHP(std::stoi(args[1]));
줄 431:  ch->SetActionPoints(std::stoi(args[1]));
줄 467:  ch->AddEffect(args[1], std::stoi(args[2]));
줄 482:  slots->RestoreOne(std::stoi(args[1]));
줄 516:  slots->Consume(std::stoi(args[1]));
```

콘솔에서 `sethp abc`처럼 숫자가 아닌 값 입력 시 `std::invalid_argument` 예외 → crash.  
**DEVELOPER_VERSION 빌드에서만 노출** — 릴리즈에는 포함되지 않음.

---

### 8. `DataRegistry.cpp:266` — JSON spell_slots stoi 예외 미처리

```cpp
int level = std::stoi(levelStr);  // ← try/catch 없음
```

`characters.json`의 spell_slots 키(`"1"`, `"2"` 등)가 숫자 문자열이 아닌 경우 예외.  
JSON 파일이 정상이면 안전하지만 수동 편집 시 위험.

---

## 🟢 LOW — 이론적 위험, 실제 발생 가능성 낮음

### 9. AI Strategy 전체 — GridSystem nullptr 가드 없음

```cpp
// FighterStrategy.cpp, ClericStrategy.cpp, RogueStrategy.cpp 동일 패턴
GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
// nullptr 체크 없이 바로 grid->GetAllCharacters() 등 사용
```

`GamePlay::Load()`에서 GridSystem이 항상 먼저 등록되므로 정상 실행 중에는 안전.  
테스트나 비정상적인 GameState 순서에서만 crash 가능.

---

## 정리표

| 파일 | 줄 | 문제 | 심각도 |
|---|---|---|---|
| `StateComponents/CombatSystem.cpp` | 29 | null 체크 후 즉시 역참조 | 🔴 CRITICAL |
| `StateComponents/CombatSystem.cpp` | 184 | DiceManager nullptr 체크 없음 | 🔴 CRITICAL |
| `StateComponents/StatusEffectHandler.cpp` | 144 | Frenzy DiceManager nullptr | 🟠 HIGH |
| `StateComponents/SpellSystem.cpp` | 314, 350 | upcast_dice stoi 예외 미처리 | 🟠 HIGH |
| `StateComponents/SpellSystem.cpp` | 326 | flat_per_level stoi 예외 미처리 | 🟠 HIGH |
| `StateComponents/SpellSystem.cpp` | 833 | move distance stoi 예외 미처리 | 🟠 HIGH |
| `Debugger/DebugManager.cpp` | 419,431,467,482,516 | 콘솔 입력 stoi 예외 미처리 | 🟡 MEDIUM |
| `StateComponents/DataRegistry.cpp` | 266 | JSON spell_slots stoi 예외 미처리 | 🟡 MEDIUM |
| `StateComponents/AI/*.cpp` | — | GridSystem nullptr 가드 없음 | 🟢 LOW |

> **우선순위**: `CombatSystem.cpp:29`는 null 체크의 의도를 완전히 반대로 구현한 케이스.  
> 지금 당장 런타임에서 hit할 수 있으므로 가장 먼저 수정 필요.
