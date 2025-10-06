/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Rect.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 27, 2025
*/
#pragma once
#include <cmath>
#include <utility>
#include "Vec2.h"

namespace Math {
    struct [[nodiscard]] rect {
        Math::vec2 point_1{ 0.0, 0.0 };
        Math::vec2 point_2{ 0.0, 0.0 };

        Math::vec2 Size() const noexcept {
            return {
                Right()-Left(),
                Top()-Bottom()
            };
        }

        double Left() const noexcept;
        double Right() const noexcept;
        double Top() const noexcept;
        double Bottom() const noexcept;
    };
    struct [[nodiscard]] irect {
        Math::ivec2 point_1{ 0, 0 };
        Math::ivec2 point_2{ 0, 0 };

        Math::ivec2 Size() const noexcept {
            return {
                Right() - Left(),
                Top() - Bottom()
            };
        }

        int Left() const noexcept;
        int Right() const noexcept;
        int Top() const noexcept;
        int Bottom() const noexcept;
    };
}


