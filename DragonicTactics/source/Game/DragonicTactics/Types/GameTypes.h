/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameTypes.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 08, 2025
*/

#pragma once

enum class Direction {
    North, West, East, South
};

enum class TurnGoal {
    Attack,
    DebuffEnemy,
    HealAlly,
    BuffAlly,
    Reposition,
    Flee,
    None
};