/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "DrawDepth.h"
#include "TextManager.h"

void TextManager::DrawText(const std::string& text, const Math::vec2& position, Fonts font, const Math::vec2& scale, CS200::RGBA color, float depth) const
{
	if (auto text_texture = fonts[font]->PrintToTexture(text, color); text_texture)
	{
		const auto transform = Math::TranslationMatrix(position) * Math::ScaleMatrix(scale);
		text_texture->Draw(transform, color, depth);
	}
}

Math::ivec2 TextManager::CalculateTextSize(const std::string& text, Fonts font) const
{
	if (auto text_texture = fonts[font]->PrintToTexture(text, 0xFFFFFFFF); text_texture)
	{
		return text_texture->GetSize();
	}
  return { 0, 0 };
}

void TextManager::Init()
{
	add_font("Assets/fonts/Font_Simple.png");
	add_font("Assets/fonts/Font_Outlined.png");
	add_font("Assets/fonts/Font_Kings.png");
}

void TextManager::add_font(const std::filesystem::path& file_name)
{
	fonts.push_back(std::make_unique<CS230::Font>(file_name));
}
