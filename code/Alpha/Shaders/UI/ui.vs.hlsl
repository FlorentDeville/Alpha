/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "ui.rs.hlsl"
#include "ui.hlsli"

#pragma pack_matrix(row_major)

struct FrameParameterStruct
{
    matrix m_view;
    matrix m_proj;
};
ConstantBuffer<FrameParameterStruct> FrameParameter : register(b0);

struct ObjectParameterStruct
{
    float2 m_pos;
    float2 m_size;
};
ConstantBuffer<ObjectParameterStruct> ObjectParameter : register(b1);

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
    
    //render a single quad
    float4 objectPos = float4(ObjectParameter.m_pos, 0, 1);
    float4 objectSize = float4(ObjectParameter.m_size, 0, 0);
    output.position = objectPos + pos[id] * objectSize;
    
    output.position = mul(output.position, FrameParameter.m_view);
    output.position = mul(output.position, FrameParameter.m_proj);
    
    output.uv = uv[id];

	return output;
}
