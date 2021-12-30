/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "texture.rs.hlsl"

struct VS_Output
{
	float4 vertex : SV_Position;
	float2 uv : UV;
};

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

[RootSignature(RS)]
float4 main(VS_Output input) : SV_TARGET
{
	float4 color = t1.Sample(s1, input.uv);
	return color;
	//return float4(1.f, 1.f, 1.f, 1.f) * float4(input.uv.x, input.uv.y, 1.f, 1.f);
}
