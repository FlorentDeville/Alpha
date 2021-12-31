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
/*																	*/
/* Screen coordinate :  origin : center of the screen				*/ 
/*						range : [-1, 1]								*/
/*																	*/		
/* UI coordinate :	origin : top left of the screen					*/
/*					range : [0, window size]						*/
/*																	*/
/* All the widgets dimensions are in pixels and independent of the	*/
/* main window size and resolution									*/
/* The positions are in UI coordinate.								*/
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

	virtual void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize);

	virtual bool Handle(const Message& msg);

	void AddWidget(Widget* pWidget);

	void SetX(int32_t x);
	void SetY(int32_t y);

	void SetBackgroundColor(const DirectX::XMVECTOR& color);

	int32_t GetX() const; //Get the local x coordinate from the top left corner
	int32_t GetY() const; //Get the local y coordinate from the top left corner

	int32_t GetScreenX() const; //Get the x coordinate from the top left corner
	int32_t GetScreenY() const; //Get the y coordinate from the top left corner

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	bool IsInside(uint32_t screenX, uint32_t screenY) const;

protected:
	DirectX::XMUINT2 m_size; //width and height in pixels
	DirectX::XMINT3 m_locPos; //local position of the top left corner in pixel
	DirectX::XMINT3 m_absPos; //absolute position (from the top left corner)

	DirectX::XMVECTOR m_backgroundColor;
	DirectX::XMVECTOR m_borderColor;
	bool m_showBorder;
	int m_borderWidth;
	
	bool m_hover;	//Indicate if the mouse is hovering the widget;

	std::vector<Widget*> m_children;

	void ComputeWVPMatrix(DirectX::XMMATRIX& wvp) const;
};
