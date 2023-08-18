#version 310 es
//Color Fragment Shader
layout(location = 0) in flat uvec2 i_TexCoord;
layout(location = 1) in highp vec3 i_Normal;
layout(location = 0) out highp vec4 o_Color;
layout(std140, binding = 2) uniform Data
{
    highp vec4 colors[6];
} d_Data;

void main()
{
    uint i = i_TexCoord.x;
    o_Color = d_Data.colors[0];
}