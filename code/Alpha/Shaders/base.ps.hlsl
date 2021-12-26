/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "base.rs.hlsl"

struct PixelShaderInput
{
	float4 Color : COLOR;
};

[RootSignature(RS)]
float4 main(PixelShaderInput IN) : SV_Target
{
	return IN.Color;
}
