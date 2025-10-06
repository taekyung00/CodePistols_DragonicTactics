/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <array>
#include <cstdint>

namespace CS200
{
    /**
     * \brief Compact 32-bit color representation for efficient graphics operations
     *
     * RGBA provides a memory-efficient way to store and manipulate colors using a single
     * 32-bit integer. This format is widely used in graphics programming because it packs
     * four 8-bit color channels (Red, Green, Blue, Alpha) into one value that can be
     * easily passed around, stored in arrays, and processed by graphics hardware.
     *
     * The color format uses the layout: 0xRRGGBBAA where:
     * - RR = Red channel (bits 31-24)
     * - GG = Green channel (bits 23-16)
     * - BB = Blue channel (bits 15-8)
     * - AA = Alpha channel (bits 7-0)
     *
     * Each channel ranges from 0x00 (0) to 0xFF (255), providing 256 levels per channel
     * and over 16 million possible color combinations with transparency support.
     *
     * This compact representation is ideal for:
     * - Storing vertex colors in graphics buffers
     * - Passing colors as function parameters
     * - Performing fast bitwise color operations
     * - Interfacing with graphics APIs that expect packed colors
     */
    using RGBA = uint32_t;

    /** \brief Pure white color (255, 255, 255, 255) - fully opaque white */
    constexpr RGBA WHITE = 0xffffffff;

    /** \brief Pure black color (0, 0, 0, 255) - fully opaque black */
    constexpr RGBA BLACK = 0x000000ff;

    /** \brief Transparent color (0, 0, 0, 0) - completely transparent black */
    constexpr RGBA CLEAR = 0x00000000;

    /**
     * \brief Convert packed RGBA color to floating-point component array
     * \param rgba 32-bit packed color in RGBA format
     * \return Array of 4 floats [r, g, b, a] normalized to [0.0, 1.0] range
     *
     * Unpacks a compact RGBA color into separate floating-point components that
     * are commonly required by graphics APIs like OpenGL. Each 8-bit integer
     * channel (0-255) is converted to a normalized float (0.0-1.0).
     *
     * This conversion is essential for:
     * - Passing colors to OpenGL shader uniforms
     * - Performing floating-point color math operations
     * - Interfacing with APIs that expect normalized color values
     * - Blending and interpolation calculations
     *
     * The bit extraction isolates each color channel:
     * - Red: (rgba & 0xff000000) >> 24
     * - Green: (rgba & 0x00ff0000) >> 16
     * - Blue: (rgba & 0x0000ff00) >> 8
     * - Alpha: (rgba & 0x000000ff) >> 0
     *
     * Each extracted value is then divided by 255.0f to normalize to [0.0, 1.0].
     */
    constexpr std::array<float, 4> unpack_color(RGBA rgba) noexcept
    {
        constexpr float scale = 1.0f / 255.0f;
        return { static_cast<float>((rgba & 0xff000000) >> 24) * scale, static_cast<float>((rgba & 0x00ff0000) >> 16) * scale, static_cast<float>((rgba & 0x0000ff00) >> 8) * scale,
                 static_cast<float>((rgba & 0x000000ff) >> 0) * scale };
    }

    /**
     * \brief Convert RGBA format to ABGR format by reversing byte order
     * \param rgba Color in RGBA format (0xRRGGBBAA)
     * \return Color converted to ABGR format (0xAABBGGRR)
     *
     * Converts between different color byte ordering formats that are used by
     * various graphics systems and file formats. This conversion is necessary
     * when interfacing with systems that expect colors in ABGR order instead
     * of the standard RGBA order.
     *
     * The conversion process:
     * 1. Extract each color channel from the original RGBA value
     * 2. Rearrange the channels from RGBA order to ABGR order
     * 3. Pack the rearranged channels into a new 32-bit value
     *
     * Common use cases:
     * - Converting colors for specific graphics APIs or hardware
     * - Interfacing with image file formats that use different byte orders
     * - Preparing colors for systems with different endianness
     * - Working with legacy graphics formats that expect ABGR ordering
     *
     * The byte reversal ensures compatibility across different systems while
     * maintaining the same color information in a different memory layout.
     */
    constexpr uint32_t rgba_to_abgr(RGBA rgba) noexcept
    {
        const uint8_t r = static_cast<uint8_t>((rgba & 0xff000000) >> 24);
        const uint8_t g = static_cast<uint8_t>((rgba & 0x00ff0000) >> 16);
        const uint8_t b = static_cast<uint8_t>((rgba & 0x0000ff00) >> 8);
        const uint8_t a = static_cast<uint8_t>((rgba & 0x000000ff) >> 0);

        return (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(r) << 0);
    }


}
