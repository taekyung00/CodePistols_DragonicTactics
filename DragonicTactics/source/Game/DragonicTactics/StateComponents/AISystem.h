<<<<<<< HEAD
﻿// File: CS230/Game/Singletons/AISystem.h
=======
/**
 * @file AISystem.h
 * @author Sangyun Lee
 * @brief
 * @date 2025-12-04
 */

>>>>>>> main
#pragma once
#include "./Engine/Component.h"
#include "AI/IAIStrategy.h"
#include <map>

class AISystem : public CS230::Component
{
  public:
  AISystem();
  ~AISystem();

  void Init();

  AIDecision MakeDecision(Character* actor);
  void		 ExecuteDecision(Character* actor, const AIDecision& decision);

<<<<<<< HEAD
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
=======
  private:
  std::map<CharacterTypes, IAIStrategy*> m_strategies;
};
>>>>>>> main
