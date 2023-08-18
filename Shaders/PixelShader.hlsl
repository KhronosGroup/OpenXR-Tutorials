//Color Fragment Shader
struct PS_IN
{
    float4 i_Position : SV_Position;
    nointerpolation float2 i_TexCoord : TEXCOORD0;
    float3 i_Normal : TEXCOORD1;
};
struct PS_OUT
{
    float4 o_Color : SV_Target0;
};
cbuffer Data : register(b2)
{
    float4 colors[6];
};
PS_OUT main(PS_IN IN)
{
    PS_OUT OUT;
    int i = int(IN.i_TexCoord.x);
	float light=0.2+0.8*IN.i_Normal.g;
    OUT.o_Color = float4(light*colors[0].rgb,1.0);//
    return OUT;
}