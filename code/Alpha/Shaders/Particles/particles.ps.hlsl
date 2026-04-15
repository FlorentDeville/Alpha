/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "particles.rs.hlsl"
#include "particles.hlsli"

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    return float4(1, 1, 1, 1);
}
