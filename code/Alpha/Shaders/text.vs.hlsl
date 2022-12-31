/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "text.rs.hlsl"

struct VS_Input
{
	float4 pos : POSITION;  // (x, y) : top left position of the quad, (z, w) : width, height of the quad
	float4 texCoord : UV;   // (x, y) uv of the top left of the character in the texture, (z, w) width, height of the character in the texture
	float4 color : COLOR;   // color of the character
    float  z : DEPTH;       // depth of the quad in screen space (-1, 1) with the camera being at 0.
};

struct VS_Output
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord : UV;
};

[RootSignature(RS)]
VS_Output main(VS_Input input, uint vertexID : SV_VertexID)
{
	VS_Output output;

    // vert id 0 = 0000, uv = (0, 0)
    // vert id 1 = 0001, uv = (1, 0)
    // vert id 2 = 0010, uv = (0, 1)
    // vert id 3 = 0011, uv = (1, 1)
    float2 uv = float2(vertexID & 1, (vertexID >> 1) & 1);

    // set the position for the vertex based on which vertex it is (uv)
    //screen space position [-1, 1]
    output.pos = float4(input.pos.x + (input.pos.z * uv.x), input.pos.y - (input.pos.w * uv.y), input.z, 1);
    output.color = input.color;

    // set the texture coordinate based on which vertex it is (uv)
    output.texCoord = float2(input.texCoord.x + (input.texCoord.z * uv.x), input.texCoord.y + (input.texCoord.w * uv.y));

    return output;
}
