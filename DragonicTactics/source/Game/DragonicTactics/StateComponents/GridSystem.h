#pragma once
#include "./Engine/Vec2.hpp"
#include "./Game/DragonicTactics/States/Test.h"
#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include <map>
#include "./Game/DragonicTactics/Objects/Character.h"  
#include "./Engine/Component.h"
class GridSystem : public CS230::Component {
public:
    enum class TileType {
        Empty,
        Wall,
        Lava,
        Difficult,
        Invalid
    };
private:
    static const int MAP_WIDTH = 8;
    static const int MAP_HEIGHT = 8;
    TileType tile_grid[MAP_HEIGHT][MAP_WIDTH];
    Character* character_grid[MAP_HEIGHT][MAP_WIDTH];
public:
    static const int TILE_SIZE = 64;
    GridSystem();
    void SetTileType(Math::ivec2 tile, TileType type);
    TileType GetTileType(Math::ivec2 tile) const;
    bool IsWalkable(Math::ivec2 tile) const;
    void AddCharacter(Character* character, Math::ivec2 pos);
    void RemoveCharacter(Math::ivec2 pos);
    void MoveCharacter(Math::ivec2 old_pos, Math::ivec2 new_pos);
    Character* GetCharacterAt(Math::ivec2 pos) const;
    bool IsOccupied(Math::ivec2 tile) const;
    void Draw() const;
    bool IsValidTile(Math::ivec2 tile) const;
    void Update(double dt) override;
};