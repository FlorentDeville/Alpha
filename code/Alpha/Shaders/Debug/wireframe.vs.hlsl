/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "wireframe.rs.hlsl"
#include "wireframe.hlsli"

#pragma pack_matrix(row_major)

struct VertexInput
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 Uv : UV;
    float3 Normal : NORMAL;
};

struct ObjectParameterStruct
{
    matrix m_world;
    matrix m_view;
    matrix m_proj;
};
ConstantBuffer<ObjectParameterStruct> ObjectParameter : register(b0);

[RootSignature(RS)]
PSInput main(VertexInput input)
{
    PSInput output;
    
    output.position = mul(float4(input.Position, 1), ObjectParameter.m_world);
    output.position = mul(output.position, ObjectParameter.m_view);
    output.position = mul(output.position, ObjectParameter.m_proj);

	return output;
}
