#include "Food.h"
#include "../../../Engine/GameStateManager.hpp"
#include "../../../Engine/Particle.h"
#include "../../Particles.h"
#include "Passenger.h"

Food::Food(Passenger* _owner, Math::vec2 pos) : GameObject(pos + Math::vec2{ PassengerWidthHeight * 2 / 3, PassengerWidthHeight * 2 / 3 }), owner(_owner)
{
    AddGOComponent(new Bounce(this, GetPosition(), true));
    int result = Engine::GetGameStateManager().GetGSComponent<Random>()->PickRandomIndex(5, true); // index 0: Bread, 1: Hotdog, 2: KimBab, 3: Snack, 4: Strawberry

    switch (result)
    {
        case 0: AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Bread.spt", this)); break;
        case 1: AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Hotdog.spt", this)); break;
        case 2: AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/KimBab.spt", this)); break;
        case 3: AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Snack.spt", this)); break;
        case 4: AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Strawberry.spt", this)); break;
    }
    AddGOComponent(new CS230::ShowCollision());
}

void Food::Update(double dt)
{
    GameObject::Update(dt);
    if (owner->GetHasFood() == false)
    {
        const auto shining_particle = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Shining>>();
        if (!shining_particle)
        {
            throw std::logic_error("no shining particle");
        }
        shining_particle->Emit(1, GetPosition(), { 0, 0 }, { -10, -10 }, 3.1415926535 / 3);
        shining_particle->Emit(1, Math::vec2{ PlayerWidthHeight / 2, 0 } + GetPosition(), { 0, 0 }, { 10, 10 }, 3.1415926535 / 3);
        shining_particle->Emit(1, Math::vec2{ 0, PlayerWidthHeight / 2 } + GetPosition(), { 0, 0 }, { 10, -10 }, 3.1415926535 / 3);
        Destroy();
    }
}