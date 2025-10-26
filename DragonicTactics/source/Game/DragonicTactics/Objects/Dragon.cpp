#include "Dragon.h"
#include "./Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "./Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "./Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"

Dragon::Dragon(Math::ivec2 start_coordinates)
    : Character(
        CharacterTypes::Dragon,
        start_coordinates,
        3,
        { {1, 4}, {2, 3}, {3, 2}, {4, 2}, {5, 1} }
    )
{
    CharacterStats dragon_stats;
    dragon_stats.max_hp = 250;
    dragon_stats.current_hp = 250;
    dragon_stats.speed = 4;
    dragon_stats.base_attack = 10;
    dragon_stats.attack_dice = "3d8";
    dragon_stats.base_defend = 5;
    dragon_stats.defend_dice = "2d10";
    dragon_stats.attack_range = 2;

    *GetStatsComponent() = StatsComponent(dragon_stats);

    m_action_list.push_back(new ActionAttack());
}

void Dragon::OnTurnStart() {
    ActionPoints* ap = GetActionPointsComponent();
    if (ap != nullptr) {
        ap->Refresh();
    }

    DecideAction();
}

void Dragon::OnTurnEnd() {
}

void Dragon::TakeDamage(int damage, Character* attacker) {
    Character::TakeDamage(damage, attacker);
}

void Dragon::DecideAction() {
    m_turn_goal = TurnGoal::Attack;
    Engine::GetLogger().LogEvent("Dragon roars and decides to attack!");
}
