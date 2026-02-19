/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "shadowmap.rs.hlsl"

struct PixelShaderInput
{
    float4 vertex : SV_Position; //screen space
};

[RootSignature(RS)]
float main(PixelShaderInput IN) : SV_Target
{
    float depth = IN.vertex.z;
    return depth;
}
