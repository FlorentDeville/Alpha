/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "widget_viewport.rs.hlsl"

struct VS_Input
{
	float3 vertex : POSITION;
	float2 uv : UV;
};

struct VS_Output
{
	float4 vertex : SV_Position;
	float2 uv : UV;
};

struct CB_WVP_DESC
{
	matrix wvp;
};
ConstantBuffer<CB_WVP_DESC> cb_wvp : register(b0);

struct CB_VIEWPORT_DESC
{
	int2 rect;
};
ConstantBuffer<CB_VIEWPORT_DESC> cb_viewport : register(b1);

struct CB_TEXT_RECT_DESC
{
	float2 rect;
};
ConstantBuffer<CB_TEXT_RECT_DESC> cb_texture : register(b2);

[RootSignature(RS)]
VS_Output main(VS_Input input)
{
	VS_Output output;

	float halfWidthRatio = cb_viewport.rect.x / cb_texture.rect.x / 2;
	if (input.uv.x == 0)
	{
		output.uv.x = 0.5 - halfWidthRatio;
	}
	else
	{
		output.uv.x = 0.5 + halfWidthRatio;
	}
	
	output.vertex = mul(cb_wvp.wvp, float4(input.vertex, 1.f));
	output.uv.y = input.uv.y;

	return output;
}
