#pragma once
#include "../../../Engine/GameState.hpp"
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Texture.hpp"

class BadSandwich : public CS230::GameState {
public:
    BadSandwich(){}

    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;
    void DrawImGui() override { };

    gsl::czstring GetName() const override
    {
        return "Bad Sandwich";
    }

private:
    std::shared_ptr<CS230::Texture> texture;
};