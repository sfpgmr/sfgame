#include "VideoShader.hlsli"

PixelShaderInput main( VertexShaderInput pos ) 
{
  PixelShaderInput output;
  output.pos = float4(pos.pos.x,pos.pos.y,0.0,1.0);
  output.textureUV = pos.textureUV;

	return output;
}