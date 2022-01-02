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
	
	output.uv = input.uv;

	if (cb_viewport.rect.x < cb_texture.rect.x) //stretch on the x axis
	{
		float halfWidthRatio = cb_viewport.rect.x / cb_texture.rect.x / 2;
		if (input.uv.x == 0)
		{
			output.uv.x = 0.5 - halfWidthRatio;
		}
		else
		{
			output.uv.x = 0.5 + halfWidthRatio;
		}

		output.uv.y = input.uv.y;
	}
	
	if (cb_viewport.rect.y < cb_texture.rect.y) //stretch on the y axis
	{
		float halfHeightRatio = cb_viewport.rect.y / cb_texture.rect.y / 2;
		if (input.uv.y == 0)
		{
			output.uv.y = 0.5 - halfHeightRatio;
		}
		else
		{
			output.uv.y = 0.5 + halfHeightRatio;
		}

		output.uv.x = input.uv.x;
	}

	output.vertex = mul(cb_wvp.wvp, float4(input.vertex, 1.f));

	return output;
}
