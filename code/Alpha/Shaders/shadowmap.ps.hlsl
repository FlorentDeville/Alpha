/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "shadowmap.rs.hlsl"

struct PixelShaderInput
{
    float4 vertex : SV_Position; //screen space
    float4 worldPos : TEXCOORD0; //world space
};

cbuffer PerFrame : register(b1)
{
    matrix lightSpace;
    float4 lightWorldPos;
};

[RootSignature(RS)]
float4 main(PixelShaderInput IN) : SV_Target
{
    float depth = distance(IN.worldPos.xyz, lightWorldPos.xyz);
    depth = depth + fwidth(depth);
	return float4(depth, depth, depth, 1);
}
