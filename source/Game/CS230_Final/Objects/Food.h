#pragma once

#include "../../../Engine/GameObject.h"
#include "../Components/Bounce.h"
#include "../Components/Random.h"

class Passenger;

class Food : public CS230::GameObject {
public:
	Food(Passenger*, Math::vec2 pos);
	virtual int		DrawPriority() const override { return 11; }
	GameObjectTypes Type() override { return GameObjectTypes::Food; }
	std::string		TypeName() override { return "Food"; }
	void			Update(double dt) override;
private:
	Passenger* owner;
};
