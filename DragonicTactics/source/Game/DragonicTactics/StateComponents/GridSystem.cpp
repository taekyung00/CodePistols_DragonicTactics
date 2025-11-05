#include "GridSystem.h"
#include "./CS200/IRenderer2D.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"
#include "./Game/DragonicTactics/Objects/Character.h"
#include <cassert>
#include <algorithm>

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
			tile_grid[y][x]		 = TileType::Empty;
			character_grid[y][x] = nullptr;
		}
	}
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

int GridSystem::ManhattanDistance(Math::vec2 a, Math::vec2 b) const
{
	return static_cast<int>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

std::vector<Math::vec2> GridSystem::GetNeighbors(Math::vec2 position) const
{
	std::vector<Math::vec2> neighbors;

	// 4-directional movement(up,down,left,right)
	const Math::vec2 directionals[] = {
		{  0,	 1 }, //  up
		{  0,   -1 }, //  down
		{ -1,    0 }, //  left
		{  1,	 0 }  //  right
	};

	for (const Math::vec2& dir : directionals)
	{
		Math::vec2 neighbor = position + dir;
		if (IsValidTile(neighbor))
		{
			neighbors.push_back(neighbor);
		}
	}
	return neighbors;
}

void GridSystem::Draw() const
{
	auto& renderer_2d = Engine::GetRenderer2D();


	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			int screen_x = x * TILE_SIZE + TILE_SIZE;
			int screen_y = y * TILE_SIZE + TILE_SIZE;

			switch (tile_grid[y][x])
			{
				case TileType::Wall:
					renderer_2d.DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x, screen_y }) * Math::ScaleMatrix(TILE_SIZE), CS200::BROWN, 0U);
					// renderer_2d.DrawRectangle(, TILE_SIZE, TILE_SIZE, BROWN);
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
				renderer_2d.DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x, screen_y }) * Math::ScaleMatrix(TILE_SIZE), char_color);
			}
			renderer_2d.DrawRectangle(Math::TranslationMatrix(Math::ivec2{ screen_x, screen_y }) * Math::ScaleMatrix(TILE_SIZE), 0U, CS200::WHITE);
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

std::vector<Math::vec2> GridSystem::FindPath(Math::vec2 start, Math::vec2 goal)
{
	// edge cases
	if (!IsValidTile(start) || !IsValidTile(goal))
	{
		Engine::GetLogger().LogError("GridSystem : Invalid start or goal position");
		return {};
	}

	if (!IsWalkable(goal))
	{
		Engine::GetLogger().LogError("GridSystem : Goal is not walkable");
		return {};
	}

	if (IsOccupied(goal))
	{
		Engine::GetLogger().LogError("GridSystem : Goal is occupied");
		return {};
	}

	if (start == goal)
	{
		return { start };
	}

	// A* algorithm
	std::vector<Node*>			 openSet;
	std::vector<Node*>			 closedSet;
	std::map<std::pair<int, int>, Node*> allNodes; // to find nodes with positions easily!!

	// create start node
	Node* startNode = new Node(start, 0, ManhattanDistance(start, goal));
	openSet.push_back(startNode);
	allNodes[{ (int)start.x, (int)start.y }] = startNode;

	Node* goalNode = nullptr;

	while (!openSet.empty())
	{
		// find node with lowest fCost
		auto minIt = std::min_element(std::begin(openSet), std::end(openSet), [](Node* a, Node* b) { return static_cast<int>(a->fCost()) < static_cast<int>(b->fCost()); });

		Node* current = *minIt;
		openSet.erase(minIt);
		closedSet.push_back(current);

		//check if we reached the goal
		if (current->position == goal)
		{
			goalNode = current;
			break;
		}

		//check neighbors
		std::vector<Math::vec2> neighbors = GetNeighbors(current->position);
		for (const Math::vec2& neighborPos : neighbors)
		{
			//skip if not walkable or in closed set
			if (!IsWalkable(neighborPos))
				continue;
			
			bool inClosedSet = false;
			for (Node* closed : closedSet)
			{
				if (closed->position == neighborPos)
				{
					inClosedSet = true;
					break;
				}
			}

			if (inClosedSet)
				continue;

			int newGCost = current->gCost + 1;

			//check if neighbor is in open set
			Node* neighborNode = nullptr;
			auto  nodeKey	   = std::make_pair((int)neighborPos.x, (int)neighborPos.y);
			if (allNodes.find(nodeKey) == allNodes.end()) // not in open and close set
			{
				//create new node
				neighborNode = new Node(
					neighborPos,
					newGCost, ManhattanDistance(neighborPos, goal),
					current
				);
				openSet.push_back(neighborNode);
				allNodes[nodeKey] = neighborNode;
			}
		}
	}

	//reconstruct path
	std::vector<Math::vec2> path;
	


	return std::vector<Math::vec2>();
}

int GridSystem::GetPathLength([[maybe_unused]] Math::vec2 start, [[maybe_unused]] Math::vec2 goal)
{
	return 0;
}

std::vector<Math::vec2> GridSystem::GetReachableTiles([[maybe_unused]] Math::vec2 start, [[maybe_unused]] int maxDistance)
{
	return std::vector<Math::vec2>();
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
