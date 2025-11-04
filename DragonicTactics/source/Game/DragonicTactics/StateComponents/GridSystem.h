#pragma once
#include "./Engine/Component.h"
#include "./Engine/Vec2.hpp"
#include "./Game/DragonicTactics/Objects/Character.h"
#include "./Game/DragonicTactics/States/Test.h"
#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include <map>

class GridSystem : public CS230::Component
{
public:
    enum class TileType
    {
        Empty,
        Wall,
        Lava,
        Difficult,
        Invalid
    };

private:
    static const int MAP_WIDTH  = 8;
    static const int MAP_HEIGHT = 8;
    TileType         tile_grid[MAP_HEIGHT][MAP_WIDTH];
    Character*       character_grid[MAP_HEIGHT][MAP_WIDTH]; // instead of std::map<Math::vec2, Character*> occupiedTiles;
public:
    static const int TILE_SIZE = MAP_WIDTH * MAP_HEIGHT;

    GridSystem();

    // validation methods
    bool     IsValidTile(Math::ivec2 tile) const;
    bool     IsWalkable(Math::ivec2 tile) const;
    bool     IsOccupied(Math::ivec2 tile) const;
    void     SetTileType(Math::ivec2 tile, TileType type);
    TileType GetTileType(Math::ivec2 tile) const;

    // Character placement
    void       AddCharacter(Character* character, Math::ivec2 pos); //instead of void PlaceCharacter(Character* character, Math::vec2 pos);
    void       RemoveCharacter(Math::ivec2 pos);
    void       MoveCharacter(Math::ivec2 old_pos, Math::ivec2 new_pos);
    Character* GetCharacterAt(Math::ivec2 pos) const;

    void Draw() const;

    void Update(double dt) override;
};