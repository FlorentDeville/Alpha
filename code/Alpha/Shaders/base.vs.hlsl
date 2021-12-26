/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

//#define RS	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
//					"DENY_HULL_SHADER_ROOT_ACCESS |" \
//					"DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
//					"DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
//					"DENY_PIXEL_SHADER_ROOT_ACCESS)," \
//			"RootConstants(num32BitConstants=16, b0, space=0, visibility=SHADER_VISIBILITY_VERTEX)"

#include "base.rs.hlsl"

struct VertexPosColor
{
	float3 Position : POSITION;
	float3 Color : COLOR;
};

struct ModelViewProjection
{
	matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexShaderOutput
{
	float4 Color :COLOR;
	float4 Position : SV_Position;
};

[RootSignature(RS)]
VertexShaderOutput main(VertexPosColor IN)
{
	VertexShaderOutput OUT;
	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.f));
	OUT.Color = float4(IN.Color, 1.f);
	return OUT;
}
