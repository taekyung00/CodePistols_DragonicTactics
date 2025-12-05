/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "CS200/IRenderer2D.h"
#include "CS200/NDC.h"
#include "Engine.h"
#include "Logger.h"
#include "OpenGL/GL.h"
#include "Path.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Window.h"

namespace CS230
{
  std::shared_ptr<Texture> TextureManager::Load(const std::filesystem::path& file_name)
  {
	const std::filesystem::path file_path = assets::locate_asset(file_name);
	if (textures.find(file_path) == textures.end())
	{
	  // textures[file_name] = new Texture(file_name);
	  textures[file_path] = std::shared_ptr<Texture>(new Texture(file_path));

	  Engine::GetLogger().LogEvent("Loading Texture: " + file_path.string());
	}
	return textures[file_path];
  }

  void TextureManager::Init()
  {
	current_renderer_type = RendererType::Immediate;
	// Create and initialize new renderer
	switch (current_renderer_type)
	{
	  case RendererType::Immediate: renderer2D = std::make_unique<CS200::ImmediateRenderer2D>(); break;
	  case RendererType::Batch: renderer2D = std::make_unique<CS200::BatchRenderer2D>(); break;
	  case RendererType::Instanced: renderer2D = std::make_unique<CS200::InstancedRenderer2D>(); break;
	  default: renderer2D = std::make_unique<CS200::ImmediateRenderer2D>(); break;
	}

	if (renderer2D)
	{
	  renderer2D->Init();
	}
	else
	{
	  throw std::runtime_error("renderer initialize failed!");
	}
  }

  void TextureManager::Unload()
  {
	for (std::pair<std::filesystem::path, std::shared_ptr<Texture>> texture : textures)
	{
	  // delete texture.second;
	  Engine::GetLogger().LogEvent("Unload Texture: " + texture.first.string());
	}
	textures.clear();
  }

  void TextureManager::StartRenderTextureMode([[maybe_unused]] int width, [[maybe_unused]] int height)
  {
	// auto& renderer_2d = Engine::GetRenderer2D();
	auto&				render_info = get_render_info();
	//  * - Ends current 2D renderer scene to ensure clean state transition
	CS200::IRenderer2D* renderer_2d = GetRenderer2D();
	renderer_2d->EndScene();

	//  * - Creates OpenGL framebuffer with color attachment of specified dimensions
	render_info.Size   = { width, height };
	render_info.Target = OpenGL::CreateFramebufferWithColor(Math::ivec2{ width, height });

	//  * - Saves current viewport, clear color, and rendering state for restoration
	GL::GetFloatv(GL_COLOR_CLEAR_VALUE, render_info.ClearColor.data());
	GL::GetIntegerv(GL_VIEWPORT, render_info.Viewport.data());

	//  * - Sets up Y-flipped coordinate system for proper texture orientation
	const auto ndc_matrix = Math::ScaleMatrix({ 1.0, -1.0 }) * CS200::build_ndc_matrix(render_info.Size);
	renderer_2d->BeginScene(ndc_matrix);

	//  * - Binds framebuffer as render target, replacing screen rendering
	GL::BindFramebuffer(GL_FRAMEBUFFER, render_info.Target.Framebuffer);
	GL::Viewport(0, 0, render_info.Size.x, render_info.Size.y);

	//  * - Clears render target with transparent black (0,0,0,0) for clean start
	GL::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	GL::Clear(GL_COLOR_BUFFER_BIT);
  }

  std::shared_ptr<Texture> TextureManager::EndRenderTextureMode()
  {
	CS200::IRenderer2D* renderer_2d = GetRenderer2D();
	auto&				render_info = get_render_info();
	// * Cleanup and Restoration Process:
	//  * - Ends current 2D renderer scene to flush any pending draw operations
	renderer_2d->EndScene();
	//  * - Unbinds framebuffer (returns to default screen framebuffer 0)
	GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
	//  * - Restores original viewport dimensions from saved state
	GL::Viewport(render_info.Viewport[0], render_info.Viewport[1], render_info.Viewport[2], render_info.Viewport[3]);
	//  * - Restores original clear color values from saved state
	GL::ClearColor(render_info.ClearColor[0], render_info.ClearColor[1], render_info.ClearColor[2], render_info.ClearColor[3]);
	//  * - Begins new 2D renderer scene with screen-appropriate coordinate system
	renderer_2d->BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
	//  * - Deletes temporary framebuffer to free GPU resources
	auto framebuffer_to_delete = render_info.Target.Framebuffer;
	GL::DeleteFramebuffers(1, &framebuffer_to_delete);


	//          * Texture Creation:
	//  * Creates a new Texture object by wrapping the framebuffer's color attachment:
	auto scene_texture				   = new Texture(render_info.Target.ColorAttachment, render_info.Size);
	//  * - Transfers ownership of OpenGL texture ID from framebuffer to Texture object
	render_info.Target.ColorAttachment = 0; // old one
	//  * - Preserves original dimensions specified in StartRenderTextureMode()
	//  * - Maintains RGBA format with alpha channel for transparency support
	//  * - Content includes all drawing operations performed during render-to-texture mode
	return std::shared_ptr<Texture>(scene_texture);
  }

  void TextureManager::SwitchRenderer(RendererType type)
  {
	if (current_renderer_type == type)
	  return; // Already using this renderer

	// Shutdown current renderer
	if (renderer2D)
	{
	  renderer2D->Shutdown();
	  renderer2D.reset();
	}

	// Create and initialize new renderer
	current_renderer_type = type;
	switch (type)
	{
	  case RendererType::Immediate: renderer2D = std::make_unique<CS200::ImmediateRenderer2D>(); break;
	  case RendererType::Batch: renderer2D = std::make_unique<CS200::BatchRenderer2D>(); break;
	  case RendererType::Instanced: renderer2D = std::make_unique<CS200::InstancedRenderer2D>(); break;
	  default: renderer2D = std::make_unique<CS200::ImmediateRenderer2D>(); break;
	}

	if (renderer2D)
	{
	  renderer2D->Init();
	}
  }

  TextureManager::RendererType TextureManager::GetCurrentRendererType() const
  {
	return current_renderer_type;
  }

  CS200::IRenderer2D* TextureManager::GetRenderer2D()
  {
	return renderer2D.get();
  }

  void TextureManager::Shutdown()
  {
	renderer2D->Shutdown();
	renderer2D.reset();
  }
}
