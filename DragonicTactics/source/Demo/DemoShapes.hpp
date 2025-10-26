/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"
#include <array>
#include <gsl/gsl>

class DemoShapes : public CS230::GameState
{
public:
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:

    enum class RenderMode
    {
        Filled,
        Outlined,
        Both
    };


    enum class ShapeType
    {
        Rectangle,
        Circle,
        Line
    };

    // Shape properties
    Math::vec2 shapePosition{ -300.0, -300.0 };
    double     shapeRotation = 0.0;
    Math::vec2 shapeScale{ 100.0, 100.0 };

    // Target values for smooth easing
    Math::vec2 targetShapePosition{ 0.0, 0.0 };
    double     targetShapeRotation = 0.0;
    Math::vec2 targetShapeScale{ 100.0, 100.0 };

    // Visual properties
    CS200::RGBA fillColor = CS200::BLACK;
    CS200::RGBA lineColor = CS200::BLACK;
    double      lineWidth = 10.0;

    // Current visual properties (using float arrays for smooth color easing)
    std::array<float, 4> currentFillColor = { 0.259f, 0.522f, 0.957f, 1.0f };
    std::array<float, 4> currentLineColor = { 0.859f, 0.267f, 0.216f, 1.0f };
    double               targetLineWidth  = 4.0;
    std::array<float, 4> targetFillColor  = { 0.259f, 0.522f, 0.957f, 1.0f };
    std::array<float, 4> targetLineColor  = { 0.859f, 0.267f, 0.216f, 1.0f };

    // Demo state
    RenderMode currentRenderMode = RenderMode::Both;
    ShapeType  currentShape      = ShapeType::Rectangle;

    // Animation and interaction
    bool animateRotation = false;
    bool animateScale    = false;

private:
    void                       updateInput();
    void                       drawCurrentShape() const;
    void                       drawRectangles() const;
    void                       drawCircles() const;
    void                       drawLines() const;
    void                       drawColorPalette() const;
    Math::TransformationMatrix getShapeTransform() const;
    CS200::RGBA                getFillColor() const;
    CS200::RGBA                getLineColor() const;
    const char*                getRenderModeString() const;
    const char*                getShapeTypeString() const;
};
