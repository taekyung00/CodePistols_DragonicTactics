#pragma once
#include "../../../Engine/GameObject.h"
#include "../../Score.h"
#include "../Components/Random.h"
#include "../Components/Ziggle.h"

class Player;
class Obstacle : public CS230::GameObject {
public:
	Obstacle(Player*, Math::vec2 pos);
	int			DrawPriority() const override{ return 12; }
	GameObjectTypes		Type() override { return GameObjectTypes::Obstacle; }
	std::string			TypeName() override { return "Obstacle"; }

	bool				CanCollideWith(GameObjectTypes other_object_type) override;
	void				ResolveCollision(GameObject* other_object) override;
private:
	Player* player;
};