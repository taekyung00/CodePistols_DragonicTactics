#version 300 es
precision mediump float;

/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 * (Batch Renderer Version)
 */

// 1. Receive all uniforms as 'in' variables
in vec2 vTestPoint;
flat in vec4 vFillColor;
flat in vec4 vLineColor;
flat in vec2 vWorldSize;
flat in float vLineWidth;
flat in int vShape;

layout(location = 0) out vec4 FragColor;

// --- sdCircle, sdRectangle functions are identical to the original ---
float sdCircle( vec2 p, float r )
{
    return length(p) - r;
}

float sdRectangle( vec2 point, vec2 half_dim )
{
    vec2 d = abs(point)-half_dim;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

// 2. Modify evalute_color function to use 'v' (in) variables instead of uniforms
vec4 evalute_color(float sdf)
{
    float fill_alpha = (sdf < 0.0) ? 1.0 : 0.0;
    // 'uLineWidth' -> 'vLineWidth'
    float outline_alpha = (abs(sdf) < vLineWidth * 0.5) ? 1.0 : 0.0; 

    // 'uFillColor' -> 'vFillColor'
    vec4 fill_color = vec4(vFillColor.rgb, fill_alpha * vFillColor.a);
    // 'uLineColor' -> 'vLineColor'
    vec4 line_color = vec4(vLineColor.rgb, outline_alpha * vLineColor.a);

    return mix(fill_color,line_color,line_color.a);
}

void main()
{
    float sdf = 0.0;
    
    // 3. Check 'vShape' (from vertex) instead of 'uShape' (uniform)
    if(vShape == 0){ // This pixel belongs to a Circle
        // 'uWorldSize' -> 'vWorldSize'
        float radius = min(vWorldSize.x ,vWorldSize.y) * 0.5; 
        sdf = sdCircle(vTestPoint, radius);
    }
    else if(vShape == 1){ // This pixel belongs to a Rectangle
        // 'uWorldSize' -> 'vWorldSize'
        sdf = sdRectangle(vTestPoint, 0.5 * vWorldSize); 
    }
    
    vec4 color = evalute_color(sdf);
    if(color.a <= 0.0 )
        discard;
    FragColor = color;
}