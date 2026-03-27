/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "copy_render_target.rs.hlsl"

#pragma pack_matrix(row_major)

struct PSInput
{
	float4 position : SV_Position; //screen space
    float2 uv : TEXCOORD0;
};

[RootSignature(RS)]
PSInput main(uint id : SV_VertexID)
{
    PSInput output;
    
    //render a single triangle across the entire screen
    //the final position is in clip coord (center is the middle of the screen, y positive is up)
    // id0---------- id1
    //    |     | /
    //    |     |/
    //    |-----/
    //    |    /
    //    |   /
    //    |  /
    //    | /
    //    |/
    //    id2
    
    // id 0 : pos = (-1,  1) , uv = (0, 0)
    // id 1 : pos = (3,   1) , uv = (2, 0)
    // id 2 : pos = (-1, -3) , uv = (0, 2)
    
    output.uv = float2((id << 1) & 2, id & 2);
    output.position = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return output;
}
