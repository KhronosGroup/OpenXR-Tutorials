
cbuffer CameraConstants : register(b1)
{
	float4x4 worldViewProj;
}

struct vertexInput
{
	float3 position:POSITION;
	float3 normal:NORMAL;
};

struct vertexOutput
{
	float4 hPosition:SV_POSITION;
	float3 normal:TEXCOORD2;
};

vertexOutput main(vertexInput IN)
{
	vertexOutput OUT;
	float4 opos=float4(IN.position.xyz,1.0);
	OUT.hPosition=mul(opos,worldViewProj);

	OUT.normal=(IN.normal);
	return OUT;
}