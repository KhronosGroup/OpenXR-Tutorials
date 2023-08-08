//Color Fragment Shader
struct PS_IN
{
    float4 i_Position : SV_Position;
    nointerpolation float2 i_TexCoord : TEXCOORD0;
};
struct PS_OUT
{
    float4 o_Color : SV_Target0;
};
cbuffer Data : register(b0)
{
    float4 colors[6];
};
PS_OUT main(PS_IN IN)
{
    PS_OUT OUT;
    int i = int(IN.i_TexCoord.x);
    OUT.o_Color = colors[i];
    return OUT;
}