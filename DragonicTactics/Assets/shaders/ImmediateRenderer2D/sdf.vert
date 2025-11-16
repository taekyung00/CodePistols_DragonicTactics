#version 300 es

/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */


layout(location = 0) in vec2 aVertexPosition;

uniform mat3 uModel;
layout(std140) uniform NDC
{
    mat3 uToNDC;
};
uniform vec2 uSDFScale;

out vec2 vTestPoint;

uniform float uDepth;
void main()
{
    vec3 ndc_point = uToNDC * uModel * vec3(aVertexPosition,1.0);
    gl_Position = vec4(ndc_point.xy, uDepth, 1.0);
    vTestPoint = aVertexPosition * uSDFScale; //scale
}
