/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ActionPoints.h
Project:    GAM200
Author:     Seungju Song
Created:    Oct 12, 2025
*/
#pragma once
#include "../Engine/Component.h"

class ActionPoints : public CS230::Component {
public:
	ActionPoints(int max_points);
	void Refresh();
	bool Consume(int amount = 1); //인자는 사용되는 행동력을 의미
	int GetCurrentPoints() const;
	int GetMaxPoints() const;
	bool HasEnough(int amount) const;
	void Update([[maybe_unused]] double dt) override{}
private:
	int max_points;
	int current_points;
};

//Action Points를 캐릭터 스탯에 넣지않고