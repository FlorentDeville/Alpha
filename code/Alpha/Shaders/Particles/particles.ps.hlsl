/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "particles.rs.hlsl"
#include "particles.hlsli"

Texture2D particleTexture : register(t1);
SamplerState particleSample : register(s1);

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    float4 color = particleTexture.SampleLevel(particleSample, input.uv, 0);
    if(color.a != 1)
        discard;
    
    return color;
}
