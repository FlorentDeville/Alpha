/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

#include <DirectXMath.h>

/********************************************************************/
/* Base class for Widget											*/		
/* Every widget local frame of reference is its top left corner.	*/
/* X goes right, Y goes down.										*/	
/* Every position is relative to the parent widget.					*/
/********************************************************************/
class Widget
{
public:
	Widget();
	Widget(uint32_t w, uint32_t h, int32_t x, int32_t y);
	virtual ~Widget();

	virtual void Update();	//Handle events and propagate to children
	virtual void Draw(int32_t parentX, int32_t parentY, float parentZ);	//Draw the widgets using the gfx api.

	virtual void Resize();

	void AddWidget(Widget* pWidget);

	void SetX(int32_t x);
	void SetY(int32_t y);

	int32_t GetX() const;
	int32_t GetY() const;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

protected:
	std::vector<Widget*> m_children;

	uint32_t m_width;	//width in pixels
	uint32_t m_height;	//height in pixels

	int32_t m_x; //position of the top left corner
	int32_t m_y; //position of the top left corner

	void ComputeWorldPosition(int32_t parentX, int32_t parentY, int32_t& x, int32_t& y) const;
	void ComputeWVPMatrix(DirectX::XMMATRIX& wvp, int32_t parentX, int32_t parentY, float parentZ) const;
};
