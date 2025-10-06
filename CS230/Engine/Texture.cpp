/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Texture.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 19, 2025
*/

#include "Texture.h"
#include "Engine.h"




CS230::Texture::Texture(const std::filesystem::path& file_name){
    texture = LoadTexture(file_name.string().c_str());
    SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);
}

CS230::Texture::Texture(Texture2D given_texture) {
    texture = given_texture;
}


CS230::Texture::~Texture() {
    UnloadTexture(texture);
}

CS230::Texture::Texture(Texture&& temporary) noexcept : texture(temporary.texture) {
    temporary.texture.id = 0;
    temporary.texture.width = 0;
    temporary.texture.height = 0;
    temporary.texture.mipmaps = 0;
    temporary.texture.format = 0;
}

CS230::Texture& CS230::Texture::operator=(Texture&& temporary) noexcept {
    std::swap(texture.id, temporary.texture.id);
    std::swap(texture.width, temporary.texture.width);
    std::swap(texture.height, temporary.texture.height);
    std::swap(texture.mipmaps, temporary.texture.mipmaps);
    std::swap(texture.format, temporary.texture.format);
    return *this;
}


Math::ivec2 CS230::Texture::GetSize() const {
    return { texture.width, texture.height };
}


void CS230::Texture::Draw(Math::TransformationMatrix display_matrix, unsigned int color) {
    Draw(display_matrix, { 0, 0 }, { texture.width, texture.height }, color);
}


void CS230::Texture::Draw(Math::TransformationMatrix display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size, unsigned int color) {
    const double render_height = rlGetFramebufferHeight();
    Math::vec2 bottom_left = display_matrix * Math::vec2{ 0, 0 };
    Math::vec2 bottom_right = display_matrix * Math::vec2{ double(frame_size.x), 0 };
    Math::vec2 top_left = display_matrix * Math::vec2{ 0, double(frame_size.y) };
    Math::vec2 top_right = display_matrix * Math::vec2{ double(frame_size.x), double(frame_size.y) };

    bottom_left.y = bottom_left.y * -1 + render_height;
    bottom_right.y = bottom_right.y * -1 + render_height;
    top_left.y = top_left.y * -1 + render_height;
    top_right.y = top_right.y * -1 + render_height;

    const float left_u = float(texel_position.x) / float(texture.width);
    const float right_u = (float(texel_position.x) + frame_size.x) / float(texture.width);
    const float bottom_v = (float(texel_position.y) + frame_size.y) / float(texture.height);
    const float top_v = float(texel_position.y) / float(texture.height);

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);

    rlColor4ub(
        static_cast<unsigned char>((color & 0xFF000000) >> 24),
        static_cast<unsigned char>((color & 0x00FF0000) >> 16),
        static_cast<unsigned char>((color & 0x0000FF00) >> 8),
        static_cast<unsigned char>((color & 0x000000FF) >> 0));
    rlNormal3f(0.0f, 0.0f, 1.0f);

    rlTexCoord2f(left_u, top_v);
    rlVertex2f(float(top_left.x), float(top_left.y));

    rlTexCoord2f(left_u, bottom_v);
    rlVertex2f(float(bottom_left.x), float(bottom_left.y));

    rlTexCoord2f(right_u, bottom_v);
    rlVertex2f(float(bottom_right.x), float(bottom_right.y));

    rlTexCoord2f(right_u, top_v);
    rlVertex2f(float(top_right.x), float(top_right.y));

    rlEnd();
    rlSetTexture(0);
}


