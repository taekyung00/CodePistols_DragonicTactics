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

class AISystem : public CS230::Component
{
  public:
  AISystem();
  ~AISystem();

  void Init();

  AIDecision MakeDecision(Character* actor);
  // 실제로 행동이 수행되었는지 반환 (false면 도달 불가 이동/시전 실패 등 no-op → 호출측이 턴 종료)
  bool		 ExecuteDecision(Character* actor, const AIDecision& decision);

  private:
  std::map<CharacterTypes, IAIStrategy*> m_strategies;
};