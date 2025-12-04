#pragma once
#include "Engine/Vec2.h"
#include "Engine/Component.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"

class Character;
class Dragon;
class Fighter;

class CharacterFactory : public CS230::Component{
public:
    static std::unique_ptr<Character> Create(CharacterTypes type, Math::ivec2 start_position);

    static std::unique_ptr<Character> CreateWithStats(
        CharacterTypes type,
        Math::ivec2 start_position,
        const CharacterStats& custom_stats
    );

private:
    static std::unique_ptr<Dragon> CreateDragon(Math::ivec2 position);
    static std::unique_ptr<Fighter> CreateFighter(Math::ivec2 position);
};