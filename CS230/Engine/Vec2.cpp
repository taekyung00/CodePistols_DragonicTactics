/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Vec2.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 18, 2025
*/
#include "Vec2.h"


bool Math::vec2::operator==(const vec2& v)
{
    return { (x == v.x) && (y == v.y) };
}

bool Math::vec2::operator!=(const vec2& v)
{
    return !operator==(v);
}

Math::vec2 Math::vec2::operator+(const vec2& v) {
    return { x + v.x, y + v.y };
}

Math::vec2 Math::vec2::operator+(const vec2& v) const
{
    return { x + v.x, y + v.y };
}

Math::vec2& Math::vec2::operator+=(const vec2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Math::vec2 Math::vec2::operator-(const vec2& v)
{
    return { x - v.x, y - v.y };
}

Math::vec2 Math::vec2::operator-(const vec2& v) const
{
    return { x - v.x, y - v.y };
}

Math::vec2& Math::vec2::operator-=(const vec2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

Math::vec2 Math::vec2::operator*(double scale)
{
    return { x * scale, y * scale };
}

Math::vec2 Math::vec2::operator*(double scale) const
{
    return { x * scale, y * scale };
}

Math::vec2& Math::vec2::operator*=(double scale)
{
    x *= scale;
    y *= scale;
    return *this;
}



Math::vec2 Math::vec2::operator/(double divisor)
{
    return { x / divisor, y / divisor };
}

Math::vec2 Math::vec2::operator/(double divisor) const
{
    return { x / divisor, y / divisor };
}

Math::vec2& Math::vec2::operator/=(double divisor)
{
    x /= divisor;
    y /= divisor;
    return *this;
}

Math::vec2 Math::vec2::operator-()
{
    return { -x,-y };
}

Math::vec2 Math::vec2::operator-() const
{
    return { -x,-y };
}



//Math::vec2 Math::operator-(const vec2& v1, const vec2& v2)
//{
//    return { v1.x - v2.x,v1.x - v2.y };
//}

Math::vec2 Math::operator*(double scale, const vec2& v)
{
    return { scale * v.x ,scale * v.y };
}

Math::vec2& Math::vec2::Normalize()
{
    double magnitude = sqrt(x * x + y*y);
    (*this) /= magnitude;
    return (*this);
}

//Math::vec2 Math::vec2::Normalize() const
//{
//    double magnitude = sqrt(x * x + y * y);
//    Math::vec2 return_vec = (*this) /magnitude;
//    return return_vec;
//}

/*======================================================*/

bool Math::ivec2::operator==(const ivec2& v)
{
    return { (x == v.x) && (y == v.y) };
}

bool Math::ivec2::operator!=(const ivec2& v)
{
    return !operator==(v);
}

Math::ivec2 Math::ivec2::operator+(const ivec2& v) {
    return { x + v.x, y + v.y };
}

Math::ivec2 Math::ivec2::operator+(const ivec2& v) const
{
    return { x + v.x, y + v.y };
}

Math::ivec2& Math::ivec2::operator+=(const ivec2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Math::ivec2 Math::ivec2::operator-(const ivec2& v)
{
    return { x - v.x, y - v.y };
}

Math::ivec2 Math::ivec2::operator-(const ivec2& v) const
{
    return { x - v.x, y - v.y };
}

Math::ivec2& Math::ivec2::operator-=(const ivec2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

Math::ivec2 Math::ivec2::operator*(int scale)
{
    return { x * scale, y * scale };
}

Math::ivec2 Math::ivec2::operator*(int scale) const
{
    return { x * scale, y * scale };
}

Math::ivec2& Math::ivec2::operator*=(int scale)
{
    x *= scale;
    y *= scale;
    return *this;
}

Math::ivec2 Math::ivec2::operator/(int divisor)
{
    return { x / divisor, y / divisor };
}

Math::ivec2 Math::ivec2::operator/(int divisor) const
{
    return { x / divisor, y / divisor };
}

Math::ivec2& Math::ivec2::operator/=(int divisor)
{
    x /= divisor;
    y /= divisor;
    return *this;
}

Math::vec2 Math::ivec2::operator*(double scale)
{
    return { x * scale, y * scale };
}

Math::vec2 Math::ivec2::operator*(double scale) const
{
    return { x * scale, y * scale };
}

Math::vec2 Math::ivec2::operator/(double divisor)
{
    return { x / divisor, y / divisor };
}

Math::vec2 Math::ivec2::operator/(double divisor) const
{
    return { x / divisor, y / divisor };
}

Math::ivec2 Math::ivec2::operator-()
{
    return { -x, -y };
}

//Math::ivec2 Math::operator-(const ivec2& v)
//{
//    return { -v.x, -v.y };
//}

Math::ivec2 Math::operator*(int scale, const ivec2& v)
{
    return { scale * v.x ,scale * v.y };
}

Math::vec2 Math::operator*(double scale, const ivec2& v)
{
    return { scale * v.x ,scale * v.y };
}
