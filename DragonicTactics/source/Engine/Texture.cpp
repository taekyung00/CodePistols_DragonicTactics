#include "pch.h"

/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Texture.h"

#include "CS200/IRenderer2D.h"
#include "CS200/Image.h"
#include "Engine.h"
#include "Matrix.h"
#include "OpenGL/GL.h"
#include "Window.h"

namespace CS230
{

  void Texture::Draw([[maybe_unused]] const Math::TransformationMatrix& display_matrix, unsigned int color)
  {

	Draw(display_matrix, { 0, 0 }, image_size, color);
  }

  void Texture::Draw(const Math::TransformationMatrix& display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size, unsigned int color)
  {
	CS200::IRenderer2D* renderer = Engine::GetTextureManager().GetRenderer2D();


	// OpenGL Texture: (0,0) Bottom-Left
	// Image Pixel: (0,0) Top-Left
	const double u_left	  = static_cast<double>(texel_position.x) / image_size.x;
	const double u_right  = static_cast<double>(texel_position.x + frame_size.x) / image_size.x;
	// V_top  = 1.0 - (y / height)
	// V_bottom  = 1.0 - ((y + h) / height)
	const double v_top	  = 1.0 - (static_cast<double>(texel_position.y) / image_size.y);
	const double v_bottom = 1.0 - (static_cast<double>(texel_position.y + frame_size.y) / image_size.y);

	const Math::vec2 texel_coord_bl = { u_left, v_bottom };
	const Math::vec2 texel_coord_tr = { u_right, v_top };

	Math::vec2 set_bottom_left{ frame_size.x * 0.5, frame_size.y * 0.5 };
	const auto world_transformation = display_matrix * Math::TranslationMatrix(set_bottom_left) * Math::ScaleMatrix(frame_size);

	renderer->DrawQuad(world_transformation, textureHandle, texel_coord_bl, texel_coord_tr, color);
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
	image_size		 = image.GetSize();
	textureHandle	 = OpenGL::CreateTextureFromImage(image, OpenGL::Filtering::NearestPixel, OpenGL::Wrapping::ClampToEdge);
  }

  Texture::Texture([[maybe_unused]] OpenGL::TextureHandle given_texture, [[maybe_unused]] Math::ivec2 the_size) : image_size{ the_size }, textureHandle{ given_texture }
  {
  }
}
