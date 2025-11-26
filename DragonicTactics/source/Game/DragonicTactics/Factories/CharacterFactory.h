#pragma once
#include "Engine/Vec2.hpp"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

class Character;
class Dragon;
class Fighter;

class CharacterFactory {
public:
    static Character* Create(CharacterTypes type, Math::ivec2 start_position);

    static Character* CreateWithStats(
        CharacterTypes type,
        Math::ivec2 start_position,
        const CharacterStats& custom_stats
    );

private:
    static Dragon* CreateDragon(Math::ivec2 position);
    static Fighter* CreateFighter(Math::ivec2 position);
};