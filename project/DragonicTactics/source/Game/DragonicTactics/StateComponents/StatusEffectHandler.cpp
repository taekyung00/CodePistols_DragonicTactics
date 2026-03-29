#include "StatusEffectHandler.h"
#include "../Objects/Character.h"
#include "DiceManager.h"
#include "pch.h"
// ──────────────────────────────────────────────
// KNOWN_EFFECTS: 알려진 효과 이름 목록 (status_effect.csv의 NAME 컬럼과 동일)
// status_effect.csv 파싱을 대체한다 — 새 효과 추가 시 이 목록도 업데이트
// ──────────────────────────────────────────────
const std::pair<std::string, std::string> StatusEffectHandler::KNOWN_EFFECTS[NUM_EFFECTS] = {
  {	"Lifesteal", "Recover 50% of damage dealt this turn (round down)" },
  {	 "Frenzy",	  "If next attack deals 10+ damage, target receives random debuff (Curse, Fear, Exhaustion); otherwise, the Fighter receives it." },
  { "Exhaustion",						  "Speed and Action points become 0 next turn" },
  {	 "Purify",						"Removes all status effects from self" },
  {	"Blessing",						 "All damage taken -3, all damage dealt +3" },
  {	  "Curse",						 "All damage taken +3, all damage dealt -3" },
  {	  "Haste",						"Speed +1, Actions +1" },
  {	"Stealth",					  "Untargetable. The first damage dealt next turn is doubled. Cannot be used after attacking. Stealth is removed upon attacking." },
  {	   "Fear",						 "all damage dealt -3, speed -1" }
};

bool StatusEffectHandler::IsKnownEffect(const std::string& name)
{
    for(const auto& effect : KNOWN_EFFECTS)
    {
        if (effect.first == name)
            return true;
    }
  return false;
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
  if (attacker->Has("Blessing"))
	damage += 3; // Blessing: 피해 +3
  if (attacker->Has("Fear"))
	damage -= 3; // Fear: 피해 -3
  if (attacker->Has("Curse"))
	damage -= 3; // Curse: 피해 -3
  if (attacker->Has("Stealth"))
	damage *= 2; // Stealth: 첫 공격 2배 (평탄 보정 후)
  return std::max(0, damage);
}

// ──────────────────────────────────────────────
// ModifyDamageTaken: 수비자 측 피해 보정
// ──────────────────────────────────────────────
int StatusEffectHandler::ModifyDamageTaken(Character* defender, int base_damage) const
{
  int damage = base_damage;
  if (defender->Has("Blessing"))
	damage -= 3; // Blessing: 피해 감소 -3
  if (defender->Has("Curse"))
	damage += 3; // Curse: 피해 증가 +3
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
	int						 roll			  = Engine::GetGameStateManager().GetGSComponent<DiceManager>()->RollDice(1,3) - 1; // 0~2
	const std::string&		 effect			  = FRENZY_EFFECTS[roll];

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
bool StatusEffectHandler::IsTargetable(Character* target) 
{
  return !target->Has("Stealth");
}