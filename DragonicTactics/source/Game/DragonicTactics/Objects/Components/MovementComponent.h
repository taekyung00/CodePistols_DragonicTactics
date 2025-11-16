/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MovementComponent.h
Project:    GAM200
Author:     Seungju Song
Created:    Nov 16, 2025
*/

#pragma once

#include "Engine/Component.h"
#include "Engine/Matrix.hpp"
#include <vector>

class GridSystem;
class GridPosition;
class StatsComponent;

namespace CS230
{
    class GameObject;
}

class MovementComponent : public CS230::Component {
public:
    MovementComponent(CS230::GameObject* object);
    
    void Update(double dt) override;
    

    void SetPath(std::vector<Math::ivec2> path);

    void SetGridSystem(GridSystem* grid);

    bool IsMoving() const;

    void ClearPath();

private:
    std::vector<Math::ivec2>    m_current_path;
    double                      m_moveTimer = 0.0;
    static constexpr double     MOVE_TIME_PER_TILE = 0.2; 
    CS230::GameObject* m_owner = nullptr;       
    GridPosition* m_gridPosition = nullptr; 
    StatsComponent* m_stats = nullptr;      

    GridSystem* m_gridSystem = nullptr;
};