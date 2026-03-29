/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
// #include "../Spells/SpellBase.h"
#include "../../../Engine/Vec2.h"
#include "./Engine/Component.h"
#include "./Game/DragonicTactics/Test/Week3TestMocks.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Character;
class EventBus;

struct SpellMove
{
  std::string mover;	 // "self", "target"
  std::string move_type; // "stay", "knockback", "teleport"
  int		  distance;	 // 거리. "selected" 이면 -1 (target_tile 사용)
};

struct SpellTargeting
{
  std::string filter;	// "Enemy", "Ally", "Self", "Any", "Empty"
  std::string geometry; // "Single", "Around", "Line", "OddEven", "Point"
  int		  range;	// 타일 수. -1 = 무한
};

struct SpellData
{
  // ── CSV 컬럼 (col[0]~col[8]) ──
  std::string	 id;		  // col[0]  "S_ATK_050"
  std::string	 spell_name;  // col[1]  "Smite"
  std::string	 category;	  // col[2]  "Attack", "Buff", "Terrain Change" (빈 값 허용)
  int			 spell_level; // col[4]  요구 슬롯 레벨 (0 = 슬롯 불필요)
  SpellTargeting targeting;
  bool			 upcastable; //   TRUE / FALSE

  std::vector<std::string> usable_classes; // col[3]  ["Dragon", "Fighter"] — ", " 구분

  // ── Effect 템플릿 파싱 결과 (col[8] → ParseEffectField) ──
  std::string damage_formula;  // "3d8", "0", "-(1d10)", "8 * (Spell Level + 1 - ...)"
  std::string effect_status;   // status_effect.csv의 NAME. "Basic" = 상태 없음
  int		  effect_duration; // 상태 지속 턴 ("Basic"이면 0)
  SpellMove	  move;			   // ← move_type 문자열 대신 파싱된 구조체
							   //   std::string move_type;	   // "current location", "furthest position from the Dragon..."
  std::string summon_type;	   // "NULL", "Lava Zone", "Wall"

  std::string effect_raw;	  // 파싱 전 원본 Effect 문자열 (디버그/툴팁용)
  std::string special_effect; // "Special:" 줄 내용. 없으면 빈 문자열
  std::string upcast_dice;	  // 레벨 차이당 굴리는 주사위. "1d6", "2d6" 등. 없으면 빈 문자열
};

class SpellSystem : public CS230::Component
{
  public:
  void LoadFromCSV(const std::string& csv_path);

  bool					   HasSpell(const std::string& spell_id) const;
  std::vector<std::string> GetAvailableSpells(Character* caster) const;
  bool					   CanCast(Character* caster, const std::string& spell_id, Math::ivec2 target_tile, int upcast_level = 0) const;
  bool					   CastSpell(Character* caster, const std::string& spell_id, Math::ivec2 target_tile, int upcast_level = 0);

  const SpellData* GetSpellData(const std::string& spell_id) const;

  private:
  std::map<std::string, SpellData> spells_;

  std::vector<std::string> ReadCSVRecord(std::ifstream& file) const;
  SpellData				   ParseCSVRow(const std::vector<std::string>& columns) const;
  void					   ParseEffectField(const std::string& effect_str, SpellData& data) const;
  std::vector<std::string> SplitByDelimiter(const std::string& str, char delim) const;

  private:

  SpellTargeting ParseTargeting(const std::string& targeting_str) const; // ← 신규


  void		ApplySpellEffect(Character* caster, const SpellData& spell, Math::ivec2 target_tile, int upcast_level);
  int		CalculateSpellDamage(const SpellData& spell, int upcast_level);
  void		ApplySpecialEffect(Character* caster, const SpellData& spell, int upcast_level);
  void		ParseDamageFormula(const std::string& formula_str, SpellData& data) const;
  SpellMove ParseMoveField(const std::string& move_str) const;
  void		ApplyMoveEffect(Character* caster, const std::vector<Character*>& targets, const SpellData& spell, Math::ivec2 target_tile);
};