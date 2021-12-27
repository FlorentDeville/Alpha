/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

#include <DirectXMath.h>

struct Message;

/********************************************************************/
/* Base class for Widget											*/		
/* Every widget local frame of reference is its top left corner.	*/
/* X goes right, Y goes down.										*/	
/* Every position is relative to the parent widget.					*/
/********************************************************************/
class Widget
{
	friend class WidgetMgr;

public:
	Widget();
	Widget(uint32_t w, uint32_t h, int32_t x, int32_t y);
	virtual ~Widget();

	virtual void Update();	//Handle events and propagate to children
	virtual void Draw(int32_t parentX, int32_t parentY, float parentZ);	//Draw the widgets using the gfx api.

	virtual void Resize();

	virtual bool Handle(const Message& msg);

	void AddWidget(Widget* pWidget);

	void SetX(int32_t x);
	void SetY(int32_t y);

	int32_t GetX() const; //Get the local x coordinate from the top left corner
	int32_t GetY() const; //Get the local y coordinate from the top left corner

	int32_t GetScreenX() const; //Get the screen x coordinate from the top left corner
	int32_t GetScreenY() const; //Get the screen y coordinate from the top left corner

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	bool IsInside(uint32_t screenX, uint32_t screenY) const;

protected:
	std::vector<Widget*> m_children;

	uint32_t m_width;	//width in pixels
	uint32_t m_height;	//height in pixels

	int32_t m_x; //local position of the top left corner
	int32_t m_y; //local position of the top left corner

	int32_t m_wx; //absolute position (viewport). Origin is the center of the screen.
	int32_t m_wy; //absolute position (viewport). Origin is the center of the screen.

	int32_t m_screenX; //absolute position in screen space (from the top left corner)
	int32_t m_screenY; //absolute position in screen space (from the top left corner)

	DirectX::XMVECTOR m_backgroundColor;
	DirectX::XMVECTOR m_borderColor;
	bool m_showBorder;
	int m_borderWidth;
	
	bool m_hover;	//Indicate if the mouse is hovering the widget;

	void ComputeWorldPosition(int32_t parentX, int32_t parentY, int32_t& x, int32_t& y) const;
	void ComputeWVPMatrix(DirectX::XMMATRIX& wvp, int32_t parentX, int32_t parentY, float parentZ) const;
	void ComputeScreenPosition(int32_t wx, int32_t wy, int32_t& screenX, int32_t& screenY);
};
