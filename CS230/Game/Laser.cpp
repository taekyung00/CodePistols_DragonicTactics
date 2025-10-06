#include "Laser.h"

Laser::Laser(Math::vec2 pos, double rot, Math::vec2 scale, Math::vec2 vel) :
	GameObject(pos,rot,scale)
{
	SetVelocity(vel);
	AddGOComponent(new CS230::Sprite("Assets/Laser.spt",this));
}

void Laser::Update(double dt) {
	GameObject::Update(dt);
    Math::ivec2 frame_size = GetGOComponent<CS230::Sprite>()->GetFrameSize();
    if (GetPosition().y > Engine::GetWindow().GetSize().y + frame_size.y / 2.0) {
        Destroy();
    }
    else if (GetPosition().y < 0 - frame_size.y / 2.0) {
        Destroy();
    }
    if (GetPosition().x > Engine::GetWindow().GetSize().x + GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2.0) {
        Destroy();
    }
    else if (GetPosition().x < 0 - frame_size.x / 2.0) {
        Destroy();
    }
	
}

bool Laser::CanCollideWith(GameObjectTypes other_object_type) {
    if (other_object_type == GameObjectTypes::Meteor) {
        return true;
    }
    return false;
}

void Laser::ResolveCollision([[maybe_unused]]GameObject* other_object) {
    Destroy();
    other_object->ResolveCollision(this);
}
