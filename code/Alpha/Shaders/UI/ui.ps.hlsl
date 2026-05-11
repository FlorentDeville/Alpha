/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "ui.rs.hlsl"
#include "ui.hlsli"

Texture2D uiTexture : register(t0);
SamplerState uiSample : register(s0);

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    float4 color = uiTexture.SampleLevel(uiSample, input.uv, 0);
    return color;
}
