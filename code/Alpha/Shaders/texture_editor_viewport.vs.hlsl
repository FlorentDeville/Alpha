/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "texture_editor_viewport.rs.hlsl"

struct VS_Input
{
	float3 vertex : POSITION;
	float2 uv : UV;
};

struct VS_Output
{
	float4 vertex : SV_Position;
	float4 worldPosition : TEXCOORD0; //world space
	float2 uv : UV;
};

cbuffer VertexShaderParameter : register(b0)
{
    matrix world;
	matrix wvp;
};

[RootSignature(RS)]
VS_Output main( VS_Input input )
{
	VS_Output output;
	output.vertex = mul(wvp, float4(input.vertex, 1.f));
    output.worldPosition = mul(world, float4(input.vertex, 1.f));
	output.uv = input.uv;
	return output;
}
