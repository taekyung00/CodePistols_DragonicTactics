#include "Obstacle.h"
#include "Player.h"
#include "../../../Engine/GameStateManager.hpp"
Obstacle::Obstacle(Player* _player, Math::vec2 pos) :
	GameObject(pos),
	player(_player)
{
	int result = Engine::GetGameStateManager().GetGSComponent<Random>()->PickRandomIndex(2,true);// index 0: Bomb, 1: Shoes
	switch (result)
	{
	case 0:
		AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Bomb.spt", this));
		break;
	case 1:
		AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Shoes.spt", this));
		break;
	}
    AddGOComponent(new CS230::ShowCollision());
}

bool Obstacle::CanCollideWith(GameObjectTypes other_object_type) {
	if (other_object_type == GameObjectTypes::Player) {
		return true;
	}
	return false;
}

void Obstacle::ResolveCollision(GameObject* other_object) {
	if (other_object->Type() == GameObjectTypes::Player) {
		player->GetGOComponent<Score>()->Sub(10);
		Destroy();
	}
}
