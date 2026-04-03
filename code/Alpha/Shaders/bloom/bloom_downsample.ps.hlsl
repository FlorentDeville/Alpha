/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "bloom_downsample.rs.hlsl"
#include "bloom_downsample.hlsli"

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

struct ParametersStruct
{
    float2 sourceResolution;
};

ConstantBuffer<ParametersStruct> Parameters : register(b0);

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    float2 sourceTexelSize = 1.f / Parameters.sourceResolution;
    float x = sourceTexelSize.x;
    float y = sourceTexelSize.y;
    
    //now take all the samples around the current pixel
    //cf https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
    //the pattern is like this
    // A-B-C
    // -J-K-
    // D-E-F
    // -L-M-
    // G-H-I
    //
    //E is the current texel
    
    float3 A = t1.SampleLevel(s1, input.uv + float2(-2 * x, -2 * y), 0).rgb;
    float3 B = t1.SampleLevel(s1, input.uv + float2(0, -2 * y), 0).rgb;
    float3 C = t1.SampleLevel(s1, input.uv + float2(2 * x, -2 * y), 0).rgb;
         
    float3 D = t1.SampleLevel(s1, input.uv + float2(-2 * x, 0), 0).rgb;
    float3 E = t1.SampleLevel(s1, input.uv, 0).rgb;
    float3 F = t1.SampleLevel(s1, input.uv + float2(2 * x, 0), 0).rgb;
         
    float3 G = t1.SampleLevel(s1, input.uv + float2(-2 * x, 2 * y), 0).rgb;
    float3 H = t1.SampleLevel(s1, input.uv + float2(0, 2 * y), 0).rgb;
    float3 I = t1.SampleLevel(s1, input.uv + float2(2 * x, 2 * y), 0).rgb;
         
    float3 J = t1.SampleLevel(s1, input.uv + float2(-x, -y), 0).rgb;
    float3 K = t1.SampleLevel(s1, input.uv + float2(x, -y), 0).rgb;
    float3 L = t1.SampleLevel(s1, input.uv + float2(-x, y), 0).rgb;
    float3 M = t1.SampleLevel(s1, input.uv + float2(x, y), 0).rgb;
    
    float3 downsample = E * 0.125;
    downsample += (A + C + G + I) * 0.03125;
    downsample += (B + D + F + H) * 0.0625;
    downsample += (J + K + L + M) * 0.125;

    return float4(downsample, 1);
}
