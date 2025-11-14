#include "TestAStar.h"

#include "./Engine/Engine.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/Vec2.hpp"
#include "./Engine/GameStateManager.hpp"

#include "./Game/DragonicTactics/StateComponents/GridSystem.h"
#include "./Game/DragonicTactics/Test/TestAssert.h"

bool TestPathfindingStraightLine()
{
	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!gridsys)
	{
		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");	
		return false;
	}

	//setup
	gridsys->Reset();
	Math::ivec2 start{ 0, 0 };
	Math::ivec2 goal{ 0, 3 };

	//action
	std::vector<Math::ivec2> path = gridsys->FindPath(start, goal);

	// Assertions
	ASSERT_EQ(static_cast<int>(path.size()), 3); // , {0,1}, {0,2}, {0,3}
	ASSERT_EQ(path[0], { 0, 1 });
	ASSERT_EQ(path[2], { 0, 3 });

	//verify path is continuous
	for (size_t i = 1; i < path.size(); ++i)
	{
		int distance = gridsys->ManhattanDistance(path[i - 1], path[i]);
		ASSERT_EQ(distance, 1); // Each step should be exactly 1 tile apart
	}

	std::cout << " Test_Pathfinding_StraightLine passed " << std::endl;
	return true;
}

bool TestPathfindingAroundObstacle()
{
	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!gridsys)
	{
		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
		return false;
	}
	//setup
	gridsys->Reset();

	//create wall obstacle at (1,0), (1,1), (1,2)
	gridsys->SetTileType({ 1, 0 }, GridSystem::TileType::Wall);
	gridsys->SetTileType({ 1, 1 }, GridSystem::TileType::Wall);
	gridsys->SetTileType({ 1, 2 }, GridSystem::TileType::Wall);

	Math::ivec2 start{ 0, 1 };
	Math::ivec2 goal{ 2, 1 };

	std::vector<Math::ivec2> path = gridsys->FindPath(start, goal);
	ASSERT_TRUE(path.size() > 2);
	ASSERT_EQ(path[0], { 0, 2 });
	ASSERT_EQ(path[path.size() - 1], { 2, 1 });

	//verify path doesn't go through walls
	for (const Math::ivec2& tile : path)
	{
		ASSERT_TRUE(gridsys->IsWalkable(tile));
	}
	std::cout << " Test_Pathfinding_AroundObstacle passed" << std::endl;
	return true;
}

bool TestPathfindingNoPath()
{
	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!gridsys)
	{
		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
		return false;
	}
	// setup
	gridsys->Reset();

	//create impossible wall blocking thi goal
	for (int y = 0; y < 8; ++y)
	{
		gridsys->SetTileType({ 3, y }, GridSystem::TileType::Wall);
	}

	Math::ivec2 start{ 0, 0 };
	Math::ivec2 goal{ 5, 5 }; // On the other side of the wall

	// Action
	std::vector<Math::ivec2> path = gridsys->FindPath(start, goal);

	// Assertions - Should return empty path
	ASSERT_EQ(path.size(), static_cast<size_t>(0));

	std::cout << "Test_Pathfinding_NoPath passed" << std::endl;
	return true;
}

bool TestPathfindingAlreadyAtGoal()
{
	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!gridsys)
	{
		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
		return false;
	}
	// setup
	gridsys->Reset();
	Math::ivec2 start{ 4, 4 };
	Math::ivec2 goal{ 4, 4 };

	// Action
	std::vector<Math::ivec2> path = gridsys->FindPath(start, goal);

	// Assertions
	ASSERT_EQ(path.size(), static_cast<size_t>(0));
	//ASSERT_EQ(path[0], {4,4});

	return true;
}

//bool TestGetPathLengthDirect()
//{
//	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
//	if (!gridsys)
//	{
//		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
//		return false;
//	}
//	// setup
//	gridsys->Reset();
//
//	// Action
//	int length = gridsys->GetPathLength({ 0, 0 }, { 0, 3 });
//
//	// Assertions
//	ASSERT_EQ(length, 3); // 3 steps: (0,0)->(0,1)->(0,2)->(0,3)
//
//	std::cout << " Test_GetPathLength_Direct passed" << std::endl;
//	return true;
//}

//bool TestGetPathLengthNoPath()
//{
//	// Setup
//	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
//	if (!gridsys)
//	{
//		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
//		return false;
//	}
//	gridsys->Reset();
//
//	// Block all paths
//	for (int y = 0; y < 8; ++y)
//	{
//		gridsys->SetTileType({ 3, y }, GridSystem::TileType::Wall);
//	}
//
//	// Action
//	int length = gridsys->GetPathLength({ 0, 0 }, { 5, 5 });
//
//	// Assertions
//	ASSERT_EQ(length, -1); // No path found
//
//	std::cout << "Test_GetPathLength_NoPath passed" << std::endl;
//	return true;
//}

//bool TestGetReachableTilesCenterGrid()
//{
//	// Setup
//	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
//	if (!gridsys)
//	{
//		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
//		return false;
//	}
//	gridsys->Reset();
//
//	Math::ivec2 start{ 4, 4 }; // Center of 8x8 grid
//	int		   maxDistance = 2;
//
//	// Action
//	std::vector<Math::ivec2> reachable = gridsys->GetReachableTiles(start, maxDistance);
//
//	// Assertions
//	// With maxDistance=2, should reach 12 tiles (4-directional movement)
//	// Not exactly 12 because edges of grid may limit it, but should be > 8
//	ASSERT_GE(reachable.size(), (size_t)8);
//	ASSERT_LE(reachable.size(), (size_t)12);
//
//	// Verify all tiles are within distance
//	for (const Math::ivec2& tile : reachable)
//	{
//		int pathLength = gridsys->GetPathLength(start, tile);
//		ASSERT_GE(pathLength, 1);
//		ASSERT_LE(pathLength, maxDistance);
//	}
//
//	// Verify start tile NOT in reachable tiles
//	for (const Math::ivec2& tile : reachable)
//	{
//		ASSERT_FALSE(tile == start);
//	}
//
//	std::cout << "Test_GetReachableTiles_CenterGrid passed" << std::endl;
//	return true;
//}
//
//bool TestGetReachableTilesCornerGrid()
//{
//	// Setup
//	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
//	if (!gridsys)
//	{
//		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
//		return false;
//	}
//	gridsys->Reset();
//
//	Math::ivec2 start{ 0, 0 }; // Corner
//	int		   maxDistance = 3;
//
//
//	// Action
//	std::vector<Math::ivec2> reachable = gridsys->GetReachableTiles(start, maxDistance);
//
//	// Assertions - Should have fewer reachable tiles due to corner
//	ASSERT_GE(reachable.size(), (size_t)6); // At least 6 tiles reachable from corner
//
//	std::cout << "Test_GetReachableTiles_CornerGrid passed" << std::endl;
//	return true;
//}
//
//bool TestGetReachableTilesWithObstacles()
//{
//	// Setup
//	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
//	if (!gridsys)
//	{
//		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
//		return false;
//	}
//	gridsys->Reset();
//
//	// Place walls to limit movement
//	gridsys->SetTileType({ 2, 0 }, GridSystem::TileType::Wall);
//	gridsys->SetTileType({ 2, 1 }, GridSystem::TileType::Wall);
//	gridsys->SetTileType({ 2, 2 }, GridSystem::TileType::Wall);
//
//	Math::ivec2 start{ 0, 1 };
//	int		   maxDistance = 3;
//
//	// Action
//	std::vector<Math::ivec2> reachable = gridsys->GetReachableTiles(start, maxDistance);
//
//	// Assertions - Should have fewer tiles due to walls
//	ASSERT_GE(reachable.size(), (size_t)3); // At least some tiles reachable
//
//	// Verify wall tiles are NOT reachable
//	for (const Math::ivec2& tile : reachable)
//	{
//		ASSERT_FALSE(tile.x == 2 && (tile.y == 0 || tile.y == 1 || tile.y == 2));
//	}
//
//	std::cout << "Test_GetReachableTiles_WithObstacles passed" << std::endl;
//	return true;
//
//}

bool TestPathfindingInvalidStart()
{
	// Setup
	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!gridsys)
	{
		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
		return false;
	}
	gridsys->Reset();

	// Action - Invalid start position
	std::vector<Math::ivec2> path = gridsys->FindPath({ -1, -1 }, { 4, 4 });

	// Assertions
	ASSERT_EQ(path.size(), (size_t)0); // Should return empty path

	std::cout << "Test_Pathfinding_InvalidStart passed" << std::endl;
	return true;
}

bool TestPathfindingInvalidGoal()
{
	// Setup
	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!gridsys)
	{
		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
		return false;
	}
	gridsys->Reset();

	// Action - Invalid goal position
	std::vector<Math::ivec2> path = gridsys->FindPath({ 4, 4 }, { 10, 10 });

	// Assertions
	ASSERT_EQ(path.size(), (size_t)0);

	std::cout << "Test_Pathfinding_InvalidGoal passed" << std::endl;
	return true;
}

bool TestPathfindingUnwalkableGoal()
{
	// Setup
	GridSystem* gridsys = Engine::GetGameStateManager().GetGSComponent<GridSystem>();
	if (!gridsys)
	{
		Engine::GetLogger().LogEvent("GridSystem isn't uploaded!");
		return false;
	}
	gridsys->Reset();
	gridsys->SetTileType({ 5, 5 }, GridSystem::TileType::Wall);

	// Action - Goal is a wall
	std::vector<Math::ivec2> path = gridsys->FindPath({ 4, 4 }, { 5, 5 });

	// Assertions
	ASSERT_EQ(path.size(), (size_t)0);

	std::cout << "Test_Pathfinding_UnwalkableGoal passed" << std::endl;
	return true;
}
