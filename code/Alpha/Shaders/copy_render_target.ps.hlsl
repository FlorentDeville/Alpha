/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "copy_Render_target.rs.hlsl"

struct PSInput
{
    float4 position : SV_Position; //screen space
    float2 uv : TEXCOORD0;
};

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

[RootSignature(RS)]
float4 main(PSInput input) : SV_Target
{
    return t1.Sample(s1, input.uv);
}
