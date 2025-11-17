// File: CS230/Game/Singletons/AISystem.cpp
#include "AISystem.h"
#include "../StateComponents/GridSystem.h"
#include "./Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "./Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "CombatSystem.h"
#include "SpellSystem.h"
#include "EventBus.h"
#include "Game/DragonicTactics/Types/Events.h"
#include "Engine/Engine.hpp"
#include "./Engine/GameStateManager.hpp"



AIDecision AISystem::MakeDecision(Character* actor) {
    AIDecision decision;
    decision.type = AIDecisionType::None;

    // Step 1: Assess threats (find target)
    Character* target = AssessThreats(actor);
    if (!target) {
        decision.type = AIDecisionType::EndTurn;
        decision.reasoning = "No valid targets";
        return decision;
    }

    decision.target = target;
	//while (actor)
    // Step 2: Check if we should use ability
    if (ShouldUseAbility(actor, target)) { //fighter should false
        decision.type = AIDecisionType::UseAbility;
        decision.abilityName = "Shield Bash"; // Fighter only has one ability in Week 4
        decision.reasoning = "Target adjacent, Shield Bash available";
        return decision;
    }

    // Step 3: Check if we should attack
    if (ShouldAttack(actor, target)) {
        decision.type = AIDecisionType::Attack;
        decision.reasoning = "Target in attack range";
        return decision;
    }

    // Step 4: Move closer to target
    if (ShouldMoveCloser(actor, target)) {
        // Calculate path to target
        GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
        Math::ivec2 targetPos = target->GetGridPosition()->Get();
        Math::ivec2 actorPos = actor->GetGridPosition()->Get();
        // grid->Reset();

        std::vector<Math::ivec2> bestPath;
        int bestCost = INT_MAX;

        static const Math::ivec2 offsets[8] =
        {
            {  1,  0 }, { -1,  0 }, {  0,  1 }, {  0, -1 }//,
            //{  1,  1 }, {  1, -1 }, { -1,  1 }, { -1, -1 } for spell
        };

        for (const Math::ivec2& off : offsets)
        {
            Math::ivec2 candidate = targetPos + off;

            // FindPath
            auto path = grid->FindPath(actorPos, candidate);

            if (path.empty()) 
                continue; 

            if ((int)path.size() < bestCost)
            {
                bestCost = (int)path.size();
                bestPath = path;
            }
        }

        std::vector<Math::ivec2> path = bestPath;

        if (path.size() > 0) {
            // Move as far as action points allow
            int maxMove = actor->GetMovementRange();
            int moveIndex = std::min((int)path.size() - 1, maxMove - 1);

            decision.type = AIDecisionType::Move;
            decision.destination = path[moveIndex];
            decision.reasoning = "Moving closer to target";
            return decision;
        }
    }

    // No valid action, end turn
    decision.type = AIDecisionType::EndTurn;
    decision.reasoning = "No valid actions available";
    return decision;
}

Character* AISystem::AssessThreats(Character* actor) {
    // For Week 4: Simple - target the Dragon (only enemy)
    // Week 16+: More sophisticated (highest threat score)

    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    std::vector<Character*> allCharacters = grid->GetAllCharacters();

    Character* highestThreat = nullptr;
    int highestThreatScore = -1;

    for (Character* potential : allCharacters) {
        // Skip self, allies, and dead characters
        if (potential == nullptr) continue; 
        if (potential == actor || !potential->IsAlive()) continue;
        if (potential->GetCharacterType() == CharacterTypes::Dragon &&
            actor->GetCharacterType() == CharacterTypes::Dragon) continue;
        if (potential->GetCharacterType() != CharacterTypes::Dragon &&
            actor->GetCharacterType() != CharacterTypes::Dragon) continue;

        int threatScore = CalculateThreatScore(actor, potential);

        if (threatScore > highestThreatScore) {
            highestThreatScore = threatScore;
            highestThreat = potential;
        }
    }

    return highestThreat;
}

int AISystem::CalculateThreatScore(Character* actor, Character* target) {
    // Basic threat formula: Closer = higher threat
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

    int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(), target->GetGridPosition()->Get());

    // Threat decreases with distance
    int threatScore = 1000 - distance;

    // Week 16+: Factor in damage output, HP, status effects
    // For Week 4: Simple distance-based threat

    return threatScore;
}

bool AISystem::ShouldMoveCloser(Character* actor, Character* target) {
    if (!actor || !target) return false;
	if (actor->GetMovementRange() == 0) return false; //speed check
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(), target->GetGridPosition()->Get());
    int attackRange = actor->GetAttackRange();

    // Move if target is out of attack range
    return (distance > attackRange);
}

bool AISystem::ShouldAttack(Character* actor, Character* target) {
    if (!actor || !target) return false;

    // Check range
    CombatSystem& combat = Engine::GetCombatSystem();
	int			  actor_range = actor->GetAttackRange();
	if (!combat.IsInRange(actor, target, actor_range))
		return false; // range ???????

    // Check action points
    if (actor->GetActionPoints() == 0) return false; 

    return true;
}

//if actor and target are not nullptr
//find strongest spell

bool AISystem::ShouldUseAbility(Character* actor, Character* target) {
    if (!actor || !target) return false;
    //check all level's slot
	//if (actor->GetSpellSlotCount() == 0)
	 

    // Week 4: Fighter has Shield Bash (stun adjacent enemy)
    // Check if adjacent
    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(), target->GetGridPosition()->Get());

    if (distance != 1) return false; // Shield Bash requires adjacency

    // Check if ability available (cooldown)
    //if (!actor->HasAbility("Shield Bash")) return false;
    if (!actor->HasSpell("Shield Bash")) return false;

    // Check if ability is more valuable than attack
    // Use Shield Bash if target is healthy (stun > damage)
    double hpPercent = (double)target->GetHP() / target->GetMaxHP();

    return (hpPercent > 0.7); // Use stun if target above 70% HP
}

void AISystem::ExecuteDecision(Character* actor, const AIDecision& decision) {
    Engine::GetLogger().LogEvent(
        actor->TypeName() + " AI Decision: " + decision.reasoning
    );

    GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    SpellSystem& spell_system = Engine::GetSpellSystem();

    switch (decision.type) {
        case AIDecisionType::Move:
            if (grid->IsWalkable(decision.destination))
				{
					std::vector<Math::ivec2> new_path =
					grid->FindPath(actor->GetGridPosition()->Get(), decision.destination); // CalculateSimplePath(dragon->GetGridPosition()->Get(), grid_pos);
					MovementComponent* move_comp = actor->GetGOComponent<MovementComponent>();
                    if(move_comp != nullptr){
                        move_comp->SetPath(std::move(new_path));
                        //Engine::GetLogger().LogEvent("Path set. State changing to Moving.");
                    }
					//actor->GetGOComponent<MovementComponent>()->MoveTo(new_path);
				}
            break;

        case AIDecisionType::Attack:
            Engine::GetCombatSystem().ExecuteAttack(actor, decision.target);
            break;

        case AIDecisionType::UseAbility:
            //actor->UseSpell(decision.abilityName, decision.target);
            spell_system.CastSpell(actor, decision.abilityName, decision.target->GetGridPosition()->Get());
            break;

        case AIDecisionType::EndTurn:
            // Do nothing, turn will end naturally
            break;

        case AIDecisionType::None:
            Engine::GetLogger().LogError("AI made no decision!");
            break;
    }

    auto& eventbus = Engine::GetEventBus();
    // Publish AI decision event for debugging
    eventbus.Publish(AIDecisionEvent{
        reinterpret_cast<Character*>(actor), decision.type, decision.target, decision.reasoning
    });
}