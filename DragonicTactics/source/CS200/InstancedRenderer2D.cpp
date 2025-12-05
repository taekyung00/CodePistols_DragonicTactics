/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "InstancedRenderer2D.h"
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

  InstancedRenderer2D::InstancedRenderer2D([[maybe_unused]] unsigned max_sprites)
  {
	maxInstances	= max_sprites;
	maxSDFInstances = max_sprites;
	instanceData.reserve(maxInstances);
	sdfInstanceData.reserve(maxSDFInstances);
  }

  InstancedRenderer2D::InstancedRenderer2D(InstancedRenderer2D&& other) noexcept
	  : instanceData(std::move(other.instanceData)), texturingCombineShader(std::move(other.texturingCombineShader)), fixedVertexBufferHandle(other.fixedVertexBufferHandle),
		instanceBufferHandle(other.instanceBufferHandle), modelHandle(other.modelHandle), sdfFixedVertexBufferHandle(other.sdfFixedVertexBufferHandle),
		sdfInstanceBufferHandle(other.sdfInstanceBufferHandle), sdfInstanceData(std::move(other.sdfInstanceData)), sdfShader(std::move(other.sdfShader)), sdfModelHandle(other.sdfModelHandle),
		maxSDFInstances(other.maxSDFInstances), indexBufferHandle(other.indexBufferHandle), camera_uniform_buffer(other.camera_uniform_buffer), camera_array(other.camera_array),
		currentCameraMatrix(other.currentCameraMatrix), maxInstances(other.maxInstances), textureSlots(std::move(other.textureSlots)), activeTextureSize(other.activeTextureSize),
		draw_call(other.draw_call), texture_call(other.texture_call)
  {
	other.fixedVertexBufferHandle	 = 0;
	other.instanceBufferHandle		 = 0;
	other.modelHandle				 = 0;
	other.sdfFixedVertexBufferHandle = 0;
	other.sdfInstanceBufferHandle	 = 0;
	other.sdfModelHandle			 = 0;
	other.indexBufferHandle			 = 0;
	other.camera_uniform_buffer		 = 0;

	other.texturingCombineShader = {};
	other.sdfShader				 = {};

	other.maxInstances		= 0;
	other.maxSDFInstances	= 0;
	other.activeTextureSize = 0;
	other.draw_call			= 0;
	other.texture_call		= 0;
  }

  InstancedRenderer2D& InstancedRenderer2D::operator=(InstancedRenderer2D&& other) noexcept
  {
	std::swap(instanceData, other.instanceData);
	std::swap(texturingCombineShader, other.texturingCombineShader);
	std::swap(fixedVertexBufferHandle, other.fixedVertexBufferHandle);
	std::swap(instanceBufferHandle, other.instanceBufferHandle);
	std::swap(modelHandle, other.modelHandle);

	std::swap(sdfInstanceData, other.sdfInstanceData);
	std::swap(sdfFixedVertexBufferHandle, other.sdfFixedVertexBufferHandle);
	std::swap(sdfInstanceBufferHandle, other.sdfInstanceBufferHandle);
	std::swap(sdfShader, other.sdfShader);
	std::swap(sdfModelHandle, other.sdfModelHandle);
	std::swap(maxSDFInstances, other.maxSDFInstances);

	std::swap(indexBufferHandle, other.indexBufferHandle);
	std::swap(camera_uniform_buffer, other.camera_uniform_buffer);
	std::swap(camera_array, other.camera_array);
	std::swap(currentCameraMatrix, other.currentCameraMatrix);
	std::swap(maxInstances, other.maxInstances);
	std::swap(textureSlots, other.textureSlots);
	std::swap(activeTextureSize, other.activeTextureSize);
	std::swap(draw_call, other.draw_call);
	std::swap(texture_call, other.texture_call);

	return *this;
  }

  InstancedRenderer2D::~InstancedRenderer2D()
  {
	Shutdown();
  }

  void InstancedRenderer2D::Init()

  {
	// get max texture units
	// get glsl code and update the fragment shader
	// create the shader
	// set th binding values for textures array

	// get how many texture opengl can draw
	GLint max_tex_units = 0;
	GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex_units); // check with docs.gl to get minimum(16) and maximum
	textureSlots.resize(static_cast<size_t>(std::min(max_tex_units, 64)));


	// load shaders with parsing
	const std::filesystem::path vertex_file = assets::locate_asset("Assets/shaders/InstancedRenderer2D/quad.vert");
	std::ifstream				vert_stream(vertex_file);
	std::stringstream			vert_text_stream;
	vert_text_stream << vert_stream.rdbuf();
	const std::string vertex_glsl = vert_text_stream.str();

	const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/InstancedRenderer2D/quad.frag");
	std::ifstream				frag_stream(fragment_file);
	std::stringstream			frag_text_stream;
	frag_text_stream << frag_stream.rdbuf();
	std::string frag_glsl = frag_text_stream.str();

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

	// create our fixed buffer data
	// create index buffer data
	// create our instanced buffer
	// create VAO

	constexpr float fixed_sprite_vertices[][4] = {
	  // bottom left
	  { -0.5f, -0.5f, 0.0f, 0.0f },
	  // bottom right
	  {	0.5f, -0.5f, 1.0f, 0.0f },
	  // top right
	  {	0.5f,  0.5f, 1.0f, 1.0f },
	  // top left
	  { -0.5f,  0.5f, 0.0f, 1.0f }
	};

	constexpr unsigned char indicies[] = { 0, 1, 2, 0, 2, 3 };

	fixedVertexBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ fixed_sprite_vertices }));
	indexBufferHandle		= OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indicies }));
	instanceBufferHandle	= OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, static_cast<GLsizeiptr>(sizeof(QuadInstance) * maxInstances));

	auto fixedbuffer_and_instancebuffer = {
	  OpenGL::VertexBuffer{ fixedVertexBufferHandle,									 { OpenGL::Attribute::Float2, OpenGL::Attribute::Float2 }												  },
	  OpenGL::VertexBuffer{	instanceBufferHandle,
							{ OpenGL::Attribute::Float3.WithDivisor(1), OpenGL::Attribute::Float3.WithDivisor(1), OpenGL::Attribute::UByte4ToNormalized.WithDivisor(1),
							  OpenGL::Attribute::Float2.WithDivisor(1), OpenGL::Attribute::Float2.WithDivisor(1), OpenGL::Attribute::Int.WithDivisor(1), OpenGL::Attribute::Float.WithDivisor(1) } }
	};

	modelHandle = OpenGL::CreateVertexArrayObject(fixedbuffer_and_instancebuffer, indexBufferHandle);

	// SDF
	//  create vertex array object, buffer vertices, buffer indices
	sdfShader = OpenGL::CreateShader(assets::locate_asset("Assets/shaders/InstancedRenderer2D/sdf.vert"), assets::locate_asset("Assets/shaders/InstancedRenderer2D/sdf.frag"));

	constexpr float position_vertices[][2] = {
	  // bottom left
	  { -0.5f, -0.5f },
	  // bottom right
	  {	0.5f, -0.5f },
	  // top right
	  {	0.5f,  0.5f },
	  // top left
	  { -0.5f,  0.5f }
	};
	sdfFixedVertexBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ position_vertices }));
	sdfInstanceBufferHandle	   = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, static_cast<GLsizeiptr>(sizeof(SDFInstance) * maxInstances));

	const auto sdf_fix_instance = {
	  OpenGL::VertexBuffer{ sdfFixedVertexBufferHandle,{ OpenGL::Attribute::Float2 } }, //  Layout 0: aModelPosition														},
	  OpenGL::VertexBuffer{	sdfInstanceBufferHandle,
							{
							  OpenGL::Attribute::Float3.WithDivisor(1),				// Layout 1: aModelRow0
							  OpenGL::Attribute::Float3.WithDivisor(1),				// Layout 2: aModelRow1
							  OpenGL::Attribute::UByte4ToNormalized.WithDivisor(1), // Layout 3: aFillColor
							  OpenGL::Attribute::UByte4ToNormalized.WithDivisor(1), // Layout 4: aLineColor
							  OpenGL::Attribute::Float2.WithDivisor(1),				// Layout 5: aWorldSize
							  OpenGL::Attribute::Float.WithDivisor(1),				// Layout 6: aLineWidth
							  OpenGL::Attribute::Int.WithDivisor(1),				// Layout 7: aShape (0=Circle, 1=Rect)
							  OpenGL::Attribute::Float.WithDivisor(1),				// Layout 8: aDepth
							}  }
	};
	sdfModelHandle = OpenGL::CreateVertexArrayObject(sdf_fix_instance, indexBufferHandle);

	camera_uniform_buffer = OpenGL::CreateBuffer(OpenGL::BufferType::UniformBlocks, sizeof(camera_array));
	OpenGL::BindUniformBufferToShader(texturingCombineShader.Shader, 0, camera_uniform_buffer, "NDC");
	OpenGL::BindUniformBufferToShader(sdfShader.Shader, 0, camera_uniform_buffer, "NDC");
  }

  void InstancedRenderer2D::Shutdown()

  {
	OpenGL::DestroyShader(texturingCombineShader);
	OpenGL::DestroyShader(sdfShader);

	GL::DeleteBuffers(1, &fixedVertexBufferHandle), fixedVertexBufferHandle		  = 0;
	GL::DeleteBuffers(1, &instanceBufferHandle), instanceBufferHandle			  = 0;
	GL::DeleteBuffers(1, &sdfFixedVertexBufferHandle), sdfFixedVertexBufferHandle = 0;
	GL::DeleteBuffers(1, &sdfInstanceBufferHandle), sdfInstanceBufferHandle		  = 0;
	GL::DeleteBuffers(1, &indexBufferHandle), indexBufferHandle					  = 0;
	GL::DeleteBuffers(1, &camera_uniform_buffer), camera_uniform_buffer			  = 0;

	GL::DeleteVertexArrays(1, &modelHandle), modelHandle	   = 0;
	GL::DeleteVertexArrays(1, &sdfModelHandle), sdfModelHandle = 0;

	instanceData.clear();
	sdfInstanceData.clear();
	textureSlots.clear();

	activeTextureSize = 0;
	draw_call		  = 0;
	texture_call	  = 0;
  }

  void InstancedRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
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

  void InstancedRenderer2D::EndScene()
  {
	flush();
  }

  void InstancedRenderer2D::DrawQuad(
	const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor, float depth)
  {
	if (instanceData.size() >= maxInstances)
	{
	  flush();
	}

	if (sdfInstanceData.size() >= maxSDFInstances)
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


	const float left   = static_cast<float>(texture_coord_bl.x);
	const float bottom = static_cast<float>(texture_coord_bl.y);
	const float right  = static_cast<float>(texture_coord_tr.x);
	const float top	   = static_cast<float>(texture_coord_tr.y);

	QuadInstance instance;
	instance.textureIndex	  = tex_index;
	instance.texScale[0]	  = right - left;
	instance.texScale[1]	  = top - bottom;
	instance.texOffset[0]	  = left;
	instance.texOffset[1]	  = bottom;
	instance.transformrow0[0] = static_cast<float>(transform[0][0]);
	instance.transformrow0[1] = static_cast<float>(transform[0][1]);
	instance.transformrow0[2] = static_cast<float>(transform[0][2]);
	instance.transformrow1[0] = static_cast<float>(transform[1][0]);
	instance.transformrow1[1] = static_cast<float>(transform[1][1]);
	instance.transformrow1[2] = static_cast<float>(transform[1][2]);
	instance.tint			  = ColorArray(tintColor);
	instance.depth			  = depth;

	instanceData.push_back(instance);

	++texture_call;
  }

  void InstancedRenderer2D::startBatch()

  {
	instanceData.clear();

	activeTextureSize = 0;


	sdfInstanceData.clear();
  }

  void InstancedRenderer2D::flush()
  {
	if (!instanceData.empty()) [[unlikely]]
	{
	  GL::BindBuffer(GL_ARRAY_BUFFER, instanceBufferHandle);
	  GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(QuadInstance) * maxInstances), nullptr, GL_DYNAMIC_DRAW);
	  OpenGL::UpdateBufferData(OpenGL::BufferType::Vertices, instanceBufferHandle, std::as_bytes(std::span{ instanceData.data(), instanceData.size() }));

	  // select our texture
	  for (size_t i = 0; i < activeTextureSize; ++i)
	  {
		GL::ActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
		GL::BindTexture(GL_TEXTURE_2D, textureSlots[i]);
	  }
	  GL::UseProgram(texturingCombineShader.Shader);
	  GL::BindVertexArray(modelHandle);
	  GL::DrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr, static_cast<GLsizei>(instanceData.size()));
	  ++draw_call;
	}

	if (!sdfInstanceData.empty())
	{
	  GL::BindBuffer(GL_ARRAY_BUFFER, sdfInstanceBufferHandle);
	  GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(SDFInstance) * maxSDFInstances), nullptr, GL_DYNAMIC_DRAW);

	  OpenGL::UpdateBufferData(OpenGL::BufferType::Vertices, sdfInstanceBufferHandle, std::as_bytes(std::span{ sdfInstanceData.data(), sdfInstanceData.size() }));

	  GL::UseProgram(sdfShader.Shader);
	  GL::BindVertexArray(sdfModelHandle);
	  GL::DrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr, static_cast<GLsizei>(sdfInstanceData.size()));
	  ++draw_call;
	}
	GL::BindVertexArray(0);
	GL::UseProgram(0);
	GL::BindTexture(GL_TEXTURE_2D, 0);
	GL::BindBuffer(GL_ARRAY_BUFFER, 0);

	startBatch();
  }

  void InstancedRenderer2D::DrawCircle(
	[[maybe_unused]] const Math::TransformationMatrix& transform, [[maybe_unused]] CS200::RGBA fill_color, [[maybe_unused]] CS200::RGBA line_color, [[maybe_unused]] double line_width, float depth)
  {
	if (instanceData.size() >= maxInstances)
	{
	  flush();
	}

	if (sdfInstanceData.size() >= maxSDFInstances)
	{
	  flush();
	}

	const auto sdf_transform = Renderer2DUtils::CalculateSDFTransform(transform, line_width);
	const auto fill_bytes	 = ColorArray(fill_color);
	const auto line_bytes	 = ColorArray(line_color);

	SDFInstance sdf_instance;

	sdf_instance.transformrow0[0] = sdf_transform.QuadTransform[0];
	sdf_instance.transformrow0[1] = sdf_transform.QuadTransform[3];
	sdf_instance.transformrow0[2] = sdf_transform.QuadTransform[6];

	sdf_instance.transformrow1[0] = sdf_transform.QuadTransform[1];
	sdf_instance.transformrow1[1] = sdf_transform.QuadTransform[4];
	sdf_instance.transformrow1[2] = sdf_transform.QuadTransform[7];

	sdf_instance.fillColor	 = fill_bytes;
	sdf_instance.lineColor	 = line_bytes;
	sdf_instance.worldSize_x = static_cast<float>(sdf_transform.WorldSize[0]);
	sdf_instance.worldSize_y = static_cast<float>(sdf_transform.WorldSize[1]);
	sdf_instance.lineWidth	 = static_cast<float>(line_width);
	sdf_instance.shape		 = static_cast<int>(SDFShape::Circle); // 0
	sdf_instance.depth		 = depth;

	sdfInstanceData.push_back(sdf_instance);

	++texture_call;
  }

  void InstancedRenderer2D::DrawRectangle(
	[[maybe_unused]] const Math::TransformationMatrix& transform, [[maybe_unused]] CS200::RGBA fill_color, [[maybe_unused]] CS200::RGBA line_color, [[maybe_unused]] double line_width, float depth)
  {
	if (instanceData.size() >= maxInstances)
	{
	  flush();
	}

	if (sdfInstanceData.size() >= maxSDFInstances)
	{
	  flush();
	}

	const auto sdf_transform = Renderer2DUtils::CalculateSDFTransform(transform, line_width);
	const auto fill_bytes	 = ColorArray(fill_color);
	const auto line_bytes	 = ColorArray(line_color);

	SDFInstance sdf_instance;

	sdf_instance.transformrow0[0] = sdf_transform.QuadTransform[0];
	sdf_instance.transformrow0[1] = sdf_transform.QuadTransform[3];
	sdf_instance.transformrow0[2] = sdf_transform.QuadTransform[6];

	sdf_instance.transformrow1[0] = sdf_transform.QuadTransform[1];
	sdf_instance.transformrow1[1] = sdf_transform.QuadTransform[4];
	sdf_instance.transformrow1[2] = sdf_transform.QuadTransform[7];

	sdf_instance.fillColor	 = fill_bytes;
	sdf_instance.lineColor	 = line_bytes;
	sdf_instance.worldSize_x = static_cast<float>(sdf_transform.WorldSize[0]);
	sdf_instance.worldSize_y = static_cast<float>(sdf_transform.WorldSize[1]);
	sdf_instance.lineWidth	 = static_cast<float>(line_width);
	sdf_instance.shape		 = static_cast<int>(SDFShape::Rectangle); // 1
	sdf_instance.depth		 = depth;

	sdfInstanceData.push_back(sdf_instance);

	++texture_call;
  }

  void InstancedRenderer2D::DrawLine(
	[[maybe_unused]] const Math::TransformationMatrix& transform, [[maybe_unused]] Math::vec2 start_point, [[maybe_unused]] Math::vec2 end_point, [[maybe_unused]] CS200::RGBA line_color,
	[[maybe_unused]] double line_width, float depth)
  {
	const auto line_transform = Renderer2DUtils::CalculateLineTransform(transform, start_point, end_point, line_width);
	DrawRectangle(line_transform, line_color, line_color, line_width, depth);
  }

  void InstancedRenderer2D::DrawLine(
	[[maybe_unused]] Math::vec2 start_point, [[maybe_unused]] Math::vec2 end_point, [[maybe_unused]] CS200::RGBA line_color, [[maybe_unused]] double line_width, float depth)
  {
	DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width, depth);
  }

  void InstancedRenderer2D::updateCameraUniformValues(const Math::TransformationMatrix& view_projection)
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

  size_t InstancedRenderer2D::GetDrawCallCounter()
  {
	return draw_call;
  }

  size_t InstancedRenderer2D::GetDrawTextureCounter()
  {
	return texture_call;
  }
}