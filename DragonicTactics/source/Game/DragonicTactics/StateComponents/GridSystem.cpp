#include "GridSystem.h"
#include <cassert>

GridSystem::GridSystem() {
<<<<<<< Updated upstream
    // Initialize all tiles to Empty
    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            tiles[x][y] = TileType::Empty;
=======
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            tile_grid[y][x] = TileType::Empty;
            character_grid[y][x] = nullptr;
        }
    }
}
bool GridSystem::IsValidTile(Math::ivec2 pos) const {
    return pos.x >= 0 && pos.x < MAP_WIDTH && pos.y >= 0 && pos.y < MAP_HEIGHT;
}
void GridSystem::SetTileType(Math::ivec2 pos, TileType type) {
    if (!IsValidTile(pos)) {
        Engine::GetLogger().LogError("SetTileType: Invalid tile position.");
        return;
    }
    tile_grid[pos.y][pos.x] = type;
}
GridSystem::TileType GridSystem::GetTileType(Math::ivec2 pos) const {
    if (!IsValidTile(pos)) {
        return TileType::Invalid;
    }
    return tile_grid[pos.y][pos.x];
}
bool GridSystem::IsOccupied(Math::ivec2 pos) const {
    if (!IsValidTile(pos)) {
        return true;
    }
    return character_grid[pos.y][pos.x] != nullptr;
}
void GridSystem::Draw() const {
    auto& renderer_2d = Engine::GetRenderer2D();
    

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {

            int screen_x = x * TILE_SIZE + TILE_SIZE;
            int screen_y = y * TILE_SIZE + TILE_SIZE;

            switch (tile_grid[y][x]) {
            case TileType::Wall:
                    renderer_2d.DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x, screen_y }) * Math::ScaleMatrix(TILE_SIZE), CS200::BROWN, 0U);
                 //renderer_2d.DrawRectangle(, TILE_SIZE, TILE_SIZE, BROWN);
                break;
            case TileType::Empty:
                break;
            default:
                break;
            }
            Character* character = character_grid[y][x];
            if (character != nullptr) {
                CS200::RGBA char_color = CS200::WHITE;
                switch (character->GetCharacterType()) {
                case CharacterTypes::Fighter:
                    char_color = 0x0000FFFF;
                    break;
                case CharacterTypes::Dragon:
                    char_color = 0xFF0000FF;
                    break;
                default:
                    break;
                }
                renderer_2d.DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x, screen_y }) * Math::ScaleMatrix(TILE_SIZE),  char_color);
            }
            renderer_2d.DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x, screen_y }) * Math::ScaleMatrix(TILE_SIZE), 0U, CS200::WHITE);
>>>>>>> Stashed changes
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