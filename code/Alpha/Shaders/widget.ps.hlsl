/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "widget.rs.hlsl"

struct PixelShaderInput
{
	float4 Position : SV_Position;
};

struct CBStruct
{
	float4 Color;
};

ConstantBuffer<CBStruct> ColorCB : register(b1);

[RootSignature(RS)]
float4 main(PixelShaderInput IN) : SV_Target
{
	return ColorCB.Color;
}
