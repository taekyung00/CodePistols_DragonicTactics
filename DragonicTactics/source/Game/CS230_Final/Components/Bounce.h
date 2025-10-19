#pragma once
#include "../../../Engine/Component.h"
#include "../../../Engine/GameObject.h"

class Bounce : public CS230::Component {
public:
	Bounce(CS230::GameObject*, Math::vec2,  bool, double timer = 0.0);
	void Update(double dt) override;
	static constexpr double bounce_velocity = 50;
private:
	CS230::GameObject* given_object;
	Math::vec2 start_position;
	bool looping;
	double timer;
};