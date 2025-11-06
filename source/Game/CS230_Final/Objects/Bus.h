#pragma once
#include "../../../Engine/GameObjectManager.h"
#include "../../../Engine/Collision.h"

#include "Constants.h"
#include "../../States.h"
#include "BusLine.h"

class Player;
class Bus : public CS230::GameObject {
public:
	Bus(Player* player_ptr);
	void			Update(double dt) override;
	GameObjectTypes Type() override { return GameObjectTypes::Bus; }
	std::string		TypeName() override { return "Bus"; }
	int DrawPriority() const override { return 9; }
	
	//bool			CanCollideWith(GameObjectTypes other_object_type) override;

	bool IsEnd() const
    {
        return is_end;
    }
    enum class SandwichLevel
    {
		GoodSandwich,
		NormalSandwich,
		BadSandwich
    }sandwich_level = SandwichLevel::NormalSandwich;

private:
	Player* player;
    bool    is_end = false;

	const std::vector<double> start_line = {
		0,144,288,432,576
	};

	const std::vector<double> end_line = {
		96,240,384,528,672
	};
	
};