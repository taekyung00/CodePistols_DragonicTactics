#pragma once
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Texture.hpp"
#include "../../../Engine/GameState.hpp"

class GoodSandwich : public CS230::GameState {
public:
    GoodSandwich(){}

    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;
    void DrawImGui() override { };

    gsl::czstring GetName() const override
    {
        return "Good Sandwich";
    }

private:
    std::shared_ptr<CS230::Texture> texture;
};