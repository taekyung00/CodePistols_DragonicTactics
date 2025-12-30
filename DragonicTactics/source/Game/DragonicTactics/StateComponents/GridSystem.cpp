/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "./CS200/IRenderer2D.h"
#include "./Engine/Engine.h"
#include "./Engine/Logger.h"
#include "./Game/DragonicTactics/Objects/Character.h"
#include "GridSystem.h"
#include <cassert>
#include <queue>
#include <set>
#include <algorithm>

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
  // ========================================
  // 2. 이동 가능 타일 시각화 (낮은 알파 초록색)
  // ========================================
  if (movement_mode_active_)
  {
	int alpha{ 0 };
	for (const auto& tile : reachable_tiles_)
	{
	  int screen_x = tile.x * TILE_SIZE + TILE_SIZE;
	  int screen_y = tile.y * TILE_SIZE + TILE_SIZE;

	  alpha = static_cast<int>(80 + 40 * std::sin(pulse_timer_ * 3.0));
	  renderer_2d->DrawRectangle(
		  Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE),
		  CS200::pack_color({ 0 / 255.0f, 255 / 255.0f, 0 / 255.0f, alpha / 255.0f }), // 낮은 알파 초록색 (fill_color)
		  0U,																																						   // line_color: 없음
		  0.0,																																						   // line_width
		  0.2f																																						   // depth
	  );
	}
  }

  // ========================================
  // 3. 마우스 호버 경로 시각화 (진한 초록색)
  // ========================================
  if (!hovered_path_.empty())
  {
	for (const auto& tile : hovered_path_)
	{
	  int screen_x = tile.x * TILE_SIZE + TILE_SIZE;
	  int screen_y = tile.y * TILE_SIZE + TILE_SIZE;

	  // 진한 초록색 오버레이
	  renderer_2d->DrawRectangle(
		  Math::TranslationMatrix(Math::ivec2{ screen_x - (TILE_SIZE / 2), screen_y - (TILE_SIZE / 2) }) * Math::ScaleMatrix(TILE_SIZE), 
		  CS200::pack_color({ 0 / 255.0f, 200 / 255.0f, 0 / 255.0f, 150 / 255.0f }), // 진한 초록색 (fill_color)
		  CS200::pack_color({ 0 / 255.0f, 255 / 255.0f, 0 / 255.0f, 255 / 255.0f }), // 밝은 초록색 테두리 (line_color)
		  2.0,							 // line_width
		  0.1f							 // depth (경로가 이동 가능 타일보다 위에 그려지도록)
	  );
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
  if (movement_mode_active_)
  {
	pulse_timer_ += dt;
  }
  else
  {
	pulse_timer_ = 0.0;
  }
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

// ========================================
// BFS 기반 이동 가능 타일 계산
// ========================================
std::vector<Math::ivec2> GridSystem::GetReachableTiles(Math::ivec2 start, int max_distance)
{
  std::vector<Math::ivec2> reachable;

  if (!IsValidTile(start))
  {
	Engine::GetLogger().LogError("GetReachableTiles: Invalid start position");
	return reachable;
  }

  // BFS 탐색
  std::queue<std::pair<Math::ivec2, int>> queue; // {position, distance}
  std::set<Math::ivec2>					  visited;

  queue.push({ start, 0 });
  visited.insert(start);

  while (!queue.empty())
  {
	auto [current_pos, distance] = queue.front();
	queue.pop();

	// 시작 위치는 제외 (현재 위치이므로 이동할 수 없음)
	if (current_pos != start)
	{
	  reachable.push_back(current_pos);
	}

	// 최대 거리 도달 시 더 이상 탐색 안 함
	if (distance >= max_distance)
	{
	  continue;
	} 

	// 인접 타일 탐색
	std::vector<Math::ivec2> neighbors = GetNeighbors(current_pos);
	for (const auto& neighbor : neighbors)
	{
	  // 방문하지 않았고, 걸을 수 있는 타일만 추가
	  if (visited.find(neighbor) == visited.end() && GetTileType(neighbor) == TileType::Empty && !IsOccupied(neighbor))
	  {
		visited.insert(neighbor);
		queue.push({ neighbor, distance + 1 });
	  }
	}
  }

  Engine::GetLogger().LogEvent("GetReachableTiles: Found " + std::to_string(reachable.size()) + " reachable tiles");
  return reachable;
}

// ========================================
// 이동 모드 활성화
// ========================================
void GridSystem::EnableMovementMode(Math::ivec2 character_pos, int movement_range)
{
  movement_mode_active_ = true;
  movement_source_pos_	= character_pos;

  // 이동 가능한 타일 계산
  std::vector<Math::ivec2> reachable = GetReachableTiles(character_pos, movement_range);
  reachable_tiles_.clear();
  for (const auto& tile : reachable)
  {
	reachable_tiles_.insert(tile);
  }

  Engine::GetLogger().LogEvent("GridSystem: Movement mode enabled at (" + std::to_string(character_pos.x) + ", " + std::to_string(character_pos.y) + ") with range " + std::to_string(movement_range));
}

// ========================================
// 이동 모드 비활성화
// ========================================
void GridSystem::DisableMovementMode()
{
  movement_mode_active_ = false;
  movement_source_pos_	= { -1, -1 };
  reachable_tiles_.clear();
  hovered_path_.clear();
  hovered_tile_ = { -1, -1 };

  Engine::GetLogger().LogEvent("GridSystem: Movement mode disabled");
}

// ========================================
// 마우스 호버 타일 설정 (경로 계산)
// ========================================
void GridSystem::SetHoveredTile(Math::ivec2 hovered_tile)
{
  // 이미 호버 중인 타일이면 무시
  if (hovered_tile_ == hovered_tile)
  {
	return;
  }

  hovered_tile_ = hovered_tile;

  // 이동 가능한 타일이 아니면 경로 초기화
  if (!IsReachable(hovered_tile))
  {
	hovered_path_.clear();
	return;
  }

  // A* 경로 찾기
  hovered_path_ = FindPath(movement_source_pos_, hovered_tile);

  if (!hovered_path_.empty())
  {
	Engine::GetLogger().LogDebug(
		"GridSystem: Path to (" + std::to_string(hovered_tile.x) + ", " + std::to_string(hovered_tile.y) + ") calculated (" + std::to_string(hovered_path_.size()) + " tiles)");
  }
}

// ========================================
// 마우스 호버 해제
// ========================================
void GridSystem::ClearHoveredTile()
{
  hovered_tile_ = { -1, -1 };
  hovered_path_.clear();
}

// ========================================
// 특정 타일이 이동 가능한지 확인
// ========================================
bool GridSystem::IsReachable(Math::ivec2 tile) const
{
  return reachable_tiles_.find(tile) != reachable_tiles_.end();
}

void GridSystem::LoadMap(const MapData& map_data)
{
  Engine::GetLogger().LogEvent("GridSystem::LoadMap - Loading map: " + map_data.id);

  Reset();

  for (int y = 0; y < map_data.height; ++y)
  {
    if (y >= static_cast<int>(map_data.tiles.size()))
    {
      Engine::GetLogger().LogError("GridSystem::LoadMap - Row " + std::to_string(y) + " out of bounds");
      break;
    }

    const std::string& row = map_data.tiles[static_cast<std::size_t>(y)];

    for (int x = 0; x < map_data.width; ++x)
    {
      if (x >= static_cast<int>(row.length()))
      {
        Engine::GetLogger().LogError("GridSystem::LoadMap - Column " + std::to_string(x) + " out of bounds");
        break;
      }

      char tile_char = row[static_cast<std::size_t>(x)];

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
