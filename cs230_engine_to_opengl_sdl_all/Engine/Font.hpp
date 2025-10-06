/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Rect.hpp"
#include "Texture.hpp"
#include "Vec2.hpp"
#include <filesystem>
#include <string>

namespace CS230
{
    /**
     * \brief Bitmap font system for rendering text using pre-generated font textures
     *
     * Font provides a lightweight and efficient text rendering system based on
     * bitmap font textures. Rather than using complex font rasterization at runtime,
     * this system loads pre-rendered character sets from image files and uses them
     * to compose text strings quickly and efficiently.
     *
     * Bitmap Font Advantages:
     * - Consistent visual appearance across all platforms and systems
     * - Fast rendering performance with minimal runtime computation
     * - Predictable memory usage and loading times
     * - Simple integration with existing texture and rendering systems
     * - Pixel-perfect control over character appearance and spacing
     *
     * Font File Format:
     * The system expects specially formatted font textures where characters are
     * arranged horizontally in a single row, with specific color markers to
     * indicate character boundaries. The first pixel must be white, and color
     * changes signal the separation between characters.
     *
     * Character Set:
     * Supports ASCII characters from space (' ') to lowercase 'z', covering
     * basic alphanumeric characters, punctuation, and symbols commonly used
     * in game interfaces and debugging displays.
     *
     * Text Rendering Workflow:
     * 1. Load font texture and parse character boundaries during construction
     * 2. Measure text dimensions for layout calculations
     * 3. Render text to texture or directly to screen using character sprites
     * 4. Support for colored text and transformation matrices
     *
     * This font system is particularly well-suited for game development where
     * consistent, fast text rendering is more important than dynamic font
     * sizing or complex typography features.
     */
    class Font
    {
    public:
        /**
         * \brief Load and initialize a bitmap font from an image file
         * \param file_name Path to the font texture image file
         *
         * Constructs a Font object by loading a specially formatted bitmap font
         * texture and parsing the character boundary information. The constructor
         * analyzes the font image to determine where each character begins and
         * ends, creating a lookup table for efficient character rendering.
         *
         * Font Image Requirements:
         * - Characters arranged horizontally in a single row
         * - First pixel must be white (0xFFFFFFFF) as a format marker
         * - Color changes between characters indicate boundaries
         * - Characters cover ASCII range from space (' ') to 'z'
         * - Image should contain exactly the expected number of characters
         *
         * Parsing Process:
         * The constructor scans the top row of pixels to detect color changes,
         * which indicate character boundaries. Each character's rectangular
         * region is calculated and stored for later use during text rendering.
         *
         * Error Handling:
         * If the font file is malformed (wrong format, missing characters, or
         * incorrect structure), the constructor will throw an error to indicate
         * the problem. This ensures that only valid fonts are used for rendering.
         *
         * The loaded font is immediately ready for text rendering operations
         * and will remain valid for the lifetime of the Font object.
         */
        Font(const std::filesystem::path& file_name);

        /**
         * \brief Render text string to a new texture for flexible use
         * \param text String of text to render using this font
         * \param color RGBA color value for the text (default: white)
         * \return Pointer to newly created texture containing the rendered text
         *
         * Creates a new texture containing the rendered text string, allowing
         * the text to be used like any other texture in the rendering system.
         * This approach enables text to be drawn with transformations, effects,
         * and blending modes just like sprite graphics.
         *
         * Rendering Process:
         * 1. Measures the total text dimensions to determine texture size
         * 2. Creates a new render target texture of the appropriate size
         * 3. Renders each character from the font texture to the target
         * 4. Returns the completed texture for use in rendering operations
         *
         * Text-to-Texture Benefits:
         * - Text can be cached and reused multiple times without re-rendering
         * - Supports transformation matrices for rotation, scaling, and positioning
         * - Enables text to be used in sprite batching systems
         * - Allows text to participate in post-processing effects
         * - Provides consistent text rendering across different rendering paths
         *
         * Color Support:
         * The color parameter tints the entire text string with the specified
         * RGBA value, allowing for colored text while maintaining the original
         * font character shapes and anti-aliasing.
         *
         * Memory Management:
         * The returned texture is managed by the TextureManager and will be
         * cleaned up automatically when no longer referenced. The caller
         * can use the texture immediately without manual memory management.
         */
        Texture* PrintToTexture(const std::string& text, unsigned int color = 0xFFFFFFFF);

    private:
        // TODO
    };
}
