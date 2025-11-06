#include "Ziggle.h"

Ziggle::Ziggle(CS230::GameObject* _given_object, bool _looping, double _timer) : 
	given_object(_given_object),
	looping(_looping),
	timer(_timer){}

void Ziggle::Update(double dt) {
	if (looping == false) {
		if (timer >= 0.0 && (timer - dt) >= 0.0) {
			timer -= dt;
		}
		else {
			timer = 0.0;
			
		}
	}
	if (looping == true || (looping == false && timer > 0.0)) {
		if (toggle) {
			toggle = !toggle;
			given_object->SetPosition({ given_object->GetPosition().x - ziggle_velocity,given_object->GetPosition().y });
		}
		else {
			toggle = !toggle;
			given_object->SetPosition({ given_object->GetPosition().x + ziggle_velocity,given_object->GetPosition().y });
		}
	}
}

void Ziggle::Set(double new_timer) {
	timer = new_timer;
}
