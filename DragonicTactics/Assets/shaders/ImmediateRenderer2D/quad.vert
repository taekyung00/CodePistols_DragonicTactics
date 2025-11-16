#version 300 es

/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */


layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aTexCoord;


out vec2 vTexCoord;

layout(std140) uniform NDC
{
    mat3 uToNDC;
};

uniform mat3 uModel;
uniform mat3 uTexCoordTransform; //for drawing just part of image
uniform float uDepth;

void main()
{
    vec3 ndc_position   = uToNDC * uModel * vec3(aVertexPosition, 1.0);
    gl_Position         = vec4(ndc_position.xy, uDepth, 1.0);
    vec3 tex_coords = uTexCoordTransform * vec3(aTexCoord,1.0);
    vTexCoord = tex_coords.st ;
}
