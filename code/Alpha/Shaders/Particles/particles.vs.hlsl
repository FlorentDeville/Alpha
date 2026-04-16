/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "particles.rs.hlsl"
#include "particles.hlsli"

#pragma pack_matrix(row_major)

struct ParticlePositionStruct
{
    float4 m_pos;
};
StructuredBuffer<ParticlePositionStruct> ParticlePosition : register(t0);

struct FrameParameterStruct
{
    matrix m_view;
    matrix m_proj;
};
ConstantBuffer<FrameParameterStruct> FrameParameter : register(b0);

static const float4 pos[4] =
{
    float4(-0.5, 0.5, 0, 1),
    float4(0.5, 0.5, 0, 1),
    float4(-0.5, -0.5, 0, 1),
    float4(0.5, -0.5, 0, 1)
};

static const float2 uv[4] =
{
    float2(0, 0),
    float2(1, 0),
    float2(0, 1),
    float2(1, 1)
};

[RootSignature(RS)]
PSInput main(uint id : SV_VertexID, uint instanceID : SV_InstanceID)
{
    PSInput output;
    
    float4 viewLocation = FrameParameter.m_view[3];

    //render a single quad using the vertex id and awlays facing the camera.
    output.position = pos[id] + ParticlePosition[instanceID].m_pos + viewLocation;
    output.position = mul(output.position, FrameParameter.m_proj);
    
    output.uv = uv[id];

	return output;
}
