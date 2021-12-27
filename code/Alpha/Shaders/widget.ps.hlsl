/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "widget.rs.hlsl"

struct PixelShaderInput
{
	float4 Position : SV_Position;
	float2 Uv : TEXCOORD;
};

struct CB_BackgroundColor
{
	float4 Color;
};
ConstantBuffer<CB_BackgroundColor> backgroundColor : register(b1);

struct CB_ShowBorder
{
	bool ShowBorder;
};
ConstantBuffer<CB_ShowBorder> showBorder : register(b2);

struct CB_BorderColor
{
	float4 Color;
};
ConstantBuffer<CB_BorderColor> borderColor : register(b3);

struct CB_Rect
{
	float2 Rect;
};
ConstantBuffer<CB_Rect> rect : register(b4);

struct CB_BorderWidth
{
	int BorderWidth;
};
ConstantBuffer<CB_BorderWidth> borderWidth : register(b5);

[RootSignature(RS)]
float4 main(PixelShaderInput IN) : SV_Target
{
	if (showBorder.ShowBorder)
	{
		float2 fPixelCoord = rect.Rect * IN.Uv;
		int2 pixelCoord = floor(fPixelCoord + 0.49f);

		int min = borderWidth.BorderWidth;
		int2 max = rect.Rect - borderWidth.BorderWidth;

		if (pixelCoord.x < min || pixelCoord.x >= max.x || pixelCoord.y < min || pixelCoord.y >= max.y)
			return borderColor.Color;
	}

	return backgroundColor.Color;
}
