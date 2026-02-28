/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "texture_editor_viewport.rs.hlsl"

struct VS_Output
{
	float4 vertex : SV_Position;
	float2 uv : UV;
};

cbuffer PixelShaderParameter : register(b0)
{
    int type;
};

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

TextureCube cubemap : register(t1);

[RootSignature(RS)]
float4 main(VS_Output input) : SV_TARGET
{
	if(type == 0)
    {
        float4 color = t1.Sample(s1, input.uv);
        return color;
    }
    else
    {
        float3 uvw = normalize(float3(input.uv, 0));
        float4 color = cubemap.Sample(s1, uvw);
        return color;
    }
}
