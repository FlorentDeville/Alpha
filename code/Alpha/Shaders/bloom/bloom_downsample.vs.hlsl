/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "bloom_downsample.rs.hlsl"
#include "bloom_downsample.hlsli"


[RootSignature(RS)]
PSInput main(uint id : SV_VertexID)
{
    PSInput output;
    output.uv = float2((id << 1) & 2, id & 2);
    output.position = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

    return output;
}
