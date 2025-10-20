#pragma once
#include "../../../Engine/Camera.h"
#include "../../../Engine/Component.h"
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Fonts.h"
#include "../../../Engine/Texture.hpp"
#include "../../../Engine/Vec2.hpp"
#include <filesystem>
#include <memory>

class Grid : public CS230::Component
{
public:
    Grid();
    void Update(double dt) override;
    void Draw(const CS230::Camera& camera);

private:
    Math::ivec2                     window_size;
    bool                            draw = true;
    std::shared_ptr<CS230::Texture> black_dot;
    std::shared_ptr<CS230::Texture> red_dot;
    std::shared_ptr<CS230::Texture> blue_dot;
    std::shared_ptr<CS230::Texture> dot_number;
};