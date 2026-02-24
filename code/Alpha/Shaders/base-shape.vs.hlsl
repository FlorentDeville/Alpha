/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "base-shape.rs.hlsl"

struct VertexInput
{
	float3 Position : POSITION;
	float3 Color : COLOR;
    float2 Uv : UV;
    float3 Normal : NORMAL;
};

struct ModelViewProjection
{
	matrix MVP;
	float4 Color;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexShaderOutput
{
	float4 Color :COLOR;
	float4 Position : SV_Position;
};

[RootSignature(RS)]
VertexShaderOutput main(VertexInput IN)
{
	VertexShaderOutput OUT;
	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.f));
	OUT.Color = ModelViewProjectionCB.Color;
	return OUT;
}
