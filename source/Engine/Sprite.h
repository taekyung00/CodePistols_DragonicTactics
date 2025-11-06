/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Sprite.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 19, 2025
*/

#pragma once
#include <string>
#include "Vec2.hpp"
#include "Texture.hpp"
#include "Matrix.hpp"
#include "Engine.hpp"
#include "Animation.h"
#include "Component.h"
#include "Collision.h"


namespace CS230 {
    class GameObject;
    class Sprite : public Component {
    public:
        Sprite(const std::filesystem::path& sprite_file,GameObject* given_object);
        ~Sprite();

        Sprite(const Sprite&) = delete;
        Sprite& operator=(const Sprite&) = delete;

        Sprite(Sprite&& temporary) noexcept;
        Sprite& operator=(Sprite&& temporary) noexcept;
        void Update(double dt) override;
        void Load(const std::filesystem::path& sprite_file, GameObject* _given_object);
        void Draw(Math::TransformationMatrix display_matrix);
        Math::ivec2 GetHotSpot(size_t index);
        Math::ivec2 GetFrameSize();

        void PlayAnimation(size_t animation);
        bool AnimationEnded();

        const size_t& CurrentAnimation() const { return current_animation; }
    private:
        Math::ivec2 GetFrameTexel(size_t index) const;
        // Texture* texture;
        std::shared_ptr<CS230::Texture> texture;
        std::vector<Math::ivec2> hotspots;

        size_t current_animation;
        Math::ivec2 frame_size;
        std::vector<Math::ivec2> frame_texels;
        std::vector<Animation*> animations;

        GameObject* given_object;
    };
}
