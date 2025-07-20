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

struct CB_AlternateColor
{
    float4 Color1;
    float4 Color2;
    uint Size;
};
ConstantBuffer<CB_AlternateColor> alternateColor: register(b6);

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
	
	if(alternateColor.Size > 0)
    {
        float2 fPixelCoord = rect.Rect * IN.Uv;
        int2 pixelCoord = floor(fPixelCoord + 0.49f);
		
        uint row = (uint)pixelCoord.y / alternateColor.Size;
        if ((row % 2) == 0)
            return alternateColor.Color1;
		
        return alternateColor.Color2;

    }

	return backgroundColor.Color;
}
