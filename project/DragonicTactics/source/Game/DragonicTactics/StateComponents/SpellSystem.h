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


struct SpellData
{
    std::string id;           // "S_ATK_010"
    std::string spell_name;   // "화염탄"
    std::string spell_school; // "공격", "버프", "디버프", "강화", "지형"
    int         spell_level;  // 요구 슬롯 레벨 (0 = 슬롯 불필요)
    std::string target_type;  // "적 단일", "아군 단일", "자신", "범위"
    int         range;        // 타일 수 ("-"는 0으로 처리)
    bool        upcastable;   // 업캐스트 가능 여부
    std::string effect_desc;  // 효과 설명 전체
    std::string base_damage;  // "2d6", "3d8+2" 형식 (없으면 빈 문자열)

    std::vector<std::string> usable_classes; // ["Dragon", "Wizard"] — '/' 구분 파싱
};


class SpellSystem : public CS230::Component
{
public:
    void LoadFromCSV(const std::string& csv_path);

    bool HasSpell(const std::string& spell_id) const;
    std::vector<std::string> GetAvailableSpells(Character* caster) const;
    bool CanCast(Character* caster, const std::string& spell_id,
                 Math::ivec2 target_tile) const;

    // AISystem::ExecuteDecision이 이 이름으로 호출 — 이름 변경 금지
    bool CastSpell(Character* caster, const std::string& spell_id,
                   Math::ivec2 target_tile, int upcast_level = 0);

    const SpellData* GetSpellData(const std::string& spell_id) const;

private:
    std::map<std::string, SpellData> spells_;

    SpellData                ParseCSVRow(const std::vector<std::string>& columns) const;
    std::vector<std::string> SplitByDelimiter(const std::string& str, char delim) const;
    int                      ParseRange(const std::string& range_str) const;

    void ApplySpellEffect(Character* caster, const SpellData& spell,
                          Math::ivec2 target_tile, int upcast_level);
    int  CalculateSpellDamage(const SpellData& spell, Character* caster, int upcast_level);
};