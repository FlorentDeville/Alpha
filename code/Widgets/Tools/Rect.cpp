/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Tools/Rect.h"

namespace Widgets
{
	Rect::Rect()
		: Rect(0, 0, 0, 0)
	{ }

	Rect::Rect(int32_t left, int32_t right, int32_t top, int32_t bottom)
		: m_left(left)
		, m_right(right)
		, m_top(top)
		, m_bottom(bottom)
	{ }

	void Rect::Intersect(const Rect& other)
	{
		m_left = m_left > other.m_left ? m_left : other.m_left;
		m_right = m_right < other.m_right ? m_right : other.m_right;
		m_top = m_top > other.m_top ? m_top : other.m_top;
		m_bottom = m_bottom < other.m_bottom ? m_bottom : other.m_bottom;

		if (m_left > m_right)
		{
			m_left = 0;
			m_right = 0;
		}
		if (m_top > m_bottom)
		{
			m_top = 0;
			m_bottom = 0;
		}
	}

	bool Rect::IsInside(int32_t x, int32_t y) const
	{
		if (x <= m_left || x >= m_right || y <= m_top || y >= m_bottom)
			return false;

		return true;
	}
}
