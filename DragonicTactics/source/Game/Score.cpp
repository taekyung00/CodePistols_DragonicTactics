/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Score.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 29, 2025
*/
#include "Score.h"


Score::Score( int _value) 
	:  value(_value){}

void Score::Add(int dv)
{
	value += dv;
}

void Score::Sub(int dv)
{
    value -= dv;
}
