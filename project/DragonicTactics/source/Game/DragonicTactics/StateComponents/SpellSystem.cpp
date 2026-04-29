/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

// File: CS230/Game/System/SpellSystem.cpp
#include "../../../Engine/GameObjectManager.h"
#include "../../../Engine/Particle.h"
#include "../Objects/Character.h"
#include "../StateComponents/CombatSystem.h"
#include "../StateComponents/DiceManager.h"
#include "../StateComponents/EventBus.h"
#include "../StateComponents/GridSystem.h"
#include "../Types/CharacterTypes.h"
#include "../Types/Events.h"
#include "../Types/GameObjectTypes.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Logger.h"
#include "Engine/Path.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/StateComponents/StatusEffectHandler.h"
#include "Game/DragonicTactics/Debugger/DebugManager.h"
#include "Game/DragonicTactics/StateComponents/TurnManager.h"
#include "Game/Particles.h"
#include "SpellSystem.h"

std::vector<std::string> SpellSystem::SplitByDelimiter(const std::string& str, char delim) const
{
	std::vector<std::string> result;
	std::stringstream		 ss(str);
	std::string				 token;
	while (std::getline(ss, token, delim))
	{
		size_t start = token.find_first_not_of(" \t\r\n");
		size_t end	 = token.find_last_not_of(" \t\r\n");
		if (start != std::string::npos)
			result.push_back(token.substr(start, end - start + 1));
	}
	return result;
}

std::vector<std::string> SpellSystem::ReadCSVRecord(std::ifstream& file) const
{
	std::vector<std::string> columns;
	std::string				 cell;
	bool					 in_quotes = false;
	char					 c;

	while (file.get(c))
	{
		if (c == '"')
		{
			if (in_quotes && file.peek() == '"') // escaped quote ""
			{
				cell += '"';
				file.get(c); // 두 번째 " 소비
			}
			else
			{
				in_quotes = !in_quotes;
			}
		}
		else if (c == ',' && !in_quotes)
		{
			columns.push_back(cell);
			cell.clear();
		}
		else if (c == '\n' && !in_quotes)
		{
			columns.push_back(cell);
			return columns; // 레코드 완료
		}
		else if (c == '\r')
		{
			// skip
		}
		else
		{
			cell += c;
		}
	}

	if (!cell.empty() || !columns.empty())
		columns.push_back(cell); // 마지막 컬럼

	return columns;
}

void SpellSystem::LoadFromCSV(const std::string& csv_path)
{
	const std::filesystem::path full_path = assets::locate_asset(csv_path);
	std::ifstream				file(full_path);
	if (!file.is_open())
	{
		Engine::GetLogger().LogError("SpellSystem: Failed to open " + csv_path);
		return;
	}

	ReadCSVRecord(file); // 헤더 스킵

	while (file.good())
	{
		auto columns = ReadCSVRecord(file);
		if (columns.size() < 8 || columns[0].empty())
			continue;

		SpellData data	 = ParseCSVRow(columns);
		spells_[data.id] = data;
	}
	Engine::GetLogger().LogEvent("SpellSystem: Loaded " + std::to_string(spells_.size()) + " spells");
}

SpellTargeting SpellSystem::ParseTargeting(const std::string& targeting_str) const
{
	// "Enemy:Single:4"  →  filter=Enemy, geometry=Single, range=4
	// "Any:OddEven:-1"  →  filter=Any,   geometry=OddEven, range=-1
	auto parts = SplitByDelimiter(targeting_str, ':');

	SpellTargeting t;
	t.filter   = (parts.size() > 0) ? parts[0] : "Any";
	t.geometry = (parts.size() > 1) ? parts[1] : "Single";
	if (parts.size() > 2 && !parts[2].empty())
		t.range = std::stoi(parts[2]);
	else
		t.range = -1; // 파싱 실패 → 무한으로 처리
	return t;
}

SpellData SpellSystem::ParseCSVRow(const std::vector<std::string>& col) const
{
	SpellData data;
	data.id				= col[0];
	data.spell_name		= col[1];
	data.category		= col[2];
	data.usable_classes = SplitByDelimiter(col[3], ','); // "Dragon, Fighter" → vector
	data.spell_level	= col[4].empty() ? 0 : std::stoi(col[4]);
	data.targeting		= ParseTargeting(col[5]);
	data.upcastable		= (col[6] == "TRUE");
	data.effect_raw		= col[7];

	ParseEffectField(col[7], data);
	return data;
}

// Effect 4줄 템플릿 파싱:
//   Line 1: Deals {damage} damage.
//   Line 2: Applies "{STATUS}" status for {N} turns.
//   Line 3: Move to {location}.
//   Line 4: Summons {entity} at {location}.
void SpellSystem::ParseEffectField(const std::string& effect_str, SpellData& data) const
{
	std::istringstream ss(effect_str);
	std::string		   line;

	// Line 1: "Deals X damage." — 전체 formula 문자열 추출 후 ParseDamageFormula에 위임
	if (std::getline(ss, line))
	{
		const std::string prefix = "Deals ";
		const std::string suffix = " damage.";
		auto			  p		 = line.find(prefix);
		auto			  s		 = line.rfind(suffix); // rfind: suffix가 문장 끝에 있음
		if (p != std::string::npos && s != std::string::npos)
		{
			std::string full_formula = line.substr(p + prefix.size(), s - (p + prefix.size()));
			ParseDamageFormula(full_formula, data);
		}
		else
		{
			data.damage_formula = "0";
			data.upcast_dice	= "";
		}
	}

	// Line 2: Applies "STATUS" status for N turns.
	if (std::getline(ss, line))
	{
		// 따옴표 사이의 STATUS 이름 추출
		auto q1 = line.find('"');
		auto q2 = line.find('"', q1 + 1);
		if (q1 != std::string::npos && q2 != std::string::npos)
			data.effect_status = line.substr(q1 + 1, q2 - q1 - 1);
		else
			data.effect_status = "Basic";

		// "for N turns" 에서 N 추출
		auto ft = line.find("for ");
		auto tt = line.find(" turn", ft);
		if (ft != std::string::npos && tt != std::string::npos)
		{
			std::string n_str	 = line.substr(ft + 4, tt - (ft + 4));
			data.effect_duration = n_str.empty() ? 0 : std::stoi(n_str);
		}
		else
			data.effect_duration = 0;
	}

	// Line 3: "Move to {template}."
	if (std::getline(ss, line))
	{
		const std::string prefix = "Move to ";
		auto			  p		 = line.find(prefix);
		if (p != std::string::npos)
		{
			std::string loc = line.substr(p + prefix.size());
			if (!loc.empty() && loc.back() == '.')
				loc.pop_back();
			data.move = ParseMoveField(loc); // ← 구조체로 저장
		}
		else
		{
			data.move = { "self", "stay", 0 };
		}
	}

	// Line 4: "Summons {entity} at {location}."
	if (std::getline(ss, line))
	{
		const std::string prefix = "Summons ";
		auto			  p		 = line.find(prefix);
		auto			  at	 = line.find(" at ");
		if (p != std::string::npos && at != std::string::npos)
			data.summon_type = line.substr(p + prefix.size(), at - (p + prefix.size()));
		else
			data.summon_type = "NULL";
	}
	// Line 5+: "Special: ..." (선택적, 복수 줄 가능)
	while (std::getline(ss, line))
	{
		const std::string special_prefix = "Special:";
		if (line.rfind(special_prefix, 0) == 0)
		{
			std::string content = line.substr(special_prefix.size());
			// 앞 공백 제거
			auto		start	= content.find_first_not_of(" \t");
			if (start != std::string::npos)
				content = content.substr(start);

			// 복수 Special 줄은 "; "로 이어붙임
			if (data.special_effect.empty())
				data.special_effect = content;
			else
				data.special_effect += "; " + content;
		}
	}
}

void SpellSystem::ApplySpecialEffect(Character* caster, const SpellData& spell, int upcast_level)
{
	if (spell.special_effect.empty())
		return;

	// ── "Recover a Spell Slot of (formula) level." ──
	if (spell.special_effect.find("Recover a Spell Slot") != std::string::npos)
	{
		// 공식: Spell Level - Required Spell Level + 1
		int used_level	  = (upcast_level > 0) ? upcast_level : spell.spell_level;
		int restore_level = used_level - spell.spell_level + 1;
		restore_level	  = std::max(1, restore_level); // 최소 1레벨

		SpellSlots* slots = caster->GetSpellSlots();
		if (slots)
		{
			slots->RestoreOne(restore_level);
			Engine::GetLogger().LogEvent(caster->TypeName() + " recovered 1 level-" + std::to_string(restore_level) + " slot via Mana Conversion");
		}
	}
	// 향후 다른 Special 패턴은 여기 추가
}

int SpellSystem::CalculateSpellDamage(const SpellData& spell, int upcast_level)
{
	// ── 조기 반환: 피해 없음 ──
	if (spell.damage_formula == "0" || spell.damage_formula.empty())
	{
		// [Mana Conversion 패턴] damage_formula=="0" 이지만 upcast_dice가 있으면
		// "0 + (level_diff+1)d10" 의미 → 여기서 굴리고 반환
		if (!spell.upcast_dice.empty())
		{
			auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
			if (!dice)
				return 0;

			int level_diff = std::max(0, upcast_level - spell.spell_level) + 1; // +1: 기본도 1회

			auto d_pos = spell.upcast_dice.find('d');
			if (d_pos != std::string::npos)
			{
				int			per_level  = (d_pos > 0) ? std::stoi(spell.upcast_dice.substr(0, d_pos)) : 1;
				std::string face	   = spell.upcast_dice.substr(d_pos);				// "d10"
				std::string rolled_str = std::to_string(level_diff * per_level) + face; // "1d10", "3d10"
				return dice->RollDiceFromString(rolled_str);
			}
		}
		return 0; // upcast_dice도 없으면 진짜 0
	}

	// ── flat_per_level:N (Magic Missile) ──
	if (spell.damage_formula.rfind("flat_per_level:", 0) == 0)
	{
		int multiplier = std::stoi(spell.damage_formula.substr(15));
		int level_diff = std::max(0, upcast_level - spell.spell_level) + 1;
		return multiplier * level_diff;
	}

	auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
	if (!dice)
		return 0;

	int	 level_diff = std::max(0, upcast_level - spell.spell_level);
	bool negative	= (!spell.damage_formula.empty() && spell.damage_formula[0] == '-');

	std::string base_str = spell.damage_formula;
	if (negative)
		base_str = base_str.substr(2, base_str.size() - 3); // "-(X)" → "X"

	int total = dice->RollDiceFromString(base_str);

	// 업캐스트 보너스 다이스
	if (level_diff > 0 && spell.upcastable && !spell.upcast_dice.empty())
	{
		auto d_pos = spell.upcast_dice.find('d');
		if (d_pos != std::string::npos)
		{
			int			per_level  = (d_pos > 0) ? std::stoi(spell.upcast_dice.substr(0, d_pos)) : 1;
			std::string face	   = spell.upcast_dice.substr(d_pos);
			std::string rolled_str = std::to_string(level_diff * per_level) + face;
			total += dice->RollDiceFromString(rolled_str);
		}
	}

	return negative ? -total : total;
}

void SpellSystem::ApplySpellEffect(Character* caster, const SpellData& spell, Math::ivec2 target_tile, int upcast_level)
{
	auto* grid	 = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	auto* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
	if (!grid || !combat)
		return;

	// ─────────────────────────────────────────────────
	// 피해 대상 목록 결정 (Geometry 기반)
	// ─────────────────────────────────────────────────
	std::vector<Character*> targets;
	const SpellTargeting&	t = spell.targeting;

	if (t.geometry == "Self")
	{
		targets.push_back(caster);
	}
	else if (t.geometry == "Single" || t.geometry == "Point")
	{
		Character* hit = grid->GetCharacterAt(target_tile);
		if (hit)
		{
			bool sameTeam = (caster->IsAIControlled() == hit->IsAIControlled());
			bool filterOk = (t.filter == "Any")
			             || (t.filter == "Ally"  && sameTeam)
			             || (t.filter == "Enemy" && !sameTeam)
			             || (t.filter == "Self"  && hit == caster);
			if (filterOk)
				targets.push_back(hit);
		}
	}
	else if (t.geometry == "Around")
	{
		// 시전자 중심 원형 AOE
		int radius = (t.range < 0) ? 99 : t.range;
		for (auto* c : grid->GetAllCharacters())
		{
			if (!c || !c->IsAlive())
				continue;
			bool sameTeam = (caster->IsAIControlled() == c->IsAIControlled());
			if (t.filter == "Self"  && c != caster) continue;
			if (t.filter == "Ally"  && !sameTeam)   continue;
			if (t.filter == "Enemy" && sameTeam)     continue;
			int dist = grid->ManhattanDistance(caster->GetGridPosition()->Get(), c->GetGridPosition()->Get());
			if (dist <= radius)
				targets.push_back(c);
		}
	}
	else if (t.geometry == "Line")
	{
		// 4방향 직선 (십자 모양)
		const Math::ivec2 dirs[4] = {
			{  1,	 0 },
			  { -1,	0 },
			 {  0,  1 },
			   {	 0, -1 }
		};
		int reach = (t.range < 0) ? 99 : t.range;
		for (const auto& dir : dirs)
		{
			for (int step = 1; step <= reach; ++step)
			{
				Math::ivec2 tile = caster->GetGridPosition()->Get() + Math::ivec2{ dir.x * step, dir.y * step };
				if (!grid->IsValidTile(tile))
					break;
				// 벽 통과 불가
				if (grid->GetTileType(tile) == GridSystem::TileType::Wall)
					break;
				Character* hit = grid->GetCharacterAt(tile);
				if (hit && hit->IsAlive())
					targets.push_back(hit);
			}
		}
	}
	else if (t.geometry == "OddEven")
	{
		// 바둑판 패턴 — 홀수 or 짝수 타일의 모든 캐릭터
		// (플레이어가 홀/짝 선택하는 기능은 추후 구현, 현재는 전체 대상)
		for (auto* c : grid->GetAllCharacters())
		{
			if (!c || !c->IsAlive() || c == caster)
				continue;
			targets.push_back(c);
		}
	}

	// ─────────────────────────────────────────────────
	// 피해 적용
	// ─────────────────────────────────────────────────
	if (spell.damage_formula != "0" && !spell.damage_formula.empty())
	{
		auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();

		// [추가] 파티클 매니저 가져오기
		// 다른 파티클 사용 시 CS230::ParticleManager<Particles::Hit> 에서 Hit을 다른 파티클로 수정
		// EX) CS230::ParticleManager<Particles::Spell>
		auto* particleManager = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Hit>>();

		for (auto* tgt : targets)
		{
			int damage = CalculateSpellDamage(spell, upcast_level);

			// Weakpoint Strike: 대상이 디버프 상태이면 2d20으로 교체
			if (!spell.special_effect.empty() && spell.special_effect.find("If target is debuffed") != std::string::npos)
			{
				bool is_debuffed = tgt->Has("Curse") || tgt->Has("Fear") || tgt->Has("Exhaustion");
				if (is_debuffed && dice)
					damage = dice->RollDiceFromString("2d20");
			}

			if (damage > 0)
			{
				// 기존 데미지 적용 코드
				combat->ApplyDamage(caster, tgt, damage);

				// [추가] 피격당한 타겟이 드래곤이 아닐 경우 파티클 발생
				// if (tgt != nullptr && tgt->GetCharacterType() != CharacterTypes::Dragon)
				//{
				if (particleManager)
				{
					// 피격 대상의 위치(tgt->GetPosition())에 파티클 생성
					// 위치 그대로 생성하면 좌측 하단으로 파티클이 쏠림
					// particleManager->Emit(5, { tgt->GetPosition().x + 30, tgt->GetPosition().y + 30 }, { 0, 0 }, { 0, 100 }, 3.14159265f / 2.0f);
					particleManager->Emit(10, tgt->GetPosition() + Math::vec2{ GridSystem::TILE_SIZE / 2.0, GridSystem::TILE_SIZE / 2.0 }, { 0, 0 }, { 0, 100 }, 3.14159265f);
				}
				//}
			}
			else if (damage < 0)
			{
				int hpBefore   = tgt->GetHP();
				tgt->SetHP(std::min(hpBefore + (-damage), tgt->GetMaxHP()));
				int actualHeal = tgt->GetHP() - hpBefore;
				auto& gs       = Engine::GetGameStateManager();
				if (auto* eventBus = gs.GetGSComponent<EventBus>())
				  eventBus->Publish(CharacterHealedEvent{ tgt, actualHeal, tgt->GetHP(), tgt->GetMaxHP(), caster });
			}
		}
	}

	// ─────────────────────────────────────────────────
	// 상태 효과 적용 (기존 로직 유지, targets 루프로 통합)
	// ─────────────────────────────────────────────────
	if (spell.effect_status != "Basic" && spell.effect_duration > 0)
	{
		auto* handler = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();
		// Around/Line/OddEven은 이미 targets에 대상 목록이 있음
		// Single/Self 도 동일 루프 사용
		for (auto* tgt : targets)
		{
			tgt->AddEffect(spell.effect_status, spell.effect_duration);
			if (handler)
				handler->OnApplied(tgt, spell.effect_status);
		}
		// Around 타겟 없고 filter==Self 인 경우 (ex. Purify) 이미 Self geometry로 처리됨
	}

	// ─────────────────────────────────────────────────
	// 지형 / 특수 이동 (기존 유지)
	// ─────────────────────────────────────────────────
	if (spell.summon_type == "Wall")
	{
		// auto* grid         = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
		auto* tm			= Engine::GetGameStateManager().GetGSComponent<TurnManager>();
		int	  current_round = tm ? tm->GetRoundNumber() : 0;
		if (grid && grid->IsValidTile(target_tile) && grid->GetTileType(target_tile) == GridSystem::TileType::Empty && !grid->IsOccupied(target_tile))
		{
			grid->SetTileType(target_tile, GridSystem::TileType::Wall);
			m_terrain_effects.push_back({ { target_tile }, 0, current_round, spell.effect_duration });
			Engine::GetLogger().LogEvent("SpellSystem: Wall created at (" + std::to_string(target_tile.x) + "," + std::to_string(target_tile.y) + ")");
		}
	}
	else if (spell.summon_type == "Lava Zone")
	{
		// auto* grid         = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
		auto* tm = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
		if (!grid || !grid->IsValidTile(target_tile))
			return;

		int current_round = tm ? tm->GetRoundNumber() : 0;
		int damage		  = CalculateSpellDamage(spell, upcast_level);
		int duration	  = spell.effect_duration;

		grid->SetTileType(target_tile, GridSystem::TileType::Lava);
		m_terrain_effects.push_back({ { target_tile }, damage, current_round, duration });
		Engine::GetLogger().LogEvent(
			"SpellSystem: Lava Zone created at (" + std::to_string(target_tile.x) + "," + std::to_string(target_tile.y) + ") dmg=" + std::to_string(damage) + " duration=" + std::to_string(duration));
	}

	ApplyMoveEffect(caster, targets, spell, target_tile);

	// Special 효과 처리
	ApplySpecialEffect(caster, spell, upcast_level);
}

bool SpellSystem::CastSpell(Character* caster, const std::string& spell_id, Math::ivec2 target_tile, int upcast_level)
{
	if (!caster)
		return false;

	auto it = spells_.find(spell_id);
	if (it == spells_.end())
	{
		Engine::GetLogger().LogError("SpellSystem: Unknown spell id " + spell_id);
		return false;
	}

	const SpellData& spell = it->second;

	if (!CanCast(caster, spell_id, target_tile))
		return false;

	int consume_level = (upcast_level > 0) ? upcast_level : spell.spell_level;
	if (consume_level > 0)
		caster->ConsumeSpell(consume_level);

	// AP 소모 — 갓모드 Dragon은 AP 소모 없음
	{
		auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
		if (!(debug_mgr && debug_mgr->IsGodModeEnabled() && caster->GetCharacterType() == CharacterTypes::Dragon))
			caster->GetActionPointsComponent()->Consume(1);
	}

	// 1. 시전자가 드래곤인지 검사하여 파티클 생성
	// 시전자 무관 스펠 종류에 따라서 나타나는 파티클 분류 필요
	if (caster != nullptr && caster->GetCharacterType() == CharacterTypes::Dragon)
	{
		auto* particleManager = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Hit>>();
		if (particleManager)
		{
			particleManager->Emit(10, caster->GetPosition() + Math::vec2{ GridSystem::TILE_SIZE / 2.0, GridSystem::TILE_SIZE / 2.0 }, { 0, 0 }, { 0, 100 }, 3.14159265f / 2.0f);
		}
	}

	// 2. 람다(Lambda) 함수를 이용해 딜레이 후 실행할 작업 정의
	auto* gom = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();
	if (gom)
	{
		double delayTime = 0.5; // 0.5초 딜레이

		// this 포인터를 캡처하여 private 함수인 ApplySpellEffect에 접근합니다.
		auto callback = [this, caster, spell, target_tile, upcast_level]()
		{
			// 0.5초 뒤 시전자가 아직 살아있을 때만 데미지와 피격 파티클 적용
			if (caster != nullptr && caster->IsAlive())
			{
				this->ApplySpellEffect(caster, spell, target_tile, upcast_level);
			}
		};

		// 타이머 객체 등록 (지정된 시간 뒤에 위 callback이 실행됨)
		gom->Add(std::unique_ptr<CS230::GameObject>(new SpellDelayObject(delayTime, callback)));
	}

	Engine::GetLogger().LogEvent(caster->TypeName() + " cast " + spell.spell_name + " [" + spell_id + "]");

	// SpellCastEvent 발행 — UI 스펠 로그 표시용
	auto& gs = Engine::GetGameStateManager();
	if (auto* eventBus = gs.GetGSComponent<EventBus>())
	{
		eventBus->Publish(SpellCastEvent{ caster, spell.spell_name, upcast_level, target_tile, consume_level });
	}

	return true;
}

bool SpellSystem::HasSpell(const std::string& spell_id) const
{
	return spells_.count(spell_id) > 0;
}

std::vector<std::string> SpellSystem::GetAvailableSpells(Character* caster) const
{
	std::vector<std::string> available_spells;
	for (const auto& pair : spells_)
	{
		// 타겟 미확정이므로 시전자 위치를 사용 → 거리=0, 사거리 체크 항상 통과
		if (CanCast(caster, pair.first, caster->GetGridPosition()->Get()))
		{
			available_spells.push_back(pair.first);
		}
	}
	return available_spells;
}

bool SpellSystem::CanCast(Character* caster, const std::string& spell_id, Math::ivec2 target_tile, int upcast_level) const
{
	if (!caster)
		return false;

	auto it = spells_.find(spell_id);
	if (it == spells_.end())
		return false;

	const SpellData& spell = it->second;

	// 클래스 사용 가능 여부
	if (std::find(spell.usable_classes.begin(), spell.usable_classes.end(), caster->TypeName()) == spell.usable_classes.end())
		return false;

	// 주문 슬롯 여부
	if (upcast_level > 0)
	{
		// 특정 레벨로 시전: 해당 레벨 >= spell_level 이고 슬롯 존재 확인
		if (upcast_level < spell.spell_level)
			return false;
		if (!caster->HasSpellSlot(upcast_level))
			return false;
	}
	else if (spell.spell_level > 0)
	{
		// GetAvailableSpells 호출 (레벨 미지정): spell_level 이상 슬롯 하나라도 있으면 OK
		bool has_any = false;
		for (int lv = spell.spell_level; lv <= 5; ++lv)
			if (caster->HasSpellSlot(lv))
			{
				has_any = true;
				break;
			}
		if (!has_any)
			return false;
	}

	const SpellTargeting& t = spell.targeting;

	// Self / OddEven 은 사거리 무관 (항상 허용)
	bool range_ok = (t.geometry == "Self" || t.geometry == "OddEven" || t.range < 0);
	if (!range_ok)
	{
		auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
		if (!grid)
			return false;
		int dist = grid->ManhattanDistance(caster->GetGridPosition()->Get(), target_tile);
		if (dist > t.range)
			return false;
	}

	// 사거리 체크 이후, return true 바로 앞에 추가
	if (caster->GetActionPoints() < 1)
		return false;

	// Ally/Enemy/Self/Empty 필터 검증 (Single/Point 지오메트리)
	if (t.geometry == "Single" || t.geometry == "Point")
	{
		auto* grid_check = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
		if (grid_check)
		{
			Character* hit = grid_check->GetCharacterAt(target_tile);
			if (t.filter == "Empty")
			{
				if (hit != nullptr)
					return false;
			}
			else if (t.filter == "Self")
			{
				if (hit != caster)
					return false;
			}
			else if (t.filter == "Ally" || t.filter == "Enemy")
			{
				if (!hit)
					return false;
				bool sameTeam = (caster->IsAIControlled() == hit->IsAIControlled());
				if (t.filter == "Ally"  && !sameTeam) return false;
				if (t.filter == "Enemy" && sameTeam)  return false;
			}
		}
	}

	return true;
}

const SpellData* SpellSystem::GetSpellData(const std::string& spell_id) const
{
	auto it = spells_.find(spell_id);
	return (it != spells_.end()) ? &(it->second) : nullptr;
}

void SpellSystem::ParseDamageFormula(const std::string& formula_str, SpellData& data) const
{
	// 힐링: "-(...)" — 음수 래퍼 처리
	bool		negative = (formula_str.size() >= 2 && formula_str[0] == '-' && formula_str[1] == '(');
	std::string f		 = negative ? formula_str.substr(2, formula_str.size() - 3) // "-(X)" → "X"
									: formula_str;

	// " + " 로 베이스와 업캐스트 분리
	auto plus_pos = f.find(" + ");
	if (plus_pos == std::string::npos)
	{
		// 업캐스트 없음: 전체가 베이스
		data.damage_formula = negative ? "-(" + f + ")" : f;
		data.upcast_dice	= "";
		return;
	}

	std::string base_part	= f.substr(0, plus_pos);  // "2d8", "(...)d10" 등
	std::string upcast_part = f.substr(plus_pos + 3); // "(...)d6" 또는 "(...) * 2d6"

	data.damage_formula = negative ? "-(" + base_part + ")" : base_part;

	// 업캐스트 주사위 추출
	// 패턴 A: "(...) * NdX" → "NdX"
	auto star_pos = upcast_part.find("* ");
	if (star_pos != std::string::npos)
	{
		data.upcast_dice = upcast_part.substr(star_pos + 2); // "2d6", "1d20"
		return;
	}
	// 패턴 B: "(...)dX" → "1dX"
	auto d_pos = upcast_part.rfind('d');
	if (d_pos != std::string::npos)
	{
		data.upcast_dice = "1" + upcast_part.substr(d_pos); // "1d6", "1d8"
		return;
	}

	data.upcast_dice = "";
}

SpellMove SpellSystem::ParseMoveField(const std::string& move_str) const
{
	// "current location" → {self, stay, 0}
	if (move_str == "current location" || move_str.empty())
		return { "self", "stay", 0 };

	// "target:knockback:2" → {target, knockback, 2}
	// "self:teleport:selected" → {self, teleport, -1}
	auto	  parts = SplitByDelimiter(move_str, ':');
	SpellMove m;
	m.mover		= (parts.size() > 0) ? parts[0] : "self";
	m.move_type = (parts.size() > 1) ? parts[1] : "stay";

	if (parts.size() > 2)
	{
		if (parts[2] == "selected")
			m.distance = -1; // target_tile 사용
		else
		{
			try
			{
				m.distance = std::stoi(parts[2]);
			}
			catch (...)
			{
				m.distance = 0;
			}
		}
	}
	else
	{
		m.distance = 0;
	}
	return m;
}

void SpellSystem::TickTerrainEffects(int current_round)
{
	auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!grid)
		return;

	m_terrain_effects.erase(
		std::remove_if(
			m_terrain_effects.begin(), m_terrain_effects.end(),
			[&](const TerrainEffect& e)
			{
				if (current_round >= e.created_round + e.duration_rounds)
				{
					for (const auto& tile : e.affected_tiles)
						grid->SetTileType(tile, GridSystem::TileType::Empty);
					return true;
				}
				return false;
			}),
		m_terrain_effects.end());
}

int SpellSystem::GetLavaDamageAt(Math::ivec2 tile) const
{
	for (const auto& effect : m_terrain_effects)
	{
		if (effect.damage_per_turn <= 0)
			continue;
		for (const auto& t : effect.affected_tiles)
			if (t.x == tile.x && t.y == tile.y)
				return effect.damage_per_turn;
	}
	return 0;
}

bool SpellSystem::CastWalls(Character* caster, const std::string& spell_id, const std::vector<Math::ivec2>& tiles, int upcast_level)
{
	if (!caster || tiles.empty())
		return false;

	auto it = spells_.find(spell_id);
	if (it == spells_.end())
		return false;
	const SpellData& spell = it->second;

	auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	auto* tm   = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
	if (!grid || !tm)
		return false;

	int consume_level = (upcast_level > 0) ? upcast_level : spell.spell_level;
	if (consume_level > 0)
		caster->ConsumeSpell(consume_level);
	{
		auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
		if (!(debug_mgr && debug_mgr->IsGodModeEnabled() && caster->GetCharacterType() == CharacterTypes::Dragon))
			caster->GetActionPointsComponent()->Consume(1);
	}

	int current_round = tm->GetRoundNumber();

	for (const auto& tile : tiles)
	{
		if (grid->IsValidTile(tile) && grid->GetTileType(tile) == GridSystem::TileType::Empty && !grid->IsOccupied(tile))
		{
			grid->SetTileType(tile, GridSystem::TileType::Wall);
			m_terrain_effects.push_back({ { tile }, 0, current_round, spell.effect_duration });
		}
	}

	Engine::GetLogger().LogEvent(caster->TypeName() + " cast Wall Creation: " + std::to_string(tiles.size()) + " wall(s)");
	return true;
}

bool SpellSystem::CastLavaZones(Character* caster, const std::string& spell_id, const std::vector<Math::ivec2>& tiles, int upcast_level)
{
	if (!caster || tiles.empty())
		return false;

	auto it = spells_.find(spell_id);
	if (it == spells_.end())
		return false;
	const SpellData& spell = it->second;

	auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	auto* tm   = Engine::GetGameStateManager().GetGSComponent<TurnManager>();
	if (!grid || !tm)
		return false;

	int consume_level = (upcast_level > 0) ? upcast_level : spell.spell_level;
	if (consume_level > 0)
		caster->ConsumeSpell(consume_level);
	{
		auto* debug_mgr = Engine::GetGameStateManager().GetGSComponent<DebugManager>();
		if (!(debug_mgr && debug_mgr->IsGodModeEnabled() && caster->GetCharacterType() == CharacterTypes::Dragon))
			caster->GetActionPointsComponent()->Consume(1);
	}

	int current_round = tm->GetRoundNumber();
	int damage		  = CalculateSpellDamage(spell, upcast_level);

	for (const auto& tile : tiles)
	{
		if (grid->IsValidTile(tile) && grid->GetTileType(tile) == GridSystem::TileType::Empty && !grid->IsOccupied(tile))
		{
			grid->SetTileType(tile, GridSystem::TileType::Lava);
			m_terrain_effects.push_back({ { tile }, damage, current_round, spell.effect_duration });
		}
	}

	Engine::GetLogger().LogEvent(caster->TypeName() + " cast Magma Blast: " + std::to_string(tiles.size()) + " lava zone(s)");
	return true;
}

void SpellSystem::ApplyMoveEffect(Character* caster, const std::vector<Character*>& targets, const SpellData& spell, Math::ivec2 target_tile)
{
	if (spell.move.move_type == "stay")
		return;

	auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!grid)
		return;

	// ── knockback: 피격 대상을 시전자 기준 밀쳐냄 ──
	if (spell.move.move_type == "knockback")
	{
		int			dist	   = spell.move.distance;
		Math::ivec2 caster_pos = caster->GetGridPosition()->Get();

		for (auto* tgt : targets)
		{
			if (!tgt || tgt == caster)
				continue;
			Math::ivec2 tgt_pos = tgt->GetGridPosition()->Get();

			// 밀쳐내는 방향: 시전자 → 대상 방향
			int dx = tgt_pos.x - caster_pos.x;
			int dy = tgt_pos.y - caster_pos.y;

			// 지배적인 축 기준 단위 벡터 (대각선 불가)
			Math::ivec2 dir{ 0, 0 };
			if (std::abs(dx) >= std::abs(dy))
				dir.x = (dx >= 0) ? 1 : -1;
			else
				dir.y = (dy >= 0) ? 1 : -1;

			// 가능한 만큼 이동 (벽/맵 끝에서 멈춤)
			Math::ivec2 new_pos = tgt_pos;
			for (int step = 0; step < dist; ++step)
			{
				Math::ivec2 next{ new_pos.x + dir.x, new_pos.y + dir.y };
				if (!grid->IsValidTile(next))
					break;
				if (!grid->IsWalkable(next))
					break;
				if (grid->IsOccupied(next))
					break;
				new_pos = next;
			}

			if (new_pos.x != tgt_pos.x || new_pos.y != tgt_pos.y)
			{
				grid->MoveCharacter(tgt_pos, new_pos);
				tgt->SetGridPosition(new_pos);
			}
		}
	}

	// ── teleport (self:teleport:selected): 시전자를 target_tile로 이동 ──
	else if (spell.move.move_type == "teleport" && spell.move.mover == "self")
	{
		Math::ivec2 caster_pos = caster->GetGridPosition()->Get();
		Math::ivec2 dest	   = (spell.move.distance == -1) ? target_tile : target_tile;
		// Empty:Point 타겟팅이 이미 빈 타일만 선택하게 보장함

		if (grid->IsValidTile(dest) && !grid->IsOccupied(dest))
		{
			grid->MoveCharacter(caster_pos, dest);
			caster->SetGridPosition(dest);
			Engine::GetLogger().LogEvent(caster->TypeName() + " teleported to (" + std::to_string(dest.x) + ", " + std::to_string(dest.y) + ")");
		}
	}
}
