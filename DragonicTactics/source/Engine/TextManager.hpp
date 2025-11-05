/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par GAM200 Engine Porting
 * \copyright DigiPen Institute of Technology
 */
#include "Font.hpp"
#include "Fonts.h"
#include <memory>
#include <vector>

class TextManager
{
public:
    TextManager() = default;
    void Init();
    void DrawText(const std::string& text, const Math::vec2& position, Fonts font, const Math::vec2& scale = { 1.0, 1.0 }, CS200::RGBA color = CS200::WHITE) const;

private:
    // static CS230::Font* get_font(size_t);

    void add_font(const std::filesystem::path& file_name);

    std::vector<std::unique_ptr<CS230::Font>> fonts{};
};