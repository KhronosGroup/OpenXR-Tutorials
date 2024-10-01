// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#version 310 es
layout(std140, binding = 0) uniform CameraConstants {
    mat4 viewProj;
    mat4 modelViewProj;
    mat4 model;
    vec4 colour;
    vec4 pad1;
    vec4 pad2;
    vec4 pad3;
};
layout(std140, binding = 1) uniform Normals {
    vec4 normals[6];
};
layout(location = 0) in highp vec4 a_Positions;
layout(location = 0) out flat uvec2 o_TexCoord;
layout(location = 1) out highp vec3 o_Normal;
layout(location = 2) out flat vec3 o_Colour;
void main() {
    gl_Position = modelViewProj * a_Positions;
    int face = gl_VertexID / 6;
    o_TexCoord = uvec2(face, 0);
    o_Normal = (model * normals[face]).xyz;
    o_Colour = colour.rgb;
}
