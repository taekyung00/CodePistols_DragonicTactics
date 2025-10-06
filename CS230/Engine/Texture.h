/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Texture.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 19, 2025
*/

#pragma once
#include "Matrix.h"
#include "Vec2.h"
#include <filesystem>
#include <raylib.h>

namespace CS230 {
    class Font;//because raylib also has font

    class Texture {
    public:
        friend class TextureManager;
        friend class Font;

        void Draw(Math::TransformationMatrix display_matrix, unsigned int color = 0xFFFFFFFF);
        void Draw(
            Math::TransformationMatrix display_matrix,
            Math::ivec2 texel_position,
            Math::ivec2 frame_size,
            unsigned int color = 0xFFFFFFFF
        );
        Math::ivec2 GetSize() const;
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& temporary) noexcept;
        Texture& operator=(Texture&& temporary) noexcept;

    private:
        Texture(Texture2D given_texture);
        Texture(const std::filesystem::path& file_name);

        Texture2D texture = {};
    };
}


