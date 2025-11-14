// File: CS230/Game/Singletons/AISystem.cpp
#include "AISystem.h"
#include "../StateComponents/GridSystem.h"
#include "CombatSystem.h"
#include "EventBus.h"
#include "../../Engine/Engine.h"

AISystem& AISystem::GetInstance() {
    static AISystem instance;
    return instance;
}

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

    // Step 2: Check if we should use ability
    if (ShouldUseAbility(actor, target)) {
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
        GridSystem& grid = GridSystem::GetInstance();
        Math::ivec2 actorPos = actor->GetGridPosition();
        Math::ivec2 targetPos = target->GetGridPosition();

        std::vector<Math::ivec2> path = grid.FindPath(actorPos, targetPos);

        if (path.size() > 1) {
            // Move as far as action points allow
            int maxMove = actor->GetActionPoints() / actor->GetMovementCost();
            int moveIndex = std::min((int)path.size() - 1, maxMove);

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

    GridSystem& grid = GridSystem::GetInstance();
    std::vector<Character*> allCharacters = grid.GetAllCharacters();

    Character* highestThreat = nullptr;
    int highestThreatScore = -1;

    for (Character* potential : allCharacters) {
        // Skip self, allies, and dead characters
        if (potential == actor || !potential->IsAlive()) continue;
        if (potential->GetTeam() == actor->GetTeam()) continue;

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
    GridSystem& grid = GridSystem::GetInstance();

    int distance = grid.GetDistance(actor->GetGridPosition(), target->GetGridPosition());

    // Threat decreases with distance
    int threatScore = 1000 - distance;

    // Week 16+: Factor in damage output, HP, status effects
    // For Week 4: Simple distance-based threat

    return threatScore;
}

bool AISystem::ShouldMoveCloser(Character* actor, Character* target) {
    if (!actor || !target) return false;

    GridSystem& grid = GridSystem::GetInstance();
    int distance = grid.GetDistance(actor->GetGridPosition(), target->GetGridPosition());
    int attackRange = actor->GetAttackRange();

    // Move if target is out of attack range
    return (distance > attackRange);
}

bool AISystem::ShouldAttack(Character* actor, Character* target) {
    if (!actor || !target) return false;

    // Check range
    CombatSystem& combat = CombatSystem::GetInstance();
    if (!combat.IsInRange(actor, target)) return false;

    // Check action points
    if (actor->GetActionPoints() < actor->GetAttackCost()) return false;

    return true;
}

bool AISystem::ShouldUseAbility(Character* actor, Character* target) {
    if (!actor || !target) return false;

    // Week 4: Fighter has Shield Bash (stun adjacent enemy)
    // Check if adjacent
    GridSystem& grid = GridSystem::GetInstance();
    int distance = grid.GetDistance(actor->GetGridPosition(), target->GetGridPosition());

    if (distance != 1) return false; // Shield Bash requires adjacency

    // Check if ability available (cooldown)
    if (!actor->HasAbility("Shield Bash")) return false;

    // Check if ability is more valuable than attack
    // Use Shield Bash if target is healthy (stun > damage)
    double hpPercent = (double)target->GetHP() / target->GetMaxHP();

    return (hpPercent > 0.7); // Use stun if target above 70% HP
}

void AISystem::ExecuteDecision(Character* actor, const AIDecision& decision) {
    Engine::GetLogger().LogEvent(
        actor->TypeName() + " AI Decision: " + decision.reasoning
    );

    switch (decision.type) {
        case AIDecisionType::Move:
            actor->MoveTo(decision.destination);
            break;

        case AIDecisionType::Attack:
            actor->PerformAttack(decision.target);
            break;

        case AIDecisionType::UseAbility:
            actor->UseAbility(decision.abilityName, decision.target);
            break;

        case AIDecisionType::EndTurn:
            // Do nothing, turn will end naturally
            break;

        case AIDecisionType::None:
            Engine::GetLogger().LogError("AI made no decision!");
            break;
    }

    // Publish AI decision event for debugging
    EventBus::GetInstance().Publish(AIDecisionEvent{
        actor, decision.type, decision.target, decision.reasoning
    });
}