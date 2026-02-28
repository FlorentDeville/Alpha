/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "texture.rs.hlsl"

struct VS_Input
{
	float3 vertex : POSITION;
	float2 uv : UV;
};

struct VS_Output
{
	float4 vertex : SV_Position;
	float2 uv : UV;
};

struct CB_WVP_DESC
{
	matrix wvp;
};
ConstantBuffer<CB_WVP_DESC> cb_wvp : register(b0);

[RootSignature(RS)]
VS_Output main( VS_Input input )
{
	VS_Output output;
	output.vertex = mul(cb_wvp.wvp, float4(input.vertex, 1.f));
	output.uv = input.uv;
	return output;
}
