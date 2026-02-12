/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Core/Color/Color.h"

namespace Core
{
	Color::Color()
		: Color(0, 0, 0)
	{ }

	Color::Color(float r, float g, float b)
		: m_r(r)
		, m_g(g)
		, m_b(b)
	{ }

	float Color::GetRed() const
	{
		return m_r;
	}

	float Color::GetGreen() const
	{
		return m_g;
	}

	float Color::GetBlue() const
	{
		return m_b;
	}

	void Color::SetRed(float r)
	{
		m_r = r;
	}

	void Color::SetGreen(float g)
	{
		m_g = g;
	}

	void Color::SetBlue(float b)
	{
		m_b = b;
	}
}
