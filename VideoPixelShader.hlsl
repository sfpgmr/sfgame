#include "VideoShader.hlsli"

Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
	return diffuseTexture.Sample(linearSampler, input.textureUV);
}