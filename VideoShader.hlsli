struct VertexShaderInput
{
	float2 pos : POSITION;
  float2 textureUV : TEXCOORD;
};

struct PixelShaderInput
{
  float4 pos : SV_POSITION;
  float2 textureUV : TEXCOORD;

};