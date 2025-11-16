/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Texture.hpp"

#include "CS200/IRenderer2D.hpp"
#include "CS200/Image.hpp"
#include "Engine.hpp"
#include "Window.hpp"
#include "Matrix.hpp"
#include "OpenGL/GL.hpp"

namespace CS230
{
    
    void Texture::Draw([[maybe_unused]]const Math::TransformationMatrix& display_matrix, unsigned int color)
    {
        const auto window_size = Engine::GetWindow().GetSize();
        const Math::ivec2 draw_size   = { std::min(window_size.x, image_size.x), std::min(window_size.y, image_size.y) };

        Draw(display_matrix, { 0, 0 }, draw_size, color);
    }

    void Texture::Draw(const Math::TransformationMatrix& display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size, unsigned int color)
    {
        CS200::IRenderer2D& renderer = Engine::GetRenderer2D();

        const Math::ivec2 slice_amount = { image_size.x / frame_size.x, image_size.y / frame_size.y };

        // make texture slice matrix
        const Math::vec2           frame_size_scale = { static_cast<double>(frame_size.x) / image_size.x, static_cast<double>(frame_size.y) / image_size.y };
        
        const Math::vec2 translation_before_frame_size_scale = { static_cast<double>(texel_position.x) / frame_size.x, static_cast<double>(slice_amount.y - (texel_position.y / frame_size.y) - 1) };
        Math::TransformationMatrix texcoord_transform = Math::ScaleMatrix(frame_size_scale) * Math::TranslationMatrix(translation_before_frame_size_scale);
        Math::vec2                 texel_coord_bl                      = texcoord_transform * Math::vec2{ 0.0, 0.0 };
        Math::vec2                 texel_coord_tr                      = texcoord_transform * Math::vec2{ 1.0, 1.0 };

        // world transformation
        const Math::vec2 world_scale = { display_matrix[0][0] * frame_size.x, display_matrix[1][1] * frame_size.y };
        Math::vec2       set_bottom_left{};
        Math::vec2       world_tranlation{};
        if (Math::IsIdentityMatrix(display_matrix) || slice_amount == Math::ivec2{1,1})//background or just draw without slicing
        {
            //make bottomleft always one, slice reallign
            const auto bl_y_left = texel_coord_bl.y;
            texel_coord_bl.y -= bl_y_left;
            texel_coord_tr.y -= bl_y_left;

            const auto bl_x_left = texel_coord_bl.x;
            texel_coord_bl.x -= bl_x_left;
            texel_coord_tr.x -= bl_x_left;

            set_bottom_left = { (image_size.x < frame_size.x ? image_size.x - frame_size.x : 0.0) + frame_size.x * 0.5,
                                (image_size.y < frame_size.y ? image_size.y - frame_size.y : 0.0) + frame_size.y * 0.5 };
            
        }
        else
        {
            texcoord_transform =
                Math::ScaleMatrix(Math::vec2{ static_cast<double>(frame_size.x) / image_size.x, static_cast<double>(frame_size.y) / image_size.y }) *
                Math::TranslationMatrix(Math::vec2{ static_cast<double>(texel_position.x) / frame_size.x, static_cast<double>(image_size.y - (texel_position.y + frame_size.y)) / frame_size.y });
            set_bottom_left = { (world_scale.x >= 0.0 ? static_cast<double>(frame_size.x) * 0.5 : -static_cast<double>(frame_size.x) * 0.5), frame_size.y * 0.5 };
        }
        
        
        
        world_tranlation                = { display_matrix[0][2], display_matrix[1][2] };
        const auto       world_transfromation = Math::TranslationMatrix(world_tranlation) * Math::TranslationMatrix(set_bottom_left) * Math::ScaleMatrix(world_scale);

        renderer.DrawQuad(world_transfromation, textureHandle, texel_coord_bl, texel_coord_tr, color);
    }

    Math::ivec2 Texture::GetSize() const
    {
        return image_size;
    }

    Texture::~Texture()
    {
        GL::DeleteTextures(1, &textureHandle), textureHandle = 0;
    }

    Texture::Texture(Texture&& temporary) noexcept : image_size{ std::move(temporary.image_size) }, textureHandle{ std::move(temporary.textureHandle) }
    {

        
    }

    Texture& Texture::operator=(Texture&& temporary) noexcept
    {
        std::swap(image_size, (temporary.image_size));
        std::swap(textureHandle, temporary.textureHandle);
        return *this;
    }

    Texture::Texture(const std::filesystem::path& file_name)
    {
        const auto image = CS200::Image{ file_name, true };
        image_size       = image.GetSize();
        textureHandle    = OpenGL::CreateTextureFromImage(image, OpenGL::Filtering::NearestPixel, OpenGL::Wrapping::ClampToEdge);
    }

    Texture::Texture([[maybe_unused]] OpenGL::TextureHandle given_texture, [[maybe_unused]] Math::ivec2 the_size) : image_size{ the_size }, textureHandle{ given_texture }
    {
    }
}
