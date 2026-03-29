/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

// File: CS230/Game/System/SpellSystem.cpp
#include "../Objects/Character.h"
#include "../StateComponents/CombatSystem.h"
#include "../StateComponents/DiceManager.h"
#include "../StateComponents/EventBus.h"
#include "../StateComponents/GridSystem.h"
#include "../Types/Events.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Logger.h"
#include "Engine/Path.h"
#include "Game/DragonicTactics/StateComponents/StatusEffectHandler.h"
#include "SpellSystem.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"

std::vector<std::string> SpellSystem::SplitByDelimiter(const std::string& str, char delim) const
{
  std::vector<std::string> result;
  std::stringstream		   ss(str);
  std::string			   token;
  while (std::getline(ss, token, delim))
  {
	size_t start = token.find_first_not_of(" \t\r\n");
	size_t end	 = token.find_last_not_of(" \t\r\n");
	if (start != std::string::npos)
	  result.push_back(token.substr(start, end - start + 1));
  }
  return result;
}

int SpellSystem::ParseRange(const std::string& range_str) const
{
  if (range_str.empty() || range_str == "-")
	return 0;

  std::string digits;
  for (char c : range_str)
  {
	if (std::isdigit(static_cast<unsigned char>(c)))
	  digits += c;
  }
  return digits.empty() ? 0 : std::stoi(digits);
}

std::vector<std::string> SpellSystem::ReadCSVRecord(std::ifstream& file) const
{
  std::vector<std::string> columns;
  std::string			   cell;
  bool					   in_quotes = false;
  char					   c;

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
  std::ifstream				  file(full_path);
  if (!file.is_open())
  {
	Engine::GetLogger().LogError("SpellSystem: Failed to open " + csv_path);
	return;
  }

  ReadCSVRecord(file); // 헤더 스킵

  while (file.good())
  {
	auto columns = ReadCSVRecord(file);
	if (columns.size() < 9 || columns[0].empty())
	  continue;

	SpellData data	 = ParseCSVRow(columns);
	spells_[data.id] = data;
  }
  Engine::GetLogger().LogEvent("SpellSystem: Loaded " + std::to_string(spells_.size()) + " spells");
}

SpellData SpellSystem::ParseCSVRow(const std::vector<std::string>& col) const
{
  SpellData data;
  data.id			  = col[0];
  data.spell_name	  = col[1];
  data.category		  = col[2];
  data.usable_classes = SplitByDelimiter(col[3], ','); // "Dragon, Fighter" → vector
  data.spell_level	  = col[4].empty() ? 0 : std::stoi(col[4]);
  data.target_type	  = col[5];
  data.range		  = ParseRange(col[6]); // "4 spaces" → 4
  data.upcastable	  = (col[7] == "TRUE");
  data.effect_raw	  = col[8];

  ParseEffectField(col[8], data);
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
  std::string		 line;

  // Line 1: "Deals X damage."
  if (std::getline(ss, line))
  {
	// "Deals " 이후 " damage." 이전 문자열 추출
	const std::string prefix = "Deals ";
	const std::string suffix = " damage.";
	auto			  p		 = line.find(prefix);
	auto			  s		 = line.find(suffix);
	if (p != std::string::npos && s != std::string::npos)
	  data.damage_formula = line.substr(p + prefix.size(), s - (p + prefix.size()));
	else
	  data.damage_formula = "0";
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
	auto tt = line.find(" turns", ft);
	if (ft != std::string::npos && tt != std::string::npos)
	{
	  std::string n_str	   = line.substr(ft + 4, tt - (ft + 4));
	  data.effect_duration = n_str.empty() ? 0 : std::stoi(n_str);
	}
	else
	  data.effect_duration = 0;
  }

  // Line 3: "Move to {location}."
  if (std::getline(ss, line))
  {
	const std::string prefix = "Move to ";
	auto			  p		 = line.find(prefix);
	if (p != std::string::npos)
	{
	  std::string loc = line.substr(p + prefix.size());
	  if (!loc.empty() && loc.back() == '.')
		loc.pop_back();
	  data.move_type = loc; // "current location", "furthest position..."
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
}

int SpellSystem::CalculateSpellDamage(const SpellData& spell, int upcast_level)
{
  if (spell.damage_formula == "0" || spell.damage_formula.empty())
	return 0;

  auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
  if (!dice)
	return 0;

  int level_diff = std::max(0, upcast_level - spell.spell_level);

  std::string base_dice = spell.damage_formula.substr(0, spell.damage_formula.find(' '));
  int		  base		= dice->RollDiceFromString(base_dice);

  if (level_diff > 0 && spell.upcastable)
	base += dice->RollDiceFromString("1d8") * level_diff; // TODO: 스펠별 업캐스트 다이스 파싱

  return base;
}

void SpellSystem::ApplySpellEffect(Character* caster, const SpellData& spell, Math::ivec2 target_tile, int upcast_level)
{
  auto* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  auto* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
  if (!grid || !combat)
	return;

  Character* target = grid->GetCharacterAt(target_tile);

  // ── 피해 ──
  if (spell.damage_formula != "0" && !spell.damage_formula.empty())
  {
	int damage = CalculateSpellDamage(spell, upcast_level);

	if (damage > 0 && target)
	{
	  combat->ApplyDamage(caster, target, damage);
	}
	else if (damage < 0 && target) // 음수 = 회복 (S_ENH_030 Healing Touch)
	{
	  int heal = -damage;
	  target->SetHP(std::min(target->GetHP() + heal, target->GetMaxHP()));
	}
  }

  // ── 상태 효과 ──
  if (spell.effect_status != "Basic" && spell.effect_duration > 0)
  {
	auto*	   handler	 = Engine::GetGameStateManager().GetGSComponent<StatusEffectHandler>();
	Character* se_target = target ? target : caster;

	if (spell.target_type == "Enemies Around Caster" || spell.target_type == "All Enemies in Straight Line")
	{
	  // 범위 디버프: 시전자 주변 spell.range칸 내 모든 적
	  for (auto* c : grid->GetAllCharacters())
	  {
		if (!c || !c->IsAlive() || c == caster)
		  continue;
		int dist = grid->ManhattanDistance(caster->GetGridPosition()->Get(), c->GetGridPosition()->Get());
		if (dist <= spell.range)
		{
		  c->AddEffect(spell.effect_status, spell.effect_duration);
		  // 즉시 실행 효과 (Purify 등) 위임
		  if (handler)
			handler->OnApplied(c, spell.effect_status);
		}
	  }
	}
	else if (se_target)
	{
	  se_target->AddEffect(spell.effect_status, spell.effect_duration);
	  // 즉시 실행 효과 (Purify 등) 위임
	  if (handler)
		handler->OnApplied(se_target, spell.effect_status);
	}
  }

  // ── 지형 생성 (summon_type != "NULL") ──
  if (spell.summon_type != "NULL" && !spell.summon_type.empty())
  {
	// TODO: Terrain 시스템 구현 후 연동
	Engine::GetLogger().LogEvent("SpellSystem: Summon " + spell.summon_type + " at (" + std::to_string(target_tile.x) + ", " + std::to_string(target_tile.y) + ")");
  }

  // ── 특수 이동 ──
  if (spell.move_type != "current location" && !spell.move_type.empty())
  {
	// TODO: 텔레포트 등 특수 이동 구현 (S_GEO_030)
	Engine::GetLogger().LogEvent("SpellSystem: Special move — " + spell.move_type);
  }
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

  caster->ConsumeSpell(spell.spell_level);

  // AP 소모 (신규) — CanCast에서 이미 >= 1 보장됨
  caster->GetActionPointsComponent()->Consume(1);

  ApplySpellEffect(caster, spell, target_tile, upcast_level);

  Engine::GetLogger().LogEvent(caster->TypeName() + " cast " + spell.spell_name + " [" + spell_id + "]");
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

bool SpellSystem::CanCast(Character* caster, const std::string& spell_id, Math::ivec2 target_tile) const
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
  if (spell.spell_level > 0 && !caster->HasSpellSlot(spell.spell_level))
	return false;

  // 사거리 내 타일 여부
  auto* grid = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
  if (!grid)
	return false;

  int dist = grid->ManhattanDistance(caster->GetGridPosition()->Get(), target_tile);
  if (dist > spell.range)
	return false;

  // 사거리 체크 이후, return true 바로 앞에 추가
  if (caster->GetActionPoints() < 1)
	return false;

  return true;
}

const SpellData* SpellSystem::GetSpellData(const std::string& spell_id) const
{
  auto it = spells_.find(spell_id);
  return (it != spells_.end()) ? &(it->second) : nullptr;
}
