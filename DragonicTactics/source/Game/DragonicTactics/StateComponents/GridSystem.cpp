#include "GridSystem.h"
#include <cassert>
#include "./Game/DragonicTactics/Objects/Character.h"  
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"
GridSystem::GridSystem() {
    // Initialize all tiles to Empty
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
        return true; // 맵 밖은 항상 점유된 것으로 간주하여 이동 불가 처리
    }
    return character_grid[pos.y][pos.x] != nullptr;
}


void GridSystem::Draw() const {

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {

            int screen_x = x * TILE_SIZE;
            int screen_y = y * TILE_SIZE;

            switch (tile_grid[y][x]) {
            case TileType::Wall:
                //DrawRectangle(screen_x, screen_y, TILE_SIZE, TILE_SIZE, BROWN);
                break;
            case TileType::Empty:
                break;
            default:
                break;
            }
            Character* character = character_grid[y][x];
            if (character != nullptr) {
                CS200::RGBA char_color = CS200::WHITE; // 기본 색상
                switch (character->GetCharacterType()) {
                case CharacterTypes::Fighter:
                    char_color = 0x0000FFFF; // 파이터(f)는 파란색
                    break;
                case CharacterTypes::Dragon:
                    char_color = 0xFF0000FF; // 드래곤(d)은 빨간색
                    break;
                default:
                    break;
                }
                //DrawRectangle(screen_x, screen_y, TILE_SIZE, TILE_SIZE, char_color);
            }

            //DrawRectangleLines(screen_x, screen_y, TILE_SIZE, TILE_SIZE, CS200::BLACK);
        }
    }
}

bool GridSystem::IsWalkable(Math::ivec2 pos) const {
    // 타일이 비어있고(Empty), 캐릭터도 없어야(Not Occupied) 걸을 수 있음
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
    // 기존 위치의 캐릭터를 새로운 위치로 옮기고, 기존 위치는 비웁니다.
    character_grid[new_pos.y][new_pos.x] = character_grid[old_pos.y][old_pos.x];
    character_grid[old_pos.y][old_pos.x] = nullptr;
}

void GridSystem::Update([[maybe_unused]]double dt) {
    // GridSystem이 매 프레임마다 처리할 로직이 있다면 여기에 작성
}
