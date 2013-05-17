#include "2dshader.hlsli"

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 pos = float4(input.pos, 1.0f);
	//pos = mul(pos,transform);

	// Transform the vertex position into projected space.
	//pos = mul(pos, model);
	//pos = mul(pos, view);
	//pos = mul(pos, projection);
	output.pos = pos;

	// Pass through the color without modification.
	output.color = color;

	return output;
}
