/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "BatchRenderer2D.h"
#include "pch.h"

#include "Engine/Path.h"
#include "OpenGL/Buffer.h"
#include "OpenGL/GL.h"
#include "OpenGL/VertexArray.h"
#include "Renderer2DUtils.h"
#include <fstream>
#include <numeric>
#include <sstream>

namespace CS200
{
  BatchRenderer2D::BatchRenderer2D(unsigned max_quads)
  {
	maxVertices = max_quads * 4; // each quad have 4 vertices
	maxIndices	= max_quads * 6;
	vertexData.resize(maxVertices);
	sdfVertexData.resize(maxVertices);

	// wait until other stuffs are ready.. ->Init
  }

  BatchRenderer2D::BatchRenderer2D(BatchRenderer2D&& other) noexcept
	  : vertexData(std::move(other.vertexData)), vertexBufferHandle(other.vertexBufferHandle), texturingCombineShader(std::move(other.texturingCombineShader)), modelHandle(other.modelHandle),
		sdfVertexData(std::move(other.sdfVertexData)), sdfVertexBufferHandle(other.sdfVertexBufferHandle), sdfShader(std::move(other.sdfShader)), sdfModelHandle(other.sdfModelHandle),
		sdfVertexDataEnd(other.sdfVertexDataEnd), sdfIndexCount(other.sdfIndexCount), indexBufferHandle(other.indexBufferHandle), camera_uniform_buffer(other.camera_uniform_buffer),
		camera_array(other.camera_array), currentCameraMatrix(other.currentCameraMatrix), maxVertices(other.maxVertices), maxIndices(other.maxIndices), vertexDataEnd(other.vertexDataEnd),
		indexCount(other.indexCount), textureSlots(std::move(other.textureSlots)), activeTextureSize(other.activeTextureSize), draw_call(other.draw_call), texture_call(other.texture_call)
  {
	other.vertexBufferHandle	 = 0;
	other.modelHandle			 = 0;
	other.sdfVertexBufferHandle	 = 0;
	other.sdfModelHandle		 = 0;
	other.indexBufferHandle		 = 0;
	other.camera_uniform_buffer	 = 0;
	other.texturingCombineShader = {};
	other.sdfShader				 = {};
	other.vertexDataEnd			 = nullptr;
	other.indexCount			 = 0;
	other.sdfVertexDataEnd		 = nullptr;
	other.sdfIndexCount			 = 0;
	other.activeTextureSize		 = 0;
	other.draw_call				 = 0;
	other.texture_call			 = 0;
  }

  BatchRenderer2D& BatchRenderer2D::operator=(BatchRenderer2D&& other) noexcept
  {
	std::swap(vertexData, other.vertexData);
	std::swap(vertexBufferHandle, other.vertexBufferHandle);
	std::swap(indexBufferHandle, other.indexBufferHandle);
	std::swap(modelHandle, other.modelHandle);
	std::swap(texturingCombineShader, other.texturingCombineShader);
	std::swap(currentCameraMatrix, other.currentCameraMatrix);

	std::swap(sdfVertexData, other.sdfVertexData);
	std::swap(sdfVertexBufferHandle, other.sdfVertexBufferHandle);
	std::swap(sdfShader, other.sdfShader);
	std::swap(sdfModelHandle, other.sdfModelHandle);
	std::swap(sdfVertexDataEnd, other.sdfVertexDataEnd);
	std::swap(sdfIndexCount, other.sdfIndexCount);

	std::swap(camera_uniform_buffer, other.camera_uniform_buffer);
	std::swap(camera_array, other.camera_array);
	std::swap(draw_call, other.draw_call);
	std::swap(texture_call, other.texture_call);

	std::swap(maxVertices, other.maxVertices);
	std::swap(maxIndices, other.maxIndices);
	std::swap(vertexDataEnd, other.vertexDataEnd);
	std::swap(indexCount, other.indexCount);
	std::swap(textureSlots, other.textureSlots);
	std::swap(activeTextureSize, other.activeTextureSize);
	return *this;
  }

  BatchRenderer2D::~BatchRenderer2D()
  {
	Shutdown();
  }

  void BatchRenderer2D::Init()
  {
	// get how many texture opengl can draw
	GLint max_tex_units = 0;
	GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex_units); // check with docs.gl to get minimum(16) and maximum
	textureSlots.resize(static_cast<size_t>(std::min(max_tex_units, 64)));

	// load shaders with parsing
	const std::filesystem::path vertex_file = assets::locate_asset("Assets/shaders/BatchRenderer2D/quad.vert");
	std::ifstream				vert_stream(vertex_file);
	std::stringstream			vert_text_stream;
	vert_text_stream << vert_stream.rdbuf();
	const std::string vertex_glsl = vert_text_stream.str();


	const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/BatchRenderer2D/quad.frag");
	std::ifstream				frag_stream(fragment_file);
	std::stringstream			frag_text_stream;
	frag_text_stream << frag_stream.rdbuf();
	std::string		  frag_glsl		= frag_text_stream.str();
	const size_t	  first_newline = frag_glsl.find('\n');
	const std::string define_line	= "\n#define MAX_TEXTURE_SLOTS " + std::to_string(textureSlots.size());
	frag_glsl.insert(first_newline, define_line);

	texturingCombineShader = OpenGL::CreateShader(std::string_view{ vertex_glsl }, std::string_view{ frag_glsl });


	// have to set their binding index
	GL::UseProgram(texturingCombineShader.Shader);

	std::vector<int> sampler_binding_values(textureSlots.size());
	std::iota(sampler_binding_values.begin(), sampler_binding_values.end(), 0);
	const GLint location = GL::GetUniformLocation(texturingCombineShader.Shader, "uTextures");
	GL::Uniform1iv(location, static_cast<GLsizei>(textureSlots.size()), sampler_binding_values.data());

	GL::UseProgram(0);

	// create vertex array object, buffer vertices, buffer indices
	vertexBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, static_cast<GLsizeiptr>(sizeof(QuadVertex) * maxVertices));


	// setup index buffer

	// unlike vertex buffer that gonna change every frame, index don't change
	// i.e. 0 1 2 2 3 0 ... << this pattern repeat
	// but just amount of index gonna change(how many do we need?)
	std::vector<unsigned> indice_values(maxIndices);
	unsigned			  offset = 0;
	for (unsigned i = 0; i < maxIndices; i += 6)
	{
	  indice_values[i + 0] = offset + 0;
	  indice_values[i + 1] = offset + 1;
	  indice_values[i + 2] = offset + 2;
	  indice_values[i + 3] = offset + 2;
	  indice_values[i + 4] = offset + 3;
	  indice_values[i + 5] = offset + 0;
	  offset += 4;
	}

	indexBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indice_values }));

	// Create vertex array object
	const auto layout = {
	  OpenGL::VertexBuffer{ vertexBufferHandle, { OpenGL::Attribute::Float2, OpenGL::Attribute::Float2, OpenGL::Attribute::UByte4ToNormalized, OpenGL::Attribute::Int, OpenGL::Attribute::Float } }
	};
	modelHandle = OpenGL::CreateVertexArrayObject(layout, indexBufferHandle);


	// SDF
	//  create vertex array object, buffer vertices, buffer indices
	sdfShader = OpenGL::CreateShader(assets::locate_asset("Assets/shaders/BatchRenderer2D/sdf.vert"), assets::locate_asset("Assets/shaders/BatchRenderer2D/sdf.frag"));

	sdfVertexBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, static_cast<GLsizeiptr>(sizeof(SDFVertex) * maxVertices));
	const auto sdfLayout  = {
	   OpenGL::VertexBuffer{ sdfVertexBufferHandle,
							 {
							   OpenGL::Attribute::Float2,			  // aWorldPosition
							   OpenGL::Attribute::Float2,			  // aTestPoint
							   OpenGL::Attribute::UByte4ToNormalized, // aFillColor
							   OpenGL::Attribute::UByte4ToNormalized, // aLineColor
							   OpenGL::Attribute::Float2,			  // aWorldSize
							   OpenGL::Attribute::Float,			  // aLineWidth
							   OpenGL::Attribute::Int,				  // aShape
							   OpenGL::Attribute::Float				  // aDepth
							 } }
	};
	sdfModelHandle = OpenGL::CreateVertexArrayObject(sdfLayout, indexBufferHandle);

	//- Create uniform buffer for camera/view-projection matrix
	camera_uniform_buffer = OpenGL::CreateBuffer(OpenGL::BufferType::UniformBlocks, sizeof(camera_array));

	OpenGL::BindUniformBufferToShader(texturingCombineShader.Shader, 0, camera_uniform_buffer, "NDC");
	OpenGL::BindUniformBufferToShader(sdfShader.Shader, 0, camera_uniform_buffer, "NDC");
  }

  void BatchRenderer2D::Shutdown()
  {
	OpenGL::DestroyShader(texturingCombineShader);
	OpenGL::DestroyShader(sdfShader);

	GL::DeleteBuffers(1, &vertexBufferHandle), vertexBufferHandle		= 0;
	GL::DeleteBuffers(1, &indexBufferHandle), indexBufferHandle			= 0;
	GL::DeleteBuffers(1, &sdfVertexBufferHandle), sdfVertexBufferHandle = 0;

	GL::DeleteBuffers(1, &camera_uniform_buffer), camera_uniform_buffer = 0;

	GL::DeleteVertexArrays(1, &modelHandle), modelHandle	   = 0;
	GL::DeleteVertexArrays(1, &sdfModelHandle), sdfModelHandle = 0;
  }

  void BatchRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
  {
	//- Store matrix for potential later use
	currentCameraMatrix = view_projection;

	//- Convert 3x3 matrix to 4x3 format for uniform buffer
	updateCameraUniformValues(currentCameraMatrix);

	//- Update uniform buffer with new matrix data
	OpenGL::UpdateBufferData(OpenGL::BufferType::UniformBlocks, camera_uniform_buffer, std::as_bytes(std::span{ camera_array }));

	//- Bind uniform buffer for use by shaders
	GL::BindBuffer(GL_UNIFORM_BUFFER, camera_uniform_buffer);

	draw_call	 = 0;
	texture_call = 0;
	startBatch();
  }

  void BatchRenderer2D::EndScene()
  {
	flush();
  }

  void
	BatchRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor, float depth)
  {
	if (sdfIndexCount + 6 > maxIndices)
	{
	  flush();
	}

	if (indexCount + 6 > maxIndices)
	{
	  flush();
	}

	int	 tex_index = 0;
	bool found	   = false;
	for (size_t i = 0; i < activeTextureSize; ++i)
	{
	  if (textureSlots[i] == texture)
	  {
		found	  = true;
		tex_index = static_cast<int>(i);
	  }
	}

	if (!found)
	{
	  if (activeTextureSize >= textureSlots.size())
	  {
		flush();
	  }
	  tex_index						  = static_cast<int>(activeTextureSize);
	  textureSlots[activeTextureSize] = texture;
	  ++activeTextureSize;
	}

	// Convert texture_coords_lbrt (left, bottom, right, top) to texture coordinate transform matrix
	const float left   = static_cast<float>(texture_coord_bl.x);
	const float bottom = static_cast<float>(texture_coord_bl.y);
	const float right  = static_cast<float>(texture_coord_tr.x);
	const float top	   = static_cast<float>(texture_coord_tr.y);

	const std::array<float, 2> texture_coords[4] = {
	  {	left, bottom }, //  bottom left
	  { right, bottom }, //  bottom right
	  { right,	   top }, //  top right
	  {	left,	  top }	 //  top left
	};

	// we don't have to make texcoord_transform matrix, just use 4 texture coords right away!

	// const std::array<unsigned char, 4> tint = pack_color(tint_color);

	constexpr std::array<float, 2> model_positions[4] = {
	  { -0.5, -0.5 }, //  bottom left
	  { +0.5, -0.5 }, //  bottom right
	  { +0.5, +0.5 }, //  top right
	  { -0.5, +0.5 }  //  top left
	};


	for (unsigned i = 0; i < 4; ++i) // i is for 4 vertex(bottom/top - right/left)
	{
	  // matrix multiply manually (3by 3, transform matrix) * (3 by 1, position matrix) => model to world!
	  const float x = static_cast<float>(static_cast<double>(model_positions[i][0]) * transform[0][0] + static_cast<double>(model_positions[i][1]) * transform[0][1] + transform[0][2]);
	  const float y = static_cast<float>(static_cast<double>(model_positions[i][0]) * transform[1][0] + static_cast<double>(model_positions[i][1]) * transform[1][1] + transform[1][2]);

	  vertexDataEnd->x			  = x;
	  vertexDataEnd->y			  = y;
	  vertexDataEnd->s			  = texture_coords[i][0];
	  vertexDataEnd->t			  = texture_coords[i][1];
	  vertexDataEnd->tint		  = ColorArray(tintColor);
	  vertexDataEnd->textureIndex = tex_index;
	  vertexDataEnd->depth		  = depth;
	  ++vertexDataEnd;
	}
	indexCount += 6;

	++texture_call;
  }

  void BatchRenderer2D::DrawCircle(
	[[maybe_unused]] const Math::TransformationMatrix& transform, [[maybe_unused]] CS200::RGBA fill_color, [[maybe_unused]] CS200::RGBA line_color, [[maybe_unused]] double line_width, float depth)
  {
	if (indexCount + 6 > maxIndices)
	{
	  flush();
	}
	if (sdfIndexCount + 6 > maxIndices)
	{
	  flush();
	}

	const auto sdf_transform = Renderer2DUtils::CalculateSDFTransform(transform, line_width);
	const auto fill_bytes	 = ColorArray(fill_color);
	const auto line_bytes	 = ColorArray(line_color);

	constexpr std::array<float, 2> model_positions[4] = {
	  { -0.5, -0.5 }, //  bottom left
	  { +0.5, -0.5 }, //  bottom right
	  { +0.5, +0.5 }, //  top right
	  { -0.5, +0.5 }  //  top left
	};

	for (unsigned i = 0; i < 4; ++i)
	{
	  const float x = model_positions[i][0] * sdf_transform.QuadTransform[0] + model_positions[i][1] * sdf_transform.QuadTransform[3] + sdf_transform.QuadTransform[6];
	  const float y = model_positions[i][0] * sdf_transform.QuadTransform[1] + model_positions[i][1] * sdf_transform.QuadTransform[4] + sdf_transform.QuadTransform[7];

	  const float s = model_positions[i][0] * sdf_transform.QuadSize[0];
	  const float t = model_positions[i][1] * sdf_transform.QuadSize[1];

	  sdfVertexDataEnd->x			= x;
	  sdfVertexDataEnd->y			= y;
	  sdfVertexDataEnd->testPoint_s = s;
	  sdfVertexDataEnd->testPoint_t = t;
	  sdfVertexDataEnd->fillColor	= fill_bytes;
	  sdfVertexDataEnd->lineColor	= line_bytes;
	  sdfVertexDataEnd->worldSize_x = sdf_transform.WorldSize[0];
	  sdfVertexDataEnd->worldSize_y = sdf_transform.WorldSize[1];
	  sdfVertexDataEnd->lineWidth	= static_cast<float>(line_width);
	  sdfVertexDataEnd->shape		= static_cast<int>(SDFShape::Circle); // 0
	  sdfVertexDataEnd->depth		= depth;

	  ++sdfVertexDataEnd;
	}
	sdfIndexCount += 6;

	++texture_call;
  }

  void BatchRenderer2D::DrawRectangle(
	[[maybe_unused]] const Math::TransformationMatrix& transform, [[maybe_unused]] CS200::RGBA fill_color, [[maybe_unused]] CS200::RGBA line_color, [[maybe_unused]] double line_width, float depth)
  {
	if (indexCount + 6 > maxIndices)
	{
	  flush();
	}
	if (sdfIndexCount + 6 > maxIndices)
	{
	  flush();
	}
	const auto sdf_transform = Renderer2DUtils::CalculateSDFTransform(transform, line_width);
	const auto fill_bytes	 = ColorArray(fill_color);
	const auto line_bytes	 = ColorArray(line_color);

	constexpr std::array<float, 2> model_positions[4] = {
	  { -0.5, -0.5 }, //  bottom left
	  { +0.5, -0.5 }, //  bottom right
	  { +0.5, +0.5 }, //  top right
	  { -0.5, +0.5 }  //  top left
	};

	for (unsigned i = 0; i < 4; ++i)
	{
	  const float x = model_positions[i][0] * sdf_transform.QuadTransform[0] + model_positions[i][1] * sdf_transform.QuadTransform[3] + sdf_transform.QuadTransform[6];
	  const float y = model_positions[i][0] * sdf_transform.QuadTransform[1] + model_positions[i][1] * sdf_transform.QuadTransform[4] + sdf_transform.QuadTransform[7];
	  const float s = model_positions[i][0] * sdf_transform.QuadSize[0];
	  const float t = model_positions[i][1] * sdf_transform.QuadSize[1];

	  sdfVertexDataEnd->x			= x;
	  sdfVertexDataEnd->y			= y;
	  sdfVertexDataEnd->testPoint_s = s;
	  sdfVertexDataEnd->testPoint_t = t;
	  sdfVertexDataEnd->fillColor	= fill_bytes;
	  sdfVertexDataEnd->lineColor	= line_bytes;
	  sdfVertexDataEnd->worldSize_x = sdf_transform.WorldSize[0];
	  sdfVertexDataEnd->worldSize_y = sdf_transform.WorldSize[1];
	  sdfVertexDataEnd->lineWidth	= static_cast<float>(line_width);
	  sdfVertexDataEnd->shape		= static_cast<int>(SDFShape::Rectangle); // 1
	  sdfVertexDataEnd->depth		= depth;

	  ++sdfVertexDataEnd;
	}
	sdfIndexCount += 6;

	++texture_call;
  }

  void BatchRenderer2D::DrawLine(
	[[maybe_unused]] const Math::TransformationMatrix& transform, [[maybe_unused]] Math::vec2 start_point, [[maybe_unused]] Math::vec2 end_point, [[maybe_unused]] CS200::RGBA line_color,
	[[maybe_unused]] double line_width, float depth)
  {
	const auto line_transform = Renderer2DUtils::CalculateLineTransform(transform, start_point, end_point, line_width);
	DrawRectangle(line_transform, line_color, line_color, line_width, depth);
  }

  void
	BatchRenderer2D::DrawLine([[maybe_unused]] Math::vec2 start_point, [[maybe_unused]] Math::vec2 end_point, [[maybe_unused]] CS200::RGBA line_color, [[maybe_unused]] double line_width, float depth)
  {
	DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width, depth);
  }

  void BatchRenderer2D::startBatch()
  {
	vertexDataEnd	  = vertexData.data();
	indexCount		  = 0;
	activeTextureSize = 0;

	sdfVertexDataEnd = sdfVertexData.data();
	sdfIndexCount	 = 0;
  }

  void BatchRenderer2D::flush()
  {
	if (indexCount > 0)
	{
	  // upload our vertices(vertex buffer is dynamic)
	  const ptrdiff_t				   vertex_count	 = vertexDataEnd - vertexData.data();
	  const std::span				   data_span	 = std::span{ vertexData.data(), static_cast<size_t>(vertex_count) };
	  const std::span<const std::byte> bytes_to_send = std::as_bytes(data_span);

	  GL::BindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	  GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(QuadVertex) * maxVertices), nullptr, GL_DYNAMIC_DRAW); // orphaning

	  OpenGL::UpdateBufferData(OpenGL::BufferType::Vertices, vertexBufferHandle, bytes_to_send);


	  // select our texture
	  for (size_t i = 0; i < activeTextureSize; ++i)
	  {
		GL::ActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
		GL::BindTexture(GL_TEXTURE_2D, textureSlots[i]);
	  }

	  // draw
	  GL::UseProgram(texturingCombineShader.Shader);
	  GL::BindVertexArray(modelHandle);
	  GL::DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
	  ++draw_call;
	}

	if (sdfIndexCount > 0)
	{
	  const ptrdiff_t				   sdf_vertex_count_ptrdiff = sdfVertexDataEnd - sdfVertexData.data();
	  const std::span				   sdf_data_span			= std::span{ sdfVertexData.data(), static_cast<size_t>(sdf_vertex_count_ptrdiff) };
	  const std::span<const std::byte> sdf_bytes_to_send		= std::as_bytes(sdf_data_span);

	  GL::BindBuffer(GL_ARRAY_BUFFER, sdfVertexBufferHandle);
	  GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(SDFVertex) * maxVertices), nullptr, GL_DYNAMIC_DRAW); // orphaning
	  OpenGL::UpdateBufferData(OpenGL::BufferType::Vertices, sdfVertexBufferHandle, sdf_bytes_to_send);

	  GL::UseProgram(sdfShader.Shader);
	  GL::BindVertexArray(sdfModelHandle);
	  GL::DrawElements(GL_TRIANGLES, static_cast<GLsizei>(sdfIndexCount), GL_UNSIGNED_INT, nullptr);
	  ++draw_call;
	}


	// unbind stuff
	GL::BindVertexArray(0);
	GL::UseProgram(0);
	GL::BindTexture(GL_TEXTURE_2D, 0);
	GL::BindBuffer(GL_ARRAY_BUFFER, 0);

	startBatch(); // reset
  }

  void BatchRenderer2D::updateCameraUniformValues(const Math::TransformationMatrix& view_projection)
  {
	const auto as_3x3 = Renderer2DUtils::to_opengl_mat3(view_projection);
	for (std::size_t col = 0; col < 3; ++col)
	{
	  const std::size_t src_offset = col * 3;
	  const std::size_t dst_offset = col * 4;

	  camera_array[dst_offset + 0] = as_3x3[src_offset + 0]; // how?****
	  camera_array[dst_offset + 1] = as_3x3[src_offset + 1];
	  camera_array[dst_offset + 2] = as_3x3[src_offset + 2];
	  camera_array[dst_offset + 3] = 0.0f;
	}
  }

  size_t BatchRenderer2D::GetDrawCallCounter()
  {
	return draw_call;
  }

  size_t BatchRenderer2D::GetDrawTextureCounter()
  {
	return texture_call;
  }

} // namespace CS200
