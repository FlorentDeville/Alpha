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
    matrix m_wvp;
    float4 m_color;
};
ConstantBuffer<ObjectParameterStruct> ObjectParameter : register(b0);

[RootSignature(RS)]
PSInput main(VertexInput input)
{
    PSInput output;
    
    output.position = mul(float4(input.Position, 1), ObjectParameter.m_wvp);
    output.color = ObjectParameter.m_color;
    
	return output;
}
