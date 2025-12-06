/**
 * @file FighterStrategy.cpp
 * @author Sangyun Lee
 * @brief 파이터 AI 구현: 드래곤에게 접근하여 공격
 * @date 2025-12-04
 */
#include "pch.h"

#include "../../Objects/Components/ActionPoints.h"
#include "../../Objects/Components/GridPosition.h"
#include "../../Objects/Components/SpellSlots.h"
#include "../../Objects/Components/StatsComponent.h"
#include "../../StateComponents/CombatSystem.h"
#include "../../StateComponents/GridSystem.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "FighterStrategy.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

AIDecision FighterStrategy::MakeDecision(Character* actor)
{
  GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();

  // ============================================================
  // 1단계: 타겟 설정 (플로우차트: SettingTarget)
  // ============================================================

  Character*  target	  = nullptr;
  std::string target_type = "";

  // [조건 1] 보물을 가지고 있는가?
  if (actor->HasTreasure()) // ← Character의 팩트 쿼리
  {
	// → Yes → 목표 = 출구
	target_type = "Exit";

	// GridSystem에서 출구 위치 가져오기 (하드코딩 제거!)
	if (!grid->HasExit())
	{
	  Engine::GetLogger().LogError("Fighter has treasure but no exit found!");
	  return { AIDecisionType::EndTurn, nullptr, {}, "", "No exit available" };
	}

	Math::ivec2 exitPos = grid->GetExitPosition();

	// 출구에 도달했는가?
	if (actor->GetGridPosition()->Get() == exitPos)
	{
	  // 게임 패배 (적 탈출 성공)
	  return { AIDecisionType::EndTurn, nullptr, {}, "", "Escaped with treasure!" };
	}

	// 출구로 이동
	return { AIDecisionType::Move, nullptr, exitPos, "", "Escaping with treasure" };
  }

  // [조건 2] 내 체력이 30% 이하인가?
  if (IsInDanger(actor)) // ← Character의 GetHPPercentage() 사용
  {
	// → Yes → 클레릭이 살아 있는가?
	Character* cleric = FindCleric();
	if (cleric != nullptr)
	{
	  // → Yes → 목표 = 클레릭
	  target	  = cleric;
	  target_type = "Cleric";
	}
	else
	{
	  // → No → 목표 = 드래곤
	  target	  = FindDragon();
	  target_type = "Dragon";
	}
  }
  else
  {
	// → No → 목표 = 드래곤
	target		= FindDragon();
	target_type = "Dragon";
  }

  // 타겟이 없으면 턴 종료
  if (target == nullptr)
  {
	return { AIDecisionType::EndTurn, nullptr, {}, "", "No valid target found" };
  }

  // ============================================================
  // 2단계: 행동 시작 (플로우차트: MoveStart)
  // ============================================================

  // 거리 계산
  int distance = grid->ManhattanDistance(actor->GetGridPosition()->Get(), target->GetGridPosition()->Get());

  int  attackRange = actor->GetAttackRange();
  bool onTarget	   = false;

  // 목표에 도달했나? (출구: 타일 위, 그 외: 사거리 내)
  if (target_type == "Exit")
  {
	onTarget = (distance == 0); // 출구는 정확히 같은 타일
  }
  else
  {
	onTarget = (distance <= attackRange); // 사거리 내
  }

  // ============================================================
  // 3단계: 목표 도달 시 행동 분기
  // ============================================================

  if (onTarget)
  {
	// [분기] 현재 목표가 무엇인가?
	if (target_type == "Exit")
	{
	  // → 출구 → 게임 패배 (적 탈출 성공)
	  Engine::GetGameStateManager().GetGSComponent<EventBus>()->Publish(CharacterEscapedEvent{ actor });
	  return { AIDecisionType::EndTurn, nullptr, {}, "", "Reached exit!" };
	}
	else if (target_type == "Cleric")
	{
	  // → 클레릭 → 치료 대기 (턴 종료)
	  return { AIDecisionType::EndTurn, nullptr, {}, "", "Waiting for heal from Cleric" };
	}
	else if (target_type == "Dragon")
	{
	  // → 드래곤 → 공격 가능한가? (행동력 AND 공격 범위)
	  if (actor->GetActionPoints() > 0 && distance <= attackRange)
	  {
		return DecideAttackAction(actor, target, distance);
	  }
	  else
	  {
		// 공격 불가 (행동력 부족 또는 사거리 밖) → 이동 가능한가? (Speed 체크!)
		if (actor->GetMovementRange() > 0) // Speed (이동력) 체크
		{
		  Math::ivec2 movePos = FindNextMovePos(actor, target, grid);
		  if (movePos != actor->GetGridPosition()->Get())
		  {
			return { AIDecisionType::Move, nullptr, movePos, "", "Moving closer" };
		  }
		}

		// 이동도 불가 → 턴 종료
		return { AIDecisionType::EndTurn, nullptr, {}, "", "No movement left" };
	  }
	}
  }

  // ============================================================
  // 4단계: 목표 미도달 시 이동
  // ============================================================

  // 이동력이 1 이상인가? (Speed 체크!)
  if (actor->GetMovementRange() > 0) //Speed (이동력) 체크
  {
	// → Yes → 목표로 1칸 이동
	Math::ivec2 movePos = FindNextMovePos(actor, target, grid);

	if (movePos != actor->GetGridPosition()->Get())
	{
	  return { AIDecisionType::Move, nullptr, movePos, "", "Moving towards " + target_type };
	}
  }

  // → No → 턴 종료
  return { AIDecisionType::EndTurn, nullptr, {}, "", "No movement left" };
}

// ============================================================
// 헬퍼 함수들
// ============================================================

Character* FighterStrategy::FindDragon()
{
  GridSystem* grid	   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  auto		  allChars = grid->GetAllCharacters();

  for (auto* c : allChars)
  {
	// 살아있는 드래곤만 찾음
	if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Dragon)
	{
	  return c;
	}
  }
  return nullptr;
}

Character* FighterStrategy::FindCleric()
{
  // TODO: Cleric 구현 후 활성화
  // GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  // auto allChars = grid->GetAllCharacters();
  //
  // for (auto* c : allChars)
  // {
  //     if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Cleric)
  //     {
  //         return c;
  //     }
  // }
  return nullptr; // 현재는 null 반환
}

// ============================================================
// 전략별 판단 헬퍼 (Decision Helpers)
// Character의 팩트 쿼리를 사용하여 Fighter만의 기준으로 판단
// ============================================================

bool FighterStrategy::IsInDanger(Character* actor) const
{
  // Fighter 전략: HP 30% 이하를 위험으로 판단
  // (Cleric은 50% 이하, Rogue는 40% 이하 등 전략마다 다름)
  return (actor->GetHPPercentage() <= 0.3f);
}

bool FighterStrategy::ShouldUseSpellAttack(Character* actor, [[maybe_unused]] Character* target) const
{
  // Fighter 전략: 클레릭이 있을 때는 버프/디버프 확인 후 주문 사용
  // 조건: 내가 버프 받고, 드래곤이 디버프 받고, 주문 슬롯이 있을 때

  // TODO: Week 6+ StatusEffect 시스템 구현 후 활성화
  // bool iBuffed = actor->HasBuff("Blessed");
  // bool targetDebuffed = target->HasDebuff("Weakened");
  // bool hasSlots = actor->HasAnySpellSlot();
  // return iBuffed && targetDebuffed && hasSlots;

  // 현재는 주문 슬롯만 체크
  return actor->HasAnySpellSlot();
}

Math::ivec2 FighterStrategy::FindNextMovePos(Character* actor, Character* target, GridSystem* grid)
{
  Math::ivec2 targetPos = target->GetGridPosition()->Get();
  Math::ivec2 myPos		= actor->GetGridPosition()->Get();

  std::vector<Math::ivec2> bestPath;
  int					   bestPathCost = 999999;

  // 드래곤의 상하좌우 4방향 중 갈 수 있는 가장 가까운 곳 탐색
  static const Math::ivec2 offsets[4] = {
	{  0,	 1 },
	  {	0, -1 },
	 { -1,  0 },
	{  1,	 0 }
  };

  for (const auto& offset : offsets)
  {
	Math::ivec2 attackPos = targetPos + offset;

	// 맵 밖이거나 막힌 곳이면 패스
	if (!grid->IsValidTile(attackPos) || !grid->IsWalkable(attackPos))
	{
	  continue;
	}

	// 해당 위치까지 경로 계산
	std::vector<Math::ivec2> currentPath = grid->FindPath(myPos, attackPos);

	// 경로가 있고 더 짧다면 갱신
	if (!currentPath.empty() && (int)currentPath.size() < bestPathCost)
	{
	  bestPathCost = (int)currentPath.size();
	  bestPath	   = currentPath;
	}
  }

  // 경로가 존재한다면
  if (!bestPath.empty())
  {
	// 내 이동력(Speed) 한계 내에서 가장 멀리 갈 수 있는 칸 선택
	int maxReach  = std::min((int)bestPath.size(), actor->GetMovementRange());
	int destIndex = maxReach - 1;

	if (destIndex >= 0)
	{
	  return bestPath[destIndex];
	}
  }

  return myPos; // 갈 곳 없으면 제자리 반환
}