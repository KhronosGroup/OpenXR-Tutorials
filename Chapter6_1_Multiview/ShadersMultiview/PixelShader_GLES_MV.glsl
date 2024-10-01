// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#version 310 es
layout(location = 0) in flat uvec2 i_TexCoord;
layout(location = 1) in highp vec3 i_Normal;
layout(location = 2) in flat highp vec3 i_Color;
layout(location = 0) out highp vec4 o_Color;
layout(std140, binding = 2) uniform Data {
    highp vec4 colors[6];
} d_Data;

void main() {
    uint i = i_TexCoord.x;
    highp float light = 0.1 + 0.9 * clamp(i_Normal.g, 0.0, 1.0);
    o_Color = highp vec4(light * i_Color.rgb, 1.0);
}
