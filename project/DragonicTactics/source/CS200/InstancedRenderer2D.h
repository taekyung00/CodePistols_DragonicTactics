/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "IRenderer2D.h"

#include "Engine/Matrix.h"

#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include <array>
#include <vector>

/**
 one model,
 lots of instances
 sharing buffer static -> positions(because we use single quad!!)
and each instance has their own dynamic buffer

 ->color
 ->texture
 ->model xform
 ->texcoords xform
 */


namespace CS200
{
  class InstancedRenderer2D : public IRenderer2D
  {
public:
	InstancedRenderer2D(unsigned max_sprites = 10'000); // means max_instances
	InstancedRenderer2D(const InstancedRenderer2D& other) = delete;
	InstancedRenderer2D(InstancedRenderer2D&& other) noexcept;
	InstancedRenderer2D& operator=(const InstancedRenderer2D& other) = delete;
	InstancedRenderer2D& operator=(InstancedRenderer2D&& other) noexcept;
	~InstancedRenderer2D() override;

	void Init() override;
	void Shutdown() override;
	void BeginScene(const Math::TransformationMatrix& view_projection) override;
	void EndScene() override;
	void DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor, float depth) override;
	// void DrawQuad(std::span<const float, 9> transform, OpenGL::Handle texture, std::span<const float, 4> texture_coords_lbrt, std::span<const float, 4> tint_color) override;

	void DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, float depth) override;
	void DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, float depth) override;
	void DrawLine(const Math::TransformationMatrix& transform, Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width, float depth) override;
	void DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width, float depth) override;

private:
	struct QuadInstance // maybe we can make more compact? bit width, ...
	{
	  /*float x = 0, y = 0;*/						   // don't need for each instance anymore!!
	  float						   transformrow0[3]{}; // instead having vertex for each instance, we have transform mat for each instance!
	  float						   transformrow1[3]{};
	  std::array<unsigned char, 4> tint{};		  // caution !!! to use createvao helper func, make order same as in shader's attrib!!!!!
	  /*float s = 0, t = 0;*/					  // don't need for each instance anymore!!
	  float						   texScale[2]{}; // instead having texcoord for each instance, we have transform mat of texcoord for each instance with compacted version
	  float						   texOffset[2]{};
	  int						   textureIndex = 0;
	  float						   depth		= 0.f;
	};

	std::vector<QuadInstance> instanceData{};
	OpenGL::CompiledShader	  texturingCombineShader;
	OpenGL::BufferHandle	  fixedVertexBufferHandle{};
	OpenGL::BufferHandle	  instanceBufferHandle{};
	OpenGL::VertexArrayHandle modelHandle{};

	// sdf
	struct SDFInstance
	{
	  // float						 x = 0, y = 0;
	  float						   transformrow0[3]{};				 // Layout 1: aModelRow0
	  float						   transformrow1[3]{};				 // Layout 2: aModelRow1
	  std::array<unsigned char, 4> fillColor{};						 // Layout 3: aFillColor
	  std::array<unsigned char, 4> lineColor{};						 // Layout 4: aLineColor
	  float						   worldSize_x = 0, worldSize_y = 0; // Layout 5: aWorldSize
	  float						   lineWidth = 0;					 // Layout 6: aLineWidth
	  int						   shape	 = 0;					 // Layout 7: aShape (0=Circle, 1=Rect)
	  float						   depth	 = 0.f;					 // Layout 8: aDepth
	};

	OpenGL::BufferHandle	  sdfFixedVertexBufferHandle{};
	OpenGL::BufferHandle	  sdfInstanceBufferHandle{};
	std::vector<SDFInstance>  sdfInstanceData{};
	OpenGL::CompiledShader	  sdfShader{};
	OpenGL::VertexArrayHandle sdfModelHandle{};

	unsigned maxSDFInstances = 0;

	OpenGL::BufferHandle indexBufferHandle{};

	enum class SDFShape : uint8_t
	{
	  Circle	= 0,
	  Rectangle = 1,
	};


	// ndc block

	OpenGL::BufferHandle camera_uniform_buffer{};

	std::array<float, 12> camera_array{};

	Math::TransformationMatrix currentCameraMatrix{};


	unsigned maxInstances = 0;


	std::vector<OpenGL::TextureHandle> textureSlots;

	size_t activeTextureSize = 0;


private:

	void updateCameraUniformValues(const Math::TransformationMatrix& view_projection);

	void flush(); // when quad amount is reached to max_quad

	void startBatch();

	size_t draw_call;
	size_t GetDrawCallCounter() override;

	size_t texture_call = 0;
	size_t GetDrawTextureCounter() override;
  };

}
