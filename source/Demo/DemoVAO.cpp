/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "DemoVAO.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "OpenGL/GL.hpp"
#include "OpenGL/Shader.hpp"
#include "OpenGL/VertexArray.hpp"
#include "../Game/MainMenu.h"
#include <algorithm>
#include <imgui.h>

void DemoVAO::Load()
{
    using filepath = std::filesystem::path;
    simpleShader   = OpenGL::CreateShader(filepath{ "Assets/shaders/pass_thru_pos2d_clr.vert" }, filepath{ "Assets/shaders/basic_vtx_clr_attribute.frag" });

    GL::GenBuffers(static_cast<GLsizei>(bufferHandles.size()), bufferHandles.data());
    GL::GenBuffers(static_cast<GLsizei>(indexBufferHandles.size()), indexBufferHandles.data());

    create_1buffer_struct_of_arrays_style();
    create_1buffer_array_of_structs_style();
    create_parallel_buffers_style();
}

void DemoVAO::Unload()
{
    OpenGL::DestroyShader(simpleShader);
    GL::DeleteVertexArrays(1, &leftEyeModel), leftEyeModel   = 0;
    GL::DeleteVertexArrays(1, &rightEyeModel), rightEyeModel = 0;
    GL::DeleteVertexArrays(1, &mouthModel), mouthModel       = 0;
    GL::DeleteBuffers(static_cast<GLsizei>(bufferHandles.size()), bufferHandles.data());
    std::fill(std::begin(bufferHandles), std::end(bufferHandles), 0);
    GL::DeleteBuffers(static_cast<GLsizei>(indexBufferHandles.size()), indexBufferHandles.data());
    std::fill(std::begin(indexBufferHandles), std::end(indexBufferHandles), 0);
}

void DemoVAO::Update([[maybe_unused]]double dt)
{
    hue += 0.25f;
    if (hue >= 360.0f)
        hue -= 360.0f;
}

void DemoVAO::Draw() 
{
    constexpr GLsizei        indices_count            = 6;
    constexpr GLenum         primitive_pattern        = GL_TRIANGLES;
    constexpr GLenum         left_eye_indices_type    = GL_UNSIGNED_INT;
    constexpr GLenum         right_eye_indices_type   = GL_UNSIGNED_SHORT;
    constexpr GLenum         mouth_indices_type       = GL_UNSIGNED_BYTE;
    constexpr GLvoid*        byte_offset_into_indices = nullptr;
    constexpr OpenGL::Handle no_object                = 0;

    calculate_and_set_clear_color();
    CS200::RenderingAPI::Clear();

    GL::UseProgram(simpleShader.Shader);

    GL::BindVertexArray(leftEyeModel);
    GL::DrawElements(primitive_pattern, indices_count, left_eye_indices_type, byte_offset_into_indices);

    GL::BindVertexArray(rightEyeModel);
    GL::DrawElements(primitive_pattern, indices_count, right_eye_indices_type, byte_offset_into_indices);

    GL::BindVertexArray(mouthModel);
    GL::DrawElements(primitive_pattern, indices_count, mouth_indices_type, byte_offset_into_indices);

    GL::BindVertexArray(no_object);
    GL::UseProgram(no_object);
}

void DemoVAO::DrawImGui()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%d", Engine::GetWindowEnvironment().FPS);
        {
            float r = 0, g = 0, b = 0;
            ImGui::ColorConvertHSVtoRGB(hue / 360.0f, saturation, value, r, g, b);
            ImGui::LabelText("Background Color", "RGB(%.0f,%.0f,%.0f)", static_cast<double>(r * 255), static_cast<double>(g * 255), static_cast<double>(b * 255));
        }
        ImGui::SeparatorText("Switch Demo");
        if (ImGui::Button("Switch to MainMenu"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<MainMenu>();
        }

    }
    ImGui::End();
}

DemoVAO::~DemoVAO()
{
    Unload();
}

void DemoVAO::calculate_and_set_clear_color() const
{
    float r = 0, g = 0, b = 0;
    ImGui::ColorConvertHSVtoRGB(hue / 360.0f, saturation, value, r, g, b);
    GL::ClearColor(r, g, b, 1.0f);
}

namespace
{
    struct vec2
    {
        float x = 0;
        float y = 0;
    };

    struct color3
    {
        float r = 0;
        float g = 0;
        float b = 0;
    };
}

void DemoVAO::create_1buffer_struct_of_arrays_style() //left eye
{
    constexpr std::array positions = {
        vec2{ -0.2f, 0.2f },
        vec2{ -0.2f, 0.6f },
        vec2{ -0.6f, 0.6f },
        vec2{ -0.6f, 0.2f }
    };
    constexpr auto       positions_byte_size = static_cast<GLsizeiptr>(sizeof(vec2) * positions.size());
    constexpr std::array colors              = {
        color3{ 1, 1, 1 },
        color3{ 1, 0, 0 },
        color3{ 0, 1, 0 },
        color3{ 0, 0, 1 }
    };
    constexpr auto                    colors_byte_size   = static_cast<GLsizeiptr>(sizeof(color3) * colors.size());
    constexpr auto                    buffer_size        = positions_byte_size + colors_byte_size;
    constexpr std::array<unsigned, 6> indices            = { 0, 1, 2, 2, 3, 0 };
    constexpr const void*             no_data            = nullptr;
    const auto&                       leftEyeVertBuffer  = bufferHandles[0];
    const auto&                       leftEyeIndexBuffer = indexBufferHandles[0];

    GL::BindBuffer(GL_ARRAY_BUFFER, leftEyeVertBuffer);
    GL::BufferData(GL_ARRAY_BUFFER, buffer_size, no_data, GL_STATIC_DRAW);//whole data size
    GL::BufferSubData(GL_ARRAY_BUFFER, 0, positions_byte_size, positions.data()); //sub data size
    GL::BufferSubData(GL_ARRAY_BUFFER, positions_byte_size, colors_byte_size, colors.data());
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, leftEyeIndexBuffer);
    GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    const auto layout_description = {
        OpenGL::VertexBuffer{ leftEyeVertBuffer,                      OpenGL::BufferLayout{ { OpenGL::Attribute::Float2 } } },
        OpenGL::VertexBuffer{ leftEyeVertBuffer, OpenGL::BufferLayout{ positions_byte_size, { OpenGL::Attribute::Float3 } } }
    };
    leftEyeModel = OpenGL::CreateVertexArrayObject(layout_description, leftEyeIndexBuffer);
}

namespace
{
    struct Vertex
    {
        vec2   position{};
        color3 color{};
    };

}

void DemoVAO::create_1buffer_array_of_structs_style() //right eye
{
    constexpr std::array vertices = {
        Vertex{ vec2{ 0.6f, 0.2f }, color3{ 1, 0, 0 } },
        Vertex{ vec2{ 0.6f, 0.6f }, color3{ 0, 1, 0 } },
        Vertex{ vec2{ 0.2f, 0.6f }, color3{ 0, 0, 1 } },
        Vertex{ vec2{ 0.2f, 0.2f }, color3{ 1, 1, 1 } }
    };
    constexpr std::array<unsigned short, 6> indices = { 0, 1, 2, 2, 3, 0 };

    const auto& rightEyeVertBuffer  = bufferHandles[1];
    const auto& rightEyeIndexBuffer = indexBufferHandles[1];

    GL::BindBuffer(GL_ARRAY_BUFFER, rightEyeVertBuffer);
    GL::BufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightEyeIndexBuffer);
    GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    const auto layout = OpenGL::VertexBuffer{ rightEyeVertBuffer, OpenGL::BufferLayout{ { OpenGL::Attribute::Float2, OpenGL::Attribute::Float3 } } };
    rightEyeModel     = OpenGL::CreateVertexArrayObject(layout, rightEyeIndexBuffer);
}

void DemoVAO::create_parallel_buffers_style() //mouth //why parallel?
{
    constexpr std::array positions = {
        vec2{  0.6f, -0.6f },
        vec2{  0.6f, -0.2f },
        vec2{ -0.6f, -0.2f },
        vec2{ -0.6f, -0.6f }
    };
    constexpr std::array colors = {
        color3{ 0, 0, 1 },
        color3{ 1, 1, 1 },
        color3{ 1, 0, 0 },
        color3{ 0, 1, 0 }
    };
    constexpr std::array<unsigned char, 6> indices = { 0, 1, 2, 2, 3, 0 };

    const auto& mouthVertBufferPosition = bufferHandles[2];
    const auto& mouthVertBufferColor    = bufferHandles[3];
    const auto& mouthIndexBuffer        = indexBufferHandles[2];


    GL::BindBuffer(GL_ARRAY_BUFFER, mouthVertBufferPosition);
    GL::BufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), positions.data(), GL_STATIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);


    GL::BindBuffer(GL_ARRAY_BUFFER, mouthVertBufferColor);
    GL::BufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(colors[0]), colors.data(), GL_STATIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mouthIndexBuffer);
    GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    const auto layout_description = {
        OpenGL::VertexBuffer{ mouthVertBufferPosition, OpenGL::BufferLayout{ { OpenGL::Attribute::Float2 } } },
        OpenGL::VertexBuffer{    mouthVertBufferColor, OpenGL::BufferLayout{ { OpenGL::Attribute::Float3 } } }
    };
    mouthModel = OpenGL::CreateVertexArrayObject(layout_description, mouthIndexBuffer);
}
