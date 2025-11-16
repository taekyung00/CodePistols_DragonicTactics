/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  CharacterTypes.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 7, 2025
*/
#include <string>
#include <map>
#include <vector>
#pragma once
enum class CharacterTypes
{
    None,
    Dragon,
    Fighter,
    Rogue,
    Cleric,
    Wizard,
    Count
};

struct CharacterStats {
    std::string character_type;
    int max_hp = 10;
    int current_hp = 10;
    int base_attack = 1;
    std::string attack_dice = "1d6";
    int base_defend = 1;
    std::string defend_dice = "1d4";
    int speed = 5;
    int attack_range = 1;
    std::map<int, int> spell_slots;              // level -> count
    std::vector<std::string> known_spells;       // ["Fireball", "CreateWall", ...]
    std::vector<std::string> known_abilities;    // ["MeleeAttack", "ShieldBash"]
};

struct SpellData {
    std::string spell_name;
    int spell_level;
    std::string spell_school;
    int range;
    std::string target_type;
    int area_radius;
    std::string base_damage;
    std::string damage_type;
    std::string upcast_bonus;
    std::string description;
};