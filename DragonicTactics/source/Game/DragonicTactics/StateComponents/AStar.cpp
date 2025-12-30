/**
 * \file
 * \author Taekyung Ho
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

int GridSystem::ManhattanDistance(Math::ivec2 a, Math::ivec2 b) const
{
  return static_cast<int>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

std::vector<Math::ivec2> GridSystem::GetNeighbors(Math::ivec2 position) const
{
  std::vector<Math::ivec2> neighbors;

  // 4-directional movement(up,down,left,right)
  const Math::ivec2 directionals[] = {
	{  0,	 1 }, //  up
	{  0, -1 }, //  down
	{ -1,  0 }, //  left
	{  1,	 0 }  //  right
  };

  for (const Math::ivec2& dir : directionals)
  {
	Math::ivec2 neighbor = position + dir;
	if (IsValidTile(neighbor))
	{
	  neighbors.push_back(neighbor);
	}
  }
  return neighbors;
}

std::vector<Math::ivec2> GridSystem::FindPath(Math::ivec2 start, Math::ivec2 goal)
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
	return {};
  }

  // A* algorithm
  std::vector<Node*>				   openSet;
  std::vector<Node*>				   closedSet;
  std::map<std::pair<int, int>, Node*> allNodes; // to find nodes with positions easily!!

  // create start node
  Node* startNode = new Node(start, 0, ManhattanDistance(start, goal));
  openSet.push_back(startNode);
  allNodes[{ static_cast<int>(start.x), static_cast<int>(start.y) }] = startNode;

  Node* goalNode = nullptr;

  while (!openSet.empty())
  {
	// find node with lowest fCost
	auto minIt = std::min_element(std::begin(openSet), std::end(openSet), [](Node* a, Node* b) { return static_cast<int>(a->fCost()) < static_cast<int>(b->fCost()); });

	Node* current = *minIt;
	openSet.erase(minIt);
	closedSet.push_back(current);

	// check if we reached the goal
	if (current->position == goal)
	{
	  goalNode = current;
	  break;
	}

	// check neighbors
	std::vector<Math::ivec2> neighbors = GetNeighbors(current->position);
	for (const Math::ivec2& neighborPos : neighbors)
	{
	  // skip if not walkable or in closed set
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

	  // check if neighbor is in open set
	  Node* neighborNode = nullptr;
	  auto	nodeKey		 = std::make_pair(static_cast<int>(neighborPos.x), static_cast<int>(neighborPos.y));
	  if (allNodes.find(nodeKey) == allNodes.end()) // not in open and close set
	  {
		// create new node
		neighborNode = new Node(neighborPos, newGCost, ManhattanDistance(neighborPos, goal), current);
		openSet.push_back(neighborNode);
		allNodes[nodeKey] = neighborNode;
	  }
	}
  }

  // reconstruct path
  std::vector<Math::ivec2> path;

  if (goalNode != nullptr)
  {
	Node* current = goalNode;
	while (current != nullptr)
	{
	  if (current->position != start)
	  {
		path.push_back(current->position);
	  }
	  current = current->parent;
	}
	std::reverse(path.begin(), path.end());
  }

  // cleanup
  for (auto& pair : allNodes)
  {
	delete pair.second;
  }

  if (path.empty())
  {
	Engine::GetLogger().LogError(
	  "GridSystem: No path found from (" + std::to_string(static_cast<int>(start.x)) + "," + std::to_string(static_cast<int>(start.y)) + ") to (" + std::to_string(static_cast<int>(goal.x)) + "," + std::to_string(static_cast<int>(goal.y)) + ")");
  }


  return path;
}

// int GridSystem::GetPathLength(Math::ivec2 start, Math::ivec2 goal)
//{
//	std::vector<Math::ivec2> path = FindPath(start, goal);
//	return path.empty() ? -1 : (int)path.size() - 1; // -1 because start tile doesn't count
// }
//
// std::vector<Math::ivec2> GridSystem::GetReachableTiles(Math::ivec2 start, int maxDistance)
//{
//	std::vector<Math::ivec2> reachable;
//
//	// Check all tiles on the grid
//	for (int y = 0; y < MAP_HEIGHT; ++y)
//	{
//		for (int x = 0; x < MAP_WIDTH; ++x)
//		{
//			Math::ivec2 tile{ x, y };
//
//			// Skip start tile
//			if (tile.x == start.x && tile.y == start.y)
//				continue;
//
//			// Skip if not walkable
//			if (!IsWalkable(tile))
//				continue;
//
//			// Skip if occupied
//			if (IsOccupied(tile))
//				continue;
//
//			// Check path length
//			int pathLength = GetPathLength(start, tile);
//			if (pathLength > 0 && pathLength <= maxDistance)
//			{
//				reachable.push_back(tile);
//			}
//		}
//	}
//
//	return reachable;
// }
