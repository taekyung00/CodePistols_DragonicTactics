/**
 * @file FighterStrategy.h
 * @author Sangyun Lee
 * @brief 파이터 전용 AI 전략 (드래곤 추적 및 근접 공격)
 * @date 2025-12-04
 */
#pragma once
#include "IAIStrategy.h"

class GridSystem;

class FighterStrategy : public IAIStrategy {
public:
    AIDecision MakeDecision(Character* actor) override;

private:
    Character* FindDragon();

    Math::ivec2 FindNextMovePos(Character* actor, Character* target, GridSystem* grid);
};