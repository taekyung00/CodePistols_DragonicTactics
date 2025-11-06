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
	Math::vec2 start{ 0, 0 };
	Math::vec2 goal{ 0, 3 };

	//action
	std::vector<Math::vec2> path = gridsys->FindPath(start, goal);

	// Assertions
	ASSERT_EQ(static_cast<int>(path.size()), 4); // {0,0}, {0,1}, {0,2}, {0,3}
	ASSERT_EQ(path[0], { 0, 0 });
	ASSERT_EQ(path[3], { 0, 3 });
	//ASSERT_EQ(static_cast<int>(path[0].x), 0);
	//ASSERT_EQ(static_cast<int>(path[0].y), 0);
	//ASSERT_EQ(static_cast<int>(path[3].x), 0);
	//ASSERT_EQ(static_cast<int>(path[3].y), 3);

	//verify path is continuous
	for (size_t i = 1; i < path.size(); ++i)
	{
		int distance = gridsys->ManhattanDistance(path[i - 1], path[i]);
		ASSERT_EQ(distance, 1); // Each step should be exactly 1 tile apart
	}

	std::cout << " Test_Pathfinding_StraightLine passed " << std::endl;
	return true;
}
