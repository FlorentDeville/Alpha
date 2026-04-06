/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "bloom_emissive_filter.rs.hlsl"
#include "bloom_emissive_filter.hlsli"

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    float4 color = t1.SampleLevel(s1, input.uv, 0);
    return color;
}
