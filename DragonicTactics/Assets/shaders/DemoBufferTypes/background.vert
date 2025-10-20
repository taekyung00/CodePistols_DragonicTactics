#version 300 es

/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vColor;

layout(std140) uniform NDC
{
    mat3 uToNDC;
};

void main()
{
    gl_Position = vec4((uToNDC * vec3(aVertexPosition, 1.0)).xy, 0.0, 1.0);
    vColor      = aVertexColor;
}
