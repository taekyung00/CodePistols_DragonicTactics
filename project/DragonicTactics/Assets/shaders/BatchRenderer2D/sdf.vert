#version 300 es

/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 * (Batch Renderer Version)
 */

// 1. Receive all per-object data as attributes
layout(location = 0) in vec2 aWorldPosition;  // World coordinates calculated on the CPU
layout(location = 1) in vec2 aTestPoint;      // vTestPoint calculated on the CPU
layout(location = 2) in vec4 aFillColor;
layout(location = 3) in vec4 aLineColor;
layout(location = 4) in vec2 aWorldSize;
layout(location = 5) in float aLineWidth;
layout(location = 6) in int aShape;
layout(location = 7) in float aDepth;

// 2. Common UBO (Camera)
layout(std140) uniform NDC
{
    mat3 uToNDC;
};

// 3. Variables to pass to the Fragment Shader
out vec2 vTestPoint;
flat out vec4 vFillColor;
flat out vec4 vLineColor;
flat out vec2 vWorldSize;
flat out float vLineWidth;
flat out int vShape;

void main()
{
    // 4. Use the pre-calculated aWorldPosition, same as the Batch Texture Shader
    vec3 ndc_point = uToNDC * vec3(aWorldPosition, 1.0); 
    gl_Position = vec4(ndc_point.xy, aDepth, 1.0);

    // 5. Pass all attributes directly to the Fragment Shader
    vTestPoint = aTestPoint;
    vFillColor = aFillColor;
    vLineColor = aLineColor;
    vWorldSize = aWorldSize;
    vLineWidth = aLineWidth;
    vShape = aShape;
}