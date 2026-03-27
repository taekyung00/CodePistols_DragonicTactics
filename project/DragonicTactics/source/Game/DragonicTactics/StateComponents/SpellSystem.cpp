/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

// File: CS230/Game/System/SpellSystem.cpp
#include "../Objects/Character.h"
#include "../StateComponents/EventBus.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/CombatSystem.h"
#include "../StateComponents/DiceManager.h"
#include "../Types/Events.h"
#include "./Engine/Engine.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Logger.h"
#include "SpellSystem.h"

std::vector<std::string> SpellSystem::SplitByDelimiter(const std::string& str,
                                                        char delim) const
{
    std::vector<std::string> result;
    std::stringstream        ss(str);
    std::string              token;
    while (std::getline(ss, token, delim))
    {
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end   = token.find_last_not_of(" \t\r\n");
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

void SpellSystem::LoadFromCSV(const std::string& csv_path)
{
    std::ifstream file(csv_path);
    if (!file.is_open())
    {
        Engine::GetLogger().LogError("SpellSystem: Failed to open " + csv_path);
        return;
    }

    std::string line;
    bool        first_line = true;
    while (std::getline(file, line))
    {
        if (first_line) { first_line = false; continue; } // 헤더 스킵
        if (line.empty()) continue;

        auto columns = SplitByDelimiter(line, ',');
        if (columns.size() < 9 || columns[0].empty()) continue;

        SpellData data   = ParseCSVRow(columns);
        spells_[data.id] = data;
    }
    Engine::GetLogger().LogEvent(
        "SpellSystem: Loaded " + std::to_string(spells_.size()) + " spells");
}

SpellData SpellSystem::ParseCSVRow(const std::vector<std::string>& col) const
{
    SpellData data;
    data.id             = col[0];
    data.spell_name     = col[1];
    data.spell_school   = col[2];
    data.usable_classes = SplitByDelimiter(col[3], '/'); // "Dragon/Wizard" → vector
    data.spell_level    = col[4].empty() ? 0 : std::stoi(col[4]);
    data.target_type    = col[5];
    data.range          = ParseRange(col[6]);
    data.upcastable     = (col[7] == "TRUE" || col[7] == "true" || col[7] == "O");
    data.effect_desc    = col[8];
    return data;
}

int SpellSystem::CalculateSpellDamage(const SpellData& spell,
                                      [[maybe_unused]] Character* caster, int upcast_level)
{
    if (spell.base_damage.empty())
        return 0;

    auto* dice = Engine::GetGameStateManager().GetGSComponent<DiceManager>();
    if (!dice)
        return 0;

    int base = dice->RollDiceFromString(spell.base_damage);

    // 업캐스트: 레벨 차이당 1d6 추가 (스펠별로 규칙이 다르면 이 부분을 수정)
    if (upcast_level > spell.spell_level)
    {
        int extra = upcast_level - spell.spell_level;
        base += dice->RollDiceFromString("1d6") * extra;
    }

    return base;
}

void SpellSystem::ApplySpellEffect(Character* caster, const SpellData& spell,
                                    Math::ivec2 target_tile, int upcast_level)
{
    auto* grid   = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
    auto* combat = Engine::GetGameStateManager().GetGSComponent<CombatSystem>();
    if (!grid || !combat)
        return;

    Character* target = grid->GetCharacterAt(target_tile);

    if (spell.spell_school == "공격" && target)
    {
        int   damage = CalculateSpellDamage(spell, caster, upcast_level);
        combat->ApplyDamage(caster, target, damage);
    }
    else if (spell.spell_school == "버프" && target)
    {
        // TODO: Week 6+ StatusEffect 시스템 연동 후 구현
        Engine::GetLogger().LogEvent("SpellSystem: Buff stub — " + spell.id);
    }
}

bool SpellSystem::CastSpell(Character* caster, const std::string& spell_id,
                              Math::ivec2 target_tile, int upcast_level)
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
    ApplySpellEffect(caster, spell, target_tile, upcast_level);

    Engine::GetLogger().LogEvent(
        caster->TypeName() + " cast " + spell.spell_name + " [" + spell_id + "]");
    return true;
}