/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "shadowmap.rs.hlsl"

struct PixelShaderInput
{
	float4 vertex : SV_Position;
};

[RootSignature(RS)]
float4 main(PixelShaderInput IN) : SV_Target
{
	float depth = IN.vertex.z / IN.vertex.w;
	return float4(depth, depth, depth, 1);
}
