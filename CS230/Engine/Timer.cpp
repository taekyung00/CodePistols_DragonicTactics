/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Timer.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 17, 2025
*/
#include "Timer.h"

CS230::Timer::Timer(double timer_max):
	timer(timer_max)
{}

void CS230::Timer::Set(double time_remaining)
{
	pendulum = false;
	timer_max = time_remaining;
	Reset();
}

void CS230::Timer::Reset() {
	timer = timer_max;
}

bool CS230::Timer::TickTock()
{
	return pendulum;
}

void CS230::Timer::Update(double dt)
{
	if (timer >= 0.0 && (timer - dt) >= 0.0) {
		timer -= dt;
	}
	else {
		timer = 0.0;
	}
	
	pendulum = !pendulum;
}
