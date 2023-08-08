//Color Vertex Shader

cbuffer CameraConstants : register(b1)
{
    float4x4 viewProj;
    float4x4 modelViewProj;
    float4x4 model;
};
struct VS_IN
{
    uint vertexId : SV_VertexId;
    float4 a_Positions : TEXCOORD0;
};
struct VS_OUT
{
    float4 o_Position : SV_Position;
    nointerpolation float2 o_TexCoord : TEXCOORD0;
};
VS_OUT main(VS_IN IN)
{
    VS_OUT OUT;
    OUT.o_Position = mul(modelViewProj,IN.a_Positions);
    int face = IN.vertexId / 6;
    OUT.o_TexCoord = float2(float(face), 0);
    return OUT;
}