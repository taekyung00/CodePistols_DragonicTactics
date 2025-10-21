#include "GridSystem.h"
#include <cassert>
#include "./Game/DragonicTactics/Objects/Character.h"  
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"
#include "./CS200/IRenderer2D.hpp"
GridSystem::GridSystem() {
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
        }
    }
}

bool GridSystem::IsWalkable(Math::ivec2 pos) const {
    return GetTileType(pos) == TileType::Empty && !IsOccupied(pos);
}

void GridSystem::AddCharacter(Character* character, Math::ivec2 pos) {
    if (!IsValidTile(pos)) {
        Engine::GetLogger().LogError("AddCharacter: Invalid tile position.");
        return;
    }
    if (IsOccupied(pos)) {
        Engine::GetLogger().LogError("AddCharacter: Tile is already occupied.");
        return;
    }
    character_grid[pos.y][pos.x] = character;
}

void GridSystem::RemoveCharacter(Math::ivec2 pos) {
    if (!IsValidTile(pos)) return;
    character_grid[pos.y][pos.x] = nullptr;
}

Character* GridSystem::GetCharacterAt(Math::ivec2 pos) const {
    if (!IsValidTile(pos)) {
        return nullptr;
    }
    return character_grid[pos.y][pos.x];
}

void GridSystem::MoveCharacter(Math::ivec2 old_pos, Math::ivec2 new_pos) {
    if (!IsValidTile(old_pos) || !IsValidTile(new_pos)) {
        Engine::GetLogger().LogError("MoveCharacter: Invalid tile position.");
        return;
    }
    character_grid[new_pos.y][new_pos.x] = character_grid[old_pos.y][old_pos.x];
    character_grid[old_pos.y][old_pos.x] = nullptr;
}

void GridSystem::Update([[maybe_unused]]double dt) {
}
