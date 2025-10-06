/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Font.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 1, 2025
*/

#pragma once
#include "Rect.h"
#include "Texture.h"
#include "Vec2.h"
#include <filesystem>
#include <string>

namespace CS230 {
    class Font {
    public:
        Font(const std::filesystem::path& file_name);
        Texture* PrintToTexture(const std::string& text, unsigned int color = 0xFFFFFFFF);

    private:
        void FindCharRects();
        Math::irect& GetCharRect(char c);
        Math::ivec2  MeasureText(std::string text);
        void DrawChar(Math::TransformationMatrix& matrix, char c, unsigned int color = 0xFFFFFFFF);
        unsigned int GetPixel(Math::ivec2 texel);

        Image original_image;
        Texture texture;
        static constexpr int num_chars = 'z' - ' ' + 1;
        Math::irect char_rects[num_chars];
    };
}


