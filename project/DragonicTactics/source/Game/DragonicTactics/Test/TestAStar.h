/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
bool TestPathfindingStraightLine();
bool TestPathfindingAroundObstacle();
bool TestPathfindingNoPath();
bool TestPathfindingAlreadyAtGoal();

// bool TestGetPathLengthDirect();
// bool TestGetPathLengthNoPath();
//
// bool TestGetReachableTilesCenterGrid();
// bool TestGetReachableTilesCornerGrid();
// bool TestGetReachableTilesWithObstacles();

bool TestPathfindingInvalidStart();
bool TestPathfindingInvalidGoal();
bool TestPathfindingUnwalkableGoal();

extern bool TestAStar;
