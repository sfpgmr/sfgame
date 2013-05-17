
struct VertexShaderInput
{
	float3 pos : POSITION;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};

cbuffer Buffer2D : register (b4)
{
	matrix transform;
	float4 color;
}

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
