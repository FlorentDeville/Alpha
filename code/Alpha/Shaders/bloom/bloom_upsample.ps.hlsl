/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "bloom_upsample.rs.hlsl"
#include "bloom_upsample.hlsli"

Texture2D lowerMip : register(t0);
SamplerState s1 : register(s0);

struct ParametersStruct
{
    float filterRadius;
};

ConstantBuffer<ParametersStruct> Parameters : register(b0);

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    //cf https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
    
    //the radius of the filter needs to be dependent on the mip resolution, that's why it's not hardcoded in the shader.
    float x = Parameters.filterRadius;
    float y = Parameters.filterRadius;
    
    //Taking 9 sample around the current textel:
    // A-B-C
    // D-E-F
    // G-H-I
    // E is the current texel
    
    float4 A = lowerMip.SampleLevel(s1, input.uv + float2(-x, -y), 0);
    float4 B = lowerMip.SampleLevel(s1, input.uv + float2(0, -y), 0);
    float4 C = lowerMip.SampleLevel(s1, input.uv + float2(x, -y), 0);
    
    float4 D = lowerMip.SampleLevel(s1, input.uv + float2(-x, 0), 0);
    float4 E = lowerMip.SampleLevel(s1, input.uv + float2(0, 0), 0);
    float4 F = lowerMip.SampleLevel(s1, input.uv + float2(x, 0), 0);
    
    float4 G = lowerMip.SampleLevel(s1, input.uv + float2(-x, y), 0);
    float4 H = lowerMip.SampleLevel(s1, input.uv + float2(0, y), 0);
    float4 I = lowerMip.SampleLevel(s1, input.uv + float2(x, y), 0);
    
    // Apply weighted distribution
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    
    float4 upsample = E * 4;
    upsample += (B + D + F + H) * 2;
    upsample += (A + C + G + I);
    upsample *= (1.f / 16.f);
    return upsample;
}
