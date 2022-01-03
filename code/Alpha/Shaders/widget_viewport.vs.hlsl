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
	
	float textureRatio = cb_texture.rect.x / cb_texture.rect.y;

	float h = cb_viewport.rect.y;
	float w = h * textureRatio;
	if (w < cb_viewport.rect.x)
	{
		w = cb_viewport.rect.x;
		h = w / textureRatio;
	}

	float dw = w - cb_viewport.rect.x;
	float pixelOffsetX = dw / 2;
	float uvOffsetX = pixelOffsetX / w;

	float dh = h - cb_viewport.rect.y;
	float pixelOffsetY = dh / 2;
	float uvOffsetY = pixelOffsetY / h;

	if (input.uv.x == 0)
		output.uv.x = 0 + uvOffsetX;
	else
		output.uv.x = 1 - uvOffsetX;

	if (input.uv.y == 0)
		output.uv.y = 0 + uvOffsetY;
	else
		output.uv.y = 1 - uvOffsetY;

	output.vertex = mul(cb_wvp.wvp, float4(input.vertex, 1.f));

	return output;
}
