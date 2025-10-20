#include "Passenger.h"
#include "../../../Engine/ShowCollision.h"
#include "../../../Engine/Timer.hpp"
#include "../../Score.h"

Passenger::Passenger(BusLine _busline, Player* _player, int is_right)
    : GameObject(
          { BusStartPosition + static_cast<double>(is_right * SeatWidthHeight * 2) + PassengerPadding,
            static_cast<double>(static_cast<int>(_busline) * (SeatWidthHeight + GapHeight)) + PassengerPadding }),
      player(_player), busline(_busline)
{
    AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Passenger.spt", this));
    AddGOComponent(new util::Timer());
    AddGOComponent(new CS230::ShowCollision());
    current_state = &state_idle;
    current_state->Enter(this);
    Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(new Food(this, GetPosition()));
}

bool Passenger::CanCollideWith(GameObjectTypes other_object_type)
{
    if (other_object_type == GameObjectTypes::Player)
    {
        return true;
    }
    return false;
}

void Passenger::ResolveCollision(GameObject* other_object)
{
    if (other_object->Type() == GameObjectTypes::Player)
    {
        if (has_food == true && get_mad == false)
        {
            if (player->GetCanSteel()[static_cast<size_t>(busline)] == true)
            {
                has_food = false;
                change_state(&state_sad);
            }
            else
            {
                get_mad = true;
                change_state(&state_angry);
            }
        }
    }
}

void Passenger::State_Idle::Enter([[maybe_unused]] GameObject* object)
{
    Passenger* passenger = static_cast<Passenger*>(object);
    passenger->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Idle));
}

void Passenger::State_Idle::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
}

void Passenger::State_Idle::CheckExit([[maybe_unused]] GameObject* object)
{
}

void Passenger::State_Angry::Enter([[maybe_unused]] GameObject* object)
{
    Passenger* passenger = static_cast<Passenger*>(object);
    passenger->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Angry));
    passenger->player->GetGOComponent<Score>()->Sub(10);
}

void Passenger::State_Angry::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
}

void Passenger::State_Angry::CheckExit([[maybe_unused]] GameObject* object)
{
}

void Passenger::State_Sad::Enter([[maybe_unused]] GameObject* object)
{
    Passenger* passenger = static_cast<Passenger*>(object);
    passenger->player->GetGOComponent<Score>()->Add(20);
    passenger->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Sad));
    const auto timer = passenger->GetGOComponent<util::Timer>();
    if (!timer)
    {
        throw std::logic_error("No Timer in Passenger!");
    }
    else
    {
        timer->ResetTimeStamp();
    }
    const auto tears_particle = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Tears>>();
    if (tears_particle)
    {
        tears_particle->Emit(1, Math::vec2{ 10, PassengerWidthHeight * 2 / 3 } + passenger->GetPosition(), { 0, 0 }, { -10, -10 }, 3.1415926535 / 3);
        tears_particle->Emit(1, Math::vec2{ PassengerWidthHeight - 10, PassengerWidthHeight * 2 / 3 } + passenger->GetPosition(), { 0, 0 }, { 10, -10 }, 3.1415926535 / 3);
    }
}

void Passenger::State_Sad::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
    Passenger* passenger = static_cast<Passenger*>(object);
    const auto timer = passenger->GetGOComponent<util::Timer>();
    if ((timer)&&(timer->GetElapsedSeconds() >= cry_timer))
    {
        if (!timer)
        {
            throw std::logic_error("No Timer in Passenger!");
        }
        else{
            timer->ResetTimeStamp();
        }
        
        const auto tears_particle = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Tears>>();
        if (tears_particle)
        {
            tears_particle->Emit(1, Math::vec2{ 10, PassengerWidthHeight * 2 / 3 } + passenger->GetPosition(), { 0, 0 }, { -10, -10 }, 3.1415926535 / 3);
            tears_particle->Emit(1, Math::vec2{ PassengerWidthHeight - 10, PassengerWidthHeight * 2 / 3 } + passenger->GetPosition(), { 0, 0 }, { 10, -10 }, 3.1415926535 / 3);
        }
    }
}

void Passenger::State_Sad::CheckExit([[maybe_unused]] GameObject* object)
{
}