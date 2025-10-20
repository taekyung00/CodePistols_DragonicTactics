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
    int max_hp = 10;
    int current_hp = 10;
    int base_attack = 1;
    std::string attack_dice = "1d6";
    int base_defend = 1;
    std::string defend_dice = "1d4";
    int speed = 5;
    int attack_range = 1;
};