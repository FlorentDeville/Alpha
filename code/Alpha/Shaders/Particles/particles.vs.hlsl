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

struct ParticleIndicesStruct
{
    min16int m_index;
};
StructuredBuffer<ParticleIndicesStruct> ParticleIndices : register(t2);

struct FrameParameterStruct
{
    matrix m_view;
    matrix m_proj;
};
ConstantBuffer<FrameParameterStruct> FrameParameter : register(b0);

static const float4 pos[4] =
{
    float4(-0.5, 0.5, 0, 0),
    float4(0.5, 0.5, 0, 0),
    float4(-0.5, -0.5, 0, 0),
    float4(0.5, -0.5, 0, 0)
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
    
    //render a single quad using the vertex id and awlays facing the camera.
    
    //first transform the quad center to the view space
    int positionIndex = ParticleIndices[instanceID].m_index;
    output.position = mul(ParticlePosition[positionIndex].m_pos, FrameParameter.m_view);
    
    //in the view space, add the offset of the vertex
    output.position = output.position + pos[id];
    
    //now project it
    output.position = mul(output.position, FrameParameter.m_proj);
    
    output.uv = uv[id];

	return output;
}
