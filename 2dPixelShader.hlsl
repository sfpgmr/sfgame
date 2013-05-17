#include "2dshader.hlsli"

float4 main(PixelShaderInput input) : SV_Target
{
	return float4(input.color);
}
