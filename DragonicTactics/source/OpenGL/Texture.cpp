/**
 * \file
 * \author Rudy Castan
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Texture.hpp"
#include "CS200/Image.hpp"
#include "Environment.hpp"
#include "GL.hpp"

namespace OpenGL
{
    TextureHandle CreateTextureFromImage(const CS200::Image& image, Filtering filtering, Wrapping wrapping) noexcept
    {
        Math::ivec2 image_size = image.GetSize();
        return CreateTextureFromMemory(image_size, { image.data(), static_cast<size_t>(image_size.x * image_size.y) }, filtering, wrapping);
    }

    TextureHandle CreateTextureFromMemory(Math::ivec2 size, std::span<const CS200::RGBA> colors, Filtering filtering, Wrapping wrapping) noexcept
    {
        TextureHandle texture{};
        GL::GenTextures(1, &texture);
        GL::BindTexture(GL_TEXTURE_2D, texture);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

        constexpr int base_mipmap_level = 0; // just bare level, we don't care
        constexpr int zero_border       = 0;
        GL::TexImage2D(GL_TEXTURE_2D, base_mipmap_level, GL_RGBA8, size.x, size.y, zero_border, GL_RGBA, GL_UNSIGNED_BYTE, colors.data());
        GL::BindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

    TextureHandle CreateRGBATexture(Math::ivec2 size, Filtering filtering, Wrapping wrapping) noexcept
    {
        TextureHandle texture{};
        GL::GenTextures(1, &texture);
        GL::BindTexture(GL_TEXTURE_2D, texture);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

        constexpr int base_mipmap_level = 0; // just bare level, we don't care
        constexpr int zero_border       = 0;
         if (OpenGL::IsWebGL || OpenGL::current_version() >= OpenGL::version(4, 2))
         {
             GL::TexStorage2D(GL_TEXTURE_2D, base_mipmap_level + 1, GL_RGBA8, size.x, size.y);
         }
         else
         {
            GL::TexImage2D(GL_TEXTURE_2D, base_mipmap_level, GL_RGBA8, size.x, size.y, zero_border, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);//match channel!!!!!!!!!!!!!!
         }
        
        GL::BindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

    void SetFiltering(TextureHandle texture_handle, Filtering filtering) noexcept
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void SetWrapping(TextureHandle texture_handle, Wrapping wrapping, TextureCoordinate coord) noexcept
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        switch (coord)
        {
            case TextureCoordinate::Both:
                GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
                GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));
                break;
            case TextureCoordinate::S: GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping)); break;
            case TextureCoordinate::T: GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping)); break;
        }

        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    
}
