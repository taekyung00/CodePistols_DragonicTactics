#include "TextManager.hpp"


void TextManager::DrawText(const std::string& text, const Math::vec2& position, Fonts font, const Math::vec2& scale, CS200::RGBA color) const
{
    if (auto text_texture = fonts[font]->PrintToTexture(text, color); text_texture)
    {
        const auto transform = Math::TranslationMatrix(position) * Math::ScaleMatrix(scale);
        text_texture->Draw(transform, color);
    }
}



void TextManager::Init()
{
    add_font("Assets/fonts/Font_Simple.png");
    add_font("Assets/fonts/Font_Outlined.png");
}

void TextManager::add_font(const std::filesystem::path& file_name)
{
    fonts.push_back(std::make_unique<CS230::Font>(file_name));
}
