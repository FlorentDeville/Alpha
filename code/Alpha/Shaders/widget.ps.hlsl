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

[RootSignature(RS)]
float4 main(PixelShaderInput IN) : SV_Target
{
	if (showBorder.ShowBorder)
	{
		if (IN.Uv.x < 0.1f || IN.Uv.x > 0.9 || IN.Uv.y < 0.1f || IN.Uv.y > 0.9)
			return borderColor.Color;
	}

	return backgroundColor.Color;
}
