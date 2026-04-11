# Fighter 스펠 체크 + Dragon Purify 수정

**프로젝트**: Dragonic Tactics - 턴제 전술 RPG  
**기능**: Fighter 스펠 동작 검증 + FighterStrategy 통합 확인 + Dragon Purify 버그 수정  
**작성일**: 2026-04-07

**관련 파일**:

- [spell_table.csv](../../../DragonicTactics/Assets/Data/spell_table.csv)
- [StatusEffectHandler.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/StatusEffectHandler.cpp)
- [FighterStrategy.cpp](../../../DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/FighterStrategy.cpp)
- [fighter_strategy.md](./fighter_strategy.md)
- [spell_system.md](./spell_system.md)

---

## 개요

TODO 항목 3개를 분석한 결과, 모두 `spell_table.csv`의 버그 2개로 귀결된다.

| TODO 항목 | 실제 문제 | 수정 대상 |
|---|---|---|
| 파이터 스펠 구현 및 체크 | Smite 효과 필드 오류 (Lifesteal 0턴) | spell_table.csv 라인 3 |
| FighterStrategy와 통합하여 체크 | 위 Smite 버그로 인한 전략 효과 오작동 | spell_table.csv 라인 3 |
| 드래곤 정화 체크 | Purify targeting 오류 (자신이 아닌 범위 정화) | spell_table.csv 라인 72 |

---

## Bug 1: Smite (S_ATK_050) — Lifesteal duration=0

### 증상

`spell_table.csv` 라인 3:
```
Applies "Lifesteal" status for 0 turns.
```

SpellSystem의 상태효과 적용 조건:
```cpp
if (spell.effect_status != "Basic" && spell.effect_duration > 0)  // duration=0 → 조건 실패
```

→ Smite를 시전해도 Lifesteal이 **절대 적용되지 않는다**.

### 원인

Smite는 **순수 피해 스펠**이다. Lifesteal은 Bloodlust(S_ENH_010)가 별도로 부여한다.  
FighterStrategy의 생존 시퀀스 설계:

```
1. Bloodlust 시전 → Lifesteal 1턴 획득
2. Smite 시전     → 강한 피해 + OnAfterAttack에서 Lifesteal 발동 → HP 회복
```

즉 Smite의 `"Lifesteal" for 0 turns`는 의미 없는 잔여 값이다.

### 수정

**파일**: `DragonicTactics/Assets/Data/spell_table.csv` 라인 3

```
// 변경 전
Applies "Lifesteal" status for 0 turns.

// 변경 후
Applies "Basic" status for 0 turns.
```

---

## Bug 2: Purify (S_ENH_050) — targeting Any:Around:4

### 증상

`spell_table.csv` 라인 72:
```
S_ENH_050,Purify,Buff,Dragon,1,Any:Around:4,FALSE,...
```

SpellSystem의 Around 타겟팅 처리:
```cpp
// filter="Any" → 아무것도 거르지 않음
// → 반경 4 이내 모든 캐릭터(Dragon + Fighter)의 상태효과 제거
```

→ Dragon이 Purify를 쓰면 **Fighter의 Fear, Curse 등도 함께 제거**된다.

### 원인

StatusEffectHandler의 KNOWN_EFFECTS 설명:
```cpp
{ "Purify", "Removes all status effects from self" }
```

의도는 **자기 자신의 상태효과만 제거**이다.  
CSV의 `Any:Around:4`는 올바른 targeting이 아니다.

### 수정

**파일**: `DragonicTactics/Assets/Data/spell_table.csv` 라인 72

```
// 변경 전
S_ENH_050,Purify,Buff,Dragon,1,Any:Around:4,FALSE,...

// 변경 후
S_ENH_050,Purify,Buff,Dragon,1,Self:Single:0,FALSE,...
```

> **⚠️ duration=1은 반드시 유지**  
> SpellSystem은 `effect_duration > 0`일 때만 `AddEffect` 및 `OnApplied`를 호출한다.  
> duration=0으로 변경하면 Purify 자체가 동작하지 않는다.  
> Purify는 OnApplied에서 모든 효과를 즉시 제거하므로 duration=1이어도 실질적으로 1턴 유지되지 않는다.

---

## Bug 3: FighterStrategy — HasBuff("Bloodlust") 이름 불일치

### 증상

`FighterStrategy.cpp` 라인 190:
```cpp
if (!HasBuff(actor, "Bloodlust"))
```

그런데 S_ENH_010 (Bloodlust 스펠)이 실제로 부여하는 상태효과:
```
spell_table.csv: Applies "Lifesteal" status for 1 turns.
```

`"Bloodlust"` 효과는 존재하지 않는다 (`StatusEffectHandler::KNOWN_EFFECTS`에 없음).

### 결과

```
1. Fighter가 Bloodlust(S_ENH_010) 시전 → "Lifesteal" 상태 부여됨
2. 다음 MakeDecision 호출: HasBuff(actor, "Bloodlust") → 항상 false
3. HasSpellSlot(2) 있으면 → Bloodlust 또 시전 → 슬롯 소진까지 반복
```

생존 시퀀스가 피흡 극대화(Bloodlust → Smite 콤보) 대신 **슬롯 낭비**로 동작한다.

### 수정

**파일**: `DragonicTactics/source/Game/DragonicTactics/StateComponents/AI/FighterStrategy.cpp` 라인 190

```cpp
// 변경 전
if (!HasBuff(actor, "Bloodlust"))

// 변경 후
if (!HasBuff(actor, "Lifesteal"))
```

---

## 정상 동작 항목 (수정 불필요)

| 스펠 | ID | 상태 | 근거 |
|---|---|---|---|
| Bloodlust → Lifesteal 1턴 | S_ENH_010 | ✅ 정상 | CSV duration=1, OnAfterAttack: `heal = damage / 2` |
| Frenzy → Frenzy 1턴 → 공격 후 랜덤 디버프 | S_ENH_020 | ✅ 정상 | CSV duration=1, OnAfterAttack: Curse/Fear/Exhaustion 중 1개 |
| Fearful Cry → Fear 3턴 | S_DEB_020 | ✅ 정상 | CSV duration=3, OnApplied: base speed -1 |
| FighterStrategy UseAbility target 설정 | — | ✅ 정상 | Self 스펠=actor, Enemy 스펠=dragon |
| Smite distance≤1 검증 | S_ATK_050 | ✅ 정상 | 모든 Smite 사용처에서 거리 검증 존재 → 무한루프 없음 |
| Fearful Cry Around 스펠 target=actor | S_DEB_020 | ✅ 정상 | CanCast 거리=0으로 항상 통과 |

---

## StatusEffectHandler — Lifesteal/Frenzy 처리 흐름

FighterStrategy가 Bloodlust → 공격 순서로 사용할 때 실제 실행 경로:

```
1. SpellSystem::CastSpell("S_ENH_010")
   → AddEffect("Lifesteal", 1) → OnApplied 없음 (Lifesteal은 즉시 효과 없음)

2. CombatSystem::ExecuteAttack / SpellSystem::CastSpell("S_ATK_050")
   → 피해 적용
   → StatusEffectHandler::OnAfterAttack(attacker=fighter, damage_dealt)
       → Has("Lifesteal") == true → heal = damage / 2 → fighter HP 회복
       [Frenzy도 동일 훅에서 처리: 10+ 피해 → 타겟에 랜덤 디버프, 미만 → 자신에게]
```

---

## Purify — OnApplied 처리 흐름

Dragon이 Purify를 시전할 때 (수정 후 Self:Single:0 기준):

```
1. SpellSystem::CastSpell("S_ENH_050", target=dragon_pos)
   → CanCast: Self:Single:0 → 항상 통과
   → 0.5초 딜레이 후 ApplySpellEffect
   → targets = [dragon] (Self 필터)
   → effect_duration=1 > 0 → AddEffect("Purify", 1) 호출
   → StatusEffectHandler::OnApplied(dragon, "Purify")
       → GetAllEffects() 순회 → 각 효과 OnRemoved 호출 (Fear→speed+1 복원 등)
       → RemoveAllEffects() → 모든 효과 제거 (Purify 포함)
```

---

## 검증 방법

```bash
cd DragonicTactics
cmake --build --preset windows-debug
build/windows-debug/dragonic_tactics.exe
```

1. **Smite 수정 검증**: Fighter AI가 Bloodlust 없이 Smite를 쓸 때 HP가 변하지 않는지 확인
2. **Bloodlust + Smite 콤보**: Fighter가 Bloodlust 시전 → 다음 Smite/공격 후 HP 회복 발생 확인
3. **Purify 수정 검증**:
   - Fighter가 Dragon에게 Fear 적용
   - Dragon이 Purify 시전 → Dragon의 Fear만 제거, Fighter에는 영향 없음 확인
