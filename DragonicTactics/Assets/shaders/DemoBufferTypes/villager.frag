#version 300 es
precision mediump float;

/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

in vec3 vColor;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
