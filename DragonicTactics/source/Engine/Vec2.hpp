/**
 * \file
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <cmath>
#include <iostream>
#include <limits>

namespace Math
{


    struct vec2;
    struct vec3;

    struct [[nodiscard]] ivec2
    {
        int x{ 0 };
        int y{ 0 };

        // Constructors
        ivec2() noexcept = default;
        // ivec2(int _x, int _y) noexcept : x(_x), y(_y) { };
        constexpr ivec2(int _x, int _y) noexcept : x(_x), y(_y) { };
        // ivec2(const ivec2& rhs) = default;
        //  ivec2(int xy) noexcept;

        // Conversion
        ivec2(const vec2& v) noexcept;

        // Arithmetic operators
        ivec2& operator+=(const ivec2& rhs) noexcept;
        ivec2& operator-=(const ivec2& rhs) noexcept;
        ivec2& operator*=(int scalar) noexcept;
        ivec2& operator/=(int scalar) noexcept;

        // Unary operators
        ivec2 operator+() const noexcept;
        ivec2 operator-() const noexcept;

        // length
        double Length();
    };

    // Non-member operators
    ivec2 operator+(const ivec2& lhs, const ivec2& rhs) noexcept;
    ivec2 operator-(const ivec2& lhs, const ivec2& rhs) noexcept;
    ivec2 operator*(const ivec2& v, int scalar) noexcept;
    ivec2 operator*(int scalar, const ivec2& v) noexcept;
    ivec2 operator/(const ivec2& v, int scalar) noexcept;

    // Comparison operators
    bool operator==(const ivec2& lhs, const ivec2& rhs) noexcept;
    bool operator!=(const ivec2& lhs, const ivec2& rhs) noexcept;

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const ivec2& v);
    std::istream& operator>>(std::istream& is, ivec2& v);

    struct ivec3
    {
        int x{ 0 };
        int y{ 0 };
        int z{ 0 };

        // Constructors
        ivec3() noexcept = default;
        ivec3(int _x, int _y, int _z) noexcept : x(_x), y(_y), z(_z) { };
        // ivec3(const ivec3& rhs) = default;
        //  ivec3(int xy) noexcept;

        // Conversion
        ivec3(const vec3& v) noexcept;

        // Arithmetic operators
        ivec3& operator+=(const ivec3& rhs) noexcept;
        ivec3& operator-=(const ivec3& rhs) noexcept;
        ivec3& operator*=(int scalar) noexcept;
        ivec3& operator/=(int scalar) noexcept;

        // Unary operators
        ivec3 operator+() const noexcept;
        ivec3 operator-() const noexcept;

        // length
        double Length();
    };

    // Non-member operators
    ivec3 operator+(const ivec3& lhs, const ivec3& rhs) noexcept;
    ivec3 operator-(const ivec3& lhs, const ivec3& rhs) noexcept;
    ivec3 operator*(const ivec3& v, int scalar) noexcept;
    ivec3 operator*(int scalar, const ivec3& v) noexcept;
    ivec3 operator/(const ivec3& v, int scalar) noexcept;

    // Comparison operators
    bool operator==(const ivec3& lhs, const ivec3& rhs) noexcept;
    bool operator!=(const ivec3& lhs, const ivec3& rhs) noexcept;

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const ivec3& v);
    std::istream& operator>>(std::istream& is, ivec3& v);

    struct vec2
    {
        double x{ 0 };
        double y{ 0 };

        // Constructors
        vec2() noexcept = default;
        constexpr vec2(double _x, double _y) noexcept : x(_x), y(_y) { };
        //  vec2(const vec2& v) noexcept = default;
        // vec2(const vec2& rhs) = default;
        //  vec2(int xy) noexcept;

        // Conversion
        vec2(const ivec2& v) noexcept : x(static_cast<double>(v.x)), y(static_cast<double>(v.y)) { };

        // Arithmetic operators
        vec2& operator+=(const vec2& rhs) noexcept;
        vec2& operator-=(const vec2& rhs) noexcept;
        vec2& operator*=(int scalar) noexcept;
        vec2& operator/=(int scalar) noexcept;

        // Unary operators
        vec2 operator+() const noexcept;
        vec2 operator-() const noexcept;

        // length
        double Length() const noexcept;

        bool operator<(const vec2& other) const
        {
            return (x < other.x) || (x == other.x && y < other.y);
        }

        // normalize
        vec2 Normalize();
    };

    // Non-member operators
    vec2 operator+(const vec2& lhs, const vec2& rhs) noexcept;
    vec2 operator-(const vec2& lhs, const vec2& rhs) noexcept;
    vec2 operator*(const vec2& v, int scalar) noexcept;
    vec2 operator*(int scalar, const vec2& v) noexcept;
    vec2 operator*(const vec2& v, double scalar) noexcept;
    vec2 operator*(double scalar, const vec2& v) noexcept;
    vec2 operator/(const vec2& v, int scalar) noexcept;

    // Comparison operators
    bool operator==(const vec2& lhs, const vec2& rhs) noexcept;
    bool operator!=(const vec2& lhs, const vec2& rhs) noexcept;

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const vec2& v);
    std::istream& operator>>(std::istream& is, vec2& v);

    struct vec3
    {
        double x{ 0 };
        double y{ 0 };
        double z{ 0 };

        // Constructors
        vec3() noexcept = default;
        vec3(double _x, double _y, double _z) noexcept : x(_x), y(_y), z(_z) { };
        // vec3(const vec3& rhs) = default;
        //  vec3(int xy) noexcept;

        // Conversion
        vec3(const ivec3& v) noexcept : x(static_cast<double>(v.x)), y(static_cast<double>(v.y)), z(static_cast<double>(v.z)) { };

        // Arithmetic operators
        vec3& operator+=(const vec3& rhs) noexcept;
        vec3& operator-=(const vec3& rhs) noexcept;
        vec3& operator*=(int scalar) noexcept;
        vec3& operator/=(int scalar) noexcept;

        // Unary operators
        vec3 operator+() const noexcept;
        vec3 operator-() const noexcept;

        // length
        double Length();
    };

    // Non-member operators
    vec3 operator+(const vec3& lhs, const vec3& rhs) noexcept;
    vec3 operator-(const vec3& lhs, const vec3& rhs) noexcept;
    vec3 operator*(const vec3& v, int scalar) noexcept;
    vec3 operator*(int scalar, const vec3& v) noexcept;
    vec3 operator/(const vec3& v, int scalar) noexcept;

    // Comparison operators
    bool operator==(const vec3& lhs, const vec3& rhs) noexcept;
    bool operator!=(const vec3& lhs, const vec3& rhs) noexcept;

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const vec3& v);
    std::istream& operator>>(std::istream& is, vec3& v);
}
