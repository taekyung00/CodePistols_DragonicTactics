#pragma once
#include "../../../Engine/GameState.hpp"
#include "../../../Engine/Texture.hpp"
#include "../../../Engine/Camera.h"
#include "../../../Engine/GameObjectManager.h"

#include "../Components/Grid.h"
#include "../Objects/Player.h"
#include "../Objects/Bus.h"
#include "../Objects/BusLine.h"
#include "../Objects/Passenger.h"
#include "../../Particles.h"
#include "../Objects/Obstacle.h"
#include "../Components/Random.h"
class Project : public CS230::GameState {
public:
    Project();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    Player* player_ptr;
    Bus* bus_ptr;
    //CS230::Camera* camera;
    //static constexpr int default_width = 600;
    //static constexpr int default_height = 800;
};