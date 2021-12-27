/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "widget.rs.hlsl"

struct VertexPosUv
{
	float3 Position : POSITION;
	float2 Uv : UV;
};

struct VertexShaderOutput
{
	float4 Position : SV_Position;
	float2 Uv : TEXCOORD;
};

struct ModelViewProjection
{
	matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

[RootSignature(RS)]
VertexShaderOutput main(VertexPosUv IN)
{
	VertexShaderOutput OUT;
	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.f));
	OUT.Uv = IN.Uv;
	return OUT;
}
