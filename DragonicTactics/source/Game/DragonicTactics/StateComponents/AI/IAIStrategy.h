/**
 * @file IAIStrategy.h
 * @author Sangyun Lee
 * @brief 모든 AI 전략의 공통 인터페이스 및 데이터 구조
 * @date 2025-12-04
 */

#pragma once
#include "Engine/Vec2.h"
#include <string>
#include <vector>

// 전방 선언
class Character;

// 1. 의사결정 종류
enum class AIDecisionType
{
  Move,		  // 이동
  Attack,	  // 공격
  UseAbility, // 스킬
  EndTurn,	  // 대기
  None
};

// 2. 의사결정 데이터 (명령서)
struct AIDecision
{
  AIDecisionType type		 = AIDecisionType::None;
  Character*	 target		 = nullptr;	 // 대상
  Math::ivec2	 destination = { 0, 0 }; // 목적지
  std::string	 abilityName = "";		 // 스킬명
  std::string	 reasoning	 = "";		 // 디버그용 메모
};

// 3. 전략 인터페이스
class IAIStrategy
{
  public:
  virtual ~IAIStrategy() = default;

  // 상황을 판단하여 행동을 결정하는 핵심 함수
  virtual AIDecision MakeDecision(Character* actor) = 0;
};