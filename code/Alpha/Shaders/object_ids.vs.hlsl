/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "object_ids.rs.hlsl"

#pragma pack_matrix(row_major)

struct VS_Input
{
    float3 vertex : POSITION;
    float3 color : COLOR;
    float2 uv : UV;
    float3 normal : NORMAL;
};

struct VS_Ouput
{
    uint4 color : COLOR;
    float4 vertex : SV_Position;
};

cbuffer PerObject
{
    matrix worldMatrix;
    uint4 objectId;
};

cbuffer PerFrame
{
    matrix viewMatrix;
    matrix projMatrix;
};

[RootSignature(RS)]
VS_Ouput main(VS_Input input)
{
    VS_Ouput output;
    
    float4 pos = float4(input.vertex, 1.f);
    output.vertex = mul(pos, worldMatrix);
    output.vertex = mul(output.vertex, viewMatrix);
    output.vertex = mul(output.vertex, projMatrix);
    
    output.color = objectId;
    return output;
}