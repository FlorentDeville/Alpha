/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "object_ids.rs.hlsl"

struct PixelShaderInput
{
    uint4 Color : COLOR;
};

[RootSignature(RS)]
uint4 main(PixelShaderInput IN) : SV_Target
{
    return IN.Color;
}
