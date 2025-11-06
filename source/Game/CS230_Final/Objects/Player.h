#pragma once

#include "../../../Engine/GameObject.h"

#include "Bus.h"
#include "Constants.h"
#include "Passenger.h"
#include "../../Score.h"
#include "../Components/Ziggle.h"


class Player : public CS230::GameObject {
public:
	Player();
	void				Update(double dt) override;
	void				Draw(Math::TransformationMatrix camera_matrix) override;
	GameObjectTypes		Type() override { return GameObjectTypes::Player; }
	std::string			TypeName() override { return "Player"; }

	bool				CanCollideWith(GameObjectTypes other_object_type) override;
	void				ResolveCollision(GameObject* other_object) override;

	int					DrawPriority() const override { return 11; }

	const Math::vec2&	GetPosition() const { return GameObject::GetPosition(); }

	std::vector<bool>&			SetCanSteel() { return can_steel; }
	const std::vector<bool>&	GetCanSteel() const{ return can_steel; }

	//int							GetScore() { return score; }
	
private:
	const Math::vec2	player_velocity = { 96.0,96.0 };
	
	int					score = 0;
	// CS230::Texture*		score_texture = nullptr;
	// CS230::Texture*		score_amount_texture = nullptr;
	void				update_score_text();

	std::vector<bool>	can_steel = { false,false,false,false,false };

	double window_height;
};