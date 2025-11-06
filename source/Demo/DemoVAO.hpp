#pragma once
/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Engine/GameState.hpp"
#include "OpenGL/Handle.hpp"
#include "OpenGL/Shader.hpp"
#include <array>

namespace OpenGL
{
    using BufferHandle      = Handle;
    using VertexArrayHandle = Handle;
}

class DemoVAO : public CS230::GameState
{
public:
    void Load() override;
    void Unload() override;
    void Update(double dt) override;
    void Draw() override;
    void DrawImGui() override;

    gsl::czstring GetName() const override
    {
        return "Demo VOA";
    }

public:
    DemoVAO() = default;
    ~DemoVAO() override;
    DemoVAO(const DemoVAO&)            = delete;
    DemoVAO(DemoVAO&&)                 = delete;
    DemoVAO& operator=(const DemoVAO&) = delete;
    DemoVAO& operator=(DemoVAO&&)      = delete;

private:
    float                               hue        = 218.5f;
    static constexpr float              saturation = 0.578f;
    static constexpr float              value      = 0.929f;
    OpenGL::CompiledShader              simpleShader{};
    std::array<OpenGL::BufferHandle, 4> bufferHandles{};
    std::array<OpenGL::BufferHandle, 3> indexBufferHandles{};
    OpenGL::VertexArrayHandle           leftEyeModel{}, rightEyeModel{}, mouthModel{};

private:
    void calculate_and_set_clear_color() const;
    void create_1buffer_struct_of_arrays_style(); // left eye
    void create_1buffer_array_of_structs_style(); // right eye
    void create_parallel_buffers_style();         // mouth
};
