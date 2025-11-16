/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Score.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 29, 2025
*/
#pragma once

#include "../Engine/GameObject.h"
#include "../Engine/Vec2.hpp"
class Score : public CS230::Component {
public:
	Score(int value);
	void Add(int dv = 1);
    void Sub(int dv = 1);
	const int& Value() const { return value; }
private:
	int value;
};
