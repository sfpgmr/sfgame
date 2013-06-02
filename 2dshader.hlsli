
struct VertexShaderInput
{
	float3 pos : POSITION;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};

cbuffer body : register (b1)
{
  float2 body_pos;
  float2 body_rot;
	float4 body_color;
};

cbuffer screen : register (b0)
{
  float2 screen_offset;
  float2 screen_scale;
  float2 screen_aspect;
};

