

#pragma once
#include "CS200/BatchRenderer2D.h"
#include "CS200/IRenderer2D.h"
#include "CS200/ImmediateRenderer2D.h"
#include "CS200/InstancedRenderer2D.h"
#include "OpenGL/Framebuffer.h"
#include <filesystem>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CS230
{
  class Texture;

  class TextureManager
  {
public:
	enum class RendererType
	{
	  Immediate,
	  Batch,
	  Instanced
	};

	std::shared_ptr<Texture> Load(const std::filesystem::path& file_name);

	void							Init();
	void							Unload();
	static void						StartRenderTextureMode(int width, int height);
	static std::shared_ptr<Texture> EndRenderTextureMode();
	void							SwitchRenderer(RendererType type);
	RendererType					GetCurrentRendererType() const;
	static CS200::IRenderer2D*		GetRenderer2D();
	void							Shutdown();


private:
	RendererType									  current_renderer_type = RendererType::Instanced;
	inline static std::unique_ptr<CS200::IRenderer2D> renderer2D{};

	std::map<std::filesystem::path, std::shared_ptr<Texture>> textures;

	struct RenderInfo
	{
	  // RenderInfo() = default;
	  OpenGL::FramebufferWithColor Target{};
	  Math::ivec2				   Size{};
	  std::array<GLfloat, 4>	   ClearColor{};
	  std::array<GLint, 4>		   Viewport{};
	};

	// inline static RenderInfo render_info{};

	static RenderInfo& get_render_info()
	{
	  static RenderInfo instance;
	  return instance;
	}
  };
}
