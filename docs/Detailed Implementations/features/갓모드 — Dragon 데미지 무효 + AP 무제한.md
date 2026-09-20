# 갓모드 — Dragon 데미지 무효 + AP 무제한

## Context

갓모드(데미지 0 + AP 무제한)를 Dragon에만 적용한다. 데미지 차단은 `CombatSystem::ApplyDamage()`에 **이미 구현**되어 있고, DebugManager ImGui 패널에 체크박스도 있다. **미구현**인 부분은 AP 소모 차단 뿐이다.

## 현재 상태 요약

| 기능                       | 구현 여부 | 위치                                                        |
| ------------------------ | ----- | --------------------------------------------------------- |
| God Mode 토글 UI           | ✅ 완료  | `DebugManager.cpp:201` — ImGui Checkbox                   |
| God Mode 플래그             | ✅ 완료  | `DebugManager.h` — `bool god_mode`                        |
| `IsGodModeEnabled()`     | ✅ 완료  | `DebugManager.cpp:401` — `debug_mode && god_mode`         |
| 데미지 차단                   | ✅ 완료  | `CombatSystem.cpp:63-70` — `ApplyDamage()` 내 early return |
| 공격 AP 차단                 | ❌ 미구현 | `CombatSystem.cpp:161` — `ExecuteAttack()` 내 Consume      |
| 스펠 AP 차단 (CastSpell)     | ❌ 미구현 | `SpellSystem.cpp:542`                                     |
| 스펠 AP 차단 (CastWallZones) | ❌ 미구현 | `SpellSystem.cpp:802`                                     |
| 스펠 AP 차단 (CastLavaZones) | ❌ 미구현 | `SpellSystem.cpp:837`                                     |

## 수정 파일 및 변경 내용

### 1. `CombatSystem.cpp` — `ExecuteAttack()` 내 AP 소모 (line 160-161)

```cpp
// 기존
attacker->GetActionPointsComponent()->Consume(attackCost);

// 변경 후
auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
if (!(debug_mgr && debug_mgr->IsGodModeEnabled() && attacker->GetCharacterType() == CharacterTypes::Dragon))
    attacker->GetActionPointsComponent()->Consume(attackCost);
```

- `DebugManager.h`는 이미 include되어 있음 (line 19) — 추가 include 불필요

### 2. `SpellSystem.cpp` — AP 소모 3곳 (lines 542, 802, 837)

**include 추가** (기존 include 블록 끝에):

```cpp
#include "Game/DragonicTactics/Debugger/DebugManager.h"
```

**각 Consume(1) 호출을 동일 패턴으로 교체**:

```cpp
// 기존
caster->GetActionPointsComponent()->Consume(1);

// 변경 후
{
    auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
    if (!(debug_mgr && debug_mgr->IsGodModeEnabled() && caster->GetCharacterType() == CharacterTypes::Dragon))
        caster->GetActionPointsComponent()->Consume(1);
}
```

적용 위치:

- `CastSpell()` line 542
- `CastWallZones()` line 802
- `CastLavaZones()` line 837

## 사용법

1. 게임 실행 후 `F1` → Debug Mode 활성화
2. Debug Tools 패널에서 **God Mode** 체크박스 ON
3. Dragon은 공격받아도 HP 감소 없음, 공격/스펠 사용 후 AP 감소 없음

## 검증

- Dragon AP가 공격 후에도 줄지 않는지 UI에서 확인
- Fighter의 공격을 맞아도 Dragon HP가 줄지 않는지 확인
- Fighter(AI) 턴에는 정상적으로 AP 소모되는지 확인 (god mode는 Dragon에만 적용)
- God Mode OFF 시 AP/데미지 모두 정상 동작 확인
