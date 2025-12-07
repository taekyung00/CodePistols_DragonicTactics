#include "pch.h"

#include "./CS200/IRenderer2D.h"
#include "./Engine/Engine.h"
#include "./Engine/Logger.h"
#include "./Game/DragonicTactics/Objects/Character.h"
#include "GridSystem.h"
#include <cassert>

void GridSystem::SetExitPosition(Math::ivec2 pos)
{
  exit_position_ = pos;
}

Math::ivec2 GridSystem::GetExitPosition() const
{
  return exit_position_;
}

bool GridSystem::HasExit() const
{
  return exit_position_ != Math::ivec2{ -1, -1 };
}

GridSystem::GridSystem()
{
  Reset();
}

void GridSystem::Reset()
{
  for (int y = 0; y < MAP_HEIGHT; ++y)
  {
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
	  tile_grid[y][x]	   = TileType::Empty;
	  character_grid[y][x] = nullptr;
	}
  }
  exit_position_ = { -1, -1 };
}

bool GridSystem::IsValidTile(Math::ivec2 pos) const
{
  return pos.x >= 0 && pos.x < MAP_WIDTH && pos.y >= 0 && pos.y < MAP_HEIGHT;
}

void GridSystem::SetTileType(Math::ivec2 pos, TileType type)
{
  if (!IsValidTile(pos))
  {
	Engine::GetLogger().LogError("SetTileType: Invalid tile position.");
	return;
  }
  tile_grid[pos.y][pos.x] = type;
}

GridSystem::TileType GridSystem::GetTileType(Math::ivec2 pos) const
{
  if (!IsValidTile(pos))
  {
	return TileType::Invalid;
  }
  return tile_grid[pos.y][pos.x];
}

bool GridSystem::IsOccupied(Math::ivec2 pos) const
{
  if (!IsValidTile(pos))
  {
	return true;
  }
  return character_grid[pos.y][pos.x] != nullptr;
}

void GridSystem::Draw() const
{
  auto renderer_2d = Engine::GetTextureManager().GetRenderer2D();


  for (int y = 0; y < MAP_HEIGHT; ++y)
  {
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
	  int screen_x = x * TILE_SIZE + TILE_SIZE;
	  int screen_y = y * TILE_SIZE + TILE_SIZE;

	  switch (tile_grid[y][x])
	  {
	  case TileType::Wall:
	  renderer_2d->DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE), CS200::BROWN, 0U);
	  // renderer_2d.DrawRectangle(, TILE_SIZE, TILE_SIZE, BROWN);
	  break;
	  case TileType::Exit:
	  renderer_2d->DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE), CS200::GREEN, 0U);
	  break;
	  case TileType::Lava:
	    renderer_2d->DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE), 0xFF8000FF, 0U);
	      break;
        case TileType::Difficult:
          renderer_2d->DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE), 0x4080FFFF, 0U);
          break;
        case TileType::Empty: break;
        default: break;
      }
	  Character* character = character_grid[y][x];
	  if (character != nullptr)
	  {
		CS200::RGBA char_color = CS200::WHITE;
		switch (character->GetCharacterType())
		{
		  case CharacterTypes::Fighter: char_color = 0x0000FFFF; break;
		  case CharacterTypes::Dragon: char_color = 0xFF0000FF; break;
		  default: break;
		}
		renderer_2d->DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE), char_color);
	  }
	  renderer_2d->DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE), 0U, CS200::WHITE);
	}
  }
}

bool GridSystem::IsWalkable(Math::ivec2 pos) const
{
  return GetTileType(pos) == TileType::Empty && !IsOccupied(pos);
}

void GridSystem::AddCharacter(Character* character, Math::ivec2 pos)
{
  if (!IsValidTile(pos))
  {
	Engine::GetLogger().LogError("AddCharacter: Invalid tile position.");
	return;
  }
  if (IsOccupied(pos))
  {
	Engine::GetLogger().LogError("AddCharacter: Tile is already occupied.");
	return;
  }
  character_grid[pos.y][pos.x] = character;
}

void GridSystem::RemoveCharacter(Math::ivec2 pos)
{
  if (!IsValidTile(pos))
	return;
  character_grid[pos.y][pos.x] = nullptr;
}

Character* GridSystem::GetCharacterAt(Math::ivec2 pos) const
{
  if (!IsValidTile(pos))
  {
	return nullptr;
  }
  return character_grid[pos.y][pos.x];
}

void GridSystem::MoveCharacter(Math::ivec2 old_pos, Math::ivec2 new_pos)
{
  if (!IsValidTile(old_pos) || !IsValidTile(new_pos))
  {
	Engine::GetLogger().LogError("MoveCharacter: Invalid tile position.");
	return;
  }
  character_grid[new_pos.y][new_pos.x] = character_grid[old_pos.y][old_pos.x];
  character_grid[old_pos.y][old_pos.x] = nullptr;
}

void GridSystem::Update([[maybe_unused]] double dt)
{
}

/// @brief //////////////////////
std::vector<Character*> GridSystem::GetAllCharacters()
{
  std::vector<Character*> result;
  for (int y = 0; y < MAP_HEIGHT; ++y)
  {
	for (int x = 0; x < MAP_WIDTH; ++x)
	{
	  result.push_back(character_grid[y][x]);
	}
  }
  return result;
}

void GridSystem::LoadMap(const MapData& map_data)
{
  Engine::GetLogger().LogEvent("GridSystem::LoadMap - Loading map: " + map_data.id);

  Reset();

  for (int y = 0; y < map_data.height; ++y)
  {
    if (y >= map_data.tiles.size())
    {
      Engine::GetLogger().LogError("GridSystem::LoadMap - Row " + std::to_string(y) + " out of bounds");
      break;
    }

    const std::string& row = map_data.tiles[y];

    for (int x = 0; x < map_data.width; ++x)
    {
      if (x >= row.length())
      {
        Engine::GetLogger().LogError("GridSystem::LoadMap - Column " + std::to_string(x) + " out of bounds");
        break;
      }

      char tile_char = row[x];

      Math::ivec2 pos{ x, map_data.height - 1 - y };

      auto it = map_data.legend.find(tile_char);
      if (it == map_data.legend.end())
      {
        continue;
      }

      std::string tile_type_str = it->second;

      TileType tile_type = TileType::Empty;
      if (tile_type_str == "wall")
      {
        tile_type = TileType::Wall;
      }
      else if (tile_type_str == "floor")
      {
        tile_type = TileType::Empty;
      }
      else if (tile_type_str == "lava")
      {
        tile_type = TileType::Lava;
      }
      else if (tile_type_str == "water")
      {
        tile_type = TileType::Difficult;
      }

      SetTileType(pos, tile_type);
    }
  }

  Engine::GetLogger().LogEvent("GridSystem::LoadMap - Completed (" + std::to_string(map_data.width * map_data.height) + " tiles)");
}

////////////////////////////////////
