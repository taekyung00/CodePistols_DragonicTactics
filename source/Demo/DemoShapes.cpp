/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DemoShapes.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoFramebuffer.hpp"
#include "DemoText.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Window.hpp"
#include "../Game/MainMenu.h"
#include <cmath>
#include <imgui.h>
#include <iomanip>
#include <numbers>
#include <sstream>

// Arithmetic operators for std::array<float, 4> to support color easing
namespace
{
    std::array<float, 4> operator-(const std::array<float, 4>& a, const std::array<float, 4>& b)
    {
        return { a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3] };
    }

    std::array<float, 4> operator*(double scalar, const std::array<float, 4>& arr)
    {
        const auto s = static_cast<float>(scalar);
        return { s * arr[0], s * arr[1], s * arr[2], s * arr[3] };
    }

    std::array<float, 4>& operator+=(std::array<float, 4>& a, const std::array<float, 4>& b)
    {
        a[0] += b[0];
        a[1] += b[1];
        a[2] += b[2];
        a[3] += b[3];
        return a;
    }
}

void DemoShapes::Load()
{
    CS200::RenderingAPI::SetClearColor(0x252525FF);

    targetShapePosition     = shapePosition;
    targetShapeRotation     = 0.0;
    targetShapeScale        = { 100.0, 100.0 };
    currentFillColor        = CS200::unpack_color(fillColor);
    currentLineColor        = CS200::unpack_color(lineColor);
}

template <typename T, typename FLOAT = double>
static void ease_to_target(T& current, const T& target, FLOAT delta_time, FLOAT weight = 1.0)
{
    const auto easing = std::min(delta_time * weight, static_cast<FLOAT>(1.0));
    current += easing * (target - current);
}

void DemoShapes::Update([[maybe_unused]]double dt)
{
    const auto& environment = Engine::GetWindowEnvironment();

    // Update input handling (which modifies target values)
    updateInput();

    // Apply animations if enabled (modify target values)
    if (animateRotation)
    {
        targetShapeRotation += environment.DeltaTime * 1.5; // 1.5 radians per second
    }
    if (targetShapeRotation > std::numbers::pi * 2.0 && shapeRotation > std::numbers::pi * 2.0)
    {
        targetShapeRotation -= std::numbers::pi * 2.0;
        shapeRotation -= std::numbers::pi * 2.0;
    }
    if (animateScale)
    {
        const double scale_factor_x = 1.0 + 0.3 * std::sin(environment.ElapsedTime * 2.0);
        const double scale_factor_y = 1.0 + 0.3 * std::cos(environment.ElapsedTime * 2.0);
        targetShapeScale            = { 200.0 * scale_factor_x, 200.0 * scale_factor_y };
    }

    // Check if shape is offscreen and bring it back to center
    const auto   window_size  = Engine::GetWindow().GetSize();
    // Use shape size as margin - larger shapes trigger reset sooner
    const double margin       = std::max(shapeScale.x, shapeScale.y) + 50.0; // Shape size + 50px buffer
    const bool   is_offscreen = (shapePosition.x < -margin || shapePosition.x > window_size.x + margin || shapePosition.y < -margin || shapePosition.y > window_size.y + margin);

    if (is_offscreen)
    {
        // Reset target position to window center to bring shape back
        targetShapePosition.x = window_size.x / 2.0;
        targetShapePosition.y = window_size.y / 2.0;
    }

    // Ease current values toward target values
    ease_to_target(shapePosition, targetShapePosition, environment.DeltaTime, 2.0);
    ease_to_target(shapeRotation, targetShapeRotation, environment.DeltaTime, 2.0);
    ease_to_target(shapeScale, targetShapeScale, environment.DeltaTime, 2.0);
    ease_to_target(lineWidth, targetLineWidth, environment.DeltaTime, 2.0);

    // Ease colors toward target colors and update packed colors
    ease_to_target(currentFillColor, targetFillColor, environment.DeltaTime, 2.0);
    ease_to_target(currentLineColor, targetLineColor, environment.DeltaTime, 2.0);
    fillColor = CS200::pack_color(currentFillColor);
    lineColor = CS200::pack_color(currentLineColor);
}

void DemoShapes::Draw() 
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();
    renderer2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    drawCurrentShape();

    renderer2d.EndScene();
}

void DemoShapes::DrawImGui()
{
    if (ImGui::Begin("Demo Controls"))
    {
        // Shape selection
        ImGui::Text("Shape Type:");
        if (ImGui::RadioButton("Rectangle", currentShape == ShapeType::Rectangle))
            currentShape = ShapeType::Rectangle;
        ImGui::SameLine();
        if (ImGui::RadioButton("Circle", currentShape == ShapeType::Circle))
            currentShape = ShapeType::Circle;
        ImGui::SameLine();
        if (ImGui::RadioButton("Line", currentShape == ShapeType::Line))
            currentShape = ShapeType::Line;

        ImGui::SeparatorText("Render Mode");

        // Render mode selection
        if (ImGui::RadioButton("Filled", currentRenderMode == RenderMode::Filled))
            currentRenderMode = RenderMode::Filled;
        ImGui::SameLine();
        if (ImGui::RadioButton("Outlined", currentRenderMode == RenderMode::Outlined))
            currentRenderMode = RenderMode::Outlined;
        ImGui::SameLine();
        if (ImGui::RadioButton("Both", currentRenderMode == RenderMode::Both))
            currentRenderMode = RenderMode::Both;

        ImGui::SeparatorText("Transform");

        // Transform controls (modify target values for smooth easing)
        float      pos[2]      = { static_cast<float>(targetShapePosition.x), static_cast<float>(targetShapePosition.y) };
        const auto window_size = Engine::GetWindow().GetSize();
        const auto range_limit = static_cast<float>(std::max(window_size.x, window_size.y) + 100);
        if (ImGui::SliderFloat2("Position", pos, -100.0f, range_limit))
        {
            targetShapePosition.x = static_cast<double>(pos[0]);
            targetShapePosition.y = static_cast<double>(pos[1]);
        }

        float rotation = static_cast<float>(targetShapeRotation);
        if (ImGui::SliderAngle("Rotation", &rotation))
            targetShapeRotation = static_cast<double>(rotation);

        float scale[2] = { static_cast<float>(targetShapeScale.x), static_cast<float>(targetShapeScale.y) };
        if (ImGui::SliderFloat2("Scale", scale, 10.0f, 300.0f))
        {
            targetShapeScale.x = static_cast<double>(scale[0]);
            targetShapeScale.y = static_cast<double>(scale[1]);
        }

        ImGui::SeparatorText("Colors");

        // Color controls (modify target colors for smooth easing)
        ImGui::ColorEdit3("Fill Color", targetFillColor.data());
        ImGui::ColorEdit3("Line Color", targetLineColor.data());

        float line_width_float = static_cast<float>(targetLineWidth);
        if (ImGui::SliderFloat("Line Width", &line_width_float, 0.5f, 64.0f))
        {
            targetLineWidth = static_cast<double>(line_width_float);
        }

        ImGui::SeparatorText("Animation");

        // Animation controls
        ImGui::Checkbox("Animate Rotation", &animateRotation);
        ImGui::Checkbox("Animate Scale", &animateScale);

        ImGui::SeparatorText("Statistics");

        // Statistics
        ImGui::Text("Controls: Arrow Keys - Move, Q/E - Rotate");
        ImGui::Text("Current Mode: %s", getRenderModeString());
        ImGui::Text("Current Shape: %s", getShapeTypeString());

        ImGui::SeparatorText("Switch Demo");
        if (ImGui::Button("Switch to Demo Framebuffer"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoFramebuffer>();
        }
        if (ImGui::Button("Switch to Demo Text"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoText>();
        }

        if (ImGui::Button("Switch to MainMenu"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<MainMenu>();
        }
    }
    ImGui::End();
}

void DemoShapes::Unload()
{
    Engine::GetLogger().LogEvent("DemoShapes: Unloaded");
}

gsl::czstring DemoShapes::GetName() const
{
    return "Demo Shapes - 2D Rendering Showcase";
}

void DemoShapes::updateInput()
{
    const auto&  input          = Engine::GetInput();
    const auto&  environment    = Engine::GetWindowEnvironment();
    const double move_speed     = 200.0; // pixels per second
    const double rotation_speed = 2.0;   // radians per second

    // Movement controls (modify target values for smooth easing)
    if (input.KeyDown(CS230::Input::Keys::Left))
        targetShapePosition.x -= move_speed * environment.DeltaTime;
    if (input.KeyDown(CS230::Input::Keys::Right))
        targetShapePosition.x += move_speed * environment.DeltaTime;
    if (input.KeyDown(CS230::Input::Keys::Up))
        targetShapePosition.y += move_speed * environment.DeltaTime;
    if (input.KeyDown(CS230::Input::Keys::Down))
        targetShapePosition.y -= move_speed * environment.DeltaTime;

    // Rotation controls (modify target values for smooth easing)
    if (input.KeyDown(CS230::Input::Keys::Q))
        targetShapeRotation -= rotation_speed * environment.DeltaTime;
    if (input.KeyDown(CS230::Input::Keys::E))
        targetShapeRotation += rotation_speed * environment.DeltaTime;
}

void DemoShapes::drawCurrentShape() const
{
    switch (currentShape)
    {
        case ShapeType::Rectangle: drawRectangles(); break;
        case ShapeType::Circle: drawCircles(); break;
        case ShapeType::Line: drawLines(); break;
    }
}

void DemoShapes::drawRectangles() const
{
    auto&      renderer2d = Engine::GetRenderer2D();
    const auto transform  = getShapeTransform();

    // Draw main rectangle with current settings
    renderer2d.DrawRectangle(transform, getFillColor(), getLineColor(), lineWidth);

    // Draw additional example rectangles to show different modes
    const double offset_x    = 200.0;
    const double small_scale = 60.0;

    // Filled only example
    auto filled_transform =
        Math::TranslationMatrix(Math::vec2{ shapePosition.x - offset_x, shapePosition.y + 150.0 }) * Math::RotationMatrix(shapeRotation * 0.5) * Math::ScaleMatrix({ small_scale, small_scale });
    renderer2d.DrawRectangle(filled_transform, 0x4CAF50FF, CS200::CLEAR, 0.0);

    // Outlined only example
    auto outlined_transform =
        Math::TranslationMatrix(Math::vec2{ shapePosition.x, shapePosition.y + 150.0 }) * Math::RotationMatrix(-shapeRotation * 0.5) * Math::ScaleMatrix({ small_scale, small_scale });
    renderer2d.DrawRectangle(outlined_transform, CS200::CLEAR, 0xFF9800FF, 2.0);

    // Both fill and outline example
    auto both_transform =
        Math::TranslationMatrix(Math::vec2{ shapePosition.x + offset_x, shapePosition.y + 150.0 }) * Math::RotationMatrix(shapeRotation * 0.3) * Math::ScaleMatrix({ small_scale, small_scale });
    renderer2d.DrawRectangle(both_transform, 0x9C27B0FF, 0xFFEB3BFF, 3.0);
}

void DemoShapes::drawCircles() const
{
    auto&      renderer2d = Engine::GetRenderer2D();
    const auto transform  = getShapeTransform();

    // Draw main circle with current settings
    renderer2d.DrawCircle(transform, getFillColor(), getLineColor(), lineWidth);

    // Draw additional example circles to show different modes
    const double offset_x    = 200.0;
    const double small_scale = 60.0;

    // Filled only example
    auto filled_transform = Math::TranslationMatrix(Math::vec2{ shapePosition.x - offset_x, shapePosition.y + 150.0 }) * Math::ScaleMatrix({ small_scale, small_scale });
    renderer2d.DrawCircle(filled_transform, 0xF44336FF, CS200::CLEAR, 0.0);

    // Outlined only example
    auto outlined_transform = Math::TranslationMatrix(Math::vec2{ shapePosition.x, shapePosition.y + 150.0 }) * Math::ScaleMatrix({ small_scale, small_scale });
    renderer2d.DrawCircle(outlined_transform, CS200::CLEAR, 0x2196F3FF, 2.0);

    // Both fill and outline example
    auto both_transform = Math::TranslationMatrix(Math::vec2{ shapePosition.x + offset_x, shapePosition.y + 150.0 }) * Math::ScaleMatrix({ small_scale, small_scale });
    renderer2d.DrawCircle(both_transform, 0xFF5722FF, 0x00BCD4FF, 3.0);
}

void DemoShapes::drawLines() const
{
    auto& renderer2d = Engine::GetRenderer2D();

    // Draw transformation-based line (transforms with the shape)
    const auto transform = Math::TranslationMatrix(shapePosition) * Math::RotationMatrix(shapeRotation);
    renderer2d.DrawLine(transform * Math::ScaleMatrix({ shapeScale.x, 1.0 }), Math::vec2{ -0.5, 0.0 }, Math::vec2{ 0.5, 0.0 }, getLineColor(), lineWidth);
    renderer2d.DrawLine(transform * Math::ScaleMatrix({ 1.0, shapeScale.y }), Math::vec2{ 0.0, -0.5 }, Math::vec2{ 0.0, 0.5 }, getLineColor(), lineWidth);

    const auto display_size = Engine::GetWindowEnvironment().DisplaySize;
    // Draw world-coordinate lines (fixed positions)
    const auto into_screen  = lineWidth * 0.5;
    renderer2d.DrawLine({ 0.0, into_screen }, { display_size.x, into_screen }, lineColor, lineWidth);
    renderer2d.DrawLine({ into_screen, 0.0 }, { into_screen, display_size.y }, lineColor, lineWidth);
    renderer2d.DrawLine({ 0.0, display_size.y - into_screen }, { display_size.x, display_size.y - into_screen }, lineColor, lineWidth);
    renderer2d.DrawLine({ display_size.x - into_screen, 0.0 }, { display_size.x - into_screen, display_size.y }, lineColor, lineWidth);
}

Math::TransformationMatrix DemoShapes::getShapeTransform() const
{
    return Math::TranslationMatrix(shapePosition) * Math::RotationMatrix(shapeRotation) * Math::ScaleMatrix(shapeScale);
}

CS200::RGBA DemoShapes::getFillColor() const
{
    switch (currentRenderMode)
    {
        case RenderMode::Filled:
        case RenderMode::Both: return fillColor;
        case RenderMode::Outlined: return CS200::CLEAR;
        default: return CS200::CLEAR;
    }
}

CS200::RGBA DemoShapes::getLineColor() const
{
    switch (currentRenderMode)
    {
        case RenderMode::Outlined:
        case RenderMode::Both: return lineColor;
        case RenderMode::Filled: return CS200::CLEAR;
        default: return CS200::CLEAR;
    }
}

const char* DemoShapes::getRenderModeString() const
{
    switch (currentRenderMode)
    {
        case RenderMode::Filled: return "Filled";
        case RenderMode::Outlined: return "Outlined";
        case RenderMode::Both: return "Both";
        default: return "Unknown";
    }
}

const char* DemoShapes::getShapeTypeString() const
{
    switch (currentShape)
    {
        case ShapeType::Rectangle: return "Rectangle";
        case ShapeType::Circle: return "Circle";
        case ShapeType::Line: return "Line";
        default: return "Unknown";
    }
}
