/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

class Widget
{
public:
	Widget();
	Widget(uint32_t w, uint32_t h, int32_t x, int32_t y);
	virtual ~Widget();

	virtual void Update();	//Handle events and propagate to children
	virtual void Draw();	//Draw the widgets using the gfx api.

	void SetX(int32_t x);
	void SetY(int32_t y);

	int32_t GetX() const;
	int32_t GetY() const;

protected:
	std::vector<Widget*> m_children;

	uint32_t m_width;	//width in pixels
	uint32_t m_height;	//height in pixels

	int32_t m_x; //position of the top left corner
	int32_t m_y; //position of the top left corner
};
