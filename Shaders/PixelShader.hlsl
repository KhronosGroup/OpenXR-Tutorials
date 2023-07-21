

struct vertexOutput
{
	float4 hPosition:SV_POSITION;
	float3 normal:TEXCOORD2;
};


float4 main(vertexOutput IN):SV_TARGET
{
	float4 c=float4(IN.normal,1.0);
	return c;
}