/**
 * \file
 * \author Sangyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "../StateComponents/DiceManager.h"
#include "../StateComponents/EventBus.h"
#include "../Types/Events.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Logger.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/Debugger/DebugManager.h"
#include "Game/DragonicTactics/StateComponents/StatusEffectHandler.h"
#include "CombatSystem.h"
#include "Engine/GameObjectManager.h"
#include "Game/GameObjectTypes.h"

namespace
{
// SpellDelayObject와 동일한 패턴 — AI 공격 데미지를 N초 뒤에 적용
static constexpr double AI_ATTACK_DELAY = 0.3;

class AttackDelayObject : public CS230::GameObject
{
    double                m_delay;
    std::function<void()> m_callback;

public:
    AttackDelayObject(double delay, std::function<void()> cb)
        : CS230::GameObject({ 0, 0 }), m_delay(delay), m_callback(std::move(cb))
    {
    }
    std::string     TypeName() override { return "AttackDelayObject"; }
    GameObjectTypes Type() override { return static_cast<GameObjectTypes>(0); }
    void            Update(double dt) override
    {
        m_delay -= dt;
        if (m_delay <= 0.0)
        {
            m_callback();
            Destroy();
        }
    }
};
} // namespace

int CombatSystem::CalculateDamage(Character* attacker, Character* defender, const std::string& damageDice, int baseDamage)
{
  if (attacker == nullptr || defender == nullptr)
  {
	Engine::GetLogger().LogError("CombatSystem: Null attacker or defender in CalculateDamage");
	return 0;
  }

  // Roll attack dice
  int diceRoll = 0;
  if (diceManager)
  {
	diceRoll = diceManager->RollDiceFromString(damageDice);
  }
  else
  {
	// Fallback to GameStateManager if no DiceManager was set
	auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
	if (dice)
	{
	  diceRoll = dice->RollDiceFromString(damageDice);
	}
  }
  int totalDamage = diceRoll + baseDamage;
  Engine::GetLogger().LogEvent(
	"CombatSystem: " + attacker->TypeName() + " rolled " + damageDice + " = " + std::to_string(diceRoll) + " + " + std::to_string(baseDamage) + " = " + std::to_string(totalDamage) + " damage");

  return totalDamage;
}

void CombatSystem::ApplyDamage(Character* attacker, Character* defender, int damage)
{
  if (defender == nullptr)
  {
	Engine::GetLogger().LogError("CombatSystem: Null defender in ApplyDamage");
	return;
  }

  // God Mode: Dragon은 데미지 무효
  auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
  if (debug_mgr && debug_mgr->IsGodModeEnabled()
	  && defender->GetCharacterType() == CharacterTypes::Dragon)
  {
	Engine::GetLogger().LogDebug("[GodMode] Damage blocked for Dragon");
	return;
  }

  if (damage < 0)
  {
	Engine::GetLogger().LogError("CombatSystem: Negative damage (" + std::to_string(damage) + ")");
	damage = 0;
  }

  // Apply damage to defender
  auto hpBefore = defender->GetStatsComponent()->GetCurrentHP();
  defender->TakeDamage(damage, attacker);
  int hpAfter = defender->GetStatsComponent()->GetCurrentHP();

  Engine::GetLogger().LogEvent("CombatSystem: " + defender->TypeName() + " took " + std::to_string(damage) + " damage (" + std::to_string(hpBefore) + " -> " + std::to_string(hpAfter) + " HP)");

  // Publish damage event
  auto* eventBus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
  if (eventBus)
  {
	eventBus->Publish(CharacterDamagedEvent{ defender, damage, hpAfter, attacker, !defender->IsAlive() });
  }

  // Check if defender died
  if (!defender->IsAlive())
  {
	Engine::GetLogger().LogEvent("CombatSystem: " + defender->TypeName() + " retired!");
	auto* eventBus2 = Engine::GetGameStateManager().GetGSComponent<EventBus>();
	if (eventBus2)
	{
	  eventBus2->Publish(CharacterDeathEvent{ defender, attacker });
	}
  }
}

bool CombatSystem::ExecuteAttack(Character* attacker, Character* defender)
{
  if (attacker == nullptr || defender == nullptr)
  {
	Engine::GetLogger().LogError("CombatSystem: Null " + attacker->TypeName() + " or " + defender->TypeName());
	return false;
  }

  if (!attacker->IsAlive())
  {
	Engine::GetLogger().LogError("CombatSystem: Dead attacker cannot attack");
	return false;
  }

  if (!defender->IsAlive())
  {
	Engine::GetLogger().LogError("CombatSystem: Cannot attack dead " + defender->TypeName());
	return false;
  }

  // Stealth: 은신 중인 캐릭터는 공격 불가 (알림은 PlayerInputHandler 호버에서 처리)
  if (defender->Has("Stealth"))
  {
	Engine::GetLogger().LogEvent("CombatSystem: Cannot attack " + defender->TypeName() + " (Stealth)");
	return false;
  }

  // Check range
  if (!IsInRange(attacker, defender, attacker->GetStatsComponent()->GetAttackRange()))
  {
	Engine::GetLogger().LogError("CombatSystem: Target out of range");
	return false;
  }

  // Check AP (attack costs 1 AP)
  int attackCost = 1;
  if (attacker->GetActionPoints() < attackCost)
  {
	Engine::GetLogger().LogError("CombatSystem: " + attacker->TypeName() + " has no Action Points to attack!");
	return false;
  }

  auto* handler = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();


    

  // Calculate and apply damage
  int damage = CalculateDamage(attacker, defender, attacker->GetStatsComponent()->GetAttackDice(), attacker->GetStatsComponent()->GetBaseAttack());

  // 상태 효과 피해 보정 (Blessing, Fear, Curse, Stealth)
    if (handler)
    {
        damage = handler->ModifyDamageDealt(attacker, damage);
        damage = handler->ModifyDamageTaken(defender, damage);
    }
  // 공격 SFX 먼저 발행 (action SFX → hurt SFX 순서 보장)
  auto* eventBus = Engine::GetGameStateManager().GetGSComponent<EventBus>();
  if (eventBus)
  {
	eventBus->Publish(CharacterAttackedEvent{ attacker, defender, damage });
  }

  if (attacker->IsAIControlled())
  {
	// AI 공격: 0.3초 후 데미지 적용 → SFX와 이펙트가 함께 등장
	auto* gom = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();
	if (gom)
	{
	  gom->Add(std::unique_ptr<CS230::GameObject>(new AttackDelayObject(
		AI_ATTACK_DELAY,
		[this, attacker, defender, damage, handler]()
		{
		  if (!defender->IsAlive()) return; // 다른 경로로 이미 사망 시 skip
		  ApplyDamage(attacker, defender, damage);
		  if (handler)
			handler->OnAfterAttack(attacker, defender, damage);
		})));
	}
  }
  else
  {
	// 플레이어 공격: 즉시 데미지 적용
	ApplyDamage(attacker, defender, damage);
	if (handler)
	  handler->OnAfterAttack(attacker, defender, damage);
  }

  attacker->SetHasAttackedThisTurn(true);

  // Consume AP (갓모드 Dragon은 AP 소모 없음)
  auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
  if (!(debug_mgr && debug_mgr->IsGodModeEnabled() && attacker->GetCharacterType() == CharacterTypes::Dragon))
    attacker->GetActionPointsComponent()->Consume(attackCost);

  return true;
}

int CombatSystem::RollAttackDamage(const std::string& damageDice, int baseDamage)
{
  auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
  int diceRoll = dice ? dice->RollDiceFromString(damageDice) : 0;
  return diceRoll + baseDamage;
}

bool CombatSystem::IsCriticalHit()
{
  return false;
}

bool CombatSystem::IsInRange(Character* attacker, Character* target, int range)
{
  if (attacker == nullptr || target == nullptr)
	return false;

  int distance = GetDistance(attacker, target);
  return distance <= range;
}

int CombatSystem::GetDistance(Character* char1, Character* char2)
{
  if (char1 == nullptr || char2 == nullptr)
	return -1;

  auto sub = char1->GetGridPosition()->Get() - char2->GetGridPosition()->Get();
  if (sub.x < 0)
	sub.x = -sub.x;
  if (sub.y < 0)
	sub.y = -sub.y;

  return sub.x + sub.y;
}