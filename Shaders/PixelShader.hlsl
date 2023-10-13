// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: MIT

struct PS_IN
{
    float4 i_Position : SV_Position;
    nointerpolation float2 i_TexCoord : TEXCOORD0;
    float3 i_Normal : TEXCOORD1;
    nointerpolation float3 i_Colour : TEXCOORD2;
};
struct PS_OUT
{
    float4 o_Colour : SV_Target0;
};

PS_OUT main(PS_IN IN)
{
    PS_OUT OUT;
    int i = int(IN.i_TexCoord.x);
    float light = 0.2 + 0.8 * IN.i_Normal.g;
    OUT.o_Colour = float4(light * IN.i_Colour.rgb, 1.0);
    return OUT;
}