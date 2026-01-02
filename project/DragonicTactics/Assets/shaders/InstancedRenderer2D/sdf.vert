#version 300 es

/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 * (Batch Renderer Version)
 */


layout(location = 0) in vec2 aModelPosition;

layout(location = 1) in vec3 aModelRow0;
layout(location = 2) in vec3 aModelRow1;
// layout(location = 3) in vec2 aTestPoint;      // vTestPoint doesn't calculated in CPU anymore, but instead in here!!
layout(location = 3) in vec4 aFillColor;
layout(location = 4) in vec4 aLineColor;
layout(location = 5) in vec2 aWorldSize;
layout(location = 6) in float aLineWidth;
layout(location = 7) in int aShape;
layout(location = 8) in float aDepth;


layout(std140) uniform NDC
{
    mat3 uToNDC;
};


out vec2 vTestPoint;
flat out vec4 vFillColor;
flat out vec4 vLineColor;
flat out vec2 vWorldSize;
flat out float vLineWidth;
flat out int vShape;

void main()
{
    vec2 world_position;
    world_position.x = aModelPosition.x * aModelRow0[0] + aModelPosition.y * aModelRow0[1] + aModelRow0[2];
    world_position.y = aModelPosition.x * aModelRow1[0] + aModelPosition.y * aModelRow1[1] + aModelRow1[2];
    vec3 ndc_point = uToNDC * vec3(world_position, 1.0); 
    gl_Position = vec4(ndc_point.xy, aDepth, 1.0); // Use 1 depth for SDF


    // vTestPoint = aTestPoint;
    // vTestPoint = aModelPosition * aWorldSize;
    vec2 quadSize = aWorldSize + vec2(aLineWidth);
    vTestPoint = aModelPosition * quadSize;
    vFillColor = aFillColor;
    vLineColor = aLineColor;
    vWorldSize = aWorldSize;
    vLineWidth = aLineWidth;
    vShape = aShape;
}