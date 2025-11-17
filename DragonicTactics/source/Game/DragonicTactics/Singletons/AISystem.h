// File: CS230/Game/Singletons/AISystem.h
#pragma once
#include "../Objects/Character.h"
#include <vector>

enum class AIDecisionType {
    Move,
    Attack,
    UseAbility,
    EndTurn,
    None
};

struct AIDecision {
    AIDecisionType type;
    Character* target;        // Target for attack/ability
    Math::ivec2 destination;  // Destination for move
    std::string abilityName;  // Ability to use
    std::string reasoning;    // Debug: Why this decision?
};

class AISystem{
public:

    AISystem() = default;
    // Main AI entry point
    AIDecision MakeDecision(Character* actor);

    // Decision helpers
    Character* AssessThreats(Character* actor);
    bool ShouldMoveCloser(Character* actor, Character* target);
    bool ShouldAttack(Character* actor, Character* target);
    bool ShouldUseAbility(Character* actor, Character* target);

    // Execution
    void ExecuteDecision(Character* actor, const AIDecision& decision);

private:

    // Threat assessment
    int CalculateThreatScore(Character* actor, Character* target);
};