/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Engine/Matrix.h"
#include "IRenderer2D.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include <array>
#include <vector>

/**
 *
 * basic idea - either buffer is full(reached max_quads) or user invoked endscene-> draw one time
 */
namespace CS200
{
	class BatchRenderer2D : public IRenderer2D
	{
	public:
		BatchRenderer2D(unsigned max_quads = 10'000);
		BatchRenderer2D(const BatchRenderer2D& other) = delete;
		BatchRenderer2D(BatchRenderer2D&& other) noexcept;
		BatchRenderer2D& operator=(const BatchRenderer2D& other) = delete;
		BatchRenderer2D& operator=(BatchRenderer2D&& other) noexcept;
		~BatchRenderer2D() override;

		void Init() override;
		void Shutdown() override;
		// void BeginScene(std::span<const float, 9> ndc_matrix) override;
		void BeginScene(const Math::TransformationMatrix& view_projection) override;
		void EndScene() override;
		void
			DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor, float depth) override;
		// void DrawQuad(std::span<const float, 9> transform, OpenGL::Handle texture, std::span<const float, 4> texture_coords_lbrt, std::span<const float, 4> tint_color) override;
		void DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, float depth) override;
		void DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, float depth) override;
		void DrawLine(const Math::TransformationMatrix& transform, Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width, float depth) override;
		void DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width, float depth) override;

	private:
		struct QuadVertex
		{
			float						 x = 0, y = 0;
			float						 s = 0, t = 0;
			std::array<unsigned char, 4> tint{};
			int							 textureIndex = 0;
			float						 depth;
		};

		std::vector<QuadVertex> vertexData{};
		OpenGL::BufferHandle	vertexBufferHandle{};

		OpenGL::CompiledShader texturingCombineShader{};

		OpenGL::VertexArrayHandle modelHandle{};

		// sdf
		struct SDFVertex
		{
			float						 x = 0, y = 0;					   // Layout 0: aWorldPosition
			float						 testPoint_s = 0, testPoint_t = 0; // Layout 1: aTestPoint
			std::array<unsigned char, 4> fillColor{};					   // Layout 2: aFillColor
			std::array<unsigned char, 4> lineColor{};					   // Layout 3: aLineColor
			float						 worldSize_x = 0, worldSize_y = 0; // Layout 4: aWorldSize
			float						 lineWidth = 0;					   // Layout 5: aLineWidth
			int							 shape	   = 0;					   // Layout 6: aShape (0=Circle, 1=Rect)
			float						 depth	   = 0;					   // Layout 7: aDepth
		};

		std::vector<SDFVertex>	  sdfVertexData{};
		OpenGL::BufferHandle	  sdfVertexBufferHandle{};
		OpenGL::CompiledShader	  sdfShader{};
		OpenGL::VertexArrayHandle sdfModelHandle{};
		SDFVertex*				  sdfVertexDataEnd = nullptr; // pointing where we are
		unsigned				  sdfIndexCount	   = 0;

		OpenGL::BufferHandle	   indexBufferHandle{};
		OpenGL::BufferHandle	   camera_uniform_buffer{};
		std::array<float, 12>	   camera_array{};
		Math::TransformationMatrix currentCameraMatrix{};
		// limit how much we're going to put into that vertex buffer
		unsigned				   maxVertices = 0;
		unsigned				   maxIndices  = 0;

		void updateCameraUniformValues(const Math::TransformationMatrix& view_projection);


		enum class SDFShape : uint8_t
		{
			Circle	  = 0,
			Rectangle = 1,
		};
		// void DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape);


		QuadVertex* vertexDataEnd = nullptr; // pointing where we are
		unsigned	indexCount	  = 0;

		// OpenGL::Handle theTexture = 0;
		std::vector<OpenGL::TextureHandle> textureSlots;
		size_t							   activeTextureSize = 0;

	private:
		void flush(); // when quad amount is reached to max_quad
		void startBatch();

		size_t draw_call = 0;
		size_t GetDrawCallCounter() override;

		size_t texture_call = 0;
		size_t GetDrawTextureCounter() override;
	};

}
