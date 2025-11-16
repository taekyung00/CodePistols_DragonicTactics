/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Font.hpp"

#include "CS200/Image.hpp"
#include "Engine.hpp"
#include "Error.hpp"
#include "Matrix.hpp"
#include "Path.hpp"
#include "TextureManager.hpp"
#include <algorithm>
#include <stb_image.h>

/*
 * 1. Load font texture and parse character boundaries during construction
 * 2. Measure text dimensions for layout calculations
 * 3. Render text to texture or directly to screen using character sprites
 * 4. Support for colored text and transformation matrices
 */
namespace CS230
{
    Font::Font(const std::filesystem::path& file_name) : texture(file_name), image(file_name /*std::filesystem::path("Assets/Test.png")*/, is_image_flipped)
    {
        //  * Font Image Requirements:
        //  * - Characters arranged horizontally in a single row
        //  * - First pixel must be white (0xFFFFFFFF) as a format marker


        /*[[maybe_unused]] const auto _00       = GetPixel({ 0, 0 });
        [[maybe_unused]] const auto _00_array = CS200::ColorArray(_00);

        [[maybe_unused]] const auto _10       = GetPixel({ 1, 0 });
        [[maybe_unused]] const auto _10_array = CS200::ColorArray(_10);

        [[maybe_unused]] const auto _20       = GetPixel({ 2, 0 });
        [[maybe_unused]] const auto _20_array = CS200::ColorArray(_20);

        [[maybe_unused]] const auto _30       = GetPixel({ 3, 0 });
        [[maybe_unused]] const auto _30_array = CS200::ColorArray(_30);

        [[maybe_unused]] const auto _01       = GetPixel({ 0, 1 });
        [[maybe_unused]] const auto _01_array = CS200::ColorArray(_01);

        [[maybe_unused]] const auto _11       = GetPixel({ 1, 1 });
        [[maybe_unused]] const auto _11_array = CS200::ColorArray(_11);

        [[maybe_unused]] const auto _21       = GetPixel({ 2, 1 });
        [[maybe_unused]] const auto _21_array = CS200::ColorArray(_21);

        [[maybe_unused]] const auto _31       = GetPixel({ 3, 1 });
        [[maybe_unused]] const auto _31_array = CS200::ColorArray(_31);

        [[maybe_unused]] const auto _02       = GetPixel({ 0, 2 });
        [[maybe_unused]] const auto _02_array = CS200::ColorArray(_02);

        [[maybe_unused]] const auto _12       = GetPixel({ 1, 2 });
        [[maybe_unused]] const auto _12_array = CS200::ColorArray(_12);

        [[maybe_unused]] const auto _22       = GetPixel({ 2, 2 });
        [[maybe_unused]] const auto _22_array = CS200::ColorArray(_22);

        [[maybe_unused]] const auto _32       = GetPixel({ 3, 2 });
        [[maybe_unused]] const auto _32_array = CS200::ColorArray(_32);*/


        if (GetPixel({ 0, 0 }) == CS200::WHITE)
        {
            FindCharRects();
        }
        else
        {
            Engine::GetLogger().LogError("Font " + file_name.string() + " texture has wrong format!");
            throw std::runtime_error("Font fromat error");
        }
        //  * - Color changes between characters indicate boundaries
        //  * - Characters cover ASCII range from space (' ') to 'z'
        //  * - Image should contain exactly the expected number of characters
        //  *
        //  * Parsing Process:
        //  * The constructor scans the top row of pixels to detect color changes,
        //  * which indicate character boundaries. Each character's rectangular
        //  * region is calculated and stored for later use during text rendering.
        //  *
        //  * Error Handling:
        //  * If the font file is malformed (wrong format, missing characters, or
        //  * incorrect structure), the constructor will throw an error to indicate
        //  * the problem. This ensures that only valid fonts are used for rendering.
    }

    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA color)
    {
        const auto&       window_environment = Engine::GetWindowEnvironment();
        //  * Advanced Caching System:
        //  * - Cache key: Combination of text string and color (format: "text_0xCOLOR")
        const std::string cache_key{ text + std::string("_0x") + std::to_string(color) };

        //  * - Frame tracking: Each cached texture records when it was last accessed
        for (auto it = textures.begin(); it != textures.end();)
        {
            // ++(it->second.time);
            //  * - Reference counting: Uses shared_ptr for automatic memory management

            //  * - Intelligent cleanup: Removes old textures only when they have single reference and are older than 60 frames
            if ((window_environment.FrameCount - it->second.timestamp) > 60 && it->second.texture.use_count() == 1)
            {
                it = textures.erase(it);
            }
            else
            {
                ++it;
            }
        }

        //  * - Performance optimization: Eliminates redundant text rendering operations

        //          * Caching Strategy:
        //  * 1. Check cache using generated key (text + color combination)
        //  * 2. If found: Update last-used frame timestamp and return cached texture
        //  * 3. If not found: Create new texture, cache it, and return
        //  * 4. Periodic cleanup: Remove textures older than 60 frames with reference count of 1

        if (textures.find(cache_key) == textures.end())
        {
            //          * Rendering Process (for new textures):
            //  * 1. Measure total text dimensions to determine optimal texture size
            const Math::ivec2 text_size = MeasureText(text /*std::string("hello")*/);

            //  * 2. Create render target texture using TextureManager
            auto& texture_manager = Engine::GetTextureManager();
            texture_manager.StartRenderTextureMode(text_size.x, text_size.y);
            Math::TransformationMatrix matrix{};
            for (const char c : text /*std::string("hello")*/)
            {
                DrawChar(matrix, c, color);
            }
            auto target_texture = texture_manager.EndRenderTextureMode();
            //  * 3. Render each character from font atlas to the target texture

            //  * 4. Store in cache with current frame timestamp
            //  * 5. Return shared_ptr for client use

            textures[cache_key] = { window_environment.FrameCount, target_texture };

            Engine::GetLogger().LogEvent("Loading Texture: " + text);
        }
        else
        {
            textures[cache_key].timestamp = window_environment.FrameCount;
        }
        return textures[cache_key].texture;
    }

    void Font::FindCharRects()
    {
        CS200::RGBA check_color = GetPixel({ 0, 0 });
        CS200::RGBA next_color;

        int height = texture.GetSize().y;

        int x = 0;
        for (int index = 0; index < num_chars; index++)
        {
            int width = 0;

            do
            {
                width++;
                next_color = GetPixel({ x + width, 0 });
            } while (check_color == next_color);

            check_color = next_color;

            char_rects[index].point_2 = { x + width , height };
            char_rects[index].point_1 = { x, 1  }; // 1 mean ignore line above
            x += width;
        }
        // stbi_image_free(image_data);
        // image.~Image();
    }

    Math::irect& Font::GetCharRect(char c)
    {
        if (c >= ' ' && c <= 'z')
        {
            return char_rects[c - ' '];
        }
        else
        {
            Engine::GetLogger().LogError("Char '" + std::to_string(c) + "' not found");
            return char_rects[0];
        }
    }

    Math::ivec2 Font::MeasureText(std::string text)
    {
        Math::ivec2 text_size = GetCharRect(text[0]).Size();
        for (size_t i = 1; i < text.size(); ++i)
        {
            text_size.x += GetCharRect(text[i]).Size().x;
            if (GetCharRect(text[i]).Size().y > GetCharRect(text[i - 1]).Size().y)
            {
                text_size.y = GetCharRect(text[i]).Size().y;
            }
        }
        return text_size;
    }

    void Font::DrawChar(Math::TransformationMatrix& matrix, char c, CS200::RGBA color)
    {
        const Math::irect& display_rect   = GetCharRect(c);
        
        const Math::ivec2 top_left_texel = { display_rect.Left(), display_rect.Bottom() }; //top_left is 0,0!!
        if (c != ' ')
        {
            //const auto to_center     = Math::TranslationMatrix(Math::vec2(display_rect.Size().x / 2.0, display_rect.Size().y / 2.0));
            const auto flip          = Math::ScaleMatrix(Math::vec2{ 1, -1 });
            //const auto to_bottomleft = Math::TranslationMatrix(Math::vec2(display_rect.Size().x / 2.0, -display_rect.Size().y / 2.0));
            const auto flip_quad     = flip;
            texture.Draw(matrix * flip_quad, top_left_texel, display_rect.Size(), color);
        }
        matrix *= Math::TranslationMatrix(Math::ivec2{ display_rect.Size().x, 0 });
    }

    CS200::RGBA Font::GetPixel(Math::ivec2 texel) // tl is (0,0) !!
    {
        const CS200::RGBA* image_data = image.data();
        const auto         image_size = image.GetSize();
        // if (is_image_flipped)
        //{
        //     const int   index        = (image_size.x * image_size.y - 1) - (texel.x + texel.y * image_size.x);
        //     CS200::RGBA return_color = CS200::rgba_to_abgr(CS200::argb_to_rgba(image_data[index])); // very weird..argb?

        //    return return_color; // endian!!!
        //}
        // else
        //{
        const int   index        = texel.x + texel.y * image_size.x;
        CS200::RGBA return_color = CS200::rgba_to_abgr(image_data[index]);

        return return_color; // endian!!!
        //}
    }

}    
