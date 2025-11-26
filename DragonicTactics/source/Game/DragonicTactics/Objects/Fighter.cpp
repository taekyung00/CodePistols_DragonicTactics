#include "pch.h"

#include "Fighter.h"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"

Fighter::Fighter(Math::ivec2 start_coordinates)
	: Character(
		  CharacterTypes::Fighter, start_coordinates, 2,
		  {
			  { 1, 2 },
				{ 2, 2 }
})
{
	CharacterStats fighter_stats;
	fighter_stats.max_hp	   = 90;
	fighter_stats.current_hp   = 90;
	fighter_stats.speed		   = 3;
	fighter_stats.base_attack  = 5;
	fighter_stats.attack_dice  = "2d6";
	fighter_stats.base_defend  = 0;
	fighter_stats.defend_dice  = "1d10";
	fighter_stats.attack_range = 1;

	*GetStatsComponent() = StatsComponent(fighter_stats);
	m_action_list.push_back(new ActionAttack());
}

void Fighter::OnTurnStart()
{
	ActionPoints* ap = GetActionPointsComponent();
	if (ap != nullptr)
	{
		ap->Refresh();
	}
	// DecideAction();

}

void Fighter::Action()
{
	// If AI-controlled, make decisions
	if (IsAIControlled())
	{
		AISystem* ai = Engine::GetGameStateManager().GetGSComponent<AISystem>();
	
		AIDecision decision = ai->MakeDecision(this);
		ai->ExecuteDecision(this, decision);
	}
}

bool Fighter::IsAIControlled() const
{
	// For Week 4: Fighter is always AI-controlled
	// Week 5+: Player can control via manual mode
	return true;
}

void Fighter::OnTurnEnd()
{
}

void Fighter::Update(double dt)
{
	Character::Update(dt);
}

void Fighter::TakeDamage(int damage, Character* attacker)
{
	Character::TakeDamage(damage, attacker);
}

// void Fighter::DecideAction() {
//     m_turn_goal = TurnGoal::Attack;
//
//     Character* target = nullptr;
//     CS230::GameObjectManager* gom = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();
//
//     if (gom != nullptr) {
//         for (const auto& obj_smart_ptr : gom->GetAll()) {
//             CS230::GameObject* obj = obj_smart_ptr.get();
//             if (obj->Type() == GameObjectTypes::Character) {
//                 Character* character = static_cast<Character*>(obj);
//                 if (character->GetCharacterType() == CharacterTypes::Dragon) {
//                     target = character;
//                     break;
//                 }
//             }
//         }
//     }
//
//     if (target != nullptr) {
//         Action* basic_attack = m_action_list[0];
//         if (basic_attack != nullptr && basic_attack->CanExecute(this, nullptr)) {
//             PerformAction(basic_attack, target, {});
//             Engine::GetLogger().LogEvent("Fighter executes a Basic Attack on " + target->TypeName() + "!");
//         }
//         else {
//             Engine::GetLogger().LogEvent("Fighter wants to attack " + target->TypeName() + ", but cannot execute action! (No AP or out of range)");
//         }
//     }
//     else {
//         Engine::GetLogger().LogEvent("Fighter has no target to attack!");
//     }
// }

// Ginam
AbilityResult Fighter::PerformMeleeAttack(Character* target)
{
	// using basic melee attack ability
	return meleeAttack.Use(this, target);
}

AbilityResult Fighter::PerformShieldBash(Character* target)
{
	// using shield bash ability with knockback
	return shieldBash.Use(this, target);
}
