#include "GridSystem.h"
#include <cassert>

GridSystem::GridSystem() {
    // Initialize all tiles to Empty
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            tiles[x][y] = TileType::Empty;
        }
    }
}

bool GridSystem::IsValidTile(Math::vec2 tile) const
{
    int x = static_cast<int>(tile.x);
    int y = static_cast<int>(tile.y);
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
}

bool GridSystem::IsWalkable(Math::vec2 tile) const
{
    if (!IsValidTile(tile)) return false;
    else if (IsOccupied(tile)) return false; 
    TileType type = GetTileType(tile);
    return type == TileType::Empty || type == TileType::Difficult || type == TileType::Lava;
}

bool GridSystem::IsOccupied(Math::vec2 tile) const
{
    return occupiedTiles.find(tile) != occupiedTiles.end();
}

GridSystem::TileType GridSystem::GetTileType(Math::vec2 tile) const
{
    if (!IsValidTile(tile))
    {
        std::cerr << "GetTileType: invalid tile position (" << tile.x << ", " << tile.y << ")\n";
        return TileType::Invalid;  // 무시하거나 예외처리 또는 로그만 남기고 함수 종료
    }
    return tiles[static_cast<int>(tile.x)][static_cast<int>(tile.y)];
}

void GridSystem::SetTileType(Math::vec2 tile, TileType type)
{
    if (!IsValidTile(tile))
    {
        std::cerr << "SetTileType: invalid tile position (" << tile.x << ", " << tile.y << ")\n";
        return;  // 무시하거나 예외처리 또는 로그만 남기고 함수 종료
    }
    tiles[static_cast<int>(tile.x)][static_cast<int>(tile.y)] = type;
}

void GridSystem::PlaceCharacter(MockCharacter* character, Math::vec2 pos)
{
    /*assert(IsValidTile(pos) && "PlaceCharacter: invalid tile");
    assert(!IsOccupied(pos) && "PlaceCharacter: tile already occupied");*/
    if (!IsValidTile(pos))
    {
        std::cerr << "PlaceCharacter: invalid tile" << "\n";
        return;  // 무시하거나 예외처리 또는 로그만 남기고 함수 종료
    }
    if (IsOccupied(pos))
    {
        std::cerr << "PlaceCharacter: tile already occupied" << "\n";
        return;  // 무시하거나 예외처리 또는 로그만 남기고 함수 종료
    }

    occupiedTiles[pos] = character;
}

void GridSystem::RemoveCharacter(Math::vec2 pos)
{
    occupiedTiles.erase(pos);  
}
MockCharacter* GridSystem::GetCharacterAt(Math::vec2 pos) const
{
    auto it = occupiedTiles.find(pos);
    if (it != occupiedTiles.end())
        return it->second;
    return nullptr;
}