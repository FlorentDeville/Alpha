/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "text.rs.hlsl"

Texture2D fontTexture : register(t0);
SamplerState fontSampler : register(s0);

struct VS_Output
{
	float4 pos : VS_POSITION;
	float4 color : COLOR;
	float2 texCoord : UV;
};

[RootSignature(RS)]
float4 main(VS_Output input) : SV_TARGET
{
	return input.color * fontTexture.Sample(fontSampler, input.texCoord);
}
