/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "wireframe.rs.hlsl"
#include "wireframe.hlsli"

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    return float4(1, 0, 0, 1);
}
