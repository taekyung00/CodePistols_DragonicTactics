/**
 * \file
 * \author Rudy Castan
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Image.hpp"

#include "Engine/Error.hpp"
#include "Engine/Path.hpp"

#include <stb_image.h>
#include <utility>

namespace CS200
{
    Image::Image(const std::filesystem::path& image_path, bool flip_vertical)
    {
        const std::filesystem::path image_path_ctor = assets::locate_asset(image_path);
        stbi_set_flip_vertically_on_load(flip_vertical);
        constexpr int num_channels       = 4;                                                                                                            // rgba
        int           files_num_channels = 0;                                                                                                            // to here
        image_data                       = stbi_load(image_path_ctor.string().c_str(), &dimensions.x, &dimensions.y, &files_num_channels, num_channels); // loading, use dynamic memory so we need free
        if (!image_data)
        {
            throw_error_message("Loading Fail ");
        }
    }

    Image::Image(Image&& temporary) noexcept : image_data{ temporary.image_data }, dimensions{ temporary.dimensions }
    {
        temporary.image_data = nullptr;
        temporary.dimensions = { 0, 0 };
    }

    Image& Image::operator=(Image&& temporary) noexcept
    {
        std::swap(image_data, temporary.image_data);
        std::swap(dimensions, temporary.dimensions);
        return *this;
    }

    Image::~Image()
    {
        if (image_data)
        {
            stbi_image_free(image_data);
        }
    }

    const RGBA* Image::data() const noexcept
    {
        return reinterpret_cast<const RGBA*>(image_data);
    }

    RGBA* Image::data() noexcept
    {
        return reinterpret_cast<RGBA*>(image_data);
    }

    Math::ivec2 Image::GetSize() const noexcept
    {
        return dimensions;
    }


    
}
