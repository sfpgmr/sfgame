#include "2dshader.hlsli"

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;

  // Box2Dでのボディーからシェイプの頂点を求める
  // float32 x = (T.q.c * v.x - T.q.s * v.y) + T.p.x;
	// float32 y = (T.q.s * v.x + T.q.c * v.y) + T.p.y;
  float x = (body_rot.x * input.pos.x - body_rot.y * input.pos.y + body_pos.x) * screen_aspect.x * screen_scale.x + screen_offset.x;
  float y = (body_rot.y * input.pos.x + body_rot.x * input.pos.y + body_pos.y) * screen_aspect.y * screen_scale.y + screen_offset.y;

  float4 pos = float4(x,y,input.pos.z,1.0f);
	//pos = mul(pos,transform);

	output.pos = pos;//float4(input.pos,1.0f);

	// Pass through the color without modification.
	output.color = body_color;

	return output;
}
