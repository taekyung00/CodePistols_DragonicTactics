#include "Bus.h"
#include "../../../Engine/GameStateManager.hpp"
#include "../../Score.h"
#include "Player.h"

Bus::Bus(Player* player_ptr) :  GameObject({ BusStartPosition, 0 }), player(player_ptr)
{
    AddGOComponent(new CS230::Sprite("Assets/sprites/CS230_Final/Bus.spt", this));
    AddGOComponent(new CS230::ShowCollision());
}

void Bus::Update(double dt)
{
    GameObject::Update(dt);

    // double player_bottom = player->GetGOComponent<CS230::RectCollision>()->WorldBoundary().Bottom();
    double player_top = player->GetGOComponent<CS230::RectCollision>()->WorldBoundary().Top();

    std::vector<bool>& player_can_steel = player->SetCanSteel();

    // line1
    if (player_top >= start_line[static_cast<int>(BusLine::line1)] && player_top < end_line[static_cast<int>(BusLine::line1)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = true;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }
    else if (player_top >= end_line[static_cast<int>(BusLine::line1)] && player_top < start_line[static_cast<int>(BusLine::line2)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }

    // line2
    else if (player_top >= start_line[static_cast<int>(BusLine::line2)] && player_top < end_line[static_cast<int>(BusLine::line2)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = true;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }
    else if (player_top >= end_line[static_cast<int>(BusLine::line2)] && player_top < start_line[static_cast<int>(BusLine::line3)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }

    // line3
    else if (player_top >= start_line[static_cast<int>(BusLine::line3)] && player_top < end_line[static_cast<int>(BusLine::line3)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = true;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }
    else if (player_top >= end_line[static_cast<int>(BusLine::line3)] && player_top < start_line[static_cast<int>(BusLine::line4)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }

    // line4
    else if (player_top >= start_line[static_cast<int>(BusLine::line4)] && player_top < end_line[static_cast<int>(BusLine::line4)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = true;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }
    else if (player_top >= end_line[static_cast<int>(BusLine::line4)] && player_top < start_line[static_cast<int>(BusLine::line5)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;
    }

    // line5
    else if (player_top >= start_line[static_cast<int>(BusLine::line5)] && player_top < end_line[static_cast<int>(BusLine::line5)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = true;
    }
    else if (player_top >= end_line[static_cast<int>(BusLine::line5)])
    {
        player_can_steel[static_cast<int>(BusLine::line1)] = false;
        player_can_steel[static_cast<int>(BusLine::line2)] = false;
        player_can_steel[static_cast<int>(BusLine::line3)] = false;
        player_can_steel[static_cast<int>(BusLine::line4)] = false;
        player_can_steel[static_cast<int>(BusLine::line5)] = false;

        const auto score = player->GetGOComponent<Score>();
        if(score == nullptr){
            throw std::runtime_error("no score, logical issue");
        }
        if (score->Value() >= 80)
        {
            is_end         = true;
            sandwich_level = SandwichLevel::GoodSandwich;
            // Engine::GetGameStateManager().PopState();
            // Engine::GetGameStateManager().PushState<GoodSandwich>();
        }
        else if (player->GetGOComponent<Score>()->Value() < 80 && player->GetGOComponent<Score>()->Value() >= 40)
        {
            is_end         = true;
            sandwich_level = SandwichLevel::NormalSandwich;
            // Engine::GetGameStateManager().PopState();
            // Engine::GetGameStateManager().PushState<NormalSandwich>();
        }
        else
        {
            is_end         = true;
            sandwich_level = SandwichLevel::BadSandwich;
            // Engine::GetGameStateManager().PopState();
            // Engine::GetGameStateManager().PushState<BadSandwich>();
        }
    }

    // if (player_can_steel[static_cast<int>(BusLine::line1)]) {
    //	Engine::GetLogger().LogDebug("line1 true");
    // }
    // else if (player_can_steel[static_cast<int>(BusLine::line2)]) {
    //	Engine::GetLogger().LogDebug("line2 true");
    // }
    // else if (player_can_steel[static_cast<int>(BusLine::line3)]) {
    //	Engine::GetLogger().LogDebug("line3 true");
    // }
    // else if (player_can_steel[static_cast<int>(BusLine::line4)]) {
    //	Engine::GetLogger().LogDebug("line4 true");
    // }
    // else if (player_can_steel[static_cast<int>(BusLine::line5)]) {
    //	Engine::GetLogger().LogDebug("line5 true");
    // }
}