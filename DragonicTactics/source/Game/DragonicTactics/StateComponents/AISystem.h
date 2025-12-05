/**
 * @file AISystem.h
 * @author Sangyun Lee
 * @brief 
 * @date 2025-12-04
 */

#pragma once
#include "./Engine/Component.h"
#include "AI/IAIStrategy.h"
#include <map>

class AISystem : public CS230::Component {
public:
    AISystem();
    ~AISystem();

    void Init();

    AIDecision MakeDecision(Character* actor);
    void ExecuteDecision(Character* actor, const AIDecision& decision);

private:
    std::map<CharacterTypes, IAIStrategy*> m_strategies;
};