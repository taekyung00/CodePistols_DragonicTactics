/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Vec2.hpp"
#include <algorithm>

namespace Math
{
    struct [[nodiscard]] rect
    {
        Math::vec2 point1{ 0.0, 0.0 };
        Math::vec2 point2{ 0.0, 0.0 };

        double Left() const noexcept; // TODO

        double Right() const noexcept; // TODO

        double Top() const noexcept; // TODO

        double Bottom() const noexcept; // TODO

        vec2 Size() const noexcept; // TODO
    };

    struct [[nodiscard]] irect
    {
        Math::ivec2 point1{ 0, 0 };
        Math::ivec2 point2{ 0, 0 };

        int Left() const noexcept; // TODO

        int Right() const noexcept; // TODO

        int Top() const noexcept; // TODO

        int Bottom() const noexcept; // TODO

        ivec2 Size() const noexcept; // TODO
    };
}
