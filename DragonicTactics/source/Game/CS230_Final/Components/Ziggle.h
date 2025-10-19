#pragma once
#include "../../../Engine/Component.h"
#include "../../../Engine/GameObject.h"

class Ziggle : public CS230::Component {
public:
	Ziggle(CS230::GameObject* , bool , double _timer = 0.0);
	void Update(double dt) override;
	void Set(double new_timer) ;
	static constexpr double ziggle_velocity = 5;
private:
	CS230::GameObject* given_object;
	bool looping;
	// bool finished = false;
	double timer;
	bool toggle = false;
};