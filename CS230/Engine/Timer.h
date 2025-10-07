/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Timer.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 17, 2025
*/
#pragma once

#include "Component.h"
namespace CS230 {
	class Timer : public Component {
	public:
		Timer(double timer_max);
		void Set(double time_remaining);
		void Update(double dt)override;
		void Reset();
		bool TickTock();
		const double Remaining() const { return timer; }
		const int RemainingInt() const { return static_cast<int>(timer); }
		
	private:
		double timer;
		double timer_max;
		bool pendulum;
	};
}