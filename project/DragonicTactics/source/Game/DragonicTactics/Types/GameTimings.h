/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameTimings.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    June 9, 2026
*/
#pragma once

// 전투 타이밍 상수 집중 관리 파일
// 수정 시 이 파일만 변경하면 된다.
//
// 의존 관계:
//   AI_THINK          → BattleOrchestrator (m_think_timer_ 초기값)
//   AI_WAIT_SPELL     → BattleOrchestrator (UseAbility 후 m_wait_timer)
//   AI_WAIT_ATTACK    → BattleOrchestrator (Attack 후 m_wait_timer)
//   AI_WAIT_MOVE      → BattleOrchestrator (Move 후 m_wait_timer)
//   ATTACK_APPLY      → CombatSystem (AttackDelayObject 지연) + GamePlay (AI_HIT_DELAY)
//   SPELL_APPLY       → SpellSystem (SpellDelayObject 지연)   + GamePlay (SPELL_APPLY_SEC)
//   EFFECT_LEAD       → GamePlay (EFFECT_LEAD_TIME — SFX 종료 직전 이펙트 등장)
//   SPELL_PRE_IMPACT  → GamePlay (SPELL_PRE_IMPACT_SEC — 스펠 SFX 침묵+딜레이 합산)
//   PROJ_DURATION     → GamePlay (투사체 이동 시간)
//   MOVE_PER_TILE     → MovementComponent (타일당 이동 시간)

struct GameTimings
{
	// ── AI 행동 간격 ────────────────────────────────────────────────────────
	static constexpr double AI_THINK      = 1.2; // 턴 시작 "생각 중" 말풍선 시간 (초)
	static constexpr double AI_WAIT_SPELL = 0.6; // 스펠 시전 후 다음 MakeDecision까지 대기
	static constexpr double AI_WAIT_ATTACK= 0.6; // 공격 후 대기 (ATTACK_APPLY + 여유 0.3s)
	static constexpr double AI_WAIT_MOVE  = 0.3; // 이동 후 시각적 간격

	// ── 딜레이 오브젝트 발화 시각 ───────────────────────────────────────────
	static constexpr double ATTACK_APPLY  = 0.3; // AttackDelayObject: AI 공격 → 데미지 적용
	static constexpr double SPELL_APPLY   = 0.5; // SpellDelayObject: 스펠 시전 → 효과 적용

	// ── 이펙트 타이밍 ────────────────────────────────────────────────────────
	// SPELL_PRE_IMPACT = SPELL_APPLY(0.5) + SFX 앞부분 침묵(~1.0)
	// CharacterDamagedEvent 데미지 이펙트 딜레이: max(0, sfx_dur - SPELL_PRE_IMPACT - EFFECT_LEAD)
	static constexpr double SPELL_PRE_IMPACT = 1.5;
	static constexpr double EFFECT_LEAD      = 0.3; // SFX 종료 이전에 이펙트가 등장할 리드 타임

	// ── 비주얼 이펙트 ────────────────────────────────────────────────────────
	static constexpr double PROJ_DURATION    = 0.4; // 투사체(Fire Bolt 등) 이동 시간 (초)
	static constexpr double SLOW_SPELL_EXTRA = 0.8; // Fire/MagicMissile/Meteor SFX 스펠 추가 딜레이

	// ── 캐릭터 이동 ──────────────────────────────────────────────────────────
	static constexpr double MOVE_PER_TILE = 0.2; // 타일 1칸 이동 애니메이션 시간 (초)
};
