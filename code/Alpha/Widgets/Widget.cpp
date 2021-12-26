/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widget.h"

Widget::Widget()
	: m_children()
	, m_width(0)
	, m_height(0)
	, m_x(0)
	, m_y(0)
{}

Widget::Widget(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: m_children()
	, m_width(w)
	, m_height(h)
	, m_x(x)
	, m_y(y)
{}

Widget::~Widget()
{
	for (const Widget* pWidget : m_children)
		delete pWidget;
}

void Widget::Update()
{}

void Widget::Draw()
{}

void Widget::SetX(int32_t x)
{
	m_x = x;
}

void Widget::SetY(int32_t y)
{
	m_y = y;
}

int32_t Widget::GetX() const
{
	return m_x;
}

int32_t Widget::GetY() const
{
	return m_y;
}
