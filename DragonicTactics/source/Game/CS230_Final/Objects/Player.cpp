#include "Player.h"
#include "../../../Engine/Window.hpp"
#include "../../../Engine/Input.hpp"
#include "../../../Engine/TextManager.hpp"

Player::Player() : 
	GameObject({ BusStartPosition+SeatWidthHeight + SeatWidthHeight/2 - PlayerWidthHeight/2,0 })
{
	AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Player.spt", this));
	AddGOComponent(new Score(0));
    AddGOComponent(new Ziggle(this, false,0.0));
    AddGOComponent(new CS230::ShowCollision());
    score = GetGOComponent<Score>()->Value();
    //score_texture = (Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Score", 0xFFFFFFFF));
    //score_amount_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture(std::to_string(score), 0xFFFFFFFF);
    window_height = Engine::GetWindow().GetSize().y;
    SetVelocity({ 0,100 });
}

void Player::Update([[maybe_unused]] double dt) {
    GameObject::Update(dt);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        SetVelocity({ player_velocity.x,GetVelocity().y});
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        SetVelocity({ -player_velocity.x,GetVelocity().y });
    }
    else {
        SetVelocity({ 0,GetVelocity().y });
    }

    //if (Engine::GetInput().KeyDown(CS230::Input::Keys::Up)) {
    //    SetVelocity({ GetVelocity().x,player_velocity.y });
    //}
    //else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Down)) {
    //    SetVelocity({ GetVelocity().x,-player_velocity.y });
    //}
    //else {
    //    SetVelocity({ GetVelocity().x,0 });
    //}
    update_score_text();
}

void Player::Draw(Math::TransformationMatrix camera_matrix) {
    GameObject::Draw(camera_matrix);
    auto& text_manager = Engine::GetTextManager();
    text_manager.DrawText("Score", Math::vec2{ 0, window_height - 68 }, Fonts::Outlined, { 1.0, 1.0 }, 0xFFFFFFFF);
    text_manager.DrawText(std::to_string(score), Math::vec2{ 0, window_height - 68 - 68 }, Fonts::Outlined, { 1.0, 1.0 }, 0xFFFFFFFF);
}

bool Player::CanCollideWith(GameObjectTypes other_object_type) {
    if (other_object_type == GameObjectTypes::Passenger || other_object_type == GameObjectTypes::Obstacle) {
        return true;
    }
    return false;
}

void Player::ResolveCollision(GameObject* other_object) {
    if (other_object->Type() == GameObjectTypes::Passenger) {
        Math::rect player_rect = GetGOComponent<CS230::RectCollision>()->WorldBoundary();
        Math::rect passenger_rect = other_object->GetGOComponent<CS230::RectCollision>()->WorldBoundary();

        if (player_rect.Left() < passenger_rect.Left()) {
            UpdatePosition(Math::vec2{ (passenger_rect.Left() - player_rect.Right()-7), 0.0 });
            SetVelocity({ 0, GetVelocity().y });
        }
        else {
            UpdatePosition(Math::vec2{ (passenger_rect.Right() - player_rect.Left()+7), 0.0 });
            SetVelocity({ 0, GetVelocity().y });
        }
    }
    else if (other_object->Type() == GameObjectTypes::Obstacle) {
        GetGOComponent<Ziggle>()->Set(0.5);
    }
}

void Player::update_score_text() {
    const auto current_score = GetGOComponent<Score>();
    if ((current_score)&& (score != current_score->Value())) {
        score = current_score->Value();
    }
}