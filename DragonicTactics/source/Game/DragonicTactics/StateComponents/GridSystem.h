#pragma once
#include "./Engine/Vec2.hpp"
#include "./Game/DragonicTactics/States/Test.h"
#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include <map>
//#include "Character.h"  

class GridSystem {
public:
    enum class TileType {
        Empty,
        Wall,
        Lava,
        Difficult,  // costs 2 movement
        Invalid
    };

private:
    static const int GRID_WIDTH = 8;
    static const int GRID_HEIGHT = 8;
    static const int TILE_SIZE = 64;  // pixels

    

    TileType tiles[GRID_WIDTH][GRID_HEIGHT];
    std::map<Math::vec2, MockCharacter*> occupiedTiles;

public:
    GridSystem();
    bool IsValidTile(Math::vec2 tile) const;
    bool IsWalkable(Math::vec2 tile) const;
    bool IsOccupied(Math::vec2 tile) const;
    TileType GetTileType(Math::vec2 tile) const;
    void SetTileType(Math::vec2 tile, TileType type);
    void PlaceCharacter(MockCharacter* character, Math::vec2 pos);
    void RemoveCharacter(Math::vec2 pos);
    MockCharacter* GetCharacterAt(Math::vec2 pos) const;

};

