/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Sprite.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 19, 2025
*/

#include "GameObject.h"
#include "Sprite.h"
#include "TextureManager.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include <fstream>

CS230::Sprite::Sprite(const std::filesystem::path& sprite_file, GameObject* _given_object) {
    Load(sprite_file, _given_object);
}

CS230::Sprite::~Sprite()
{
    for (Animation* animation : animations) {
        delete animation;
    }
    animations.clear();
}

CS230::Sprite::Sprite(Sprite&& temporary) noexcept :
    texture(std::move(temporary.texture)),
    hotspots(std::move(temporary.hotspots)),
    current_animation(temporary.current_animation),
    frame_size(temporary.frame_size),
    frame_texels(std::move(temporary.frame_texels)),
    animations(std::move(temporary.animations))
{}

CS230::Sprite& CS230::Sprite::operator=(Sprite && temporary) noexcept
{
    std::swap(texture, temporary.texture);
    std::swap(hotspots, temporary.hotspots);
    std::swap(current_animation, temporary.current_animation);
    std::swap(frame_size, temporary.frame_size);
    std::swap(frame_texels, frame_texels);
    std::swap(animations, temporary.animations);
    return *this;
}

void CS230::Sprite::Update(double dt)
{
    animations[current_animation]->Update(dt);
}

void CS230::Sprite::Load(const std::filesystem::path& sprite_file, GameObject* _given_object)
{
    const std::filesystem::path sprite_path = assets::locate_asset(sprite_file);
    given_object = _given_object;
    animations.clear();
    if (sprite_path.extension() != ".spt")
    {
        throw std::runtime_error(sprite_path.generic_string() + " is not a .spt file");
    }
    
    std::ifstream               in_file(sprite_path);

    if (in_file.is_open() == false) {
        throw std::runtime_error("Failed to load " + sprite_path.generic_string());
    }

    hotspots.clear();
    frame_texels.clear();

    std::string text;
    in_file >> text;
    texture = Engine::GetTextureManager().Load(text);
    frame_size = Engine::GetTextureManager().Load(text)->GetSize();

    in_file >> text;
    while (in_file.eof() == false) {
        if (text == "FrameSize") {
            in_file >> frame_size.x;
            in_file >> frame_size.y;
        }
        
        else if (text == "NumFrames") {
            int frame_count;
            in_file >> frame_count;
            for (int i = 0; i < frame_count; i++) {
                frame_texels.push_back({ frame_size.x * i, 0 });
            }
        }
        else if (text == "Frame") {
            int frame_location_x, frame_location_y;
            in_file >> frame_location_x;
            in_file >> frame_location_y;
            frame_texels.push_back({ frame_location_x, frame_location_y });
        }
        else if (text == "HotSpot") {
            int hotspot_x, hotspot_y;
            in_file >> hotspot_x;
            in_file >> hotspot_y;
            hotspots.push_back({ hotspot_x, hotspot_y });
        }
        else if (text == "Anim") {
            in_file >> text;
            animations.push_back(new Animation(text));
        }
        else if (text == "RectCollision") {
            Math::irect boundary;
            in_file >> boundary.point_1.x >> boundary.point_1.y >> boundary.point_2.x >> boundary.point_2.y;
            if (given_object == nullptr) {
                Engine::GetLogger().LogError("Cannot add collision to a null object");
            }
            else {
                given_object->AddGOComponent(new RectCollision(boundary, given_object));
            }
        }
        else if (text == "CircleCollision") {
            double radius;
            in_file >> radius;
            if (given_object == nullptr) {
                Engine::GetLogger().LogError("Cannot add collision to a null object");
            }
            else {
                given_object->AddGOComponent(new CircleCollision(radius, given_object));
            }
        }
        else {
            Engine::GetLogger().LogError("Unknown command: " + text);
        }
        in_file >> text;
    }
    if (frame_texels.empty() == true) {
        frame_texels.push_back({ 0,0 });
    }

    if (animations.empty() == true) {
        animations.push_back(new Animation());
        PlayAnimation(0);
    }
    

}

void CS230::Sprite::Draw(Math::TransformationMatrix display_matrix){
    texture->Draw(display_matrix * Math::TranslationMatrix(-GetHotSpot(0)), GetFrameTexel(animations[current_animation]->CurrentFrame()), GetFrameSize());
}

Math::ivec2 CS230::Sprite::GetHotSpot(size_t index)
{
	if ( index > hotspots.size()) {
		Engine::GetLogger().LogDebug("Invalid index in hospot!");
		return Math::ivec2{ 0,0 };
	}
	return hotspots[index];
}

Math::ivec2 CS230::Sprite::GetFrameSize()
{
    return frame_size;
}

void CS230::Sprite::PlayAnimation(size_t animation)
{
    if (animation >= animations.size()) {
        Engine::GetLogger().LogDebug("Invalid index in animation!");
        current_animation = 0;
        return;
    }
    current_animation = animation;
    animations[current_animation]->Reset();
}

bool CS230::Sprite::AnimationEnded()
{
    return animations[current_animation]->Ended();
}

Math::ivec2 CS230::Sprite::GetFrameTexel(size_t index) const
{
    if ( index >= frame_texels.size()) {
        Engine::GetLogger().LogDebug("Invalid index in frametexles!");
        return Math::ivec2{ 0,0 };
    }
    return frame_texels[index];
}
