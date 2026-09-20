#version 300 es
precision         mediump float;

/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

uniform sampler2D uTex2d;

in vec2 vTexCoord;

layout(location = 0) out vec4 fFragClr;

uniform vec4 uTint; //  cover with new color

//use all variable!!!!!!!!!!!!!!
void main()
{
    fFragClr = texture(uTex2d, vTexCoord) * uTint;

    if(fFragClr.a == 0.0)
    discard;
}

