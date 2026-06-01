#include "pch.h"

#include "WizardStrategy.h"
#include "../../Objects/Components/ActionPoints.h"
#include "../../Objects/Components/GridPosition.h"
#include "../../Objects/Components/SpellSlots.h"
#include "../../Objects/Components/StatsComponent.h"
#include "../../StateComponents/GridSystem.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

// ============================================================
// MakeDecision — wizard.mmd 플로우차트 최상위 흐름
// ============================================================

AIDecision WizardStrategy::MakeDecision(Character* actor)
{
	GridSystem* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	Character*  dragon = FindDragon();

	if (!dragon || !grid)
		return { AIDecisionType::EndTurn, nullptr, {}, "", "No dragon or grid" };

	bool has_fear = actor->Has("Fear");

	// ── [1순위] 확정 킬 ────────────────────────────────────────
	if (CanKillThisTurn(actor, dragon, grid, has_fear))
		return MakeKillLoopDecision(actor, dragon, grid, has_fear);

	if (actor->GetActionPoints() <= 0)
		return { AIDecisionType::EndTurn, nullptr, {}, "", "No AP" };

	// ── [2단계] Sweet Spot 이동 ────────────────────────────────
	if (!IsInSweetSpot(actor, dragon, grid))
		return MakeMoveDecision(actor, dragon, grid, has_fear);

	// ── [3단계] Sweet Spot 내 — 슬롯 상태로 분기 ───────────────
	float ratio = GetSlotRatio(actor);
	if (ratio > SLOT_THRESHOLD)
	{
		return MakeAttackDecision(actor, dragon, grid);
	}
	else
	{
		// 마력 전환 조건: Fear 없음 + HP > 30% + Lv1 슬롯 존재
		if (!has_fear && actor->GetHPPercentage() > MANA_CONV_HP_THRESHOLD && HasSpellSlot(actor, 1))
		{
			return { AIDecisionType::UseAbility, actor, {}, "S_ENH_040",
			         "Sweet Spot: Mana Conversion", 0, 1 };
		}
		// 캔트립 fallback
		return MakeAttackDecision(actor, dragon, grid);
	}
}

// ============================================================
// Kill_Loop: 확정 킬 시퀀스 (wizard.mmd Kill_Loop 서브그래프)
// ============================================================

AIDecision WizardStrategy::MakeKillLoopDecision(
	Character* actor, Character* dragon, GridSystem* grid, bool has_fear)
{
	if (actor->GetActionPoints() <= 0)
		return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill: No AP" };

	int dist     = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
	                                        dragon->GetGridPosition()->Get());
	int dmg_est  = AVG_FIRE_BOLT_DAMAGE - (has_fear ? FEAR_DMG_PENALTY : 0);
	int dragon_hp = dragon->GetHP();

	// 1레벨로 처치 가능 → Fire Bolt (사거리 내일 때)
	if (dragon_hp <= dmg_est && HasSpellSlot(actor, 1) && dist <= FIRE_BOLT_RANGE)
		return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_010",
		         "Kill: Fire Bolt (sufficient)", 0, 1 };

	// Magic Missile (무한 사거리) — 더 확실한 처치
	if (HasSpellSlot(actor, 2))
		return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_060",
		         "Kill: Magic Missile", 0, 2 };

	// Lv2 없으면 Fire Bolt
	if (HasSpellSlot(actor, 1) && dist <= FIRE_BOLT_RANGE)
		return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_010",
		         "Kill: Fire Bolt", 0, 1 };

	return { AIDecisionType::EndTurn, nullptr, {}, "", "Kill: No spell available" };
}

// ============================================================
// Sweet Spot 이동 결정
// ============================================================

AIDecision WizardStrategy::MakeMoveDecision(
	Character* actor, Character* dragon, GridSystem* grid, bool has_fear)
{
	Math::ivec2 myPos      = actor->GetGridPosition()->Get();
	Math::ivec2 dragonPos  = dragon->GetGridPosition()->Get();
	int         move_range = actor->GetMovementRange();

	// ── [1] 걷기로 Sweet Spot 도달 가능 여부 확인 (AP 절약 우선) ────
	if (move_range > 0)
	{
		auto        walk_tiles = grid->GetReachableTiles(myPos, move_range);
		Math::ivec2 walk_sweet = myPos;
		bool        found      = false;

		for (const auto& tile : walk_tiles)
		{
			if (grid->IsOccupied(tile)) continue;
			GridSystem::TileType t = grid->GetTileType(tile);
			if (t != GridSystem::TileType::Empty && t != GridSystem::TileType::Lava) continue;

			int d = grid->ManhattanDistance(tile, dragonPos);
			if (d >= SWEET_SPOT_MIN && d <= SWEET_SPOT_MAX)
			{
				// 용암 회피 우선 — non-lava 발견 시 즉시 확정
				if (!found || t == GridSystem::TileType::Empty)
				{
					walk_sweet = tile;
					found      = true;
					if (t == GridSystem::TileType::Empty) break;
				}
			}
		}

		if (found)
		{
			Math::ivec2 move_pos = FindNextMoveToward(actor, walk_sweet, grid);
			if (move_pos != myPos)
				return { AIDecisionType::Move, nullptr, move_pos,
				         "", "Move: Walk to sweet spot (AP saved)", LAVA_TILE_PENALTY };
		}
	}

	// ── [2] 걷기로 Sweet Spot 불가 → Teleport 또는 Fear 이동 ─────────
	Math::ivec2 sweet_tile = FindSweetSpotTile(actor, dragon, grid);

	if (sweet_tile == myPos)
		return { AIDecisionType::EndTurn, nullptr, {}, "", "Move: Already optimal" };

	if (!has_fear)
	{
		// 텔레포트 (AP-1, 슬롯 불필요) — target=nullptr, destination=sweet_tile
		// AISystem::ExecuteDecision에서 target==nullptr → destination 사용
		return { AIDecisionType::UseAbility, nullptr, sweet_tile, "S_GEO_030",
		         "Move: Teleport to sweet spot", 0, 0 };
	}
	else
	{
		// Fear 상태: 텔레포트 불가, 최대한 Sweet Spot 방향으로 이동
		if (move_range > 0)
		{
			Math::ivec2 move_pos = FindNextMoveToward(actor, sweet_tile, grid);
			if (move_pos != myPos)
				return { AIDecisionType::Move, nullptr, move_pos,
				         "", "Move: Walk toward sweet spot (Fear)", LAVA_TILE_PENALTY };
		}
		return { AIDecisionType::EndTurn, nullptr, {}, "", "Move: Fear, no movement left" };
	}
}

// ============================================================
// 공격 결정 (Sweet Spot 내에서)
// ============================================================

AIDecision WizardStrategy::MakeAttackDecision(
	Character* actor, Character* dragon, GridSystem* grid)
{
	int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
	                                    dragon->GetGridPosition()->Get());

	// Fire Bolt (사거리 4)
	if (dist <= FIRE_BOLT_RANGE && HasSpellSlot(actor, 1))
		return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_010",
		         "Attack: Fire Bolt", 0, 1 };

	// Magic Missile (무한 사거리)
	if (HasSpellSlot(actor, 2))
		return { AIDecisionType::UseAbility, dragon, {}, "S_ATK_060",
		         "Attack: Magic Missile", 0, 2 };

	// 슬롯 없음 → 턴 종료
	return { AIDecisionType::EndTurn, nullptr, {}, "", "Attack: No spell slots" };
}

// ============================================================
// 헬퍼 함수들
// ============================================================

Character* WizardStrategy::FindDragon()
{
	GridSystem* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!grid) return nullptr;

	for (auto* c : grid->GetAllCharacters())
	{
		if (c && c->IsAlive() && c->GetCharacterType() == CharacterTypes::Dragon)
			return c;
	}
	return nullptr;
}

bool WizardStrategy::IsInSweetSpot(Character* actor, Character* dragon, GridSystem* grid) const
{
	int dist = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
	                                    dragon->GetGridPosition()->Get());
	return dist >= SWEET_SPOT_MIN && dist <= SWEET_SPOT_MAX;
}

bool WizardStrategy::CanKillThisTurn(
	Character* actor, Character* dragon, GridSystem* grid, bool has_fear) const
{
	if (!dragon->IsAlive()) return false;
	if (actor->GetActionPoints() <= 0) return false;

	int fear_pen  = has_fear ? FEAR_DMG_PENALTY : 0;
	int dragon_hp = dragon->GetHP();
	int dist      = grid->ManhattanDistance(actor->GetGridPosition()->Get(),
	                                         dragon->GetGridPosition()->Get());

	// Fire Bolt (S_ATK_010): avg 9 - fear_pen, 사거리 4
	if (HasSpellSlot(actor, 1) && dist <= FIRE_BOLT_RANGE)
	{
		if (dragon_hp <= AVG_FIRE_BOLT_DAMAGE - fear_pen)
			return true;
	}

	// Magic Missile (S_ATK_060): flat 8 - fear_pen, 무한 사거리
	if (HasSpellSlot(actor, 2))
	{
		if (dragon_hp <= 8 - fear_pen)
			return true;
	}

	return false;
}

bool WizardStrategy::HasSpellSlot(Character* actor, int level) const
{
	return actor->GetAvailableSpellSlots(level) > 0;
}

float WizardStrategy::GetSlotRatio(Character* actor) const
{
	int remaining = actor->GetAvailableSpellSlots(1) + actor->GetAvailableSpellSlots(2);
	return static_cast<float>(remaining) / static_cast<float>(MAX_TOTAL_SLOTS);
}

// ============================================================
// Sweet Spot 타일 탐색
// ============================================================

Math::ivec2 WizardStrategy::FindSweetSpotTile(
	Character* actor, Character* dragon, GridSystem* grid) const
{
	Math::ivec2 myPos     = actor->GetGridPosition()->Get();
	Math::ivec2 dragonPos = dragon->GetGridPosition()->Get();

	// 이미 Sweet Spot이면 현재 위치 반환
	int cur_dist = grid->ManhattanDistance(myPos, dragonPos);
	if (cur_dist >= SWEET_SPOT_MIN && cur_dist <= SWEET_SPOT_MAX)
		return myPos;

	// 텔레포트 사거리(TELEPORT_RANGE = CSV Empty:Point:4) 내 타일 중
	// Dragon에서 SWEET_SPOT_MIN~SWEET_SPOT_MAX 거리인 가장 가까운 빈 타일 탐색
	int           teleport_range = TELEPORT_RANGE;
	auto          candidates     = grid->GetReachableTiles(myPos, teleport_range);
	Math::ivec2   best           = myPos;
	int           best_priority  = -1; // 높을수록 선호 (Sweet Spot 내: 2, 더 멀리: 1)

	for (const auto& tile : candidates)
	{
		if (grid->IsOccupied(tile)) continue;
		GridSystem::TileType t = grid->GetTileType(tile);
		if (t != GridSystem::TileType::Empty && t != GridSystem::TileType::Lava) continue;

		int d = grid->ManhattanDistance(tile, dragonPos);

		// Sweet Spot 정확히 맞는 타일 최우선
		if (d >= SWEET_SPOT_MIN && d <= SWEET_SPOT_MAX)
		{
			// 용암 타일 회피 우선, 같은 조건이면 더 가까운 위치 선호
			int priority = (t == GridSystem::TileType::Lava) ? 2 : 3;
			if (priority > best_priority)
			{
				best_priority = priority;
				best          = tile;
			}
		}
		// Sweet Spot 밖: 너무 가까우면 후퇴, 너무 멀면 접근
		else if (best_priority < 0)
		{
			bool should_pick = (cur_dist < SWEET_SPOT_MIN && d > cur_dist)
			                || (cur_dist > SWEET_SPOT_MAX && d < cur_dist);
			if (should_pick)
			{
				best_priority = 1;
				best          = tile;
			}
		}
	}

	return best;
}

// ============================================================
// Sweet Spot 방향으로 A* 1스텝 이동
// ============================================================

Math::ivec2 WizardStrategy::FindNextMoveToward(
	Character* actor, Math::ivec2 goal, GridSystem* grid) const
{
	Math::ivec2 myPos = actor->GetGridPosition()->Get();
	if (myPos == goal) return myPos;

	auto path = grid->FindPath(myPos, goal, LAVA_TILE_PENALTY);
	if (path.empty()) return myPos;

	int maxReach  = std::min(static_cast<int>(path.size()), actor->GetMovementRange());
	int destIndex = maxReach - 1;
	if (destIndex >= 0)
		return path[static_cast<std::size_t>(destIndex)];

	return myPos;
}
