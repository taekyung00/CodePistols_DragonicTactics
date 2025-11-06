/**
 * \file
 * \author Rudy Castan
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "ImmediateRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Path.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Window.hpp"
#include "NDC.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/GL.hpp"
#include "Renderer2DUtils.hpp"
#include "RenderingAPI.hpp"
#include <span>
#include <utility>

namespace CS200
{


    void ImmediateRenderer2D::Init()
    {
        /** - Create index buffer with quad indices (0,1,2,2,3,0)
         * - Create vertex buffer with quad vertices (-0.5 to 0.5 range)
         * - Set up VAO with position and texture coordinate attributes
         * - Create SDF vertex buffer (position-only attributes) */

        //- Load and compile vertex/fragment shaders from Assets/shaders/
        const std::filesystem::path vertex_file   = assets::locate_asset("Assets/shaders/ImmediateRenderer2D/quad.vert");
        const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/ImmediateRenderer2D/quad.frag");
        texturingCombineShader                    = OpenGL::CreateShader(vertex_file, fragment_file);

        sdfShader = OpenGL::CreateShader(assets::locate_asset("Assets/shaders/ImmediateRenderer2D/sdf.vert"), assets::locate_asset("Assets/shaders/ImmediateRenderer2D/sdf.frag"));

        struct position
        {
            float x, y;
        };

        struct texture_coordinate
        {
            float s, t;
        };

        constexpr std::array positions = {
            position{ -0.5f, -0.5f }, // bottom-left
            position{ -0.5f,  0.5f }, // top-left
            position{  0.5f,  0.5f }, // top-right
            position{  0.5f, -0.5f }  // bottom-right
        };

        constexpr std::array texture_coordinates = {
            texture_coordinate{ 0.0f, 0.0f }, // bottom-left
            texture_coordinate{ 0.0f, 1.0f }, // top-left
            texture_coordinate{ 1.0f, 1.0f }, // top-right
            texture_coordinate{ 1.0f, 0.0f }  // bottom-right
        };

        constexpr std::array<unsigned char, 6> indices = { 0, 3, 2, 0, 2, 1 };

        quad.positionBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ positions }));
        quad.texCoordBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ texture_coordinates }));
        quad.indexBufferHandle    = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indices }));
        quad.indicesCount         = static_cast<GLsizei>(indices.size());
        const auto layout         = {
            OpenGL::VertexBuffer{ quad.positionBufferHandle, { OpenGL::Attribute::Float2 } },
            OpenGL::VertexBuffer{ quad.texCoordBufferHandle, { OpenGL::Attribute::Float2 } }
        };
        quad.modelHandle = OpenGL::CreateVertexArrayObject(layout, quad.indexBufferHandle);


        //- Create SDF vertex buffer (position-only attributes)

        sdfBufferHandle                 = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ positions }));
        const auto layout_position_only = {
            OpenGL::VertexBuffer{ quad.positionBufferHandle, { OpenGL::Attribute::Float2 } }
        };
        sdfVeretexArrayHandle = OpenGL::CreateVertexArrayObject(layout_position_only, quad.indexBufferHandle);
        //- Create uniform buffer for camera/view-projection matrix
       camera_uniform_buffer = OpenGL::CreateBuffer(OpenGL::BufferType::UniformBlocks, sizeof(camera_array));


        //- Bind uniform buffer to both shaders with name "Camera"
        //OpenGL::BindUniformBufferToShader(texturingCombineShader.Shader, 0, camera_uniform_buffer, "NDC");
    }

    void ImmediateRenderer2D::Shutdown()
    {
        OpenGL::DestroyShader(texturingCombineShader);
        GL::DeleteBuffers(1, &quad.positionBufferHandle), quad.positionBufferHandle = 0;
        GL::DeleteBuffers(1, &quad.texCoordBufferHandle), quad.texCoordBufferHandle = 0;
        GL::DeleteBuffers(1, &quad.indexBufferHandle), quad.indexBufferHandle       = 0;
        GL::DeleteBuffers(1, &sdfBufferHandle), sdfBufferHandle                     = 0;
    }

    void ImmediateRenderer2D::BeginScene([[maybe_unused]] const Math::TransformationMatrix& view_projection)
    {
        //- Store matrix for potential later use
        currentCameraMatrix = view_projection;

        //- Convert 3x3 matrix to 4x3 format for uniform buffer
        updateCameraUniformValues(currentCameraMatrix);

        //- Update uniform buffer with new matrix data
        OpenGL::UpdateBufferData(OpenGL::BufferType::UniformBlocks, camera_uniform_buffer, std::as_bytes(std::span{ camera_array }));

        //- Bind uniform buffer for use by shaders
        GL::BindBuffer(GL_UNIFORM_BUFFER, camera_uniform_buffer);

        OpenGL::BindUniformBufferToShader(texturingCombineShader.Shader, 0, camera_uniform_buffer, "NDC");
    }

    void ImmediateRenderer2D::EndScene()
    {
    }

    void ImmediateRenderer2D::DrawQuad(
        [[maybe_unused]] const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        //- Bind texture to texture unit 0
        GL::UseProgram(texturingCombineShader.Shader);
        GL::ActiveTexture(GL_TEXTURE0);
        GL::BindTexture(GL_TEXTURE_2D, texture);

        // - Calculate texture coordinate transformation matrix - by bl and tr, and translate to opengl version !!
        std::array<float, 9> texture_transform = { static_cast<float>(texture_coord_tr.x - texture_coord_bl.x),
                                                   0.f,
                                                   0.f, // column1
                                                   0.f,
                                                   static_cast<float>(texture_coord_tr.y - texture_coord_bl.y),
                                                   0.f, // column2
                                                   static_cast<float>(texture_coord_bl.x),
                                                   static_cast<float>(texture_coord_bl.y),
                                                   1.f };

        //- Set shader uniforms: model matrix, depth, texture transform, tint color
        const auto& locations = texturingCombineShader.UniformLocations;
        GL::Uniform1i(locations.at("uTex2d"), 0);

        const auto world_transform_opengl = Renderer2DUtils::to_opengl_mat3(transform);
        // std::array<float,9> world_transform_opengl{ 128.f, 0.0f, 0.0f, 0.0f, 128.f, 0.0f, 0.0f,0.0f, 1.0f };
        GL::UniformMatrix3fv(locations.at("uModel"), 1, GL_FALSE, world_transform_opengl.data());

        GL::UniformMatrix3fv(locations.at("uTexCoordTransform"), 1, GL_FALSE, texture_transform.data());


        GL::Uniform1f(locations.at("uDepth"), 0);


        const auto colors = unpack_color(tintColor);
        GL::Uniform4f(locations.at("uTint"), colors[0], colors[1], colors[2], colors[3]);


        //- Draw using quad VAO and index buffer
        GL::BindVertexArray(quad.modelHandle);
        constexpr GLenum  primitive_pattern        = GL_TRIANGLES;
        constexpr GLenum  indices_type             = GL_UNSIGNED_BYTE;
        constexpr GLvoid* byte_offset_into_indices = nullptr;
        GL::DrawElements(primitive_pattern, quad.indicesCount, indices_type, byte_offset_into_indices);

        GL::BindTexture(GL_TEXTURE_2D, 0);
        GL::BindVertexArray(0);
        GL::UseProgram(0);
    }

    void ImmediateRenderer2D::DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Circle);
    }

    void ImmediateRenderer2D::DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(const Math::TransformationMatrix& transform, Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        const auto line_transform = Renderer2DUtils::CalculateLineTransform(transform, start_point, end_point, line_width);
        DrawSDF(line_transform, line_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width);
    }

    void ImmediateRenderer2D::updateCameraUniformValues(const Math::TransformationMatrix& view_projection)
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

    void ImmediateRenderer2D::DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape)
    {
        GL::UseProgram(sdfShader.Shader);
        // Calculate SDF-specific transform using Renderer2DUtils::CalculateSDFTransform()
        const auto  sdf_transform = Renderer2DUtils::CalculateSDFTransform(transform, line_width);
        // Set all SDF shader uniforms (model, colors, size, line width, shape type)
        const auto& locations     = sdfShader.UniformLocations;

        // vertex
       //GL::UniformMatrix3fv(locations.at("uToNDC"), 1, GL_FALSE, CS200::Renderer2DUtils::to_opengl_mat3(CS200::build_ndc_matrix(Engine::GetWindow().GetSize())).data());
        GL::UniformMatrix3fv(locations.at("uModel"), 1, GL_FALSE, sdf_transform.QuadTransform.data());
        GL::Uniform2f(locations.at("uSDFScale"), sdf_transform.QuadSize[0], sdf_transform.QuadSize[1]);
        GL::Uniform1f(locations.at("uDepth"), 1);

        // fragment
        GL::Uniform4fv(locations.at("uFillColor"), 1, CS200::unpack_color(fill_color).data());
        GL::Uniform4fv(locations.at("uLineColor"), 1, CS200::unpack_color(line_color).data());
        GL::Uniform2fv(locations.at("uWorldSize"), 1, sdf_transform.WorldSize.data());
        GL::Uniform1f(locations.at("uLineWidth"), static_cast<float>(line_width));
        GL::Uniform1i(locations.at("uShape"), static_cast<int>(sdf_shape));

        // Use SDF vertex array and draw triangles
        GL::BindVertexArray(sdfVeretexArrayHandle);
        constexpr GLenum  primitive_pattern        = GL_TRIANGLES;
        constexpr GLenum  indices_type             = GL_UNSIGNED_BYTE;
        constexpr GLvoid* byte_offset_into_indices = nullptr;
        GL::DrawElements(primitive_pattern, quad.indicesCount, indices_type, byte_offset_into_indices);

        // Shape rendering handled entirely in fragment shader
        GL::BindVertexArray(0);
        GL::UseProgram(0);
    }

    ImmediateRenderer2D::ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept
    {
        //- Transfer all OpenGL handles (VAOs, buffers, shaders)
        quad.positionBufferHandle = other.quad.positionBufferHandle;
        quad.texCoordBufferHandle = other.quad.texCoordBufferHandle;
        quad.indexBufferHandle    = other.quad.indexBufferHandle;
        quad.indicesCount         = other.quad.indicesCount;
        quad.modelHandle          = other.quad.modelHandle;
        sdfBufferHandle           = other.sdfBufferHandle;
        sdfShader                 = other.sdfShader;
        sdfVeretexArrayHandle     = other.sdfVeretexArrayHandle;

        //- Move shader objects using std::move()
        texturingCombineShader = std::move(other.texturingCombineShader);

        //- Copy view projection matrix data
        currentCameraMatrix = other.currentCameraMatrix;

        // - Set source object's handles to zero/invalid
        other.Shutdown();
    }

    ImmediateRenderer2D& ImmediateRenderer2D::operator=(ImmediateRenderer2D&& other) noexcept
    {
        //- Use std::swap to exchange all resources
        //- Safely handles self-assignment

        std::swap(quad.positionBufferHandle, other.quad.positionBufferHandle);
        std::swap(quad.texCoordBufferHandle, other.quad.texCoordBufferHandle);
        std::swap(quad.indexBufferHandle, other.quad.indexBufferHandle);
        std::swap(quad.indicesCount, other.quad.indicesCount);
        std::swap(quad.modelHandle, other.quad.modelHandle);
        std::swap(texturingCombineShader, other.texturingCombineShader);
        std::swap(currentCameraMatrix, other.currentCameraMatrix);
        std::swap(sdfBufferHandle, other.sdfBufferHandle);
        std::swap(sdfShader, other.sdfShader);
        std::swap(sdfVeretexArrayHandle, other.sdfVeretexArrayHandle);

        other.Shutdown();

        return *this;
    }

    ImmediateRenderer2D::~ImmediateRenderer2D()
    {
        Shutdown();
    }


}
