/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "widget.rs.hlsl"

struct VertexPos
{
	float3 Position : POSITION;
};

struct VertexShaderOutput
{
	float4 Position : SV_Position;
};

struct ModelViewProjection
{
	matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

[RootSignature(RS)]
VertexShaderOutput main(VertexPos IN)
{
	VertexShaderOutput OUT;
	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.f));
	return OUT;
}
